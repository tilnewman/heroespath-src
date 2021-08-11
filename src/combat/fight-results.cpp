// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// fight-results.cpp
//
#include "fight-results.hpp"

#include "creature/condition-algorithms.hpp"
#include "creature/condition.hpp"
#include "creature/creature.hpp"
#include "item/item.hpp"
#include "misc/vectors.hpp"
#include "song/song.hpp"
#include "spell/spell.hpp"

#include <algorithm>
#include <exception>
#include <sstream>
#include <string>

namespace heroespath
{
namespace combat
{

    FightResultSummary::FightResultSummary()
        : hit_type(HitType::Count)
        , song_ptr_opt()
        , spell_ptr_opt()
        , effected_vec()
        , resisted_vec()
        , already_vec()
        , areResistedNotEffected_(false)
    {}

    std::size_t FightResultSummary::PtrCount() const
    {
        return (effected_vec.size() + resisted_vec.size() + already_vec.size());
    }

    bool FightResultSummary::IsValid() const
    {
        if (PtrCount() <= 1)
        {
            return false;
        }

        if ((HitType::Roar != hit_type) && (HitType::Song != hit_type)
            && (HitType::Spell != hit_type))
        {
            return false;
        }

        if ((HitType::Spell == hit_type) && (!spell_ptr_opt))
        {
            return false;
        }

        if ((HitType::Song == hit_type) && (!song_ptr_opt))
        {
            return false;
        }

        return true;
    }

    const std::string FightResultSummary::VerbThirdPerson() const
    {
        if (HitType::Song == hit_type)
        {
            return song_ptr_opt.value()->VerbThirdPerson();
        }
        else if (HitType::Spell == hit_type)
        {
            return spell_ptr_opt.value()->VerbThirdPerson();
        }
        else // roar case
        {
            return "panics";
        }
    }

    const std::string FightResultSummary::Compose(
        const std::string & FIGHTING_CREATURE_NAME, const std::string & VERB_PAST_TENSE) const
    {
        if (IsValid() == false)
        {
            return "";
        }

        std::ostringstream preSS;
        preSS << FIGHTING_CREATURE_NAME;

        if (HitType::Spell == hit_type)
        {
            preSS << " casts " << spell_ptr_opt.value()->Name();
        }
        else if (HitType::Song == hit_type)
        {
            // intentionally no prepend of empty space so that preamble can start with "'s"
            preSS << song_ptr_opt.value()->ActionPhrasePreamble() << " playing "
                  << song_ptr_opt.value()->Name();
        }
        else
        {
            preSS << " roars";
        }

        preSS << " at " << PtrCount();

        std::ostringstream postSS;
        if (resisted_vec.empty() && already_vec.empty())
        {
            postSS << " and " << VerbThirdPerson() << " ALL of them";
        }
        else if (effected_vec.empty() && already_vec.empty())
        {
            postSS << " but ALL resisted";
        }
        else if (effected_vec.empty() && resisted_vec.empty())
        {
            postSS << " but ALL were already " << VERB_PAST_TENSE;
        }
        else
        {
            if (effected_vec.empty() == false)
            {
                postSS << " and " << VerbThirdPerson() << "  " << effected_vec.size();
            }

            if (resisted_vec.empty() == false)
            {
                if (postSS.str().empty())
                {
                    postSS << " and ";
                }
                else
                {
                    postSS << ", but ";
                }

                if (areResistedNotEffected_)
                {
                    postSS << resisted_vec.size() << " were not effected";
                }
                else
                {
                    postSS << resisted_vec.size() << " resisted";
                }
            }

            if (already_vec.empty() == false)
            {
                postSS << ", and " << already_vec.size() << " were already " << VERB_PAST_TENSE;
            }
        }

        postSS << ".";
        return preSS.str() + postSS.str();
    }

    FightResult::FightResult(const CreatureEffectVec_t & CREATURE_EFFECT_VEC)
        : creatureEffectVec_(CREATURE_EFFECT_VEC)
    {}

    FightResult::FightResult(const CreatureEffect & CREATURE_EFFECT)
        : creatureEffectVec_(1, CREATURE_EFFECT)
    {}

    const HitInfo
        FightResult::GetHitInfo(const std::size_t EFFECT_INDEX, const std::size_t HIT_INDEX) const
    {
        if (EFFECT_INDEX < creatureEffectVec_.size())
        {
            const auto HIT_INFO_VEC { creatureEffectVec_[EFFECT_INDEX].GetHitInfoVec() };
            if (HIT_INDEX < HIT_INFO_VEC.size())
            {
                return HIT_INFO_VEC[HIT_INDEX];
            }
        }

        // invalid HitInfo object
        return HitInfo();
    }

    std::size_t
        FightResult::EffectedCreatures(creature::CreaturePVec_t & CreaturePVec_OutParam) const
    {
        for (const auto & NEXT_CREATURE_EFFECT : creatureEffectVec_)
        {
            CreaturePVec_OutParam.emplace_back(NEXT_CREATURE_EFFECT.GetCreature());
        }

        return creatureEffectVec_.size();
    }

} // namespace combat
} // namespace heroespath
