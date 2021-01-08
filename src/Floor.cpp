#include <thread>
#include <chrono>
#include <iostream>
#include <future>

#include "spdlog/spdlog.h"
#include "Floor.h"

using namespace std;


void Floor::operator()() {
    future<void> send;
    bool running{true};

    while (running) {

        Message message{this->message_queue->pop()};

        if (message.get_command() == "stop") {
            running = false;
        } else {
            spdlog::info("Called an elevator in floor " + to_string(this->id));

            this->file_logger->info("Called an elevator in floor " + to_string(this->id));

            send = async(launch::async, [&](){
                Message send{"Coordinator", message.get_command(), message.get_floor(), message.get_elevator_id()};
            
                this->file_logger->debug("Floor " + to_string(this->id) + " sent the " + message.to_string() + " to the Coordinator");

                this->coordinator_queue->push(send);
            });
        }
    }

    this->file_logger->debug("Stopped Floor " + to_string(this->id));
}


void Floor::push(Message m) {
    this->message_queue->push(m);
}