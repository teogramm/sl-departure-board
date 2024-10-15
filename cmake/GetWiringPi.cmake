set(WPI_VERSION 3.10)

FetchContent_Declare(
        wiringPi
        GIT_REPOSITORY https://github.com/WiringPi/WiringPi.git
        GIT_TAG ${WPI_VERSION}
        UPDATE_DISCONNECTED 1
)
FetchContent_Populate(wiringPi)
configure_file(${CMAKE_SOURCE_DIR}/patches/wiringpi_CMakeLists.txt ${wiringpi_SOURCE_DIR}/CMakeLists.txt COPYONLY)
add_subdirectory(${wiringpi_SOURCE_DIR} EXCLUDE_FROM_ALL)
