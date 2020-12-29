#include <iostream>
#include "future"

#include "spdlog/spdlog.h"
#include "Coordinator.h"
#include "Elevator.h"

using namespace std;


unsigned int closest_elevator_not_mooving(unsigned int floor, vector<Elevator> elevators){
    unsigned int diff_closest_floor{UINT_MAX};
    unsigned int closest_elevator{UINT_MAX};
    unsigned int diff{};
    
    for (unsigned int i{0}; i < elevators.size(); i++) {
        diff = max(elevators[i].get_current_floor(), floor) - min(elevators[i].get_current_floor(), floor);
        if (diff < diff_closest_floor && !elevators[i].is_moving()) {
            diff_closest_floor = diff;
            closest_elevator = i;
            
            if(diff_closest_floor == 0){
                return closest_elevator;
            }
        }
    }
    return closest_elevator;
}


unsigned int closest_elevator_with_mooving(unsigned int floor, vector<Elevator> elevators){
    unsigned int diff_closest_floor{UINT_MAX};
    unsigned int closest_elevator{UINT_MAX};
    unsigned int diff{};
    
    for (unsigned int i{0}; i < elevators.size(); i++) {
        diff = max(elevators[i].get_current_floor(), floor) - min(elevators[i].get_current_floor(), floor);
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
                future<unsigned int> closest_not_mooving = async(launch::async, closest_elevator_not_mooving
                                                            , message.get_floor(), this->elevators);

                future<unsigned int> closest_with_mooving = async(launch::async, closest_elevator_with_mooving
                                                            , message.get_floor(), this->elevators);

                closest = closest_not_mooving.get();

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