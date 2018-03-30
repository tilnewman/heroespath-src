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
#ifndef HEROESPATH_COMBAT_CONDITIONEFFECTS_HPP_INCLUDED
#define HEROESPATH_COMBAT_CONDITIONEFFECTS_HPP_INCLUDED
//
// condition-effects.hpp
//
#include "combat/hit-info.hpp"
#include "game/phase-enum.hpp"
#include "misc/boost-optional-that-throws.hpp"
#include "misc/handy-types.hpp"

#include <string>
#include <utility>
#include <vector>

namespace heroespath
{
namespace creature
{
    class Creature;
    using CreaturePtr_t = misc::NotNull<Creature *>;
} // namespace creature

namespace combat
{

    struct ConditionEffects
    {
        // returns true if an effect consumed the current turn
        // fightResult_OutParam is guaranteed to to only have one CreatureEffect
        // if hitInfoVec_OutParam is empty return value will be false
        static bool Process(
            const game::Phase::Enum HEROESPATH_PHASE,
            const creature::CreaturePtr_t CREATURE_PTR,
            HitInfoVec_t & hitInfoVec_OuParam);
    };

} // namespace combat
} // namespace heroespath

#endif // HEROESPATH_COMBAT_CONDITIONEFFECTS_HPP_INCLUDED
