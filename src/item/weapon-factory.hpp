// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
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
