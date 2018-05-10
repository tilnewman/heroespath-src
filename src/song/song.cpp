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
// song-base.cpp
//
#include "song.hpp"

#include "combat/encounter.hpp"
#include "creature/creature.hpp"
#include "creature/stats.hpp"
#include "misc/random.hpp"
#include "misc/strings.hpp"

#include <algorithm>
#include <sstream>

namespace heroespath
{
namespace song
{

    const std::string Song::RESISTED_STR_{ " resisted." };
    const std::string Song::FAILED_STR_{ " failed on " };

    Song::Song(
        const Songs::Enum WHICH,
        const SongType::Enum TYPE,
        const combat::EffectType::Enum EFFECT_TYPE,
        const game::Phase::Enum VALID_PHASES,
        const Mana_t & MANA_COST,
        const Rank_t & RANK,
        const combat::TargetType::Enum TARGET_TYPE,
        const std::string & VERB_THIRD_PERSON,
        const std::string & VERB_PAST_TENSE)
        : which_(WHICH)
        , type_(TYPE)
        , effectType_(EFFECT_TYPE)
        , rank_(RANK)
        , validPhases_(VALID_PHASES)
        , manaCost_(MANA_COST)
        , targetType_(TARGET_TYPE)
        , verbThirdPerson_(VERB_THIRD_PERSON)
        , verbPastTense_(VERB_PAST_TENSE)
    {
        // only some TargetTypes are valid for songs
        M_ASSERT_OR_LOGANDTHROW_SS(
            ((TARGET_TYPE == combat::TargetType::AllCompanions)
             || (TARGET_TYPE == combat::TargetType::AllOpponents)
             || (TARGET_TYPE == combat::TargetType::QuestSpecific)),
            "song::Song::Constructor(which="
                << Songs::ToString(WHICH) << ", song_type=" << SongType::ToString(TYPE)
                << ", effect_type=" << combat::EffectType::ToString(EFFECT_TYPE)
                << ", mana_cost=" << MANA_COST << ", rank=" << RANK
                << ") had an invalid TargetType of " << combat::TargetType::ToString(TARGET_TYPE)
                << ".  The only allowed TargetTypes are AllCompanions, AllOpponents, and "
                << "QuestSpecific.");
    }

    const std::string Song::ToString() const
    {
        std::ostringstream ss;
        ss << Name() << "  -" << DescDetails();
        return ss.str();
    }

    const std::string Song::DescDetails() const
    {
        std::ostringstream ss;
        ss << "A " << misc::String::DecorateNumber(rank_.As<int>()) << " rank"
           << combat::EffectType::Name(effectType_) << " magical ";

        if (SongType::Guitar == type_)
        {
            ss << "guitar tune";
        }
        else
        {
            ss << "drum beat";
        }

        ss << " that can be played during " << game::Phase::ToString(validPhases_, false)
           << ", targeting " << combat::TargetType::Name(targetType_) << ", and costing "
           << manaCost_ << " mana.";

        return ss.str();
    }

    const std::string Song::DescComplete() const
    {
        std::ostringstream ss;
        ss << Desc() << "  " << DescExtra() << " " << DescDetails();
        return ss.str();
    }

