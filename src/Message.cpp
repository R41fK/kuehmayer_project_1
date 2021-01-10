#include <sstream>

#include "Message.h"

using namespace std;

string Message::get_message() {
    return this->message;
}

string Message::get_command() {
    return this->command;
}

unsigned int Message::get_floor() {
    return this->floor;
}

unsigned int Message::get_elevator_id() {
    return this->elevator_id;
}

string Message::to_string() {
    stringstream Str;
    Str << "Message(message=" << this->message << ", command=" << this->command 
            << ", floor=" << this->floor << ", elevator_id=" << this->elevator_id << ")";
    return Str.str();
}