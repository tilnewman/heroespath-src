// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// brightness.cpp
//
#include "brightness-enum.hpp"

#include <exception>
#include <sstream>

namespace heroespath
{
namespace sfml_util
{

    const std::string Brightness::ToString(const Brightness::Enum E)
    {
        switch (E)
        {
            case Bright:
            {
                return "Bright";
            }
            case Medium:
            {
                return "Medium";
            }
            case Dark:
            {
                return "Dark";
            }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "sfml_util::Brightness::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }

    bool Brightness::IsValid(const Brightness::Enum E)
    {
        switch (E)
        {
            case Bright:
            case Medium:
            case Dark:
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
