project(sl LANGUAGES CXX VERSION 1.0)

set(CPR_USE_SYSTEM_CURL ON)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
        GIT_TAG 1.10.5)
FetchContent_MakeAvailable(cpr)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

file(GLOB SL_SOURCES src/*.cpp)
file(GLOB SL_HEADERS src/*.h)

add_library(sl STATIC)
target_compile_features(sl PUBLIC cxx_std_20)
target_sources(sl PRIVATE ${SL_SOURCES})
target_sources(sl PUBLIC FILE_SET HEADERS BASE_DIRS include)
target_link_libraries(sl PUBLIC cpr::cpr)
target_link_libraries(sl PRIVATE nlohmann_json::nlohmann_json)
