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
#ifndef HEROESPATH_ITEM_ARMORINFO_HPP_INCLUDED
#define HEROESPATH_ITEM_ARMORINFO_HPP_INCLUDED
//
// armor-info.hpp
//  Code that completely describes a singl piece of armor.
//
#include "misc/boost-serialize-includes.hpp"

#include "item/item-type-enum.hpp"
#include "item/armor-types.hpp"

#include <tuple>
#include <string>


namespace heroespath
{
namespace item
{
namespace armor
{

    //A simple wrapper for all the info needed to describe one piece of armor
    struct ArmorInfo
    {
        explicit ArmorInfo(const armor_type::Enum TYPE = armor_type::NotArmor);

        //when used here in this class, only one bit is ever set.
        item::armor_type::Enum type;

        bool is_aventail;
        bool is_bracer;
        bool is_shirt;
        bool is_boots;
        bool is_pants;
        bool is_gauntlets;

        base_type::Enum   base;
        cover_type::Enum  cover;
        helm_type::Enum   helm;
        shield_type::Enum shield;

    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & type;
            ar & is_aventail;
            ar & is_bracer;
            ar & is_shirt;
            ar & is_boots;
            ar & is_pants;
            ar & is_gauntlets;
            ar & base;
            ar & cover;
            ar & helm;
            ar & shield;
        }
    };


    inline bool operator==(const ArmorInfo & L, const ArmorInfo & R)
    {
        return std::tie(L.type,
                        L.is_aventail,
                        L.is_bracer,
                        L.is_shirt,
                        L.is_boots,
                        L.is_pants,
                        L.is_gauntlets,
                        L.base,
                        L.cover,
                        L.helm,
                        L.shield)
            ==
               std::tie(R.type,
                        R.is_aventail,
                        R.is_bracer,
                        R.is_shirt,
                        R.is_boots,
                        R.is_pants,
                        R.is_gauntlets,
                        R.base,
                        R.cover,
                        R.helm,
                        R.shield);
    }


    inline bool operator!=(const ArmorInfo & L, const ArmorInfo & R)
    {
        return ! (L == R);
    }


    inline bool operator<(const ArmorInfo & L, const ArmorInfo & R)
    {
        return std::tie(L.type,
                        L.is_aventail,
                        L.is_bracer,
                        L.is_shirt,
                        L.is_boots,
                        L.is_pants,
                        L.is_gauntlets,
                        L.base,
                        L.cover,
                        L.helm,
                        L.shield)
            <
               std::tie(R.type,
                        R.is_aventail,
                        R.is_bracer,
                        R.is_shirt,
                        R.is_boots,
                        R.is_pants,
                        R.is_gauntlets,
                        R.base,
                        R.cover,
                        R.helm,
                        R.shield);
    }

}
}
}

#endif //HEROESPATH_ITEM_ARMORINFO_HPP_INCLUDED
