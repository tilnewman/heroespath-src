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
#include "sfml-util/size-enum.hpp"

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
        // valid instances can only be constructed with the static Make() functions below
    private:
        explicit ItemProfileThin(
            const weapon::WeaponTypeWrapper & WEAPON_TYPE_WRAPPER,
            const misc_type::Enum MISC_TYPE = misc_type::NotMisc);

        explicit ItemProfileThin(
            const armor::ArmorTypeWrapper & ARMOR_TYPE_WRAPPER,
            const misc_type::Enum MISC_TYPE = misc_type::NotMisc);

        explicit ItemProfileThin(const misc_type::Enum MISC_TYPE);

    public:
        // this default constructor leaves the object in an invalid state
        ItemProfileThin();

        const weapon::WeaponTypeWrapper & WeaponInfo() const { return weaponInfo_; }
        const armor::ArmorTypeWrapper & ArmorInfo() const { return armorInfo_; }

        misc_type::Enum MiscType() const { return miscType_; }

        bool IsMisc() const
        {
            return ((misc_type::Count != miscType_) && (misc_type::NotMisc != miscType_));
        }

        bool IsWeapon() const { return weaponInfo_.IsTypeValid(); }

        bool IsArmor() const { return armorInfo_.IsTypeValid(); }

        const std::string ToString() const;

        const std::string ReadableName() const;
        const std::string SystemName() const;

        friend bool operator==(const ItemProfileThin & L, const ItemProfileThin & R);
        friend bool operator<(const ItemProfileThin & L, const ItemProfileThin & R);

        static const ItemProfileThin MakeArmorNonSpecific(
            const armor_type::Enum ARMOR_TYPE,
            const armor::base_type::Enum BASE_TYPE = armor::base_type::Count,
            const misc_type::Enum MISC_TYPE = misc_type::NotMisc);

        template <typename SpecificArmor_t>
        static const ItemProfileThin MakeArmorSpecific(
            const SpecificArmor_t SPECIFIC_ARMOR_TYPE,
            const misc_type::Enum MISC_TYPE = misc_type::NotMisc)
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (std::is_same<SpecificArmor_t, armor::shield_type::Enum>::value
                 || std::is_same<SpecificArmor_t, armor::helm_type::Enum>::value
                 || std::is_same<SpecificArmor_t, armor::cover_type::Enum>::value),
                "item::ItemProfileThin::MakeArmorSpecificAll<"
                    << boost::typeindex::type_id<SpecificArmor_t>().pretty_name()
                    << ">() but that type is not one of the specific armor types. "
                       "(shield/helm/cover)");

            return ItemProfileThin(armor::ArmorTypeWrapper(SPECIFIC_ARMOR_TYPE), MISC_TYPE);
        }

        template <typename SpecificArmorEnum_t>
        static const ItemProfileThinVec_t MakeArmorSpecificAll()
        {
            ItemProfileThinVec_t thinProfiles;
            thinProfiles.reserve(static_cast<std::size_t>(SpecificArmorEnum_t::Count));

            for (int i(0); i < SpecificArmorEnum_t::Count; ++i)
            {
                thinProfiles.emplace_back(MakeArmorSpecific<typename SpecificArmorEnum_t::Enum>(
                    static_cast<typename SpecificArmorEnum_t::Enum>(i)));
            }

            return thinProfiles;
        }

        static const ItemProfileThinVec_t MakeArmorNonSpecificAll(const armor_type::Enum ARMOR_TYPE)
        {
            ItemProfileThinVec_t thinProfiles;

            for (int i(0); i < armor::base_type::Count; ++i)
            {
                auto const BASE_TYPE{ static_cast<armor::base_type::Enum>(i) };
                thinProfiles.emplace_back(MakeArmorNonSpecific(ARMOR_TYPE, BASE_TYPE));
            }

            return thinProfiles;
        }

        static const ItemProfileThinVec_t
            MakeArmorNonSpecificAll(const armor::base_type::Enum BASE_TYPE)
        {
            ItemProfileThinVec_t thinProfiles;
            thinProfiles.emplace_back(MakeArmorNonSpecific(armor_type::Aventail, BASE_TYPE));
            thinProfiles.emplace_back(MakeArmorNonSpecific(armor_type::Boots, BASE_TYPE));
            thinProfiles.emplace_back(MakeArmorNonSpecific(armor_type::Bracers, BASE_TYPE));
            thinProfiles.emplace_back(MakeArmorNonSpecific(armor_type::Gauntlets, BASE_TYPE));
            thinProfiles.emplace_back(MakeArmorNonSpecific(armor_type::Pants, BASE_TYPE));
            thinProfiles.emplace_back(MakeArmorNonSpecific(armor_type::Shirt, BASE_TYPE));
            return thinProfiles;
        }

        static const ItemProfileThinVec_t MakeArmorNonSpecificAll()
        {
            ItemProfileThinVec_t thinProfiles;

            for (int i(0); i < armor::base_type::Count; ++i)
            {
                auto const BASE_TYPE{ static_cast<armor::base_type::Enum>(i) };

                for (auto const & THIN_PROFILE : MakeArmorNonSpecificAll(BASE_TYPE))
                {
                    thinProfiles.emplace_back(THIN_PROFILE);
                }
            }

            return thinProfiles;
        }

        static const ItemProfileThin MakeWeaponStaffOrQuarterstaff(
            const bool IS_QUARTERSTAFF, const misc_type::Enum MISC_TYPE = misc_type::NotMisc);

        template <typename SpecificWeapon_t>
        static const ItemProfileThin MakeWeaponSpecific(
            const SpecificWeapon_t SPECIFIC_WEAPON_TYPE,
            const misc_type::Enum MISC_TYPE = misc_type::NotMisc)
        {
            return MakeWeapon(weapon::WeaponTypeWrapper(SPECIFIC_WEAPON_TYPE), MISC_TYPE);
        }

        static const ItemProfileThinVec_t MakeWeaponOfTypeAll(const weapon_type::Enum WEAPON_TYPE)
        {
            ItemProfileThinVec_t thinProfiles;
            for (auto const & WEAPON_TYPE_WRAPPER : weapon::WeaponTypeWrapper::MakeCompleteSet())
            {
                // skip body parts
                if ((WEAPON_TYPE_WRAPPER.IsBodyPart() == false)
                    && (WEAPON_TYPE_WRAPPER.Type() & WEAPON_TYPE))
                {
                    thinProfiles.emplace_back(ItemProfileThin::MakeWeapon(WEAPON_TYPE_WRAPPER));
                }
            }
            return thinProfiles;
        }

        template <typename SpecificWeaponEnum_t>
        static const ItemProfileThinVec_t MakeWeaponSpecificAll()
        {
            ItemProfileThinVec_t thinProfiles;
            thinProfiles.reserve(static_cast<std::size_t>(SpecificWeaponEnum_t::Count));

            for (int i(0); i < SpecificWeaponEnum_t::Count; ++i)
            {
                thinProfiles.emplace_back(MakeWeaponSpecific<typename SpecificWeaponEnum_t::Enum>(
                    static_cast<typename SpecificWeaponEnum_t::Enum>(i)));
            }

            return thinProfiles;
        }

        static const ItemProfileThin MakeWeaponKnifeOrDagger(
            const bool IS_DAGGER, const sfml_util::Size::Enum SIZE = sfml_util::Size::Medium);

        static const ItemProfileThinVec_t MakeWeaponKnifeOrDaggerAll(const bool IS_DAGGER);

        static const ItemProfileThin MakeMisc(const misc_type::Enum MISC_TYPE);

    private:
        static const ItemProfileThin MakeWeapon(
            const weapon::WeaponTypeWrapper & WEAPON_TYPE_WRAPPER,
            const misc_type::Enum MISC_TYPE = misc_type::NotMisc);

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
