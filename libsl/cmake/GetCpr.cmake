message(STATUS "Getting cpr")
set(CPR_VERSION 1.11.0)
FetchContent_Declare(cpr
        GIT_REPOSITORY https://github.com/libcpr/cpr.git
        GIT_TAG ${CPR_VERSION}
        EXCLUDE_FROM_ALL)
FetchContent_MakeAvailable(cpr)