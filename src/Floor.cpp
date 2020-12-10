#include <thread>
#include <chrono>
#include <iostream>

#include "Floor.h"

using namespace std;


void Floor::operator()() {
    while (1) {


        Message message{this->message_queue->pop(this->name + to_string(this->id))};
        cout << this->name + to_string(this->id) + ": " <<  message.get_command() << endl;
        cout << ">>> " << flush;
        
        Message send{"Coordinator", message.get_command(), message.get_floor(), message.get_elevator_id()};
        this->message_queue->push(send);
    }
}