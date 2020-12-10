#include <iostream>


#include "spdlog/spdlog.h"
#include "Coordinator.h"

using namespace std;


void Coordinator::operator()() {
    while (1) {
        
        Message message{this->message_queue->pop(this->name)};
        
        spdlog::info(this->name + ": " +  message.get_command());
        cout << ">>> " << flush;

        
    }
}