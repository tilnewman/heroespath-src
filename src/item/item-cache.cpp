// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// item-cache.cpp
//
#include "item-cache.hpp"

#include "item/item.hpp"

namespace heroespath
{
namespace item
{

    ItemCache::ItemCache()
        : coins(0_coin)
        , gems(0_gem)
        , items_pvec()
    {}

    Weight_t ItemCache::Weight() const
    {
        Weight_t sum{ 0_weight };

        for (auto const & NEXT_ITEM_PTR : items_pvec)
        {
            sum += NEXT_ITEM_PTR->Weight();
        }

        return sum;
    }

    void ItemCache::Reset()
    {
        coins = 0_coin;
        gems = 0_gem;
        items_pvec.clear();
    }

} // namespace item
} // namespace heroespath
