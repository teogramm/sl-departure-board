message(STATUS "Getting u8g2")
# Also check if patch has already been applied
set(u8g2_patch git apply ${CMAKE_SOURCE_DIR}/patches/u8g2.patch || git apply ${CMAKE_SOURCE_DIR}/patches/u8g2.patch -R --check)
FetchContent_Declare(
        u8g2
        GIT_REPOSITORY https://github.com/olikraus/u8g2.git
        PATCH_COMMAND ${u8g2_patch}
        GIT_TAG 9c841fafef11f430a225aa692944264c26a2405d # 2.36.2
        UPDATE_DISCONNECTED 1
        EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(u8g2)