
#ifndef WIRINGPI_SLDISPLAY_H
#define WIRINGPI_SLDISPLAY_H

#include <U8g2lib.h>
#include <sl/DepartureFetcher.h>
#include <chrono>
#include <future>
#include <string_view>
#include <utility>
#include <sl/Data.h>
#include <sl/DepartureFetcher.h>

class SLDisplay {
public:
    class Config {
    public:
        explicit Config(int site_id,
                        std::optional<int> direction_code = std::nullopt,
                        std::optional<SL::Mode> mode = std::nullopt,
                        std::optional<std::tuple<std::string, std::string>> sleep_times = std::nullopt,
                        int update_seconds = DEFAULT_UPDATE_SECONDS) : site_id(site_id), direction_code(direction_code),
                                                                       mode(mode), update_seconds(update_seconds),
                                                                       sleep_times(std::move(sleep_times)) {

        }

        static constexpr int DEFAULT_UPDATE_SECONDS = 60;

        int site_id;
        std::optional<std::tuple<std::string, std::string>> sleep_times;
        std::optional<int> direction_code;
        std::optional<SL::Mode> mode;
        int update_seconds;
    };

    SLDisplay(U8G2 &display, SLDisplay::Config &config);

    void main_loop();

private:
    SL::DepartureFetcher departure_fetcher;
    U8G2 &display;

    static constexpr int scroll_gap_px = 40;
    static constexpr int fps = 24;

    std::optional<std::tuple<std::string, std::string>> sleep_times;
    int site_id;
    int update_seconds;
    std::optional<int> direction;
    std::optional<SL::Mode> mode;

    SL::StopStatus stop_status;
    // Used when updating
    std::future<SL::StopStatus> new_stop_status;
    std::chrono::time_point<std::chrono::steady_clock> last_update;
    std::atomic_bool update_dispatched;

    void draw_departure(SL::Departure &departure, int x_pos, int y_pos);

    void update_data();

    inline int get_scroll_start_position(int position);

    static std::string line_and_dest_string(SL::Departure &departure);

    int get_total_scroll_size(int n_departures, std::vector<std::string> &deviations);

    bool should_sleep();

    void draw_deviations(int start_x, std::vector<std::string> &deviations);
};

#endif //WIRINGPI_SLDISPLAY_H
