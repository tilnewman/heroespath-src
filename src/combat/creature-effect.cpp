// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// creature-effect.cpp
//
#include "creature-effect.hpp"

#include "creature/condition-algorithms.hpp"
#include "creature/condition.hpp"
#include "creature/creature.hpp"
#include "misc/boost-string-includes.hpp"
#include "misc/vectors.hpp"

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
        for (const auto & NEXT_HIT_INFO : hitInfoVec_)
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
        Health_t damageTotal { 0_health };

        for (const auto & NEXT_HIT_INFO : hitInfoVec_)
        {
            damageTotal += NEXT_HIT_INFO.Damage();
        }

        return damageTotal;
    }

    const creature::CondEnumVec_t CreatureEffect::GetAllCondsAdded() const
    {
        creature::CondEnumVec_t conditionsVec;

        for (const auto & NEXT_HIT_INFO : hitInfoVec_)
        {
            const auto NEXT_CONDITION_VEC { NEXT_HIT_INFO.CondsAdded() };

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

        for (const auto & NEXT_HIT_INFO : hitInfoVec_)
        {
            const auto NEXT_CONDITION_VEC { NEXT_HIT_INFO.CondsRemoved() };

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
        const auto VEC { GetAllCondsAdded() };
        return (std::find(VEC.begin(), VEC.end(), E) != VEC.end());
    }

    bool CreatureEffect::GetAllCondsRemovedContains(const creature::Conditions::Enum E) const
    {
        const auto VEC { GetAllCondsRemoved() };
        return (std::find(VEC.begin(), VEC.end(), E) != VEC.end());
    }

    bool CreatureEffect::AllCondsAddedRemove(const creature::Conditions::Enum E)
    {
        auto wasCondRemoved { false };

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
        auto wasCondRemoved { false };

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
