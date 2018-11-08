// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// direction-enum.cpp
//
#include "direction-enum.hpp"

#include "misc/log-macros.hpp"

namespace heroespath
{
namespace gui
{

    const std::string Direction::ToString(const Direction::Enum ENUM)
    {
        switch (ENUM)
        {
            case Left:
            {
                return "Left";
            }
            case Right:
            {
                return "Right";
            }
            case Up:
            {
                return "Up";
            }
            case Down:
            {
                return "Down";
            }
            case Count:
            {
                return "(Count)";
            }
            default:
            {
                M_HP_LOG_ERR(
                    "enum_value=" << static_cast<EnumUnderlying_t>(ENUM) << " is invalid. (count="
                                  << static_cast<EnumUnderlying_t>(Count) << ")");

                return "";
            }
        }
    }

    gui::Direction::Enum Direction::Opposite(const Direction::Enum ENUM)
    {
        switch (ENUM)
        {
            case Left:
            {
                return Direction::Right;
            }
            case Right:
            {
                return Direction::Left;
            }
            case Up:
            {
                return Direction::Down;
            }
            case Down:
            {
                return Direction::Up;
            }
            case Count:
            default:
            {
                M_HP_LOG_ERR(
                    "enum_value=" << static_cast<EnumUnderlying_t>(ENUM) << " is invalid. (count="
                                  << static_cast<EnumUnderlying_t>(Count) << ")");

                return Direction::Count;
            }
        }
    }

} // namespace gui
} // namespace heroespath
