#pragma once

#include <string>

class Message
{
private:
    std::string message{};
    std::string command{};
    unsigned int floor{};
    unsigned int elevator_id{};
public:
    Message(std::string message, std::string command, unsigned int floor, unsigned int elevator_id):
    message{message},
    command{command},
    floor{floor},
    elevator_id{elevator_id}
    {};

    // get the message
    std::string get_message();

    // get the command
    std::string get_command();

    // get the floor
    unsigned int get_floor();

    // get the elevator id
    unsigned int get_elevator_id();
};