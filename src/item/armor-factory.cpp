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
#include "item/weapon-info.hpp"
#include "sfml-util/gui/item-image-manager.hpp"

namespace heroespath
{
namespace item
{
    namespace armor
    {

        const ItemPtr_t ArmorFactory::Make(const ItemProfile & PROFILE)
        {
            if (PROFILE.IsBoots())
            {
                return Make_Boots(
                    PROFILE.Category(),
                    TypeWrapper(PROFILE),
                    PROFILE.BaseType(),
                    PROFILE.MaterialPrimary(),
                    PROFILE.MaterialSecondary(),
                    PROFILE.IsPixie());
            }
            else if (PROFILE.IsBracer())
            {
                return Make_Bracer(
                    PROFILE.Category(),
                    TypeWrapper(PROFILE),
                    PROFILE.BaseType(),
                    PROFILE.MaterialPrimary(),
                    PROFILE.MaterialSecondary(),
                    PROFILE.IsPixie());
            }
            else if (PROFILE.IsPants())
            {
                return Make_Pants(
                    PROFILE.Category(),
                    TypeWrapper(PROFILE),
                    PROFILE.BaseType(),
                    PROFILE.MaterialPrimary(),
                    PROFILE.MaterialSecondary(),
                    PROFILE.IsPixie());
            }
            else if (PROFILE.IsShirt())
            {
                return Make_Shirt(
                    PROFILE.Category(),
                    TypeWrapper(PROFILE),
                    PROFILE.BaseType(),
                    PROFILE.MaterialPrimary(),
                    PROFILE.MaterialSecondary(),
                    PROFILE.IsPixie());
            }
            else if (PROFILE.IsGauntlets())
            {
                return Make_Gauntlets(
                    PROFILE.Category(),
                    TypeWrapper(PROFILE),
                    PROFILE.BaseType(),
                    PROFILE.MaterialPrimary(),
                    PROFILE.MaterialSecondary(),
                    PROFILE.IsPixie());
            }
            else if (PROFILE.IsAventail())
            {
                return Make_Aventail(
                    PROFILE.Category(),
                    TypeWrapper(PROFILE),
                    PROFILE.BaseType(),
                    PROFILE.MaterialPrimary(),
                    PROFILE.MaterialSecondary());
            }
            else if (PROFILE.ShieldType() != armor::shield_type::Count)
            {
                return Make_Shield(
                    PROFILE.Category(),
                    TypeWrapper(PROFILE),
                    PROFILE.ShieldType(),
                    PROFILE.MaterialPrimary(),
                    PROFILE.MaterialSecondary());
            }
            else if (PROFILE.CoverType() != armor::cover_type::Count)
            {
                if ((PROFILE.MiscType() != misc_type::Count)
                    && (PROFILE.MiscType() != misc_type::NotMisc)
                    && (PROFILE.CoverType() == armor::cover_type::Cape))
                {
                    return Make_Cape_AsMiscItem(PROFILE);
                }
                else
                {
                    return Make_Cover(
                        PROFILE.Category(),
                        TypeWrapper(PROFILE),
                        PROFILE.CoverType(),
                        PROFILE.MaterialPrimary(),
                        PROFILE.MaterialSecondary(),
                        PROFILE.IsPixie());
                }
            }
            else if (PROFILE.HelmType() != armor::helm_type::Count)
            {
                return Make_Helm(
                    PROFILE.Category(),
                    TypeWrapper(PROFILE),
                    PROFILE.HelmType(),
                    PROFILE.MaterialPrimary(),
                    PROFILE.MaterialSecondary());
            }

            std::ostringstream ss;

            ss << "item::armor::ArmorFactory::Make(profile=" << PROFILE.ToString()
               << ") failed to create an item based on that profile.";

            throw std::runtime_error(ss.str());
        }

