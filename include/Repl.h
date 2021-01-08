#pragma once

#include <memory>

#include "spdlog/sinks/basic_file_sink.h"
#include "MessageQueue.h"
#include "Floor.h"
#include "Elevator.h"

class Repl
{
private:
    std::string name{};
    std::vector<Floor>& floors;
    std::vector<Elevator>& elevators;
    unsigned int floor_number{};
    unsigned int elevator_number{};
    bool override{false};
    bool use_simulation{false};
    bool& running;
    std::shared_ptr<spdlog::logger> file_logger;

    // create a message for the command move and push it to the queue
    void move(std::string floor_number, std::string elevator_number, bool override);

    // create a message for the command call and push it to the queue
    void call(std::string number, bool override);

    // show the commands for the repl
    void show_help();

    // stop all threads
    void stop();

public:
    Repl(std::vector<Floor>& floors, std::vector<Elevator>& elevators, unsigned int floor_number
    , unsigned int elevator_number, bool override, std::shared_ptr<spdlog::logger> file_logger, bool use_simulation, bool& running): 
    floors{floors},
    elevators{elevators},
    floor_number{floor_number},
    elevator_number{elevator_number},
    override{override},
    use_simulation{use_simulation},
    running{running},
    file_logger{file_logger}
    {};

    // a method that starts the repl, should be started in its own thread
    void operator()();
};