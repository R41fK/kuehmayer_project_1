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

    std::string get_message();
    std::string get_command();
    unsigned int get_floor();
    unsigned int get_elevator_id();
};