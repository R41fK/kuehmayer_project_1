
#include <iostream>
#include <thread>

#include "CLI11.hpp"
#include "rang.hpp"
#include "Floor.h"
#include "Coordinator.h"
#include "Elevator.h"

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
    unsigned int elevators{1};

    app.add_option("-s, --seconds-between-floor_number"
                  , travel_time
                  , "The time it takes to move between two floor_number, that are next to each other"
                  , true)->check(validate_float);

    app.add_option("-f, --floor_number"
                  , floor_number
                  , "Number of floor_number for the elevator"
                  , true)->check(validate_int);

    app.add_option("-e, --elevators"
                   ,elevators
                   ,"Number of elevators"
                   , true)->check(validate_int);

    CLI11_PARSE(app, argc, argv);

    vector<Floor> floors{};
    vector<thread> thread_pool{};


    for (unsigned int i=0; i < floor_number; i++) {
        floors.push_back(Floor{});
        thread t{floors.back()};
        thread_pool.push_back(move(t));
    }

    thread t{Coordinator{}};
    thread_pool.push_back(move(t));

    for (unsigned int i=0; i < elevators; i++) {
        thread t{Elevator{}};
        thread_pool.push_back(move(t));
    }

    for (unsigned int i=0; i < thread_pool.size(); i++) {
        thread_pool[i].join();
    }

}
