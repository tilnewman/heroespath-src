// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// item-creation-packet.cpp
//
#include "item-creation-packet.hpp"

#include "item/item-profile.hpp"

namespace heroespath
{
namespace item
{

    ItemCreationPacket::ItemCreationPacket(
        const category::Enum CATEGORY,
        const material::Enum MATERIAL_PRI,
        const material::Enum MATERIAL_SEC,
        const Coin_t PRICE,
        const Weight_t WEIGHT,
        const bool IS_PIXIE,
        const element_type::Enum ELEMENT_TYPE,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE,
        const misc_type::Enum MISC_TYPE,
        const creature::role::Enum ROLE_RESTRICTION,
        const creature::SummonInfo SUMMON_INFO) noexcept
        : category_enum(CATEGORY)
        , material_pri(MATERIAL_PRI)
        , material_sec(MATERIAL_SEC)
        , price(PRICE)
        , weight(WEIGHT)
        , element_enum(ELEMENT_TYPE)
        , named_enum(NAMED_TYPE)
        , set_enum(SET_TYPE)
        , misc_enum(MISC_TYPE)
        , role_restriction(ROLE_RESTRICTION)
        , summon_info(SUMMON_INFO)
        , is_pixie(IS_PIXIE)
    {}

    ItemCreationPacket::ItemCreationPacket(
        const ItemProfile & PROFILE, const Coin_t PRICE, const Weight_t WEIGHT) noexcept
        : category_enum(PROFILE.Category())
        , material_pri(PROFILE.MaterialPrimary())
        , material_sec(PROFILE.MaterialSecondary())
        , price(PRICE)
        , weight(WEIGHT)
        , element_enum(PROFILE.ElementType())
        , named_enum(PROFILE.NamedType())
        , set_enum(PROFILE.SetType())
        , misc_enum(PROFILE.MiscType())
        , role_restriction(PROFILE.RoleRestriction())
        , summon_info(PROFILE.SummonInfo())
        , is_pixie(PROFILE.IsPixie())
    {}

} // namespace item
} // namespace heroespath
