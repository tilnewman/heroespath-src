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
// condition-effects.cpp
//
#include "condition-effects.hpp"

#include "game/creature/creature.hpp"
#include "game/creature/condition.hpp"


namespace game
{
namespace combat
{

    bool ConditionEffects::Process(const Phase::Enum,
                                   creature::CreaturePtr_t creaturePtr,
                                   HitInfoVec_t &          hitInfoVec_OuParam)
    {
        auto condsPVec{ creaturePtr->ConditionsPVec() };

        //sort conditions by decreasing severity
        std::sort(condsPVec.begin(),
                  condsPVec.end(),
                  [](const creature::ConditionPtr_t A, const creature::ConditionPtr_t B)
                    {
                        return (A->Severity() > B->Severity());
                    });

        auto hasTurnBeenConsumed{ false };
        for (auto const NEXT_COND_PTR : condsPVec)
        {
            NEXT_COND_PTR->PerTurnEffect(creaturePtr, hitInfoVec_OuParam, hasTurnBeenConsumed);
        }

        return hasTurnBeenConsumed;
    }

}
}
