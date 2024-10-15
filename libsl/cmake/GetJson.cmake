message(STATUS "Getting Json")
set(JSON_VERSION 3.11.3)
FetchContent_Declare(json
        URL https://github.com/nlohmann/json/archive/refs/tags/v${JSON_VERSION}.tar.gz
        EXCLUDE_FROM_ALL)
FetchContent_MakeAvailable(json)