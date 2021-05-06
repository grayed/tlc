cmake_minimum_required(VERSION 3.7)

project(tlc LANGUAGES C)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

add_compile_options(-Wall)
# notsupp in 3.7 string(PREPEND CMAKE_C_FLAGS_DEBUG "-O0 ")
set(CMAKE_C_FLAGS_DEBUG "-O0 ${CMAKE_C_FLAGS_DEBUG}")
if (CMAKE_BUILD_TYPE STREQUAL Debug)
	add_definitions(-DDEBUG)
endif()

add_executable(tlc tlc.c)

if (APPLE OR CMAKE_SYSTEM_NAME MATCHES ".*BSD.*")
	message(STATUS "It looks you're running BSD system and do not need libbsd")
else()
	find_package(BSD REQUIRED)
	list(APPEND BSD_DEFINITIONS -DLIBBSD_OVERLAY)
	list(APPEND BSD_DEFINITIONS -D_BSD_SOURCE=1 -D_DEFAULT_SOURCE=1 -D_GNU_SOURCE=1 -D_XOPEN_SOURCE=700)
	target_compile_definitions(tlc PRIVATE ${BSD_DEFINITIONS})
	target_include_directories(tlc BEFORE PRIVATE ${BSD_INCLUDE_DIRS})
	target_link_libraries(tlc ${BSD_LIBRARIES})
endif()

install(TARGETS tlc RUNTIME DESTINATION bin)
