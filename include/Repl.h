#pragma once

#include "MessageQueue.h"

class Repl
{
private:
    std::string name{};
    MessageQueue* message_queue;
    unsigned int floor_number{};
    unsigned int elevator_number{};

    void move(std::string floor_number, std::string elevator_number);
    void call(std::string number);
    void show_help();

public:
    Repl(MessageQueue* message_queue, unsigned int floor_number, unsigned int elevator_number): 
    message_queue{message_queue},
    floor_number{floor_number},
    elevator_number{elevator_number}
    {};

    void operator()();
};