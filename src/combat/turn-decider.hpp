// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_COMBAT_TURNDECIDER_HPP_INCLUDED
#define HEROESPATH_COMBAT_TURNDECIDER_HPP_INCLUDED
//
// turn-decider.hpp
//
#include "combat/effect-type-enum.hpp"
#include "combat/strategy-enums.hpp"
#include "combat/strategy-info.hpp"
#include "combat/turn-action-enum.hpp"
#include "combat/turn-action-info.hpp"
#include "misc/not-null.hpp"

#include <memory>
#include <vector>

namespace heroespath
{
namespace spell
{
    class Spell;
    using SpellPtr_t = misc::NotNull<Spell *>;
} // namespace spell
namespace creature
{
    // forward declarations
    class Creature;
    using CreaturePtr_t = misc::NotNull<Creature *>;
    using CreaturePVec_t = std::vector<CreaturePtr_t>;
} // namespace creature
namespace combat
{

    // forward declarations
    class TurnInfo;

    class TurnDecider;
    using TurnDeciderSPtr_t = std::shared_ptr<TurnDecider>;

    class CombatDisplay;
    using CombatDisplayPtr_t = misc::NotNull<CombatDisplay *>;

    // Responsible for deciding what a non-player creature will do during its
    // turn in combat
    class TurnDecider
    {
    public:
        TurnDecider(const TurnDecider &) = delete;
        TurnDecider(TurnDecider &&) = delete;
        TurnDecider & operator=(const TurnDecider &) = delete;
        TurnDecider & operator=(TurnDecider &&) = delete;

        TurnDecider() = default;

        const TurnActionInfo Decide(
            const creature::CreaturePtr_t CREATURE_DECIDING_PTR,
            const CombatDisplayPtr_t COMBAT_DISPLAY_PTR) const;

    private:
        creature::CreaturePtr_t FindMostDesiredTarget(
            const TurnInfo & CREATURE_DECIDING_TURN_INFO,
            const creature::CreaturePtr_t CREATURE_DECIDING_PTR,
            const CombatDisplayPtr_t COMBAT_DISPLAY_PTR) const;

        creature::CreaturePVec_t FindSelectedTargets(const TurnInfo & TURN_INFO) const;

        creature::CreaturePVec_t
            RefineTargets(const creature::CreaturePVec_t &, const TurnInfo &) const;

        float ChanceFromFrequency(const strategy::FrequencyType::Enum) const;

        const TurnActionInfo DecideIfRetreating(
            const TurnInfo & TURN_INFO,
            const creature::CreaturePtr_t CREATURE_DECIDING_PTR,
            const CombatDisplayPtr_t COMBAT_DISPLAY_PTR,
            const std::size_t NUM_PLAYERS_IN_MELEE_RANGE,
            const std::size_t NUM_FELLOWS_IN_MELEE_RANGE) const;

        const TurnActionInfo DecideIfCasting(
            const TurnInfo & TURN_INFO,
            const creature::CreaturePtr_t CREATURE_DECIDING_PTR,
            const creature::CreaturePtr_t MOST_DESIRED_TARGET_PTR) const;

        const TurnActionInfo DecideIfMovingTowardsMostDesiredTarget(
            const TurnInfo & TURN_INFO,
            const creature::CreaturePtr_t CREATURE_DECIDING_PTR,
            const CombatDisplayPtr_t COMBAT_DISPLAY_PTR,
            const int MOST_DESIRED_TARGET_DISTANCE,
            const bool NUM_PLAYERS_IN_ATTACK_RANGE,
            const bool CAN_ATTACK_MOST_DESIRED_TARGET_WITH) const;

        const TurnActionInfo DecideIfDoingBeastAction(
            const TurnInfo & TURN_INFO,
            const creature::CreaturePtr_t CREATURE_DECIDING_PTR,
            const creature::CreaturePtr_t MOST_DESIRED_TARGET_PTR,
            const creature::CreaturePVec_t & PLAYERS_IN_MELEE_RANGE_PVEC,
            const int MOST_DESIRED_TARGET_CREATURE_DISTANCE) const;

        const TurnActionInfo ForcePickSpellToCast(
            const creature::CreaturePtr_t CREATURE_DECIDING_PTR,
            const creature::CreaturePtr_t MOST_DESIRED_TARGET_PTR) const;

        const TurnActionInfo DecideSpell(
            const creature::CreaturePtr_t CREATURE_DECIDING_PTR,
            const creature::CreaturePtr_t MOST_DESIRED_TARGET_PTR,
            const EffectTypeVec_t & SPELL_TYPES_VEC) const;

        const spell::SpellPtr_t PickSpell(
            const creature::CreaturePtr_t CREATURE_DECIDING_PTR,
            const combat::EffectType::Enum SPELL_TYPE) const;

        const spell::SpellPtr_t PickSpell(
            const creature::CreaturePtr_t CREATURE_DECIDING_PTR,
            const EffectTypeVec_t & SPELL_TYPES_VEC) const;

        const TurnActionInfo DecideIfFlying(
            const TurnInfo & TURN_INFO, const creature::CreaturePtr_t CREATURE_DECIDING_PTR) const;

        const TurnActionInfo DecideIfAdvancingTowardNonMostDesiredTarget(
            const TurnInfo & CREATURE_DECIDING_TURN_INFO,
            const creature::CreaturePtr_t CREATURE_DECIDING_PTR,
            const CombatDisplayPtr_t COMBAT_DISPLAY_PTR) const;

        void AdjustCreatueVecForMurderousIntent(
            const TurnInfo & CREATURE_DECIDING_TURN_INFO,
            creature::CreaturePVec_t & pVec_OutParam) const;

    private:
        static const EffectTypeVec_t HARM_EFFECTTYPES_VEC_;
        static const EffectTypeVec_t HELP_EFFECTTYPES_VEC_;
    };

} // namespace combat
} // namespace heroespath

#endif // HEROESPATH_COMBAT_TURNDECIDER_HPP_INCLUDED
