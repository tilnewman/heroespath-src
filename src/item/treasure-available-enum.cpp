// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// treasure-available-state.cpp
//
#include "treasure-available-enum.hpp"

#include <exception>
#include <sstream>

namespace heroespath
{
namespace item
{

    const std::string TreasureAvailable::ToString(const Enum E)
    {
        switch (E)
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
            default:
            {
                std::ostringstream ss;
                ss << "item::TreasureAvailable::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }
} // namespace item
} // namespace heroespath
