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
#ifndef HEROESPATH_COMBAT_PARTYFACTORY_HPP_INCLUDED
#define HEROESPATH_COMBAT_PARTYFACTORY_HPP_INCLUDED
//
// party-factory.hpp
//  A singleton class that creates parties of creatures that confront the player characters.
//
#include "creature/dragon-class-enum.hpp"
#include "creature/race-enum.hpp"
#include "creature/role-enum.hpp"
#include "creature/sex-enum.hpp"
#include "creature/wolfen-class-enum.hpp"
#include "misc/types.hpp"
#include "stats/stat-set.hpp"
#include "stats/trait.hpp"

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace non_player
{
    class Party;
    using PartyUPtr_t = std::unique_ptr<Party>;

    class Character;
    using CharacterPtr_t = Character *;
} // namespace non_player
namespace combat
{

    // Creates partys of creatures that confront the player characters
    class PartyFactory
    {
    public:
        PartyFactory(const PartyFactory &) = delete;
        PartyFactory(const PartyFactory &&) = delete;
        PartyFactory & operator=(const PartyFactory &) = delete;
        PartyFactory & operator=(const PartyFactory &&) = delete;

    public:
        PartyFactory();
        ~PartyFactory();

        static PartyFactory * Instance();
        static void Acquire();
        static void Release();

        non_player::PartyUPtr_t MakeParty_FirstEncounter() const;

    private:
        non_player::CharacterPtr_t MakeCharacter_GoblinGrunt() const;
        non_player::CharacterPtr_t MakeCharacter_Boar() const;

        non_player::CharacterPtr_t MakeCharacter(
            const stats::StatSet & STATS,
            const Health_t & HEALTH_MIN,
            const Health_t & HEALTH_MAX,
            const creature::sex::Enum SEX,
            const creature::race::Enum RACE,
            const creature::role::Enum ROLE,
            const Rank_t & RANK = 1_rank,
            const Experience_t & EXPERIENCE = 0_exp,
            const Mana_t & MANA = 0_mana) const;

    private:
        static std::unique_ptr<PartyFactory> instanceUPtr_;
    };
} // namespace combat
} // namespace heroespath

#endif // HEROESPATH_COMBAT_PARTYFACTORY_HPP_INCLUDED
