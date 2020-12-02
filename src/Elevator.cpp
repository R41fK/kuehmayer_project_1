#include <thread>
#include <chrono>

#include "Elevator.h"

using namespace std;


void Elevator::operator()() {
    while (1) {
        this_thread::sleep_for(chrono::seconds(1));
    }
}