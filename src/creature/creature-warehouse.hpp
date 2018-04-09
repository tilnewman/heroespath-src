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
#ifndef HEROESPATH_CREATURE_CREATURE_WAREHOUSE_HPP_INCLUDED
#define HEROESPATH_CREATURE_CREATURE_WAREHOUSE_HPP_INCLUDED
//
// creature-warehouse.hpp
//
#include "misc/not-null-warehouse.hpp"
#include "misc/not-null.hpp"

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

    // Singleton responsible for the lifetimes of player::Character objects.
    // This class does not new the objects, but it is the one and only place where they are deleted.
    class CreatureWarehouse
    {
    public:
        CreatureWarehouse(const CreatureWarehouse &) = delete;
        CreatureWarehouse(CreatureWarehouse &&) = delete;
        CreatureWarehouse & operator=(const CreatureWarehouse &) = delete;
        CreatureWarehouse & operator=(CreatureWarehouse &&) = delete;

    public:
        CreatureWarehouse();
        ~CreatureWarehouse();

        static misc::NotNull<CreatureWarehouse *> Instance();
        static void Acquire();
        static void Release();

        static misc::NotNullWarehouse<creature::Creature> & Access()
        {
            return Instance()->Warehouse();
        }

        misc::NotNullWarehouse<creature::Creature> & Warehouse() { return warehouse_; }

    private:
        static std::unique_ptr<CreatureWarehouse> instanceUPtr_;
        misc::NotNullWarehouse<creature::Creature> warehouse_;
    };

} // namespace creature
} // namespace heroespath

#endif // HEROESPATH_CREATURE_CREATURE_WAREHOUSE_HPP_INCLUDED
