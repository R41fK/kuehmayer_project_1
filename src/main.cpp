
#include <iostream>
#include <thread>
#include <fstream>

#include "CLI11.hpp"
#include "rang.hpp"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "json.hpp"
#include "toml.hpp"
#include "Floor.h"
#include "Coordinator.h"
#include "Elevator.h"
#include "Repl.h"
#include "Simulation.h"

using namespace std;
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
        return str + " is not an unsigned int";
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
            cerr << "Floor-number musst be an unsigned integer greater than 0" << endl;
            exit(0);
        }
    } else if (j.contains("elevators")) {
        if (validate_int(to_string(j["elevators"])) != "" && j["elevators"] <= 0) { //use of validate function to see if it is a integer or a float
            cerr << "Elevators musst be an unsigned integer greater than 0 " << endl;
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
    float sim_time{5.0};
    unsigned int floor_number{3};
    unsigned int number_of_elevators{1};
    bool override{false};
    bool log_to_file{false};
    bool log_level_debug{false};
    bool use_simulation{false};
    bool running{true};
    string config_file_json{};
    string config_file_toml{};
    string log_file{"control.log"};

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
                                , "Get the configuration of the program from a JSON file."
                                )->excludes(option_s)->excludes(option_f)->excludes(option_e)
                                ->check(CLI::ExistingFile)};

    auto option_t{app.add_option("-t, --config-file-toml"
                                , config_file_toml
                                , "Get the configuration of the program from a TOML file."
                                )->excludes(option_s)->excludes(option_f)->excludes(option_e)->excludes(option_j)
                                ->check(CLI::ExistingFile)};

    app.add_flag("-o, --override"
                 ,override
                 ,"Add an override option to the elevators")
                 ->excludes(option_j)->excludes(option_t);

    auto flag_l{app.add_flag("-l, --log-to-file"
                            , log_to_file
                            , "Set if the program should log to a file (Logs more than is shown in console)")};

    app.add_flag("-d, --log-level-debug"
                , log_level_debug
                , "Sets the loginglevel for log-to-file to debug (default=info)")
                ->needs(flag_l);

    app.add_option("--log-file"
                  , log_file
                  , "Define a file in that the logs are written"
                  , true)->needs(flag_l);

    auto flag_s{app.add_flag("--simulation"
                            , use_simulation
                            , "Simulate user inputs. (Deactivtes user input)")};

    app.add_option("--simulation-time"
                  , sim_time
                  , "The time the simulation waits until it simulates the next command"
                  , true)->check(validate_float)->check(CLI::PositiveNumber)->needs(flag_s);


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

    //set a pattern for the loggin messages and create a file logger

    spdlog::set_pattern("[%^%l%$] %v");

    std::shared_ptr<spdlog::logger> file_logger{spdlog::basic_logger_mt("file_logger", log_file)};
    if (log_to_file) {
        if (log_level_debug) {
            file_logger->set_level(spdlog::level::debug);
            file_logger->flush_on(spdlog::level::debug);
        } else {
            file_logger->set_level(spdlog::level::info);
            file_logger->flush_on(spdlog::level::info);
        }
    } else {
        file_logger->set_level(spdlog::level::off);
        file_logger->flush_on(spdlog::level::off);
    }
    
   
    file_logger->set_pattern("[%H:%M:%S] [thread %t] [%^%l%$] %v");
    file_logger->info("Started elevator_control");

    //create vectors and reserve their place 

    vector<Floor> floors{};
    vector<thread> thread_pool{};
    vector<Elevator> elevators{};
    MessageQueue* coordinator_queue = new MessageQueue();

    elevators.reserve(number_of_elevators);
    floors.reserve(floor_number);
    thread_pool.reserve(floor_number+number_of_elevators*2+2+use_simulation);

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


    if (log_to_file) {
        cout << endl;
        cout << rang::style::bold << rang::fg::yellow
             << "Logging activated" << rang::fg::green
             << " with the " << rang::fg::yellow
             << " logging-level ";
        if (log_level_debug) {
            cout << "debug";
        } else {
            cout << "info";
        }
        cout << rang::fg::green << ". The logger writes in the file "
             << rang::fg::yellow << log_file;

        cout << rang::style::reset 
         << rang::fg::reset << endl;
    }
    print_stars();
    cout << endl;


    if (use_simulation) {
        spdlog::info("To end the simulation, press enter");
    } else {
        spdlog::info("Type help to get a list of commands");
    }

    //create all floor threads

    for (unsigned int i=1; i <= floor_number; i++) {
        floors.insert(floors.begin() + i - 1, Floor{i, coordinator_queue, file_logger});
        thread t{ref(floors[i-1])};
        thread_pool.push_back(move(t));
    }

    //create all elevator threads, one for the movement of the elevator and one for the buttons clicked in the elevator

    for (unsigned int i=1; i <= number_of_elevators; i++) {
        elevators.insert(elevators.begin() + i - 1, Elevator{i, travel_time, coordinator_queue, file_logger});
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

    thread tc{Coordinator{ref(elevators), coordinator_queue, file_logger, ref(running)}};
    thread_pool.push_back(move(tc));

    //create the repl thread
    if (use_simulation) {
        thread tr{Simulation{ref(floors), ref(elevators), floor_number, number_of_elevators, override, file_logger, sim_time, ref(running)}};
        thread_pool.push_back(move(tr));
    }

    thread tr{Repl{ref(floors), ref(elevators), floor_number, number_of_elevators, override, file_logger, use_simulation, ref(running)}};
    thread_pool.push_back(move(tr));
    
   

    //join all threads

    for (unsigned int i=0; i < thread_pool.size(); i++) {
        thread_pool[i].join();
    }

    cout << endl;
    print_stars();

    cout << rang::fg::yellow
         << rang::style::bold
         << "elevator_control finished"
         << rang::style::reset 
         << rang::fg::reset 
         << endl;

    print_stars();
    cout << endl;

    file_logger->info("Finished elevator_control");

    delete coordinator_queue;
}