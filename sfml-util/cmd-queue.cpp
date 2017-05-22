//
// cmd-queue.cpp
//
#include "cmd-queue.hpp"
#include "game/i-popup-callback.hpp"


namespace sfml_util
{

    CommandQueue::CommandQueue()
    :
        queue_()
    {}


    CommandQueue::~CommandQueue()
    {}


    void CommandQueue::Clear()
    {
        const std::size_t CMD_COUNT(queue_.size());
        for (std::size_t i(0); i < CMD_COUNT; ++i)
            queue_.pop();
    }


    ILoopCmdSPtr_t CommandQueue::Front()
    {
        return queue_.front();
    }


    ILoopCmdSPtr_t CommandQueue::Pop()
    {
        ILoopCmdSPtr_t c(Front());
        queue_.pop();
        return c;
    }


    void CommandQueue::Push(const ILoopCmdSPtr_t & c)
    {
        return queue_.push(c);
    }

}
