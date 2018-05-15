// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// orientation.cpp
//
#include "orientation-enum.hpp"
#include <exception>
#include <sstream>

namespace heroespath
{
namespace sfml_util
{

    const std::string Orientation::ToString(const Orientation::Enum E)
    {
        if (E == Orientation::Horiz)
        {
            return "Horizontal";
        }
        else
        {
            if (E == Orientation::Vert)
            {
                return "Vertical";
            }
            else
            {
                std::ostringstream ss;
                ss << "sfml_util::Orientation::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }

    bool Orientation::IsValid(const Orientation::Enum E)
    {
        return ((E == Orientation::Horiz) || (E == Orientation::Vert));
    }
} // namespace sfml_util
} // namespace heroespath
