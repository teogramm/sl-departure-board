#include "SLDisplay.h"
#include <sl/Data.h>
#include <chrono>
#include <ctime>
#include <thread>
#include <utility>
#include "esseltub.h"

SLDisplay::SLDisplay(U8G2 &display, SLDisplay::Config config) :
        config(std::move(config)),
        display(display),
        scroll_px_per_frame(calc_scroll_px_per_frame()) {
    display.setFont(esseltub);

    // Make sure we update when we start
    last_update = std::chrono::steady_clock::now() - std::chrono::minutes(5);
}

SLDisplay::~SLDisplay() noexcept {
    // Clear the display and buffer
    this->display.clear();
}

void SLDisplay::start() {
    this->running = true;
    this->loop_thread = std::thread(&SLDisplay::loop, this);
}

void SLDisplay::loop() {
    display.clearBuffer();
    display.clearDisplay();

    auto nextFrame = std::chrono::system_clock::now() + frame_duration{0};

    while (running) {
        // Check if we need to update, ensure we do not have an update already pending
        if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - last_update).count() >
            config.update_seconds && !update_dispatched) {
            // Check if we need to sleep
            if (should_sleep()) {
                display.clearDisplay();
                display.drawUTF8(0, 20, "Sleeping");
                display.updateDisplay();
                display.setPowerSave(true);
                std::this_thread::sleep_for(std::chrono::minutes(5));
                display.setPowerSave(false);
                // Reset when the next frame should be shown, otherwise FPS limiter will stop working until
                // nextFrame catches up with real time again
                nextFrame = std::chrono::system_clock::now() + frame_duration{0};
                continue;
            }
            dispatch_update();
        }

        // Check if an update is ready to be consumed
        if (update_dispatched && new_stop_status.wait_for(std::chrono::seconds::zero()) == std::future_status::ready) {
            consume_update();
        }

        // Prevents artifacts
        display.clearBuffer();
        if (stop_status.departures.size() > 1) {
            // Render scrolling text, remove a width so we start offscreen
            // Remove a width from x, so the text starts off-screen to the right
            draw_scroll();
        }
        // Add display_width so the text scrolls off the screen
        // x varies from 0 to total_scroll_size + display width to allow the text to scroll of the screen
        screen_status.scroll_offset = (screen_status.scroll_offset + scroll_px_per_frame) %
                                      (screen_status.total_scroll_width() + display.getWidth());
        // Reduce CPU usage in half by only updating the scrolling part of the screen
        display.updateDisplayArea(0, 3, display.getBufferTileWidth(), 3);

        nextFrame += frame_duration{1};
        std::this_thread::sleep_until(nextFrame);
    }
}

void SLDisplay::stop() {
    // Notify the thread to stop
    this->running = false;
    // Wait for it to exit
    this->loop_thread.join();
    // Clear the display and buffer
    this->display.clear();
}

void SLDisplay::consume_update() {
    // Populate with new information
    stop_status = new_stop_status.get();
    // Inform that update has been consumed
    update_dispatched = false;
    last_update = std::chrono::steady_clock::now();
    // Reset offset and recalculate the total width
    display.clearDisplay();
    screen_status.scroll_offset = 0;
    update_total_scroll_size();
    // Draw the top departure
    draw_top_departure();
}

void SLDisplay::draw_scroll() {
    if (stop_status.departures.size() > 1) {
        // Start drawing the departures
        int x_pos = scroll_start_x_departures();
        // Keep drawing until we have drawn on the whole screen
        for (int i = 1; i < stop_status.departures.size() && x_pos < display.getWidth(); i++) {
            auto& cur_dep = stop_status.departures.at(i);
            x_pos += draw_scroll_departure(cur_dep, x_pos);
        }
        if (scroll_start_x_deviations() < display.getWidth())
            draw_deviations();
    }
}

int SLDisplay::draw_deviations() const {
    auto curr_pos = scroll_start_x_deviations();
    for (const auto &deviation: stop_status.deviations) {
        int text_width = display.drawUTF8(curr_pos, y_pos_scroll, deviation.c_str());
        // Add a gap after subsequent deviations
        curr_pos += text_width + scroll_gap_px;
    }
    return curr_pos - scroll_start_x_deviations();
}

int SLDisplay::scroll_start_x_departures() const {
    // When offset is 0, the text should be rendered at position displayWidth, so it is completely out of the screen,
    // and then shifted to the left
    return display.getWidth() - screen_status.scroll_offset;
}

int SLDisplay::scroll_start_x_deviations() const {
    // Draw this after the departures list
    return display.getWidth() + screen_status.departures_width() - screen_status.scroll_offset;
}

void SLDisplay::update_total_scroll_size() {
    int size_departures = 0;
    int size_deviations = 0;
    int space_width = display.getUTF8Width(" ");
    auto n_scroll_departures = static_cast<int>(stop_status.departures.size() - 1);
    if (n_scroll_departures > 0) {
        // Add the static parts of each departure: 2 spaces between line and destination, 2 between destination and
        // time and 5 after the departure
        size_departures += n_scroll_departures * space_width * 9;
        // Add the dynamic parts of each departure
        for (auto dep = stop_status.departures.begin() + 1; dep != stop_status.departures.end(); ++dep) {
            size_departures += display.getUTF8Width(dep->line.c_str());
            size_departures += display.getUTF8Width(dep->destination.c_str());
            size_departures += display.getUTF8Width(dep->arrival.c_str());
        }
    }
    for (const auto &deviation: stop_status.deviations) {
        size_deviations += scroll_gap_px + display.getStrWidth(deviation.c_str());
    }
    screen_status.set_width(size_departures, size_deviations);
}

void SLDisplay::draw_top_departure() const {
    if(!stop_status.departures.empty()){
        const auto&[line, destination, arrival] = stop_status.departures.front();
        auto line_and_dest = std::stringstream();
        line_and_dest << line << " " << destination;
        display.drawUTF8(0, y_pos_top, line_and_dest.str().c_str());
        display.drawUTF8(display.getWidth() - display.getStrWidth(arrival.c_str()),
                         y_pos_top, arrival.c_str());
        display.sendBuffer();
    }
}

int SLDisplay::draw_scroll_departure(const SL::Departure &departure, int x_pos) const {
    int drawn_px = 0;
    auto string = std::stringstream();
    string << departure.line << "  " << departure.destination << "  " << departure.arrival << "     ";
    drawn_px += static_cast<int>(display.drawUTF8(x_pos, y_pos_scroll, string.str().c_str()));
    return drawn_px;
}

void SLDisplay::dispatch_update() {
    new_stop_status =
            std::async(std::launch::async,
                       [this]() {
                           return this->departure_fetcher.fetch_departures(this->config.site_id,
                                                                           this->config.direction_code,
                                                                           this->config.mode);
                       });
    update_dispatched = true;
}

bool SLDisplay::should_sleep() const {
    if (config.sleep_times) {
        auto now = time(nullptr);
        auto *local = localtime(&now);
        char buff[sizeof("hh:mm")];
        strftime(buff, sizeof(buff), "%H:%M", local);
        auto now_str = std::string(buff);
        return (now_str >= std::get<0>(config.sleep_times.value())) &&
               (now_str <= std::get<1>(config.sleep_times.value()));
    }
    return false;
}

