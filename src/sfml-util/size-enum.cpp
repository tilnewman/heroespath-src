// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// size-enum.cpp
//
#include "size-enum.hpp"

#include <exception>
#include <sstream>

namespace heroespath
{
namespace gui
{
    const std::string Size::ToString(const Size::Enum SIZE_ENUM)
    {
        switch (SIZE_ENUM)
        {
            case Small:
            {
                return "Small";
            }
            case Medium:
            {
                return "Medium";
            }
            case Large:
            {
                return "Large";
            }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "gui::Size::ToString(" << SIZE_ENUM << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }
} // namespace gui
} // namespace heroespath
