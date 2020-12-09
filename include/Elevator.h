#pragma once

class Elevator
{
private:
    unsigned int id{};
    unsigned int current_floor{0};
    float travel_time{3.0};
public:
    Elevator(unsigned int id, float travel_time): id{id}, travel_time{travel_time} {};
    void move_to(unsigned int floor);
    unsigned int get_current_floor();
};