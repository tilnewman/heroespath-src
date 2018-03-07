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
// creature-effect.cpp
//
#include "creature-effect.hpp"

#include "creature/condition-algorithms.hpp"
#include "creature/condition.hpp"
#include "creature/creature.hpp"

#include "misc/vectors.hpp"

#include <boost/algorithm/string.hpp>

#include <algorithm>

namespace heroespath
{
namespace combat
{

    CreatureEffect::CreatureEffect(
        const creature::CreaturePtr_t CREATURE_PTR, const HitInfoVec_t & HIT_INFO_VEC)
        : hitInfoVec_(HIT_INFO_VEC)
        , creaturePtr_(CREATURE_PTR)
    {}

    bool CreatureEffect::GetWasHit() const
    {
        for (auto const & NEXT_HIT_INFO : hitInfoVec_)
        {
            if (NEXT_HIT_INFO.WasHit())
            {
                return true;
            }
        }

        return false;
    }

    Health_t CreatureEffect::GetDamageTotal() const
    {
        Health_t damageTotal{ 0_health };

        for (auto const & NEXT_HIT_INFO : hitInfoVec_)
        {
            damageTotal += NEXT_HIT_INFO.Damage();
        }

        return damageTotal;
    }

    const creature::CondEnumVec_t CreatureEffect::GetAllCondsAdded() const
    {
        creature::CondEnumVec_t conditionsVec;

        for (auto const & NEXT_HIT_INFO : hitInfoVec_)
        {
            const auto NEXT_CONDITION_VEC{ NEXT_HIT_INFO.CondsAdded() };

            if (NEXT_CONDITION_VEC.empty() == false)
            {
                conditionsVec.insert(
                    conditionsVec.end(), NEXT_CONDITION_VEC.begin(), NEXT_CONDITION_VEC.end());
            }
        }

        return conditionsVec;
    }

    const creature::CondEnumVec_t CreatureEffect::GetAllCondsRemoved() const
    {
        creature::CondEnumVec_t conditionsVec;

        for (auto const & NEXT_HIT_INFO : hitInfoVec_)
        {
            const auto NEXT_CONDITION_VEC{ NEXT_HIT_INFO.CondsRemoved() };

            if (NEXT_CONDITION_VEC.empty() == false)
            {
                conditionsVec.insert(
                    conditionsVec.end(), NEXT_CONDITION_VEC.begin(), NEXT_CONDITION_VEC.end());
            }
        }

        return conditionsVec;
    }

    bool CreatureEffect::GetAllCondsAddedContains(const creature::Conditions::Enum E) const
    {
        const auto VEC{ GetAllCondsAdded() };
        return (std::find(VEC.begin(), VEC.end(), E) != VEC.end());
    }

    bool CreatureEffect::GetAllCondsRemovedContains(const creature::Conditions::Enum E) const
    {
        const auto VEC{ GetAllCondsRemoved() };
        return (std::find(VEC.begin(), VEC.end(), E) != VEC.end());
    }

    bool CreatureEffect::AllCondsAddedRemove(const creature::Conditions::Enum E)
    {
        auto wasCondRemoved{ false };

        for (auto & nextHitInfo : hitInfoVec_)
        {
            if (nextHitInfo.CondsAddedRemove(E))
            {
                wasCondRemoved = true;
            }
        }

        return wasCondRemoved;
    }

    bool CreatureEffect::AllCondsRemovedRemove(const creature::Conditions::Enum E)
    {
        auto wasCondRemoved{ false };

        for (auto & nextHitInfo : hitInfoVec_)
        {
            if (nextHitInfo.CondsRemovedRemove(E))
            {
                wasCondRemoved = true;
            }
        }

        return wasCondRemoved;
    }

    bool operator<(const CreatureEffect & L, const CreatureEffect & R)
    {
        return std::tie(L.hitInfoVec_, L.creaturePtr_) < std::tie(R.hitInfoVec_, R.creaturePtr_);
    }

    bool operator==(const CreatureEffect & L, const CreatureEffect & R)
    {
        return std::tie(L.hitInfoVec_, L.creaturePtr_) == std::tie(R.hitInfoVec_, R.creaturePtr_);
    }
} // namespace combat
} // namespace heroespath
