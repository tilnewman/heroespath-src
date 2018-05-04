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

        if (weapon::WeaponTypeWrapper() != weaponInfo_)
        {
            ss << "weapon_info=" << weaponInfo_.ToString() << ", ";
        }

        if (armor::ArmorTypeWrapper() != armorInfo_)
        {
            ss << "armor_info=" << armorInfo_.ToString();
        }

        ss << ", misc_type="
           << ((misc_type::Count == miscType_) ? "Count" : misc_type::ToString(miscType_));

        if (armor::base_type::Count != armorBaseTypeRestriction_)
        {
            ss << ", armor_base_type_restriction="
               << armor::base_type::ToString(armorBaseTypeRestriction_);
        }

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

    const ItemProfileThin ItemProfileThin::MakeArmorNonSpecific(
        const armor_type::Enum ARMOR_TYPE,
        const armor::base_type::Enum BASE_TYPE,
        const misc_type::Enum MISC_TYPE)
    {
        M_ASSERT_OR_LOGANDTHROW_SS(
            ((ARMOR_TYPE != armor_type::Shield) && (ARMOR_TYPE != armor_type::Covering)
             && (ARMOR_TYPE != armor_type::Helm)),
            "item::ItemProfileThin::MakeArmorNonSpecific(armor_type="
                << armor_type::ToString(ARMOR_TYPE) << ", base_type="
                << ((BASE_TYPE == armor::base_type::Count) ? "Count"
                                                           : armor::base_type::ToString(BASE_TYPE))
                << ", misc_type="
                << ((MISC_TYPE == misc_type::Count) ? "Count" : misc_type::ToString(MISC_TYPE))
                << ") but the armor_type given IS a SPECIFIC type.");

        return ItemProfileThin(
            armor::ArmorTypeWrapper(ARMOR_TYPE, BASE_TYPE), armor::base_type::Count, MISC_TYPE);
    }

    const ItemProfileThin ItemProfileThin::MakeArmorNonSpecificWithBaseTypeRestriction(
        const armor_type::Enum ARMOR_TYPE,
        const armor::base_type::Enum BASE_TYPE_RESTRICTION,
        const misc_type::Enum MISC_TYPE)
    {
        M_ASSERT_OR_LOGANDTHROW_SS(
            ((ARMOR_TYPE != armor_type::Shield) && (ARMOR_TYPE != armor_type::Covering)
             && (ARMOR_TYPE != armor_type::Helm)),
            "item::ItemProfileThin::MakeArmorNonSpecificWithBaseTypeRestriction(armor_type="
                << armor_type::ToString(ARMOR_TYPE) << ", base_type_restriction="
                << ((BASE_TYPE_RESTRICTION == armor::base_type::Count)
                        ? "Count"
                        : armor::base_type::ToString(BASE_TYPE_RESTRICTION))
                << ", misc_type="
                << ((MISC_TYPE == misc_type::Count) ? "Count" : misc_type::ToString(MISC_TYPE))
                << ") but the armor_type given IS a SPECIFIC type.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (BASE_TYPE_RESTRICTION != armor::base_type::Count),
            "item::ItemProfileThin::MakeArmorNonSpecificWithBaseTypeRestriction("
            "armor_type="
                << armor_type::ToString(ARMOR_TYPE)
                << ", base_type_restriction=Count) but the base_type_restriction given was Count "
                   "(invalid).");

        return ItemProfileThin(
            armor::ArmorTypeWrapper(ARMOR_TYPE, BASE_TYPE_RESTRICTION),
            BASE_TYPE_RESTRICTION,
            MISC_TYPE);
    }

    const ItemProfileThin ItemProfileThin::MakeArmorNonSpecificInvalidWithoutBaseType(
        const armor_type::Enum ARMOR_TYPE,
        const armor::base_type::Enum BASE_TYPE_RESTRICTION,
        const misc_type::Enum MISC_TYPE)
    {
        M_ASSERT_OR_LOGANDTHROW_SS(
            ((ARMOR_TYPE != armor_type::Shield) && (ARMOR_TYPE != armor_type::Covering)
             && (ARMOR_TYPE != armor_type::Helm)),
            "item::ItemProfileThin::MakeArmorNonSpecificInvalidWithoutBaseType(armor_type="
                << armor_type::ToString(ARMOR_TYPE) << ", base_type_restriction="
                << ((BASE_TYPE_RESTRICTION == armor::base_type::Count)
                        ? "Count"
                        : armor::base_type::ToString(BASE_TYPE_RESTRICTION))
                << ", misc_type="
                << ((MISC_TYPE == misc_type::Count) ? "Count" : misc_type::ToString(MISC_TYPE))
                << ") but the armor_type given IS a SPECIFIC type.");

        return ItemProfileThin(
            armor::ArmorTypeWrapper(ARMOR_TYPE, armor::base_type::Count, true),
            BASE_TYPE_RESTRICTION,
            MISC_TYPE);
    }

    const ItemProfileThin
        ItemProfileThin::MakeArmorNonSpecificFromThinProfileThatWasMissingABaseType(
            const ItemProfileThin & THIN_PROFILE, const armor::base_type::Enum BASE_TYPE)
    {
        auto const ARMOR_TYPE{ THIN_PROFILE.ArmorInfo().Type() };

        M_ASSERT_OR_LOGANDTHROW_SS(
            ((ARMOR_TYPE != armor_type::Shield) && (ARMOR_TYPE != armor_type::Covering)
             && (ARMOR_TYPE != armor_type::Helm)),
            "item::ItemProfileThin::MakeArmorNonSpecificFromThinProfileThatWasMissingABaseType("
            "armor_type="
                << armor_type::ToString(ARMOR_TYPE) << ", base_type="
                << ((BASE_TYPE == armor::base_type::Count) ? "Count"
                                                           : armor::base_type::ToString(BASE_TYPE))
                << ") but the armor_type given IS a SPECIFIC type.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            ((BASE_TYPE != armor::base_type::Count) && (ARMOR_TYPE != armor_type::Helm)),
            "item::ItemProfileThin::MakeArmorNonSpecificFromThinProfileThatWasMissingABaseType("
            "armor_type="
                << armor_type::ToString(ARMOR_TYPE)
                << ", base_type=Count) but the base_type given was Count (invalid).");

        return ItemProfileThin(
            armor::ArmorTypeWrapper(ARMOR_TYPE, BASE_TYPE),
            THIN_PROFILE.ArmorBaseTypeRestriction(),
            THIN_PROFILE.MiscType());
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