        const ItemPtr_t ArmorFactory::Make(
            const body_part::Enum BODY_PART, const creature::CreaturePtr_t CREATURE_PTR)
        {
            if (BODY_PART == body_part::Skin)
            {
                return Make_Skin(
                    material::SkinMaterial(CREATURE_PTR->Race()),
                    CREATURE_PTR->Rank(),
                    CREATURE_PTR->IsPixie());
            }

            std::ostringstream ss;

            ss << "item::armor::ArmorFactory::MakeBodypart(body_part="
               << body_part::ToString(BODY_PART) << ", creature={" << CREATURE_PTR->ToString()
               << "}) failed to create an item based on that profile.";

            throw std::runtime_error(ss.str());
        }

        const ItemPtr_t ArmorFactory::Make_Cape_AsMiscItem(const ItemProfile & PROFILE)
        {
            Coin_t price{ TreasureScoreToCoins(PROFILE.TreasureScore()) };

            AdjustPrice(
                price, PROFILE.MaterialPrimary(), PROFILE.MaterialSecondary(), PROFILE.IsPixie());

            Weight_t weight{ 111_weight };
            AdjustWeight(weight, PROFILE.MaterialPrimary(), PROFILE.MaterialSecondary());

            auto const BASE_NAME{ [PROFILE]() {
                if ((PROFILE.UniqueType() == unique_type::Count)
                    || (PROFILE.UniqueType() == unique_type::NotUnique))
                {
                    return misc_type::Name(PROFILE.MiscType());
                }
                else
                {
                    return unique_type::Name(PROFILE.UniqueType());
                }
            }() };

            auto const DESC_PREFIX{ "A " + boost::to_lower_copy(BASE_NAME) + " " };

            armor::ArmorInfo armorInfo(armor_type::Covering);
            armorInfo.cover = armor::cover_type::Cape;

            auto const DETAILS{ ArmorDetailLoader::LookupArmorDetails(
                armor::cover_type::ToString(armor::cover_type::Cape)) };

            return ItemWarehouse::Access().Store(std::make_unique<Item>(
                Make_Name(
                    BASE_NAME,
                    PROFILE.MaterialPrimary(),
                    PROFILE.MaterialSecondary(),
                    PROFILE.IsPixie()),
                Make_Desc(
                    DESC_PREFIX,
                    PROFILE.MaterialPrimary(),
                    PROFILE.MaterialSecondary(),
                    "",
                    PROFILE.IsPixie()),
                PROFILE.Category(),
                weapon_type::NotAWeapon,
                PROFILE.ArmorType(),
                PROFILE.MaterialPrimary(),
                PROFILE.MaterialSecondary(),
                price,
                weight,
                0_health,
                0_health,
                DETAILS.armor_rating,
                TypeWrapper(PROFILE),
                weapon::WeaponInfo(),
                armorInfo,
                PROFILE.IsPixie()));
        }

