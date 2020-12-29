#pragma once

#include <vector>

#include "Message.h"
#include "Elevator.h"
#include "MessageQueue.h"

class Coordinator
{
private:
    std::vector<Elevator>& elevators;
    MessageQueue* message_queue;
    std::string name{"Coordinator"};

    unsigned int closest_elevator_not_mooving(unsigned int floor);
    unsigned int closest_elevator_with_mooving(unsigned int floor);

public:
    Coordinator(std::vector<Elevator>& elevators, MessageQueue* message_queue): 
    elevators{elevators},
    message_queue{message_queue}
    {};

    void operator()();
};
