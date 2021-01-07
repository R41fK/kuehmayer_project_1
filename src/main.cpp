
#include <iostream>
#include <thread>
#include <fstream>

#include "CLI11.hpp"
#include "rang.hpp"
#include "spdlog/spdlog.h"
#include "json.hpp"
#include "toml.hpp"
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

    try {
        ifstream i(str);
        i >> j;
    } catch (const nlohmann::detail::parse_error& err) {
        cerr << "Not Valid json" << endl;
        cerr << err.what() << endl;
        exit(0);
    }
    

    if (j.contains("floor-number")) {
        if (validate_int(to_string(j["floor-number"])) != "" && j["floor-number"] <= 0) { //use of validate function to see if it is a integer or a float
            cerr << "Floor-number musst be a unsigned integer greater than 0" << endl;
            exit(0);
        }
    } else if (j.contains("elevators")) {
        if (validate_int(to_string(j["elevators"])) != "" && j["elevators"] <= 0) { //use of validate function to see if it is a integer or a float
            cerr << "Elevators musst be a unsigned integer greater than 0 " << endl;
            exit(0);
        }
    } else if (j.contains("seconds-between-floors")) {
        if (!j["seconds-between-floors"].is_number() || j["seconds-between-floors"] <= 0) {
            cerr << "Seconds-between-floors musst be a positive float greater than 0" << endl;
            exit(0);
        }
    } else if (j.contains("override")) {
        if (!j["override"].is_boolean()) {
            cerr << "Override musst be a boolean" << endl;
            exit(0);
        }
    }

    return j;
}


void use_toml_config(string config_file_toml, uint& floor_number, uint& number_of_elevators, float& travel_time, bool& override){
    try {
        toml::table t =  toml::parse_file(config_file_toml);

        if (t["elevator_control"]["seconds-between-floors"]) {
            try {
                travel_time = t["elevator_control"]["seconds-between-floors"].value<float>().value();
            } catch (const bad_optional_access& err) {
                cerr << "Seconds-between-floors musst be a positive float greater than 0" << endl;
                exit(0);
            }
        }
        if (t["elevator_control"]["elevators"]) {
            try {
                number_of_elevators = t["elevator_control"]["elevators"].value<uint>().value();
            } catch (const bad_optional_access& err) {
                cerr << "Elevators musst be a unsigned integer greater than 0" << endl;
                exit(0);
            }
        }
        if (t["elevator_control"]["floor-number"]) {
            try {
                floor_number = t["elevator_control"]["floor-number"].value<uint>().value();
            } catch (const bad_optional_access& err) {
                cerr << "Floor-number musst be a unsigned integer greater than 0" << endl;
                exit(0);
            }
        }
        if (t["elevator_control"]["override"]) {
            try {
                override = t["elevator_control"]["override"].value<bool>().value();
            } catch (const bad_optional_access& err) {
                cerr << "Override musst be a boolean" << endl;
                exit(0);
            }
        }
        
    } catch (const toml::parse_error& err) {
        cerr << "Not Valid toml" << endl;
        cerr << err.what() << endl;
        exit(0);
    }
}


void print_stars(){
    for (int i=0; i < 143; i++){
        cout << "*";
    }
    cout << endl;
}


int main(int argc, char* argv[]) {
    
    CLI::App app("elevator_control");

    float travel_time{3.0};
    unsigned int floor_number{3};
    unsigned int number_of_elevators{1};
    bool override{false};
    string config_file_json{};
    string config_file_toml{};

    // create CLI

    auto option_s{app.add_option("-s, --seconds-between-floors"
                                , travel_time
                                , "The time it takes to move between two floor_number, that are next to each other"
                                , true)->check(validate_float)->check(CLI::PositiveNumber)};

    auto option_f{app.add_option("-f, --floor-number"
                                , floor_number
                                , "Number of floors for the elevator"
                                , true)->check(validate_int)->check(CLI::PositiveNumber)};

    auto option_e{app.add_option("-e, --elevators"
                                , number_of_elevators
                                , "Number of elevators"
                                , true)->check(validate_int)->check(CLI::PositiveNumber)};

    auto option_j{app.add_option("-j, --config-file-json"
                                , config_file_json
                                , "Get the configuration of the program from a JSON file. Overwrites other configurations"
                                )->excludes(option_s)->excludes(option_f)->excludes(option_e)
                                ->check(CLI::ExistingFile)};

    auto option_t{app.add_option("-t, --config-file-toml"
                                , config_file_toml
                                , "Get the configuration of the program from a JSON file. Overwrites other configurations"
                                )->excludes(option_s)->excludes(option_f)->excludes(option_e)->excludes(option_j)
                                ->check(CLI::ExistingFile)};


    app.add_flag("-o, --override"
                 ,override
                 ,"Add a override option to the elevators")
                 ->excludes(option_j)->excludes(option_t);

    CLI11_PARSE(app, argc, argv);

    //use config file

    if (config_file_json != "") {
        json j = validate_json(config_file_json);

        travel_time = j["seconds-between-floors"];
        number_of_elevators = j["elevators"];
        floor_number = j["floor-number"];
        override = j["override"];
    } else if (config_file_toml != "") {
        use_toml_config(config_file_toml, floor_number, number_of_elevators, travel_time, override);
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
    print_stars();
    
    cout << rang::fg::green
         << rang::style::bold  << "Started elevator_control with " 
         << rang::fg::yellow   << floor_number << " Floors " 
         << rang::fg::green    << "and "
         << rang::fg::yellow   << number_of_elevators << " Elevator(s)"
         << rang::fg::green    << ". An Elevator " 
         << rang::fg::yellow   << "travels " << travel_time << "s" 
         << rang::fg::green    << " from one floor to the next one. "
         << rang::fg::yellow   << "Override "
         << rang::fg::green    << "is ";

    if (override) {
        cout << rang::fg::yellow << "activated.";
    } else {
        cout << rang::fg::yellow << "deactivated.";
    }

    cout << rang::style::reset 
         << rang::fg::reset << endl;

    print_stars();
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
