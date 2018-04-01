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
        TurnDecider() = delete;

        static const TurnActionInfo Decide(
            const creature::CreaturePtr_t CREATURE_DECIDING_PTR,
            const CombatDisplayPtr_t COMBAT_DISPLAY_PTR);

    private:
        static creature::CreaturePtr_t FindMostDesiredTarget(
            const TurnInfo & CREATURE_DECIDING_TURN_INFO,
            const creature::CreaturePtr_t CREATURE_DECIDING_PTR,
            const CombatDisplayPtr_t COMBAT_DISPLAY_PTR);

        static creature::CreaturePVec_t FindSelectedTargets(const TurnInfo & TURN_INFO);

        static creature::CreaturePVec_t
            RefineTargets(const creature::CreaturePVec_t &, const TurnInfo &);

        static float ChanceFromFrequency(const strategy::FrequencyType::Enum);

        static const TurnActionInfo DecideIfRetreating(
            const TurnInfo & TURN_INFO,
            const creature::CreaturePtr_t CREATURE_DECIDING_PTR,
            const CombatDisplayPtr_t COMBAT_DISPLAY_PTR,
            const std::size_t NUM_PLAYERS_IN_MELEE_RANGE,
            const std::size_t NUM_FELLOWS_IN_MELEE_RANGE);

        static const TurnActionInfo DecideIfCasting(
            const TurnInfo & TURN_INFO,
            const creature::CreaturePtr_t CREATURE_DECIDING_PTR,
            const creature::CreaturePtr_t MOST_DESIRED_TARGET_PTR);

        static const TurnActionInfo DecideIfMovingTowardsMostDesiredTarget(
            const TurnInfo & TURN_INFO,
            const creature::CreaturePtr_t CREATURE_DECIDING_PTR,
            const CombatDisplayPtr_t COMBAT_DISPLAY_PTR,
            const int MOST_DESIRED_TARGET_DISTANCE,
            const bool NUM_PLAYERS_IN_ATTACK_RANGE,
            const bool CAN_ATTACK_MOST_DESIRED_TARGET_WITH);

        static const TurnActionInfo DecideIfDoingBeastAction(
            const TurnInfo & TURN_INFO,
            const creature::CreaturePtr_t CREATURE_DECIDING_PTR,
            const creature::CreaturePtr_t MOST_DESIRED_TARGET_PTR,
            const creature::CreaturePVec_t & PLAYERS_IN_MELEE_RANGE_PVEC,
            const int MOST_DESIRED_TARGET_CREATURE_DISTANCE);

        static const TurnActionInfo ForcePickSpellToCast(
            const creature::CreaturePtr_t CREATURE_DECIDING_PTR,
            const creature::CreaturePtr_t MOST_DESIRED_TARGET_PTR);

        static const TurnActionInfo DecideSpell(
            const creature::CreaturePtr_t CREATURE_DECIDING_PTR,
            const creature::CreaturePtr_t MOST_DESIRED_TARGET_PTR,
            const EffectTypeVec_t & SPELL_TYPES_VEC);

        static const spell::SpellPtr_t PickSpell(
            const creature::CreaturePtr_t CREATURE_DECIDING_PTR,
            const combat::EffectType::Enum SPELL_TYPE);

        static const spell::SpellPtr_t PickSpell(
            const creature::CreaturePtr_t CREATURE_DECIDING_PTR,
            const EffectTypeVec_t & SPELL_TYPES_VEC);

        static const TurnActionInfo DecideIfFlying(
            const TurnInfo & TURN_INFO, const creature::CreaturePtr_t CREATURE_DECIDING_PTR);

        static const TurnActionInfo DecideIfAdvancingTowardNonMostDesiredTarget(
            const TurnInfo & CREATURE_DECIDING_TURN_INFO,
            const creature::CreaturePtr_t CREATURE_DECIDING_PTR,
            const CombatDisplayPtr_t COMBAT_DISPLAY_PTR);

        static void AdjustCreatueVecForMurderousIntent(
            const TurnInfo & CREATURE_DECIDING_TURN_INFO, creature::CreaturePVec_t & pVec_OutParam);

    private:
        static const EffectTypeVec_t HARM_EFFECTTYPES_VEC_;
        static const EffectTypeVec_t HELP_EFFECTTYPES_VEC_;
    };

} // namespace combat
} // namespace heroespath

#endif // HEROESPATH_COMBAT_TURNDECIDER_HPP_INCLUDED
