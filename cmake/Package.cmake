# these are cache variables, so they could be overwritten with -D,
set(CPACK_PACKAGE_NAME sl-departure-board
        CACHE STRING "The resulting package name"
)
# which is useful in case of packing only selected components instead of the whole thing
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "SL departure display, inspired by the Tunnelbana screens."
        CACHE STRING "Package description for the package metadata"
)
set(CPACK_PACKAGE_VENDOR "Theo Grammenos")

set(CPACK_VERBATIM_VARIABLES YES)

set(CPACK_PACKAGE_INSTALL_DIRECTORY ${CPACK_PACKAGE_NAME})
#SET(CPACK_OUTPUT_FILE_PREFIX "${CMAKE_SOURCE_DIR}/_packages")

# https://unix.stackexchange.com/a/11552/254512
set(CPACK_PACKAGING_INSTALL_PREFIX "/opt/${CPACK_PACKAGE_NAME}")

set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})

set(CPACK_PACKAGE_CONTACT "teogramm@outlook.com")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Theo Grammenos")

set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_COMPONENTS_GROUPING ALL_COMPONENTS_IN_ONE)
set(CPACK_DEB_COMPONENT_INSTALL YES)
# Very ugly hack to build only this. Otherwise CPack always includes files with an unspecified component name in
# the output.
set(CMAKE_INSTALL_DEFAULT_COMPONENT_NAME "sl_pi_cpp-Runtime")
set(CPACK_COMPONENTS_ALL "sl_pi_cpp-Runtime")

include(CPack)
message(STATUS "Components to pack: ${CPACK_COMPONENTS_ALL}")