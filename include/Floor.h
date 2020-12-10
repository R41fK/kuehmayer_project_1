#pragma once

#include "MessageQueue.h"

class Floor
{
private:
    std::string name{"Floor"};
    unsigned int id{};
    MessageQueue* message_queue;
public:
    Floor(unsigned int id, MessageQueue* message_queue): id{id}, message_queue(message_queue){};
    void operator()();
};
