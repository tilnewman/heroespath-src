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
#ifndef HEROESPATH_ITEM_ALGORITHMS_HPP_INCLUDED
#define HEROESPATH_ITEM_ALGORITHMS_HPP_INCLUDED
//
// algorithms.cpp
//  A set of functions for parsing vectors of ItemPPtr_ts in useful ways.
//  There are too many variables to code a complete set of functions, so instead,
//  functions will be added here as needed during development.
//
#include "item/item-type-enum.hpp"

#include "misc/vectors.hpp"

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace item
{

    // forward declarations
    class Item;
    using ItemPtr_t = Item *;
    using ItemPVec_t = std::vector<ItemPtr_t>;

    struct Algorithms
    {
        static const std::string Names(
            const ItemPVec_t &,
            const std::size_t MAX_COUNT = 0,
            const misc::Vector::JoinOpt OPTIONS = misc::Vector::JoinOpt::None);

        enum class MatchOpt
        {
            Equal,
            NotEqual
        };

        static const ItemPVec_t FindByCategory(
            const ItemPVec_t & itemSVec,
            const category::Enum CATEGORY_TYPE,
            const MatchOpt MATCH_OPTION = MatchOpt::Equal);

        static const ItemPVec_t FindByWeaponType(
            const ItemPVec_t & itemSVec,
            const weapon_type::Enum WEAPON_TYPE,
            const MatchOpt MATCH_OPTION = MatchOpt::Equal);

        static const ItemPVec_t FindByArmorType(
            const ItemPVec_t & itemSVec,
            const armor_type::Enum ARMOR_TYPE,
            const MatchOpt MATCH_OPTION = MatchOpt::Equal);

        static const ItemPVec_t FindByMiscType(
            const ItemPVec_t & itemSVec,
            const misc_type::Enum MISC_TYPE,
            const MatchOpt MATCH_OPTION = MatchOpt::Equal);

        static const ItemPVec_t
            FindByPixie(const ItemPVec_t & itemSVec, const MatchOpt MATCH_OPTION = MatchOpt::Equal);

        enum class BrokenOpt
        {
            Keep,
            Discard
        };

        static const ItemPVec_t
            FindByBroken(const ItemPVec_t & ITEM_SVEC, const BrokenOpt KEEP_BROKEN_OPTION);
    };
}
}

#endif // HEROESPATH_ITEM_ALGORITHMS_HPP_INCLUDED
