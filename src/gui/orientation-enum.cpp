// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// orientation-enum.cpp
//
#include "orientation-enum.hpp"

#include "misc/log-macros.hpp"

namespace heroespath
{
namespace gui
{

    const std::string Orientation::ToString(const Orientation::Enum ORIENTATION)
    {
        if (ORIENTATION == Orientation::Horiz)
        {
            return "Horiz";
        }
        else if (ORIENTATION == Orientation::Vert)
        {
            return "Vert";
        }
        else if (ORIENTATION == Orientation::Both)
        {
            return "HorizAndVert";
        }
        else if (ORIENTATION == Orientation::Count)
        {
            return "(Count)";
        }
        else
        {
            M_HP_LOG_ERR(ValueOutOfRangeErrorString(ORIENTATION));
            return "";
        }
    }

} // namespace gui
} // namespace heroespath
