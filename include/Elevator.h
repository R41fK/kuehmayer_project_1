#pragma once

#include "MessageQueue.h"
#include "Message.h"

class Elevator
{
private:
    std::string name{"Elevator"};
    unsigned int id{};
    unsigned int current_floor{0};
    float travel_time{3.0};
    MessageQueue* message_queue = new MessageQueue();
    MessageQueue* coordinator_queue;
public:
    Elevator(unsigned int id, float travel_time, MessageQueue* coordinator_queue): 
    id{id}, 
    travel_time{travel_time},
    coordinator_queue{coordinator_queue}
    {};

    void move_to(unsigned int floor);
    unsigned int get_current_floor();
    void operator()();
    void push(Message message);

    ~Elevator(){
        delete message_queue;
    }
};