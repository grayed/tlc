MAKEOBJDIR ?=	obj
.if exists(${.CURDIR}/../${MAKEOBJDIR})
TLC =		${.CURDIR}/../${MAKEOBJDIR}/tlc
.else
TLC =		${.CURDIR}/../tlc
.endif

TESTS !=	(cd ${.CURDIR}; ls -1 *.expected) | sed 's/\.expected$$//'

ARGS_01 =	-i 2 files
ARGS_02 =	-fpi 100 '[%i] %%%% got %s'
ARGS_03 =	-i 0
ARGS_04 =	-e 100 'number lines'
ARGS_05 =	-fe 100 '[%p%] lines'
ARGS_06 =	-d '' -p

TESTS_CHECKING_STDOUT =	02 06

.for _t in ${TESTS}
ARGS_${_t} ?=
CMDS_${_t} ?=		${.CURDIR}/${_t}.cmds
EXPECTED_${_t} ?=	${.CURDIR}/${_t}.expected
CHECK_STDOUT_${_t} ?=	no
EXTRA_${_t} ?=

STDOUT_${_t} =		${_t}.stdout
STDERR_${_t} =		${_t}.stderr
CLEAN_${_t} =		${STDOUT_${_t}} ${STDERR_${_t}}

.PHONY: ${_t} clean-${_t} run-test-${_t}

.if ${TESTS_CHECKING_STDOUT:M${_t}}
CHECK_STDOUT_${_t} =	yes
EXPECTEDOUT_${_t} ?=	${_t}.expectedout
CLEAN_${_t} +=		${EXPECTEDOUT_${_t}}
${_t}: ${EXPECTEDOUT_${_t}}
${EXPECTEDOUT_${_t}}: Makefile ${CMDS_${_t}}
	sh ${CMDS_${_t}} >$@
.else
CHECK_STDOUT_${_t} =	no
EXPECTEDOUT_${_t} ?=	/dev/null
.endif

CMakeLists.txt: gen-cmake-test-${_t}
gen-cmake-test-${_t}: .USE
	sh ${.CURDIR}/gen-cmake-test ${_t} ${CHECK_STDOUT_${_t}} \
	   "${ARGS_${_t}:Q}" >>$@

${_t}: ${STDOUT_${_t}} ${STDERR_${_t}}
${STDOUT_${_t}} ${STDERR_${_t}}: run-test-${_t}
run-test-${_t}:
	@echo ${TLC} ${ARGS_${_t}:Q} ">${STDOUT_${_t}} 2>${STDERR_${_t}}"
	@sh ${.CURDIR}/run-test ${_t} ${CHECK_STDOUT_${_t}} ${TLC} ${ARGS_${_t}}

clean: clean-${_t}
clean-${_t}:
	rm -f ${CLEAN_${_t}}
.endfor

CMakeLists.txt: Makefile gen-cmake-test
	@echo 'add_custom_target(prepare-expected-stdout ALL DEPENDS' >$@
.for _t in ${TESTS_CHECKING_STDOUT}
	@echo '                  ${EXPECTEDOUT_${_t}}' >>$@
.endfor
	@echo '                  COMMENT "generating expected stdout files"' >>$@
	@echo '                 )' >>$@

REGRESS_TARGETS ?=	${TESTS}

.include <bsd.regress.mk>
