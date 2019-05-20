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
        ItemProfileThinFactory() = delete;

        static ItemProfileThinVec_t MakeAllWeapons();
        static ItemProfileThinVec_t MakeAllArmor();
        static ItemProfileThinVec_t MakeAllNamedTypes(const named_type::Enum);
        static ItemProfileThinVec_t MakeAllSetTypes(const set_type::Enum);

        static const ItemProfileThin MakeMisc(const misc_type::Enum MISC_TYPE)
        {
            return ItemProfileThin(MISC_TYPE);
        }

        //

        static const ItemProfileThin MakeArmorNonSpecific(
            const armor_type::Enum ARMOR_TYPE,
            const armor::base_type::Enum BASE_TYPE = armor::base_type::Count,
            const misc_type::Enum MISC_TYPE = misc_type::Not);

        template <typename SpecificArmor_t>
        static const ItemProfileThin MakeArmorSpecific(
            const SpecificArmor_t SPECIFIC_ARMOR_TYPE,
            const misc_type::Enum MISC_TYPE = misc_type::Not)
        {
            return ItemProfileThin(armor::ArmorTypeWrapper(SPECIFIC_ARMOR_TYPE), MISC_TYPE);
        }

        //

        static const ItemProfileThin MakeWeaponStaffOrQuarterstaff(
            const bool IS_QUARTERSTAFF, const misc_type::Enum MISC_TYPE = misc_type::Not)
        {
            return ItemProfileThin(
                weapon::WeaponTypeWrapper(
                    weapon::WeaponTypeWrapper::StaffOrQuarterstaff(), IS_QUARTERSTAFF),
                MISC_TYPE);
        }

        template <typename SpecificWeapon_t>
        static const ItemProfileThin MakeWeaponSpecific(
            const SpecificWeapon_t SPECIFIC_WEAPON_TYPE,
            const misc_type::Enum MISC_TYPE = misc_type::Not)
        {
            return MakeWeapon(weapon::WeaponTypeWrapper(SPECIFIC_WEAPON_TYPE), MISC_TYPE);
        }

        static const ItemProfileThin MakeWeaponKnifeOrDagger(const bool IS_DAGGER)
        {
            return ItemProfileThin(
                weapon::WeaponTypeWrapper(weapon::WeaponTypeWrapper::KnifeOrDagger(), IS_DAGGER));
        }

    private:
        template <typename SpecificArmorEnum_t>
        static ItemProfileThinVec_t MakeArmorSpecificAll()
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

        static ItemProfileThinVec_t MakeArmorNonSpecificAll(const armor_type::Enum ARMOR_TYPE);
        static ItemProfileThinVec_t MakeArmorNonSpecificAll(const armor::base_type::Enum BASE_TYPE);
        static ItemProfileThinVec_t MakeArmorNonSpecificAll();

        static const ItemProfileThin MakeWeapon(
            const weapon::WeaponTypeWrapper & WEAPON_TYPE_WRAPPER,
            const misc_type::Enum MISC_TYPE = misc_type::Not)
        {
            return ItemProfileThin(WEAPON_TYPE_WRAPPER, MISC_TYPE);
        }

        template <typename SpecificWeaponEnum_t>
        static ItemProfileThinVec_t MakeWeaponSpecificAll()
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

        static ItemProfileThinVec_t MakeWeaponOfTypeAll(const weapon_type::Enum WEAPON_TYPE);
    };

} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_ITEM_PROFILE_THIN_FACTORY_HPP_INCLUDED
