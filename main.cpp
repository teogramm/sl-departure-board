#include <u8g2.h>
#include <wiringPi.h>
#include "U8G2_SSD1322_NHD_256X64_F_8080_RPi.h"
#include "SLDisplay.h"
#include "Argparser.h"


int main(int argc, char** argv) {
    wiringPiSetup();

    U8G2_SSD1322_NHD_256X64_F_8080_RPI pi{
            U8G2_R0,
            16, 1, 0, 2, 3, 4, 5, 6,
            28, 27, 25
    };
    pi.begin();
    pi.setFlipMode(1);

    auto config = Argparser::parse_arguments(argc, argv);

    auto display = SLDisplay(pi, config);
    display.start();

    return 0;
}
