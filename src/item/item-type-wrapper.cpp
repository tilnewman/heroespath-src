// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software.  If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
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
        , unique(unique_type::NotUnique)
        , name(named_type::NotNamed)
        , set(set_type::NotASet)
        , misc(misc_type::NotMisc)
        , roleRestrictionBasedOnMiscAndSetType(creature::role::Count)
        , summon()
    {}

    TypeWrapper::TypeWrapper(const ItemProfile & ITEM_PROFILE)
        : element(ITEM_PROFILE.ElementType())
        , unique(ITEM_PROFILE.UniqueType())
        , name(ITEM_PROFILE.NamedType())
        , set(ITEM_PROFILE.SetType())
        , misc(ITEM_PROFILE.MiscType())
        , roleRestrictionBasedOnMiscAndSetType(ITEM_PROFILE.RoleRestrictionBasedOnMiscAndSetType())
        , summon(ITEM_PROFILE.Summoning())
    {}

} // namespace item
} // namespace heroespath
