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
// armor-ratings.cpp
//
#include "armor-ratings.hpp"

#include "item/item-factory.hpp"
#include "item/item-warehouse.hpp"
#include "item/item.hpp"
#include "log/log-macros.hpp"
#include "misc/assertlogandthrow.hpp"

#include <numeric>

namespace heroespath
{
namespace item
{

    Armor_t ArmorRatings::clothesCloth_(0_armor);
    Armor_t ArmorRatings::clothesSoftLeather_(0_armor);
    Armor_t ArmorRatings::clothesHardLeather_(0_armor);
    Armor_t ArmorRatings::armoredLesserSoftLeather_(0_armor);
    Armor_t ArmorRatings::armoredLesserSteel_(0_armor);
    Armor_t ArmorRatings::armoredLesserDiamond_(0_armor);
    Armor_t ArmorRatings::armoredGreaterSoftLeather_(0_armor);
    Armor_t ArmorRatings::armoredGreaterSteel_(0_armor);
    Armor_t ArmorRatings::armoredGreaterDiamond_(0_armor);

    void ArmorRatings::Setup()
    {
        clothesCloth_ = ClothesSetRating(item::material::Cloth);
        clothesSoftLeather_ = ClothesSetRating(item::material::SoftLeather);
        clothesHardLeather_ = ClothesSetRating(item::material::HardLeather);

        armoredLesserSoftLeather_ = LesserArmorSetRating(item::material::SoftLeather);
        armoredLesserSteel_ = LesserArmorSetRating(item::material::Steel);
        armoredLesserDiamond_ = LesserArmorSetRating(item::material::Diamond);

        armoredGreaterSoftLeather_ = GreaterArmorSetRating(item::material::SoftLeather);
        armoredGreaterSteel_ = GreaterArmorSetRating(item::material::Steel);
        armoredGreaterDiamond_ = GreaterArmorSetRating(item::material::Diamond);
    }

    Armor_t ArmorRatings::ClothesSetRating(const item::material::Enum PRIMARY_MATERIAL)
    {
        using namespace item::armor;

        ItemPVec_t itemPVec;

        ItemProfile cloakProfile;
        cloakProfile.SetCover(cover_type::Cloak, PRIMARY_MATERIAL);
        itemPVec.emplace_back(ItemFactory::Make(cloakProfile));

        ItemProfile vestProfile;
        vestProfile.SetCover(cover_type::Vest, PRIMARY_MATERIAL);
        itemPVec.emplace_back(ItemFactory::Make(vestProfile));

        ItemProfile shirtProfile;
        shirtProfile.SetShirt(base_type::Plain, PRIMARY_MATERIAL);
        itemPVec.emplace_back(ItemFactory::Make(shirtProfile));

        ItemProfile glovesProfile;
        glovesProfile.SetGauntlets(base_type::Plain, PRIMARY_MATERIAL);
        itemPVec.emplace_back(ItemFactory::Make(glovesProfile));

        ItemProfile pantsProfile;
        pantsProfile.SetPants(base_type::Plain, PRIMARY_MATERIAL);
        itemPVec.emplace_back(ItemFactory::Make(pantsProfile));

        ItemProfile bootsProfile;
        bootsProfile.SetBoots(armor::base_type::Plain, PRIMARY_MATERIAL);
        itemPVec.emplace_back(ItemFactory::Make(bootsProfile));

        return GetTotalArmorRatingAndFree(itemPVec);
    }

