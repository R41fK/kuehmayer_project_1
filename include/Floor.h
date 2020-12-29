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

    void operator()();

    void push(Message message);

    std::string test{"Floor"};

    // ~Floor(){
    //     delete message_queue;
    // }
};
