# OpenBSD build file, for portable builds please use CMake
PROG =	tlc
COPTS =	-Wall

.PHONY: test
test:
	cd ${.CURDIR}/tests && ${.MAKE}

.include <bsd.prog.mk>
