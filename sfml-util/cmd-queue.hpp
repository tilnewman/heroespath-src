///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software.  If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef SFMLUTIL_CMDQUEUE_INCLUDED
#define SFMLUTIL_CMDQUEUE_INCLUDED
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
        CommandQueue(const CommandQueue &) =delete;
        CommandQueue & operator=(const CommandQueue &) =delete;

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
#endif //SFMLUTIL_CMDQUEUE_INCLUDED
