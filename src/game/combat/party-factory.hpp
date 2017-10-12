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
#ifndef GAME_COMBAT_PARTYFACTORY_HPP_INCLUDED
#define GAME_COMBAT_PARTYFACTORY_HPP_INCLUDED
//
// party-factory.hpp
//  A singleton class that creates parties of creatures that confront the player characters.
//
#include "game/types.hpp"
#include "game/stats/types.hpp"
#include "game/stats/stat-set.hpp"
#include "game/creature/sex-enum.hpp"
#include "game/creature/race-enum.hpp"
#include "game/creature/role-enum.hpp"
#include "game/creature/wolfen-class-enum.hpp"
#include "game/creature/dragon-class-enum.hpp"

#include <memory>
#include <vector>
#include <string>


namespace game
{
namespace non_player
{
    class Party;
    using PartyPtr_t = Party *;

    class Character;
    using CharacterPtr_t = Character *;
}
namespace combat
{

    //Creates partys of creatures that confront the player characters
    class PartyFactory
    {
        PartyFactory(const PartyFactory &) =delete;
        PartyFactory & operator=(const PartyFactory &) =delete;

    public:
        PartyFactory();
        ~PartyFactory();

        static PartyFactory * Instance();
        static void Acquire();
        static void Release();

        non_player::PartyPtr_t MakeParty_FirstEncounter() const;

    private:
        non_player::CharacterPtr_t MakeCharacter_GoblinGrunt() const;
        non_player::CharacterPtr_t MakeCharacter_Boar() const;

        non_player::CharacterPtr_t MakeCharacter(
            const stats::StatSet & STATS,
            const stats::Trait_t HEALTH_MIN,
            const stats::Trait_t HEALTH_MAX,
            const creature::sex::Enum SEX,
            const creature::race::Enum RACE,
            const creature::role::Enum ROLE,
            const stats::Trait_t RANK = 1,
            const Experience_t EXPERIENCE = 0_exp,
            const stats::Trait_t MANA = 0) const;

    private:
        static std::unique_ptr<PartyFactory> instanceUPtr_;
    };

}
}
#endif //GAME_COMBAT_PARTYFACTORY_HPP_INCLUDED
