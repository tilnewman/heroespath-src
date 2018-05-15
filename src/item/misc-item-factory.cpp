// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
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
