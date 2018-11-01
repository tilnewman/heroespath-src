// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// justified-enum.cpp
//
#include "justified-enum.hpp"

#include <exception>
#include <sstream>

namespace heroespath
{
namespace gui
{

    const std::string Justified::ToString(const Justified::Enum JUSTIFICATION)
    {
        switch (JUSTIFICATION)
        {
            case Justified::Left:
            {
                return "Left";
            }
            case Justified::Right:
            {
                return "Right";
            }
            case Justified::Center:
            {
                return "Center";
            }
            case Justified::Count:
            default:
            {
                ThrowInvalidValueForFunction(JUSTIFICATION, "ToString");
            }
        }
    }

} // namespace gui
} // namespace heroespath
