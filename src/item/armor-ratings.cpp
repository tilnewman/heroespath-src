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

#include <numeric>

namespace heroespath
{
namespace item
{

    ArmorRatings::ArmorRatings()
        : lesserSteel_(LesserArmorSetRating(item::material::Steel))
        , greaterSteel_(GreaterArmorSetRating(item::material::Steel))
        , greaterDiamond_(GreaterArmorSetRating(item::material::Diamond))
    {}

    void ArmorRatings::LogCommonArmorRatings() const
    {
        M_HP_LOG_DBG(
            "\nArmor Ratings:"
            << "\n\t Cloth Clothes: " << ClothesSetRating(item::material::Cloth)
            << "\n\t Leather Clothes: " << ClothesSetRating(item::material::Leather)
            << "\n\t Lesser Leather Armor: " << LesserArmorSetRating(item::material::Leather)
            << "\n\t Lesser Steel Armor: " << lesserSteel_
            << "\n\t Lesser Diamond Armor: " << LesserArmorSetRating(item::material::Diamond)
            << "\n\t Greater Leather Armor: " << GreaterArmorSetRating(item::material::Leather)
            << "\n\t Greater Steel Armor: " << greaterSteel_
            << "\n\t Greater Diamond Armor: " << greaterDiamond_);
    }

    Armor_t ArmorRatings::ClothesSetRating(const item::material::Enum PRIMARY_MATERIAL) const
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

    Armor_t ArmorRatings::LesserArmorSetRating(const item::material::Enum PRIMARY_MATERIAL) const
    {
        using namespace item::armor;

        ItemPVec_t itemPVec;

        ItemProfile shieldProfile;
        shieldProfile.SetShield(
            shield_type::Buckler,
            ((PRIMARY_MATERIAL == material::Leather) ? material::Wood : PRIMARY_MATERIAL));

        itemPVec.emplace_back(ItemFactory::Make(shieldProfile));

        ItemProfile helmProfile;
        helmProfile.SetHelm(
            ((PRIMARY_MATERIAL == material::Leather) ? helm_type::Leather : helm_type::Kettle),
            PRIMARY_MATERIAL);

        itemPVec.emplace_back(ItemFactory::Make(helmProfile));

        auto const BASE_TYPE{ (
            (PRIMARY_MATERIAL == material::Leather) ? base_type::Plain : base_type::Mail) };

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
        cloakProfile.SetCover(cover_type::Cloak, material::Leather);
        itemPVec.emplace_back(ItemFactory::Make(cloakProfile));

        return GetTotalArmorRatingAndFree(itemPVec);
    }

    Armor_t ArmorRatings::GreaterArmorSetRating(const item::material::Enum PRIMARY_MATERIAL) const
    {
        using namespace item::armor;

        ItemPVec_t itemPVec;

        ItemProfile shieldProfile;
        shieldProfile.SetShield(
            shield_type::Pavis,
            ((PRIMARY_MATERIAL == material::Leather) ? material::Wood : PRIMARY_MATERIAL));

        itemPVec.emplace_back(ItemFactory::Make(shieldProfile));

        ItemProfile helmProfile;
        helmProfile.SetHelm(
            ((PRIMARY_MATERIAL == material::Leather) ? helm_type::Leather : helm_type::Great),
            PRIMARY_MATERIAL);

        itemPVec.emplace_back(ItemFactory::Make(helmProfile));

        auto const BASE_TYPE{ (
            (PRIMARY_MATERIAL == material::Leather) ? base_type::Plain : base_type::Plate) };

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
        cloakProfile.SetCover(cover_type::Cloak, material::Leather);
        itemPVec.emplace_back(ItemFactory::Make(cloakProfile));

        return GetTotalArmorRatingAndFree(itemPVec);
    }

    Armor_t ArmorRatings::GetTotalArmorRatingAndFree(ItemPVec_t & itemPVec) const
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
