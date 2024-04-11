#ifndef SL_DATA_H
#define SL_DATA_H

#include <cpr/cpr.h>
#include <string>
#include <vector>
#include <optional>

namespace SL {
    enum class Mode {
        BUS,
        TRAM,
        METRO,
        TRAIN,
        FERRY,
        SHIP,
        TAXI
    };

    std::optional<Mode> string_to_mode(const std::string &mode);

    struct Departure {
        std::string line;
        std::string destination;
        std::string arrival;
    };

    struct StopStatus {
        std::vector<Departure> departures;
        std::vector<std::string> deviations;
    };
}

#endif //DATA_H
