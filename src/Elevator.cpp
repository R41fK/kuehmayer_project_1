#include <thread>
#include <chrono>
#include <iostream>
#include <stdlib.h>
#include <future>

#include "spdlog/spdlog.h"
#include "Elevator.h"

using namespace std;


void Elevator::move_to(unsigned int floor) {
    this->mooving = true;
    int floor_difference{abs((int)this->current_floor-(int)floor)};

    float travel_time{floor_difference * this->travel_time};


    this_thread::sleep_for(chrono::milliseconds(int(travel_time * 1000)));
    this->current_floor = floor;

    spdlog::info("Elevator " + to_string(this->id) + " is now on Floor " + to_string(floor));
    this->mooving = false;
}

unsigned int Elevator::get_current_floor(){ 
    return this->current_floor;
}

bool Elevator::is_mooving() {
    return this->mooving;
}

void Elevator::push(Message m) {
    this->message_queue->push(m);
}

void Elevator::operator()() {
    future<void> send;
    while (1) {
        
        Message message{this->message_queue->pop()};
        
        send = async(launch::async, [&](){
            Message send{"Coordinator", message.get_command(), message.get_floor(), message.get_elevator_id()};
            this->coordinator_queue->push(send);
        });

        spdlog::info("Floor " + to_string( message.get_floor()) + " button pressed in elevator " + to_string(this->id));
    }
}