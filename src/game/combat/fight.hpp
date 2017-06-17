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
#ifndef GAME_COMBAT_FIGHT_HPP_INCLUDED
#define GAME_COMBAT_FIGHT_HPP_INCLUDED
//
// fight.hpp
//
#include "game/combat/fight-results.hpp"

#include <memory>
#include <string>


namespace game
{
namespace spell
{
    class Spell;
    using SpellPtr_t = Spell *;
}
namespace song
{
    class Song;
    using SongPtr_t = Song *;
}
namespace item
{
    class Item;
    using ItemPtr_t = Item *;
}
namespace creature
{
    class Creature;
    using CreaturePtrC_t = Creature * const;
    using CreaturePtr_t  = Creature *;
}
namespace combat
{

    //forward declarations
    class CombatDisplay;
    using CombatDisplayPtr_t   = CombatDisplay *;
    using CombatDisplayCPtr_t  = const CombatDisplay *;
    using CombatDisplayPtrC_t  = CombatDisplay * const;
    using CombatDisplayCPtrC_t = const CombatDisplay * const;


    //all the functions required to allow two creatures to engage in one turn of fighting
    struct FightClub
    {
        static const stats::Stat_t STAT_HIGHER_THAN_AVERAGE_;
        static const float STAT_RATIO_AMAZING_;

        static stats::Stat_t IsValuetHigherThanRatioOfStat(
            const stats::Stat_t STAT_VALUE,
            const stats::Stat_t STAT_MAX,
            const float         RATIO);

        static const FightResult Fight(
            creature::CreaturePtrC_t creatureAttackingPtrC,
            creature::CreaturePtrC_t creatureDefendingPtrC,
            const bool               WILL_FORCE_HIT = false);

        //negative values are damaging, positive values are healing
        static void HandleDamage(
            creature::CreaturePtrC_t       creatureDefendingPtrC,
            HitInfoVec_t &                 hitInfoVec,
            const stats::Health_t          HEALTH_ADJ,
            creature::CondEnumVec_t & condsAddedVec,
            creature::CondEnumVec_t & condsRemovedVec);

        static void AddConditionsBasedOnDamage(
            creature::CreaturePtrC_t       creatureDefendingPtrC,
            const stats::Health_t          DAMAGE_ABS,
            creature::CondEnumVec_t & condsAddedVec);

        static void RemoveAddedCondition(
            const creature::Conditions::Enum COND_ENUM,
            creature::CreaturePtrC_t         creaturePtrC,
            HitInfoVec_t &                   hitInfoVec,
            creature::CondEnumVec_t &   condsRemovedVec);

        static const FightResult Cast(
            const spell::SpellPtr_t          SPELL_CPTR,
            creature::CreaturePtrC_t         creatureCastingPtrC,
            const creature::CreaturePVec_t & creaturesDefendingPVec);

        static const FightResult PlaySong(
            const song::SongPtr_t           SONG_CPTR,
            creature::CreaturePtrC_t         creaturePlayingPtrC,
            const creature::CreaturePVec_t & creaturesListeningPVec);

        static const FightResult Pounce(
            creature::CreaturePtrC_t creaturePouncingPtrC,
            creature::CreaturePtrC_t creatureDefendingPtrC);

        static const FightResult Roar(
            creature::CreaturePtrC_t creatureRoaringPtrC,
            CombatDisplayCPtrC_t     COMBAT_DISPLAY_CPTRC);

        static creature::CreaturePtr_t FindNonPlayerCreatureToAttack(
            creature::CreaturePtrC_t creatureAttackingtrC,
            CombatDisplayCPtrC_t     COMBAT_DISPLAY_CPTRC);

    private:
        static const HitInfoVec_t AttackWithAllWeapons(
            creature::CreaturePtrC_t creatureAttackingPtrC,
            creature::CreaturePtrC_t creatureDefendingPtrC,
            const bool               WILL_FORCE_HIT = false);

        static const HitInfo AttackWithSingleWeapon(
            HitInfoVec_t &           hitInfoVec,
            const item::ItemPtr_t    WEAPON_PTR,
            creature::CreaturePtrC_t creatureAttackingPtrC,
            creature::CreaturePtrC_t creatureDefendingPtrC,
            const bool               WILL_FORCE_HIT = false);

        static stats::Health_t DetermineDamage(
            const item::ItemPtr_t    WEAPON_PTR,
            creature::CreaturePtrC_t creatureAttackingPtrC,
            creature::CreaturePtrC_t creatureDefendingPtrC,
            bool &                   isPowerHit_OutParam,
            bool &                   isCriticalHit_OutParam);

        static bool IsCondContainedInAddedConds(
            const creature::Conditions::Enum E,
            const HitInfoVec_t &             HIT_INFO_VEC);
    };

}
}

#endif //GAME_COMBAT_FIGHT_HPP_INCLUDED
