#pragma once

#include <mutex>
#include <condition_variable>
#include <vector>
#include <memory>

#include "spdlog/sinks/basic_file_sink.h"

class NextFloor_Queue
{
private:

    std::vector<unsigned int> next_floors{};
    std::mutex m{};
    std::condition_variable con_empty{};
    bool log_to_file{false};
    std::shared_ptr<spdlog::logger> file_logger;

public:

    NextFloor_Queue(std::shared_ptr<spdlog::logger> file_logger, bool log_to_file):
    log_to_file{log_to_file},
    file_logger{file_logger}
    {}

    // get the first iteam in the queue, if there is none it return 0
    unsigned int front();

    // get the first iteam in the queue, if there is none it waits until there is one
    unsigned int get();

    // delete a certain floor from the queue
    void erase(unsigned int floor);

    // insert a floor at the front of the queue
    void insert_first(unsigned int floor);

    // insert a floor in the queue, if it is on the way it gets inserted between two floors, if it is not on the way it gets inserted at the back
    void insert(unsigned int floor);

    // returns true if the queue is empty
    bool empty();

};