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

#include "game/log-macros.hpp"
#include "game/creature/creature.hpp"
#include "game/creature/stats.hpp"
#include "game/creature/conditions.hpp"
#include "game/combat/encounter.hpp"

#include "misc/real.hpp"
#include "misc/random.hpp"

#include <string>


namespace game
{
namespace spell
{

    const ContentAndNamePos Sparks::ActionPhrase() const
    {
        auto const RANDOM_VALUE{ misc::random::Int(3) };

        if (RANDOM_VALUE == 0)
        {
            return ContentAndNamePos(" shoots sparks at ",
                                     NamePosition::SourceThenTarget);
        }
        else if (RANDOM_VALUE == 1)
        {
            return ContentAndNamePos(" is sprayed with sparks ",
                                     NamePosition::TargetBefore);
        }
        else if (RANDOM_VALUE == 2)
        {
            return ContentAndNamePos("A sputter of sparks hits ",
                                     NamePosition::TargetAfter);
        }
        else
        {
            return ContentAndNamePos("Sparks shower ",
                                     NamePosition::TargetAfter);
        }
    }


    bool Sparks::EffectCreature(
        creature::CreaturePtr_t   castingCreaturePtr,
        creature::CreaturePtr_t   creatureCastUponPtr,
        stats::Trait_t &         healthAdj,
        creature::CondEnumVec_t &,
        creature::CondEnumVec_t &,
        ContentAndNamePos &       actionPhraseCNP) const
    {
        auto const DAMAGE_ABS_ORIG{ creature::Stats::RandomRatioWithFloorAndRankBonus(
            castingCreaturePtr,
            stats::Traits::Intelligence,
            8,
            0,
            0.5f,
            true,
            true) };

        auto const DAMAGE_ABS_MAX{ creatureCastUponPtr->HealthCurrent() };

        auto const DAMAGE_ABS_FINAL{ ((DAMAGE_ABS_ORIG > DAMAGE_ABS_MAX) ?
            DAMAGE_ABS_MAX : DAMAGE_ABS_ORIG) };

        healthAdj = -1 * DAMAGE_ABS_FINAL;

        actionPhraseCNP = ActionPhrase();
        return true;
    }


    const ContentAndNamePos Bandage::ActionPhrase() const
    {
        auto const RANDOM_VALUE{ misc::random::Int(3) };

        if (RANDOM_VALUE == 0)
        {
            return ContentAndNamePos(" magically bandages ",
                                     NamePosition::SourceThenTarget);
        }
        else if (RANDOM_VALUE == 1)
        {
            return ContentAndNamePos("",
                                     " magically bandages ",
                                     "'s wounds ",
                                     NamePosition::SourceThenTarget);
        }
        else if (RANDOM_VALUE == 2)
        {
            return ContentAndNamePos("",
                                     "Magical bandages wrap ",
                                     "'s injuries ",
                                     NamePosition::TargetAfter);
        }
        else
        {
            return ContentAndNamePos("'s wounds are magically bandaged ",
                                     NamePosition::TargetBefore);
        }
    }


    bool Bandage::EffectCreature(
        creature::CreaturePtr_t   castingCreaturePtr,
        creature::CreaturePtr_t   creatureCastUponPtr,
        stats::Trait_t &         healthAdj,
        creature::CondEnumVec_t &,
        creature::CondEnumVec_t &,
        ContentAndNamePos &       actionPhraseCNP) const
    {
        auto const HEALTH_GAIN_ORIG{ creature::Stats::RandomRatioWithFloorAndRankBonus(
            castingCreaturePtr,
            stats::Traits::Charm,
            8,
            0,
            0.5f,
            true,
            true) };

        auto const HEALTH_GAIN_MAX{ creatureCastUponPtr->HealthMissing() };

        healthAdj = ((HEALTH_GAIN_ORIG > HEALTH_GAIN_MAX) ?
            HEALTH_GAIN_MAX : HEALTH_GAIN_ORIG);

        if (0 == healthAdj)
        {
            actionPhraseCNP = ContentAndNamePos(Spell::FAILED_BECAUSE_STR_,
                                                "'s health already at maximum.",
                                                "",
                                                NamePosition::TargetBefore);
            return false;
        }
        else
        {
            actionPhraseCNP = ActionPhrase();
            return true;
        }
    }


    const ContentAndNamePos Sleep::ActionPhrase() const
    {
        //TODO add more phrases
        return ContentAndNamePos("",
                                 "'s magic forces ",
                                 " to sleep.",
                                 NamePosition::SourceThenTarget);
    }


