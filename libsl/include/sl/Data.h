#ifndef SL_DATA_H
#define SL_DATA_H

#include <string>
#include <vector>
#include <optional>

namespace SL {
    /**
     * All available transport modes.
     */
    enum class Mode {
        BUS,
        TRAM,
        METRO,
        TRAIN,
        FERRY,
        SHIP,
        TAXI
    };

    /**
     * Converts the given string to a SL::Mode object.
     * @return Corresponding Mode value, std::nullopt if the string does not match any mode
     */
    std::optional<Mode> string_to_mode(const std::string &mode);

    /**
     * Converts the given Mode to its string representation
     * @param m
     * @return
     */
    std::string mode_to_string(Mode& m);

    /**
     * Contains information about an upcoming departure.
     */
    struct Departure {
        std::string line;
        std::string destination;
        std::string arrival;
    };

    /**
     * Contains information about upcoming departures and deviations applying to the stop.
     */
    struct StopStatus {
        std::vector<Departure> departures;
        std::vector<std::string> deviations;
    };
}

#endif //DATA_H
