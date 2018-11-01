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
namespace gui
{

    const std::string MouseState::ToString(const MouseState::Enum MOUSE_STATE)
    {
        switch (MOUSE_STATE)
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
            case Disabled:
            {
                return "Disabled";
            }
            case Count:
            default:
            {
                ThrowInvalidValueForFunction(MOUSE_STATE, "ToString");
            }
        }
    }

} // namespace gui
} // namespace heroespath
