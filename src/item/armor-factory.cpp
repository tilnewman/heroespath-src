// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
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
            // TODO move to MaterialFactory
            auto const MATERIALS_PAIR{ material::SkinMaterial(CREATURE_PTR->Race()) };

            M_ASSERT_OR_LOGANDTHROW_SS(
                ((MATERIALS_PAIR.first != material::Count)
                 && (MATERIALS_PAIR.first != material::Nothing)),
                "item::armor::ArmorFactory::Make(body_part="
                    << ((BODY_PART == body_part::Count) ? "Count" : body_part::ToString(BODY_PART))
                    << ", creature={" << CREATURE_PTR->ToString()
                    << "}) but that creature's skin material (material::SkinMaterial()) was "
                       "invalid: pri="
                    << ((MATERIALS_PAIR.first == material::Count)
                            ? "Count"
                            : material::ToString(MATERIALS_PAIR.first))
                    << ", sec="
                    << ((MATERIALS_PAIR.second == material::Count)
                            ? "Count"
                            : material::ToString(MATERIALS_PAIR.second))
                    << ".");

            const ArmorTypeWrapper ARMOR_TYPE_WRAPPER{ BODY_PART };

            auto const ARMOR_RATING{ Armor_t::Make(
                material::ArmorRatingBonusPri(MATERIALS_PAIR.first).As<int>()
                + CREATURE_PTR->Rank().As<int>()) };

            auto const WEIGHT{ Weight_t::Make(
                10.0f * material::WeightMult(MATERIALS_PAIR.first, MATERIALS_PAIR.second)) };

            return ItemWarehouse::Access().Store(std::make_unique<Item>(
                MakeArmorBodyPartName(MATERIALS_PAIR, CREATURE_PTR),
                MakeArmorBodyPartDescription(MATERIALS_PAIR),
                static_cast<category::Enum>(ItemProfile::CategoryArmor() | category::BodyPart),
                MATERIALS_PAIR.first,
                MATERIALS_PAIR.second,
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
