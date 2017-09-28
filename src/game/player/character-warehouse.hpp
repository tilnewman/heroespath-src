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
#ifndef GAME_PLAYER_CHARACTERWAREHOUSE_HPP_INCLUDED
#define GAME_PLAYER_CHARACTERWAREHOUSE_HPP_INCLUDED
//
// character-warehouse.hpp (player)
//
#include "game/warehouse.hpp"

#include <memory>
#include <vector>


namespace game
{
namespace player
{

    //forward declarations
    class Character;
    using CharacterPtr_t = Character *;
    using CharacterUPtr_t = std::unique_ptr<Character>;
    using CharacterUVec_t = std::vector<CharacterUPtr_t>;


    //Singleton responsible for the lifetimes of player::Character objects.
    class CharacterWarehouse
    {
        CharacterWarehouse(const CharacterWarehouse &) =delete;
        CharacterWarehouse & operator=(const CharacterWarehouse &) =delete;

    public:
        CharacterWarehouse();
        ~CharacterWarehouse();

        static CharacterWarehouse * Instance();
        static void Acquire();
        static void Release();

        CharacterPtr_t Store(const CharacterPtr_t);
        void Free(CharacterPtr_t &);

    private:
        static std::unique_ptr<CharacterWarehouse> instanceUPtr_;
        Warehouse<Character> warehouse_;
    };

}
}

#endif //GAME_PLAYER_CHARACTERWAREHOUSE_HPP_INCLUDED