    Armor_t ArmorRatings::LesserArmorSetRating(const item::material::Enum PRIMARY_MATERIAL)
    {
        using namespace item::armor;

        ItemPVec_t itemPVec;

        ItemProfile shieldProfile;
        shieldProfile.SetShield(
            shield_type::Buckler,
            ((PRIMARY_MATERIAL == material::SoftLeather) ? material::Wood : PRIMARY_MATERIAL));

        itemPVec.emplace_back(ItemFactory::Make(shieldProfile));

        ItemProfile helmProfile;
        helmProfile.SetHelm(
            ((PRIMARY_MATERIAL == material::SoftLeather) ? helm_type::Leather : helm_type::Kettle),
            PRIMARY_MATERIAL);

        itemPVec.emplace_back(ItemFactory::Make(helmProfile));

        auto const BASE_TYPE{ (
            (PRIMARY_MATERIAL == material::SoftLeather) ? base_type::Plain : base_type::Mail) };

        ItemProfile glovesProfile;
        glovesProfile.SetGauntlets(BASE_TYPE, PRIMARY_MATERIAL);
        itemPVec.emplace_back(ItemFactory::Make(glovesProfile));

        ItemProfile pantsProfile;
        pantsProfile.SetPants(BASE_TYPE, PRIMARY_MATERIAL);
        itemPVec.emplace_back(ItemFactory::Make(pantsProfile));

        ItemProfile bootsProfile;
        bootsProfile.SetBoots(BASE_TYPE, PRIMARY_MATERIAL);
        itemPVec.emplace_back(ItemFactory::Make(bootsProfile));

        ItemProfile shirtProfile;
        shirtProfile.SetShirt(BASE_TYPE, PRIMARY_MATERIAL);
        itemPVec.emplace_back(ItemFactory::Make(shirtProfile));

        ItemProfile bracerProfile;
        bracerProfile.SetBracer(BASE_TYPE, PRIMARY_MATERIAL);
        itemPVec.emplace_back(ItemFactory::Make(bracerProfile));

        ItemProfile avenProfile;
        avenProfile.SetAventail(BASE_TYPE, PRIMARY_MATERIAL);
        itemPVec.emplace_back(ItemFactory::Make(bracerProfile));

        ItemProfile cloakProfile;
        cloakProfile.SetCover(cover_type::Cloak, material::SoftLeather);
        itemPVec.emplace_back(ItemFactory::Make(cloakProfile));

        return GetTotalArmorRatingAndFree(itemPVec);
    }

    Armor_t ArmorRatings::GreaterArmorSetRating(const item::material::Enum PRIMARY_MATERIAL)
    {
        using namespace item::armor;

        ItemPVec_t itemPVec;

        ItemProfile shieldProfile;
        shieldProfile.SetShield(
            shield_type::Pavis,
            ((PRIMARY_MATERIAL == material::SoftLeather) ? material::Wood : PRIMARY_MATERIAL));

        itemPVec.emplace_back(ItemFactory::Make(shieldProfile));

        ItemProfile helmProfile;
        helmProfile.SetHelm(
            ((PRIMARY_MATERIAL == material::SoftLeather) ? helm_type::Leather : helm_type::Great),
            PRIMARY_MATERIAL);

        itemPVec.emplace_back(ItemFactory::Make(helmProfile));

        auto const BASE_TYPE{ (
            (PRIMARY_MATERIAL == material::SoftLeather) ? base_type::Plain : base_type::Plate) };

        ItemProfile glovesProfile;
        glovesProfile.SetGauntlets(BASE_TYPE, PRIMARY_MATERIAL);
        itemPVec.emplace_back(ItemFactory::Make(glovesProfile));

        ItemProfile pantsProfile;
        pantsProfile.SetPants(BASE_TYPE, PRIMARY_MATERIAL);
        itemPVec.emplace_back(ItemFactory::Make(pantsProfile));

        ItemProfile bootsProfile;
        bootsProfile.SetBoots(BASE_TYPE, PRIMARY_MATERIAL);
        itemPVec.emplace_back(ItemFactory::Make(bootsProfile));

        ItemProfile shirtProfile;
        shirtProfile.SetShirt(BASE_TYPE, PRIMARY_MATERIAL);
        itemPVec.emplace_back(ItemFactory::Make(shirtProfile));

        ItemProfile bracerProfile;
        bracerProfile.SetBracer(BASE_TYPE, PRIMARY_MATERIAL);
        itemPVec.emplace_back(ItemFactory::Make(bracerProfile));

        ItemProfile avenProfile;
        avenProfile.SetAventail(BASE_TYPE, PRIMARY_MATERIAL);
        itemPVec.emplace_back(ItemFactory::Make(bracerProfile));

        ItemProfile cloakProfile;
        cloakProfile.SetCover(cover_type::Cloak, material::HardLeather);
        itemPVec.emplace_back(ItemFactory::Make(cloakProfile));

        return GetTotalArmorRatingAndFree(itemPVec);
    }

    Armor_t ArmorRatings::GetTotalArmorRatingAndFree(ItemPVec_t & itemPVec)
    {
        auto const TOTAL_ARMOR_RATING{ std::accumulate(
            std::begin(itemPVec),
            std::end(itemPVec),
            0_armor,
            [](auto const SUBTOTAL, auto const ITEM_PTR) {
                return SUBTOTAL + ITEM_PTR->ArmorRating();
            }) };

        ItemWarehouse::Access().Free(itemPVec);
        return TOTAL_ARMOR_RATING;
    }

} // namespace item
} // namespace heroespath
