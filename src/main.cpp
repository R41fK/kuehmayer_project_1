
#include <iostream>
#include <thread>

#include "CLI11.hpp"
#include "rang.hpp"
#include "spdlog/spdlog.h"
#include "json.hpp"
#include "Floor.h"
#include "Coordinator.h"
#include "Elevator.h"
#include "Repl.h"

using namespace std;
using namespace rang;
using json = nlohmann::json;


//implement own validators for better error messages
string validate_float(const string& str) { 
    size_t found{str.find_first_not_of("0123456789.")};

    if (found <= str.length()) {
        return str + " is not a float or not positive";
    }

    return "";
}


//implement own validators for better error messages
string validate_int(const string& str) { 
    size_t found{str.find_first_not_of("0123456789")};

    if (found <= str.length()) {
        return str + " is not a int or not positive";
    }

    return "";
}


//validate the json config 
json validate_json(const string& str) {
    json j;
    ifstream i(str);
    i >> j;

    if (j.empty()) {
        cerr << "Your config File is empty" << endl;
        exit(0);
    } else if (!j.contains("floor-number")) {
        cerr << "Missing configuration for uint floor-number" << endl;
        exit(0);
    } else if (!j.contains("elevators")) {
        cerr << "Missing configuration for uint elevators" << endl;
        exit(0);
    } else if (!j.contains("seconds-between-floors")) {
        cerr << "Missing configuration for float seconds-between-floors" << endl;
        exit(0);
    } else if (!j.contains("override")) {
        cerr << "Missing configuration for bool override" << endl;
        exit(0);
    } else if (!j["override"].is_boolean()) {
        cerr << "Override musst be a boolean" << endl;
        exit(0);
    } else if (!j["elevators"].is_number()) {
        cerr << "Elevators musst be a unsigned integer" << endl;
        exit(0);
    } else if (!j["floor-number"].is_number()) {
        cerr << "Floor-number musst be a unsigned integer" << endl;
        exit(0);
    } else if (!j["seconds-between-floors"].is_number()) {
        cerr << "Seconds-between-floors musst be a float" << endl;
        exit(0);
    } else if (validate_int(to_string(j["elevators"])) != "") { //use of validate function to see if it is a integer or a float
        cerr << "Elevators musst be a unsigned integer" << endl;
        exit(0);
    } else if (validate_int(to_string(j["floor-number"])) != "") { //use of validate function to see if it is a integer or a float
        cerr << "Floor-number musst be a unsigned integer" << endl;
        exit(0);
    } else if (j["seconds-between-floors"] < 0) {
        cerr << "Seconds-between-floors musst be a positive float" << endl;
        exit(0);
    }

    return j;
}

int main(int argc, char* argv[]) {
    
    CLI::App app("elevator_control");

    float travel_time{3.0};
    unsigned int floor_number{3};
    unsigned int number_of_elevators{1};
    bool override{false};
    string config_file{};

    // create CLI

    app.add_option("-s, --seconds-between-floors"
                  , travel_time
                  , "The time it takes to move between two floor_number, that are next to each other"
                  , true)->check(validate_float);

    app.add_option("-f, --floor-number"
                  , floor_number
                  , "Number of floors for the elevator"
                  , true)->check(validate_int);

    app.add_option("-e, --elevators"
                   , number_of_elevators
                   , "Number of elevators"
                   , true)->check(validate_int);

    app.add_option("-c, --config-file"
                   , config_file
                   , "Get the configuration of the program from a JSON file. Overwrites other configurations"
                   )->check(CLI::ExistingFile);


    app.add_flag("-o, --override"
                 ,override
                 ,"Add a override option to the elevators");

    CLI11_PARSE(app, argc, argv);

    //use config file

    if (config_file != "") {
        json j = validate_json(config_file);

        travel_time = j["seconds-between-floors"];
        number_of_elevators = j["elevators"];
        floor_number = j["floor-number"];
        override = j["override"];
    }

    //create vectors and reserve their place 

    vector<Floor> floors{};
    vector<thread> thread_pool{};
    vector<Elevator> elevators{};
    MessageQueue* coordinator_queue = new MessageQueue();

    elevators.reserve(number_of_elevators);
    floors.reserve(floor_number);
    thread_pool.reserve(floor_number+number_of_elevators*2+2);

    //print a little summary of the programm

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

    //set a pattern for the loggin messages

    spdlog::set_pattern("[%^%l%$] %v");

    //create all floor threads

    for (unsigned int i=1; i <= floor_number; i++) {
        floors.insert(floors.begin() + i - 1, Floor{i, coordinator_queue});
        thread t{ref(floors[i-1])};
        thread_pool.push_back(move(t));
    }

    //create all elevator threads, one for the movement of the elevator and one for the buttons clicked in the elevator

    for (unsigned int i=1; i <= number_of_elevators; i++) {
        elevators.insert(elevators.begin() + i - 1, Elevator{i, travel_time, coordinator_queue});
        thread t{ref(elevators[i-1])};
        thread_pool.push_back(move(t));
        thread t1{[&](){
            elevators[i-1].buttons();
        }};
        thread_pool.push_back(move(t1));
        this_thread::sleep_for(chrono::milliseconds(1)); //waiting so that the thread can start instantly, 
        //because if another object gets pushed in the vector the reference would be wrong
    }

    //create the coordinator thread

    thread tc{Coordinator{ref(elevators), coordinator_queue}};
    thread_pool.push_back(move(tc));

    //create the repl thread

    thread tr{Repl{ref(floors), ref(elevators), floor_number, number_of_elevators, override}};
    thread_pool.push_back(move(tr));

    //join all threads

    for (unsigned int i=0; i < thread_pool.size(); i++) {
        thread_pool[i].join();
    }

    delete coordinator_queue;
}
