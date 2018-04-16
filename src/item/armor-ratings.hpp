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

    class ArmorRatings
    {
    public:
        ArmorRatings(const ArmorRatings &) = delete;
        ArmorRatings(ArmorRatings &&) = delete;
        ArmorRatings & operator=(const ArmorRatings &) = delete;
        ArmorRatings & operator=(ArmorRatings &&) = delete;
        ArmorRatings() = delete;

        static void Setup();

        static Armor_t FullyClothedCloth() { return clothesCloth_; }
        static Armor_t FullyClothedSoftLeather() { return clothesSoftLeather_; }
        static Armor_t FullyClothedHardLeather() { return clothesHardLeather_; }
        static Armor_t ArmoredLesserSoftLeather() { return armoredLesserSoftLeather_; }
        static Armor_t ArmoredLesserSteel() { return armoredLesserSteel_; }
        static Armor_t ArmoredLesserDiamond() { return armoredLesserDiamond_; }
        static Armor_t ArmoredGreaterSoftLeather() { return armoredGreaterSoftLeather_; }
        static Armor_t ArmoredGreaterSteel() { return armoredGreaterSteel_; }
        static Armor_t ArmoredGreaterDiamond() { return armoredGreaterDiamond_; }

    private:
        static Armor_t ClothesSetRating(const item::material::Enum);
        static Armor_t LesserArmorSetRating(const item::material::Enum);
        static Armor_t GreaterArmorSetRating(const item::material::Enum);
        static Armor_t GetTotalArmorRatingAndFree(ItemPVec_t &);

        static Armor_t clothesCloth_;
        static Armor_t clothesSoftLeather_;
        static Armor_t clothesHardLeather_;
        static Armor_t armoredLesserSoftLeather_;
        static Armor_t armoredLesserSteel_;
        static Armor_t armoredLesserDiamond_;
        static Armor_t armoredGreaterSoftLeather_;
        static Armor_t armoredGreaterSteel_;
        static Armor_t armoredGreaterDiamond_;
    };

} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_ARMORRATINGS_HPP_INCLUDED
