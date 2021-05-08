# OpenBSD build file, for portable builds please use CMake
PROG =	tlc
COPTS =	-Wall
BINDIR ?=	/usr/local/bin
MANDIR ?=	/usr/local/man/man

.include <bsd.prog.mk>

all: manlint

.PHONY: test clean-test
test:
	cd ${.CURDIR}/tests && ${.MAKE}

clean-test:
	cd ${.CURDIR}/tests && ${.MAKE} clean
