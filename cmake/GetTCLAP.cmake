message(STATUS "Getting TCLAP")
FetchContent_Declare(
        tclap
        GIT_REPOSITORY https://git.code.sf.net/p/tclap/code
        GIT_TAG 1.4
        EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(tclap)