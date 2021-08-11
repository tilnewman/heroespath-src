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

namespace heroespath
{
namespace game
{

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

} // namespace game
} // namespace heroespath
