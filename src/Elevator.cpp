#include <thread>
#include <chrono>
#include <iostream>

#include "Elevator.h"

using namespace std;


void Elevator::move_to(unsigned int floor){
    float travel_time{(this->current_floor-floor) * this->travel_time};
    this_thread::sleep_for(chrono::milliseconds(int(travel_time * 1000)));
    this->current_floor = floor;
}

unsigned int Elevator::get_current_floor(){
    return this->current_floor;
}