#include <iostream>
#include <algorithm>
#include <string>

#include "peglib.h"
#include "Repl.h"

using namespace std;
using namespace peg;

void Repl::show_help(){
    cout << ">>> Commands:" << endl;
    cout << ">>>   CALL <floor_number>                    to simulate a button click in a floor, to call a elevator" << endl;
    cout << ">>>   MOVE <floor_number> <elevator_number>  to simulate a button click in a certain elevator, to move to a certain floor" << endl;
}


void Repl::call(string number) {
    if (unsigned (stoi(number)) <= this->floor_number) {
        Message message{"Floor" + number, "call", unsigned (stoi(number)), 0};
    
        this->message_queue->push(message);
    } else {
        cout << ">>> There are only " << this->floor_number << " Floors! Your input: " + number << endl;
    }

}


void Repl::move(string floor_number, string elevator_number) {
    if (unsigned (stoi(floor_number)) <= this->floor_number && unsigned (stoi(elevator_number)) <= this->elevator_number) {
        Message message{"Elevator" + elevator_number, "move", unsigned (stoi(floor_number)), unsigned (stoi(elevator_number))};
    
        this->message_queue->push(message);
    }  else {
        cout << ">>> There are only " << this->floor_number << " Floors! Your input: " + floor_number << "!" << endl;
        cout << ">>> There are only " << this->elevator_number << " Elevators! Your input: " + elevator_number << "!" << endl;
    }
}

void Repl::operator()() {

    cout << ">>> Type help to get a list of commands" << endl;

    parser parser(R"(
        KEYWORDI    <- 'help' / 'call' Number / 'move' Number Number
        Number      <- < [0-9]+ >
        %whitespace <- [ \t]*
    )");

     while (1) {

        string input{};

        cout << ">>> ";
        getline(cin, input);


        for_each(input.begin(), input.end(), [](char & c) {
            c = ::tolower(c);
        });
        
        input.erase(0, input.find_first_not_of(" \n\r\t"));
        input.erase(input.find_last_not_of(" \n\r\t")+1);

        if (parser.parse(input.c_str())) {
            if (input.rfind("help", 0) == 0) {
                show_help();
            } else if (input.rfind("call", 0) == 0) {
                call(input.substr(input.find_first_of("0123456789"), input.find(" ")));

            } else if (input.rfind("move", 0) == 0) {
                size_t first_pos{input.find_first_of("0123456789")};
                size_t last_pos{input.find(" ", first_pos)};
                size_t elevator_pos{input.find_first_of("0123456789", last_pos+1)};
                size_t elevator_pos_last{input.find(" ", elevator_pos)};

                move(input.substr(first_pos, last_pos - first_pos), input.substr(elevator_pos, elevator_pos_last - elevator_pos));

            }
        } else {
            cout << ">>> No Command: " << input << endl;
            show_help();
        }

        
    }
}