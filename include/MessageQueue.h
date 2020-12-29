#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>

#include "Message.h"

class MessageQueue
{
private:
    std::queue<Message> message_queue{};
    std::mutex m{};
    std::condition_variable empty{};
public:
   Message pop();
   void push(Message message);
};