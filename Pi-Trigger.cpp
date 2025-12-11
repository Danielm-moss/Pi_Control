#include <iostream>
#include <gpiod.hpp>
#include <thread>
#include <chrono>

using namespace std;

int main() {

    const char* CHIP = "/dev/gpiochip4";
    const unsigned BUTTON = 21;   // Input pin - photoelectric sensor on rail
    const unsigned FLASH = 22;    // Output pin - flash trigger (HIGH during exposure)
    const unsigned time[] = {100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};  // Exposure times in microseconds
    const int time_array_size = sizeof(time) / sizeof(time[0]);

    gpiod::chip chip{CHIP};

    gpiod::line_settings button_behavior;
    button_behavior.set_direction(gpiod::line::direction::INPUT);
    button_behavior.set_bias(gpiod::line::bias::PULL_UP);

    gpiod::line_settings flash_behavior;
    flash_behavior.set_direction(gpiod::line::direction::OUTPUT);
    flash_behavior.set_output_value(gpiod::line::value::INACTIVE); // Start LOW

    auto req = chip.prepare_request()
    .add_line_settings(BUTTON, button_behavior)
    .add_line_settings(FLASH, flash_behavior)
    .do_request();

    bool flashing = false; // Debounce flag to prevent multiple triggers
    int current_time = 0; // Index into time array

    while(current_time < time_array_size){
        if(req.get_value(BUTTON) == gpiod::line::value::INACTIVE && !flashing){

            flashing = true;

            // Trigger camera and flash simultaneously
            req.set_value(FLASH, gpiod::line::value::ACTIVE);
            std::this_thread::sleep_for(std::chrono::microseconds(time[current_time]));
            req.set_value(FLASH, gpiod::line::value::INACTIVE);

            flashing = false;
            current_time++;

            std::this_thread::sleep_for(std::chrono::milliseconds(2000)); // delay to avoid rapid retriggering
        }
    }
}

// g++ -o trig trig.cpp -lgpiodcxx -std=c++17
// sudo ./trig
