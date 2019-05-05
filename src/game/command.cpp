// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// game-command.cpp
//
#include "game/command.hpp"

#include "misc/enum-util.hpp"

#include <sstream>

namespace heroespath
{
namespace game
{

    const std::string FadeCommand::ToString() const
    {
        std::ostringstream ss;

        ss << "fade, " << ((FadeDirection::In == direction) ? "in" : "out")
           << ", fade_speed=" << speed << ", fade_color=" << color;

        if (set_will_draw_under_popup_opt)
        {
            ss << ", set_will_drawn_under_popup_to=" << std::boolalpha
               << set_will_draw_under_popup_opt.value();
        }

        return ss.str();
    }

    const std::string ExecuteCommand::ToString() const
    {
        std::ostringstream ss;

        auto prefixSeparatorString = [&]() {
            if (ss.str().empty() == false)
            {
                ss << ", ";
            }
        };

        ss << "run_game_loop";

        if (fade_opt)
        {
            prefixSeparatorString();
            ss << fade_opt->ToString();
        }

        if (will_hide_mouse)
        {
            prefixSeparatorString();
            ss << "hide_mouse";
        }

        if (will_keystroke_exit)
        {
            prefixSeparatorString();
            ss << "keystrokes_exit";
        }

        if (will_keystroke_ignore)
        {
            prefixSeparatorString();
            ss << "keystrokes_ignored";
        }

        if (will_mouse_ignore)
        {
            prefixSeparatorString();
            ss << "mouse_ignored";
        }

        if (will_mouse_click_exit)
        {
            prefixSeparatorString();
            ss << "mouse_clicks_exit";
        }

        return ss.str();
    }

    const std::string MusicCommand::ToString() const
    {
        std::ostringstream ss;

        auto prefixSeparatorString = [&]() {
            if (ss.str().empty() == false)
            {
                ss << ", ";
            }
        };

        ss << "music";

        if (EnumUtil<gui::music>::IsValid(to_stop) || (gui::music::All == to_stop))
        {
            prefixSeparatorString();
            ss << "music_stop=" << gui::music::ToString(to_stop);
        }

        if (EnumUtil<gui::music>::IsValid(to_start))
        {
            prefixSeparatorString();
            ss << "music_start=" << gui::music::ToString(to_start);
        }

        if (volume_min > 0.0f)
        {
            prefixSeparatorString();
            ss << "music_vol_min=" << volume_min;
        }

        return ss.str();
    }

    const std::string Command::ToString() const
    {
        std::ostringstream ss;

        if (music_opt)
        {
            ss << music_opt->ToString();
        }

        if (stage_opt)
        {
            ss << stage_opt->ToString();
        }

        if (popup_replace_opt)
        {
            ss << popup_replace_opt->ToString();
        }

        if (will_remove_popup)
        {
            ss << "popup_remove";
        }

        if (execute_opt)
        {
            ss << execute_opt->ToString();
        }

        if (ss.str().empty())
        {
            return "GameCommand(empty=ERROR=\"game commands should never be empty\")";
        }
        else
        {
            return "GameCommand(" + ss.str() + ")";
        }
    }

} // namespace game
} // namespace heroespath
