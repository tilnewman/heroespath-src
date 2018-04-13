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

#include "item/armor-factory.hpp"
#include "item/item-warehouse.hpp"
#include "item/item.hpp"
#include "log/log-macros.hpp"
#include "misc/assertlogandthrow.hpp"

#include <numeric>

namespace heroespath
{
namespace item
{

    ArmorRatings::ArmorRatings()
        : isSetup_(false)
        , clothesCloth_(0_armor)
        , clothesSoftLeather_(0_armor)
        , clothesHardLeather_(0_armor)
        , armoredLesserSoftLeather_(0_armor)
        , armoredLesserSteel_(0_armor)
        , armoredLesserDiamond_(0_armor)
        , armoredGreaterSoftLeather_(0_armor)
        , armoredGreaterSteel_(0_armor)
        , armoredGreaterDiamond_(0_armor)
    {}

    void ArmorRatings::EnsureSetup()
    {
        if (false == isSetup_)
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

            isSetup_ = true;
        }
    }

    Armor_t ArmorRatings::ClothesSetRating(const item::material::Enum PRIMARY_MATERIAL) const
    {
        using namespace item::armor;

        ItemPVec_t itemPVec;

        itemPVec.emplace_back(ArmorFactory::Make_Skin(material::Flesh, 1_rank, false));

        itemPVec.emplace_back(
            ArmorFactory::Make_Boots(TypeWrapper(), base_type::Plain, PRIMARY_MATERIAL));

        itemPVec.emplace_back(ArmorFactory::Make_Shirt(TypeWrapper(), base_type::Plain));

        itemPVec.emplace_back(
            ArmorFactory::Make_Pants(TypeWrapper(), base_type::Plain, material::Cloth));

        itemPVec.emplace_back(
            ArmorFactory::Make_Cover(TypeWrapper(), cover_type::Vest, PRIMARY_MATERIAL));

        itemPVec.emplace_back(
            ArmorFactory::Make_Cover(TypeWrapper(), cover_type::Cloak, PRIMARY_MATERIAL));

        itemPVec.emplace_back(
            ArmorFactory::Make_Gauntlets(TypeWrapper(), base_type::Plain, PRIMARY_MATERIAL));

        return GetTotalArmorRatingAndFree(itemPVec);
    }

    Armor_t ArmorRatings::LesserArmorSetRating(const item::material::Enum PRIMARY_MATERIAL) const
    {
        using namespace item::armor;

        ItemPVec_t itemPVec;

        itemPVec.emplace_back(ArmorFactory::Make_Shield(
            TypeWrapper(),
            shield_type::Buckler,
            ((PRIMARY_MATERIAL == material::SoftLeather) ? material::Wood : PRIMARY_MATERIAL)));

        itemPVec.emplace_back(ArmorFactory::Make_Helm(
            TypeWrapper(),
            ((PRIMARY_MATERIAL == material::SoftLeather) ? helm_type::Leather : helm_type::Kettle),
            PRIMARY_MATERIAL));

        auto const BASE_TYPE{ (
            (PRIMARY_MATERIAL == material::SoftLeather) ? base_type::Plain : base_type::Mail) };

        itemPVec.emplace_back(
            ArmorFactory::Make_Gauntlets(TypeWrapper(), BASE_TYPE, PRIMARY_MATERIAL));

        itemPVec.emplace_back(ArmorFactory::Make_Pants(TypeWrapper(), BASE_TYPE, PRIMARY_MATERIAL));
        itemPVec.emplace_back(ArmorFactory::Make_Boots(TypeWrapper(), BASE_TYPE, PRIMARY_MATERIAL));
        itemPVec.emplace_back(ArmorFactory::Make_Shirt(TypeWrapper(), BASE_TYPE));

        itemPVec.emplace_back(
            ArmorFactory::Make_Bracer(TypeWrapper(), BASE_TYPE, PRIMARY_MATERIAL));

        itemPVec.emplace_back(
            ArmorFactory::Make_Aventail(TypeWrapper(), BASE_TYPE, PRIMARY_MATERIAL));

        itemPVec.emplace_back(
            ArmorFactory::Make_Cover(TypeWrapper(), cover_type::Cloak, material::SoftLeather));

        itemPVec.emplace_back(ArmorFactory::Make_Skin(material::Flesh, 1_rank, false));

        return GetTotalArmorRatingAndFree(itemPVec);
    }

    Armor_t ArmorRatings::GreaterArmorSetRating(const item::material::Enum PRIMARY_MATERIAL) const
    {
        using namespace item::armor;

        ItemPVec_t itemPVec;

        itemPVec.emplace_back(ArmorFactory::Make_Shield(
            TypeWrapper(),
            shield_type::Pavis,
            ((PRIMARY_MATERIAL == material::SoftLeather) ? material::Wood : PRIMARY_MATERIAL)));

        itemPVec.emplace_back(ArmorFactory::Make_Helm(
            TypeWrapper(),
            ((PRIMARY_MATERIAL == material::SoftLeather) ? helm_type::Leather : helm_type::Great),
            PRIMARY_MATERIAL));

        auto const BASE_TYPE{ (
            (PRIMARY_MATERIAL == material::SoftLeather) ? base_type::Plain : base_type::Plate) };

        itemPVec.emplace_back(ArmorFactory::Make_Gauntlets(
            TypeWrapper(), BASE_TYPE, PRIMARY_MATERIAL, PRIMARY_MATERIAL));

        itemPVec.emplace_back(ArmorFactory::Make_Pants(TypeWrapper(), BASE_TYPE, PRIMARY_MATERIAL));

        itemPVec.emplace_back(
            ArmorFactory::Make_Boots(TypeWrapper(), BASE_TYPE, PRIMARY_MATERIAL, PRIMARY_MATERIAL));

        itemPVec.emplace_back(ArmorFactory::Make_Shirt(TypeWrapper(), BASE_TYPE, PRIMARY_MATERIAL));

        itemPVec.emplace_back(ArmorFactory::Make_Bracer(
            TypeWrapper(), BASE_TYPE, PRIMARY_MATERIAL, PRIMARY_MATERIAL));

        itemPVec.emplace_back(ArmorFactory::Make_Aventail(
            TypeWrapper(), BASE_TYPE, PRIMARY_MATERIAL, PRIMARY_MATERIAL));

        itemPVec.emplace_back(
            ArmorFactory::Make_Cover(TypeWrapper(), cover_type::Cloak, material::HardLeather));

        itemPVec.emplace_back(ArmorFactory::Make_Skin(material::Flesh, 1_rank, false));

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
