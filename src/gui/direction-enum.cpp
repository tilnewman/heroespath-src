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

#include <exception>
#include <sstream>

namespace heroespath
{
namespace gui
{

    const std::string Direction::ToString(const Direction::Enum E)
    {
        switch (E)
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
            default:
            {
                std::ostringstream ss;
                ss << "gui::Direction::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }

    gui::Direction::Enum Direction::Opposite(const Direction::Enum E)
    {
        switch (E)
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
            {
                return Direction::Count;
            }
            default:
            {
                std::ostringstream ss;
                ss << "gui::Direction::Opposite(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }
} // namespace gui
} // namespace heroespath
