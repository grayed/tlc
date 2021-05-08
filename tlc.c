/*
 * Copyright (c) 2021 Vadim Zhukov <zhuk@openbsd.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <sys/types.h>
#include <sys/time.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <poll.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "compat.h"

struct input_state {
	int	 fd;
	char	*buf;
	size_t	 allocated;
	size_t	 available;	// number of actual data bytes in buf
	size_t	 handled;	// number of bytes already handled
};

char	 	def_delimiter = '\n';

// user-defined parameters
bool		 has_end_value, use_format, passthrough;
struct timespec	 interval = { 1, 0 };
char		*fmts, *delimiter = (char *)&def_delimiter;
size_t		 delimiter_sz = 1;
long long	 start_value, end_value;

struct timespec	 now;
const char	 nl = '\n';

// Line to be displayed after timeout.
// Managed by queue_line() and proceed_queued().
char		*queued_str;
size_t		 queued_len;	// cached length of queued_str
struct timespec	 ts_last_upd;	// time of last (queued) line output
struct timespec	 ts_zero;

// Last output string went to stderr. Set & freed by display_line().
char		*last_out_str;	
size_t		 last_out_len;	// cached length of last_out_str

// NOT strings; used to avoid calling write() many times
char		*blanks;	// filled by spaces at least as long as queued
char		*backspaces;	// filled by \b at least as long as queued


__dead void
usage(const char *errmsg) {
	if (errmsg)
		dprintf(STDERR_FILENO, "%s\n", errmsg);
	dprintf(STDERR_FILENO,
	    "usage: %s [-fp] [-d delim] [-e end] [-i interval] [-s start] [text]\n",
	    getprogname());
	exit(1);
}

void
resize_b(char **b, size_t newsize, int ch) {
	char	*newb;

	if ((newb = realloc(*b, newsize)) == NULL)
		err(1, __func__);
	memset(newb, ch, newsize);
	*b = newb;
}

void
adjust_bnb(size_t minsize) {
	static size_t	 bsize;

	if (minsize <= bsize)
		return;
	resize_b(&blanks, minsize, ' ');
	resize_b(&backspaces, minsize, '\b');
	bsize = minsize;
}

void
display_line(char *line, size_t len) {
	free(last_out_str);
	adjust_bnb(last_out_len);
	write(STDERR_FILENO, backspaces, last_out_len);
	write(STDERR_FILENO, blanks, last_out_len);
	write(STDERR_FILENO, backspaces, last_out_len);
	if (len)
		write(STDERR_FILENO, line, len);
	last_out_len = len;
	last_out_str = line;
}

char *
format_line(size_t *linelen, long long lineno, const char *cur_str, size_t cur_str_len) {
	int	 lineno_len, progress_len;
	char	*out_str, lineno_str[64], progress_str[64];

	lineno_len = snprintf(lineno_str, sizeof(lineno_str), "%lld", lineno);

	if (has_end_value) {
		long long	 progress;

		progress = (lineno - start_value) * 100;
		progress /= end_value - start_value;
		progress_len = snprintf(progress_str, sizeof(progress_str),
		    "%lld", progress);
	} else {
		progress_str[0] = '\0';
		progress_len = 0;
	}

	if (use_format) {
		size_t	 i, reslen = 0;
		char	*p;
		bool	 pct = false;

		for (reslen = i = 0; fmts[i]; i++) {
			if (pct) {
				pct = false;
				switch (fmts[i]) {
				case 'i':	reslen += lineno_len; break;
				case 'p':	reslen += progress_len; break;
				case 's':	reslen += cur_str_len; break;
				case '%':	reslen++; break;
				default:	reslen += 2; break;
				}
			} else {
				if (fmts[i] == '%')
					pct = true;
				else
					reslen++;
			}
		}
		if (pct) {
			reslen++;
			pct = false;
		}

		if ((out_str = malloc(reslen + 1)) == NULL)
			return NULL;

		p = out_str;
		for (i = 0; fmts[i]; i++) {
			if (pct) {
				pct = false;
				switch (fmts[i]) {
				case 'i':
					memcpy(p, lineno_str, lineno_len);
					p += lineno_len;
					break;
				case 'p':
					memcpy(p, progress_str, progress_len);
					p += progress_len;
					break;
				case 's':
					memcpy(p, cur_str, cur_str_len);
					p += cur_str_len;
					break;
				case '%':
					*p++ = fmts[i];
					break;
				default:
					*p++ = '%';
					*p++ = fmts[i];
				}
			} else {
				if (fmts[i] == '%')
					pct = true;
				else
					*p++ = fmts[i];
			}
		}
		if (pct)
			*p++ = '%';
		*p = '\0';
		*linelen = reslen;
	} else {
		int	res;

		if (has_end_value)
			res = asprintf(&out_str, "%s%%%s...", progress_str, fmts);
		else
			res = asprintf(&out_str, "%s%s...", lineno_str, fmts);
		if (res == -1)
			return NULL;
		*linelen = (size_t)res;
	}

	return out_str;
}

void
proceed_queued() {
	struct timespec	diff;

	if (queued_str == NULL)
		return;
	timespecsub(&now, &ts_last_upd, &diff);
	if (timespeccmp(&diff, &interval, >=)) {
		display_line(queued_str, queued_len);
		// queued_str moved to last_out_str at this point
		queued_str = NULL;;
		queued_len = 0;
		ts_last_upd = now;
	}
}

void
queue_line(char *line, size_t linelen) {
	free(queued_str);
	if (!line || !last_out_str || strcmp(line, last_out_str) != 0) {
		queued_str = line;
		queued_len = linelen;
	} else {
		queued_str = NULL;
		queued_len = 0;
	}
}

/*
 * Return values:
 *  -1 - error happened, see errno
 *   0 - EOF
 *   1 - try again later
 */
