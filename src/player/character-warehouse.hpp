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
#ifndef HEROESPATH_PLAYER_CHARACTERWAREHOUSE_HPP_INCLUDED
#define HEROESPATH_PLAYER_CHARACTERWAREHOUSE_HPP_INCLUDED
//
// character-warehouse.hpp (player)
//
#include "misc/not-null.hpp"
#include "misc/warehouse.hpp"

#include <memory>
#include <vector>

namespace heroespath
{
namespace creature
{
    class Creature;
    using CreaturePtr_t = misc::NotNull<Creature *>;
    using CreaturePVec_t = std::vector<CreaturePtr_t>;
    using CreatureUPtr_t = std::unique_ptr<Creature>;
    using CreatureUVec_t = std::vector<CreatureUPtr_t>;
}
namespace player
{

    // Singleton responsible for the lifetimes of player::Character objects.
    class CharacterWarehouse
    {
    public:
        CharacterWarehouse(const CharacterWarehouse &) = delete;
        CharacterWarehouse(CharacterWarehouse &&) = delete;
        CharacterWarehouse & operator=(const CharacterWarehouse &) = delete;
        CharacterWarehouse & operator=(CharacterWarehouse &&) = delete;

    public:
        CharacterWarehouse();
        ~CharacterWarehouse();

        static CharacterWarehouse * Instance();
        static void Acquire();
        static void Release();

        const creature::CreaturePtr_t Store(const creature::CreaturePtr_t);
        void Free(const creature::CreaturePtr_t);
        void Free(creature::CreaturePVec_t &);

    private:
        static std::unique_ptr<CharacterWarehouse> instanceUPtr_;
        misc::Warehouse<creature::Creature> warehouse_;
    };

} // namespace player
} // namespace heroespath

#endif // HEROESPATH_PLAYER_CHARACTERWAREHOUSE_HPP_INCLUDED
