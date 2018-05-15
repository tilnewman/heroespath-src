// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
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

    // Subsystem responsible for the lifetimes of item objects.
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
