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
#ifndef HEROESPATH_ITEM_ITEM_PROFILE_THIN_HPP_INCLUDE
#define HEROESPATH_ITEM_ITEM_PROFILE_THIN_HPP_INCLUDE
//
// item-profile-thin.hpp
//
#include "item/armor-type-wrapper.hpp"
#include "item/armor-types.hpp"
#include "item/item-type-enum.hpp"
#include "item/weapon-type-wrapper.hpp"

#include <boost/type_index.hpp>

#include <exception>
#include <sstream>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

namespace heroespath
{
namespace item
{
    struct ItemProfileThinFactory;

    class ItemProfileThin;
    using ItemProfileThinVec_t = std::vector<ItemProfileThin>;

    // Responsible for wrapping only the information about an item required serve as a placeholder
    // for a complete ItemProfile that will be created by ItemProfileWarehouse later.  Must be
    // constructed with either a valid WeaponTypeWrapper or valid ArmorTypeWrapper.  Only
    // weapon_type/specific_weapon_type/armor_type/specific_armor_type/armor_base_type_restriction/misc_type
    // are required to be set and valid for a thin item profile.  This makes sense because all items
    // are either weapons, armor, or misc_type.  This means that all other values can be ignored,
    // and set to whatever valid defaults are handy at the time.  Examples of these ignored fields
    // are the sfml_util::Size for weapons and the armor::base_type for armor.
    class ItemProfileThin
    {
        friend ItemProfileThinFactory;

        // valid instances can only be constructed with the static Make() functions below
    private:
        explicit ItemProfileThin(
            const weapon::WeaponTypeWrapper & WEAPON_TYPE_WRAPPER,
            const misc_type::Enum MISC_TYPE = misc_type::Not);

        explicit ItemProfileThin(
            const armor::ArmorTypeWrapper & ARMOR_TYPE_WRAPPER,
            const misc_type::Enum MISC_TYPE = misc_type::Not);

        explicit ItemProfileThin(const misc_type::Enum MISC_TYPE);

    public:
        // this default constructor leaves the object in an invalid state
        ItemProfileThin();

        const weapon::WeaponTypeWrapper & WeaponInfo() const { return weaponInfo_; }
        const armor::ArmorTypeWrapper & ArmorInfo() const { return armorInfo_; }

        misc_type::Enum MiscType() const { return miscType_; }

        bool IsMisc() const
        {
            return ((misc_type::Count != miscType_) && (misc_type::Not != miscType_));
        }

        bool IsWeapon() const { return weaponInfo_.IsTypeValid(); }

        bool IsArmor() const { return armorInfo_.IsTypeValid(); }

        const std::string ToString() const;

        const std::string ReadableName() const;
        const std::string SystemName() const;

        name_material_type::Enum NameMaterialType() const;

        friend bool operator==(const ItemProfileThin & L, const ItemProfileThin & R);
        friend bool operator<(const ItemProfileThin & L, const ItemProfileThin & R);

    private:
        weapon::WeaponTypeWrapper weaponInfo_;
        armor::ArmorTypeWrapper armorInfo_;
        misc_type::Enum miscType_;
    };

    inline bool operator==(const ItemProfileThin & L, const ItemProfileThin & R)
    {
        return std::tie(L.weaponInfo_, L.armorInfo_, L.miscType_)
            == std::tie(R.weaponInfo_, R.armorInfo_, R.miscType_);
    }

    inline bool operator!=(const ItemProfileThin & L, const ItemProfileThin & R)
    {
        return !(L == R);
    }

    inline bool operator<(const ItemProfileThin & L, const ItemProfileThin & R)
    {
        return std::tie(L.weaponInfo_, L.armorInfo_, L.miscType_)
            < std::tie(R.weaponInfo_, R.armorInfo_, R.miscType_);
    }

} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_ITEM_PROFILE_THIN_HPP_INCLUDE
