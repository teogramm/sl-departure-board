find_path(
        TCLAP_INCLUDE_DIR tclap/CmdLine.h
        HINTS ${TCLAP_ROOT} $ENV{TRCLAP_ROOT}
)

if (TCLAP_INCLUDE_DIR)
    set(TCLAP_FOUND TRUE)
    add_library(tclap::tclap IMPORTED INTERFACE)
    target_include_directories(tclap::tclap INTERFACE ${TCLAP_INCLUDE_DIR})
else()
    set(TCLAP_FOUND FALSE)
    if (TCLAP_FIND_REQUIRED)
        message(FATAL_ERROR
                "tclap could not be found!  Try setting the environment variable `tclap_ROOT` "
                "such that the file `$tclap_ROOT/tclap/CmdLine.h` can be found."
        )
    endif()
endif()
