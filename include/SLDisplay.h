
#ifndef WIRINGPI_SLDISPLAY_H
#define WIRINGPI_SLDISPLAY_H

#include <U8g2lib.h>
#include <sl/DepartureFetcher.h>
#include <chrono>
#include <future>
#include <string_view>

class SLDisplay {

public:
    SLDisplay(U8G2 &display, int site_id, std::optional<int> direction, std::optional<Mode> mode, int gap = 40);
    void main_loop();

private:
    constexpr static std::string_view time_lower_limit = "00:00";
    constexpr static std::string_view time_upper_limit = "06:00";

    DepartureFetcher departure_fetcher;
    int site_id;
    unsigned update_seconds;
    std::optional<int> direction;
    std::optional<Mode> mode;
    U8G2 &display;
    int scroll_gap_px;
    StopStatus stop_status;
    // Used when updating
    std::future<StopStatus> new_stop_status;
    std::chrono::time_point<std::chrono::steady_clock> last_update;
    std::atomic_bool update_dispatched;

    static constexpr int fps = 24;

    void draw_departure(Departure &departure, int x_pos, int y_pos);
    void update_data();
    inline int get_scroll_start_position(int position);

    static std::string line_and_dest_string(Departure &departure);

    int get_total_scroll_size(int n_departures, std::vector<std::string>& deviations);
    bool should_sleep();
    void draw_deviations(int start_x, std::vector<std::string> &deviations);
};

#endif //WIRINGPI_SLDISPLAY_H
