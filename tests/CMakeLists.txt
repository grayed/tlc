add_custom_target(prepare-expected-stdout ALL DEPENDS
                  02.expectedout
                  COMMENT "generating expected stdout files"
                 )

add_test(NAME test-01
         COMMAND sh -c "sh ${CMAKE_CURRENT_SOURCE_DIR}/run-test 01 no $<TARGET_FILE:tlc> -i\ 2\ files"
        )

add_custom_command(OUTPUT 02.expectedout
                   COMMAND sh -c "sh ${CMAKE_CURRENT_SOURCE_DIR}/02.cmds >02.expectedout"
                   DEPENDS 02.cmds
                   COMMENT "sh 02.cmds >02.expectedout"
                   VERBATIM
                  )

add_test(NAME test-02
         COMMAND sh -c "sh ${CMAKE_CURRENT_SOURCE_DIR}/run-test 02 yes $<TARGET_FILE:tlc> -fpi\ 100\ \'\[%i\]\ %%%%\ got\ %s\'"
        )

add_test(NAME test-03
         COMMAND sh -c "sh ${CMAKE_CURRENT_SOURCE_DIR}/run-test 03 no $<TARGET_FILE:tlc> -i\ 0"
        )

add_test(NAME test-04
         COMMAND sh -c "sh ${CMAKE_CURRENT_SOURCE_DIR}/run-test 04 no $<TARGET_FILE:tlc> -e\ 100\ \'number\ lines\'"
        )

add_test(NAME test-05
         COMMAND sh -c "sh ${CMAKE_CURRENT_SOURCE_DIR}/run-test 05 no $<TARGET_FILE:tlc> -fe\ 100\ \'\[%p%\]\ lines\'"
        )
