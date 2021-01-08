#include <algorithm> 
#include <iostream>

#include "NextFloor_Queue.h"

using namespace std;

unsigned int NextFloor_Queue::get() {
    unique_lock<mutex> ul{this->m};
    con_empty.wait(ul, [this]{return this->next_floors.size();});
    return this->next_floors.front();
}

unsigned int NextFloor_Queue::front() {
    lock_guard<mutex> lg{this->m};
    if (this->next_floors.empty()) {
        return 0;
    }
    return this->next_floors.front();
}

void NextFloor_Queue::erase(unsigned int pos) {
    lock_guard<mutex> lg{this->m};
    this->next_floors.erase(find(this->next_floors.begin(), this->next_floors.end(), pos));
}

void NextFloor_Queue::insert_first(unsigned int floor){
    lock_guard<mutex> lg{this->m};
    if (find(this->next_floors.begin(), this->next_floors.end(), floor) != this->next_floors.end()) {
        this->next_floors.erase(find(this->next_floors.begin(), this->next_floors.end(), floor));
    }
    this->next_floors.insert(this->next_floors.begin(), floor);
    this->con_empty.notify_one();
}

void NextFloor_Queue::insert(unsigned int floor){
    lock_guard<mutex> lg{this->m};
    if (find(this->next_floors.begin(), this->next_floors.end(), floor) == this->next_floors.end()) {
        for (unsigned int i{1}; i < this->next_floors.size(); i++) {
            if ((this->next_floors[i-1] > floor && this->next_floors[i] <= floor)
            || (this->next_floors[i-1] < floor && this->next_floors[i] >= floor)
            ){
                this->file_logger->debug("The queue decided to add the Floor " + to_string(floor) + " on position " + to_string(i));

                this->next_floors.insert(this->next_floors.begin() + i, floor);
                this->con_empty.notify_one();
                return;
            }
        }
    }

    this->file_logger->debug("The queue decided to add the Floor " + to_string(floor) + " at the end of the queue. the queue size is " 
                            + to_string(this->next_floors.size()));

    this->next_floors.push_back(floor);
    this->con_empty.notify_one();
}

bool NextFloor_Queue::empty() {
    lock_guard<mutex> lg{this->m};
    return this->next_floors.empty();
}