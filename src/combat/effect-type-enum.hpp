// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_COMBAT_EFFECTTYPEENUM_HPP_INCLUDED
#define HEROESPATH_COMBAT_EFFECTTYPEENUM_HPP_INCLUDED
//
// effect-type-enum.hpp
//
#include "misc/enum-common.hpp"

#include <string>
#include <vector>

namespace heroespath
{
namespace combat
{

    struct EffectType : public EnumBaseCounting<>
    {
        enum Enum : EnumUnderlying_t
        {
            CreatureHarmDamage = 0,
            CreatureHarmMisc,
            CreatureHelpHeal,
            CreatureHelpMisc,
            ItemHarmBreak,
            ItemHarmMisc,
            ItemHelpFix,
            ItemHelpMisc,
            Misc,
            Count
        };

        static constexpr std::string_view Name(const Enum ENUM) noexcept
        {
            switch (ENUM)
            {
                case CreatureHarmDamage: return "Creature Damage";
                case CreatureHarmMisc: return "Creature Harm";
                case CreatureHelpHeal: return "Creature Heal";
                case CreatureHelpMisc: return "Creature Help";
                case ItemHarmBreak: return "Item Break";
                case ItemHarmMisc: return "Item Harm";
                case ItemHelpFix: return "Item Fix";
                case ItemHelpMisc: return "Item Help";
                case Misc: return "Misc";
                case Count: return "Count";
                default: return "combat::EffectType::Name(ENUM=out_of_range)";
            }
        }
    };

    using EffectTypeVec_t = std::vector<EffectType::Enum>;

} // namespace combat
} // namespace heroespath

#endif // HEROESPATH_COMBAT_EFFECTTYPEENUM_HPP_INCLUDED
