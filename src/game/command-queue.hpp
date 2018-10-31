// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GAME_COMMAND_QUEUE_HPP_INCLUDED
#define HEROESPATH_GAME_COMMAND_QUEUE_HPP_INCLUDED
//
// command-queue.hpp
//
#include "game/command.hpp"

#include <vector>

namespace heroespath
{
namespace game
{

    class CommandQueue
    {
    public:
        CommandQueue() = default;

        std::size_t Size() const { return queueVec_.size(); }

        bool IsEmpty() const { return queueVec_.empty(); }

        void ClearAndPush(const CommandVec_t & COMMAND_VEC) { queueVec_ = COMMAND_VEC; }

        const CommandOpt_t Pop()
        {
            if (queueVec_.empty())
            {
                return boost::none;
            }
            else
            {
                const auto TOP_COMMAND { queueVec_.front() };
                queueVec_.erase(std::begin(queueVec_));
                return TOP_COMMAND;
            }
        }

    private:
        CommandVec_t queueVec_;
    };

} // namespace game
} // namespace heroespath

#endif // HEROESPATH_GAME_COMMAND_QUEUE_HPP_INCLUDED
