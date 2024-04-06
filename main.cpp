#include <u8g2.h>
#include <wiringPi.h>
#include "U8G2_SSD1322_NHD_256X64_F_8080_RPi.h"
#include <sl/DepartureFetcher.h>
#include "esseltub.h"
#include "SLDisplay.h"


int main() {
//
//    auto departures = std::vector{
//            Departure{"18", "Hässelby Strand", "2 min"},
//            Departure{"18", "Odenplan", "7 min"},
//            Departure{"18", "Alvik", "05:45"}
//    };
//    render_departures(departures, pi);
//    pi.clearDisplay();
//
//    drawBannerText(pi.getU8g2(), 10);
//
//    auto a = DepartureFetcher();
//
//    auto d = a.fetch_departures(siteid, 1, Mode::METRO);
//    std::cout << d[0].destination;

//    auto siteid = 9181;
    auto siteid = 9294;
    wiringPiSetup();
    U8G2_SSD1322_NHD_256X64_F_8080_RPI pi{
            U8G2_R0,
            16, 1, 0, 2, 3, 4, 5, 6,
            28, 27, 25
    };
    pi.begin();
    pi.setFlipMode(1);
    pi.setFont(esseltub);

    auto display = SLDisplay(pi, siteid, 1, Mode::METRO);
    display.main_loop();

    return 0;
}
