#ifndef DATA_H
#define DATA_H

#include "cpr/cpr.h"
#include <string>
#include <vector>

namespace SL {
    enum class Mode{
        BUS,
        TRAM,
        METRO,
        TRAIN,
        FERRY,
        SHIP,
        TAXI
    };
    struct Departure{
        std::string line;
        std::string destination;
        std::string arrival;
    };
    struct StopStatus{
        std::vector<Departure> departures;
        std::vector<std::string> deviations;
    };
}

#endif //DATA_H
