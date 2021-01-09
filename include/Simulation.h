#pragma once

#include <memory>

#include "spdlog/sinks/basic_file_sink.h"
#include "MessageQueue.h"
#include "Floor.h"
#include "Elevator.h"

class Simulation
{
private:
    std::string name{};
    std::vector<Floor>& floors;
    std::vector<Elevator>& elevators;
    bool override{false};
    bool& running;
    std::shared_ptr<spdlog::logger> file_logger;
    float sim_time{};
    unsigned int floor_number{};
    unsigned int elevator_number{};

    // create a message for the command move and push it to the queue
    void move(unsigned int floor_number, unsigned int elevator_number, bool override);

    // create a message for the command call and push it to the queue
    void call(unsigned int number, bool override);

public:
    Simulation(std::vector<Floor>& floors, std::vector<Elevator>& elevators, unsigned int floor_number
    , unsigned int elevator_number, bool override, std::shared_ptr<spdlog::logger> file_logger, float sim_time, bool& running): 
    floors{floors},
    elevators{elevators},
    override{override},
    running{running},
    file_logger{file_logger},
    sim_time{sim_time},
    floor_number{floor_number},
    elevator_number{elevator_number}
    {};

    // a method that starts the simulation, should be started in its own thread
    void operator()();
};

