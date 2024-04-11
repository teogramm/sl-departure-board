#include <iostream>
#include "sl/DepartureFetcher.h"
#include "Data.h"
#include <optional>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

SL::DepartureFetcher::DepartureFetcher() {
    this->session = create_session();
}

cpr::Session SL::DepartureFetcher::create_session() {
    auto session = cpr::Session();
    session.SetHeader(cpr::Header{{"Content-Type", "application/json"}});
    // TODO: Fix this
    session.SetVerifySsl(false);
    return session;
}

SL::StopStatus
SL::DepartureFetcher::fetch_departures(int siteId, std::optional<int> direction, std::optional<Mode> mode) {
//    auto urlStr = std::format(DepartureFetcher::REQUEST_URL, std::to_string(siteId));
    auto urlStr = assemble_url(siteId);
    auto params = cpr::Parameters();
    if(mode.has_value()){
        auto mode_str = mode_to_string(mode.value());
        params.Add(cpr::Parameter("transport", mode_str));
    }
    if(direction.has_value()){
        auto dir_str = std::to_string(direction.value());
        params.Add(cpr::Parameter("direction", dir_str));
    }
    auto url = cpr::Url(urlStr);
    session.SetUrl(url);
    session.SetParameters(params);
    auto r = session.Get();
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

std::string SL::DepartureFetcher::mode_to_string(Mode m) {
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

std::string SL::DepartureFetcher::assemble_url(int site_id) {
    auto url = std::string ("https://transport.integration.sl.se/v1/sites/");
    url.append(std::to_string(site_id));
    url.append("/departures");
    return url;
}
