
#ifndef ARGPARSER_H
#define ARGPARSER_H

#include <tclap/CmdLine.h>
#include "SLDisplay.h"

class Argparser {
public:
    static SLDisplay::Config parse_arguments(int argc, char **argv);
};

#endif //WIRINGPI_ARGPARSER_H
