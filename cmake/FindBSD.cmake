# copied from Swift-2, Apache 2.0 licensed
# author: https://github.com/stormbrew
# source: https://github.com/hsavit1/swift-2/blob/master/cmake/modules/FindBSD.cmake

# Find libbsd

find_package(PkgConfig)
pkg_check_modules(PC_BSD QUIET bsd)
set(BSD_DEFINITIONS ${PC_BSD_CFLAGS_OTHER})

find_path(BSD_INCLUDE_DIR bsd/stdlib.h
    HINTS ${PC_BSD_INCLUDEDIR} ${PC_BSD_INCLUDE_DIRS})
set(BSD_INCLUDE_DIRS ${BSD_INCLUDE_DIR}/bsd)

find_library(BSD_LIBRARY NAMES bsd
  HINTS ${PC_BSD_LIBDIR} ${PC_BSD_LIBRARY_DIRS})
set(BSD_LIBRARIES ${BSD_LIBRARY})

set(BSD_REQUIRED BSD_LIBRARY BSD_INCLUDE_DIR)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(BSD DEFAULT_MSG ${BSD_REQUIRED})

mark_as_advanced(${BSD_REQUIRED})
