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

#include "misc/log-macros.hpp"

namespace heroespath
{
namespace avatar
{

    const std::string Pose::ToString(const Pose::Enum ENUM)
    {
        switch (ENUM)
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

} // namespace avatar
} // namespace heroespath
