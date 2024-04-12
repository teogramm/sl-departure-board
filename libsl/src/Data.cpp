#include "Data.h"

namespace SL {
    std::optional<Mode> string_to_mode(const std::string &mode) {
        if (mode == "METRO")
            return Mode::METRO;
        else if (mode == "BUS")
            return Mode::BUS;
        else if (mode == "TRAM")
            return Mode::TRAM;
        else if (mode == "TRAIN")
            return Mode::TRAIN;
        else if (mode == "FERRY")
            return Mode::FERRY;
        else if (mode == "SHIP")
            return Mode::SHIP;
        else if (mode == "TAXI")
            return Mode::TAXI;
        else
            return std::nullopt;
    }

    std::string mode_to_string(Mode& m) {
        switch (m) {
            case Mode::BUS:
                return "BUS";
            case Mode::TRAM:
                return "TRAM";
            case Mode::METRO:
                return "METRO";
            case Mode::TRAIN:
                return "TRAIN";
            case Mode::FERRY:
                return "FERRY";
            case Mode::SHIP:
                return "SHIP";
            case Mode::TAXI:
                return "TAXI";
        }
        return "";
    }
}