    bool Song::EffectCreature(
        const creature::CreaturePtr_t CREATURE_PLAYING_PTR,
        const creature::CreaturePtr_t CREATURE_LISTENING_PTR,
        Health_t &,
        creature::CondEnumVec_t & condsAddedVec,
        creature::CondEnumVec_t & condsRemovedVec,
        combat::ContentAndNamePos & actionPhraseCNP) const
    {
        switch (Which())
        {
            case Songs::RallyDrum:
            {
                if (CREATURE_LISTENING_PTR->HasCondition(creature::Conditions::Bold))
                {
                    actionPhraseCNP = combat::ContentAndNamePos(
                        "",
                        "'s " + TypeToVerb() + Song::FAILED_STR_,
                        " because " + creature::sex::HeSheIt(CREATURE_LISTENING_PTR->Sex(), false)
                            + " is already emboldened.",
                        combat::NamePosition::SourceThenTarget);

                    return false;
                }
                else
                {
                    auto const DID_STAT_ROLL_SUCCEED{ creature::Stats::Test(
                        CREATURE_PLAYING_PTR,
                        { stats::Traits::Intelligence, stats::Traits::Charm },
                        static_cast<creature::Stats::With>(
                            creature::Stats::With::Luck | creature::Stats::With::RaceRoleBonus)) };

                    auto const DID_MAGIC_CAST_TRAIT_BONUS_SUCCEED{
                        CREATURE_PLAYING_PTR->TraitBonusTest(stats::Traits::MagicCast)
                    };

                    if (DID_STAT_ROLL_SUCCEED || DID_MAGIC_CAST_TRAIT_BONUS_SUCCEED)
                    {
                        CREATURE_LISTENING_PTR->ConditionAdd(creature::Conditions::Bold);
                        condsAddedVec.emplace_back(creature::Conditions::Bold);

                        actionPhraseCNP = combat::ContentAndNamePos(
                            "",
                            "'s magical " + TypeToVerb() + " emboldens ",
                            ".",
                            combat::NamePosition::SourceThenTarget);

                        return true;
                    }
                    else
                    {
                        actionPhraseCNP = combat::ContentAndNamePos(
                            "",
                            "'s " + TypeToVerb() + Song::FAILED_STR_,
                            " because "
                                + creature::sex::HeSheIt(CREATURE_LISTENING_PTR->Sex(), false)
                                + Song::RESISTED_STR_,
                            combat::NamePosition::SourceThenTarget);

                        return false;
                    }
                }
            }
            case Songs::SpiritResonance:

            {
                auto const RATIO_FROM_STATS{ creature::Stats::Ratio(
                    CREATURE_PLAYING_PTR,
                    { stats::Traits::Intelligence, stats::Traits::Charm },
                    (creature::Stats::Luck | creature::Stats::RaceRoleBonus)) };

                auto const RATIO_FROM_TRAIT_BONUS{ static_cast<float>(
                                                       CREATURE_PLAYING_PTR->TraitBonusCurrent(
                                                           stats::Traits::MagicEffect))
                                                   / 100.0f };

                auto const MANA_GAIN_ORIG{ Mana_t::Make(
                    10.0f * (RATIO_FROM_STATS + RATIO_FROM_TRAIT_BONUS)) };

                auto const MANA_GAIN_MAX{ CREATURE_LISTENING_PTR->ManaMissing() };

                auto const MANA_GAIN_FINAL{ (
                    (MANA_GAIN_ORIG > MANA_GAIN_MAX) ? MANA_GAIN_MAX : MANA_GAIN_ORIG) };

                if (MANA_GAIN_FINAL > 0_mana)
                {
                    CREATURE_LISTENING_PTR->ManaAdj(MANA_GAIN_FINAL);

                    std::ostringstream ss;
                    ss << "'s mana for " << MANA_GAIN_FINAL << ".";

                    actionPhraseCNP = combat::ContentAndNamePos(
                        "",
                        "'s " + TypeToVerb() + " recharges ",
                        ss.str(),
                        combat::NamePosition::SourceThenTarget);

                    return true;
                }
                else
                {
                    actionPhraseCNP = combat::ContentAndNamePos(
                        "",
                        "'s " + TypeToVerb() + Song::FAILED_STR_,
                        " because " + creature::sex::HeSheIt(CREATURE_LISTENING_PTR->Sex(), false)
                            + " already has full mana.",
                        combat::NamePosition::SourceThenTarget);

                    return false;
                }
            }
            case Songs::RousingRhythm:
            {
                const creature::CondEnumVec_t CONDS_TO_REMOVE_VEC{
                    creature::Conditions::AsleepNatural,
                    creature::Conditions::AsleepMagical,
                    creature::Conditions::Dazed,
                    creature::Conditions::Unconscious
                };

                creature::CondEnumVec_t condsToRemoveVec;
                for (auto const NEXT_COND_TO_REMOVE_ENUM : CONDS_TO_REMOVE_VEC)
                {
                    if (CREATURE_LISTENING_PTR->HasCondition(NEXT_COND_TO_REMOVE_ENUM))
                    {
                        CREATURE_LISTENING_PTR->ConditionRemove(NEXT_COND_TO_REMOVE_ENUM);
                        condsToRemoveVec.emplace_back(NEXT_COND_TO_REMOVE_ENUM);
                    }
                }

                if (condsToRemoveVec.empty())
                {
                    actionPhraseCNP = combat::ContentAndNamePos(
                        "",
                        "'s " + TypeToVerb() + Song::FAILED_STR_,
                        " because " + creature::sex::HeSheIt(CREATURE_LISTENING_PTR->Sex(), false)
                            + " did not need rousing.",
                        combat::NamePosition::SourceThenTarget);

                    return false;
                }
                else
                {
                    condsRemovedVec.insert(
                        condsRemovedVec.end(), condsToRemoveVec.begin(), condsToRemoveVec.end());

                    actionPhraseCNP = combat::ContentAndNamePos(
                        "",
                        "'s " + TypeToVerb() + " rouses ",
                        ".",
                        combat::NamePosition::SourceThenTarget);

                    return true;
                }
            }
            case Songs::TripBeat:
            {
                if (CREATURE_LISTENING_PTR->HasCondition(creature::Conditions::Tripped))
                {
                    actionPhraseCNP = combat::ContentAndNamePos(
                        "",
                        "'s " + TypeToVerb() + Song::FAILED_STR_,
                        " because " + creature::sex::HeSheIt(CREATURE_LISTENING_PTR->Sex(), false)
                            + " is already tripped.",
                        combat::NamePosition::SourceThenTarget);

                    return false;
                }
                else if (combat::Encounter::Instance()
                             ->GetTurnInfoCopy(CREATURE_LISTENING_PTR)
                             .GetIsFlying())
                {
                    actionPhraseCNP = combat::ContentAndNamePos(
                        "",
                        "'s " + TypeToVerb() + Song::FAILED_STR_,
                        " because " + creature::sex::HeSheIt(CREATURE_LISTENING_PTR->Sex(), false)
                            + " is flying.",
                        combat::NamePosition::SourceThenTarget);

                    return false;
                }
                else
                {
                    auto const DID_STAT_ROLL_SUCCEED{ creature::Stats::Versus(
                        CREATURE_PLAYING_PTR,
                        { stats::Traits::Intelligence, stats::Traits::Charm },
                        CREATURE_LISTENING_PTR,
                        {},
                        0,
                        0,
                        static_cast<creature::Stats::With>(
                            creature::Stats::With::Luck | creature::Stats::With::RaceRoleBonus
                            | creature::Stats::With::RankBonus
                            | creature::Stats::With::PlayerNaturalWins)) };

                    auto const DID_MAGIC_CAST_TRAIT_BONUS_SUCCEED{
                        CREATURE_PLAYING_PTR->TraitBonusTest(stats::Traits::MagicCast)
                    };

                    if (DID_STAT_ROLL_SUCCEED || DID_MAGIC_CAST_TRAIT_BONUS_SUCCEED)
                    {
                        CREATURE_LISTENING_PTR->ConditionAdd(creature::Conditions::Tripped);
                        condsAddedVec.emplace_back(creature::Conditions::Tripped);

                        actionPhraseCNP = combat::ContentAndNamePos(
                            "",
                            "'s " + TypeToVerb() + " trips ",
                            ".",
                            combat::NamePosition::SourceThenTarget);

                        return true;
                    }
                    else
                    {
                        actionPhraseCNP = combat::ContentAndNamePos(
                            "",
                            "'s " + TypeToVerb() + Song::FAILED_STR_,
                            " because "
                                + creature::sex::HeSheIt(CREATURE_LISTENING_PTR->Sex(), false)
                                + Song::RESISTED_STR_,
                            combat::NamePosition::SourceThenTarget);

                        return false;
                    }
                }
            }
            case Songs::PanicStrings:
            {
                if (CREATURE_LISTENING_PTR->HasCondition(creature::Conditions::Panic))
                {
                    actionPhraseCNP = combat::ContentAndNamePos(
                        "",
                        "'s " + TypeToVerb() + Song::FAILED_STR_,
                        " because " + creature::sex::HeSheIt(CREATURE_LISTENING_PTR->Sex(), false)
                            + " is already panicked.",
                        combat::NamePosition::SourceThenTarget);

                    return false;
                }
                else
                {
                    auto const DID_STAT_ROLL_SUCCEED{ creature::Stats::Versus(
                        CREATURE_PLAYING_PTR,
                        { stats::Traits::Intelligence, stats::Traits::Charm },
                        CREATURE_LISTENING_PTR,
                        {},
                        0,
                        0,
                        static_cast<creature::Stats::With>(
                            creature::Stats::With::Luck | creature::Stats::With::RaceRoleBonus
                            | creature::Stats::With::RankBonus
                            | creature::Stats::With::PlayerNaturalWins)) };

                    auto const DID_MAGIC_CAST_TRAIT_BONUS_SUCCEED{
                        CREATURE_PLAYING_PTR->TraitBonusTest(stats::Traits::MagicCast)
                    };

                    if (DID_STAT_ROLL_SUCCEED || DID_MAGIC_CAST_TRAIT_BONUS_SUCCEED)
                    {
                        CREATURE_LISTENING_PTR->ConditionAdd(creature::Conditions::Panic);
                        condsAddedVec.emplace_back(creature::Conditions::Panic);

                        actionPhraseCNP = combat::ContentAndNamePos(
                            "",
                            "'s " + TypeToVerb() + " panics ",
                            ".",
                            combat::NamePosition::SourceThenTarget);

                        return true;
                    }
                    else
                    {
                        actionPhraseCNP = combat::ContentAndNamePos(
                            "",
                            "'s " + TypeToVerb() + Song::FAILED_STR_,
                            " because "
                                + creature::sex::HeSheIt(CREATURE_LISTENING_PTR->Sex(), false)
                                + Song::RESISTED_STR_,
                            combat::NamePosition::SourceThenTarget);

                        return false;
                    }
                }
            }
            case Songs::Lullaby:
            {
                if ((CREATURE_LISTENING_PTR->HasCondition(creature::Conditions::AsleepNatural))
                    && (CREATURE_LISTENING_PTR->HasCondition(creature::Conditions::AsleepMagical)))
                {
                    actionPhraseCNP = combat::ContentAndNamePos(
                        "",
                        "'s " + TypeToVerb() + Song::FAILED_STR_,
                        " because " + creature::sex::HeSheIt(CREATURE_LISTENING_PTR->Sex(), false)
                            + " is already sleeping.",
                        combat::NamePosition::SourceThenTarget);

                    return false;
                }
                else
                {
                    auto const DID_STAT_ROLL_SUCCEED{ creature::Stats::Test(
                        CREATURE_PLAYING_PTR,
                        { stats::Traits::Intelligence, stats::Traits::Charm },
                        static_cast<creature::Stats::With>(
                            creature::Stats::With::Luck | creature::Stats::With::RaceRoleBonus)) };

                    auto const DID_MAGIC_CAST_TRAIT_BONUS_SUCCEED{
                        CREATURE_PLAYING_PTR->TraitBonusTest(stats::Traits::MagicCast)
                    };

                    if (DID_STAT_ROLL_SUCCEED || DID_MAGIC_CAST_TRAIT_BONUS_SUCCEED)
                    {
                        CREATURE_LISTENING_PTR->ConditionAdd(creature::Conditions::AsleepNatural);
                        condsAddedVec.emplace_back(creature::Conditions::AsleepNatural);

                        actionPhraseCNP = combat::ContentAndNamePos(
                            "",
                            "'s " + TypeToVerb() + " puts ",
                            " into a peaceful slumber.",
                            combat::NamePosition::SourceThenTarget);

                        combat::Encounter::Instance()->SetIsFlying(CREATURE_LISTENING_PTR, false);
                        return true;
                    }
                    else
                    {
                        actionPhraseCNP = combat::ContentAndNamePos(
                            "",
                            "'s " + TypeToVerb() + Song::FAILED_STR_,
                            " because "
                                + creature::sex::HeSheIt(CREATURE_LISTENING_PTR->Sex(), false)
                                + Song::RESISTED_STR_,
                            combat::NamePosition::SourceThenTarget);

                        return false;
                    }
                }
            }
            case Songs::Count:
            default:
            {
                std::ostringstream ss;
                ss << "song::Song::EffectCreature(creaturePlaying="
                   << CREATURE_PLAYING_PTR->ToString()
                   << ", creatureListening=" << CREATURE_LISTENING_PTR->ToString()
                   << ") was called when Which() was invalid (" << Which() << ").";

                throw std::runtime_error(ss.str());
            }
        }
    }

