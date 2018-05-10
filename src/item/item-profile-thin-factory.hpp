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
#ifndef HEROESPATH_ITEM_ITEM_PROFILE_THIN_FACTORY_HPP_INCLUDED
#define HEROESPATH_ITEM_ITEM_PROFILE_THIN_FACTORY_HPP_INCLUDED
//
// item-profile-thin-factory.hpp
//
#include "item/item-profile-thin.hpp"

namespace heroespath
{
namespace item
{

    // Responsible for creating thin profiles of items.
    // This struct follows the Machine pattern.
    struct ItemProfileThinFactory
    {
        const ItemProfileThinVec_t MakeAllWeapons() const;
        const ItemProfileThinVec_t MakeAllArmor() const;
        const ItemProfileThinVec_t MakeAllNamedTypes(const named_type::Enum) const;
        const ItemProfileThinVec_t MakeAllSetTypes(const set_type::Enum) const;

        const ItemProfileThin MakeMisc(const misc_type::Enum MISC_TYPE) const
        {
            return ItemProfileThin(MISC_TYPE);
        }

        const ItemProfileThin MakeArmorNonSpecific(
            const armor_type::Enum ARMOR_TYPE,
            const armor::base_type::Enum BASE_TYPE = armor::base_type::Count,
            const misc_type::Enum MISC_TYPE = misc_type::NotMisc) const;

        template <typename SpecificArmor_t>
        const ItemProfileThin MakeArmorSpecific(
            const SpecificArmor_t SPECIFIC_ARMOR_TYPE,
            const misc_type::Enum MISC_TYPE = misc_type::NotMisc) const
        {
            return ItemProfileThin(armor::ArmorTypeWrapper(SPECIFIC_ARMOR_TYPE), MISC_TYPE);
        }

        template <typename SpecificArmorEnum_t>
        const ItemProfileThinVec_t MakeArmorSpecificAll() const
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

        const ItemProfileThinVec_t MakeArmorNonSpecificAll(const armor_type::Enum ARMOR_TYPE) const;

        const ItemProfileThinVec_t
            MakeArmorNonSpecificAll(const armor::base_type::Enum BASE_TYPE) const;

        const ItemProfileThinVec_t MakeArmorNonSpecificAll() const;

        const ItemProfileThin MakeWeapon(
            const weapon::WeaponTypeWrapper & WEAPON_TYPE_WRAPPER,
            const misc_type::Enum MISC_TYPE = misc_type::NotMisc) const
        {
            return ItemProfileThin(WEAPON_TYPE_WRAPPER, MISC_TYPE);
        }

        const ItemProfileThin MakeWeaponStaffOrQuarterstaff(
            const bool IS_QUARTERSTAFF, const misc_type::Enum MISC_TYPE = misc_type::NotMisc) const
        {
            return ItemProfileThin(
                weapon::WeaponTypeWrapper(
                    weapon::WeaponTypeWrapper::StaffOrQuarterstaff(), IS_QUARTERSTAFF),
                MISC_TYPE);
        }

        template <typename SpecificWeapon_t>
        const ItemProfileThin MakeWeaponSpecific(
            const SpecificWeapon_t SPECIFIC_WEAPON_TYPE,
            const misc_type::Enum MISC_TYPE = misc_type::NotMisc) const
        {
            return MakeWeapon(weapon::WeaponTypeWrapper(SPECIFIC_WEAPON_TYPE), MISC_TYPE);
        }

        const ItemProfileThinVec_t MakeWeaponOfTypeAll(const weapon_type::Enum WEAPON_TYPE) const;

        template <typename SpecificWeaponEnum_t>
        const ItemProfileThinVec_t MakeWeaponSpecificAll() const
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

        const ItemProfileThin MakeWeaponKnifeOrDagger(const bool IS_DAGGER) const
        {
            return ItemProfileThin(
                weapon::WeaponTypeWrapper(weapon::WeaponTypeWrapper::KnifeOrDagger(), IS_DAGGER));
        }

        const ItemProfileThinVec_t MakeWeaponKnifeOrDaggerAll(const bool IS_DAGGER) const;
    };

} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_ITEM_PROFILE_THIN_FACTORY_HPP_INCLUDED
