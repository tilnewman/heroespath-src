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
#ifndef HEROESPATH_ITEM_ITEMWAREHOUSE_HPP_INCLUDED
#define HEROESPATH_ITEM_ITEMWAREHOUSE_HPP_INCLUDED
//
// item-warehouse.hpp
//
#include "misc/not-null-warehouse.hpp"
#include "misc/not-null.hpp"

#include <memory>
#include <vector>

namespace heroespath
{
namespace item
{

    class Item;
    using ItemPtr_t = misc::NotNull<Item *>;
    using ItemPVec_t = std::vector<ItemPtr_t>;
    using ItemUPtr_t = std::unique_ptr<Item>;
    using ItemUVec_t = std::vector<ItemUPtr_t>;

    // Singleton responsible for the lifetimes of item objects.
    // This class does not new the objects, but it does delete them.
    class ItemWarehouse
    {
    public:
        ItemWarehouse(const ItemWarehouse &) = delete;
        ItemWarehouse(ItemWarehouse &&) = delete;
        ItemWarehouse & operator=(const ItemWarehouse &) = delete;
        ItemWarehouse & operator=(ItemWarehouse &&) = delete;

    public:
        ItemWarehouse();
        ~ItemWarehouse();

        static misc::NotNull<ItemWarehouse *> Instance();
        static void Acquire();
        static void Release();

        static misc::NotNullWarehouse<Item> & Access() { return Instance()->Warehouse(); }
        misc::NotNullWarehouse<Item> & Warehouse() { return warehouse_; }

    private:
        static std::unique_ptr<ItemWarehouse> instanceUPtr_;
        misc::NotNullWarehouse<Item> warehouse_;
    };

} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_ITEMWAREHOUSE_HPP_INCLUDED
