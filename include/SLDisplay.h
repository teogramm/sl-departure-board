
#ifndef WIRINGPI_SLDISPLAY_H
#define WIRINGPI_SLDISPLAY_H

#include <U8g2lib.h>
#include <sl/DepartureFetcher.h>
#include <chrono>
#include <future>
#include <string_view>
#include <utility>
#include <sl/Data.h>
#include <cmath>
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
                        std::optional<std::tuple<std::string, std::string>> sleep_times = std::nullopt,
                        int update_seconds = DEFAULT_UPDATE_SECONDS) : site_id(site_id), direction_code(direction_code),
                                                                       mode(mode), update_seconds(update_seconds),
                                                                       sleep_times(std::move(sleep_times)) {

        }

        static constexpr int DEFAULT_UPDATE_SECONDS = 60; /**< Default number of seconds between updates. */

        const int site_id;
        const std::optional<std::tuple<std::string, std::string>> sleep_times;
        const std::optional<int> direction_code;
        const std::optional<SL::Mode> mode;
        const int update_seconds;
    };

    /**
     *
     * @param display u8g2 object for the display. It must be already initialised by calling begin.
     * @param config For detailed information see SLDisplay::Config documentation.
     */
    SLDisplay(U8G2 &display, Config &config);

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
    using frame_duration = std::chrono::duration<int64_t, std::ratio<1, fps>>;

     /** Time it takes from a pixel to scroll from one side of the screen to the other.
     * About 2.4 seconds in SL displays. */
    static constexpr auto scroll_duration = std::chrono::milliseconds(2400);

    /**
      * Calculate the number of pixels the display should scroll each frame.
      * @return number of pixels per frame, rounded down to nearest integer
      */
    [[nodiscard]] int scroll_px_per_frame() const {
        using namespace std::chrono;
        // Express duration as float number of seconds
        using f_seconds = duration<float>;
        // px/f = width_px/scroll_s * s/frame
        const auto pixels_per_second =
                static_cast<float>(display.getWidth()) / duration_cast<f_seconds>(scroll_duration).count();
        constexpr auto seconds_per_frame = duration_cast<f_seconds>(frame_duration(1)).count();
        // Round down, better for the sign to be faster than slower
        return std::floor(pixels_per_second * seconds_per_frame);
    };


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

    /**
     * Draw the given departure at the given position.
     * When a departure is centered on the display, line number and destination are displayed on the left side and the
     * expected arrival time on the right.
     *
     * @param departure Departure object to draw on screen
     * @param x_pos Left edge of departure
     * @param y_pos Bottom left corner of departure
     */
    void draw_departure(SL::Departure &departure, int x_pos, int y_pos) const;

    /**
     * Obtains updated data from the internet.
     */
    void fetch_update();

    /**
     * Get the x coordinate for departure at the given index.
     * @param index Position in the scrolling list
     * @return x position where the departure should be drawn. Can be larger than the display's width.
     */
    inline int get_scroll_start_position(int index);

    /**
     * Creates a string with the line and destination of the given departure.
     */
    static std::string line_and_dest_string(SL::Departure &departure);

    /**
     * Calculates the total size of the scrolling list.
     *
     * @param deviations List of deviations which are displayed.
     * @return
     */
    int get_total_scroll_size(std::vector<std::string> &deviations);

    /**
     * Checks if the display should be asleep at the current time.
     */
    bool should_sleep();

    /**
     * Draws all the deviation starting at the given position.
     *
     * The normal gap is left between them.
     * @param start_x Horizontal position
     * @return Number of pixels drawn
     */
    int draw_deviations(int start_x, std::vector<std::string> &deviations);

    /**
     * Updates the object after information has been fetched from an update.
     */
    void consume_update();

    /**
     * Draws the scrolling portion of the sign, shifted to the left by the given number of pixels.
     * @param offset Number of pixels to shit to the left
     */
    void draw_scroll(int offset);

    /**
     * Draws one departure on the scroll, at the given x position.
     * @param departure Departure object to draw
     * @param x_pos Horizontal position
     * @return Number of pixels drawn
     */
    int draw_scroll_departure(SL::Departure &departure, int x_pos) const;
};

#endif //WIRINGPI_SLDISPLAY_H
