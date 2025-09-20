project(sl LANGUAGES CXX VERSION 1.0)

include(GNUInstallDirs)

# Statically link to CPR and the other libraries, but not to curl or ssl
set(BUILD_SHARED_LIBS OFF)

find_package(CURL)
if(CURL_FOUND)
    message(STATUS "System curl found")
    set(CPR_USE_SYSTEM_CURL ON)
endif ()
include(cmake/GetCpr.cmake)

include(cmake/GetJson.cmake)

file(GLOB SL_SOURCES src/*.cpp)
file(GLOB SL_HEADERS src/*.h)

add_library(sl STATIC)
target_compile_features(sl PUBLIC cxx_std_20)

target_sources(sl PRIVATE ${SL_SOURCES})
target_sources(sl PUBLIC FILE_SET HEADERS BASE_DIRS include)
target_include_directories(sl PRIVATE include/sl)
# https://stackoverflow.com/questions/67227735/cmake-target-not-in-export-set
# Since json is an interface library, there should be no problem with including it like that
target_link_libraries(sl PRIVATE cpr::cpr $<BUILD_INTERFACE:nlohmann_json::nlohmann_json>)

install(TARGETS sl EXPORT slTargets FILE_SET HEADERS)
install(EXPORT slTargets NAMESPACE sl:: DESTINATION ${CMAKE_INSTALL_DATADIR}/sl)

write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/sl-config-version.cmake
        COMPATIBILITY AnyNewerVersion
)

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/sl-config-version.cmake
        DESTINATION ${CMAKE_INSTALL_DATADIR}/sl
)
