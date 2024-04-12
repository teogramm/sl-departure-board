
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
#include <cmath>

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

    static constexpr int scroll_gap_px = 25;
    static constexpr int fps = 30;
    using frame_duration = std::chrono::duration<int64_t, std::ratio<1, fps>>;

    /**
     * Time it takes from a pixel to scroll from one side of the screen to the other.
     * About 2.4 seconds in SL displays.
     */
    static constexpr auto scroll_duration = std::chrono::milliseconds(2400);

    /**
      * Calculate the number of pixels the display should scroll each frame.
      * @return number of pixels per frame, rounded down to nearest integer
      */
    inline int scroll_px_per_frame() {
        using namespace std::chrono;
        // Express duration as float number of seconds
        using f_seconds = duration<float>;
        // px/f = width_px/scroll_s * s/frame
        auto pixels_per_second =
                static_cast<float>(display.getWidth()) / duration_cast<f_seconds>(scroll_duration).count();
        auto seconds_per_frame = duration_cast<f_seconds>(frame_duration(1)).count();
        // Round down, better for the sign to be faster than slower
        return std::floor(pixels_per_second * seconds_per_frame);
    };


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
