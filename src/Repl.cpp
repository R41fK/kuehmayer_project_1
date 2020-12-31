#include <iostream>
#include <algorithm>
#include <string>

#include "peglib.h"
#include "spdlog/spdlog.h"
#include "Repl.h"

using namespace std;
using namespace peg;

void Repl::show_help(){
    spdlog::info("Commands:");
    spdlog::info("  OVERRIDE <CALL|MOVE>                   Oeverride a command. The Elevator will travel to the given floor without stopping. Can be used on call and move");
    spdlog::info("  CALL <floor_number>                    to simulate a button click in a floor, to call a elevator");
    spdlog::info("  MOVE <elevator_number> <floor_number>  to simulate a button click in a certain elevator, to move to a certain floor");
}


void Repl::call(string number, bool override) {
    if (unsigned (stoi(number)) <= this->floor_number) {
        if (override) {
            Message message{"Floor" + number, "override", unsigned (stoi(number)), 0};
            this->floors[stoi(number) - 1].push(message);
        } else {
            Message message{"Floor" + number, "call", unsigned (stoi(number)), 0};
            this->floors[stoi(number) - 1].push(message);
        }

        
    } else {
        spdlog::warn("There are only " + to_string(this->floor_number) + " Floors! Your input: " + number);
    }
}


void Repl::move(string floor_number, string elevator_number, bool override) {
    if (unsigned (stoi(floor_number)) <= this->floor_number && unsigned (stoi(elevator_number)) <= this->elevator_number) {
        if (override) {
            Message message{"Elevator" + elevator_number, "override", unsigned (stoi(floor_number)), unsigned (stoi(elevator_number))};
            this->elevators[stoi(elevator_number) - 1].push(message);
        } else {
            Message message{"Elevator" + elevator_number, "move", unsigned (stoi(floor_number)), unsigned (stoi(elevator_number))};
            this->elevators[stoi(elevator_number) - 1].push(message);
        }

    }  else {
        spdlog::warn("There are only " + to_string(this->floor_number) + " Floors! Your input: " + floor_number + "!");
        spdlog::warn("There are only " + to_string(this->elevator_number) + " Elevators! Your input: " + elevator_number +"!");
    }
}

void Repl::operator()() {

    spdlog::info("Type help to get a list of commands");

    parser parser(R"(
        Start    <- 'help' / Call / Move / Override
        Override    <- 'override' Call / 'override' Move
        Call        <- 'call' Number
        Move        <- 'move' Number Number
        Number      <- < [0-9]+ >
        %whitespace <- [ \t]*
    )");

    string input{};
    bool override{false};

     while (1) {
        
        getline(cin, input);


        for_each(input.begin(), input.end(), [](char & c) {
            c = ::tolower(c);
        });
        
        input.erase(0, input.find_first_not_of(" \n\r\t"));
        input.erase(input.find_last_not_of(" \n\r\t")+1);

        if (parser.parse(input.c_str())) {
            if (input.rfind("override", 0) == 0) {
                override = true;
                input.erase(0, 9);
            } else {
                override = false;
            }

            if (input.rfind("help", 0) == 0) {
                show_help();
            } else if (input.rfind("call", 0) == 0) {
                call(input.substr(input.find_first_of("0123456789"), input.find(" ")), override);

            } else if (input.rfind("move", 0) == 0) {
                size_t first_pos{input.find_first_of("0123456789")};
                size_t last_pos{input.find(" ", first_pos)};
                size_t elevator_pos{input.find_first_of("0123456789", last_pos+1)};
                size_t elevator_pos_last{input.find(" ", elevator_pos)};

                move(input.substr(elevator_pos, elevator_pos_last - elevator_pos), input.substr(first_pos, last_pos - first_pos), override);

            }
        } else {
            spdlog::warn("No Command: " + input);
            show_help();
        }        
    }
}