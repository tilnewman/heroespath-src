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

#include "item/armor-details.hpp"
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

        std::unique_ptr<ArmorFactory> ArmorFactory::instanceUPtr_{ nullptr };

        ArmorFactory::ArmorFactory() { M_HP_LOG_DBG("Singleton Construction: ArmorFactory"); }

        ArmorFactory::~ArmorFactory() { M_HP_LOG_DBG("Singleton Destruction: ArmorFactory"); }

        ArmorFactory * ArmorFactory::Instance()
        {
            if (instanceUPtr_.get() == nullptr)
            {
                M_HP_LOG_ERR("Singleton Instance() before Acquire(): ArmorFactory");
                Acquire();
            }

            return instanceUPtr_.get();
        }

        void ArmorFactory::Acquire()
        {
            if (instanceUPtr_.get() == nullptr)
            {
                instanceUPtr_ = std::make_unique<ArmorFactory>();
            }
            else
            {
                M_HP_LOG_ERR("Singleton Acquire() after Construction: ArmorFactory");
            }
        }

        void ArmorFactory::Release()
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (instanceUPtr_.get() != nullptr),
                "item::armor::ArmorFactory::Release() found instanceUPtr that was null.");

            instanceUPtr_.reset();
        }

        const ItemPtr_t ArmorFactory::Make_Shield(
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

            auto const DETAILS{ ArmorDetailLoader::Instance()->LookupArmorDetails(
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

            auto itemPtr{ ItemWarehouse::Instance()->Store(new Item(
                ssName.str(),
                ssDesc.str(),
                static_cast<category::Enum>(
                    category::Armor | category::Equippable | category::Wearable
                    | category::OneHanded),
                misc_type::NotMisc,
                weapon_type::NotAWeapon,
                armor_type::Sheild,
                MATERIAL_PRI,
                MATERIAL_SEC,
                "",
                price,
                weight,
                0_health,
                0_health,
                armorRating,
                exclusiveRole,
                weapon::WeaponInfo(),
                armorInfo)) };

            itemPtr->ImageFilename(
                sfml_util::gui::ItemImageManager::Instance()->GetImageFilename(itemPtr));

            return itemPtr;
        }

        const ItemPtr_t ArmorFactory::Make_Helm(
            const helm_type::Enum HELM_TYPE,
            const material::Enum MATERIAL_PRI,
            const material::Enum MATERIAL_SEC)
        {
            ArmorInfo armorInfo{ armor_type::Helm };
            armorInfo.helm = HELM_TYPE;

            auto const DETAILS{ ArmorDetailLoader::Instance()->LookupArmorDetails(
                item::armor::helm_type::ToString(HELM_TYPE)) };

            Coin_t price{ DETAILS.price };
            Weight_t weight{ DETAILS.weight };
            Armor_t armorRating{ DETAILS.armor_rating };

            auto materialPri{ MATERIAL_PRI };
            auto materialSec{ MATERIAL_SEC };
            if (HELM_TYPE == helm_type::Leather)
            {
                materialPri = material::HardLeather;
                materialSec = material::Nothing;
            }

            AdjustPrice(price, materialPri, materialSec);
            AdjustWeight(weight, materialPri, materialSec);
            AdjustArmorRating(armorRating, materialPri, materialSec);

            auto exclusiveRole{ creature::role::Count };
            if (HELM_TYPE == helm_type::Great)
            {
                exclusiveRole = creature::role::Knight;
            }

            auto itemPtr{ ItemWarehouse::Instance()->Store(new Item(
                Make_Name(DETAILS.name, materialPri, materialSec),
                Make_Desc(DETAILS.description, materialPri, materialSec),
                static_cast<category::Enum>(
                    category::Armor | category::Equippable | category::Wearable),
                misc_type::NotMisc,
                weapon_type::NotAWeapon,
                armor_type::Helm,
                materialPri,
                materialSec,
                "",
                price,
                weight,
                0_health,
                0_health,
                armorRating,
                exclusiveRole,
                weapon::WeaponInfo(),
                armorInfo)) };

            itemPtr->ImageFilename(
                sfml_util::gui::ItemImageManager::Instance()->GetImageFilename(itemPtr));

            return itemPtr;
        }

        const ItemPtr_t ArmorFactory::Make_Gauntlets(
            const base_type::Enum TYPE,
            const material::Enum MATERIAL_PRI,
            const material::Enum MATERIAL_SEC,
            const bool IS_PIXIE_ITEM)
        {
            ArmorInfo armorInfo{ armor_type::Gauntlets };
            armorInfo.is_gauntlets = true;
            armorInfo.base = TYPE;

            auto const DETAILS{ ArmorDetailLoader::Instance()->LookupArmorDetails([TYPE]() {
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

            auto itemPtr{ ItemWarehouse::Instance()->Store(new Item(
                Make_Name(DETAILS.name, MATERIAL_PRI, MATERIAL_SEC, IS_PIXIE_ITEM),
                Make_Desc(DETAILS.description, MATERIAL_PRI, MATERIAL_SEC, "", IS_PIXIE_ITEM),
                static_cast<category::Enum>(
                    category::Armor | category::Equippable | category::Wearable),
                misc_type::NotMisc,
                weapon_type::NotAWeapon,
                armor_type::Gauntlets,
                MATERIAL_PRI,
                MATERIAL_SEC,
                "",
                price,
                weight,
                0_health,
                0_health,
                armorRating,
                exclusiveRole,
                weapon::WeaponInfo(),
                armorInfo,
                IS_PIXIE_ITEM)) };

            itemPtr->ImageFilename(
                sfml_util::gui::ItemImageManager::Instance()->GetImageFilename(itemPtr));

            return itemPtr;
        }

        const ItemPtr_t ArmorFactory::Make_Pants(
            const base_type::Enum TYPE,
            const material::Enum MATERIAL_PRI,
            const material::Enum MATERIAL_SEC,
            const bool IS_PIXIE_ITEM)
        {
            ArmorInfo armorInfo{ armor_type::Pants };
            armorInfo.is_pants = true;
            armorInfo.base = TYPE;

            auto const DETAILS{ ArmorDetailLoader::Instance()->LookupArmorDetails(
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

            auto itemPtr{ ItemWarehouse::Instance()->Store(new Item(
                Make_Name(DETAILS.name, MATERIAL_PRI, MATERIAL_SEC, IS_PIXIE_ITEM),
                Make_Desc_Clasped(DETAILS.description, MATERIAL_PRI, MATERIAL_SEC, IS_PIXIE_ITEM),
                static_cast<category::Enum>(
                    category::Armor | category::Equippable | category::Wearable),
                misc_type::NotMisc,
                weapon_type::NotAWeapon,
                armor_type::Pants,
                MATERIAL_PRI,
                MATERIAL_SEC,
                "",
                price,
                weight,
                0_health,
                0_health,
                armorRating,
                exclusiveRole,
                weapon::WeaponInfo(),
                armorInfo,
                IS_PIXIE_ITEM)) };

            itemPtr->ImageFilename(
                sfml_util::gui::ItemImageManager::Instance()->GetImageFilename(itemPtr));

            return itemPtr;
        }

        const ItemPtr_t ArmorFactory::Make_Boots(
            const base_type::Enum TYPE,
            const material::Enum MATERIAL_PRI,
            const material::Enum MATERIAL_SEC,
            const bool IS_PIXIE_ITEM)
        {
            ArmorInfo armorInfo{ armor_type::Boots };
            armorInfo.is_boots = true;
            armorInfo.base = TYPE;

            auto const TYPE_NAME_STR{ item::armor::base_type::ToString(TYPE) };

            auto const DETAILS{ ArmorDetailLoader::Instance()->LookupArmorDetails(
                TYPE_NAME_STR + "Boots") };

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

            auto itemPtr{ ItemWarehouse::Instance()->Store(new Item(
                Make_Name(DETAILS.name, MATERIAL_PRI, MATERIAL_SEC, IS_PIXIE_ITEM),
                Make_Desc(DETAILS.description, MATERIAL_PRI, MATERIAL_SEC, "", IS_PIXIE_ITEM),
                static_cast<category::Enum>(
                    category::Armor | category::Equippable | category::Wearable),
                misc_type::NotMisc,
                weapon_type::NotAWeapon,
                armor_type::Boots,
                MATERIAL_PRI,
                MATERIAL_SEC,
                "",
                price,
                weight,
                0_health,
                0_health,
                armorRating,
                exclusiveRole,
                weapon::WeaponInfo(),
                armorInfo,
                IS_PIXIE_ITEM)) };

            itemPtr->ImageFilename(
                sfml_util::gui::ItemImageManager::Instance()->GetImageFilename(itemPtr));

            return itemPtr;
        }

        const ItemPtr_t ArmorFactory::Make_Shirt(
            const base_type::Enum TYPE,
            const material::Enum MATERIAL_PRI,
            const material::Enum MATERIAL_SEC,
            const bool IS_PIXIE_ITEM)
        {
            ArmorInfo armorInfo{ armor_type::Shirt };
            armorInfo.is_shirt = true;
            armorInfo.base = TYPE;

            auto const TYPE_NAME_STR{ item::armor::base_type::ToString(TYPE) };

            auto const DETAILS{ ArmorDetailLoader::Instance()->LookupArmorDetails(
                TYPE_NAME_STR + "Shirt") };

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

            auto itemPtr{ ItemWarehouse::Instance()->Store(new Item(
                Make_Name(DETAILS.name, MATERIAL_PRI, MATERIAL_SEC, IS_PIXIE_ITEM),
                Make_Desc_Clasped(DETAILS.description, MATERIAL_PRI, MATERIAL_SEC, IS_PIXIE_ITEM),
                static_cast<category::Enum>(
                    category::Armor | category::Equippable | category::Wearable),
                misc_type::NotMisc,
                weapon_type::NotAWeapon,
                armor_type::Shirt,
                MATERIAL_PRI,
                MATERIAL_SEC,
                "",
                price,
                weight,
                0_health,
                0_health,
                armorRating,
                exclusiveRole,
                weapon::WeaponInfo(),
                armorInfo,
                IS_PIXIE_ITEM)) };

            itemPtr->ImageFilename(
                sfml_util::gui::ItemImageManager::Instance()->GetImageFilename(itemPtr));

            return itemPtr;
        }

        const ItemPtr_t ArmorFactory::Make_Bracer(
            const base_type::Enum TYPE,
            const material::Enum MATERIAL_PRI,
            const material::Enum MATERIAL_SEC,
            const bool IS_PIXIE_ITEM)
        {
            ArmorInfo armorInfo{ armor_type::Bracer };
            armorInfo.is_bracer = true;
            armorInfo.base = TYPE;

            auto const DETAILS{ ArmorDetailLoader::Instance()->LookupArmorDetails(
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

            auto itemPtr{ ItemWarehouse::Instance()->Store(new Item(
                Make_Name(DETAILS.name, MATERIAL_PRI, MATERIAL_SEC, IS_PIXIE_ITEM),
                Make_Desc(DETAILS.description, MATERIAL_PRI, MATERIAL_SEC, "", IS_PIXIE_ITEM),
                static_cast<category::Enum>(
                    category::Armor | category::Equippable | category::Wearable),
                misc_type::NotMisc,
                weapon_type::NotAWeapon,
                armor_type::Bracer,
                MATERIAL_PRI,
                MATERIAL_SEC,
                "",
                price,
                weight,
                0_health,
                0_health,
                armorRating,
                exclusiveRole,
                weapon::WeaponInfo(),
                armorInfo,
                IS_PIXIE_ITEM)) };

            itemPtr->ImageFilename(
                sfml_util::gui::ItemImageManager::Instance()->GetImageFilename(itemPtr));

            return itemPtr;
        }

        const ItemPtr_t ArmorFactory::Make_Aventail(
            const base_type::Enum TYPE,
            const material::Enum MATERIAL_PRI,
            const material::Enum MATERIAL_SEC)
        {
            ArmorInfo armorInfo{ armor_type::Aventail };
            armorInfo.is_aventail = true;
            armorInfo.base = TYPE;

            auto const DETAILS{ ArmorDetailLoader::Instance()->LookupArmorDetails(
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

            auto itemPtr{ ItemWarehouse::Instance()->Store(new Item(
                Make_Name(DETAILS.name, MATERIAL_PRI, MATERIAL_SEC),
                Make_Desc_Clasped(DETAILS.description, MATERIAL_PRI, MATERIAL_SEC),
                static_cast<category::Enum>(
                    category::Armor | category::Equippable | category::Wearable),
                misc_type::NotMisc,
                weapon_type::NotAWeapon,
                armor_type::Aventail,
                MATERIAL_PRI,
                MATERIAL_SEC,
                "",
                price,
                weight,
                0_health,
                0_health,
                armorRating,
                exclusiveRole,
                weapon::WeaponInfo(),
                armorInfo)) };

            itemPtr->ImageFilename(
                sfml_util::gui::ItemImageManager::Instance()->GetImageFilename(itemPtr));

            return itemPtr;
        }

        const ItemPtr_t ArmorFactory::Make_Cover(
            const cover_type::Enum COVER_TYPE,
            const material::Enum MATERIAL_PRI,
            const material::Enum MATERIAL_SEC,
            const bool IS_PIXIE_ITEM)
        {
            ArmorInfo armorInfo{ armor_type::Covering };
            armorInfo.cover = COVER_TYPE;

            auto const DETAILS{ ArmorDetailLoader::Instance()->LookupArmorDetails(
                item::armor::cover_type::ToString(COVER_TYPE)) };

            Coin_t price{ DETAILS.price };
            Weight_t weight{ DETAILS.weight };
            Armor_t armorRating{ DETAILS.armor_rating };

            AdjustPrice(price, MATERIAL_PRI, MATERIAL_SEC, IS_PIXIE_ITEM);
            AdjustWeight(weight, MATERIAL_PRI, MATERIAL_SEC);

            // Prevent secondary material from altering the armor rating,
            // since it is only the material of the clasp.
            AdjustArmorRating(armorRating, MATERIAL_PRI, material::Nothing);

            auto itemPtr{ ItemWarehouse::Instance()->Store(new Item(
                Make_Name(DETAILS.name, MATERIAL_PRI, MATERIAL_SEC, IS_PIXIE_ITEM),
                Make_Desc_Clasped(DETAILS.description, MATERIAL_PRI, MATERIAL_SEC, IS_PIXIE_ITEM),
                static_cast<category::Enum>(
                    category::Armor | category::Equippable | category::Wearable),
                misc_type::NotMisc,
                weapon_type::NotAWeapon,
                armor_type::Covering,
                MATERIAL_PRI,
                MATERIAL_SEC,
                "",
                price,
                weight,
                0_health,
                0_health,
                armorRating,
                creature::role::Count,
                weapon::WeaponInfo(),
                armorInfo,
                IS_PIXIE_ITEM)) };

            itemPtr->ImageFilename(
                sfml_util::gui::ItemImageManager::Instance()->GetImageFilename(itemPtr));

            return itemPtr;
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
            ss << "Skin made of " << material::ToReadableString(MATERIAL)
               << ".  The Armor Rating increases with Rank.";

            details.description = ss.str();

            auto itemPtr{ ItemWarehouse::Instance()->Store(new Item(
                details.name,
                details.description,
                static_cast<category::Enum>(
                    category::Armor | category::Equippable | category::Wearable
                    | category::BodyPart),
                misc_type::NotMisc,
                weapon_type::NotAWeapon,
                armor_type::Skin,
                MATERIAL,
                material::Nothing,
                "",
                details.price,
                details.weight,
                0_health,
                0_health,
                details.armor_rating,
                creature::role::Count,
                weapon::WeaponInfo(),
                armorInfo,
                IS_PIXIE_ITEM)) };

            itemPtr->ImageFilename(
                sfml_util::gui::ItemImageManager::Instance()->GetImageFilename(itemPtr));

            return itemPtr;
        }
    } // namespace armor
} // namespace item
} // namespace heroespath
