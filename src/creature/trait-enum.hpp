// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_CREATURE_TRAIT_ENUM_HPP_INCLUDED
#define HEROESPATH_CREATURE_TRAIT_ENUM_HPP_INCLUDED
//
// trait-enum.hpp
//
#include "misc/enum-util.hpp"

#include <string>
#include <utility>
#include <vector>

namespace heroespath
{
namespace creature
{

    // Defines one effect an Enchantment can have.
    struct Traits : public misc::EnumBaseCounting<Traits, misc::EnumFirstValue::Valid>
    {
        enum Enum : misc::EnumUnderlying_t
        {
            // Keep stats starting at zero and in order.
            Strength = 0,
            Accuracy,
            Charm,
            Luck,
            Speed,
            Intelligence,
            StatCount,
            Mana = StatCount,
            ArmorRating,
            MagicCast,
            MagicEffect,
            MagicResist,
            Encounter,
            Surprise,
            FindReligious,
            FindMagic,
            FindCoinsAmount,
            Backstab,
            HealthGainMelee,
            HealthGainAll,
            AnimalResist,
            DamageBonusFist,
            DamageBonusMelee,
            DamageBonusProj,
            DamageBonusAll,
            PoisonOnMelee,
            PoisonOnAll,
            PanicOnSpell,
            CurseCast,
            CurseEffect,
            CurseResist,
            CurseOnDamage,
            BlessCast,
            BlessEffect,
            BlessResist,
            HitPower,
            HitCritical,
            FireDamage,
            FireResist,
            FrostDamage,
            FrostResist,
            HonorDamage,
            HonorResist,
            ShadowDamage,
            ShadowResist,
            Count
        };

        static const std::string ToString(const Enum);
        static const std::string Abbr(const Enum);
        static const std::string Name(const Enum);
        static const std::string Desc(const Enum);
    };

    using TraitsVec_t = std::vector<Traits::Enum>;
    using TraitValuePair_t = std::pair<Traits::Enum, int>;
    using TraitValueVec_t = std::vector<TraitValuePair_t>;

} // namespace creature
} // namespace heroespath

#endif // HEROESPATH_CREATURE_TRAIT_ENUM_HPP_INCLUDED
