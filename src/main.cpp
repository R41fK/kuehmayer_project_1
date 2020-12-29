
#include <iostream>
#include <thread>

#include "CLI11.hpp"
#include "rang.hpp"
#include "spdlog/spdlog.h"
#include "Floor.h"
#include "Coordinator.h"
#include "Elevator.h"
#include "Repl.h"

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
    MessageQueue* coordinator_queue = new MessageQueue();

    cout << endl;
    cout << "***********************************************************************************************************************" << endl;
    cout << rang::fg::green
         << rang::style::bold  << "Started elevator_control with " 
         << rang::fg::yellow   << floor_number << " Floors " 
         << rang::fg::green    << "and "
         << rang::fg::yellow   << number_of_elevators << " Elevator(s)"
         << rang::fg::green    << ". A Elevator " 
         << rang::fg::yellow   << "travels " << travel_time << "s" 
         << rang::fg::green    << " from one floor to the next one."
         << rang::style::reset 
         << rang::fg::reset << endl;
    cout << "***********************************************************************************************************************" << endl;
    cout << endl;

    spdlog::set_pattern("[%^%l%$] %v");

    for (unsigned int i=1; i <= floor_number; i++) {
        floors.push_back(Floor{i, coordinator_queue});
        thread t{ref(floors.back())};
        thread_pool.push_back(move(t));
    }


    for (unsigned int i=1; i <= number_of_elevators; i++) {
        elevators.push_back(Elevator{i, travel_time, coordinator_queue});
        thread t{ref(elevators.back())};
        thread_pool.push_back(move(t));
    }


    thread tc{Coordinator{ref(elevators), coordinator_queue}};
    thread_pool.push_back(move(tc));

    thread tr{Repl{ref(floors), ref(elevators), floor_number, number_of_elevators}};
    thread_pool.push_back(move(tr));


    for (unsigned int i=0; i < thread_pool.size(); i++) {
        thread_pool[i].join();
    }

    delete coordinator_queue;
}
