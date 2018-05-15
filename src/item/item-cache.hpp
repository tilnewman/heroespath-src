// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
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

        // coins and gems weigh nothing
        Weight_t Weight() const;

        bool Empty() const { return ((0_coin == coins) && (0_gem == gems) && items_pvec.empty()); }

        bool ItemsOnly() const
        {
            return ((0_coin == coins) && (0_gem == gems) && (items_pvec.empty() == false));
        }

        void Reset();

        Coin_t coins;
        Gem_t gems;
        ItemPVec_t items_pvec;
    };

} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_ITEMCACHE_HPP_INCLUDED
