
#ifndef DEPARTUREFETCHER_H
#define DEPARTUREFETCHER_H

#include <string>
#include <optional>
#include <memory>
#include "Data.h"

namespace SL {
    /**
     * Provides methods for fetching departure data from the SL API
     */
    class DepartureFetcher {
    private:
        static constexpr char REQUEST_URL[] = "https://transport.integration.sl.se/v1/sites/{}/departures";

        struct impl;
        std::unique_ptr<impl> pimpl;

        /**
         * Set the necessary parameters in the session object.
         */
        void setup_session();
        /**
         * Convert json
         * @param response JSON response as string
         * @return Tuple with vector of Departures and vector of deviation messages.
         */
        static std::tuple<std::vector<Departure>,std::vector<std::string>> response_to_dep_list(std::string& response);

        /**
         * Create URL for fetching departures from the given \p site_id.
         */
        static std::string assemble_url(int site_id);
    public:
        DepartureFetcher();

        /**
         * Get the latest information about the given stop and optionally filter.
         * @param siteId ID of the stop for which to fetch departures. A full list can be found at
         * <a href="https://transport.integration.sl.se/v1/sites">https://transport.integration.sl.se/v1/sites</a>
         * @param direction Filter departures depending on code, 1 and 2 correspond to back and forth.
         * For more detail see API documentation at
         * <a href="https://www.trafiklab.se/api/trafiklab-apis/sl/transport/#/default/Departures">
         * https://www.trafiklab.se/api/trafiklab-apis/sl/transport/#/default/Departures</a>
         * @param mode Keep only departures for the specified mode.
         */
        StopStatus fetch_departures(int siteId, std::optional<int> direction, std::optional<Mode> mode);

        // https://chrizog.com/cpp-pimpl-unique-ptr-incomplete-types-default-constructor
        // Required to use pimpl with unique_ptr
        ~DepartureFetcher();
    };
}
#endif //DEPARTUREFETCHER_H
