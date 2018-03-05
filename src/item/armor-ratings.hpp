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
#include "misc/types.hpp"

#include <memory>
#include <vector>

namespace heroespath
{
namespace item
{

    class Item;
    using ItemPtr_t = Item *;
    using ItemPVec_t = std::vector<ItemPtr_t>;

    class ArmorRatings
    {
        ArmorRatings(const ArmorRatings &) = delete;
        ArmorRatings(const ArmorRatings &&) = delete;
        ArmorRatings & operator=(const ArmorRatings &) = delete;
        ArmorRatings & operator=(const ArmorRatings &&) = delete;

    public:
        ArmorRatings();
        ~ArmorRatings();

        static ArmorRatings * Instance();
        static void Acquire();
        static void Release();

        void Setup();

        inline Armor_t FullyClothedCloth() const { return clothesCloth_; }

        inline Armor_t FullyClothedSoftLeather() const { return clothesSoftLeather_; }

        inline Armor_t FullyClothedHardLeather() const { return clothesHardLeather_; }

        inline Armor_t ArmoredLesserSoftLeather() const { return armoredLesserSoftLeather_; }

        inline Armor_t ArmoredLesserSteel() const { return armoredLesserSteel_; }

        inline Armor_t ArmoredLesserDiamond() const { return armoredLesserDiamond_; }

        inline Armor_t ArmoredGreaterSoftLeather() const { return armoredGreaterSoftLeather_; }

        inline Armor_t ArmoredGreaterSteel() const { return armoredGreaterSteel_; }

        inline Armor_t ArmoredGreaterDiamond() const { return armoredGreaterDiamond_; }

    private:
        Armor_t ClothesSetRating(const item::material::Enum) const;
        Armor_t LesserArmorSetRating(const item::material::Enum) const;
        Armor_t GreaterArmorSetRating(const item::material::Enum) const;
        Armor_t GetTotalArmorRating(const ItemPVec_t &) const;

    private:
        static std::unique_ptr<ArmorRatings> instanceUPtr_;
        //
        Armor_t clothesCloth_;
        Armor_t clothesSoftLeather_;
        Armor_t clothesHardLeather_;
        Armor_t armoredLesserSoftLeather_;
        Armor_t armoredLesserSteel_;
        Armor_t armoredLesserDiamond_;
        Armor_t armoredGreaterSoftLeather_;
        Armor_t armoredGreaterSteel_;
        Armor_t armoredGreaterDiamond_;
    };
} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_ARMORRATINGS_HPP_INCLUDED
