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
#ifndef HEROESPATH_ITEM_ITEMFACTORYBASE_HPP_INCLUDED
#define HEROESPATH_ITEM_ITEMFACTORYBASE_HPP_INCLUDED
//
// item-factory-base.hpp
//  Code that is common to all item factory classes.
//
#include "item/item-type-enum.hpp"

#include <memory>
#include <string>

namespace heroespath
{
namespace item
{

    // A base class for all item factories.
    class FactoryBase
    {
    public:
        FactoryBase(const FactoryBase &) = delete;
        FactoryBase(const FactoryBase &&) = delete;
        FactoryBase & operator=(const FactoryBase &) = delete;
        FactoryBase & operator=(const FactoryBase &&) = delete;

        FactoryBase() = default;
        virtual ~FactoryBase() = default;

    protected:
        static const std::string Make_Name(
            const std::string & BASE_NAME,
            const material::Enum MATERIAL_PRI,
            const material::Enum MATERIAL_SEC,
            const bool IS_PIXIE_ITEM = false);

        static const std::string Make_Desc(
            const std::string & DESC,
            const material::Enum MATERIAL_PRI,
            const material::Enum MATERIAL_SEC,
            const std::string & EXTRA_NAME = "",
            const bool IS_PIXIE_ITEM = false);

        static const std::string Make_Desc_Clasped(
            const std::string & DESC,
            const material::Enum MATERIAL_PRI,
            const material::Enum MATERIAL_SEC,
            const bool IS_PIXIE_ITEM = false);

        static const std::string Make_Desc_BladdedStaff(
            const std::string & BASE_NAME,
            const bool IS_SPEAR,
            const material::Enum MATERIAL_PRI,
            const material::Enum MATERIAL_SEC);

        static void AdjustPrice(
            Coin_t & price,
            const material::Enum MATERIAL_PRI,
            const material::Enum MATERIAL_SEC,
            const bool IS_PIXIE_ITEM = false);

        static void AdjustWeight(
            Weight_t & weight,
            const material::Enum MATERIAL_PRI,
            const material::Enum MATERIAL_SEC);

        static void AdjustArmorRating(
            Armor_t & armorRating,
            const material::Enum MATERIAL_PRI,
            const material::Enum MATERIAL_SEC);

        static Coin_t TreasureScoreToCoins(const Score_t &);

        static const std::string RandomCoatedPhrase();

        static const std::string RandomCoatedAdjective();

        static const std::string RandomJeweledAdjective();

        static const std::string RandomAdornedAdjective();

        static const std::string RandomChainNoun();

        static const std::string RandomClaspNoun();
    };
} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_ITEMFACTORYBASE_HPP_INCLUDED
