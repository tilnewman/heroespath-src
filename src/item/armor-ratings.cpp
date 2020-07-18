// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// armor-ratings.cpp
//
#include "armor-ratings.hpp"

#include "item/item-factory.hpp"
#include "item/item-warehouse.hpp"
#include "item/item.hpp"
#include "misc/log-macros.hpp"

#include <numeric>

namespace heroespath
{
namespace item
{

    std::unique_ptr<ArmorRatings> ArmorRatings::instanceUPtr_;

    ArmorRatings::ArmorRatings()
        : lesserSteel_(0_armor)
        , greaterSteel_(0_armor)
        , greaterDiamond_(0_armor)
    {
        M_HP_LOG_DBG("Subsystem Construction: ArmorRatings");
    }

    ArmorRatings::~ArmorRatings() { M_HP_LOG_DBG("Subsystem Destruction: ArmorRatings"); }

    misc::NotNull<ArmorRatings *> ArmorRatings::Instance()
    {
        if (!instanceUPtr_)
        {
            M_HP_LOG_ERR("Subsystem Instance() called but instanceUPtr_ was null: ArmorRatings");

            Create();
        }

        return misc::NotNull<ArmorRatings *>(instanceUPtr_.get());
    }

    void ArmorRatings::Create()
    {
        if (!instanceUPtr_)
        {
            instanceUPtr_ = std::make_unique<ArmorRatings>();
        }
        else
        {
            M_HP_LOG_ERR("Subsystem Create() after Construction: ArmorRatings");
        }
    }

    void ArmorRatings::Destroy() { instanceUPtr_.reset(); }

    void ArmorRatings::Initialize()
    {
        lesserSteel_ = LesserArmorSetRating(item::Material::Steel);
        greaterSteel_ = GreaterArmorSetRating(item::Material::Steel);
        greaterDiamond_ = GreaterArmorSetRating(item::Material::Diamond);
    }

    void ArmorRatings::LogCommonArmorRatings() const
    {
        M_HP_LOG_DBG(
            "\nArmor Ratings:"
            << "\n\t Cloth Clothes: " << ClothesSetRating(item::Material::Cloth)
            << "\n\t Leather Clothes: " << ClothesSetRating(item::Material::Leather)
            << "\n\t Lesser Leather Armor: " << LesserArmorSetRating(item::Material::Leather)
            << "\n\t Lesser Steel Armor: " << lesserSteel_
            << "\n\t Lesser Diamond Armor: " << LesserArmorSetRating(item::Material::Diamond)
            << "\n\t Greater Leather Armor: " << GreaterArmorSetRating(item::Material::Leather)
            << "\n\t Greater Steel Armor: " << greaterSteel_
            << "\n\t Greater Diamond Armor: " << greaterDiamond_);
    }

    Armor_t ArmorRatings::ClothesSetRating(const item::Material::Enum MAT_PRI) const
    {
        return GetTotalArmorRatingAndFree(
            { ItemFactory::Make(Covers::Cloak, false, MAT_PRI),
              ItemFactory::Make(Covers::Vest, false, MAT_PRI),
              ItemFactory::Make(Armor::Shirt, Forms::Plain, false, MAT_PRI),
              ItemFactory::Make(Armor::Gauntlet, Forms::Plain, false, MAT_PRI),
              ItemFactory::Make(Armor::Pant, Forms::Plain, false, MAT_PRI),
              ItemFactory::Make(Armor::Boot, Forms::Plain, false, MAT_PRI) });
    }

    Armor_t ArmorRatings::LesserArmorSetRating(const item::Material::Enum MAT_PRI) const
    {
        const auto BASE_TYPE { ((MAT_PRI == Material::Leather) ? Forms::Plain : Forms::Mail) };

        return GetTotalArmorRatingAndFree(
            { ItemFactory::Make(
                  Shields::Buckler,
                  false,
                  ((MAT_PRI == Material::Leather) ? Material::Wood : MAT_PRI)),
              ItemFactory::Make(
                  ((MAT_PRI == Material::Leather) ? Helms::Leather : Helms::Kettle),
                  false,
                  MAT_PRI),
              ItemFactory::Make(Armor::Gauntlet, BASE_TYPE, false, MAT_PRI),
              ItemFactory::Make(Armor::Pant, BASE_TYPE, false, MAT_PRI),
              ItemFactory::Make(Armor::Boot, BASE_TYPE, false, MAT_PRI),
              ItemFactory::Make(Armor::Shirt, BASE_TYPE, false, MAT_PRI),
              ItemFactory::Make(Armor::Bracer, BASE_TYPE, false, MAT_PRI),
              ItemFactory::Make(Armor::Aventail, BASE_TYPE, false, MAT_PRI),
              ItemFactory::Make(Covers::Cloak, false, Material::Leather) });
    }

    Armor_t ArmorRatings::GreaterArmorSetRating(const item::Material::Enum MAT_PRI) const
    {
        const auto BASE_TYPE { ((MAT_PRI == Material::Leather) ? Forms::Plain : Forms::Plate) };

        return GetTotalArmorRatingAndFree(
            { ItemFactory::Make(
                  Shields::Pavis,
                  false,
                  ((MAT_PRI == Material::Leather) ? Material::Wood : MAT_PRI)),
              ItemFactory::Make(
                  ((MAT_PRI == Material::Leather) ? Helms::Leather : Helms::Great), false, MAT_PRI),
              ItemFactory::Make(Armor::Gauntlet, BASE_TYPE, false, MAT_PRI),
              ItemFactory::Make(Armor::Pant, BASE_TYPE, false, MAT_PRI),
              ItemFactory::Make(Armor::Boot, BASE_TYPE, false, MAT_PRI),
              ItemFactory::Make(Armor::Shirt, BASE_TYPE, false, MAT_PRI),
              ItemFactory::Make(Armor::Bracer, BASE_TYPE, false, MAT_PRI),
              ItemFactory::Make(Armor::Aventail, BASE_TYPE, false, MAT_PRI),
              ItemFactory::Make(Covers::Cloak, false, Material::Leather) });
    }

    Armor_t ArmorRatings::GetTotalArmorRatingAndFree(const ItemPVec_t & ITEM_PVEC_ORIG) const
    {
        auto itemPVec = ITEM_PVEC_ORIG;

        const auto TOTAL_ARMOR_RATING { std::accumulate(
            std::begin(itemPVec),
            std::end(itemPVec),
            0_armor,
            [](const auto SUBTOTAL, const auto ITEM_PTR) {
                return SUBTOTAL + ITEM_PTR->ArmorRating();
            }) };

        ItemWarehouse::Access().Free(itemPVec);
        return TOTAL_ARMOR_RATING;
    }

} // namespace item
} // namespace heroespath
