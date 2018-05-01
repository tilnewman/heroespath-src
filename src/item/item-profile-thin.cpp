// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
//
// item-profile-thin.cpp
//
#include "item-profile-thin.hpp"

#include <sstream>

namespace heroespath
{
namespace item
{

    ItemProfileThin::ItemProfileThin(
        const weapon::WeaponTypeWrapper & WEAPON_TYPE_WRAPPER, const misc_type::Enum MISC_TYPE)
        : weaponInfo_(WEAPON_TYPE_WRAPPER)
        , armorInfo_()
        , armorBaseTypeRestriction_(armor::base_type::Count)
        , miscType_(MISC_TYPE)
    {}

    ItemProfileThin::ItemProfileThin(
        const armor::ArmorTypeWrapper & ARMOR_TYPE_WRAPPER,
        const armor::base_type::Enum BASE_TYPE_RESTRICTION,
        const misc_type::Enum MISC_TYPE)
        : weaponInfo_()
        , armorInfo_(ARMOR_TYPE_WRAPPER)
        , armorBaseTypeRestriction_(BASE_TYPE_RESTRICTION)
        , miscType_(MISC_TYPE)
    {}

    ItemProfileThin::ItemProfileThin(const misc_type::Enum MISC_TYPE)
        : weaponInfo_()
        , armorInfo_()
        , armorBaseTypeRestriction_(armor::base_type::Count)
        , miscType_(MISC_TYPE)
    {}

    ItemProfileThin::ItemProfileThin()
        : weaponInfo_()
        , armorInfo_()
        , armorBaseTypeRestriction_(armor::base_type::Count)
        , miscType_(misc_type::NotMisc)
    {}

    const std::string ItemProfileThin::ToString() const
    {
        std::ostringstream ss;
        ss << "weapon_info=" << weaponInfo_.ToString() << ", armor_info=" << armorInfo_.ToString()
           << ", misc_type="
           << ((misc_type::Count == miscType_) ? "Count" : misc_type::ToString(miscType_))
           << ", armor_base_type_restriction="
           << ((armor::base_type::Count == armorBaseTypeRestriction_)
                   ? "Count"
                   : armor::base_type::ToString(armorBaseTypeRestriction_));

        return ss.str();
    }

    const std::string ItemProfileThin::ReadableName() const
    {
        if (IsMisc())
        {
            return misc_type::Name(MiscType());
        }
        else if (IsWeapon())
        {
            return weaponInfo_.ReadableName();
        }
        else if (IsArmor())
        {
            return armorInfo_.ReadableName();
        }
        else
        {
            return "";
        }
    }

    const std::string ItemProfileThin::SystemName() const
    {
        if (IsMisc())
        {
            return misc_type::ToString(MiscType());
        }
        else if (IsWeapon())
        {
            return weaponInfo_.SystemName();
        }
        else if (IsArmor())
        {
            return armorInfo_.SystemName();
        }
        else
        {
            return "";
        }
    }

    const ItemProfileThin ItemProfileThin::MakeArmor(
        const armor_type::Enum ARMOR_TYPE,
        const armor::base_type::Enum BASE_TYPE_RESTRICTION,
        const misc_type::Enum MISC_TYPE)
    {
        // thin profiles ignore the armor::base_type unless it is invalid, so force it to be
        // valid here if needed

        auto const ARMOR_TYPE_WRAPPER{ armor::ArmorTypeWrapper(
            ARMOR_TYPE,
            ((BASE_TYPE_RESTRICTION == armor::base_type::Count) ? armor::base_type::Plain
                                                                : BASE_TYPE_RESTRICTION)) };

        return ItemProfileThin(ARMOR_TYPE_WRAPPER, BASE_TYPE_RESTRICTION, MISC_TYPE);
    }

    const ItemProfileThin ItemProfileThin::MakeWeapon(
        const weapon::WeaponTypeWrapper & WEAPON_TYPE_WRAPPER, const misc_type::Enum MISC_TYPE)
    {
        return ItemProfileThin(WEAPON_TYPE_WRAPPER, MISC_TYPE);
    }

    const ItemProfileThin ItemProfileThin::MakeWeaponStaffOrQuarterstaff(
        const bool IS_QUARTERSTAFF, const misc_type::Enum MISC_TYPE)
    {
        return ItemProfileThin(weapon::WeaponTypeWrapper(IS_QUARTERSTAFF), MISC_TYPE);
    }

    const ItemProfileThin ItemProfileThin::MakeWeaponKnifeOrDagger(
        const bool IS_DAGGER, const sfml_util::Size::Enum SIZE)
    {
        return ItemProfileThin(weapon::WeaponTypeWrapper(IS_DAGGER, SIZE));
    }

    const ItemProfileThinVec_t ItemProfileThin::MakeWeaponKnifeOrDaggerAll(const bool IS_DAGGER)
    {
        ItemProfileThinVec_t thinProfiles;
        thinProfiles.reserve(static_cast<std::size_t>(sfml_util::Size::Count));

        for (int i(0); i < sfml_util::Size::Count; ++i)
        {
            thinProfiles.emplace_back(
                MakeWeaponKnifeOrDagger(IS_DAGGER, static_cast<sfml_util::Size::Enum>(i)));
        }

        return thinProfiles;
    }

    const ItemProfileThin ItemProfileThin::MakeMisc(const misc_type::Enum MISC_TYPE)
    {
        return ItemProfileThin(MISC_TYPE);
    }

} // namespace item
} // namespace heroespath
