
#ifndef DEPARTUREFETCHER_H
#define DEPARTUREFETCHER_H
#include <string>
#include <cpr/cpr.h>
#include "Data.h"

namespace SL {
    class DepartureFetcher {
    private:
        static constexpr char REQUEST_URL[] = "https://transport.integration.sl.se/v1/sites/{}/departures";

        cpr::Session session;
        static cpr::Session create_session();
        static std::tuple<std::vector<Departure>,std::vector<std::string>> response_to_dep_list(std::string& response);
        static std::string mode_to_string(Mode m);
        static std::string assemble_url(int site_id);
    public:
        DepartureFetcher();
        StopStatus fetch_departures(int siteId, std::optional<int> direction, std::optional<Mode> mode);
    };
}
#endif //DEPARTUREFETCHER_H
