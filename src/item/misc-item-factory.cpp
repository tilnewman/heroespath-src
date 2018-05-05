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
// misc-item-factory.cpp
//
#include "misc-item-factory.hpp"

#include "item/armor-factory.hpp"
#include "item/item-profile.hpp"
#include "item/item-type-wrapper.hpp"
#include "item/item-warehouse.hpp"
#include "item/item.hpp"
#include "item/weapon-factory.hpp"
#include "item/weapon-type-wrapper.hpp"
#include "log/log-macros.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/boost-string-includes.hpp"
#include "misc/random.hpp"

#include <exception>
#include <sstream>

namespace heroespath
{
namespace item
{

    const ItemPtr_t MiscItemFactory::Make(const ItemProfile & PROFILE)
    {
        return ItemWarehouse::Access().Store(std::make_unique<Item>(
            // "" is okay in both places because there is special misc_type logic inside
            MakeNonBodyPartName(PROFILE),
            MakeNonBodyPartDescription(PROFILE, PROFILE.ReadableName()),
            PROFILE.Category(),
            PROFILE.MaterialPrimary(),
            PROFILE.MaterialSecondary(),
            // zero is okay in both places because there is special misc_type logic inside
            CalculatePrice(PROFILE),
            CalculateWeight(PROFILE),
            0_health,
            0_health,
            0_armor,
            TypeWrapper(PROFILE),
            PROFILE.WeaponInfo(),
            PROFILE.ArmorInfo(),
            PROFILE.IsPixie(),
            PROFILE.RoleRestriction()));
    }

} // namespace item
} // namespace heroespath
