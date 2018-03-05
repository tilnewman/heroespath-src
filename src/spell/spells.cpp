// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
//
// spells.cpp
//
#include "spells.hpp"

#include "combat/encounter.hpp"
#include "creature/conditions.hpp"
#include "creature/creature.hpp"
#include "creature/stats.hpp"
#include "log/log-macros.hpp"

#include "misc/random.hpp"
#include "misc/real.hpp"

#include <string>

namespace heroespath
{
namespace spell
{

    const combat::ContentAndNamePos Sparks::ActionPhrase() const
    {
        auto const RANDOM_VALUE{ misc::random::Int(3) };

        if (RANDOM_VALUE == 0)
        {
            return combat::ContentAndNamePos(
                " shoots sparks at ", combat::NamePosition::SourceThenTarget);
        }
        else if (RANDOM_VALUE == 1)
        {
            return combat::ContentAndNamePos(
                " is sprayed with sparks ", combat::NamePosition::TargetBefore);
        }
        else if (RANDOM_VALUE == 2)
        {
            return combat::ContentAndNamePos(
                "A sputter of sparks hits ", combat::NamePosition::TargetAfter);
        }
        else
        {
            return combat::ContentAndNamePos("Sparks shower ", combat::NamePosition::TargetAfter);
        }
    }

    bool Sparks::EffectCreature(
        creature::CreaturePtr_t castingCreaturePtr,
        creature::CreaturePtr_t creatureCastUponPtr,
        Health_t & healthAdj,
        creature::CondEnumVec_t &,
        creature::CondEnumVec_t &,
        combat::ContentAndNamePos & actionPhraseCNP) const
    {
        auto const DAMAGE_ABS_ORIG{ Health_t(creature::Stats::RandomRatio(
            castingCreaturePtr,
            stats::Traits::Intelligence,
            8,
            0.5f,
            0,
            static_cast<creature::Stats::With>(
                creature::Stats::With::Luck | creature::Stats::With::RaceRoleBonus))) };

        auto const DAMAGE_ABS_MAX{ creatureCastUponPtr->HealthCurrent() };

        auto const DAMAGE_ABS_FINAL{ (
            (DAMAGE_ABS_ORIG > DAMAGE_ABS_MAX) ? DAMAGE_ABS_MAX : DAMAGE_ABS_ORIG) };

        healthAdj = Health_t(-1) * DAMAGE_ABS_FINAL;

        actionPhraseCNP = ActionPhrase();
        return true;
    }

    const combat::ContentAndNamePos Bandage::ActionPhrase() const
    {
        auto const RANDOM_VALUE{ misc::random::Int(3) };

        if (RANDOM_VALUE == 0)
        {
            return combat::ContentAndNamePos(
                " magically bandages ", combat::NamePosition::SourceThenTarget);
        }
        else if (RANDOM_VALUE == 1)
        {
            return combat::ContentAndNamePos(
                "", " magically bandages ", "'s wounds ", combat::NamePosition::SourceThenTarget);
        }
        else if (RANDOM_VALUE == 2)
        {
            return combat::ContentAndNamePos(
                "", "Magical bandages wrap ", "'s injuries ", combat::NamePosition::TargetAfter);
        }
        else
        {
            return combat::ContentAndNamePos(
                "'s wounds are magically bandaged ", combat::NamePosition::TargetBefore);
        }
    }

    bool Bandage::EffectCreature(
        creature::CreaturePtr_t castingCreaturePtr,
        creature::CreaturePtr_t creatureCastUponPtr,
        Health_t & healthAdj,
        creature::CondEnumVec_t &,
        creature::CondEnumVec_t &,
        combat::ContentAndNamePos & actionPhraseCNP) const
    {
        auto const HEALTH_GAIN_ORIG{ Health_t(creature::Stats::RandomRatio(
            castingCreaturePtr,
            stats::Traits::Charm,
            8,
            0.5f,
            castingCreaturePtr->TraitBonusCurrent(stats::Traits::MagicEffect),
            static_cast<creature::Stats::With>(
                creature::Stats::With::Luck | creature::Stats::With::RaceRoleBonus))) };

        auto const HEALTH_GAIN_MAX{ creatureCastUponPtr->HealthMissing() };

        healthAdj = ((HEALTH_GAIN_ORIG > HEALTH_GAIN_MAX) ? HEALTH_GAIN_MAX : HEALTH_GAIN_ORIG);

        if (0_health == healthAdj)
        {
            actionPhraseCNP = combat::ContentAndNamePos(
                Spell::FAILED_BECAUSE_STR_,
                "'s health already at maximum.",
                "",
                combat::NamePosition::TargetBefore);

            return false;
        }
        else
        {
            actionPhraseCNP = ActionPhrase();
            return true;
        }
    }

