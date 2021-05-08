# TLC

**tlc** (Terminal Line Count) utility counts lines interactively on terminal.
It allows you to view some command's progress without filling whole
screen by extraneous details.

Written in C using modern POSIX and OpenBSD features.
Build testing happens on OpenBSD, Linux, macOS and FreeBSD for now.
Portability patches are welcome.

The code is distributed under BSD 2-clause license, except a few
compatibility goo that uses older BSD 3-clause license.

## Build and install instructions

### Build and install on OpenBSD

The shortest way:

	make && doas make install

You can also run regression tests after successful build:

	make test

### Build and install on non-OpenBSD

Non-BSD systems require [https://libbsd.freedesktop.org/wiki/](libbsd)
being installed. Note that if you system has separate -dev packages,
you'll need such one as well for building `tlc` not just libbsd itself.

Use CMake the usual way, for example:

	mkdir -p build
	cd build
	cmake ..
	make

To install you'll likely need elevate priviledges via sudo(8):

	sudo make install

## Usage examples

Show file removal progress:

	$ rm -Rv foo | tlc
	42...

and eventually the line above will evolve into:

	1765
	$

If you want to make things look prettier:

	$ rm -Rv foo | tlc 'files removed'
	42 files removed...
	<...>
	1765 files removed.
	$

If you now how many lines there should be, you can even print progress:

	$ rm -Rv foo | tlc -e 1000
	176%
	$

As you can see, 100% is not the real limit. :-)

You can customize the status line:

	$ rm -Rv foo | tlc -e 1000 -f '[%p%] %i files removed, last one: %s'
	[37%] 370 files removed, last one: foo/bar/buz
	<...>
	[176%] 1765 files removed, last one: foo
	$

Now a bit more complex task: lets monitor progress of the following pipe:

	find /usr/lib -type f -name '*.so' |
	xargs -J % objdump -p % >plugins.log

We can use `tee` or `tail -f` magic filling our screen:

	find /usr/local/lib -type f -name '*.so' |
	tee /dev/stderr |
	xargs -J % objdump -p % >plugins.log

This will help you to see the process is going on, but it won't be easy
to see how many files were already handled.  Lets use `tlc` instead:

	$ find /usr/lib -type f -name '*.so' |
	tlc -p |
	xargs -J % objdump -p % >plugins.log

This (shorter!) command will display just status line like:

	37...

until the end:

	1337
	$

For more details see the manual page, `tlc(1)`.
