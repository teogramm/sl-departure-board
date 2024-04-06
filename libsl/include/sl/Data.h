#ifndef WIRINGPI_DATA_H
#define WIRINGPI_DATA_H
#include <string>
#include <vector>

struct Departure{
    std::string line;
    std::string destination;
    std::string arrival;
};

struct StopStatus{
    std::vector<Departure> departures;
    std::vector<std::string> deviations;
};

#endif //WIRINGPI_DATA_H
