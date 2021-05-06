# OpenBSD build file, for portable builds please use CMake
PROG =	tlc
COPTS =	-Wall

.include <bsd.prog.mk>

all: README.md

README.md: tlc.1
	mandoc -Tmarkdown tlc.1 >$@
	sed -i '$$ d' $@

.PHONY: test clean-test
test:
	cd ${.CURDIR}/tests && ${.MAKE}

clean-test:
	cd ${.CURDIR}/tests && ${.MAKE} clean
