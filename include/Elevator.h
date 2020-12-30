#pragma once

#include <mutex>
#include <condition_variable>

#include "MessageQueue.h"
#include "Message.h"
#include "NextFloor_Queue.h"

class Elevator
{
private:
    std::string name{"Elevator"};
    unsigned int id{1};
    unsigned int current_floor{1};
    bool moving{false};
    float travel_time{3.0};
    MessageQueue* message_queue;
    MessageQueue* coordinator_queue;
    NextFloor_Queue* next_floors;

public:
    Elevator(unsigned int id, float travel_time, MessageQueue* coordinator_queue): 
    id{id}, 
    travel_time{travel_time},
    coordinator_queue{coordinator_queue}
    {
        message_queue = new MessageQueue();
        next_floors = new NextFloor_Queue();
    };

    void move_to(unsigned int floor);
    unsigned int get_current_floor();
    bool is_moving();
    void operator()();
    void buttons();
    void push(Message message);

    // ~Elevator(){
    //     delete message_queue;
    // }
};