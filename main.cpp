#include <csignal>
#include <initializer_list>
#include <functional>
#include <algorithm>

#include <u8g2.h>
#include <wiringPi.h>
#include "U8G2_SSD1322_NHD_256X64_F_8080_RPi.h"
#include "SLDisplay.h"
#include "Argparser.h"

// https://stackoverflow.com/questions/68100462/signal-handling-c
sigset_t make_sigset(std::initializer_list<int32_t> signals)
{
    sigset_t set;
    const int32_t result = sigemptyset(&set);

    std::for_each(signals.begin(), signals.end(),
                  [sigset = &set](auto& signal) {
        sigaddset(sigset, signal);
    });

    return set;
}

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

    auto sigset = make_sigset({SIGTERM, SIGINT, SIGABRT});

    pthread_sigmask(SIG_BLOCK, &sigset, nullptr);

    int32_t last_signal;
    // Wait until we receive a signal and cleanup the display
    sigwait(&sigset, &last_signal);

    display.stop();

    return 0;
}
