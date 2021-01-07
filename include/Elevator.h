#pragma once

#include <mutex>
#include <condition_variable>
#include <memory>

#include "spdlog/sinks/basic_file_sink.h"
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
    bool log_to_file{false};
    std::shared_ptr<spdlog::logger> file_logger;

public:
    Elevator(unsigned int id, float travel_time, MessageQueue* coordinator_queue, std::shared_ptr<spdlog::logger> file_logger, bool log_to_file): 
    id{id}, 
    travel_time{travel_time},
    coordinator_queue{coordinator_queue},    
    log_to_file{log_to_file},
    file_logger{file_logger}
    {
        message_queue = new MessageQueue();
        next_floors = new NextFloor_Queue(file_logger, log_to_file);
    };

    // tell the elevator to move to a certain floor. If it is bewtween its own floor and the next floor it initiates to move to this floor next
    void move_to(unsigned int floor);

    // ad floor to which the elevator should move to the front of its queue
    void first(unsigned int floor);

    // returns the current floor
    unsigned int get_current_floor();

    // returns if the elevator is currently moving
    bool is_moving();

    // a method to simulate the movment of the elevator, this method should be started as a thread
    void operator()();

    // a method to simulate the button clicks with should be started as a thread
    void buttons();

    // push a message in the elevators message queue
    void push(Message message);
};