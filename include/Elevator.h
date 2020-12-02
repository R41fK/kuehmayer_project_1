#pragma once

class Elevator
{
private:
    unsigned int current_floor{0};
public:
    Elevator(/* args */){};
    void operator()();
};