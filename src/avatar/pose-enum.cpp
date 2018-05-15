// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// pose-enum.cpp
//
#include "pose-enum.hpp"

#include <exception>
#include <sstream>

namespace heroespath
{
namespace avatar
{

    const std::string Pose::ToString(const Pose::Enum E)
    {
        switch (E)
        {
            case Standing:
            {
                return "Standing";
            }
            case Walking:
            {
                return "Walking";
            }
            case GiveTake:
            {
                return "GiveTake";
            }
            case Fight:
            {
                return "Fight";
            }
            case Cast:
            {
                return "Cast";
            }
            case Blink:
            {
                return "Blink";
            }
            case Dead:
            {
                return "Dead";
            }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "avatar::Pose::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }
} // namespace avatar
} // namespace heroespath