    const combat::ContentAndNamePos Sleep::ActionPhrase() const
    {
        // TODO add more phrases
        return combat::ContentAndNamePos(
            "", "'s magic forces ", " to sleep.", combat::NamePosition::SourceThenTarget);
    }

    bool Sleep::EffectCreature(
        creature::CreaturePtr_t,
        creature::CreaturePtr_t creatureCastUponPtr,
        Health_t &,
        creature::CondEnumVec_t & condsAddedVec,
        creature::CondEnumVec_t &,
        combat::ContentAndNamePos & actionPhraseCNP) const
    {
        if (creatureCastUponPtr->HasCondition(creature::Conditions::AsleepNatural)
            || creatureCastUponPtr->HasCondition(creature::Conditions::AsleepMagical))
        {
            actionPhraseCNP = combat::ContentAndNamePos(
                Spell::FAILED_BECAUSE_STR_,
                " is already asleep.",
                "",
                combat::NamePosition::TargetBefore);

            return false;
        }
        else
        {
            creatureCastUponPtr->ConditionAdd(creature::Conditions::AsleepNatural);
            condsAddedVec.push_back(creature::Conditions::AsleepNatural);
            actionPhraseCNP = ActionPhrase();
            combat::Encounter::Instance()->SetIsFlying(creatureCastUponPtr, false);
            return true;
        }
    }

    const combat::ContentAndNamePos Awaken::ActionPhrase() const
    {
        return combat::ContentAndNamePos(
            "", "'s magic forces ", " to wake.", combat::NamePosition::SourceThenTarget);
    }

    bool Awaken::EffectCreature(
        creature::CreaturePtr_t,
        creature::CreaturePtr_t creatureCastUponPtr,
        Health_t &,
        creature::CondEnumVec_t &,
        creature::CondEnumVec_t & condsRemovedVec,
        combat::ContentAndNamePos & actionPhraseCNP) const
    {
        const creature::CondEnumVec_t CONDS_TO_REMOVE_VEC{ creature::Conditions::AsleepNatural,
                                                           creature::Conditions::AsleepMagical,
                                                           creature::Conditions::Unconscious };

        auto wereAnyCondsRemoved{ false };
        for (auto const NEXT_COND_ENUM_TO_REMOVE : CONDS_TO_REMOVE_VEC)
        {
            if (creatureCastUponPtr->HasCondition(NEXT_COND_ENUM_TO_REMOVE))
            {
                creatureCastUponPtr->ConditionRemove(NEXT_COND_ENUM_TO_REMOVE);
                condsRemovedVec.push_back(NEXT_COND_ENUM_TO_REMOVE);
                wereAnyCondsRemoved = true;
            }
        }

        if (wereAnyCondsRemoved)
        {
            actionPhraseCNP = ActionPhrase();
            return true;
        }
        else
        {
            actionPhraseCNP = combat::ContentAndNamePos(
                Spell::FAILED_BECAUSE_STR_,
                " is already awake.",
                "",
                combat::NamePosition::TargetBefore);

            return false;
        }
    }

    const combat::ContentAndNamePos Trip::ActionPhrase() const
    {
        return combat::ContentAndNamePos(
            "", "'s spell trips ", ".", combat::NamePosition::SourceThenTarget);
    }

    bool Trip::EffectCreature(
        creature::CreaturePtr_t,
        creature::CreaturePtr_t creatureCastUponPtr,
        Health_t &,
        creature::CondEnumVec_t & condsAddedVec,
        creature::CondEnumVec_t &,
        combat::ContentAndNamePos & actionPhraseCNP) const
    {
        if (creatureCastUponPtr->HasCondition(creature::Conditions::Tripped))
        {
            actionPhraseCNP = combat::ContentAndNamePos(
                Spell::FAILED_BECAUSE_STR_,
                " is already tripped.",
                "",
                combat::NamePosition::TargetBefore);

            return false;
        }
        else if (combat::Encounter::Instance()->GetTurnInfoCopy(creatureCastUponPtr).GetIsFlying())
        {
            actionPhraseCNP = combat::ContentAndNamePos(
                Spell::FAILED_BECAUSE_STR_,
                " flying creatures can't be tripped.",
                "",
                combat::NamePosition::TargetBefore);

            return false;
        }
        else
        {
            creatureCastUponPtr->ConditionAdd(creature::Conditions::Tripped);
            condsAddedVec.push_back(creature::Conditions::Tripped);
            actionPhraseCNP = ActionPhrase();
            return true;
        }
    }

    const combat::ContentAndNamePos
        Lift::ActionPhrase(creature::CreaturePtr_t creatureCastUponPtr) const
    {
        return combat::ContentAndNamePos(
            "",
            "'s magic forces ",
            " to " + creature::sex::HisHerIts(creatureCastUponPtr->Sex(), false, false) + " feet.",
            combat::NamePosition::SourceThenTarget);
    }

