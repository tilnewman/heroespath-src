// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// mouse-state-enum.cpp
//
#include "mouse-state-enum.hpp"

#include <exception>
#include <sstream>

namespace heroespath
{
namespace sfml_util
{

    const std::string MouseState::ToString(const MouseState::Enum E)
    {
        switch (E)
        {
            case Up:
            {
                return "Up";
            }
            case Over:
            {
                return "Over";
            }
            case Down:
            {
                return "Down";
            }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "sfml_util::MouseState::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }

    bool MouseState::IsValid(const MouseState::Enum E)
    {
        switch (E)
        {
            case Up:
            case Over:
            case Down:
            {
                return true;
            }
            case Count:
            default:
            {
                return false;
            }
        }
    }
} // namespace sfml_util
} // namespace heroespath
