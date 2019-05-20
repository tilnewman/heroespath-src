// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// item-factory.cpp
//
#include "item-factory.hpp"

#include "creature/creature.hpp"
#include "creature/summon-info.hpp"
#include "item/armor-details.hpp"
#include "item/armor-type-wrapper.hpp"
#include "item/item-creation-packet.hpp"
#include "item/item-profile-warehouse.hpp"
#include "item/item-profile.hpp"
#include "item/item-warehouse.hpp"
#include "item/item.hpp"
#include "item/weapon-details.hpp"
#include "item/weapon-type-wrapper.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/enum-util.hpp"
#include "misc/filesystem.hpp"
#include "misc/log-macros.hpp"
#include "misc/vector-map.hpp"
#include "stage/i-stage.hpp"

#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace heroespath
{
namespace item
{

    const ItemPtr_t ItemFactory::Make(const ItemProfile & PROFILE) const
    {
        // Note that some misc_type items are also weapons or armor.  In these cases item
        // construction is handled by WeaponFactory or ArmorFactory.
        if (PROFILE.IsArmor())
        {
            return MakeArmor(PROFILE);
        }
        else if (PROFILE.IsWeapon())
        {
            return MakeWeapon(PROFILE);
        }
        else if (PROFILE.IsMisc())
        {
            return MakeMisc(PROFILE);
        }
        else
        {
            std::ostringstream ss;

            ss << "item::ItemFactory::Make(profile=" << PROFILE.ToString()
               << ") failed to create an item based on that profile because it was not misc, "
                  "weapon, "
                  "or armor.";

            throw std::runtime_error(ss.str());
        }
    }

    const ItemPtr_t ItemFactory::Make(
        const body_part::Enum BODY_PART, const creature::CreaturePtr_t CREATURE_PTR) const
    {
        if (BODY_PART == body_part::Skin)
        {
            return MakeArmor(BODY_PART, CREATURE_PTR);
        }
        else
        {
            return MakeWeapon(BODY_PART, CREATURE_PTR);
        }
    }

    const ItemPtr_t ItemFactory::MakeArmor(const ItemProfile & PROFILE) const
    {
        const auto & ARMOR_TYPE_WRAPPER { PROFILE.ArmorInfo() };

        const auto ARMOR_DETAILS { armor::ArmorDetailLoader::LookupArmorDetails(
            ARMOR_TYPE_WRAPPER.DetailsKeyName()) };

        return ItemWarehouse::Access().Store(std::make_unique<Item>(
            nameFactory_.MakeNonBodyPartName(PROFILE),
            nameFactory_.MakeNonBodyPartDescription(PROFILE, ARMOR_DETAILS.description),
            ItemCreationPacket(
                PROFILE,
                CalculatePrice(PROFILE, ARMOR_DETAILS.price),
                CalculateWeight(PROFILE, ARMOR_DETAILS.weight)),
            weapon::WeaponTypeWrapper(),
            0_health,
            0_health,
            ARMOR_TYPE_WRAPPER,
            CalculateArmorRating(PROFILE, ARMOR_DETAILS.armor_rating)));
    }

    const ItemPtr_t ItemFactory::MakeArmor(
        const body_part::Enum BODY_PART, const creature::CreaturePtr_t CREATURE_PTR) const
    {
        // TODO move to MaterialFactory
        const auto MATERIALS_PAIR { material::SkinMaterial(CREATURE_PTR->Race()) };

        M_HP_ASSERT_OR_LOG_AND_THROW(
            ((MATERIALS_PAIR.first != material::Count)
             && (MATERIALS_PAIR.first != material::Nothing)),
            "item::armor::ArmorFactory::Make(body_part="
                << ((BODY_PART == body_part::Count) ? "Count" : NAMEOF_ENUM(BODY_PART))
                << ", creature={" << CREATURE_PTR->ToString()
                << "}) but that creature's skin material (material::SkinMaterial()) was "
                   "invalid: pri="
                << ((MATERIALS_PAIR.first == material::Count) ? "Count"
                                                              : NAMEOF_ENUM(MATERIALS_PAIR.first))
                << ", sec="
                << ((MATERIALS_PAIR.second == material::Count) ? "Count"
                                                               : NAMEOF_ENUM(MATERIALS_PAIR.second))
                << ".");

        const armor::ArmorTypeWrapper ARMOR_TYPE_WRAPPER { BODY_PART };

        const auto ARMOR_RATING { Armor_t::Make(
            (material::ArmorRatingBonusPri(MATERIALS_PAIR.first)).GetAsTrait()
            + (CREATURE_PTR->Rank()).GetAsTrait()) };

        const auto WEIGHT { Weight_t::Make(
            10.0f * material::WeightMult(MATERIALS_PAIR.first, MATERIALS_PAIR.second)) };

        return ItemWarehouse::Access().Store(std::make_unique<Item>(
            nameFactory_.MakeArmorBodyPartName(MATERIALS_PAIR, CREATURE_PTR),
            nameFactory_.MakeArmorBodyPartDescription(MATERIALS_PAIR),
            ItemCreationPacket(
                static_cast<category::Enum>(category::Equipable | category::BodyPart),
                MATERIALS_PAIR.first,
                MATERIALS_PAIR.second,
                0_coin,
                WEIGHT,
                CREATURE_PTR->IsPixie()),
            weapon::WeaponTypeWrapper(),
            0_health,
            0_health,
            ARMOR_TYPE_WRAPPER,
            ARMOR_RATING));
    }

    const ItemPtr_t ItemFactory::MakeWeapon(const ItemProfile & PROFILE) const
    {
        const auto & WEAPON_TYPE_WRAPPER { PROFILE.WeaponInfo() };

        const auto WEAPON_DETAILS { weapon::WeaponDetailLoader::LookupWeaponDetails(
            WEAPON_TYPE_WRAPPER.DetailsKeyName()) };

        return ItemWarehouse::Access().Store(std::make_unique<Item>(
            nameFactory_.MakeNonBodyPartName(PROFILE),
            nameFactory_.MakeNonBodyPartDescription(PROFILE, WEAPON_DETAILS.description),
            ItemCreationPacket(
                PROFILE,
                CalculatePrice(PROFILE, WEAPON_DETAILS.price),
                CalculateWeight(PROFILE, WEAPON_DETAILS.weight)),
            WEAPON_TYPE_WRAPPER,
            WEAPON_DETAILS.damage_min,
            WEAPON_DETAILS.damage_max));
    }

    const ItemPtr_t ItemFactory::MakeWeapon(
        const body_part::Enum BODY_PART, const creature::CreaturePtr_t CREATURE_PTR) const
    {
        const weapon::WeaponTypeWrapper WEAPON_TYPE_WRAPPER(BODY_PART);

        if (BODY_PART == body_part::Breath)
        {
            M_HP_ASSERT_OR_LOG_AND_THROW(
                ((CREATURE_PTR->Role() == creature::role::Sylavin)
                 || (CREATURE_PTR->Role() == creature::role::Firebrand)),
                "WeaponFactory::Make(body_part=Breath, creature=\n{"
                    << CREATURE_PTR->ToString()
                    << "}\nbody part was breath but the creature's role was not Sylavin or "
                       "Firebrand.");
        }

        const auto WEAPON_DETAILS_NAME { (
            (body_part::Breath == BODY_PART)
                ? (WEAPON_TYPE_WRAPPER.DetailsKeyName() + NAMEOF_ENUM_STR(CREATURE_PTR->Role()))
                : WEAPON_TYPE_WRAPPER.DetailsKeyName()) };

        const weapon::WeaponDetails WEAPON_DETAILS {
            weapon::WeaponDetailLoader::LookupWeaponDetails(WEAPON_DETAILS_NAME)
        };

        // TODO move into MaterialsFactory
        const auto MATERIALS { [BODY_PART]() {
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
            nameFactory_.MakeWeaponBodyPartName(CREATURE_PTR, WEAPON_TYPE_WRAPPER.ReadableName()),
            nameFactory_.MakeWeaponBodyPartDescription(WEAPON_DETAILS.description, CREATURE_PTR),
            ItemCreationPacket(
                ItemProfile::CategoryWeaponBodypart(BODY_PART),
                MATERIALS.first,
                MATERIALS.second,
                WEAPON_DETAILS.price,
                WEAPON_DETAILS.weight,
                CREATURE_PTR->IsPixie()),
            WEAPON_TYPE_WRAPPER,
            WEAPON_DETAILS.damage_min,
            WEAPON_DETAILS.damage_max));
    }

    const ItemPtr_t ItemFactory::MakeMisc(const ItemProfile & PROFILE) const
    {
        return ItemWarehouse::Access().Store(std::make_unique<Item>(
            nameFactory_.MakeNonBodyPartName(PROFILE),
            nameFactory_.MakeNonBodyPartDescription(PROFILE, PROFILE.ReadableName()),
            ItemCreationPacket(PROFILE, CalculatePrice(PROFILE), CalculateWeight(PROFILE)),
            PROFILE.WeaponInfo(),
            0_health,
            0_health,
            PROFILE.ArmorInfo()));
    }

    Coin_t ItemFactory::CalculatePrice(
        const ItemProfile & PROFILE, const Coin_t BASE_PRICE_PARAM) const
    {
        const auto BASE_PRICE_FINAL { (
            (BASE_PRICE_PARAM > 0_coin) ? BASE_PRICE_PARAM
                                        : TreasureScoreToCoins(PROFILE.TreasureScore())) };

        Coin_t price { BASE_PRICE_FINAL
                       + material::PriceAdj(
                           PROFILE.MaterialPrimary(), PROFILE.MaterialSecondary()) };

        if (PROFILE.IsPixie())
        {
            price = Coin_t::Make(price.GetAs<float>() * 1.5f);
        }

        return price;
    }

    Weight_t ItemFactory::CalculateWeight(
        const ItemProfile & PROFILE, const Weight_t BASE_WEIGHT_PARAM) const
    {
        auto weight { (
            (BASE_WEIGHT_PARAM > 0_weight) ? BASE_WEIGHT_PARAM
                                           : misc_type::Weight(PROFILE.MiscType())) };

        weight = Weight_t::Make(
            weight.GetAs<float>()
            * material::WeightMult(PROFILE.MaterialPrimary(), PROFILE.MaterialSecondary()));

        if (PROFILE.IsPixie())
        {
            weight /= 250_weight;
        }

        if (weight < 1_weight)
        {
            weight = 1_weight;
        }

        return weight;
    }

    Armor_t ItemFactory::CalculateArmorRating(
        const ItemProfile & PROFILE, const Armor_t & BASE_ARMOR_RATING) const
    {
        // armor items with a clasp will not consider the secondary material in terms of armor
        // rating because that is just the material of the clasp

        return BASE_ARMOR_RATING
            + material::ArmorRatingBonus(
                   PROFILE.MaterialPrimary(),
                   (((PROFILE.NameMaterialType() == name_material_type::Clasped)
                     || (PROFILE.NameMaterialType() == name_material_type::ClaspedWithBase))
                        ? material::Nothing
                        : PROFILE.MaterialSecondary()));
    }

    Coin_t ItemFactory::TreasureScoreToCoins(const Score_t & TREASURE_SCORE) const
    {
        // For now Treasure Score equals the price in coins
        return Coin_t::Make(TREASURE_SCORE.Get());
    }

} // namespace item
} // namespace heroespath
