#pragma once

#include "MessageQueue.h"
#include "Message.h"

class Floor
{
private:
    std::string name{"Floor"};
    unsigned int id{};
    MessageQueue* message_queue;
    MessageQueue* coordinator_queue;
public:
    Floor(unsigned int id, MessageQueue* coordinator_queue): 
    id{id},
    coordinator_queue{coordinator_queue}
    {
        message_queue = new MessageQueue();
    };

    // a method that simulates a button clicked in a floor
    void operator()();

    // push a message in the queue of the floor
    void push(Message message);
};
