// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_ITEM_ITEM_TYPE_WRAPPER_HPP_INCLUDED
#define HEROESPATH_ITEM_ITEM_TYPE_WRAPPER_HPP_INCLUDED
//
// item-creation-packet.hpp
//
#include "creature/race-enum.hpp"
#include "creature/role-enum.hpp"
#include "creature/summon-info.hpp"
#include "item/item-type-enum.hpp"
#include "misc/strong-numeric-type.hpp"

namespace heroespath
{
namespace item
{
    class ItemProfile;

    // wraps stuff always needed to make an item
    struct ItemCreationPacket
    {
        ItemCreationPacket(
            const category::Enum CATEGORY = category::None,
            const material::Enum MATERIAL_PRI = material::Count,
            const material::Enum MATERIAL_SEC = material::Count,
            const Coin_t PRICE = 0_coin,
            const Weight_t WEIGHT = 0_weight,
            const bool IS_PIXIE = false,
            const element_type::Enum ELEMENT_TYPE = element_type::None,
            const named_type::Enum NAMED_TYPE = named_type::Count,
            const set_type::Enum SET_TYPE = set_type::Count,
            const misc_type::Enum MISC_TYPE = misc_type::Count,
            const creature::role::Enum ROLE_RESTRICTION = creature::role::Count,
            const creature::SummonInfo SUMMON_INFO = creature::SummonInfo()) noexcept;

        ItemCreationPacket(
            const ItemProfile & PROFILE, const Coin_t PRICE, const Weight_t WEIGHT) noexcept;

        category::Enum category_enum;
        material::Enum material_pri;
        material::Enum material_sec;
        Coin_t price;
        Weight_t weight;
        element_type::Enum element_enum;
        named_type::Enum named_enum;
        set_type::Enum set_enum;
        misc_type::Enum misc_enum;
        creature::role::Enum role_restriction;
        creature::SummonInfo summon_info;
        bool is_pixie;
    };

} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_ITEM_TYPE_WRAPPER_HPP_INCLUDED
