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
#include "misc/enum-common.hpp"

#include <string>

namespace heroespath
{
namespace combat
{

    struct TargetType : public EnumBaseCounting<EnumNameOfZeroIsNone>
    {
        enum Enum : EnumUnderlying_t
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

        static constexpr std::string_view Name(const Enum TARGET_TYPE) noexcept
        {
            switch (TARGET_TYPE)
            {
                case None: return "(None)";
                case SingleOpponent: return "Single Opponent";
                case AllOpponents: return "All Opponents";
                case SingleCompanion: return "Single Companion";
                case AllCompanions: return "All Companions";
                case Item: return "Item";
                case QuestSpecific: return "(Quest Specific)";
                case Count: return "Count";
                default: return "combat::TargetType::Name(ENUM=out_of_range)";
            }
        }

        static const std::string ActionPhrase(const Enum);
    };

} // namespace combat
} // namespace heroespath

#endif // HEROESPATH_COMBAT_TARGETENUM_HPP_INCLUDED
