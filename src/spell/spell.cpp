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
// spell.cpp
//
#include "spell.hpp"

#include "combat/encounter.hpp"
#include "creature/condition.hpp"
#include "creature/creature.hpp"
#include "creature/stats.hpp"
#include "misc/random.hpp"
#include "misc/real.hpp"
#include "misc/strings.hpp"

#include <algorithm>
#include <sstream>

namespace heroespath
{
namespace spell
{

    const std::string Spell::RESISTED_STR_{ " resisted." };
    const std::string Spell::FAILED_BECAUSE_STR_{ "Spell failed because " };

    Spell::Spell(
        const Spells::Enum WHICH,
        const combat::EffectType::Enum EFFECT_TYPE,
        const game::Phase::Enum VALID_PHASES,
        const Mana_t & MANA_COST,
        const Rank_t & RANK,
        const combat::TargetType::Enum TARGET_TYPE,
        const std::string & VERB_THIRD_PERSON,
        const std::string & VERB_PAST_TENSE)
        : which_(WHICH)
        , rank_(RANK)
        , effectType_(EFFECT_TYPE)
        , validPhases_(VALID_PHASES)
        , manaCost_(MANA_COST)
        , targetType_(TARGET_TYPE)
        , verbThirdPerson_(VERB_THIRD_PERSON)
        , verbPastTense_(VERB_PAST_TENSE)
    {}

    const std::string Spell::ToString() const
    {
        std::ostringstream ss;
        ss << Name() << "  -" << DescDetails();
        return ss.str();
    }

    const std::string Spell::DescDetails() const
    {
        std::ostringstream ss;
        ss << "A " << misc::String::DecorateNumber(rank_.As<int>()) << " rank"
           << " " << combat::EffectType::Name(effectType_) << " spell"
           << " that can be cast during " << game::Phase::ToString(validPhases_) << ", targeting "
           << combat::TargetType::Name(targetType_) << ", and costing " << manaCost_ << " mana.";

        return ss.str();
    }

    const std::string Spell::DescComplete() const
    {
        std::ostringstream ss;
        ss << Desc() << "  " << DescExtra() << " " << DescDetails();
        return ss.str();
    }

