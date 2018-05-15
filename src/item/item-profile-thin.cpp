// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
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
        , miscType_(MISC_TYPE)
    {}

    ItemProfileThin::ItemProfileThin(
        const armor::ArmorTypeWrapper & ARMOR_TYPE_WRAPPER, const misc_type::Enum MISC_TYPE)
        : weaponInfo_()
        , armorInfo_(ARMOR_TYPE_WRAPPER)
        , miscType_(MISC_TYPE)
    {}

    ItemProfileThin::ItemProfileThin(const misc_type::Enum MISC_TYPE)
        : weaponInfo_()
        , armorInfo_()
        , miscType_(MISC_TYPE)
    {}

    ItemProfileThin::ItemProfileThin()
        : weaponInfo_()
        , armorInfo_()
        , miscType_(misc_type::Not)
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

    name_material_type::Enum ItemProfileThin::NameMaterialType() const
    {
        if (IsWeapon())
        {
            return weaponInfo_.NameMaterialType();
        }
        else if (IsArmor())
        {
            return armorInfo_.NameMaterialType();
        }
        else if (IsMisc())
        {
            return name_material_type::Misc;
        }
        else
        {
            // should never get here if valid
            return name_material_type::Count;
        }
    }

} // namespace item
} // namespace heroespath
