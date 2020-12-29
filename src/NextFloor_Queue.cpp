#include <algorithm> 

#include "NextFloor_Queue.h"

using namespace std;

unsigned int NextFloor_Queue::get() {
    unique_lock<mutex> ul{this->m};
    con_empty.wait(ul, [this]{return this->next_floors.size();});
    return this->next_floors.front();
}

unsigned int NextFloor_Queue::front() {
    return this->next_floors.front();
}

void NextFloor_Queue::erase(unsigned int pos) {
    this->next_floors.erase(find(this->next_floors.begin(), this->next_floors.end(), pos));
}

void NextFloor_Queue::insert_l(unsigned int floor){
    this->next_floors.insert(lower_bound(this->next_floors.begin(), this->next_floors.end(), floor), floor);
    this->con_empty.notify_one();
}

void NextFloor_Queue::insert_u(unsigned int floor){
    this->next_floors.insert(upper_bound(this->next_floors.begin(), this->next_floors.end(), floor), floor);
    this->con_empty.notify_one();
}

bool NextFloor_Queue::empty() {
    return this->next_floors.empty();
}