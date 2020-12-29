#include <thread>
#include <chrono>
#include <iostream>
#include <stdlib.h>
#include <future>

#include "spdlog/spdlog.h"
#include "Elevator.h"

using namespace std;


void Elevator::move_to(unsigned int floor) {
    if (floor > this->current_floor) {
        this->next_floors->insert_l(floor);
    } else {
        this->next_floors->insert_u(floor);
    }
}

void Elevator::move(){
    unsigned int next{};

    cout << "started" << endl;

    while (1) {
        if (next == 0) {
            this->moving = false;
            next = this->next_floors->get();
            this->moving = true;

        } else if (this->current_floor == next) {
            spdlog::info("Elevator " + to_string(this->id) + " is now on Floor " + to_string(next));
            this->next_floors->erase(next);
            
            this_thread::sleep_for(chrono::seconds(1));

            if (!this->next_floors->empty()) {
                next = this->next_floors->front();
            } else {
                this->moving = false;
                next = 0;
            }

        } else {

            if (next != this->next_floors->front()) {
                next = this->next_floors->front();
            }

            this_thread::sleep_for(chrono::milliseconds(int(travel_time * 1000)));

            if (this->current_floor > next) {
                this->current_floor--;
            } else {
                this->current_floor++;
            }        
        }
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
    future<void> send;

    future<void> move = async(launch::async, [&](){
        this->move();
    });

    while (1) {
        
        Message message{this->message_queue->pop()};
        
        send = async(launch::async, [&](){
            Message send{"Coordinator", message.get_command(), message.get_floor(), message.get_elevator_id()};
            this->coordinator_queue->push(send);
        });

        spdlog::info("Floor " + to_string( message.get_floor()) + " button pressed in elevator " + to_string(this->id));
    }
}