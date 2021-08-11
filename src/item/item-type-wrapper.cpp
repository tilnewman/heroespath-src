// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// item-type-wrapper.cpp
//
#include "item-type-wrapper.hpp"

#include "item/item-profile.hpp"

namespace heroespath
{
namespace item
{

    TypeWrapper::TypeWrapper()
        : element(element_type::None)
        , name(named_type::Not)
        , set(set_type::Not)
        , misc(misc_type::Not)
        , roleRestriction(creature::role::Count)
        , summon()
    {}

    TypeWrapper::TypeWrapper(const ItemProfile & ITEM_PROFILE)
        : element(ITEM_PROFILE.ElementType())
        , name(ITEM_PROFILE.NamedType())
        , set(ITEM_PROFILE.SetType())
        , misc(ITEM_PROFILE.MiscType())
        , roleRestriction(ITEM_PROFILE.RoleRestriction())
        , summon(ITEM_PROFILE.SummonInfo())
    {}

} // namespace item
} // namespace heroespath