    bool Lift::EffectCreature(
        creature::CreaturePtr_t,
        creature::CreaturePtr_t creatureCastUponPtr,
        Health_t &,
        creature::CondEnumVec_t &,
        creature::CondEnumVec_t & condsRemovedVec,
        combat::ContentAndNamePos & actionPhraseCNP) const
    {
        if (creatureCastUponPtr->HasCondition(creature::Conditions::Tripped))
        {
            creatureCastUponPtr->ConditionRemove(creature::Conditions::Tripped);
            condsRemovedVec.push_back(creature::Conditions::Tripped);
            actionPhraseCNP = ActionPhrase(creatureCastUponPtr);
            return true;
        }
        else if (creatureCastUponPtr->HasCondition(creature::Conditions::Pounced))
        {
            creatureCastUponPtr->ConditionRemove(creature::Conditions::Pounced);
            condsRemovedVec.push_back(creature::Conditions::Pounced);
            actionPhraseCNP = ActionPhrase(creatureCastUponPtr);
            return true;
        }
        else
        {
            actionPhraseCNP = combat::ContentAndNamePos(
                Spell::FAILED_BECAUSE_STR_,
                " is already upright.",
                "",
                combat::NamePosition::TargetBefore);
            return false;
        }
    }

    const combat::ContentAndNamePos Daze::ActionPhrase() const
    {
        return combat::ContentAndNamePos(
            "", "'s magic puts ", " in a daze.", combat::NamePosition::SourceThenTarget);
    }

    bool Daze::EffectCreature(
        creature::CreaturePtr_t,
        creature::CreaturePtr_t creatureCastUponPtr,
        Health_t &,
        creature::CondEnumVec_t & condsAddedVec,
        creature::CondEnumVec_t &,
        combat::ContentAndNamePos & actionPhraseCNP) const
    {
        if (creatureCastUponPtr->HasCondition(creature::Conditions::Dazed))
        {
            actionPhraseCNP = combat::ContentAndNamePos(
                Spell::FAILED_BECAUSE_STR_,
                " is already dazed.",
                "",
                combat::NamePosition::TargetBefore);

            return false;
        }
        else
        {
            creatureCastUponPtr->ConditionAdd(creature::Conditions::Dazed);
            condsAddedVec.push_back(creature::Conditions::Dazed);
            actionPhraseCNP = ActionPhrase();
            return true;
        }
    }

    const combat::ContentAndNamePos Panic::ActionPhrase() const
    {
        return combat::ContentAndNamePos(
            "", "'s spell panics ", ".", combat::NamePosition::SourceThenTarget);
    }

    bool Panic::EffectCreature(
        creature::CreaturePtr_t,
        creature::CreaturePtr_t creatureCastUponPtr,
        Health_t &,
        creature::CondEnumVec_t & condsAddedVec,
        creature::CondEnumVec_t &,
        combat::ContentAndNamePos & actionPhraseCNP) const
    {
        if (creatureCastUponPtr->HasCondition(creature::Conditions::Panic))
        {
            actionPhraseCNP = combat::ContentAndNamePos(
                Spell::FAILED_BECAUSE_STR_,
                " is already panicked.",
                "",
                combat::NamePosition::TargetBefore);

            return false;
        }
        else
        {
            creatureCastUponPtr->ConditionAdd(creature::Conditions::Panic);
            condsAddedVec.push_back(creature::Conditions::Panic);
            actionPhraseCNP = ActionPhrase();
            return true;
        }
    }

    const combat::ContentAndNamePos ClearMind::ActionPhrase() const
    {
        return combat::ContentAndNamePos(
            "", "'s spell clears ", "'s mind.", combat::NamePosition::SourceThenTarget);
    }

    bool ClearMind::EffectCreature(
        creature::CreaturePtr_t,
        creature::CreaturePtr_t creatureCastUponPtr,
        Health_t &,
        creature::CondEnumVec_t &,
        creature::CondEnumVec_t & condsRemovedVec,
        combat::ContentAndNamePos & actionPhraseCNP) const
    {
        const creature::CondEnumVec_t CONDS_TO_REMOVE_VEC{
            creature::Conditions::Panic, creature::Conditions::Daunted, creature::Conditions::Dazed
        };

        auto wereAnyCondsRemoved{ false };
        for (auto const NEXT_COND_ENUM_TO_REMOVE : CONDS_TO_REMOVE_VEC)
        {
            if (creatureCastUponPtr->HasCondition(NEXT_COND_ENUM_TO_REMOVE))
            {
                creatureCastUponPtr->ConditionRemove(NEXT_COND_ENUM_TO_REMOVE);
                condsRemovedVec.push_back(NEXT_COND_ENUM_TO_REMOVE);
                wereAnyCondsRemoved = true;
            }
        }

        if (wereAnyCondsRemoved)
        {
            actionPhraseCNP = ActionPhrase();
            return true;
        }
        else
        {
            actionPhraseCNP = combat::ContentAndNamePos(
                Spell::FAILED_BECAUSE_STR_,
                " is already clear headed.",
                "",
                combat::NamePosition::TargetBefore);

            return false;
        }
    }

