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
CHECK_STDOUT_02 =	yes

CMDS_03 =	jot 10
ARGS_03 =	-i 0

CMDS_04 =	jot 1000
ARGS_04 =	-e 100 'number lines'

CMDS_05 =	jot 1000
ARGS_05 =	-fe 100 '[%p%] lines'

.for _t in ${TESTS}
ARGS_${_t} ?=
EXPECTED_${_t} ?=	${.CURDIR}/${_t}.expected
CHECK_STDOUT_${_t} ?=	no
EXTRA_${_t} ?=

STDOUT_${_t} =		${.OBJDIR}/${_t}.stdout
STDERR_${_t} =		${.OBJDIR}/${_t}.stderr
CLEAN_${_t} =		${STDOUT_${_t}} ${STDERR_${_t}}

.PHONY: ${_t} clean-${_t} run-tlc-${_t}

.if ${CHECK_STDOUT_${_t}:L} == "yes"
EXPECTEDOUT_${_t} ?=	${.OBJDIR}/${_t}.expectedout
CLEAN_${_t} +=		${EXPECTEDOUT_${_t}}
${_t}: ${EXPECTEDOUT_${_t}}
${EXPECTEDOUT_${_t}}: Makefile
	${CMDS_${_t}} >$@
.else
EXPECTEDOUT_${_t} ?=	/dev/null
.endif

${_t}: ${STDOUT_${_t}} ${STDERR_${_t}}
	@stdout_okay=true; stderr_okay=true; \
	 diff -u ${EXPECTED_${_t}} ${STDERR_${_t}} || stdout_okay=false; \
	 diff -u ${EXPECTEDOUT_${_t}} ${STDOUT_${_t}} || stderr_okay=false; \
	 $$stdout_okay && $$stderr_okay

${STDOUT_${_t}} ${STDERR_${_t}}: run-tlc-${_t}

run-tlc-${_t}:
	(${CMDS_${_t}}) | ${TLC} ${ARGS_${_t}} >${STDOUT_${_t}} 2>${STDERR_${_t}}

clean: clean-${_t}
clean-${_t}:
	rm -f ${CLEAN_${_t}}
.endfor

REGRESS_TARGETS ?=	${TESTS}

.include <bsd.regress.mk>
