set(WPI_VERSION 3.2)
project(wiringPi LANGUAGES C VERSION ${WPI_VERSION})


set(wpi_patch git apply ${CMAKE_SOURCE_DIR}/patches/wiringpi.patch)
FetchContent_Declare(
        wiringPi
        GIT_REPOSITORY https://github.com/WiringPi/WiringPi.git
        GIT_TAG ${WPI_VERSION}
        PATCH_COMMAND ${wpi_patch}
        UPDATE_DISCONNECTED 1
)
FetchContent_Populate(wiringPi)

set(CMAKE_THREAD_LIBS_INIT "-lpthread")
set(CMAKE_HAVE_THREADS_LIBRARY 1)
set(CMAKE_USE_WIN32_THREADS_INIT 0)
set(CMAKE_USE_PTHREADS_INIT 1)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

file(GLOB WPI_SRC_FILES ${wiringpi_SOURCE_DIR}/wiringPi/*.c)
file(GLOB WPI_HEADERS ${wiringpi_SOURCE_DIR}/wiringPi/*.h)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# https://blog.vito.nyc/posts/cmake-pkg/

# Create our library target
add_library(wiringPi)
#
target_sources(wiringPi PRIVATE ${WPI_SRC_FILES})
target_sources(wiringPi PUBLIC
        FILE_SET HEADERS
        BASE_DIRS ${wiringpi_SOURCE_DIR}/wiringPi
        FILES ${WPI_HEADERS}
)
set_target_properties(wiringPi
        PROPERTIES
        LINKER_LANGUAGE C
)
target_link_libraries(wiringPi ${CMAKE_THREAD_LIBS_INIT} m rt crypt)

#write_basic_package_version_file(
#        ${CMAKE_CURRENT_BINARY_DIR}/wiringpi-config-version.cmake
#        COMPATIBILITY AnyNewerVersion
#)

install(TARGETS wiringPi EXPORT wiringPiTargets FILE_SET HEADERS)
install(EXPORT wiringPiTargets DESTINATION ${CMAKE_INSTALL_DATADIR}/wiringpi)

