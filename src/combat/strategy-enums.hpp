// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_COMBAT_STRATEGYENUMS_HPP_INCLUDED
#define HEROESPATH_COMBAT_STRATEGYENUMS_HPP_INCLUDED
//
//  strategy-enums.hpp
//
#include "misc/enum-common.hpp"
#include "misc/vector-map.hpp"

#include <string>

namespace heroespath
{
namespace combat
{
    namespace strategy
    {

        struct SelectType : public EnumBaseBitField<SeparatorVaries>
        {
            enum Enum : EnumUnderlying_t
            {
                None = 0,
                Pixie = 1 << 0,
                Dragon = 1 << 1,
                Human = 1 << 2,
                Gnome = 1 << 3,
                Wolfen = 1 << 4,
                Archer = 1 << 5,
                Sorcerer = 1 << 6,
                Knight = 1 << 7,
                Beastmaster = 1 << 8,
                Cleric = 1 << 9,
                Thief = 1 << 10,
                Bard = 1 << 11,
                FireBrand = 1 << 12,
                Sylavin = 1 << 13,
                Projectile = 1 << 14,
                Caster = 1 << 15,
                CanFly = 1 << 16,
                CantFly = 1 << 17,
                Beast = 1 << 18,
                NotBeast = 1 << 19
            };

            static constexpr Enum Last = NotBeast;

            static constexpr std::array<std::string_view, 20> TO_STRINGS {
                "Pixie",      "Dragon",      "Human",  "Gnome",   "Wolfen", "Archer",    "Sorcerer",
                "Knight",     "Beastmaster", "Cleric", "Thief",   "Bard",   "FireBrand", "Sylavin",
                "Projectile", "Caster",      "CanFly", "CantFly", "Beast",  "NotBeast"
            };
        };

        struct RefineType : public EnumBaseBitField<SeparatorVaries>
        {
            enum Enum : EnumUnderlying_t
            {
                None = 0,

                // selects whoever is unconscious
                Murderer = 1 << 0,

                // selects whoever has the least health current
                Bloodthirsty = 1 << 1,

                // selects whoever can't fight back
                Coward = 1 << 2,

                // selects whoever first hits him/her/it
                Hit = 1 << 3,

                // selects whoever first attacks him/her/it
                Attack = 1 << 4,

                // selects whoever was last to play a song/music/bard-spell
                MusicMaker = 1 << 5,

                // selects whoever casts spells
                Caster = 1 << 6,

                // selects whoever is enchanted
                Enchanted = 1 << 7,
                // selects whoever is NOT enchanted
                NotEnchanted = 1 << 8,

                // won't change selected target until they die
                Steadfast = 1 << 9,

                // will change selection/target to whoever last triggers the refinement type
                LastTo = 1 << 10,

                // selects whoever is doing the most damage
                MostDamage = 1 << 11
            };

            static constexpr Enum Last = MostDamage;

            static constexpr std::array<std::string_view, 12> TO_STRINGS {
                "Murderer", "Bloodthirsty", "Coward",       "Hit",       "Attack", "MusicMaker",
                "Caster",   "Enchanted",    "NotEnchanted", "Steadfast", "LastTo", "MostDamage"
            };
        };

        struct AdvanceType : public EnumBaseCounting<EnumNameOfZeroIsNone>
        {
            enum Enum : EnumUnderlying_t
            {
                None = 0, // advances toward most desired target but stops whenever able to attack
                Cowardly, // Never advances
                Hesitant, // only advances if being hit and can't hit back
                Charger, // always advances until reaching most desired target
                Count
            };
        };

        struct RetreatType : public EnumBaseCounting<EnumNameOfZeroIsNone>
        {
            enum Enum : EnumUnderlying_t
            {
                None = 0, // never retreats
                Wary, // retreats if melee is outnumbered
                Coward, // retreats if there are any opponents able to attack in the melee
                Count
            };
        };

        struct FrequencyType : public EnumBaseCounting<EnumNameOfZeroIsdNever>
        {
            enum Enum : EnumUnderlying_t
            {
                Never = 0,
                Once,
                Twice,
                Thrice,
                Rarely,
                Seldom,
                Neutral,
                Commonly,
                Often,
                Always,
                Count
            };
        };

        using SelectChanceMap_t = misc::VectorMap<SelectType::Enum, float>;
        using RefineChanceMap_t = misc::VectorMap<RefineType::Enum, float>;
        using AdvanceChanceMap_t = misc::VectorMap<AdvanceType::Enum, float>;
        using RetreatChanceMap_t = misc::VectorMap<RetreatType::Enum, float>;
        using OutnumberRetreatChanceMap_t = misc::VectorMap<std::size_t, float>;
        using FrequencyChanceMap_t = misc::VectorMap<FrequencyType::Enum, float>;

    } // namespace strategy
} // namespace combat
} // namespace heroespath

#endif // HEROESPATH_COMBAT_STRATEGYENUMS_HPP_INCLUDED