int
proceed_input(struct input_state *ins, long long *lineno) {
	size_t		 linelen, out_str_len;
	char		*line, *out_str, *p;
	bool		 has_delimiter;

/*
 * Buffer layout and variable values:
 *
 *        |>line begins here   |>read(2) here  |>p points here... |>... or here
 * [====\n=====================================\n=================]
 *  ^buf  ^buf+handled         ^buf+available        buf+allocated^
 */

	for (;;) {
		p = memmem(ins->buf + ins->handled, ins->available - ins->handled,
		    delimiter, delimiter_sz);
		if (p) {
			p += delimiter_sz;
			has_delimiter = true;
		}
		while (p == NULL) {
			// try to read more data from fd
			ssize_t	 nread;

			if (ins->available == ins->allocated) {
				size_t	 newsz;
				char	*t;

				if (ins->allocated == SSIZE_MAX) {
					errno = EOVERFLOW;
					return -1;
				}

				if (ins->allocated > SSIZE_MAX / 2)
					newsz = SSIZE_MAX / 2;
				else if (ins->allocated == 0)
					newsz = PATH_MAX;	// Good Enough(TM)
				else
					newsz = ins->allocated * 2;

				t = realloc(ins->buf, newsz);
				if (t == NULL)
					return -1;
				ins->buf = t;
				ins->allocated = newsz;
			}

			memmove(ins->buf, ins->buf + ins->handled,
			    ins->available - ins->handled);
			ins->available -= ins->handled;
			ins->handled = 0;

			nread = read(ins->fd, ins->buf + ins->available,
			    ins->allocated - ins->available);
			if (nread == -1)
				return (errno == EAGAIN) ? 1 : -1;
			if (nread == 0) {
				if (ins->available == 0)
					return 0;
				// last line of input lacks '\n', be precious
				p = ins->buf + ins->available;
				has_delimiter = false;
			} else {
				ins->available += (size_t)nread;
				p = memmem(ins->buf, ins->available,
				    delimiter, delimiter_sz);
				if (p) {
					p += delimiter_sz;
					has_delimiter = true;
				}
			}
		}

		(*lineno)++;
		line = ins->buf + ins->handled;
		linelen = (size_t)(p - line);
		ins->handled += linelen;

		// 'ins' should not be accessed directly in this loop cycle anymore

		if (passthrough)
			fwrite(line, 1, linelen, stdout);	// check for error?

		if (has_delimiter)
			line[linelen -= delimiter_sz] = '\0';

		if ((out_str = format_line(&out_str_len, *lineno, line, linelen)) == NULL)
			return -1;
		if (timespecisset(&interval))
			queue_line(out_str, out_str_len);
		else {
			if (!last_out_str || strcmp(out_str, last_out_str) != 0)
				display_line(out_str, out_str_len);
			else
				free(out_str);
		}
	}
}

