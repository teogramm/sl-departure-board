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
file(GLOB SL_HEADERS include/**/*.h)

add_library(sl STATIC)
target_compile_features(sl PUBLIC cxx_std_20)

target_sources(sl PRIVATE ${SL_SOURCES})
target_sources(sl PUBLIC FILE_SET HEADERS BASE_DIRS include FILES ${SL_HEADERS})
target_include_directories(sl PRIVATE include/sl)
# https://stackoverflow.com/questions/67227735/cmake-target-not-in-export-set
# Since json is an interface library, there should be no problem with including it like that
target_link_libraries(sl PRIVATE cpr::cpr $<BUILD_INTERFACE:nlohmann_json::nlohmann_json>)

install(TARGETS sl
        EXPORT slTargets
        FILE_SET HEADERS
            COMPONENT ${PROJECT_NAME}-Development
        LIBRARY
            COMPONENT ${PROJECT_NAME}-Runtime
            NAMELINK_COMPONENT ${PROJECT_NAME}-Development
        ARCHIVE
            COMPONENT ${PROJECT_NAME}-Development
)
install(EXPORT slTargets
        NAMESPACE sl::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sl
        COMPONENT ${PROJECT_NAME}-Development
)

write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/slConfigVersion2.cmake
        COMPATIBILITY AnyNewerVersion
)

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/slConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sl
)
