project(sl LANGUAGES CXX VERSION 1.0)

# Statically link to CPR and the other libraries, but not to curl or ssl
set(BUILD_SHARED_LIBS OFF)

set(CPR_USE_SYSTEM_CURL ON)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
        GIT_TAG 1.11.0)
FetchContent_MakeAvailable(cpr)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

file(GLOB SL_SOURCES src/*.cpp)
file(GLOB SL_HEADERS src/*.h)

add_library(sl STATIC)
target_compile_features(sl PUBLIC cxx_std_20)

target_sources(sl PRIVATE ${SL_SOURCES})
target_sources(sl PUBLIC FILE_SET HEADERS BASE_DIRS include)
target_include_directories(sl PRIVATE include/sl)
target_link_libraries(sl PRIVATE cpr::cpr nlohmann_json::nlohmann_json)
