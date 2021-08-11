// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_ITEM_ITEM_TYPE_WRAPPER_HPP_INCLUDED
#define HEROESPATH_ITEM_ITEM_TYPE_WRAPPER_HPP_INCLUDED
//
// item-type-wrapper.hpp
//
#include "creature/race-enum.hpp"
#include "creature/role-enum.hpp"
#include "creature/summon-info.hpp"
#include "item/item-type-enum.hpp"

namespace heroespath
{
namespace item
{
    class ItemProfile;

    // Responsible for wrapping all the "types" information about an item.
    struct TypeWrapper
    {
        TypeWrapper();

        explicit TypeWrapper(const ItemProfile &);

        element_type::Enum element;
        named_type::Enum name;
        set_type::Enum set;
        misc_type::Enum misc;
        creature::role::Enum roleRestriction;
        creature::SummonInfo summon;
    };

} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_ITEM_TYPE_WRAPPER_HPP_INCLUDED
