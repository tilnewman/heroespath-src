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
// weapon-facotry.cpp
//
#include "weapon-factory.hpp"

#include "creature/creature.hpp"
#include "item/armor-type-wrapper.hpp"
#include "item/item-profile.hpp"
#include "item/item-type-enum.hpp"
#include "item/item-warehouse.hpp"
#include "item/item.hpp"
#include "stats/trait.hpp"

#include <exception>
#include <sstream>

namespace heroespath
{
namespace item
{
    namespace weapon
    {

        const ItemPtr_t WeaponFactory::Make(const ItemProfile & PROFILE)
        {
            auto const WEAPON_TYPE_WRAPPER{ PROFILE.WeaponInfo() };

            WeaponDetails weaponDetails{ WeaponDetailLoader::LookupWeaponDetails(
                WEAPON_TYPE_WRAPPER.DetailsKeyName()) };

            if (PROFILE.WeaponInfo().IsKnife() || PROFILE.WeaponInfo().IsDagger())
            {
                if (PROFILE.Size() == sfml_util::Size::Small)
                {
                    weaponDetails.damage_min -= 1_health;
                    weaponDetails.damage_max -= 1_health;
                }
                else if (PROFILE.Size() == sfml_util::Size::Large)
                {
                    weaponDetails.damage_min += 1_health;
                    weaponDetails.damage_max += 1_health;
                }
            }

            return ItemWarehouse::Access().Store(std::make_unique<Item>(
                MakeNonBodyPartName(PROFILE),
                MakeNonBodyPartDescription(PROFILE, weaponDetails.description),
                PROFILE.Category(),
                PROFILE.MaterialPrimary(),
                PROFILE.MaterialSecondary(),
                CalculatePrice(PROFILE, weaponDetails.price),
                CalculateWeight(PROFILE, weaponDetails.weight),
                weaponDetails.damage_min,
                weaponDetails.damage_max,
                0_armor,
                TypeWrapper(PROFILE),
                WEAPON_TYPE_WRAPPER,
                PROFILE.ArmorInfo(),
                PROFILE.IsPixie(),
                PROFILE.RoleRestriction()));
        }

        const ItemPtr_t WeaponFactory::Make(
            const body_part::Enum BODY_PART, const creature::CreaturePtr_t CREATURE_PTR)
        {
            const WeaponTypeWrapper WEAPON_TYPE_WRAPPER(BODY_PART);

            if (BODY_PART == body_part::Breath)
            {
                M_ASSERT_OR_LOGANDTHROW_SS(
                    ((CREATURE_PTR->Role() == creature::role::Sylavin)
                     || (CREATURE_PTR->Role() == creature::role::Firebrand)),
                    "WeaponFactory::Make(body_part=Breath, creature=\n{"
                        << CREATURE_PTR->ToString()
                        << "}\nbody part was breath but the creature's role was not Sylavin or "
                           "Firebrand.");
            }

            auto const WEAPON_DETAILS_NAME{ (
                (body_part::Breath == BODY_PART)
                    ? (WEAPON_TYPE_WRAPPER.DetailsKeyName()
                       + creature::role::ToString(CREATURE_PTR->Role()))
                    : WEAPON_TYPE_WRAPPER.DetailsKeyName()) };

            const WeaponDetails WEAPON_DETAILS{ WeaponDetailLoader::LookupWeaponDetails(
                WEAPON_DETAILS_NAME) };

            // TODO move into MaterialsFactory
            auto const MATERIALS{ [BODY_PART]() {
                if (BODY_PART == body_part::Bite)
                {
                    return std::make_pair(material::Tooth, material::Nothing);
                }
                else if (BODY_PART == body_part::Breath)
                {
                    return std::make_pair(material::Gas, material::Nothing);
                }
                else if (BODY_PART == body_part::Claws)
                {
                    return std::make_pair(material::Claw, material::Nothing);
                }
                else if (BODY_PART == body_part::Fists)
                {
                    return std::make_pair(material::Bone, material::Hide);
                }
                else // tentacles
                {
                    return std::make_pair(material::Plant, material::Nothing);
                }
            }() };

            return ItemWarehouse::Access().Store(std::make_unique<Item>(
                MakeWeaponBodyPartName(CREATURE_PTR, WEAPON_TYPE_WRAPPER.ReadableName()),
                MakeWeaponBodyPartDescription(WEAPON_DETAILS.description, CREATURE_PTR),
                ItemProfile::CategoryWeaponBodypart(BODY_PART),
                MATERIALS.first,
                MATERIALS.second,
                WEAPON_DETAILS.price,
                WEAPON_DETAILS.weight,
                WEAPON_DETAILS.damage_min,
                WEAPON_DETAILS.damage_max,
                0_armor,
                TypeWrapper(),
                WEAPON_TYPE_WRAPPER,
                armor::ArmorTypeWrapper(),
                CREATURE_PTR->IsPixie()));
        }

    } // namespace weapon
} // namespace item
} // namespace heroespath
