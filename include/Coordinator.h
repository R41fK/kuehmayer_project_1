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

public:
    Coordinator(std::vector<Elevator>& elevators, MessageQueue* message_queue): 
    elevators{elevators},
    message_queue{message_queue}
    {};

    void operator()();
};