        const ItemPtr_t ArmorFactory::Make_Shield(
            const category::Enum CATEGORY,
            const TypeWrapper & TYPE_WRAPPER,
            const shield_type::Enum SHIELD_TYPE,
            const material::Enum MATERIAL_PRI,
            const material::Enum MATERIAL_SEC)
        {
            ArmorInfo armorInfo{ armor_type::Sheild };
            armorInfo.shield = SHIELD_TYPE;

            auto exclusiveRole{ creature::role::Count };

            if (SHIELD_TYPE == shield_type::Pavis)
            {
                exclusiveRole = creature::role::Knight;
            }

            auto const DETAILS{ ArmorDetailLoader::LookupArmorDetails(
                item::armor::shield_type::ToString(SHIELD_TYPE)) };

            Coin_t price{ DETAILS.price };
            Weight_t weight{ DETAILS.weight };
            Armor_t armorRating{ DETAILS.armor_rating };

            AdjustPrice(price, MATERIAL_PRI, MATERIAL_SEC, false);
            AdjustWeight(weight, MATERIAL_PRI, MATERIAL_SEC);
            AdjustArmorRating(armorRating, MATERIAL_PRI, MATERIAL_SEC);

            std::ostringstream ssName;
            ssName << material::ToReadableString(MATERIAL_PRI) << " ";
            if ((MATERIAL_SEC != MATERIAL_PRI) && (MATERIAL_SEC != material::Nothing))
            {
                if (material::IsRigid(MATERIAL_SEC))
                {
                    if (material::IsJewel(MATERIAL_SEC))
                    {
                        ssName << FactoryBase::RandomJeweledAdjective() << " ";
                    }
                    else
                    {
                        ssName << material::ToReadableString(MATERIAL_SEC) << " reinforced ";
                    }
                }
                else if (material::IsLiquid(MATERIAL_SEC))
                {
                    ssName << " and " << material::ToReadableString(MATERIAL_SEC) << " "
                           << FactoryBase::RandomCoatedAdjective() << " ";
                }
                else
                {
                    ssName << " and " << material::ToReadableString(MATERIAL_SEC) << " ";
                }
            }
            ssName << DETAILS.name;

            std::ostringstream ssDesc;
            ssDesc << DETAILS.description << " made of "
                   << material::ToReadableString(MATERIAL_PRI);
            if ((MATERIAL_SEC != MATERIAL_PRI) && (MATERIAL_SEC != material::Nothing))
            {
                if (material::IsRigid(MATERIAL_SEC))
                {
                    if (material::IsJewel(MATERIAL_SEC))
                    {
                        ssDesc << " and " << FactoryBase::RandomJeweledAdjective() << " with ";
                    }
                    else
                    {
                        ssDesc << " and reinforced with ";
                    }
                }
                else if (material::IsLiquid(MATERIAL_SEC))
                {
                    ssDesc << " and is " << FactoryBase::RandomCoatedAdjective() << " in ";
                }
                else
                {
                    ssDesc << " and ";
                }

                ssDesc << material::ToReadableString(MATERIAL_SEC);
            }
            ssDesc << ".";

            return ItemWarehouse::Access().Store(std::make_unique<Item>(
                ssName.str(),
                ssDesc.str(),
                CATEGORY,
                weapon_type::NotAWeapon,
                armor_type::Sheild,
                MATERIAL_PRI,
                MATERIAL_SEC,
                price,
                weight,
                0_health,
                0_health,
                armorRating,
                TYPE_WRAPPER,
                weapon::WeaponInfo(),
                armorInfo,
                false,
                exclusiveRole));
        }

        const ItemPtr_t ArmorFactory::Make_Helm(
            const category::Enum CATEGORY,
            const TypeWrapper & TYPE_WRAPPER,
            const helm_type::Enum HELM_TYPE,
            const material::Enum MATERIAL_PRI,
            const material::Enum MATERIAL_SEC)
        {
            ArmorInfo armorInfo{ armor_type::Helm };
            armorInfo.helm = HELM_TYPE;

            auto const DETAILS{ ArmorDetailLoader::LookupArmorDetails(
                item::armor::helm_type::ToString(HELM_TYPE)) };

            Coin_t price{ DETAILS.price };
            Weight_t weight{ DETAILS.weight };
            Armor_t armorRating{ DETAILS.armor_rating };

            AdjustPrice(price, MATERIAL_PRI, MATERIAL_SEC);
            AdjustWeight(weight, MATERIAL_PRI, MATERIAL_SEC);
            AdjustArmorRating(armorRating, MATERIAL_PRI, MATERIAL_SEC);

            auto exclusiveRole{ creature::role::Count };
            if (HELM_TYPE == helm_type::Great)
            {
                exclusiveRole = creature::role::Knight;
            }

            return ItemWarehouse::Access().Store(std::make_unique<Item>(
                Make_Name(DETAILS.name, MATERIAL_PRI, MATERIAL_SEC),
                Make_Desc(DETAILS.description, MATERIAL_PRI, MATERIAL_SEC),
                CATEGORY,
                weapon_type::NotAWeapon,
                armor_type::Helm,
                MATERIAL_PRI,
                MATERIAL_SEC,
                price,
                weight,
                0_health,
                0_health,
                armorRating,
                TYPE_WRAPPER,
                weapon::WeaponInfo(),
                armorInfo,
                false,
                exclusiveRole));
        }

