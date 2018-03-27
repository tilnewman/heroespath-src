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
#ifndef HEROESPATH_ITEM_ITEMCACHE_HPP_INCLUDED
#define HEROESPATH_ITEM_ITEMCACHE_HPP_INCLUDED
//
// item-cache.hpp
//
#include "misc/not-null.hpp"
#include "misc/types.hpp"

#include <vector>

namespace heroespath
{
namespace item
{

    class Item;
    using ItemPtr_t = misc::NotNull<Item *>;
    using ItemPVec_t = std::vector<ItemPtr_t>;

    // Handy container of items used by Encounter to store treasure
    // both on enemy creatures and cached in the chest/lockbox.
    struct ItemCache
    {
        ItemCache();

        Coin_t coins{ 0_coin };
        Gem_t gems{ 0_gem };
        ItemPVec_t items_pvec;

        // coins and gems weigh nothing
        Weight_t Weight() const;

        bool Empty() const { return ((0_coin == coins) && (0_gem == gems) && items_pvec.empty()); }

        bool ItemsOnly() const
        {
            return ((0_coin == coins) && (0_gem == gems) && (items_pvec.empty() == false));
        }
    };

} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_ITEMCACHE_HPP_INCLUDED
