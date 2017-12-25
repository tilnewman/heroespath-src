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
#ifndef HEROESPATH_ITEM_WEAPONFINFO_HPP_INCLUDED
#define HEROESPATH_ITEM_WEAPONFINFO_HPP_INCLUDED
//
// weapons-info.hpp
//  Describes a specific or general weapon.
//
#include "sfml-util/size-enum.hpp"
#include "misc/boost-serialize-includes.hpp"

#include "item/weapon-types.hpp"
#include "item/item-type-enum.hpp"

#include <tuple>
#include <string>
#include <vector>


namespace heroespath
{
namespace item
{
namespace weapon
{

    //wrapper for all the information needed to identify a single weapon
    struct WeaponInfo
    {
        explicit WeaponInfo(const weapon_type::Enum TYPE = weapon_type::NotAWeapon,
                            const std::string &     NAME = "");

        void ProcessName();

        inline bool IsAWeapon() const { return (type != weapon_type::NotAWeapon); }

        std::string name;

        //When used here in this class, never more than one bit is set.
        //In the case of a dagger, this is set to Knife because there is no Dagger weapon_type.
        item::weapon_type::Enum type;

        //ensure only one of these is true
        bool is_bite;
        bool is_claws;
        bool is_fists;
        bool is_staff;
        bool is_knife;
        bool is_dagger;
        bool is_tendrils;
        bool is_quarterstaff;
        bool is_breath;

        //If is_knife or is_dagger and this member is 'Count', then the weapon is 'general'.
        sfml_util::Size::Enum knife_size;

        //ensure only one of these is not Count
        sword_type::Enum       sword;
        axe_type::Enum         axe;
        club_type::Enum        club;
        whip_type::Enum        whip;
        projectile_type::Enum  projectile;
        bladedstaff_type::Enum bladedstaff;

    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & name;
            ar & type;
            ar & is_bite;
            ar & is_claws;
            ar & is_fists;
            ar & is_staff;
            ar & is_knife;
            ar & is_dagger;
            ar & is_tendrils;
            ar & is_quarterstaff;
            ar & is_breath;
            ar & knife_size;
            ar & sword;
            ar & axe;
            ar & club;
            ar & whip;
            ar & projectile;
            ar & bladedstaff;
        }
    };


    inline bool operator==(const WeaponInfo & L, const WeaponInfo & R)
    {
        return std::tie(L.name,
                        L.type,
                        L.is_bite,
                        L.is_claws,
                        L.is_dagger,
                        L.is_fists,
                        L.is_knife,
                        L.is_quarterstaff,
                        L.is_breath,
                        L.is_staff,
                        L.is_tendrils,
                        L.knife_size,
                        L.sword,
                        L.axe,
                        L.club,
                        L.whip,
                        L.projectile,
                        L.bladedstaff)
            ==
            std::tie(R.name,
                     R.type,
                     R.is_bite,
                     R.is_claws,
                     R.is_dagger,
                     R.is_fists,
                     R.is_knife,
                     R.is_quarterstaff,
                     R.is_breath,
                     R.is_staff,
                     R.is_tendrils,
                     R.knife_size,
                     R.sword,
                     R.axe,
                     R.club,
                     R.whip,
                     R.projectile,
                     R.bladedstaff);
    }

    inline bool operator!=(const WeaponInfo & L, const WeaponInfo & R)
    {
        return ! (L == R);
    }

    inline bool operator<(const WeaponInfo & L, const WeaponInfo & R)
    {
        return std::tie(L.name,
                        L.type,
                        L.is_bite,
                        L.is_claws,
                        L.is_dagger,
                        L.is_fists,
                        L.is_knife,
                        L.is_quarterstaff,
                        L.is_breath,
                        L.is_staff,
                        L.is_tendrils,
                        L.knife_size,
                        L.sword,
                        L.axe,
                        L.club,
                        L.whip,
                        L.projectile,
                        L.bladedstaff)
            <
            std::tie(R.name,
                     R.type,
                     R.is_bite,
                     R.is_claws,
                     R.is_dagger,
                     R.is_fists,
                     R.is_knife,
                     R.is_quarterstaff,
                     R.is_breath,
                     R.is_staff,
                     R.is_tendrils,
                     R.knife_size,
                     R.sword,
                     R.axe,
                     R.club,
                     R.whip,
                     R.projectile,
                     R.bladedstaff);
    }

    using WeaponInfoVec_t = std::vector<WeaponInfo>;
    using WeaponInfoVecVec_t = std::vector<WeaponInfoVec_t>;

}
}
}

#endif //HEROESPATH_ITEM_WEAPONFINFO_HPP_INCLUDED