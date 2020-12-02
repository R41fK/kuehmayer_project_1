#include <thread>
#include <chrono>

#include "Coordinator.h"

using namespace std;


void Coordinator::operator()() {
    while (1) {
        this_thread::sleep_for(chrono::seconds(1));
    }
}