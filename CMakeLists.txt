cmake_minimum_required(VERSION 3.25)
project(sl_pi_cpp)

include(FetchContent)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

set(BUILD_SHARED_LIBS OFF)
#set(CMAKE_EXE_LINKER_FLAGS "-static")

find_package(wiringPi)
if (NOT wiringPi_FOUND)
    include(cmake/GetWiringPi.cmake)
endif ()
include(cmake/Getu8g2.cmake)
find_package(TCLAP)
if (NOT TCLAP_FOUND)
    include(cmake/GetTCLAP.cmake)
endif ()

add_subdirectory(libsl)

set(CMAKE_CXX_STANDARD 20)

file(GLOB SOURCES "src/*.cpp" "src/*.c")
add_executable(sl_pi_cpp main.cpp ${SOURCES} include/U8G2_SSD1322_NHD_256X64_F_8080_RPi.h)
target_include_directories(sl_pi_cpp PRIVATE include)
target_link_libraries(sl_pi_cpp PRIVATE u8g2 wiringPi sl TCLAP)
target_compile_features(sl_pi_cpp PUBLIC cxx_std_20)

install(TARGETS sl_pi_cpp RUNTIME DESTINATION bin)
include(CPack)

#    target_compile_options(sl_pi_cpp PRIVATE -fsanitize=address)
#    target_link_options(sl_pi_cpp PRIVATE -fsanitize=address)
