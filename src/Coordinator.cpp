#include <iostream>
#include "future"

#include "spdlog/spdlog.h"
#include "Coordinator.h"
#include "Elevator.h"

using namespace std;


unsigned int Coordinator::closest_elevator_not_mooving(unsigned int floor){
    unsigned int diff_closest_floor{UINT_MAX};
    unsigned int closest_elevator{UINT_MAX};
    unsigned int diff{};
    
    for (unsigned int i{0}; i < elevators.size(); i++) {
        diff = max(this->elevators[i].get_current_floor(), floor) - min(this->elevators[i].get_current_floor(), floor);
        if (diff < diff_closest_floor && !this->elevators[i].is_moving()) {
            diff_closest_floor = diff;
            closest_elevator = i;
            
            if(diff_closest_floor == 0){
                return closest_elevator;
            }
        }
    }
    return closest_elevator;
}


unsigned int Coordinator::closest_elevator_with_mooving(unsigned int floor){
    unsigned int diff_closest_floor{UINT_MAX};
    unsigned int closest_elevator{UINT_MAX};
    unsigned int diff{};
    
    for (unsigned int i{0}; i < this->elevators.size(); i++) {
        diff = max(this->elevators[i].get_current_floor(), floor) - min(this->elevators[i].get_current_floor(), floor);
        if (diff < diff_closest_floor) {
            diff_closest_floor = diff;
            closest_elevator = i;
            
            if(diff_closest_floor == 0){
                return closest_elevator;
            }
        }
    }
    return closest_elevator;
}


void Coordinator::operator()() {
    unsigned int closest{};
    future<void> send{};

    while (1) {
        
        Message message{this->message_queue->pop()};
        
        // spdlog::info(this->name + ": " +  message.get_command());
        send = async(launch::async, [&](){
            if (message.get_command() == "move") {
                this->elevators[message.get_elevator_id()-1].move_to(message.get_floor());

            } else if (message.get_command() == "call") {
                future<unsigned int> closest_not_mooving = async(launch::async, [&](){
                    return this->closest_elevator_not_mooving(message.get_floor());
                });

                future<unsigned int> closest_with_mooving = async(launch::async, [&](){
                    return this->closest_elevator_with_mooving(message.get_floor());
                });

                closest = closest_not_mooving.get();
                cout << this->elevators[closest].test << " T" << endl;

                if (closest < this->elevators.size()) {
                    this->elevators[closest].move_to(message.get_floor());
                } else {
                    closest = closest_with_mooving.get();
                    this->elevators[closest].move_to(message.get_floor());
                }
            }
        });
    }
}