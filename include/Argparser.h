
#ifndef ARGPARSER_H
#define ARGPARSER_H

#include <tclap/CmdLine.h>
#include "SLDisplay.h"

/**
 * Command line argument parser.
 */
class Argparser {
private:
    /**
     * Implements a TCLAP Constraint for the sleep time parameter and ensures it is a valid time range of the format
     * XX:XX-XX:XX
     */
    class TimeVerifier: public TCLAP::Constraint<std::string>{
    public:
        bool check(const std::basic_string<char> &value) const override;
        std::string description() const override;
        std::string shortID() const override;
    };
public:
    /**
     * Converts the command-line arguments into an SLDisplay::Config object.
     */
    static SLDisplay::Config parse_arguments(int argc, char **argv);
};

#endif //WIRINGPI_ARGPARSER_H
