#pragma once

#include <queue>

class Coordinator
{
private:
    std::queue<int> floor_queue{};
public:
    Coordinator(){};
    void operator()();
};
