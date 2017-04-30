#ifndef HEROESPATH_COMBAT_TURNDECIDER_HPP_INCLUDED
#define HEROESPATH_COMBAT_TURNDECIDER_HPP_INCLUDED
//
// turn-decider.hpp
//
#include "heroespath/combat/turn-action-enum.hpp"
#include "heroespath/combat/strategy-enums.hpp"
#include "heroespath/combat/strategy-info.hpp"
#include "heroespath/combat/turn-action-info.hpp"
#include "heroespath/spell/spell-type-enum.hpp"

#include <memory>
#include <vector>


namespace heroespath
{
namespace spell
{
    class Spell;
    using SpellPtr_t = Spell *;
}
namespace creature
{
    //forward declarations
    class Creature;
    using CreaturePtr_t   = Creature *;
    using CreatureCPtr_t  = const Creature *;
    using CreaturePtrC_t  = Creature * const;
    using CreatureCPtrC_t = const Creature * const;
    using CreaturePVec_t  = std::vector<CreaturePtr_t>;
}
namespace combat
{

    //forward declarations
    class TurnInfo;

    class TurnDecider;
    using TurnDeciderSPtr_t = std::shared_ptr<TurnDecider>;

    class CombatDisplay;
    using CombatDisplayPtr_t   = CombatDisplay *;
    using CombatDisplayCPtr_t  = const CombatDisplay *;
    using CombatDisplayPtrC_t  = CombatDisplay * const;
    using CombatDisplayCPtrC_t = const CombatDisplay * const;


    //Singletone that is responsible for deciding what a non-player creature will do during its turn in combat
    //TODO change this into a struct of static functions since the singleton pattern is being abused here and there is no state! -zTn 2017-4-3
    class TurnDecider
    {
        TurnDecider() = delete;

    public:
        static const TurnActionInfo Decide(const creature::CreaturePtrC_t CREATURE_DECIDING_CPTRC,
                                           CombatDisplayCPtrC_t           COMBAT_DISPLAY_CPTRC);

    private:
        static creature::CreaturePtr_t FindMostDesiredTarget(const TurnInfo &               TURN_INFO,
                                                             const creature::CreaturePtrC_t CREATURE_DECIDING_CPTRC,
                                                             CombatDisplayCPtrC_t           COMBAT_DISPLAY_CPTRC);

        static creature::CreaturePVec_t FindSelectedTargets(const TurnInfo & TURN_INFO);

        static creature::CreaturePVec_t RefineTargets(const creature::CreaturePVec_t &, const TurnInfo &);

        static float ChanceFromFrequency(const strategy::FrequencyType::Enum);

        static const TurnActionInfo DecideIfRetreating(const TurnInfo &               TURN_INFO,
                                                       const creature::CreaturePtrC_t CREATURE_DECIDING_CPTRC,
                                                       CombatDisplayCPtrC_t           COMBAT_DISPLAY_CPTRC,
                                                       const std::size_t              NUM_PLAYERS_IN_MELEE_RANGE,
                                                       const std::size_t              NUM_FELLOWS_IN_MELEE_RANGE);

        static const TurnActionInfo DecideIfCasting(const TurnInfo &               TURN_INFO,
                                                    const creature::CreaturePtrC_t CREATURE_DECIDING_CPTRC,
                                                    const creature::CreaturePtrC_t MOST_DESIRED_TARGET_CPTRC);

        static const TurnActionInfo DecideIfMovingTowardsMostDesiredTarget(const TurnInfo &               TURN_INFO,
                                                                           const creature::CreaturePtrC_t CREATURE_DECIDING_CPTRC,
                                                                           CombatDisplayCPtrC_t           COMBAT_DISPLAY_CPTRC,
                                                                           const int                      MOST_DESIRED_TARGET_DISTANCE,
                                                                           const bool                     NUM_PLAYERS_IN_ATTACK_RANGE,
                                                                           const bool                     CAN_ATTACK_MOST_DESIRED_TARGET_WITH);

        static const TurnActionInfo DecideIfDoingBeastAction(const TurnInfo &               TURN_INFO,
                                                             const creature::CreaturePtrC_t CREATURE_DECIDING_CPTRC,
                                                             const creature::CreaturePtrC_t MOST_DESIRED_TARGET_CPTRC,
                                                             const creature::CreaturePVec_t PLAYERS_IN_MELEE_RANGE_PVEC,
                                                             const int                      MOST_DESIRED_TARGET_CREATURE_DISTANCE);

        static const TurnActionInfo ForcePickSpellToCast(const creature::CreaturePtrC_t CREATURE_DECIDING_CPTRC,
                                                         const creature::CreaturePtrC_t MOST_DESIRED_TARGET_CPTRC);

        static const TurnActionInfo DecideSpell(const creature::CreaturePtrC_t CREATURE_DECIDING_CPTRC,
                                                const creature::CreaturePtrC_t MOST_DESIRED_TARGET_CPTRC,
                                                const spell::SpellTypeVec_t    SPELL_TYPES_VEC);


        static spell::SpellPtr_t PickSpell(const creature::CreaturePtrC_t CREATURE_DECIDING_CPTRC,
                                           const spell::SpellType::Enum   SPELL_TYPE);

        static spell::SpellPtr_t PickSpell(const creature::CreaturePtrC_t CREATURE_DECIDING_CPTRC,
                                           const spell::SpellTypeVec_t    SPELL_TYPES_VEC);

        static const TurnActionInfo DecideIfFlying(const TurnInfo &               TURN_INFO,
                                                   const creature::CreaturePtrC_t CREATURE_DECIDING_CPTRC);

    private:
        static const spell::SpellTypeVec_t COMBAT_SPELLS_VEC_;
        static const spell::SpellTypeVec_t AFFLICTION_SPELLS_VEC_;
        static const spell::SpellTypeVec_t BENEFICIAL_SPELLS_VEC_;
    };

}
}

#endif //HEROESPATH_COMBAT_TURNDECIDER_HPP_INCLUDED
