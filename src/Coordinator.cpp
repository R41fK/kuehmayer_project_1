#include <iostream>


#include "spdlog/spdlog.h"
#include "Coordinator.h"

using namespace std;


void Coordinator::operator()() {
    while (1) {
        
        Message message{this->message_queue->pop()};
        
        // spdlog::info(this->name + ": " +  message.get_command());

        if (message.get_command() == "move") {
            this->elevators[message.get_elevator_id()-1].move_to(message.get_floor());

        } else if (message.get_command() == "call") {
            this->elevators[message.get_elevator_id()].move_to(message.get_floor());
        }
    }
}