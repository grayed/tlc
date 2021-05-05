TLC(1) - General Commands Manual

# NAME

**tlc** - count input line interactively

# SYNOPSIS

**tlc**
\[**-fp**]
\[**-i**&nbsp;*interval*]
\[**-s**&nbsp;*start*]
\[*text*]

# DESCRIPTION

The
**tlc**
utility reads lines from standard input and, by default,
regularly updates summary line with increasing count of lines read
on standard error descriptor.

If
*text*
is specified, it is appended to summary line.

The options are as follows:

**-f**

> Makes
> *text*
> interpreted as format string to be used for displaing summary.
> The following format codes are supported:

> %%

> > Encodes single percent sign.

> %i

> > Replaced with number of lines read.

> %s

> > Replaced with last line read; empty if no lines were read yet.

> Every other combination of percent sign and some character will
> result in outputting them literally.

**-i** *interval*

> Sets minimal number of seconds between summary line updates to
> *interval*.
> This helps to avoid flickering due too often screen updates.
> If set to zero, screen will be updated upon each input line read.
> Default value is 1 second.

**-p**

> Enables passthrough mode: every input line gets duplicated on
> standard output.

**-s** *start*

> Sets initial value of lines read to
> *start*.
> Defaults to 0.

# EXIT STATUS

The **tlc** utility exits&#160;0 on success, and&#160;&gt;0 if an error occurs.

# EXAMPLES

Show file removal progress:

	$ rm -Rv foo | tlc 'files removed'
	42 files removed...

and eventually the line above will evolve into:

	1764 files removed.

Same with more details and logging to the file:

	$ rm -Rv foo | tlc -fp '%i files removed, last: %s' >rm.log

# SEE ALSO

getline(3)

# AUTHORS

**at**
was written by
Vadim Zhukov &lt;[zhuk@openbsd.org](mailto:zhuk@openbsd.org)&gt;.
