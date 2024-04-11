#include "Argparser.h"
#include <sl/Data.h>


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

        cmd.parse(argc, argv);

        // Handle optional arguments
        auto direction_param = direction_code.isSet() ? std::optional(direction_code.getValue()) : std::nullopt;
        auto mode_param = modes.isSet() ? ::SL::string_to_mode(modes.getValue()) : std::nullopt;

        return SLDisplay::Config(site_id.getValue(), direction_param, mode_param);
    } catch (TCLAP::ArgException &e) {
        std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
    }
}
