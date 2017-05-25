// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
