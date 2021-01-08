#include <thread>
#include <chrono>
#include <random>

#include "spdlog/spdlog.h"
#include "Simulation.h"

using namespace std;


void Simulation::call(string number, bool override) {
        
    if (override) {
        this->file_logger->debug("Simulation input override call " + number);

        Message message{"Floor" + number, "override", unsigned (stoi(number)), 0};
        this->floors[stoi(number) - 1].push(message);
    } else {
        this->file_logger->debug("Simulation input call " + number);

        Message message{"Floor" + number, "call", unsigned (stoi(number)), 0};
        this->floors[stoi(number) - 1].push(message);
    }
}


void Simulation::move(string floor_number, string elevator_number, bool override) {
    if (override) {
        this->file_logger->debug("Simulation input override move " + elevator_number + " " + floor_number);

        Message message{"Elevator" + elevator_number, "override", unsigned (stoi(floor_number)), unsigned (stoi(elevator_number))};
        this->elevators[stoi(elevator_number) - 1].push(message);
    } else {
        this->file_logger->debug("Simulation input move " + elevator_number + " " + floor_number);

        Message message{"Elevator" + elevator_number, "move", unsigned (stoi(floor_number)), unsigned (stoi(elevator_number))};
        this->elevators[stoi(elevator_number) - 1].push(message);
    }
}

void Simulation::operator()() {
    random_device rd;
    mt19937 gen{rd()};
    uniform_real_distribution<> rand_elevator{1, double(this->elevators.size())};
    uniform_real_distribution<> rand_floor{1, double(this->floors.size())};
    uniform_real_distribution<> rand_override{0, 5};
    uniform_real_distribution<> rand_command{0, 10};
    bool override{false};

    while (1) {
        if (int(rand_override(gen)) == 2 && this->override) {
            override = true;
        }

        if (int(rand_command(gen)) % 2 == 0) {
            move(to_string(int(rand_floor(gen))), to_string(int(rand_elevator(gen))), override);
        } else {
            call(to_string(int(rand_floor(gen))), override);
        }

        override = false;
        this_thread::sleep_for(chrono::milliseconds(int(this->sim_time * 1000)));
    }
}