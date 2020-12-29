#pragma once

#include <mutex>
#include <condition_variable>
#include <vector>

class NextFloor_Queue
{
private:

    std::vector<unsigned int> next_floors{};
    std::mutex m{};
    std::condition_variable con_empty{};

public:
    
    unsigned int front();
    unsigned int get();
    void erase(unsigned int floor);
    void insert_first(unsigned int floor);
    void insert(unsigned int floor);
    bool empty();

};