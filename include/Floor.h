#pragma once

#include <memory>

#include "spdlog/sinks/basic_file_sink.h"
#include "MessageQueue.h"
#include "Message.h"

class Floor
{
private:
    std::string name{"Floor"};
    unsigned int id{};
    MessageQueue* message_queue;
    MessageQueue* coordinator_queue;
    bool log_to_file{false};
    std::shared_ptr<spdlog::logger> file_logger;

public:
    Floor(unsigned int id, MessageQueue* coordinator_queue, std::shared_ptr<spdlog::logger> file_logger, bool log_to_file): 
    id{id},
    coordinator_queue{coordinator_queue},
    log_to_file{log_to_file},
    file_logger{file_logger}
    {
        message_queue = new MessageQueue();
    };

    // a method that simulates a button clicked in a floor
    void operator()();

    // push a message in the queue of the floor
    void push(Message message);
};
