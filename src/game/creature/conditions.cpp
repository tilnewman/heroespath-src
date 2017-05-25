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
// conditions.cpp
//
#include "conditions.hpp"

#include "game/loop-manager.hpp"
#include "game/creature/condition-warehouse.hpp"

#include "utilz/assertlogandthrow.hpp"
#include "utilz/random.hpp"

#include <sstream>
#include <exception>


namespace game
{
namespace creature
{
namespace condition
{

    const ConditionEnumVec_t Poisoned::PerTurnChange(CreaturePtrC_t creaturePtrC)
    {
        const stats::Health_t DAMAGE_BASE{ utilz::random::Int(2, 6) };
        auto const DAMAGE_FROM_HEALTH_NORMAL{ static_cast<stats::Health_t>(static_cast<float>(creaturePtrC->HealthNormal()) * 0.1f) };
        creaturePtrC->HealthCurrentAdj(-1 * (DAMAGE_BASE + DAMAGE_FROM_HEALTH_NORMAL));

        if (creaturePtrC->HealthCurrent() <= 0)
        {
            creaturePtrC->ConditionAdd(Conditions::Dead);
            return ConditionEnumVec_t(1, Conditions::Dead );
        }
        else
        {
            return ConditionEnumVec_t();
        }
    }

}
}
}

