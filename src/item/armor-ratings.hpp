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
#ifndef HEROESPATH_ITEM_ARMORRATINGS_HPP_INCLUDED
#define HEROESPATH_ITEM_ARMORRATINGS_HPP_INCLUDED
//
// armor-ratings.hpp
//
#include "item/item-type-enum.hpp"
#include "misc/not-null.hpp"
#include "misc/types.hpp"

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
        ArmorRatings(const ArmorRatings &) = delete;
        ArmorRatings(ArmorRatings &&) = delete;
        ArmorRatings & operator=(const ArmorRatings &) = delete;
        ArmorRatings & operator=(ArmorRatings &&) = delete;

        ArmorRatings();

        Armor_t LesserSteel() const { return lesserSteel_; }
        Armor_t GreaterSteel() const { return greaterSteel_; }
        Armor_t GreaterDiamond() const { return greaterDiamond_; }

        void LogCommonArmorRatings() const;

    private:
        Armor_t ClothesSetRating(const item::material::Enum) const;
        Armor_t LesserArmorSetRating(const item::material::Enum) const;
        Armor_t GreaterArmorSetRating(const item::material::Enum) const;
        Armor_t GetTotalArmorRatingAndFree(ItemPVec_t &) const;

        Armor_t lesserSteel_;
        Armor_t greaterSteel_;
        Armor_t greaterDiamond_;
    };

} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_ARMORRATINGS_HPP_INCLUDED
