///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software.  If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef HEROESPATH_COMBAT_FIGHT_HPP_INCLUDED
#define HEROESPATH_COMBAT_FIGHT_HPP_INCLUDED
//
// fight.hpp
//
#include "combat/fight-results.hpp"
#include "misc/not-null.hpp"
#include "misc/types.hpp"

#include <memory>
#include <string>

namespace heroespath
{
namespace spell
{
    class Spell;
    using SpellPtr_t = misc::NotNull<Spell *>;
} // namespace spell
namespace song
{
    class Song;
    using SongPtr_t = misc::NotNull<Song *>;
} // namespace song
namespace item
{
    class Item;
    using ItemPtr_t = misc::NotNull<Item *>;
} // namespace item
namespace creature
{
    class Creature;
    using CreaturePtr_t = misc::NotNull<Creature *>;
} // namespace creature
namespace combat
{

    class Trap;

    class CombatDisplay;
    using CombatDisplayPtr_t = misc::NotNull<CombatDisplay *>;

    // all the functions required to allow two creatures to engage in one turn of fighting
    struct FightClub
    {
        static stats::Trait_t IsValuetHigherThanRatioOfStat(
            const stats::Trait_t STAT_VALUE, const stats::Trait_t STAT_MAX, const float RATIO);

        static const FightResult Fight(
            const creature::CreaturePtr_t CREATURE_ATTACKING_PTR,
            const creature::CreaturePtr_t CREATURE_DEFENDING_PTR,
            const bool WILL_FORCE_HIT = false);

        // negative values are damaging, positive values are healing
        static void HandleDamage(
            const creature::CreaturePtr_t CREATURE_DEFENDING_PTR,
            HitInfoVec_t & hitInfoVec,
            const Health_t & HEALTH_ADJ,
            creature::CondEnumVec_t & condsAddedVec,
            creature::CondEnumVec_t & condsRemovedVec,
            const bool CAN_ADD_CONDITIONS = true);

        static void AddConditionsBasedOnDamage(
            const creature::CreaturePtr_t CREATURE_DEFENDING_PTR,
            const Health_t & DAMAGE_ABS,
            creature::CondEnumVec_t & condsAddedVec,
            creature::CondEnumVec_t & condsRemovedVec,
            HitInfoVec_t & hitInfoVec);

        // returns true if any conditions were removed from the creature,
        // not from the hitInfoVec
        static bool RemoveAddedConditions(
            const creature::CondEnumVec_t & CONDS_VEC,
            const creature::CreaturePtr_t CREATURE_PTR,
            HitInfoVec_t & hitInfoVec,
            creature::CondEnumVec_t & condsRemovedVec);

        // returns true if a condition wwas removed from the creature,
        // not from the hitInfoVec
        static bool RemoveAddedCondition(
            const creature::Conditions::Enum COND_ENUM,
            const creature::CreaturePtr_t CREATURE_PTR,
            HitInfoVec_t & hitInfoVec,
            creature::CondEnumVec_t & condsRemovedVec);

        static const FightResult Cast(
            const spell::SpellPtr_t SPELL_PTR,
            const creature::CreaturePtr_t CREATURE_CASTING_PTR,
            const creature::CreaturePVec_t & creaturesDefendingPVec);

        static const FightResult PlaySong(
            const song::SongPtr_t SONG_PTR,
            const creature::CreaturePtr_t CREATURE_PLAYING_PTR,
            const creature::CreaturePVec_t & creaturesListeningPVec);

        static const FightResult Pounce(
            const creature::CreaturePtr_t CREATURE_POUNCING_PTR,
            const creature::CreaturePtr_t CREATURE_DEFENDING_PTR);

        static const FightResult Roar(
            const creature::CreaturePtr_t CREATURE_ROARING_PTR,
            const CombatDisplayPtr_t COMBAT_DISPLAY_PTR);

        static const creature::CreaturePtr_t FindNonPlayerCreatureToAttack(
            const creature::CreaturePtr_t CREATURE_ATTACKING_PTR,
            const CombatDisplayPtr_t COMBAT_DISPLAY_PTR);

        static const FightResult TreasureTrap(
            const combat::Trap & TRAP, const creature::CreaturePtr_t CREATURE_PICKING_THE_LOCK_PTR);

    private:
        static const creature::CreaturePVec_t RandomSelectWhoIsHurtByTrap(
            const combat::Trap & TRAP, const creature::CreaturePtr_t CREATURE_PICKING_THE_LOCK_PTR);

        static const HitInfoVec_t AttackWithAllWeapons(
            const creature::CreaturePtr_t CREATURE_ATTACKING_PTR,
            const creature::CreaturePtr_t CREATURE_DEFENDING_PTR,
            const bool WILL_FORCE_HIT = false);

        static const HitInfo AttackWithSingleWeapon(
            HitInfoVec_t & hitInfoVec,
            const item::ItemPtr_t WEAPON_PTR,
            const creature::CreaturePtr_t CREATURE_ATTACKING_PTR,
            const creature::CreaturePtr_t CREATURE_DEFENDING_PTR,
            const bool WILL_FORCE_HIT = false);

        static Health_t DetermineDamage(
            const item::ItemPtr_t WEAPON_PTR,
            const creature::CreaturePtr_t CREATURE_ATTACKING_PTR,
            const creature::CreaturePtr_t CREATURE_DEFENDING_PTR,
            bool & isPowerHit_OutParam,
            bool & isCriticalHit_OutParam,
            bool & didArmorAbsorb_OutParam);

        static bool AreAnyOfCondsContained(
            const creature::CondEnumVec_t & CONDS_VEC,
            const creature::CreaturePtr_t CREATURE_PTR,
            const HitInfoVec_t & HIT_INFO_VEC);
    };

} // namespace combat
} // namespace heroespath

#endif // HEROESPATH_COMBAT_FIGHT_HPP_INCLUDED
