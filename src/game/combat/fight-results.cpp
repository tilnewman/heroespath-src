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

#include "misc/vectors.hpp"

#include <sstream>
#include <exception>
#include <algorithm>


namespace game
{
namespace combat
{

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
