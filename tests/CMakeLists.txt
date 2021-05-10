# This file is generated from (BSD make) Makefile

add_custom_target(prepare-expected-stdout ALL DEPENDS
                  02.expectedout
                  06.expectedout
                  COMMENT "generating expected stdout files"
                 )

add_test(NAME test-01
         COMMAND sh ${CMAKE_CURRENT_SOURCE_DIR}/run-test 01 no $<TARGET_FILE:tlc> -i 2 files
        )

add_custom_target(test-01
                  sh ${CMAKE_CURRENT_SOURCE_DIR}/run-test 01 no $<TARGET_FILE:tlc> -i 2 files
                  COMMENT "Running test 01"
                  VERBATIM
                 )

add_custom_command(OUTPUT 02.expectedout
                   COMMAND sh ${CMAKE_CURRENT_SOURCE_DIR}/02.cmds ">02.expectedout"
                   DEPENDS 02.cmds
                   COMMENT "sh 02.cmds >02.expectedout"
                  )

add_test(NAME test-02
         COMMAND sh ${CMAKE_CURRENT_SOURCE_DIR}/run-test 02 yes $<TARGET_FILE:tlc> -fpi 100 "\[%i\] %%%% got %s"
        )

add_custom_target(test-02
                  sh ${CMAKE_CURRENT_SOURCE_DIR}/run-test 02 yes $<TARGET_FILE:tlc> -fpi 100 "\[%i\] %%%% got %s"
                  COMMENT "Running test 02"
                  DEPENDS 02.expectedout
                  VERBATIM
                 )

add_test(NAME test-03
         COMMAND sh ${CMAKE_CURRENT_SOURCE_DIR}/run-test 03 no $<TARGET_FILE:tlc> -i 0
        )

add_custom_target(test-03
                  sh ${CMAKE_CURRENT_SOURCE_DIR}/run-test 03 no $<TARGET_FILE:tlc> -i 0
                  COMMENT "Running test 03"
                  VERBATIM
                 )

add_test(NAME test-04
         COMMAND sh ${CMAKE_CURRENT_SOURCE_DIR}/run-test 04 no $<TARGET_FILE:tlc> -e 100 "number lines"
        )

add_custom_target(test-04
                  sh ${CMAKE_CURRENT_SOURCE_DIR}/run-test 04 no $<TARGET_FILE:tlc> -e 100 "number lines"
                  COMMENT "Running test 04"
                  VERBATIM
                 )

add_test(NAME test-05
         COMMAND sh ${CMAKE_CURRENT_SOURCE_DIR}/run-test 05 no $<TARGET_FILE:tlc> -fe 100 "\[%p%\] lines"
        )

add_custom_target(test-05
                  sh ${CMAKE_CURRENT_SOURCE_DIR}/run-test 05 no $<TARGET_FILE:tlc> -fe 100 "\[%p%\] lines"
                  COMMENT "Running test 05"
                  VERBATIM
                 )

add_custom_command(OUTPUT 06.expectedout
                   COMMAND sh ${CMAKE_CURRENT_SOURCE_DIR}/06.cmds ">06.expectedout"
                   DEPENDS 06.cmds
                   COMMENT "sh 06.cmds >06.expectedout"
                  )

add_test(NAME test-06
         COMMAND sh ${CMAKE_CURRENT_SOURCE_DIR}/run-test 06 yes $<TARGET_FILE:tlc> -d "" -p
        )

add_custom_target(test-06
                  sh ${CMAKE_CURRENT_SOURCE_DIR}/run-test 06 yes $<TARGET_FILE:tlc> -d "" -p
                  COMMENT "Running test 06"
                  DEPENDS 06.expectedout
                  VERBATIM
                 )
