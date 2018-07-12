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
#include "misc/enum-util.hpp"
#include "misc/vector-map.hpp"

#include <string>

namespace heroespath
{
namespace combat
{
    namespace strategy
    {

        struct SelectType : public misc::EnumBaseBitField<SelectType>
        {
            enum Enum : misc::EnumUnderlying_t
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
                Theif = 1 << 10,
                Bard = 1 << 11,
                FireBrand = 1 << 12,
                Sylavin = 1 << 13,
                Projectile = 1 << 14,
                Caster = 1 << 15,
                CanFly = 1 << 16,
                CantFly = 1 << 17,
                Beast = 1 << 18,
                NotBeast = 1 << 19,
                Last = NotBeast
            };

            static void ToStringPopulate(
                std::ostringstream & ss,
                const misc::EnumUnderlying_t ENUM_VALUE,
                const std::string & SEPARATOR);
        };

        struct RefineType : public misc::EnumBaseBitField<RefineType>
        {
            enum Enum : misc::EnumUnderlying_t
            {
                None = 0,
                Murderer = 1 << 0, // selects whoever is unconcious
                Bloodthirsty = 1 << 1, // selects whoever has the least health current
                Coward = 1 << 2, // selects whoever can't fight back
                Hit = 1 << 3, // selects whoever first hits him/her/it
                Attack = 1 << 4, // selects whoever first attacks him/her/it
                MusicMaker = 1 << 5, // selects whoever was last to play a song/music/bard-spell
                Caster = 1 << 6, // selects whoever casts spells
                Enchanted = 1 << 7, // selects whoever is enchanted
                NotEnchanted = 1 << 8, // selects whoever is NOT enchanted
                Steadfast = 1 << 9, // won't change selected target until they die
                LastTo = 1 << 10, // will change selection/target to whoever last triggers the
                                  // refinement type
                MostDamage = 1 << 11, // selects whoever is doing the most damage

                Last = MostDamage
            };

            static void ToStringPopulate(
                std::ostringstream & ss,
                const misc::EnumUnderlying_t ENUM_VALUE,
                const std::string & SEPARATOR);
        };

        struct AdvanceType : public misc::EnumBaseCounting<AdvanceType, misc::EnumFirstValueNone>
        {
            enum Enum : misc::EnumUnderlying_t
            {
                None = 0, // advances toward most desired target but stops whenever able to attack
                Cowardly, // Never advances
                Hesitant, // only advances if being hit and can't hit back
                Charger, // always advances until reaching most desired target
                Count
            };

            static const std::string ToString(const Enum);
        };

        struct RetreatType : public misc::EnumBaseCounting<RetreatType, misc::EnumFirstValueNone>
        {
            enum Enum : misc::EnumUnderlying_t
            {
                None = 0, // never retreats
                Wary, // retreats if melee is outnumbered
                Coward, // retreats if there are any opponents able to attack in the melee
                Count
            };

            static const std::string ToString(const Enum);
        };

        struct FrequencyType
            : public misc::EnumBaseCounting<FrequencyType, misc::EnumFirstValueNever>
        {
            enum Enum : misc::EnumUnderlying_t
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

            static const std::string ToString(const Enum);
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
