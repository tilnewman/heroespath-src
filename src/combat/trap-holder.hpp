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
#ifndef HEROESPATH_COMBAT_TRAP_TRAP_HOLDER_HPP_INCLUDED
#define HEROESPATH_COMBAT_TRAP_TRAP_HOLDER_HPP_INCLUDED
//
// trap-holder.hpp
//
#include "combat/trap.hpp"

#include <memory>
#include <vector>

namespace heroespath
{
namespace combat
{
    namespace trap
    {

        // Responsible for the lifetimes of all Traps in the game.
        struct Holder
        {
            static void Fill();
            static void Empty();

            static int GetMinSeverity();
            static int GetMaxSeverity();

            static const TrapVec_t
                GetWithSeverityRatioBetween(const float THE_MIN, const float THE_MAX);

            static const Trap SelectRandomWithSeverityRatioNear(const float);

        private:
            static TrapVec_t traps_;
        };

    } // namespace trap
} // namespace combat
} // namespace heroespath

#endif // HEROESPATH_COMBAT_TRAP_TRAP_HOLDER_HPP_INCLUDED
