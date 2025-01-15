#ifndef WIRINGPI_SLDISPLAY_H
#define WIRINGPI_SLDISPLAY_H

#include <U8g2lib.h>
#include <sl/DepartureFetcher.h>
#include <chrono>
#include <future>
#include <utility>
#include <sl/Data.h>
#include <cmath>
#include <iostream>
#include <ranges>

/**
 * SL Departure board with a u8g2 display.
 */
class SLDisplay {
public:
    /**
     * Configuration parameters for the SLDisplay class.
     */
    class Config {
    public:
        /**
         * All parameters besides site_id are optional.
         * @param site_id ID of the stop for which to fetch departures. A full list can be found at
         * <a href="https://transport.integration.sl.se/v1/sites">https://transport.integration.sl.se/v1/sites</a>
         * @param direction_code Filter departures depending on code, 1 and 2 correspond to back and forth.
         * For more detail see API documentation at
         * <a href="https://www.trafiklab.se/api/trafiklab-apis/sl/transport/#/default/Departures">
         * https://www.trafiklab.se/api/trafiklab-apis/sl/transport/#/default/Departures</a>
         * @param mode Keep only departures for the specified mode.
         * @param sleep_times Times when the display is turned off.
         * Tuple of time strings in the format XX:XX. First element is starting time and second is ending time.
         * @param update_seconds Seconds between updates of data
         */
        explicit Config(int site_id,
                        std::optional<int> direction_code = std::nullopt,
                        std::optional<SL::Mode> mode = std::nullopt,
                        std::optional<std::tuple<std::string, std::string> > sleep_times = std::nullopt,
                        int update_seconds = DEFAULT_UPDATE_SECONDS) : site_id(site_id), direction_code(direction_code),
                                                                       mode(mode), update_seconds(update_seconds),
                                                                       sleep_times(std::move(sleep_times)) {
        }

        static constexpr int DEFAULT_UPDATE_SECONDS = 60; /**< Default number of seconds between updates. */

        const int site_id;
        const std::optional<std::tuple<std::string, std::string> > sleep_times;
        const std::optional<int> direction_code;
        const std::optional<SL::Mode> mode;
        const int update_seconds;
    };

    /**
     *
     * @param display u8g2 object for the display. It must be already initialised by calling begin.
     * @param config For detailed information see SLDisplay::Config documentation.
     */
    SLDisplay(U8G2 &display, Config config);

    ~SLDisplay() noexcept;

    /**
     * Starts displaying information on the display and returns execution to the caller.
     */
    void start();

    /**
     * Stops displaying information on the display.
     */
    void stop();

private:
    SL::DepartureFetcher departure_fetcher;
    U8G2 &display;
    Config config;
    std::thread loop_thread;

    void loop();


    static constexpr int scroll_gap_px = 12; /**< Pixels between consecutive entries at the scrolling list. */
    static constexpr int y_pos_top = 20; /**< Bottom right corner y position for top departure */
    static constexpr int y_pos_scroll = 40; /**< Bottom right corner y position for scrolling text */
    static constexpr int fps = 30;
    using frame_duration = std::chrono::duration<int64_t, std::ratio<1, fps> >;

    /** Time it takes from a pixel to scroll from one side of the screen to the other.
    * About 2.4 seconds in SL displays. */
    static constexpr auto scroll_duration = std::chrono::milliseconds(2400);

    /**
      * Calculate the number of pixels the display should scroll each frame.
      * @return number of pixels per frame, rounded down to nearest integer
      */
    [[nodiscard]] int calc_scroll_px_per_frame() const {
        using namespace std::chrono;
        // Express duration as float number of seconds
        using f_seconds = duration<float>;
        // px/f = width_px/scroll_s * s/frame
        const auto pixels_per_second =
                static_cast<float>(display.getWidth()) / duration_cast<f_seconds>(scroll_duration).count();
        std::cout << pixels_per_second << '\n';
        constexpr auto seconds_per_frame = duration_cast<f_seconds>(frame_duration(1)).count();
        // Round down, better for the sign to be slower than faster
        return std::floor(pixels_per_second * seconds_per_frame);
    };

    int scroll_px_per_frame; /**< Number of pixels to scroll to the left per frame */

    SL::StopStatus stop_status; /**< The information that is currently displayed. */
    std::future<SL::StopStatus> new_stop_status; /**< Data fetched from an update */
    /***
     * Indicates when the current data was updated. This should only be updated when the new data has been inserted.
     */
    std::chrono::time_point<std::chrono::steady_clock> last_update;
    /**
     * Indicates that an update is being executed. Used to prevent updating multiple times, since last_update
     * is only updated after the data has been inserted.
     */
    std::atomic_bool update_dispatched;

    /**
     * Indicates whether the display is currently operating. Used to communicate with the looping thread.
     */
    std::atomic_bool running = false;

    // Screen status information
    class {
    public:
        /** How many pixels to the left the text is shifted to the left, compared to the right edge of the screen.
         * When offset is 0, the text should be rendered at pixel screen_width.
        */
        int scroll_offset = 0;

        void set_width(int departures_width, int deviations_width) {
            this->p_departures_width = departures_width;
            this->p_deviations_width = deviations_width;
            this->p_total_scroll_width = this->p_departures_width + this->p_deviations_width;
        }

        [[nodiscard]] int total_scroll_width() const {
            return p_total_scroll_width;
        }

        [[nodiscard]] int departures_width() const {
            return p_departures_width;
        }

        [[nodiscard]] int deviations_width() const {
            return p_deviations_width;
        }

    private:
        int p_total_scroll_width = 0; /**< Width in pixels of all text that is displayed on the scroll */
        int p_departures_width = 0; /**< Width in pixels of departures */
        int p_deviations_width = 0; /**< Width in pixels of deviations */
        //        std::optional<std::size_t> current_deviation_idx; /**< Index of the deviation currently displayed */
    } screen_status;

    /**
     * Draw the given departure at the given position.
     * When a departure is centered on the display, line number and destination are displayed on the left side and the
     * expected arrival time on the right.
     *
     */
    void draw_top_departure() const;

    /**
     * Obtains updated data from the internet.
     */
    void dispatch_update();

    /**
    * Get the start x coordinate for the departures in the scroll.
     */
    [[nodiscard]] int scroll_start_x_departures() const;

    /**
    * Get the start x coordinate for the deviations in the scroll.
     */
    [[nodiscard]] int scroll_start_x_deviations() const;

    /**
     * Calculates the total size of the scrolling list.
     *
     * @return Pixels
     */
    void update_total_scroll_size();

    /**
     * Checks if the display should be asleep at the current time.
     */
    bool should_sleep() const;

    /**
     * Draws all the deviation starting at the given position.
     *
     * @return Number of pixels drawn
     */
    int draw_deviations() const;

    /**
     * Updates the screen and the object after information has been fetched from an update.
     */
    void consume_update();

    /**
     * Draws the scrolling portion of the sign
     */
    void draw_scroll();

    /**
     * Draws one departure on the scroll, at the given x position.
     * @param departure Departure object to draw
     * @param x_pos Horizontal position
     * @return Number of pixels drawn
     */
    int draw_scroll_departure(const SL::Departure &departure, int x_pos) const;
};

#endif //WIRINGPI_SLDISPLAY_H
