#pragma once

#include <vector>
#include <memory>

#include "spdlog/sinks/basic_file_sink.h"
#include "Message.h"
#include "Elevator.h"
#include "MessageQueue.h"

class Coordinator
{
private:
    std::vector<Elevator>& elevators;
    MessageQueue* message_queue;
    std::string name{"Coordinator"};
    bool log_to_file{false};
    std::shared_ptr<spdlog::logger> file_logger;

    unsigned int get_closest_elevator(Message message);

public:
    Coordinator(std::vector<Elevator>& elevators, MessageQueue* message_queue, std::shared_ptr<spdlog::logger> file_logger, bool log_to_file): 
    elevators{elevators},
    message_queue{message_queue},
    log_to_file{log_to_file},
    file_logger{file_logger}
    {};

    // a method to start the coordinator, should be started in a own thread
    void operator()();
};
