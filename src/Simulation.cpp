#include <thread>
#include <chrono>
#include <random>

#include "spdlog/spdlog.h"
#include "Simulation.h"

using namespace std;


void Simulation::call(unsigned int number, bool override) {
        
    if (override) {
        this->file_logger->debug("Simulation input override call " + to_string(number));

        Message message{"Floor" + to_string(number), "override", number, 0};
        this->floors[number - 1].push(message);
    } else {
        this->file_logger->debug("Simulation input call " + to_string(number));

        Message message{"Floor" + to_string(number), "call", number, 0};
        this->floors[number - 1].push(message);
    }
}


void Simulation::move(unsigned int floor_number, unsigned int elevator_number, bool override) {
    if (override) {
        this->file_logger->debug("Simulation input override move " + to_string(elevator_number) + " " + to_string(floor_number));

        Message message{"Elevator" + elevator_number, "override",  (floor_number), elevator_number};
        this->elevators[elevator_number - 1].push(message);
    } else {
        this->file_logger->debug("Simulation input move " + to_string(elevator_number) + " " + to_string(floor_number));

        Message message{"Elevator" + to_string(elevator_number), "move", floor_number, elevator_number};
        this->elevators[elevator_number - 1].push(message);
    }
}

void Simulation::operator()() {
    random_device rd;
    mt19937 gen{rd()};
    uniform_int_distribution<uint> rand_elevator{1, this->elevator_number};
    uniform_int_distribution<uint> rand_floor{1, this->floor_number};
    uniform_int_distribution<> rand_override{0, 5};
    uniform_int_distribution<> rand_command{1, 10};
    bool override{false};

    while (this->running) {
        if (rand_override(gen) == 2 && this->override) {
            override = true;
        }

        if (rand_command(gen) % 2 == 0) {
            move(rand_floor(gen), rand_elevator(gen), override);
        } else {
            call(rand_floor(gen), override);
        }

        override = false;
        this_thread::sleep_for(chrono::milliseconds(int(this->sim_time * 1000)));
    }

    this->file_logger->debug("Stopped Simulation");
}