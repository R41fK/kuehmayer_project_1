
#include <iostream>
#include <thread>

#include "CLI11.hpp"
#include "rang.hpp"
#include "Floor.h"
#include "Coordinator.h"
#include "Elevator.h"
#include "MessageQueue.h"

using namespace std;
using namespace rang;

string validate_float(const string& str) {
    size_t found{str.find_first_not_of("0123456789.")};

    if (found <= str.length()) {
        return str + " is not a float or not positive";
    }

    return "";
}


string validate_int(const string& str) {
    size_t found{str.find_first_not_of("0123456789")};

    if (found <= str.length()) {
        return str + " is not a int or not positive";
    }

    return "";
}


int main(int argc, char* argv[]) {
    
    CLI::App app("elevator_control");

    float travel_time{3.0};
    unsigned int floor_number{3};
    unsigned int number_of_elevators{1};

    app.add_option("-s, --seconds-between-floor_number"
                  , travel_time
                  , "The time it takes to move between two floor_number, that are next to each other"
                  , true)->check(validate_float);

    app.add_option("-f, --floor_number"
                  , floor_number
                  , "Number of floor_number for the elevator"
                  , true)->check(validate_int);

    app.add_option("-e, --elevators"
                   ,number_of_elevators
                   ,"Number of elevators"
                   , true)->check(validate_int);

    CLI11_PARSE(app, argc, argv);

    vector<Floor> floors{};
    vector<thread> thread_pool{};
    vector<Elevator> elevators{};
    MessageQueue message_queue{};

    for (unsigned int i=0; i < floor_number; i++) {
        floors.push_back(Floor{});
        thread t{floors.back()};
        thread_pool.push_back(move(t));
    }

    for (unsigned int i=0; i < number_of_elevators; i++) {
        elevators.push_back({1, travel_time});
    }

    thread t{Coordinator{elevators}};
    thread_pool.push_back(move(t));

    for (unsigned int i=0; i < thread_pool.size(); i++) {
        thread_pool[i].join();
    }

}
