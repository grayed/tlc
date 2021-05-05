# OpenBSD build file, for portable builds please use CMake
PROG =	tlc
COPTS =	-Wall

.PHONY: test
test:
	cd ${.CURDIR}/tests && ${.MAKE}

all: README.md
README.md: tlc.1
	mandoc -Tmarkdown tlc.1 >$@
	sed -i '$$ d' $@

.include <bsd.prog.mk>
