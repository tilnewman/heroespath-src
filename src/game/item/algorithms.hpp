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
#ifndef GAME_ITEM_ALGORITHMS_HPP_INCLUDED
#define GAME_ITEM_ALGORITHMS_HPP_INCLUDED
//
// algorithms.cpp
//  A set of functions for parsing vectors of ItemSPtr_ts in useful ways.
//  There are too many variables to code a complete set of functions, so instead,
//  functions will be added here as needed during development.
//
#include "game/item/item-type-enum.hpp"

#include <memory>
#include <vector>
#include <string>


namespace game
{
namespace item
{

    //forward declarations
    class Item;
    using ItemSPtr_t = std::shared_ptr<Item>;
    using ItemSVec_t = std::vector<ItemSPtr_t>;


    struct Algorithms
    {
        static const std::string Names(const ItemSVec_t &,
                                       const bool         WILL_WRAP       = false,
                                       const bool         WILL_APPEND_AND = false,
                                       const std::size_t  MAX_COUNT       = 0,
                                       const bool         WILL_ELLIPSIS   = false);

        static const ItemSVec_t FindByCategory(const ItemSVec_t &   itemSVec,
                                               const category::Enum CATEGORY_TYPE,
                                               const bool           IS_MATCH = true);

        static const ItemSVec_t FindByWeaponType(const ItemSVec_t &      itemSVec,
                                                 const weapon_type::Enum WEAPON_TYPE,
                                                 const bool              IS_MATCH = true);

        static const ItemSVec_t FindByArmorType(const ItemSVec_t &     itemSVec,
                                                const armor_type::Enum ARMOR_TYPE,
                                                const bool             IS_MATCH = true);

        static const ItemSVec_t FindByMiscType(const ItemSVec_t &    itemSVec,
                                               const misc_type::Enum MISC_TYPE,
                                               const bool            IS_MATCH = true);

        static const ItemSVec_t FindByPixie(const ItemSVec_t & itemSVec,
                                            const bool         IS_MATCH = true);

        static const ItemSVec_t FindByBroken(const ItemSVec_t & ITEM_SVEC,
                                             const bool         WILL_KEEP_BROKEN);
    };

}
}

#endif //GAME_ITEM_ALGORITHMS_HPP_INCLUDED
