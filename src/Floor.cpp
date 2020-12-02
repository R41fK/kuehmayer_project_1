#include <thread>
#include <chrono>

#include "Floor.h"

using namespace std;


void Floor::operator()() {
    while (1) {
        this_thread::sleep_for(chrono::seconds(1));
    }
}