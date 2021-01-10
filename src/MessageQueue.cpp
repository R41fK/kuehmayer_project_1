#include "Message.h"
#include "MessageQueue.h"

using namespace std;

void MessageQueue::push(Message message){
    lock_guard<mutex> lg{this->m};
    this->message_queue.push(message);
    this->empty.notify_one();
}


Message MessageQueue::pop(){
    unique_lock<mutex> ul{this->m};
    empty.wait(ul, [this]{return this->message_queue.size();});
    Message return_message = this->message_queue.front();
    this->message_queue.pop();
    return return_message;
}