        const ItemPtr_t ArmorFactory::Make_Gauntlets(
            const category::Enum CATEGORY,
            const TypeWrapper & TYPE_WRAPPER,
            const base_type::Enum TYPE,
            const material::Enum MATERIAL_PRI,
            const material::Enum MATERIAL_SEC,
            const bool IS_PIXIE_ITEM)
        {
            ArmorInfo armorInfo{ armor_type::Gauntlets };
            armorInfo.is_gauntlets = true;
            armorInfo.base = TYPE;

            auto const DETAILS{ ArmorDetailLoader::LookupArmorDetails([TYPE]() {
                if (TYPE != item::armor::base_type::Plain)
                {
                    return item::armor::base_type::ToString(TYPE) + "Gauntlets";
                }
                else
                {
                    return std::string("Gloves");
                }
            }()) };

            Coin_t price{ DETAILS.price };
            Weight_t weight{ DETAILS.weight };
            Armor_t armorRating{ DETAILS.armor_rating };

            AdjustPrice(price, MATERIAL_PRI, MATERIAL_SEC, IS_PIXIE_ITEM);
            AdjustWeight(weight, MATERIAL_PRI, MATERIAL_SEC);
            AdjustArmorRating(armorRating, MATERIAL_PRI, MATERIAL_SEC);

            creature::role::Enum exclusiveRole(creature::role::Count);
            if (TYPE == base_type::Plate)
            {
                exclusiveRole = creature::role::Knight;
            }

            return ItemWarehouse::Access().Store(std::make_unique<Item>(
                Make_Name(DETAILS.name, MATERIAL_PRI, MATERIAL_SEC, IS_PIXIE_ITEM),
                Make_Desc(DETAILS.description, MATERIAL_PRI, MATERIAL_SEC, "", IS_PIXIE_ITEM),
                CATEGORY,
                weapon_type::NotAWeapon,
                armor_type::Gauntlets,
                MATERIAL_PRI,
                MATERIAL_SEC,
                price,
                weight,
                0_health,
                0_health,
                armorRating,
                TYPE_WRAPPER,
                weapon::WeaponInfo(),
                armorInfo,
                IS_PIXIE_ITEM,
                exclusiveRole));
        }

        const ItemPtr_t ArmorFactory::Make_Pants(
            const category::Enum CATEGORY,
            const TypeWrapper & TYPE_WRAPPER,
            const base_type::Enum TYPE,
            const material::Enum MATERIAL_PRI,
            const material::Enum MATERIAL_SEC,
            const bool IS_PIXIE_ITEM)
        {
            ArmorInfo armorInfo{ armor_type::Pants };
            armorInfo.is_pants = true;
            armorInfo.base = TYPE;

            auto const DETAILS{ ArmorDetailLoader::LookupArmorDetails(
                item::armor::base_type::ToString(TYPE) + "Pants") };

            Coin_t price{ DETAILS.price };
            Weight_t weight{ DETAILS.weight };
            Armor_t armorRating{ DETAILS.armor_rating };

            AdjustPrice(price, MATERIAL_PRI, MATERIAL_SEC, IS_PIXIE_ITEM);
            AdjustWeight(weight, MATERIAL_PRI, MATERIAL_SEC);

            // Prevent secondary material from altering the armor rating,
            // since it is only the material of the clasp.
            AdjustArmorRating(armorRating, MATERIAL_PRI, material::Nothing);

            creature::role::Enum exclusiveRole(creature::role::Count);
            if (TYPE == base_type::Plate)
            {
                exclusiveRole = creature::role::Knight;
            }

            return ItemWarehouse::Access().Store(std::make_unique<Item>(
                Make_Name(DETAILS.name, MATERIAL_PRI, MATERIAL_SEC, IS_PIXIE_ITEM),
                Make_Desc_Clasped(DETAILS.description, MATERIAL_PRI, MATERIAL_SEC, IS_PIXIE_ITEM),
                CATEGORY,
                weapon_type::NotAWeapon,
                armor_type::Pants,
                MATERIAL_PRI,
                MATERIAL_SEC,
                price,
                weight,
                0_health,
                0_health,
                armorRating,
                TYPE_WRAPPER,
                weapon::WeaponInfo(),
                armorInfo,
                IS_PIXIE_ITEM,
                exclusiveRole));
        }