    const combat::ContentAndNamePos Poison::ActionPhrase() const
    {
        return combat::ContentAndNamePos(
            "", "'s spell poisons ", ".", combat::NamePosition::SourceThenTarget);
    }

    bool Poison::EffectCreature(
        creature::CreaturePtr_t,
        creature::CreaturePtr_t creatureCastUponPtr,
        Health_t &,
        creature::CondEnumVec_t & condsAddedVec,
        creature::CondEnumVec_t &,
        combat::ContentAndNamePos & actionPhraseCNP) const
    {
        if (creatureCastUponPtr->HasCondition(creature::Conditions::Poisoned))
        {
            actionPhraseCNP = combat::ContentAndNamePos(
                Spell::FAILED_BECAUSE_STR_,
                " is already poisoned.",
                "",
                combat::NamePosition::TargetBefore);

            return false;
        }
        else
        {
            creatureCastUponPtr->ConditionAdd(creature::Conditions::Poisoned);
            condsAddedVec.push_back(creature::Conditions::Poisoned);
            actionPhraseCNP = ActionPhrase();
            return true;
        }
    }

    const combat::ContentAndNamePos Antidote::ActionPhrase() const
    {
        return combat::ContentAndNamePos(
            "", "'s spell cures ", ".", combat::NamePosition::SourceThenTarget);
    }

    bool Antidote::EffectCreature(
        creature::CreaturePtr_t,
        creature::CreaturePtr_t creatureCastUponPtr,
        Health_t &,
        creature::CondEnumVec_t &,
        creature::CondEnumVec_t & condsRemovedVec,
        combat::ContentAndNamePos & actionPhraseCNP) const
    {
        if (creatureCastUponPtr->HasCondition(creature::Conditions::Poisoned))
        {
            creatureCastUponPtr->ConditionRemove(creature::Conditions::Poisoned);
            condsRemovedVec.push_back(creature::Conditions::Poisoned);
            actionPhraseCNP = ActionPhrase();
            return true;
        }
        else
        {
            actionPhraseCNP = combat::ContentAndNamePos(
                Spell::FAILED_BECAUSE_STR_,
                " is already cured.",
                "",
                combat::NamePosition::TargetBefore);

            return false;
        }
    }

    const combat::ContentAndNamePos PoisonCloud::ActionPhrase() const
    {
        return combat::ContentAndNamePos(
            "", "'s spell poisons the ", ".", combat::NamePosition::SourceThenTarget);
    }

    bool PoisonCloud::EffectCreature(
        creature::CreaturePtr_t castingCreaturePtr,
        creature::CreaturePtr_t creatureCastUponPtr,
        Health_t &,
        creature::CondEnumVec_t & condsAddedVec,
        creature::CondEnumVec_t &,
        combat::ContentAndNamePos & actionPhraseCNP) const
    {
        if (creatureCastUponPtr->HasCondition(creature::Conditions::Poisoned))
        {
            actionPhraseCNP = combat::ContentAndNamePos(
                Spell::FAILED_BECAUSE_STR_,
                " is already poisoned.",
                "",
                combat::NamePosition::TargetBefore);

            return false;
        }
        else
        {
            auto const DID_STAT_ROLL_SUCCEED{ creature::Stats::Versus(
                castingCreaturePtr,
                stats::Traits::Intelligence,
                creatureCastUponPtr,
                stats::Traits::Count,
                0,
                0,
                static_cast<creature::Stats::With>(
                    creature::Stats::With::Luck | creature::Stats::With::RaceRoleBonus
                    | creature::Stats::With::RankBonus
                    | creature::Stats::With::PlayerNaturalWins)) };

            auto const DID_MAGIC_CAST_TRAIT_BONUS_SUCCEED{ castingCreaturePtr->TraitBonusTest(
                stats::Traits::MagicCast) };

            if (DID_STAT_ROLL_SUCCEED || DID_MAGIC_CAST_TRAIT_BONUS_SUCCEED)
            {
                creatureCastUponPtr->ConditionAdd(creature::Conditions::Poisoned);
                condsAddedVec.push_back(creature::Conditions::Poisoned);
                actionPhraseCNP = ActionPhrase();
                return true;
            }
            else
            {
                actionPhraseCNP = combat::ContentAndNamePos(
                    Spell::FAILED_BECAUSE_STR_,
                    Spell::RESISTED_STR_,
                    "",
                    combat::NamePosition::TargetBefore);

                return false;
            }
        }
    }
} // namespace spell
} // namespace heroespath
