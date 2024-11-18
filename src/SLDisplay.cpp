#include "SLDisplay.h"
#include <sl/Data.h>
#include <chrono>
#include <ctime>
#include <thread>
#include "esseltub.h"

SLDisplay::SLDisplay(U8G2 &display, SLDisplay::Config &config) :
        config(config),
        display(display) {
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

    int px_per_frame = scroll_px_per_frame();
    int total_scroll_size = 0;
    int x = 0;
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
                continue;
            }
            auto time = std::chrono::local_seconds();
            fetch_update();
        }

        // Check if an update is ready to be consumed
        if (update_dispatched && new_stop_status.wait_for(std::chrono::seconds::zero()) == std::future_status::ready) {
            consume_update();
            // Reset the screen
            total_scroll_size = get_total_scroll_size(stop_status.deviations);
            x = 0;
        }

        // Prevents artifacts
        display.clearBuffer();
        if (!stop_status.departures.empty()) {
            auto top_departure = stop_status.departures.front();
            // Render top departure
            draw_departure(top_departure, 0, y_pos_top);
            // Render scrolling text, remove a width so we start offscreen
            // Remove a width from x, so the text starts off-screen to the right
            draw_scroll(x - display.getWidth());
        }
        // Add display_width so the text scrolls off the screen
        // x varies from 0 to total_scroll_size + display width to allow the text to scroll of the screen
        x = (x + px_per_frame) % (total_scroll_size + display.getWidth());
        display.updateDisplay();

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
    stop_status = new_stop_status.get();
    update_dispatched = false;
    last_update = std::chrono::steady_clock::now();
}

void SLDisplay::draw_scroll(int offset) {
    // Start drawing the departures
    int drawn_px = 0;
    int x_pos = -offset;
    // Keep drawing until we have drawn on the whole screen
    // If we are about to draw a departrue
    for (int i = 1; i < stop_status.departures.size() && x_pos < display.getWidth(); i++) {
        auto cur_dep = stop_status.departures.at(i);
        drawn_px += draw_scroll_departure(cur_dep, x_pos);
        // Update the x position
        x_pos = drawn_px - offset;
    }
    if (x_pos < display.getWidth())
        draw_deviations(x_pos, stop_status.deviations);
}

int SLDisplay::draw_deviations(int start_x, std::vector<std::string> &deviations) {
    auto curr_pos = start_x;
    for (const auto &deviation: deviations){
        auto text_width = display.drawUTF8(curr_pos, y_pos_scroll, deviation.c_str());
        // Add a gap after subsequent deviations
        curr_pos += text_width + scroll_gap_px;
    }
    return curr_pos - start_x;
}

int SLDisplay::get_scroll_start_position(int index) {
    // Add 1 width so they start off-screen
    return index * (display.getWidth() + scroll_gap_px) + display.getWidth();
}

int SLDisplay::get_total_scroll_size(std::vector<std::string> &deviations) {
    int size = 0;
    int space_width = display.getUTF8Width(" ");
    auto n_departures = stop_status.departures.size() - 1;
    if (n_departures > 0) {
        // Add the static parts of each departure: 2 spaces between line and destination, 2 between destination and
        // time and 5 after the departure
        size += n_departures * space_width * 9;
        // Add the dynamic parts of each departure
        for (auto dep = stop_status.departures.begin() + 1; dep != stop_status.departures.end(); ++dep) {
            size += display.getUTF8Width(dep->line.c_str());
            size += display.getUTF8Width(dep->destination.c_str());
            size += display.getUTF8Width(dep->arrival.c_str());
        }
    }
    for (const auto &deviation: deviations) {
        size += scroll_gap_px + display.getStrWidth(deviation.c_str());
    }
    return size;
}

std::string SLDisplay::line_and_dest_string(SL::Departure &departure) {
    auto str = std::string(departure.line);
    str += " ";
    str.append(departure.destination);
    return str;
}

void SLDisplay::draw_departure(SL::Departure &departure, int x_pos, int y_pos) const {
    auto line_and_dest = line_and_dest_string(departure);
    display.drawUTF8(x_pos, y_pos, line_and_dest.c_str());
    display.drawUTF8(x_pos + display.getWidth() - display.getStrWidth(departure.arrival.c_str()),
                     y_pos, departure.arrival.c_str());
}

int SLDisplay::draw_scroll_departure(SL::Departure &departure, int x_pos) const {
    int drawn_px = 0;
    auto string = std::stringstream();
    string << departure.line << "  " << departure.destination << "  " << departure.arrival << "     ";
    drawn_px += display.drawUTF8(x_pos, y_pos_scroll, string.str().c_str());
    return drawn_px;
}

void SLDisplay::fetch_update() {
    new_stop_status =
            std::async(std::launch::async,
                       [this]() {
                           return this->departure_fetcher.fetch_departures(this->config.site_id,
                                                                           this->config.direction_code,
                                                                           this->config.mode);
                       });
    update_dispatched = true;
}

bool SLDisplay::should_sleep() {
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

