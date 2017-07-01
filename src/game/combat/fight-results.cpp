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
// fight-results.cpp
//
#include "fight-results.hpp"

#include "game/item/item.hpp"
#include "game/creature/condition.hpp"
#include "game/creature/condition-algorithms.hpp"
#include "game/creature/creature.hpp"
#include "game/spell/spell-base.hpp"
#include "game/song/song.hpp"

#include "misc/vectors.hpp"

#include <sstream>
#include <exception>
#include <algorithm>


namespace game
{
namespace combat
{

    std::size_t FightResultSummary::PtrCount() const
    {
        return (effected_pvec.size() + resisted_pvec.size() + already_pvec.size());
    }


    bool FightResultSummary::IsValid() const
    {
        if (PtrCount() <= 1)
        {
            return false;
        }
        
        if ((HitType::Roar != hit_type) &&
            (HitType::Song != hit_type) &&
            (HitType::Spell != hit_type))
        {
            return false;
        }

        if ((HitType::Spell == hit_type) && (spell_ptr == nullptr))
        {
            return false;
        }

        if ((HitType::Song == hit_type) && (song_ptr == nullptr))
        {
            return false;
        }
        
        return true;
    }


    const std::string FightResultSummary::VerbThirdPerson() const
    {
        if (HitType::Song == hit_type)
        {
            return song_ptr->VerbThirdPerson();
        }
        else if (HitType::Spell == hit_type)
        {
            return spell_ptr->VerbThirdPerson();
        }
        else if (HitType::Roar == hit_type)
        {
            return "panics";
        }
        else
        {
            return "";
        }
    }


    const std::string FightResultSummary::Compose(
        const std::string & FIGHTING_CREATURE_NAME,
        const std::string & VERB_PAST_TENSE) const
    {
        if (IsValid() == false)
        {
            return "";
        }

        std::ostringstream preSS;
        preSS << FIGHTING_CREATURE_NAME;

        switch (hit_type)
        {
            case HitType::Spell:
            {
                preSS << " casts " << spell_ptr->Name();
                break;
            }
            case HitType::Song:
            {
                preSS << song_ptr->ActionPhrasePreamble() << " playing " << song_ptr->Name();
                break;
            }
            case HitType::Roar:
            {
                preSS << " roars";
                break;
            }
            case HitType::Count:
            case HitType::Weapon:
            case HitType::Pounce:
            case HitType::Condition:
            default:
            {
                return "";
            }
        }

        preSS << " at " << PtrCount() + 1;

        std::ostringstream postSS;
        if (resisted_pvec.empty() && already_pvec.empty())
        {
            postSS << " and " << VerbThirdPerson() << " ALL of them";
        }
        else if (effected_pvec.empty() && already_pvec.empty())
        {
            postSS << " but ALL resisted";
        }
        else if (effected_pvec.empty() && resisted_pvec.empty())
        {
            postSS << " but ALL were already " << VERB_PAST_TENSE;
        }
        else
        {
            if (effected_pvec.empty() == false)
            {
                postSS << " and " << VerbThirdPerson() << " " << effected_pvec.size();
            }

            if (resisted_pvec.empty() == false)
            {
                if (postSS.str().empty())
                {
                    postSS << " and ";
                }
                else
                {
                    postSS << ", but ";
                }

                postSS << resisted_pvec.size() << " resisted";
            }

            if (already_pvec.empty() == false)
            {
                postSS << ", and " << already_pvec.size() << " were already " << VERB_PAST_TENSE;
            }
        }

        postSS << ".";
        return preSS.str() + postSS.str();
    }


    FightResult::FightResult(const CreatureEffectVec_t & CREATURE_EFFECT_VEC)
    :
        creatureEffectVec_(CREATURE_EFFECT_VEC)
    {}


    FightResult::FightResult(const CreatureEffect & CREATURE_EFFECT)
    :
        creatureEffectVec_(1, CREATURE_EFFECT)
    {}


    const HitInfo FightResult::GetHitInfo(const std::size_t EFFECT_INDEX,
                                          const std::size_t HIT_INDEX) const
    {
        if (EFFECT_INDEX < creatureEffectVec_.size())
        {
            auto const HIT_INFO_VEC{ creatureEffectVec_[EFFECT_INDEX].GetHitInfoVec() };
            if (HIT_INDEX < HIT_INFO_VEC.size())
            {
                return HIT_INFO_VEC[HIT_INDEX];
            }
        }

        return HitInfo();//invalid HitInfo object
    }


    std::size_t FightResult::EffectedCreatures(
        creature::CreaturePVec_t & CreaturePVec_OutParam) const
    {
        for (auto const & NEXT_CREATURE_EFFECT : creatureEffectVec_)
        {
            CreaturePVec_OutParam.push_back(NEXT_CREATURE_EFFECT.GetCreature());
        }

        return creatureEffectVec_.size();
    }

}
}
