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
#ifndef HEROESPATH_ITEM_WEAPONFACTORY_HPP_INCLUDED
#define HEROESPATH_ITEM_WEAPONFACTORY_HPP_INCLUDED
//
// weapons-factory.hpp
//
#include "item/item-factory-base.hpp"
#include "item/item-type-wrapper.hpp"
#include "item/weapon-details.hpp"
#include "item/weapon-types.hpp"
#include "misc/not-null.hpp"

#include <memory>
#include <string>
#include <tuple>

namespace heroespath
{
namespace creature
{
    class Creature;
    using CreaturePtr_t = misc::NotNull<Creature *>;
} // namespace creature
namespace item
{
    class Item;
    using ItemPtr_t = misc::NotNull<Item *>;

    class ItemProfile;

    namespace weapon
    {

        // Responsible for creating weapon items.
        class WeaponFactory : public ItemFactoryBase
        {
        public:
            WeaponFactory(const WeaponFactory &) = delete;
            WeaponFactory(WeaponFactory &&) = delete;
            WeaponFactory & operator=(const WeaponFactory &) = delete;
            WeaponFactory & operator=(WeaponFactory &&) = delete;
            WeaponFactory() = delete;

            static const ItemPtr_t Make(const ItemProfile &);
            static const ItemPtr_t Make(const body_part::Enum, const creature::CreaturePtr_t);
        };

    } // namespace weapon
} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_WEAPONFACTORY_HPP_INCLUDED