    bool Sleep::EffectCreature(
        creature::CreaturePtr_t,
        creature::CreaturePtr_t   creatureCastUponPtr,
        stats::Trait_t &,
        creature::CondEnumVec_t & condsAddedVec,
        creature::CondEnumVec_t &,
        ContentAndNamePos &       actionPhraseCNP) const
    {
        if (creatureCastUponPtr->HasCondition(creature::Conditions::AsleepNatural) ||
            creatureCastUponPtr->HasCondition(creature::Conditions::AsleepMagical))
        {
            actionPhraseCNP =  ContentAndNamePos(Spell::FAILED_BECAUSE_STR_,
                                                 " is already asleep.",
                                                 "",
                                                 NamePosition::TargetBefore);
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


    const ContentAndNamePos Awaken::ActionPhrase() const
    {
        return ContentAndNamePos("",
                                 "'s magic forces ",
                                 " to wake.",
                                 NamePosition::SourceThenTarget);
    }


    bool Awaken::EffectCreature(
        creature::CreaturePtr_t,
        creature::CreaturePtr_t   creatureCastUponPtr,
        stats::Trait_t &,
        creature::CondEnumVec_t &,
        creature::CondEnumVec_t & condsRemovedVec,
        ContentAndNamePos &       actionPhraseCNP) const
    {
        const creature::CondEnumVec_t CONDS_TO_REMOVE_VEC{
            creature::Conditions::AsleepNatural,
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
            actionPhraseCNP = ContentAndNamePos(Spell::FAILED_BECAUSE_STR_,
                                                " is already awake.",
                                                "",
                                                NamePosition::TargetBefore);
            return false;
        }
    }


    const ContentAndNamePos Trip::ActionPhrase() const
    {
        return ContentAndNamePos("",
                                 "'s spell trips ",
                                 ".",
                                 NamePosition::SourceThenTarget);
    }


    bool Trip::EffectCreature(
        creature::CreaturePtr_t,
        creature::CreaturePtr_t   creatureCastUponPtr,
        stats::Trait_t &,
        creature::CondEnumVec_t & condsAddedVec,
        creature::CondEnumVec_t &,
        ContentAndNamePos &       actionPhraseCNP) const
    {
        if (creatureCastUponPtr->HasCondition(creature::Conditions::Tripped))
        {
            actionPhraseCNP = ContentAndNamePos(Spell::FAILED_BECAUSE_STR_,
                                                " is already tripped.",
                                                "",
                                                NamePosition::TargetBefore);
            return false;
        }
        else if (combat::Encounter::Instance()->GetTurnInfoCopy(creatureCastUponPtr).GetIsFlying())
        {
            actionPhraseCNP = ContentAndNamePos(Spell::FAILED_BECAUSE_STR_,
                                                " flying creatures can't be tripped.",
                                                "",
                                                NamePosition::TargetBefore);
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


    const ContentAndNamePos Lift::ActionPhrase(
        creature::CreaturePtr_t creatureCastUponPtr) const
    {
        return ContentAndNamePos(
            "",
            "'s magic forces ",
            " to " + creature::sex::HisHerIts(creatureCastUponPtr->Sex(), false, false) + " feet.",
            NamePosition::SourceThenTarget);
    }


    bool Lift::EffectCreature(
        creature::CreaturePtr_t,
        creature::CreaturePtr_t   creatureCastUponPtr,
        stats::Trait_t &,
        creature::CondEnumVec_t &,
        creature::CondEnumVec_t & condsRemovedVec,
        ContentAndNamePos &       actionPhraseCNP) const
    {
        if (creatureCastUponPtr->HasCondition(creature::Conditions::Tripped))
        {
            creatureCastUponPtr->ConditionRemove(creature::Conditions::Tripped);
            condsRemovedVec.push_back(creature::Conditions::Tripped);
            actionPhraseCNP = ActionPhrase(creatureCastUponPtr);
            return true;
        }
        else if(creatureCastUponPtr->HasCondition(creature::Conditions::Pounced))
        {
            creatureCastUponPtr->ConditionRemove(creature::Conditions::Pounced);
            condsRemovedVec.push_back(creature::Conditions::Pounced);
            actionPhraseCNP = ActionPhrase(creatureCastUponPtr);
            return true;
        }
        else
        {
            actionPhraseCNP = ContentAndNamePos(
                Spell::FAILED_BECAUSE_STR_,
                " is already upright.",
                "",
                NamePosition::TargetBefore);
            return false;
        }
    }


    const ContentAndNamePos Daze::ActionPhrase() const
    {
        return ContentAndNamePos("",
                                 "'s magic puts ",
                                 " in a daze.",
                                 NamePosition::SourceThenTarget);
    }


    bool Daze::EffectCreature(
        creature::CreaturePtr_t,
        creature::CreaturePtr_t   creatureCastUponPtr,
        stats::Trait_t &,
        creature::CondEnumVec_t & condsAddedVec,
        creature::CondEnumVec_t &,
        ContentAndNamePos &       actionPhraseCNP) const
    {
        if (creatureCastUponPtr->HasCondition(creature::Conditions::Dazed))
        {
            actionPhraseCNP = ContentAndNamePos(Spell::FAILED_BECAUSE_STR_,
                                                " is already dazed.",
                                                "",
                                                NamePosition::TargetBefore);
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


    const ContentAndNamePos Panic::ActionPhrase() const
    {
        return ContentAndNamePos("",
                                 "'s spell panics ",
                                 ".",
                                 NamePosition::SourceThenTarget);
    }


    bool Panic::EffectCreature(
        creature::CreaturePtr_t,
        creature::CreaturePtr_t   creatureCastUponPtr,
        stats::Trait_t &,
        creature::CondEnumVec_t & condsAddedVec,
        creature::CondEnumVec_t &,
        ContentAndNamePos &       actionPhraseCNP) const
    {
        if (creatureCastUponPtr->HasCondition(creature::Conditions::Panic))
        {
            actionPhraseCNP = ContentAndNamePos(Spell::FAILED_BECAUSE_STR_,
                                                " is already panicked.",
                                                "",
                                                NamePosition::TargetBefore);
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


    const ContentAndNamePos ClearMind::ActionPhrase() const
    {
        return ContentAndNamePos("",
                                 "'s spell clears ",
                                 "'s mind.",
                                 NamePosition::SourceThenTarget);
    }


    bool ClearMind::EffectCreature(
        creature::CreaturePtr_t,
        creature::CreaturePtr_t   creatureCastUponPtr,
        stats::Trait_t &,
        creature::CondEnumVec_t &,
        creature::CondEnumVec_t & condsRemovedVec,
        ContentAndNamePos &       actionPhraseCNP) const
    {
        const creature::CondEnumVec_t CONDS_TO_REMOVE_VEC{
            creature::Conditions::Panic,
            creature::Conditions::Daunted,
            creature::Conditions::Dazed };

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
            actionPhraseCNP = ContentAndNamePos(Spell::FAILED_BECAUSE_STR_,
                                                " is already clear headed.",
                                                "",
                                                NamePosition::TargetBefore);
            return false;
        }
    }


    const ContentAndNamePos Poison::ActionPhrase() const
    {
        return ContentAndNamePos("",
                                 "'s spell poisons ",
                                 ".",
                                 NamePosition::SourceThenTarget);
    }


    bool Poison::EffectCreature(
        creature::CreaturePtr_t,
        creature::CreaturePtr_t   creatureCastUponPtr,
        stats::Trait_t &,
        creature::CondEnumVec_t & condsAddedVec,
        creature::CondEnumVec_t &,
        ContentAndNamePos &       actionPhraseCNP) const
    {
        if (creatureCastUponPtr->HasCondition(creature::Conditions::Poisoned))
        {
            actionPhraseCNP = ContentAndNamePos(Spell::FAILED_BECAUSE_STR_,
                                                " is already poisoned.",
                                                "",
                                                NamePosition::TargetBefore);
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


    const ContentAndNamePos Antidote::ActionPhrase() const
    {
        return ContentAndNamePos("",
                                 "'s spell cures ",
                                 ".",
                                 NamePosition::SourceThenTarget);
    }


    bool Antidote::EffectCreature(
        creature::CreaturePtr_t,
        creature::CreaturePtr_t   creatureCastUponPtr,
        stats::Trait_t &,
        creature::CondEnumVec_t &,
        creature::CondEnumVec_t & condsRemovedVec,
        ContentAndNamePos &       actionPhraseCNP) const
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
            actionPhraseCNP = ContentAndNamePos(Spell::FAILED_BECAUSE_STR_,
                                                " is already cured.",
                                                "",
                                                NamePosition::TargetBefore);
            return false;
        }
    }


    const ContentAndNamePos PoisonCloud::ActionPhrase() const
    {
        return ContentAndNamePos("",
                                 "'s spell poisons the ",
                                 ".",
                                 NamePosition::SourceThenTarget);
    }


    bool PoisonCloud::EffectCreature(
        creature::CreaturePtr_t   castingCreaturePtr,
        creature::CreaturePtr_t   creatureCastUponPtr,
        stats::Trait_t &,
        creature::CondEnumVec_t & condsAddedVec,
        creature::CondEnumVec_t &,
        ContentAndNamePos &       actionPhraseCNP) const
    {
        if (creatureCastUponPtr->HasCondition(creature::Conditions::Poisoned))
        {
            actionPhraseCNP = ContentAndNamePos(Spell::FAILED_BECAUSE_STR_,
                                                " is already poisoned.",
                                                "",
                                                NamePosition::TargetBefore);
            return false;
        }
        else
        {
            auto const DID_STAT_ROLL_SUCCEED{ creature::Stats::Versus(castingCreaturePtr,
                stats::Traits::Intelligence,
                creatureCastUponPtr,
                stats::Traits::Count,
                0,
                0,
                true,
                true,
                true,
                true) };

            auto const DID_MAGIC_CAST_TRAIT_BONUS_SUCCEED{
                castingCreaturePtr->TraitBonusTest(stats::Traits::MagicCast) };

            if (DID_STAT_ROLL_SUCCEED || DID_MAGIC_CAST_TRAIT_BONUS_SUCCEED)
            {
                creatureCastUponPtr->ConditionAdd(creature::Conditions::Poisoned);
                condsAddedVec.push_back(creature::Conditions::Poisoned);
                actionPhraseCNP = ActionPhrase();
                return true;
            }
            else
            {
                actionPhraseCNP = ContentAndNamePos(Spell::FAILED_BECAUSE_STR_,
                                                    Spell::RESISTED_STR_,
                                                    "",
                                                    NamePosition::TargetBefore);
                return false;
            }
        }
    }

}
}
