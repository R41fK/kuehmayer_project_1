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
    std::shared_ptr<spdlog::logger> file_logger;
    float sim_time{};

    // create a message for the command move and push it to the queue
    void move(std::string floor_number, std::string elevator_number, bool override);

    // create a message for the command call and push it to the queue
    void call(std::string number, bool override);
    
public:
    Simulation(std::vector<Floor>& floors, std::vector<Elevator>& elevators, bool override, std::shared_ptr<spdlog::logger> file_logger, float sim_time): 
    floors{floors},
    elevators{elevators},
    override{override},
    file_logger{file_logger},
    sim_time{sim_time}
    {};

    // a method that starts the simulation, should be started in its own thread
    void operator()();
};

