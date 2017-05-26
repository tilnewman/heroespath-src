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
    using SpellCPtrC_t = const Spell * const;
}
namespace item
{
    class Item;
    using ItemSPtr_t = std::shared_ptr<Item>;
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


        static stats::Stat_t IsValuetHigherThanRatioOfStat(const stats::Stat_t STAT_VALUE,
                                                           const stats::Stat_t STAT_MAX,
                                                           const float         RATIO);


        static const FightResult Fight(creature::CreaturePtrC_t creatureAttackingPtrC,
                                       creature::CreaturePtrC_t creatureDefendingPtrC,
                                       const bool               WILL_FORCE_HIT = false);

        static const creature::ConditionEnumVec_t HandleDamage(creature::CreaturePtrC_t creatureDefendingPtrC,
                                                               HitInfoVec_t &           hitInfoVec,
                                                               const stats::Health_t    TOTAL_DAMAGE);

        static const FightResult Cast(const spell::SpellPtr_t          SPELL_CPTR,
                                      creature::CreaturePtrC_t         creatureCastingPtrC,
                                      const creature::CreaturePVec_t & creaturesDefendingPVec);

        static const FightResult Pounce(creature::CreaturePtrC_t creaturePouncingPtrC,
                                        creature::CreaturePtrC_t creatureDefendingPtrC);

        static const FightResult Roar(creature::CreaturePtrC_t creatureRoaringPtrC,
                                      CombatDisplayCPtrC_t     COMBAT_DISPLAY_CPTRC);

        static creature::CreaturePtr_t FindNonPlayerCreatureToAttack(creature::CreaturePtrC_t creatureAttackingtrC,
                                                                     CombatDisplayCPtrC_t     COMBAT_DISPLAY_CPTRC);

    private:
        static const HitInfoVec_t AttackWithAllWeapons(creature::CreaturePtrC_t creatureAttackingPtrC,
                                                       creature::CreaturePtrC_t creatureDefendingPtrC,
                                                       const bool               WILL_FORCE_HIT = false);

        static const HitInfo AttackWithSingleWeapon(HitInfoVec_t &           hitInfoVec,
                                                    const item::ItemSPtr_t & WEAPON_SPTR,
                                                    creature::CreaturePtrC_t creatureAttackingPtrC,
                                                    creature::CreaturePtrC_t creatureDefendingPtrC,
                                                    const bool               WILL_FORCE_HIT = false);

        static const CreatureEffect CastSpellUpon(const spell::SpellPtr_t              SPELL_CPTR,
                                                  const std::string &                  EFFECT_STR,
                                                  creature::CreaturePtrC_t             creatureCastingPtrC,
                                                  creature::CreaturePtrC_t             creatureCastUponPtrC,
                                                  const stats::Health_t                HEALTH_ADJ,
                                                  const creature::ConditionEnumVec_t & CONDITIONS_VEC);

        static stats::Health_t DetermineDamage(const item::ItemSPtr_t & WEAPON_SPTR,
                                               creature::CreaturePtrC_t creatureAttackingPtrC,
                                               creature::CreaturePtrC_t creatureDefendingPtrC,
                                               bool &                   isPowerHit_OutParam,
                                               bool &                   isCriticalHit_OutParam);

        static bool IsConditionContained(const creature::Conditions::Enum E, const HitInfoVec_t & HIT_INFO_VEC);
    };

}
}

#endif //GAME_COMBAT_FIGHT_HPP_INCLUDED
