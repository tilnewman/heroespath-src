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
// armor-factory.cpp
//
#include "armor-factory.hpp"

#include "creature/creature.hpp"
#include "item/armor-details.hpp"
#include "item/item-profile.hpp"
#include "item/item-warehouse.hpp"
#include "item/item.hpp"
#include "item/weapon-type-wrapper.hpp"

namespace heroespath
{
namespace item
{
    namespace armor
    {

        const ItemPtr_t ArmorFactory::Make(const ItemProfile & PROFILE)
        {
            auto const ARMOR_TYPE_WRAPPER{ PROFILE.ArmorInfo() };

            auto const ARMOR_DETAILS{ ArmorDetailLoader::LookupArmorDetails(
                ARMOR_TYPE_WRAPPER.DetailsKeyName()) };

            return ItemWarehouse::Access().Store(std::make_unique<Item>(
                MakeNonBodyPartName(PROFILE),
                MakeNonBodyPartDescription(PROFILE, ARMOR_DETAILS.description),
                PROFILE.Category(),
                PROFILE.WeaponType(),
                PROFILE.MaterialPrimary(),
                PROFILE.MaterialSecondary(),
                CalculatePrice(PROFILE, ARMOR_DETAILS.price),
                CalculateWeight(PROFILE, ARMOR_DETAILS.weight),
                0_health,
                0_health,
                CalculateArmorRating(PROFILE, ARMOR_DETAILS.armor_rating),
                TypeWrapper(PROFILE),
                PROFILE.WeaponInfo(),
                ARMOR_TYPE_WRAPPER,
                PROFILE.IsPixie(),
                PROFILE.RoleRestriction()));
        }

        const ItemPtr_t ArmorFactory::Make(
            const body_part::Enum BODY_PART, const creature::CreaturePtr_t CREATURE_PTR)
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (DoesCreatureRequireSkinArmor(CREATURE_PTR)),
                "item::armor::ArmorFactory::Make(body_part="
                    << ((BODY_PART == body_part::Count) ? "Count" : body_part::ToString(BODY_PART))
                    << ", creature={" << CREATURE_PTR->ToString()
                    << "}) but that creature does not require skin armor.");

            auto const MATERIAL{ material::SkinMaterial(CREATURE_PTR->Race()) };

            M_ASSERT_OR_LOGANDTHROW_SS(
                ((MATERIAL != material::Count) && (MATERIAL != material::Nothing)),
                "item::armor::ArmorFactory::Make(body_part="
                    << ((BODY_PART == body_part::Count) ? "Count" : body_part::ToString(BODY_PART))
                    << ", creature={" << CREATURE_PTR->ToString()
                    << "}) but that creature's skin material was invalid.");

            const ArmorTypeWrapper ARMOR_TYPE_WRAPPER{ BODY_PART };

            auto const ARMOR_RATING{ Armor_t::Make(
                material::ArmorRatingBonusPri(MATERIAL).As<int>()
                + CREATURE_PTR->Rank().As<int>()) };

            auto const WEIGHT{ Weight_t::Make(
                10.0f * material::WeightMult(MATERIAL, material::Nothing)) };

            std::ostringstream nameSS;
            nameSS << CREATURE_PTR->RaceName() << "'s " << material::Name(MATERIAL) << " Skin";

            std::ostringstream descSS;
            descSS << "tough skin made of " << material::Name(MATERIAL);

            return ItemWarehouse::Access().Store(std::make_unique<Item>(
                nameSS.str(),
                descSS.str(),
                static_cast<category::Enum>(ItemProfile::CategoryArmor() | category::BodyPart),
                weapon_type::NotAWeapon,
                MATERIAL,
                material::Nothing,
                0_coin,
                WEIGHT,
                0_health,
                0_health,
                ARMOR_RATING,
                TypeWrapper(),
                weapon::WeaponTypeWrapper(),
                ARMOR_TYPE_WRAPPER,
                CREATURE_PTR->IsPixie()));
        }

    } // namespace armor
} // namespace item
} // namespace heroespath
