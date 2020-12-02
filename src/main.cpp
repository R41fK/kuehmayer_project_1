
#include <iostream>
#include <thread>

#include "CLI11.hpp"

using namespace std;

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
    unsigned int floors{3};

    app.add_option("-s, --seconds-between-floors"
                  , travel_time
                  , "The time it takes to move between two floors, that are next to each other"
                  , true)->check(validate_float);

    app.add_option("-f, --floors"
                  , floors
                  , "Number of floors for the elevator"
                  , true)->check(validate_int);

    CLI11_PARSE(app, argc, argv);

    cout << travel_time << " " << floors << endl;
}
