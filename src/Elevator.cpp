#include <thread>
#include <chrono>
#include <stdlib.h>
#include <future>

#include "spdlog/spdlog.h"
#include "Elevator.h"

using namespace std;


void Elevator::move_to(unsigned int floor) {
    if ((this->next_floors->front() >= floor && this->current_floor <= floor)
     || (this->next_floors->front() <= floor && this->current_floor >= floor)){
        this->file_logger->debug("Elevator " + to_string(this->id) + " added Floor " + to_string(floor) + " as first in its queue");

        this->next_floors->insert_first(floor);
    } else {
        this->file_logger->debug("Elevator " + to_string(this->id) + " added Floor " + to_string(floor) + " to its queue");

        this->next_floors->insert(floor);
    }
}

void Elevator::first(unsigned int floor) {
    this->next_floors->insert_first(floor);    
}

void Elevator::buttons(){
    future<void> send;
    bool running{true};

    while (running) {
        
        Message message{this->message_queue->pop()};

        if (message.get_command() == "stop") {
            running = false;
            this->coordinator_queue->push(message);
        } else {
        
            send = async(launch::async, [&](){
                Message send{"Coordinator", message.get_command(), message.get_floor(), message.get_elevator_id()};
                
                this->file_logger->debug("Elevator " + to_string(this->id) + " sent the " + message.to_string() + " to the Coordinator");

                this->coordinator_queue->push(send);
            });

            spdlog::info("Floor " + to_string( message.get_floor()) + " button pressed in elevator " + to_string(this->id));

            this->file_logger->info("Floor " + to_string( message.get_floor()) + " button pressed in elevator " + to_string(this->id));
        }
    }

    this->file_logger->debug("Stopped Buttons " + to_string(this->id));
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
    unsigned int next{1};
    bool running{true};

    while (running) {
    
        if (this->current_floor == next) {
            spdlog::info("Elevator " + to_string(this->id) + " is now on Floor " + to_string(this->current_floor));

            this->file_logger->info("Elevator " + to_string(this->id) + " is now on Floor " + to_string(this->current_floor));

             if (this->next_floors->empty()) {

                this->file_logger->info("Elevator " + to_string(this->id) + " is not moving any more");
                
                this->moving = false;
            } else {
                this->next_floors->erase(next);

                this->file_logger->debug("Elevator " + to_string(this->id) + " deletes " + to_string(next) + " from his queue");

                if (this->next_floors->empty()) {
                    this->file_logger->info("Elevator " + to_string(this->id) + " is not moving any more");

                    this->moving = false;
                }
            }

            this_thread::sleep_for(chrono::seconds(1));

            next = this->next_floors->get();

            if (next == UINT_MAX) {
                running = false;
            } else {
                this->file_logger->debug("Elevator " + to_string(this->id) + " got next floor " + to_string(next) + " is now moving");

                this->moving = true;
            }
        }

        if (next != this->next_floors->front()) {
            this->file_logger->debug("Elevator " + to_string(this->id) + " got a new next floor " + to_string(this->next_floors->front()) 
                                    + " instead of " + to_string(next));


            next = this->next_floors->get();
            this->moving = true;
            if (next == UINT_MAX) {
                running = false;
            }
        }

        if (next != UINT_MAX) {
            if (this->current_floor > next) {
                this->current_floor--;
                this->file_logger->info("Elevator " + to_string(this->id) + " is mooving one floor down. Next floor he reaches is " 
                                        + to_string(this->current_floor));
                
                this_thread::sleep_for(chrono::milliseconds(int(travel_time * 1000)));
            } else if (this->current_floor < next) {
                this->current_floor++;
                this->file_logger->info("Elevator " + to_string(this->id) + " is mooving one floor up. Next floor he reaches is " 
                                        + to_string(this->current_floor));
                
                this_thread::sleep_for(chrono::milliseconds(int(travel_time * 1000)));
            }      
        }
    }

    this->file_logger->debug("Stopped Elevator " + to_string(this->id));
}