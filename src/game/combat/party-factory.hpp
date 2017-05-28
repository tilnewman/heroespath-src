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
#ifndef GAME_COMBAT_PARTYFACTORY_INCLUDED
#define GAME_COMBAT_PARTYFACTORY_INCLUDED
//
// party-factory.hpp
//  A singleton class that creates parties of creatures that confront the player characters.
//
#include "game/item/types.hpp"

#include <memory>
#include <vector>
#include <string>


namespace game
{
namespace non_player
{
    class Party;
    using PartySPtr_t = std::shared_ptr<Party>;

    class Character;
    using CharacterSPtr_t = std::shared_ptr<Character>;
}
namespace combat
{

    //Creates partys of creatures that confront the player characters
    class PartyFactory
    {
        //prevent copy construction
        PartyFactory(const PartyFactory &) =delete;

        //prevent copy assignment
        PartyFactory & operator=(const PartyFactory &) =delete;

        //prevent non-singletone instantiation
        PartyFactory();

    public:
        ~PartyFactory();

        static PartyFactory * Instance();
        static void Acquire();
        static void Release();

        non_player::PartySPtr_t MakeParty_FirstEncounter() const;

    private:
        non_player::CharacterSPtr_t MakeCreature_GoblinGrunt() const;

    private:
        static std::unique_ptr<PartyFactory> instanceUPtr_;
    };

}
}
#endif //GAME_COMBAT_PARTYFACTORY_INCLUDED
