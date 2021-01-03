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
    // return the first message that was pushed in and deletes it from the queue. If there is none it waits until there is one.
    Message pop();

    // push a message in the queue and send a signal to a waiting thread.
    void push(Message message);
};