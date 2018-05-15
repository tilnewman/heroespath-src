// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
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
