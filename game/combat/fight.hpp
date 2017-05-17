#ifndef GAME_COMBAT_FIGHT_HPP_INCLUDED
#define GAME_COMBAT_FIGHT_HPP_INCLUDED
//
// fight.hpp
//
#include "game/combat/fight-results.hpp"

#include <memory>


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

        static const FightResult Fight(creature::CreaturePtrC_t creatureAttackingPtrC,
                                       creature::CreaturePtrC_t creatureDefendingPtrC,
                                       const bool               WILL_FORCE_HIT = false);

        static const creature::ConditionEnumVec_t HandleDamage(creature::CreaturePtrC_t creatureDefendingPtrC,
                                                               HitInfoVec_t &           hitInfoVec,
                                                               const stats::Health_t    TOTAL_DAMAGE);

        static const FightResult Cast(spell::SpellCPtrC_t        SPELL_CPTRC,
                                      creature::CreaturePtrC_t   creatureCastingPtrC,
                                      creature::CreaturePVec_t & creaturesDefendingPVec);

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
