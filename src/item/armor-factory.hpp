// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
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
