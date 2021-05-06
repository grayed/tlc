MAKEOBJDIR ?=	obj
.if exists(${.CURDIR}/../${MAKEOBJDIR})
TLC =		${.CURDIR}/../${MAKEOBJDIR}/tlc
.else
TLC =		${.CURDIR}/../tlc
.endif

TESTS !=	(cd ${.CURDIR}; ls -1 *.expected) | sed 's/\.expected$$//'

CMDS_01 =	sleep 1; echo foo; sleep 2; echo bar; sleep 1
ARGS_01 =	-i 2 files

CMDS_02 =	for b in `jot 99`; do \
			{ jot -s n $$b; echo -n _; } | \
			awk -v RS=_ -v ORS=_ '{system("sleep 0.1"); print}'; \
		done
ARGS_02 =	-fpi 100 '[%i] %%%% got %s'
EXPECTEDOUT_02 =	${.OBJDIR}/02.expectedout
EXTRA_02 =	${EXPECTEDOUT_02}
02: ${EXPECTEDOUT_02}
${EXPECTEDOUT_02}:
	${CMDS_02} >$@

CMDS_03 =	jot 10
ARGS_03 =	-i 0

CMDS_04 =	jot 1000
ARGS_04 =	-e 100 'number lines'

CMDS_05 =	jot 1000
ARGS_05 =	-fe 100 '[%p%] lines'

.PHONY:	gen

.for _t in ${TESTS}
ARGS_${_t} ?=
EXPECTED_${_t} ?=	${.CURDIR}/${_t}.expected
EXPECTEDOUT_${_t} ?=
EXTRA_${_t} ?=
STDOUT_${_t} =		${.OBJDIR}/${_t}.stdout
STDERR_${_t} =		${.OBJDIR}/${_t}.stderr

.PHONY: ${_t} clean-${_t}

${_t}:
	rm -f ${STDOUT_${_t}} ${STDERR_${_t}}
	(${CMDS_${_t}}) | ${TLC} ${ARGS_${_t}} >${STDOUT_${_t}} 2>${STDERR_${_t}}
	diff -u ${EXPECTED_${_t}} ${STDERR_${_t}}
.if empty(EXPECTEDOUT_${_t})
	diff -u /dev/null ${STDOUT_${_t}}
.else
	diff -u ${EXPECTEDOUT_${_t}} ${STDOUT_${_t}}
.endif

clean: clean-${_t}
clean-${_t}:
	rm -f ${STDOUT_${_t}} ${STDERR_${_t}} ${EXTRA_${_t}}
.endfor

REGRESS_TARGETS ?=	${TESTS}

.include <bsd.regress.mk>