    const combat::ContentAndNamePos
        Spell::ActionPhrase(const creature::CreaturePtr_t CREATURE_CAST_UPON_PTR) const
    {
        switch (Which())
        {
            case Spells::Sparks:
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
                    return combat::ContentAndNamePos(
                        "Sparks shower ", combat::NamePosition::TargetAfter);
                }
            }
            case Spells::Bandage:
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
                        "",
                        " magically bandages ",
                        "'s wounds ",
                        combat::NamePosition::SourceThenTarget);
                }
                else if (RANDOM_VALUE == 2)
                {
                    return combat::ContentAndNamePos(
                        "",
                        "Magical bandages wrap ",
                        "'s injuries ",
                        combat::NamePosition::TargetAfter);
                }
                else
                {
                    return combat::ContentAndNamePos(
                        "'s wounds are magically bandaged ", combat::NamePosition::TargetBefore);
                }
            }
            case Spells::Sleep:
            {
                // TODO add more phrases
                return combat::ContentAndNamePos(
                    "", "'s magic forces ", " to sleep.", combat::NamePosition::SourceThenTarget);
            }
            case Spells::Awaken:
            {
                return combat::ContentAndNamePos(
                    "", "'s magic forces ", " to wake.", combat::NamePosition::SourceThenTarget);
            }
            case Spells::Trip:
            {
                return combat::ContentAndNamePos(
                    "", "'s spell trips ", ".", combat::NamePosition::SourceThenTarget);
            }
            case Spells::Lift:
            {
                return combat::ContentAndNamePos(
                    "",
                    "'s magic forces ",
                    " to " + creature::sex::HisHerIts(CREATURE_CAST_UPON_PTR->Sex(), false, false)
                        + " feet.",
                    combat::NamePosition::SourceThenTarget);
            }
            case Spells::Daze:
            {
                return combat::ContentAndNamePos(
                    "", "'s magic puts ", " in a daze.", combat::NamePosition::SourceThenTarget);
            }
            case Spells::Panic:
            {
                return combat::ContentAndNamePos(
                    "", "'s spell panics ", ".", combat::NamePosition::SourceThenTarget);
            }
            case Spells::ClearMind:
            {
                return combat::ContentAndNamePos(
                    "", "'s spell clears ", "'s mind.", combat::NamePosition::SourceThenTarget);
            }
            case Spells::Poison:
            {
                return combat::ContentAndNamePos(
                    "", "'s spell poisons ", ".", combat::NamePosition::SourceThenTarget);
            }
            case Spells::Antidote:
            {
                return combat::ContentAndNamePos(
                    "", "'s spell cures ", ".", combat::NamePosition::SourceThenTarget);
            }
            case Spells::PoisonCloud:
            {
                return combat::ContentAndNamePos(
                    "", "'s spell poisons the ", ".", combat::NamePosition::SourceThenTarget);
            }
            case Spells::Count:
            default:
            {
                std::ostringstream ss;
                ss << "spell::Spell::ActionPhrase(creatureCastUpon="
                   << CREATURE_CAST_UPON_PTR->ToString()
                   << ") was called when Which() was invalid (" << Which() << ").";

                throw std::runtime_error(ss.str());
            }
        }
    }

    bool Spell::EffectCreature(
        const creature::CreaturePtr_t CREATURE_CASTING_PTR,
        const creature::CreaturePtr_t CREATURE_CAST_UPON_PTR,
        Health_t & healthAdj,
        creature::CondEnumVec_t & condsAddedVec,
        creature::CondEnumVec_t & condsRemovedVec,
        combat::ContentAndNamePos & actionPhraseCNP) const
    {
        switch (Which())
        {
            case Spells::Sparks:
            {
                auto const DAMAGE_ABS_ORIG{ Health_t(creature::Stats::RandomRatio(
                    CREATURE_CASTING_PTR,
                    stats::Traits::Intelligence,
                    8,
                    0.5f,
                    0,
                    static_cast<creature::Stats::With>(
                        creature::Stats::With::Luck | creature::Stats::With::RaceRoleBonus))) };

                auto const DAMAGE_ABS_MAX{ CREATURE_CAST_UPON_PTR->HealthCurrent() };

                auto const DAMAGE_ABS_FINAL{ (
                    (DAMAGE_ABS_ORIG > DAMAGE_ABS_MAX) ? DAMAGE_ABS_MAX : DAMAGE_ABS_ORIG) };

                healthAdj = Health_t(-1) * DAMAGE_ABS_FINAL;

                actionPhraseCNP = ActionPhrase(CREATURE_CAST_UPON_PTR);
                return true;
            }
            case Spells::Bandage:
            {
                auto const HEALTH_GAIN_ORIG{ Health_t(creature::Stats::RandomRatio(
                    CREATURE_CASTING_PTR,
                    stats::Traits::Charm,
                    8,
                    0.5f,
                    CREATURE_CASTING_PTR->TraitBonusCurrent(stats::Traits::MagicEffect),
                    static_cast<creature::Stats::With>(
                        creature::Stats::With::Luck | creature::Stats::With::RaceRoleBonus))) };

                auto const HEALTH_GAIN_MAX{ CREATURE_CAST_UPON_PTR->HealthMissing() };

                healthAdj
                    = ((HEALTH_GAIN_ORIG > HEALTH_GAIN_MAX) ? HEALTH_GAIN_MAX : HEALTH_GAIN_ORIG);

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
                    actionPhraseCNP = ActionPhrase(CREATURE_CAST_UPON_PTR);
                    return true;
                }
            }
            case Spells::Sleep:
            {
                if (CREATURE_CAST_UPON_PTR->HasCondition(creature::Conditions::AsleepNatural)
                    || CREATURE_CAST_UPON_PTR->HasCondition(creature::Conditions::AsleepMagical))
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
                    CREATURE_CAST_UPON_PTR->ConditionAdd(creature::Conditions::AsleepNatural);
                    condsAddedVec.emplace_back(creature::Conditions::AsleepNatural);
                    actionPhraseCNP = ActionPhrase(CREATURE_CAST_UPON_PTR);
                    combat::Encounter::Instance()->SetIsFlying(CREATURE_CAST_UPON_PTR, false);
                    return true;
                }
            }
            case Spells::Awaken:
            {
                const creature::CondEnumVec_t CONDS_TO_REMOVE_VEC{
                    creature::Conditions::AsleepNatural,
                    creature::Conditions::AsleepMagical,
                    creature::Conditions::Unconscious
                };

                auto wereAnyCondsRemoved{ false };
                for (auto const NEXT_COND_ENUM_TO_REMOVE : CONDS_TO_REMOVE_VEC)
                {
                    if (CREATURE_CAST_UPON_PTR->HasCondition(NEXT_COND_ENUM_TO_REMOVE))
                    {
                        CREATURE_CAST_UPON_PTR->ConditionRemove(NEXT_COND_ENUM_TO_REMOVE);
                        condsRemovedVec.emplace_back(NEXT_COND_ENUM_TO_REMOVE);
                        wereAnyCondsRemoved = true;
                    }
                }

                if (wereAnyCondsRemoved)
                {
                    actionPhraseCNP = ActionPhrase(CREATURE_CAST_UPON_PTR);
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
            case Spells::Trip:
            {
                if (CREATURE_CAST_UPON_PTR->HasCondition(creature::Conditions::Tripped))
                {
                    actionPhraseCNP = combat::ContentAndNamePos(
                        Spell::FAILED_BECAUSE_STR_,
                        " is already tripped.",
                        "",
                        combat::NamePosition::TargetBefore);

                    return false;
                }
                else if (combat::Encounter::Instance()
                             ->GetTurnInfoCopy(CREATURE_CAST_UPON_PTR)
                             .GetIsFlying())
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
                    CREATURE_CAST_UPON_PTR->ConditionAdd(creature::Conditions::Tripped);
                    condsAddedVec.emplace_back(creature::Conditions::Tripped);
                    actionPhraseCNP = ActionPhrase(CREATURE_CAST_UPON_PTR);
                    return true;
                }
            }
            case Spells::Lift:
            {
                if (CREATURE_CAST_UPON_PTR->HasCondition(creature::Conditions::Tripped))
                {
                    CREATURE_CAST_UPON_PTR->ConditionRemove(creature::Conditions::Tripped);
                    condsRemovedVec.emplace_back(creature::Conditions::Tripped);
                    actionPhraseCNP = ActionPhrase(CREATURE_CAST_UPON_PTR);
                    return true;
                }
                else if (CREATURE_CAST_UPON_PTR->HasCondition(creature::Conditions::Pounced))
                {
                    CREATURE_CAST_UPON_PTR->ConditionRemove(creature::Conditions::Pounced);
                    condsRemovedVec.emplace_back(creature::Conditions::Pounced);
                    actionPhraseCNP = ActionPhrase(CREATURE_CAST_UPON_PTR);
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
            case Spells::Daze:
            {
                if (CREATURE_CAST_UPON_PTR->HasCondition(creature::Conditions::Dazed))
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
                    CREATURE_CAST_UPON_PTR->ConditionAdd(creature::Conditions::Dazed);
                    condsAddedVec.emplace_back(creature::Conditions::Dazed);
                    actionPhraseCNP = ActionPhrase(CREATURE_CAST_UPON_PTR);
                    return true;
                }
            }
            case Spells::Panic:
            {
                if (CREATURE_CAST_UPON_PTR->HasCondition(creature::Conditions::Panic))
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
                    CREATURE_CAST_UPON_PTR->ConditionAdd(creature::Conditions::Panic);
                    condsAddedVec.emplace_back(creature::Conditions::Panic);
                    actionPhraseCNP = ActionPhrase(CREATURE_CAST_UPON_PTR);
                    return true;
                }
            }
            case Spells::ClearMind:
            {
                const creature::CondEnumVec_t CONDS_TO_REMOVE_VEC{ creature::Conditions::Panic,
                                                                   creature::Conditions::Daunted,
                                                                   creature::Conditions::Dazed };

                auto wereAnyCondsRemoved{ false };
                for (auto const NEXT_COND_ENUM_TO_REMOVE : CONDS_TO_REMOVE_VEC)
                {
                    if (CREATURE_CAST_UPON_PTR->HasCondition(NEXT_COND_ENUM_TO_REMOVE))
                    {
                        CREATURE_CAST_UPON_PTR->ConditionRemove(NEXT_COND_ENUM_TO_REMOVE);
                        condsRemovedVec.emplace_back(NEXT_COND_ENUM_TO_REMOVE);
                        wereAnyCondsRemoved = true;
                    }
                }

                if (wereAnyCondsRemoved)
                {
                    actionPhraseCNP = ActionPhrase(CREATURE_CAST_UPON_PTR);
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
            case Spells::Poison:
            {
                if (CREATURE_CAST_UPON_PTR->HasCondition(creature::Conditions::Poisoned))
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
                    CREATURE_CAST_UPON_PTR->ConditionAdd(creature::Conditions::Poisoned);
                    condsAddedVec.emplace_back(creature::Conditions::Poisoned);
                    actionPhraseCNP = ActionPhrase(CREATURE_CAST_UPON_PTR);
                    return true;
                }
            }
            case Spells::Antidote:
            {
                if (CREATURE_CAST_UPON_PTR->HasCondition(creature::Conditions::Poisoned))
                {
                    CREATURE_CAST_UPON_PTR->ConditionRemove(creature::Conditions::Poisoned);
                    condsRemovedVec.emplace_back(creature::Conditions::Poisoned);
                    actionPhraseCNP = ActionPhrase(CREATURE_CAST_UPON_PTR);
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
            case Spells::PoisonCloud:
            {
                if (CREATURE_CAST_UPON_PTR->HasCondition(creature::Conditions::Poisoned))
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
                        CREATURE_CASTING_PTR,
                        stats::Traits::Intelligence,
                        CREATURE_CAST_UPON_PTR,
                        stats::Traits::Count,
                        0,
                        0,
                        static_cast<creature::Stats::With>(
                            creature::Stats::With::Luck | creature::Stats::With::RaceRoleBonus
                            | creature::Stats::With::RankBonus
                            | creature::Stats::With::PlayerNaturalWins)) };

                    auto const DID_MAGIC_CAST_TRAIT_BONUS_SUCCEED{
                        CREATURE_CASTING_PTR->TraitBonusTest(stats::Traits::MagicCast)
                    };

                    if (DID_STAT_ROLL_SUCCEED || DID_MAGIC_CAST_TRAIT_BONUS_SUCCEED)
                    {
                        CREATURE_CAST_UPON_PTR->ConditionAdd(creature::Conditions::Poisoned);
                        condsAddedVec.emplace_back(creature::Conditions::Poisoned);
                        actionPhraseCNP = ActionPhrase(CREATURE_CAST_UPON_PTR);
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
            case Spells::Count:
            default:
            {
                std::ostringstream ss;
                ss << "spell::Spell::EffectCreature(castingCreature="
                   << CREATURE_CASTING_PTR->ToString()
                   << ", creatureCastUpon=" << CREATURE_CAST_UPON_PTR->ToString()
                   << ") was called when Which() was invalid (" << Which() << ").";

                throw std::runtime_error(ss.str());
            }
        }
    }

    const std::string Spell::EffectItem(const creature::CreaturePtr_t, const item::ItemPtr_t) const
    {
        // TODO
        return "";
    }

} // namespace spell
} // namespace heroespath
