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
#ifndef GAME_ITEM_ITEMPROFILE_HPP_INCLUDE
#define GAME_ITEM_ITEMPROFILE_HPP_INCLUDE
//
// item-profile.hpp
//
#include "game/item/item-type-enum.hpp"
#include "game/item/armor-types.hpp"
#include "game/item/weapon-types.hpp"

#include <tuple>
#include <string>
#include <vector>


namespace game
{
namespace item
{

    //A collection of details about an item that uniquely idenify an item.
    class ItemProfile
    {
        ItemProfile() =delete;

    public:
        //use this constructor to create a unique profile for an item
        ItemProfile(const std::string &     BASE_NAME,
                    const category::Enum    CATEGORY,
                    const armor_type::Enum  ARMOR,
                    const weapon_type::Enum WEAPON,
                    const unique_type::Enum UNIQUE,
                    const misc_type::Enum   MISC,
                    const set_type::Enum    SET,
                    const named_type::Enum  NAMED);

        //these constructors are used for specific items
        
        friend bool operator==(const ItemProfile & L, const ItemProfile & R);
        friend bool operator<(const ItemProfile & L, const ItemProfile & R);

    private:
        std::string       baseName_;
        category::Enum    category_;
        armor_type::Enum  armor_;
        weapon_type::Enum weapon_;
        unique_type::Enum unique_;
        misc_type::Enum   misc_;
        set_type::Enum    set_;
        named_type::Enum  named_;
    };


    using ItemProfileVec_t = std::vector<ItemProfile>;

    
    inline bool operator==(const ItemProfile & L, const ItemProfile & R)
    {
        return std::tie(L.baseName_,
                        L.category_,
                        L.armor_,
                        L.weapon_,
                        L.unique_,
                        L.misc_,
                        L.set_,
                        L.named_)
                ==
               std::tie(R.baseName_,
                        R.category_,
                        R.armor_,
                        R.weapon_,
                        R.unique_,
                        R.misc_,
                        R.set_,
                        R.named_);
    }


    inline bool operator!=(const ItemProfile & L, const ItemProfile & R)
    {
        return ! (L == R);
    }
    
    
    inline bool operator<(const ItemProfile & L, const ItemProfile & R)
    {
        return std::tie(L.baseName_,
                        L.category_,
                        L.armor_,
                        L.weapon_,
                        L.unique_,
                        L.misc_,
                        L.set_,
                        L.named_)
                <
               std::tie(R.baseName_,
                        R.category_,
                        R.armor_,
                        R.weapon_,
                        R.unique_,
                        R.misc_,
                        R.set_,
                        R.named_);
    }

}
}

#endif //GAME_ITEM_ITEMPROFILE_HPP_INCLUDE
