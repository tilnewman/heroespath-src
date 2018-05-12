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
