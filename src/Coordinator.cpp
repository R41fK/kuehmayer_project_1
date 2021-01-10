#include <future>

#include "spdlog/spdlog.h"
#include "Coordinator.h"
#include "Elevator.h"

using namespace std;


unsigned int closest_elevator_not_mooving(unsigned int floor, vector<Elevator>& elevators){
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


unsigned int closest_elevator_with_mooving(unsigned int floor, vector<Elevator>& elevators){
    unsigned int diff_closest_floor{UINT_MAX};
    unsigned int closest_elevator{UINT_MAX};
    unsigned int diff{};

    for (unsigned int i{0}; i < elevators.size(); i++) {
        diff = max(elevators[i].get_current_floor(), floor) - min(elevators[i].get_current_floor(), floor);
        if (diff < diff_closest_floor) {
            diff_closest_floor = diff;
            closest_elevator = i;

            if(diff_closest_floor == 1){
                return closest_elevator;
            }
        }
    }
    return closest_elevator;
}

unsigned int Coordinator::get_closest_elevator(Message message){
    unsigned int closest{};

    future<unsigned int> closest_not_mooving = async(launch::async, closest_elevator_not_mooving, message.get_floor(), ref(this->elevators));

    future<unsigned int> closest_with_mooving = async(launch::async, closest_elevator_with_mooving, message.get_floor(), ref(this->elevators));

    closest = closest_not_mooving.get();

    if (closest >= this->elevators.size()) {
        closest = closest_with_mooving.get();

        this->file_logger->info("Coordinator found no elevator that is not moving. Nearest that is mooving is elevator " + to_string(closest+1));
        
    } else {
        this->file_logger->info("Coordinator found an elevator that is not moving. That elevator is elevator " + to_string(closest+1));
    }

    return closest;
}


void Coordinator::operator()() {
    future<void> send{};
    bool running{true};

    while (running) {
        
        Message message{this->message_queue->pop()};

        if (message.get_command() == "stop") {
            running = false;
            for (Elevator e : this->elevators) {
                e.move_to(UINT_MAX);
            }
        } else {

            this->file_logger->debug("Coordinator got the " + message.to_string());

            //spdlog::info(this->name + ": " +  message.get_command());
            send = async(launch::async, [&](){
                if (message.get_command() == "override") {
                    if (message.get_elevator_id() == 0) {
                        unsigned int elevator_id{this->get_closest_elevator(message)};

                        this->file_logger->debug("Coordinator got Override and tells Elevator " + to_string(elevator_id+1) 
                                                + " to move to Floor " + to_string(message.get_floor())  + " without stopping");

                        this->elevators[elevator_id].first(message.get_floor());
                    } else {
                        this->file_logger->debug("Coordinator got Override and tells Elevator " + to_string(message.get_elevator_id()) 
                                                + " to move to Floor " + to_string(message.get_floor()) + " without stopping");

                        this->elevators[message.get_elevator_id()-1].first(message.get_floor());
                    }

                } else if (message.get_command() == "move") {
                    this->file_logger->debug("Coordinator tells Elevator " + to_string(message.get_elevator_id()) 
                                            + " to move to Floor " + to_string(message.get_floor()));                

                    this->elevators[message.get_elevator_id()-1].move_to(message.get_floor());

                } else if (message.get_command() == "call") {
                    unsigned int elevator_id{this->get_closest_elevator(message)};

                    this->file_logger->debug("Coordinator tells Elevator " + to_string(elevator_id+1) 
                                            + " to move to Floor " + to_string(message.get_floor()));

                    this->elevators[elevator_id].move_to(message.get_floor());
                }
            });
        }
    }

    this->file_logger->debug("Stopped Coordinator");
}