        const ItemPtr_t ArmorFactory::Make_Boots(
            const category::Enum CATEGORY,
            const TypeWrapper & TYPE_WRAPPER,
            const base_type::Enum TYPE,
            const material::Enum MATERIAL_PRI,
            const material::Enum MATERIAL_SEC,
            const bool IS_PIXIE_ITEM)
        {
            ArmorInfo armorInfo{ armor_type::Boots };
            armorInfo.is_boots = true;
            armorInfo.base = TYPE;

            auto const TYPE_NAME_STR{ item::armor::base_type::ToString(TYPE) };

            auto const DETAILS{ ArmorDetailLoader::LookupArmorDetails(TYPE_NAME_STR + "Boots") };

            Coin_t price{ DETAILS.price };
            Weight_t weight{ DETAILS.weight };
            Armor_t armorRating{ DETAILS.armor_rating };

            AdjustPrice(price, MATERIAL_PRI, MATERIAL_SEC, IS_PIXIE_ITEM);
            AdjustWeight(weight, MATERIAL_PRI, MATERIAL_SEC);
            AdjustArmorRating(armorRating, MATERIAL_PRI, MATERIAL_SEC);

            creature::role::Enum exclusiveRole(creature::role::Count);
            if (TYPE == base_type::Plate)
            {
                exclusiveRole = creature::role::Knight;
            }

            return ItemWarehouse::Access().Store(std::make_unique<Item>(
                Make_Name(DETAILS.name, MATERIAL_PRI, MATERIAL_SEC, IS_PIXIE_ITEM),
                Make_Desc(DETAILS.description, MATERIAL_PRI, MATERIAL_SEC, "", IS_PIXIE_ITEM),
                CATEGORY,
                weapon_type::NotAWeapon,
                armor_type::Boots,
                MATERIAL_PRI,
                MATERIAL_SEC,
                price,
                weight,
                0_health,
                0_health,
                armorRating,
                TYPE_WRAPPER,
                weapon::WeaponInfo(),
                armorInfo,
                IS_PIXIE_ITEM,
                exclusiveRole));
        }

        const ItemPtr_t ArmorFactory::Make_Shirt(
            const category::Enum CATEGORY,
            const TypeWrapper & TYPE_WRAPPER,
            const base_type::Enum TYPE,
            const material::Enum MATERIAL_PRI,
            const material::Enum MATERIAL_SEC,
            const bool IS_PIXIE_ITEM)
        {
            ArmorInfo armorInfo{ armor_type::Shirt };
            armorInfo.is_shirt = true;
            armorInfo.base = TYPE;

            auto const TYPE_NAME_STR{ item::armor::base_type::ToString(TYPE) };

            auto const DETAILS{ ArmorDetailLoader::LookupArmorDetails(TYPE_NAME_STR + "Shirt") };

            Coin_t price{ DETAILS.price };
            Weight_t weight{ DETAILS.weight };
            Armor_t armorRating{ DETAILS.armor_rating };

            AdjustPrice(price, MATERIAL_PRI, MATERIAL_SEC, IS_PIXIE_ITEM);
            AdjustWeight(weight, MATERIAL_PRI, MATERIAL_SEC);

            // Prevent secondary material from altering the armor rating,
            // since it is only the material of the clasp.
            AdjustArmorRating(armorRating, MATERIAL_PRI, material::Nothing);

            creature::role::Enum exclusiveRole(creature::role::Count);
            if (TYPE == base_type::Plate)
            {
                exclusiveRole = creature::role::Knight;
            }

            return ItemWarehouse::Access().Store(std::make_unique<Item>(
                Make_Name(DETAILS.name, MATERIAL_PRI, MATERIAL_SEC, IS_PIXIE_ITEM),
                Make_Desc_Clasped(DETAILS.description, MATERIAL_PRI, MATERIAL_SEC, IS_PIXIE_ITEM),
                CATEGORY,
                weapon_type::NotAWeapon,
                armor_type::Shirt,
                MATERIAL_PRI,
                MATERIAL_SEC,
                price,
                weight,
                0_health,
                0_health,
                armorRating,
                TYPE_WRAPPER,
                weapon::WeaponInfo(),
                armorInfo,
                IS_PIXIE_ITEM,
                exclusiveRole));
        }

