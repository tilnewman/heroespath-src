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
#ifndef HEROESPATH_ITEM_ITEMFACTORY_HPP_INCLUDED
#define HEROESPATH_ITEM_ITEMFACTORY_HPP_INCLUDED
//
// item-factory.hpp
//
#include "item/item-factory-base.hpp"
#include "item/item-type-enum.hpp"
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"

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
    class ItemProfile;

    class Item;
    using ItemPtr_t = misc::NotNull<Item *>;
    using ItemPtrOpt_t = boost::optional<ItemPtr_t>;

    // Responsible for making new (and properly warehousing) item objects from (fat) ItemProfiles.
    class ItemFactory : public ItemFactoryBase
    {
    public:
        ItemFactory(const ItemFactory &) = delete;
        ItemFactory(ItemFactory &&) = delete;
        ItemFactory & operator=(const ItemFactory &) = delete;
        ItemFactory & operator=(ItemFactory &&) = delete;
        ItemFactory() = delete;

        static bool Test();

        static const ItemPtr_t Make(const ItemProfile &);
        static const ItemPtr_t Make(const body_part::Enum, const creature::CreaturePtr_t);

    private:
        static void TestItem(const ItemPtr_t & ITEM_PTR, const ItemProfile & ITEM_PROFILE);
    };

} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_ITEMFACTORY_HPP_INCLUDED
