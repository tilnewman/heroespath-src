// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_ITEM_ARMORRATINGS_HPP_INCLUDED
#define HEROESPATH_ITEM_ARMORRATINGS_HPP_INCLUDED
//
// armor-ratings.hpp
//
#include "game/strong-types.hpp"
#include "item/material-enum.hpp"
#include "misc/not-null.hpp"

#include <memory>
#include <vector>

namespace heroespath
{
namespace item
{

    class Item;
    using ItemPtr_t = misc::NotNull<Item *>;
    using ItemPVec_t = std::vector<ItemPtr_t>;

    // Responsible for calculating an approximate armor rating for a creature that is wearing a
    // certain kind of common armor configuration.
    class ArmorRatings
    {
    public:
        ArmorRatings();
        ~ArmorRatings();

        ArmorRatings(const ArmorRatings &) = delete;
        ArmorRatings(ArmorRatings &&) = delete;
        ArmorRatings & operator=(const ArmorRatings &) = delete;
        ArmorRatings & operator=(ArmorRatings &&) = delete;

        static misc::NotNull<ArmorRatings *> Instance();
        static void Create();
        static void Destroy();

        void Initialize();

        Armor_t LesserSteel() const { return lesserSteel_; }
        Armor_t GreaterSteel() const { return greaterSteel_; }
        Armor_t GreaterDiamond() const { return greaterDiamond_; }

        void LogCommonArmorRatings() const;

    private:
        Armor_t ClothesSetRating(const item::Material::Enum) const;
        Armor_t LesserArmorSetRating(const item::Material::Enum) const;
        Armor_t GreaterArmorSetRating(const item::Material::Enum) const;
        Armor_t GetTotalArmorRatingAndFree(const ItemPVec_t &) const;

    private:
        static std::unique_ptr<ArmorRatings> instanceUPtr_;

        Armor_t lesserSteel_;
        Armor_t greaterSteel_;
        Armor_t greaterDiamond_;
    };

} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_ARMORRATINGS_HPP_INCLUDED
