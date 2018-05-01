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
#ifndef HEROESPATH_ITEM_ARMORFACTORY_HPP_INCLUDED
#define HEROESPATH_ITEM_ARMORFACTORY_HPP_INCLUDED
//
// armor-factory.hpp
//  Code that is responsible for making armor objects.
//
#include "item/armor-type-wrapper.hpp"
#include "item/armor-types.hpp"
#include "item/item-factory-base.hpp"
#include "item/item-type-wrapper.hpp"
#include "misc/not-null.hpp"
#include "misc/types.hpp"

#include <memory>
#include <string>

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

    namespace armor
    {

        // Responsible for creating armor items.
        class ArmorFactory : public ItemFactoryBase
        {
        public:
            ArmorFactory(const ArmorFactory &) = delete;
            ArmorFactory(ArmorFactory &&) = delete;
            ArmorFactory & operator=(const ArmorFactory &) = delete;
            ArmorFactory & operator=(ArmorFactory &&) = delete;
            ArmorFactory() = delete;

            static const ItemPtr_t Make(const ItemProfile &);
            static const ItemPtr_t Make(const body_part::Enum, const creature::CreaturePtr_t);
        };

    } // namespace armor
} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_ARMORFACTORY_HPP_INCLUDED
