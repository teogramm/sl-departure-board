#include "Argparser.h"
#include <sl/Data.h>
#include <regex>
#include <sstream>

std::tuple<std::string, std::string> create_time_tuple(const std::string &param_string) {
    auto result = std::vector<std::string>();
    auto ss = std::stringstream(param_string);
    auto segment = std::string();

    while (std::getline(ss, segment, '-')) {
        result.push_back(segment);
    }
    return std::make_tuple(result.at(0), result.at(1));
}


SLDisplay::Config Argparser::parse_arguments(int argc, char **argv) {
    using namespace TCLAP;
    try {
        auto cmd = CmdLine("SL Departures Display");

        auto site_id = UnlabeledValueArg<int>("site_id", "ID of the site to fetch departures for", true, 0, "number");
        cmd.add(site_id);

        auto direction_code = ValueArg<int>("d", "direction", "direction code", false, 0, "integer");
        cmd.add(direction_code);

        auto allowed_modes = std::vector<std::string>();
        allowed_modes.emplace_back("BUS");
        allowed_modes.emplace_back("METRO");
        allowed_modes.emplace_back("TRAM");
        allowed_modes.emplace_back("TRAIN");
        allowed_modes.emplace_back("FERRY");
        allowed_modes.emplace_back("SHIP");

        auto modes_constraint = ValuesConstraint(allowed_modes);
        auto modes = ValueArg<std::string>("m", "mode", "mode to fetch departures", false, "", &modes_constraint);
        cmd.add(modes);

        auto time_verifier = TimeVerifier();
        auto sleep_time = ValueArg<std::string>("s", "sleep-time", "set time period when the screen deactivates", false,
                                                "", &time_verifier);
        cmd.add(sleep_time);

        cmd.parse(argc, argv);

        // Handle optional arguments
        auto direction_param = direction_code.isSet() ? std::optional(direction_code.getValue()) : std::nullopt;
        auto mode_param = modes.isSet() ? ::SL::string_to_mode(modes.getValue()) : std::nullopt;
        auto sleep_time_param = sleep_time.isSet() ? std::optional<std::tuple<std::string, std::string>>(
                create_time_tuple(sleep_time.getValue())) : std::nullopt;

        return SLDisplay::Config(site_id.getValue(), direction_param, mode_param,
                                 sleep_time_param);
    } catch (TCLAP::ArgException &e) {
        std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
    }
}

bool Argparser::TimeVerifier::check(const std::basic_string<char> &value) const {
    // TODO: Check that start <= end
    auto reg = std::regex{"(0[0-9]|1[0-9]|2[0-3]):[0-5][0-9]-(0[0-9]|1[0-9]|2[0-3]):[0-5][0-9]"};
    return std::regex_match(value, reg);
}

std::string Argparser::TimeVerifier::description() const {
    return {"Time range in the form XX:XX-XX:XX"};
}

std::string Argparser::TimeVerifier::shortID() const {
    return {"XX:XX-XX:XX"};
}
