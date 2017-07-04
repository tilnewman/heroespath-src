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
#ifndef GAME_ITEM_ARMORRATINGS_HPP_INCLUDED
#define GAME_ITEM_ARMORRATINGS_HPP_INCLUDED
//
// armor-ratings.hpp
//
#include "game/stats/types.hpp"
#include "game/item/item-type-enum.hpp"

#include <memory>
#include <vector>


namespace game
{
namespace item
{

    class Item;
    using ItemPtr_t = Item *;
    using ItemPVec_t = std::vector<ItemPtr_t>;


    class ArmorRatings
    {
        //prevent copy construction
        ArmorRatings(const ArmorRatings &) = delete;

        //prevent copy assignment
        ArmorRatings & operator=(const ArmorRatings &) = delete;

        //prevent non-singleton construction
        ArmorRatings();

    public:
        ~ArmorRatings();

        static ArmorRatings * Instance();
        static void Acquire();
        static void Release();

        void Setup();


        inline stats::Armor_t FullyClothedCloth() const
        {
            return clothesCloth_;
        }

        inline stats::Armor_t FullyClothedSoftLeather() const
        {
            return clothesSoftLeather_;
        }

        inline stats::Armor_t FullyClothedHardLeather() const
        {
            return clothesHardLeather_;
        }

        inline stats::Armor_t ArmoredLesserSoftLeather() const
        {
            return armoredLesserSoftLeather_;
        }

        inline stats::Armor_t ArmoredLesserSteel() const
        {
            return armoredLesserSteel_;
        }

        inline stats::Armor_t ArmoredLesserDiamond() const
        {
            return armoredLesserDiamond_;
        }

        inline stats::Armor_t ArmoredGreaterSoftLeather() const
        {
            return armoredGreaterSoftLeather_;
        }

        inline stats::Armor_t ArmoredGreaterSteel() const
        {
            return armoredGreaterSteel_;
        }

        inline stats::Armor_t ArmoredGreaterDiamond() const
        {
            return armoredGreaterDiamond_;
        }

    private:

        stats::Armor_t ClothesSetRating(const item::material::Enum) const;
        stats::Armor_t LesserArmorSetRating(const item::material::Enum) const;
        stats::Armor_t GreaterArmorSetRating(const item::material::Enum) const;
        stats::Armor_t GetTotalArmorRating(const ItemPVec_t &) const;

    private:
        static std::unique_ptr<ArmorRatings> instanceUPtr_;
        //
        stats::Armor_t clothesCloth_;
        stats::Armor_t clothesSoftLeather_;
        stats::Armor_t clothesHardLeather_;
        stats::Armor_t armoredLesserSoftLeather_;
        stats::Armor_t armoredLesserSteel_;
        stats::Armor_t armoredLesserDiamond_;
        stats::Armor_t armoredGreaterSoftLeather_;
        stats::Armor_t armoredGreaterSteel_;
        stats::Armor_t armoredGreaterDiamond_;
    };

}
}


#endif //GAME_ITEM_ARMORRATINGS_HPP_INCLUDED