void
proceed_file(int fd, long long *lineno) {
	struct pollfd		pfd[1];
	struct timespec		timeout, *pt, next;
	struct input_state	ins;
	int			flags, nready, poll_timeout;

	if ((flags = fcntl(fd, F_GETFL)) == -1)
		err(1, "fcntl(F_GETFL)");
	flags |= O_NONBLOCK;
	if (fcntl(fd, F_SETFL, flags) == -1)
		err(1, "fcntl(F_SETFL)");

	pfd[0].fd = fd;
	pfd[0].events = POLLIN;

	memset(&ins, 0, sizeof(struct input_state));
	ins.fd = fd;

	if (timespecisset(&interval)) {
		timeout = interval;
		pt = &timeout;
		timespecadd(&now, &interval, &next);
	} else
		pt = NULL;

	for (;;) {
		pfd[0].revents = 0;
		if (pt) {
			clock_gettime(CLOCK_MONOTONIC, &now);
			timespecsub(&next, &now, pt);
			if (timespeccmp(pt, &ts_zero, <))
				timespecclear(pt);
		}

		// ppoll(2) is not supported on macOS
		poll_timeout = pt ?
		    (int)(pt->tv_sec * 1000 + pt->tv_nsec / 1000000) : -1;
		nready = poll(pfd, 1, poll_timeout);
		if (nready == -1) {
			if (errno == EINTR)
				continue;
			warn("ppoll");
			break;
		}

		clock_gettime(CLOCK_MONOTONIC, &now);
		if (pt)
			timespecadd(&now, &interval, &next);

		if (nready) {
			switch (proceed_input(&ins, lineno)) {
			case -1:
				warn("proceed_input");
				goto finish;

			case 0:
				goto finish;
			}
		}
		if (timespecisset(&interval))
			proceed_queued();
	}

finish:
	free(ins.buf);
}

int
main(int argc, char *argv[]) {
	long long	 lineno = 0;
	size_t		 out_str_len;
	unsigned long	 ul;
	int		 ch;
	char		*ep, *out_str;

	// do not define 'c', 'l', 'm' and 'w', unless they mean same as in wc(1)
	while ((ch = getopt(argc, argv, "d:e:fi:ps:")) != -1) {
		switch(ch) {
		case 'd':
			if (delimiter && delimiter != &def_delimiter)
				free(delimiter);
			if (optarg[0] == '\0') {
				def_delimiter = '\0';
				delimiter = &def_delimiter;
				delimiter_sz = 1;
			} else {
				if ((delimiter = strdup(optarg)) == NULL)
					err(1, "strdup");
				delimiter_sz = strlen(delimiter);
			}
			break;
		case 'e':
			has_end_value = true;
			end_value = strtoll(optarg, &ep, 10);
			if (*ep || ep == optarg)
				usage("invalid end value");
			break;
		case 'f':
			use_format = true;
			break;
		case 'i':
			ul = strtoul(optarg, &ep, 10);
			if (ul > INT_MAX / 1000 - 1)	// used in poll(2)
				usage("interval is too large");
			if (*ep || ep == optarg)
				usage("invalid interval");
			interval.tv_sec = (time_t)ul;
			break;
		case 'p':
			passthrough = true;
			break;
		case 's':
			start_value = strtoll(optarg, &ep, 10);
			if (*ep || ep == optarg)
				usage("invalid start value");
			break;
		default:
			usage(NULL);
		}
	}
	argc -= optind;
	argv += optind;

	if (argc > 1)
		usage(NULL);

	if (use_format) {
		if (argc == 0)
			usage("format text is required when -f is set");
		fmts = argc ? argv[0] : "";
	} else {
		if (argc) {
			if (asprintf(&fmts, " %s", argv[0]) == -1)
				err(1, "%s: asprintf", __func__);
		} else
			fmts = "";
	}

	if (has_end_value && end_value <= start_value)
		usage("invalid combination of the start and end values");
	lineno = start_value;

	// display initial result line, e.g. "0 lines seen"
	if ((out_str = format_line(&out_str_len, lineno, "", 0)) == NULL)
		err(1, "format_line");
	display_line(out_str, out_str_len);
	clock_gettime(CLOCK_MONOTONIC, &now);
	ts_last_upd = now;

	proceed_file(STDIN_FILENO, &lineno);

	if (queued_str)
		display_line(queued_str, queued_len);
	if (!use_format) {
		size_t adjnum = (fmts[0] == '\0') ? 3 : 2;

		adjust_bnb(adjnum);
		write(STDERR_FILENO, backspaces, adjnum);
		write(STDERR_FILENO, blanks, adjnum);
		write(STDERR_FILENO, backspaces, adjnum);
	}
	write(STDERR_FILENO, &nl, 1);
	free(last_out_str);
	free(blanks);
	free(backspaces);
	if (!use_format && argc)
		free(fmts);
	if (delimiter != &def_delimiter)
		free(delimiter);

	return 0;
}
