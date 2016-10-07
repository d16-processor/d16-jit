include(LibFindMacros)
libfind_pkg_check_modules(libjit_PKGCONF libjit)

find_path(libjit_INCLUDE_DIR
        NAMES jit/jit.h
        PATHS ${libjit_PKGCONF_INCLUDE_DIRS}
)
find_library(libjit_LIBRARY
        NAMES jit
        PATHS ${libjit_PKGCONF_LIBRARY_DIRS}
)
set(libjit_PROCESS_INCLUDES libjit_INCLUDE_DIR )
set(libjit_PROCESS_LIBS libjit_LIBRARY)
libfind_process(libjit)