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
#ifndef HEROESPATH_CREATURE_CREATURE_FACTORY_HPP_INCLUDED
#define HEROESPATH_CREATURE_CREATURE_FACTORY_HPP_INCLUDED
//
// creature-factory.hpp
//
#include "creature/dragon-class-enum.hpp"
#include "creature/race-enum.hpp"
#include "creature/role-enum.hpp"
#include "creature/sex-enum.hpp"
#include "creature/wolfen-class-enum.hpp"
#include "misc/not-null.hpp"
#include "misc/types.hpp"
#include "stats/stat-set.hpp"
#include "stats/trait.hpp"

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace creature
{
    class Creature;
    using CreaturePtr_t = misc::NotNull<Creature *>;
    using CreaturePVec_t = std::vector<CreaturePtr_t>;

    // Creates sets of creatures that are enemies, not player characters.
    class CreatureFactory
    {
    public:
        CreatureFactory(const CreatureFactory &) = delete;
        CreatureFactory(CreatureFactory &&) = delete;
        CreatureFactory & operator=(const CreatureFactory &) = delete;
        CreatureFactory & operator=(CreatureFactory &&) = delete;

    public:
        CreatureFactory() = default;

        const CreaturePVec_t Make_FirstEncounter() const;

    private:
        const CreaturePtr_t Make_GoblinGrunt() const;
        const CreaturePtr_t Make_Boar() const;

        const CreaturePtr_t Make(
            const stats::StatSet & STATS,
            const Health_t & HEALTH_MIN,
            const Health_t & HEALTH_MAX,
            const sex::Enum SEX,
            const race::Enum RACE,
            const role::Enum ROLE,
            const Rank_t & RANK = 1_rank,
            const Experience_t & EXPERIENCE = 0_exp,
            const Mana_t & MANA = 0_mana) const;
    };

} // namespace creature
} // namespace heroespath

#endif // HEROESPATH_CREATURE_CREATURE_FACTORY_HPP_INCLUDED
