#include <thread>
#include <chrono>
#include <iostream>

#include "spdlog/spdlog.h"
#include "Floor.h"

using namespace std;


void Floor::operator()() {
    while (1) {


        Message message{this->message_queue->pop(this->name + to_string(this->id))};
        spdlog::info("Called a elevator in floor " + to_string(this->id));
        cout << ">>> " << flush;
        
        Message send{"Coordinator", message.get_command(), message.get_floor(), message.get_elevator_id()};
        this->message_queue->push(send);
    }
}