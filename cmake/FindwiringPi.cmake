# https://stackoverflow.com/questions/30424236/add-wiringpi-lib-to-cmake-on-raspberrypi
find_library(wiringPi_LIBRARY NAMES wiringPi)
find_path(wiringPi_INCLUDE_DIRS NAMES wiringPi.h)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(wiringPi DEFAULT_MSG wiringPi_LIBRARY wiringPi_INCLUDE_DIRS)

if(wiringPi_FOUND AND NOT TARGET wiringPi::wiringPi)
    add_library(wiringPi::wiringPi UNKNOWN IMPORTED)
    set_target_properties(wiringPi::wiringPi PROPERTIES
            IMPORTED_LOCATION "${wiringPi_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${wiringPi_INCLUDE_DIRS}"
    )
endif ()