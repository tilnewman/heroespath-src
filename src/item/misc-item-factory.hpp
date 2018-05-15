// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_ITEM_MISCITEMFACTORY_HPP_INCLUDED
#define HEROESPATH_ITEM_MISCITEMFACTORY_HPP_INCLUDED
//
// misc-item-factory.hpp
//  Responsible for making individual item objects.
//
#include "item/item-factory-base.hpp"
#include "item/item-type-enum.hpp"
#include "misc/not-null.hpp"

#include <memory>
#include <string>

namespace heroespath
{
namespace item
{

    class Item;
    using ItemPtr_t = misc::NotNull<Item *>;

    class ItemProfile;

    // A Subsystem class responsible for making misc item objects.
    class MiscItemFactory : public ItemFactoryBase
    {
    public:
        MiscItemFactory(const MiscItemFactory &) = delete;
        MiscItemFactory(MiscItemFactory &&) = delete;
        MiscItemFactory & operator=(const MiscItemFactory &) = delete;
        MiscItemFactory & operator=(MiscItemFactory &&) = delete;
        MiscItemFactory() = delete;

        static const ItemPtr_t Make(const ItemProfile &);
    };

} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_MISCITEMFACTORY_HPP_INCLUDED
