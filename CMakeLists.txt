cmake_minimum_required(VERSION 3.1)

project(tlc LANGUAGES C)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# find_path(HAVE_SYS_SYSMACROS_H sys/sysmacros.h)
# if (HAVE_SYS_SYSMACROS_H)
#     add_compile_definitions(HAVE_SYS_SYSMACROS_H)
# endif()

add_executable(tlc tlc.c)
install(TARGETS tlc RUNTIME DESTINATION bin)