        const ItemPtr_t ArmorFactory::Make_Bracer(
            const category::Enum CATEGORY,
            const TypeWrapper & TYPE_WRAPPER,
            const base_type::Enum TYPE,
            const material::Enum MATERIAL_PRI,
            const material::Enum MATERIAL_SEC,
            const bool IS_PIXIE_ITEM)
        {
            ArmorInfo armorInfo{ armor_type::Bracer };
            armorInfo.is_bracer = true;
            armorInfo.base = TYPE;

            auto const DETAILS{ ArmorDetailLoader::LookupArmorDetails(
                item::armor::base_type::ToString(TYPE) + "Bracers") };

            Coin_t price{ DETAILS.price };
            Weight_t weight{ DETAILS.weight };
            Armor_t armorRating{ DETAILS.armor_rating };

            AdjustPrice(price, MATERIAL_PRI, MATERIAL_SEC, IS_PIXIE_ITEM);
            AdjustWeight(weight, MATERIAL_PRI, MATERIAL_SEC);
            AdjustArmorRating(armorRating, MATERIAL_PRI, MATERIAL_SEC);

            creature::role::Enum exclusiveRole(creature::role::Count);
            if (TYPE == base_type::Plate)
            {
                exclusiveRole = creature::role::Knight;
            }

            return ItemWarehouse::Access().Store(std::make_unique<Item>(
                Make_Name(DETAILS.name, MATERIAL_PRI, MATERIAL_SEC, IS_PIXIE_ITEM),
                Make_Desc(DETAILS.description, MATERIAL_PRI, MATERIAL_SEC, "", IS_PIXIE_ITEM),
                CATEGORY,
                weapon_type::NotAWeapon,
                armor_type::Bracer,
                MATERIAL_PRI,
                MATERIAL_SEC,
                price,
                weight,
                0_health,
                0_health,
                armorRating,
                TYPE_WRAPPER,
                weapon::WeaponInfo(),
                armorInfo,
                IS_PIXIE_ITEM,
                exclusiveRole));
        }

        const ItemPtr_t ArmorFactory::Make_Aventail(
            const category::Enum CATEGORY,
            const TypeWrapper & TYPE_WRAPPER,
            const base_type::Enum TYPE,
            const material::Enum MATERIAL_PRI,
            const material::Enum MATERIAL_SEC)
        {
            ArmorInfo armorInfo{ armor_type::Aventail };
            armorInfo.is_aventail = true;
            armorInfo.base = TYPE;

            auto const DETAILS{ ArmorDetailLoader::LookupArmorDetails(
                item::armor::base_type::ToString(TYPE) + "Aventail") };

            Coin_t price{ DETAILS.price };
            Weight_t weight{ DETAILS.weight };
            Armor_t armorRating{ DETAILS.armor_rating };

            AdjustPrice(price, MATERIAL_PRI, MATERIAL_SEC);
            AdjustWeight(weight, MATERIAL_PRI, MATERIAL_SEC);

            // Prevent secondary material from altering the armor rating,
            // since it is only the material of the clasp.
            AdjustArmorRating(armorRating, MATERIAL_PRI, material::Nothing);

            creature::role::Enum exclusiveRole(creature::role::Count);
            if (TYPE == base_type::Plate)
            {
                exclusiveRole = creature::role::Knight;
            }

            return ItemWarehouse::Access().Store(std::make_unique<Item>(
                Make_Name(DETAILS.name, MATERIAL_PRI, MATERIAL_SEC),
                Make_Desc_Clasped(DETAILS.description, MATERIAL_PRI, MATERIAL_SEC),
                CATEGORY,
                weapon_type::NotAWeapon,
                armor_type::Aventail,
                MATERIAL_PRI,
                MATERIAL_SEC,
                price,
                weight,
                0_health,
                0_health,
                armorRating,
                TYPE_WRAPPER,
                weapon::WeaponInfo(),
                armorInfo,
                exclusiveRole));
        }

