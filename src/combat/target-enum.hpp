// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_COMBAT_TARGETENUM_HPP_INCLUDED
#define HEROESPATH_COMBAT_TARGETENUM_HPP_INCLUDED
//
// target-enum.hpp
//
#include "misc/enum-util.hpp"

#include <string>

namespace heroespath
{
namespace combat
{

    struct TargetType : public misc::EnumBaseCounting<TargetType, misc::EnumFirstValueNone>
    {
        enum Enum : misc::EnumUnderlying_t
        {
            None = 0,
            SingleOpponent,
            AllOpponents,
            SingleCompanion,
            AllCompanions,
            Item,
            QuestSpecific,
            Count
        };

        static const std::string ToString(const Enum);
        static const std::string Name(const Enum);
        static const std::string ActionPhrase(const Enum);
    };

} // namespace combat
} // namespace heroespath

#endif // HEROESPATH_COMBAT_TARGETENUM_HPP_INCLUDED
