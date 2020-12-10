#include <thread>
#include <chrono>
#include <iostream>

#include "spdlog/spdlog.h"
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

void Elevator::operator()(){

    while (1) {
        
        Message message{this->message_queue->pop(this->name + to_string(this->id))};
        
        spdlog::info("Floor " + to_string( message.get_floor()) + " button pressed in elevator " + to_string(this->id));
        cout << ">>> " << flush;

        Message send{"Coordinator", message.get_command(), message.get_floor(), message.get_elevator_id()};
        this->message_queue->push(send);
    }
}