        const ItemPtr_t ArmorFactory::Make_Cover(
            const category::Enum CATEGORY,
            const TypeWrapper & TYPE_WRAPPER,
            const cover_type::Enum COVER_TYPE,
            const material::Enum MATERIAL_PRI,
            const material::Enum MATERIAL_SEC,
            const bool IS_PIXIE_ITEM)
        {
            ArmorInfo armorInfo{ armor_type::Covering };
            armorInfo.cover = COVER_TYPE;

            auto const DETAILS{ ArmorDetailLoader::LookupArmorDetails(
                item::armor::cover_type::ToString(COVER_TYPE)) };

            Coin_t price{ DETAILS.price };
            Weight_t weight{ DETAILS.weight };
            Armor_t armorRating{ DETAILS.armor_rating };

            AdjustPrice(price, MATERIAL_PRI, MATERIAL_SEC, IS_PIXIE_ITEM);
            AdjustWeight(weight, MATERIAL_PRI, MATERIAL_SEC);

            // Prevent secondary material from altering the armor rating,
            // since it is only the material of the clasp.
            AdjustArmorRating(armorRating, MATERIAL_PRI, material::Nothing);

            return ItemWarehouse::Access().Store(std::make_unique<Item>(
                Make_Name(DETAILS.name, MATERIAL_PRI, MATERIAL_SEC, IS_PIXIE_ITEM),
                Make_Desc_Clasped(DETAILS.description, MATERIAL_PRI, MATERIAL_SEC, IS_PIXIE_ITEM),
                CATEGORY,
                weapon_type::NotAWeapon,
                armor_type::Covering,
                MATERIAL_PRI,
                MATERIAL_SEC,
                price,
                weight,
                0_health,
                0_health,
                armorRating,
                TYPE_WRAPPER,
                weapon::WeaponInfo(),
                armorInfo,
                IS_PIXIE_ITEM));
        }

        const ItemPtr_t ArmorFactory::Make_Skin(
            const material::Enum MATERIAL, const Rank_t & CREATURE_RANK, const bool IS_PIXIE_ITEM)
        {
            ArmorInfo armorInfo{ armor_type::Skin };

            ArmorDetails details;

            details.armor_rating = Armor_t(static_cast<Armor_t::type>(
                item::material::ArmorRatingBonusPri(MATERIAL).As<int>() + CREATURE_RANK.As<int>()));

            details.complexity = non_player::ownership::complexity_type::Simple;
            details.name = material::ToReadableString(MATERIAL) + " skin";
            details.price = 0_coin;

            details.weight = Weight_t(static_cast<Weight_t::type>(
                100.0f * material::WeightMult(MATERIAL, material::Nothing)));

            std::ostringstream ss;
            ss << "Skin made of " << material::ToReadableString(MATERIAL);

            details.description = ss.str();

            return ItemWarehouse::Access().Store(std::make_unique<Item>(
                details.name,
                details.description,
                static_cast<category::Enum>(ItemProfile::CategoryArmor() | category::BodyPart),
                weapon_type::NotAWeapon,
                armor_type::Skin,
                MATERIAL,
                material::Nothing,
                details.price,
                details.weight,
                0_health,
                0_health,
                details.armor_rating,
                TypeWrapper(),
                weapon::WeaponInfo(),
                armorInfo,
                IS_PIXIE_ITEM));
        }

    } // namespace armor
} // namespace item
} // namespace heroespath
