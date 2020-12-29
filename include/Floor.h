#pragma once

#include "MessageQueue.h"
#include "Message.h"

class Floor
{
private:
    std::string name{"Floor"};
    unsigned int id{};
    MessageQueue* message_queue = new MessageQueue();
    MessageQueue* coordinator_queue;
public:
    Floor(unsigned int id, MessageQueue* coordinator_queue): 
    id{id},
    coordinator_queue{coordinator_queue}
    {};

    void operator()();

    void push(Message message);

    ~Floor(){
        delete message_queue;
    }
};
