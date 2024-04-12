cmake_minimum_required(VERSION 3.25)
project(sl_pi_cpp)

include(FetchContent)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

set(BUILD_SHARED_LIBS OFF)
#set(CMAKE_EXE_LINKER_FLAGS "-static")

include(GetWiringPi)

message(STATUS "Getting u8g2")
set(u8g2_patch git apply ${CMAKE_SOURCE_DIR}/patches/u8g2.patch)
FetchContent_Declare(
        u8g2
        GIT_REPOSITORY https://github.com/olikraus/u8g2.git
        PATCH_COMMAND ${u8g2_patch}
        UPDATE_DISCONNECTED 1
)
FetchContent_MakeAvailable(u8g2)

set(TCLAP_VERSION 1.4.0-rc1)

message(STATUS "Getting TCLAP")
FetchContent_Declare(
        tclap
        GIT_REPOSITORY https://git.code.sf.net/p/tclap/code tclap-code
        GIT_TAG 1.4
)
FetchContent_MakeAvailable(tclap)

add_subdirectory(libsl)

set(CMAKE_CXX_STANDARD 20)

file(GLOB SOURCES "src/*.cpp" "src/*.c")
add_executable(sl_pi_cpp main.cpp ${SOURCES} include/U8G2_SSD1322_NHD_256X64_F_8080_RPi.h)
target_include_directories(sl_pi_cpp PRIVATE include)
target_link_libraries(sl_pi_cpp PRIVATE  u8g2 wiringPi sl TCLAP)
target_compile_features(sl_pi_cpp PUBLIC cxx_std_20)

#    target_compile_options(sl_pi_cpp PRIVATE -fsanitize=address)
#    target_link_options(sl_pi_cpp PRIVATE -fsanitize=address)