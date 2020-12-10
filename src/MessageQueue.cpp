
#include "Message.h"
#include "MessageQueue.h"

using namespace std;

void MessageQueue::push(Message message){
    lock_guard<mutex> lg{this->m};
    this->message_queue.push(message);
    this->empty.notify_all();
}


Message MessageQueue::pop(string message){
    unique_lock<mutex> ul{this->m};
    empty.wait(ul, [this, message]{return this->message_queue.size() 
                                   && this->message_queue.front().get_message() == message
                                 ;});
                            
    Message return_message = this->message_queue.front();
    this->message_queue.pop();
    return return_message;
}