#include <iostream>
#include <nlohmann/json.hpp>
#include <cpr/cpr.h>

#include "DepartureFetcher.h"

using json = nlohmann::json;

struct SL::DepartureFetcher::impl{
    cpr::Session session;
};

SL::DepartureFetcher::DepartureFetcher(): pimpl(std::make_unique<impl>()) { // pimpl must be manually initialised
    setup_session();
}

SL::DepartureFetcher::~DepartureFetcher() = default;

void SL::DepartureFetcher::setup_session() {
    pimpl->session.SetHeader(cpr::Header{{"Content-Type", "application/json"}});
}

SL::StopStatus
SL::DepartureFetcher::fetch_departures(int siteId, std::optional<int> direction, std::optional<Mode> mode) {
//    auto urlStr = std::format(DepartureFetcher::REQUEST_URL, std::to_string(siteId));
    auto urlStr = assemble_url(siteId);
    auto params = cpr::Parameters();
    if(mode.has_value()){
        auto mode_str = SL::mode_to_string(mode.value());
        params.Add(cpr::Parameter("transport", mode_str));
    }
    if(direction.has_value()){
        auto dir_str = std::to_string(direction.value());
        params.Add(cpr::Parameter("direction", dir_str));
    }
    auto url = cpr::Url(urlStr);
    pimpl->session.SetUrl(url);
    pimpl->session.SetParameters(params);
    auto r = pimpl->session.Get();
    if(r.status_code != cpr::status::HTTP_OK){
        std::cerr << "Error: " <<  r.status_code << " " << r.error.message;
        return StopStatus({}, {});
    }
    auto [departures, disruptions] = response_to_dep_list(r.text);
    return StopStatus(departures, disruptions);
}

std::vector<std::string> get_disruptions(nlohmann::json& response){
    auto disruptions = response["stop_deviations"];
    auto messages = std::vector<std::string>{};
    for(const auto& disruption: disruptions){
        messages.push_back(disruption["message"]);
    }
    return messages;
}

std::tuple<std::vector<SL::Departure>,std::vector<std::string>> SL::DepartureFetcher::response_to_dep_list(std::string &response) {
    auto departures = std::vector<Departure>{};
    auto resp_json = json::parse(response);
    for (auto dep: resp_json["departures"]) {
        if (dep["state"] == "CANCELLED") {
            continue;
        }
        auto dest = dep["destination"];
        auto arrival = dep["display"];
        auto line = dep["line"]["designation"];
        departures.emplace_back(line, dest, arrival);
    }
    return std::make_tuple(departures, get_disruptions(resp_json));
}

std::string SL::DepartureFetcher::assemble_url(int site_id) {
    auto url = std::string ("https://transport.integration.sl.se/v1/sites/");
    url.append(std::to_string(site_id));
    url.append("/departures");
    return url;
}
