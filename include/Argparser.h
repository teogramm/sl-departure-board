
#ifndef ARGPARSER_H
#define ARGPARSER_H

#include <tclap/CmdLine.h>
#include "SLDisplay.h"

class Argparser {
    class TimeVerifier: public TCLAP::Constraint<std::string>{
    public:
        bool check(const std::basic_string<char> &value) const override;
        std::string description() const override;
        std::string shortID() const override;
    };
public:
    static SLDisplay::Config parse_arguments(int argc, char **argv);
};

#endif //WIRINGPI_ARGPARSER_H
