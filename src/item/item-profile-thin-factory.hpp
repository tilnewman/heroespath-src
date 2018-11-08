// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
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
            const misc_type::Enum MISC_TYPE = misc_type::Not) const;

        template <typename SpecificArmor_t>
        const ItemProfileThin MakeArmorSpecific(
            const SpecificArmor_t SPECIFIC_ARMOR_TYPE,
            const misc_type::Enum MISC_TYPE = misc_type::Not) const
        {
            return ItemProfileThin(armor::ArmorTypeWrapper(SPECIFIC_ARMOR_TYPE), MISC_TYPE);
        }

        template <typename SpecificArmorEnum_t>
        const ItemProfileThinVec_t MakeArmorSpecificAll() const
        {
            ItemProfileThinVec_t thinProfiles;
            thinProfiles.reserve(static_cast<std::size_t>(SpecificArmorEnum_t::Count));

            for (EnumUnderlying_t i(0); i < SpecificArmorEnum_t::Count; ++i)
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
            const misc_type::Enum MISC_TYPE = misc_type::Not) const
        {
            return ItemProfileThin(WEAPON_TYPE_WRAPPER, MISC_TYPE);
        }

        const ItemProfileThin MakeWeaponStaffOrQuarterstaff(
            const bool IS_QUARTERSTAFF, const misc_type::Enum MISC_TYPE = misc_type::Not) const
        {
            return ItemProfileThin(
                weapon::WeaponTypeWrapper(
                    weapon::WeaponTypeWrapper::StaffOrQuarterstaff(), IS_QUARTERSTAFF),
                MISC_TYPE);
        }

        template <typename SpecificWeapon_t>
        const ItemProfileThin MakeWeaponSpecific(
            const SpecificWeapon_t SPECIFIC_WEAPON_TYPE,
            const misc_type::Enum MISC_TYPE = misc_type::Not) const
        {
            return MakeWeapon(weapon::WeaponTypeWrapper(SPECIFIC_WEAPON_TYPE), MISC_TYPE);
        }

        const ItemProfileThinVec_t MakeWeaponOfTypeAll(const weapon_type::Enum WEAPON_TYPE) const;

        template <typename SpecificWeaponEnum_t>
        const ItemProfileThinVec_t MakeWeaponSpecificAll() const
        {
            ItemProfileThinVec_t thinProfiles;
            thinProfiles.reserve(static_cast<std::size_t>(SpecificWeaponEnum_t::Count));

            for (EnumUnderlying_t i(0); i < SpecificWeaponEnum_t::Count; ++i)
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
