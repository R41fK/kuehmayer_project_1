#pragma once

#include "MessageQueue.h"

class Elevator
{
private:
    std::string name{"Elevator"};
    unsigned int id{};
    unsigned int current_floor{0};
    float travel_time{3.0};
    MessageQueue* message_queue;
public:
    Elevator(unsigned int id, float travel_time, MessageQueue* message_queue): 
    id{id}, 
    travel_time{travel_time},
    message_queue{message_queue} 
    {};

    void move_to(unsigned int floor);
    unsigned int get_current_floor();
    void operator()();
};