    bool Song::EffectItem(const creature::CreaturePtr_t, const item::ItemPtr_t) const
    {
        // TODO
        return false;
    }

    const std::string Song::ActionPhrasePreamble() const
    {
        std::ostringstream ss;

        ss << "'s ";

        if (SongType::Guitar == type_)
        {
            ss << "fingers ";

            switch (misc::random::Int(3))
            {
                case 1:
                {
                    ss << "tickle";
                    break;
                }
                case 2:
                {
                    ss << "pluck";
                    break;
                }
                case 3:
                {
                    ss << "strum";
                    break;
                }
                default:
                {
                    ss << "dance along";
                    break;
                }
            }

            ss << " the strings";
        }
        else
        {
            ss << "hands ";

            switch (misc::random::Int(3))
            {
                case 1:
                {
                    ss << "tap";
                    break;
                }
                case 2:
                {
                    ss << "rap";
                    break;
                }
                case 3:
                {
                    ss << "drum";
                    break;
                }
                default:
                {
                    ss << "pound";
                    break;
                }
            }

            ss << " the back of the lute";
        }

        return ss.str();
    }

    const std::string Song::TypeToVerb() const
    {
        if (SongType::Guitar == type_)
        {
            switch (misc::random::Int(3))
            {
                case 1:
                {
                    return "melody";
                }
                case 2:
                {
                    return "tune";
                }
                case 3:
                {
                    return "song";
                }
                default:
                {
                    return "strumming";
                }
            }
        }
        else
        {
            switch (misc::random::Int(3))
            {
                case 1:
                {
                    return "drumming";
                }
                case 2:
                {
                    return "pounding";
                }
                case 3:
                {
                    return "rapping";
                }
                default:
                {
                    return "tapping";
                }
            }
        }
    }

    const std::string Song::TypeToNoun() const
    {
        if (SongType::Guitar == type_)
        {
            switch (misc::random::Int(2))
            {
                case 1:
                {
                    return "melody";
                }
                case 2:
                {
                    return "tune";
                }
                default:
                {
                    return "song";
                }
            }
        }
        else
        {
            if (misc::random::Bool())
            {
                return "rhythm";
            }
            else
            {
                return "beat";
            }
        }
    }

} // namespace song
} // namespace heroespath
