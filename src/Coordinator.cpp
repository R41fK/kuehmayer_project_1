#include <iostream>

#include "Coordinator.h"

using namespace std;


void Coordinator::operator()() {
    while (1) {
        
        Message message{this->message_queue->pop(this->name)};
        
        cout << this->name + ": " <<  message.get_command() << endl;
        cout << ">>> " << flush;

        
    }
}