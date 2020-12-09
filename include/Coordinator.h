#pragma once

#include <vector>

#include "Elevator.h"

class Coordinator
{
private:
    std::vector<int> floor_queue{};
    std::vector<Elevator> elevators{};
public:
    Coordinator(std::vector<Elevator> &elevators): elevators{elevators} {};
    void operator()();
};
