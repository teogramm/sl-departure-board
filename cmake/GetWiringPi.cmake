message(STATUS "Getting wiringPi")

set(WPI_VERSION 3.16)

set(wiringPi_patch git apply ${CMAKE_SOURCE_DIR}/patches/wiringPi.patch || git apply ${CMAKE_SOURCE_DIR}/patches/wiringPi.patch -R --check)
FetchContent_Declare(
        wiringPi
        GIT_REPOSITORY https://github.com/WiringPi/WiringPi.git
        GIT_TAG ${WPI_VERSION}
        PATCH_COMMAND ${wiringPi_patch}
        UPDATE_DISCONNECTED 1
)
FetchContent_MakeAvailable(wiringPi)