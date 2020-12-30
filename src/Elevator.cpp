#include <thread>
#include <chrono>
#include <iostream>
#include <stdlib.h>
#include <future>

#include "spdlog/spdlog.h"
#include "Elevator.h"

using namespace std;


void Elevator::move_to(unsigned int floor) {
    if ((this->next_floors->front() >= floor && this->current_floor <= floor)
     || (this->next_floors->front() <= floor && this->current_floor >= floor)){
        this->next_floors->insert_first(floor);
     } else {
        this->next_floors->insert(floor);
     }
}

void Elevator::buttons(){
    future<void> send;

    while (1) {
        
        Message message{this->message_queue->pop()};
        
        send = async(launch::async, [&](){
            Message send{"Coordinator", message.get_command(), message.get_floor(), message.get_elevator_id()};
            this->coordinator_queue->push(send);
        });

        spdlog::info("Floor " + to_string( message.get_floor()) + " button pressed in elevator " + to_string(this->id) + " it is on Floor " + to_string(this->current_floor));
    }
}


unsigned int Elevator::get_current_floor() { 
    return this->current_floor;
}

bool Elevator::is_moving() {
    return this->moving;
}

void Elevator::push(Message m) {
    this->message_queue->push(m);
}

void Elevator::operator()() {
    unsigned int next{1};

    while (1) {
    
        if (this->current_floor == next) {
            spdlog::info("Elevator " + to_string(this->id) + " is now on Floor " + to_string(this->current_floor));
            
        
            this_thread::sleep_for(chrono::seconds(1));

            if (this->next_floors->empty()) {
                this->moving = false;
            } else {
                this->next_floors->erase(next);
            }
            next = this->next_floors->get();
            this->moving = true;
        }

        if (next != this->next_floors->front()) {
            next = this->next_floors->get();
            this->moving = true;
        }

        if (this->current_floor > next) {
            this->current_floor--;
            this_thread::sleep_for(chrono::milliseconds(int(travel_time * 1000)));
        } else if (this->current_floor < next) {
            this->current_floor++;
            this_thread::sleep_for(chrono::milliseconds(int(travel_time * 1000)));
        }       
    }
}