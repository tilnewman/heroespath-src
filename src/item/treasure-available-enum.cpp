// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// treasure-available-enum.cpp
//
#include "treasure-available-enum.hpp"

#include "misc/log-macros.hpp"

namespace heroespath
{
namespace item
{

    const std::string TreasureAvailable::ToString(const Enum ENUM)
    {
        switch (ENUM)
        {
            case NoTreasure:
            {
                return "NoTreasure";
            }
            case HeldOnly:
            {
                return "HeldOnly";
            }
            case LockboxOnly:
            {
                return "LockboxOnly";
            }
            case HeldAndLockbox:
            {
                return "HeldAndLockbox";
            }
            case Count:
            {
                return "(Count)";
            }
            default:
            {
                M_HP_LOG_ERR(ValueOutOfRangeErrorString(ENUM));
                return "";
            }
        }
    }

} // namespace item
} // namespace heroespath
