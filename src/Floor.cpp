#include <thread>
#include <chrono>
#include <iostream>
#include <future>

#include "spdlog/spdlog.h"
#include "Floor.h"

using namespace std;


void Floor::operator()() {
    future<void> send;
    while (1) {

        Message message{this->message_queue->pop()};
        spdlog::info("Called an elevator in floor " + to_string(this->id));

        send = async(launch::async, [&](){
            Message send{"Coordinator", message.get_command(), message.get_floor(), message.get_elevator_id()};
            this->coordinator_queue->push(send);
        });
    }
}

void Floor::push(Message m){
    this->message_queue->push(m);
}