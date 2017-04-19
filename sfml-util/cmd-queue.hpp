#ifndef APPBASE_SFMLUTIL_CMDQUEUE_INCLUDED
#define APPBASE_SFMLUTIL_CMDQUEUE_INCLUDED
//
// cmd-queue.hpp
//  Code that manages a queue of Loop commands.
//
#include "sfml-util/loop-cmd.hpp"

#include <memory>
#include <queue>


namespace sfml_util
{

    //A class that manages a queue of Loop command objects.
    class CommandQueue
    {
        //prevent copy construction and copy assignment
        CommandQueue(const CommandQueue &);
        CommandQueue & operator=(const CommandQueue &);

    public:
        CommandQueue();
        virtual ~CommandQueue();

        void Clear();
        ILoopCmdSPtr_t Pop();
        ILoopCmdSPtr_t Front();
        void Push(const ILoopCmdSPtr_t &);
        inline bool IsEmpty() const { return queue_.empty(); }

    private:
        std::queue<ILoopCmdSPtr_t> queue_;
    };


    using CommandQueueSPtr_t = std::shared_ptr<CommandQueue>;

}
#endif //APPBASE_SFMLUTIL_CMDQUEUE_INCLUDED
