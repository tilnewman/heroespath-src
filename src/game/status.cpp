// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// status.cpp
//
#include "status.hpp"

#include <sstream>

namespace heroespath
{
namespace game
{

    const std::string FadeStatus::ToString() const
    {
        std::ostringstream ss;

        ss << "FadeStatus(is_fading" << std::boolalpha << is_fading
           << ", direction=" << ((FadeDirection::In == direction) ? "in" : "out")
           << ", current_color=" << current_color << ", target_color=" << target_color
           << ", will_draw_under_popup=" << will_draw_under_popup << ")";

        return ss.str();
    }

    const std::string Status::ToString() const
    {
        std::ostringstream ss;

        auto prefixSeparatorString = [&]() {
            if (ss.str().empty() == false)
            {
                ss << ", ";
            }
        };

        if (is_loop_running)
        {
            prefixSeparatorString();
            ss << "loop_is_running";
        }

        // if (is_loop_stop_requested)
        //{
        //    prefixSeparatorString();
        //    ss << "loop_exit_requested";
        //}

        if (is_game_exit_requested)
        {
            prefixSeparatorString();
            ss << "game_exit_requested";
        }

        if (fade_status.is_fading)
        {
            prefixSeparatorString();
            ss << fade_status.ToString();
        }

        if (ss.str().empty())
        {
            return "Status(none)";
        }
        else
        {
            return "Status(" + ss.str() + ")";
        }
    }

} // namespace game
} // namespace heroespath
