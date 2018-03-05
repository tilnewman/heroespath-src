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
#ifndef HEROESPATH_CREATURE_ROLESTATS_HPP_INCLUDED
#define HEROESPATH_CREATURE_ROLESTATS_HPP_INCLUDED
//
// role-stats.hpp
//  Specific creature role stat modifiers.
//
#include "creature/role-enum.hpp"
#include "stats/stat-set.hpp"

#include <map>
#include <string>

namespace heroespath
{
namespace creature
{

    using RoleStatSetMap_t = std::map<role::Enum, stats::StatSet>;

    // Responsible for role related stat modifiers, for loading them from the
    // game data file and for providing access to them.
    struct RoleStatModifier
    {
        static const stats::StatSet Get(const role::Enum);

    private:
        static RoleStatSetMap_t roleStatSetMap_;
    };
}
}

#endif // HEROESPATH_CREATURE_ROLESTATS_HPP_INCLUDED
