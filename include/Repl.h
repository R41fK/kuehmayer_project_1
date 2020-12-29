#pragma once

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

    void move(std::string floor_number, std::string elevator_number);
    void call(std::string number);
    void show_help();

public:
    Repl(std::vector<Floor>& floors, std::vector<Elevator>& elevators, unsigned int floor_number, unsigned int elevator_number): 
    floors{floors},
    elevators{elevators},
    floor_number{floor_number},
    elevator_number{elevator_number}
    {};

    void operator()();
};