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
#ifndef HEROESPATH_ITEM_ITEMPROFILE_HPP_INCLUDE
#define HEROESPATH_ITEM_ITEMPROFILE_HPP_INCLUDE
//
// item-profile.hpp
//
#include "creature/race-enum.hpp"
#include "item/armor-types.hpp"
#include "item/item-type-enum.hpp"
#include "item/weapon-types.hpp"
#include "log/log-macros.hpp"

#include "sfml-util/size-enum.hpp"

#include "misc/handy-types.hpp"
#include "misc/types.hpp"

#include <string>
#include <tuple>
#include <vector>

namespace heroespath
{
namespace item
{

    struct MemberStrings
    {
        MemberStrings(
            const misc::StrVec_t & NORMAL_STRINGS,
            const misc::StrVec_t & WEAPON_STRINGS,
            const misc::StrVec_t & ARMOR_STRINGS)
            : normalStrings(NORMAL_STRINGS)
            , weaponStrings(WEAPON_STRINGS)
            , armorStrings(ARMOR_STRINGS)
        {}

        const std::string InvalidString() const;

        bool IsValid() const { return InvalidString().empty(); }

        const std::string ToString() const;

    private:
        misc::StrVec_t normalStrings;
        misc::StrVec_t weaponStrings;
        misc::StrVec_t armorStrings;
    };

    namespace profile
    {
        struct ArmorSettings
        {
            ArmorSettings()
                : shield(armor::shield_type::Count)
                , helm(armor::helm_type::Count)
                , base(armor::base_type::Count)
                , cover(armor::cover_type::Count)
                , restriction(armor::base_type::Count)
            {}

            armor::shield_type::Enum shield;
            armor::helm_type::Enum helm;
            armor::base_type::Enum base;
            armor::cover_type::Enum cover;
            armor::base_type::Enum restriction;
        };

        inline bool operator==(const ArmorSettings & L, const ArmorSettings & R)
        {
            return std::tie(L.shield, L.helm, L.base, L.cover, L.restriction)
                == std::tie(R.shield, R.helm, R.base, R.cover, R.restriction);
        }

        inline bool operator!=(const ArmorSettings & L, const ArmorSettings & R)
        {
            return !(L == R);
        }

        inline bool operator<(const ArmorSettings & L, const ArmorSettings & R)
        {
            return std::tie(L.shield, L.helm, L.base, L.cover, L.restriction)
                < std::tie(R.shield, R.helm, R.base, R.cover, R.restriction);
        }

        struct WeaponSettings
        {
            WeaponSettings()
                : sword(weapon::sword_type::Count)
                , axe(weapon::axe_type::Count)
                , club(weapon::club_type::Count)
                , whip(weapon::whip_type::Count)
                , proj(weapon::projectile_type::Count)
                , bstaff(weapon::bladedstaff_type::Count)
            {}

            weapon::sword_type::Enum sword;
            weapon::axe_type::Enum axe;
            weapon::club_type::Enum club;
            weapon::whip_type::Enum whip;
            weapon::projectile_type::Enum proj;
            weapon::bladedstaff_type::Enum bstaff;
        };

        inline bool operator==(const WeaponSettings & L, const WeaponSettings & R)
        {
            return std::tie(L.sword, L.axe, L.club, L.whip, L.proj, L.bstaff)
                == std::tie(R.sword, R.axe, R.club, R.whip, R.proj, R.bstaff);
        }

        inline bool operator!=(const WeaponSettings & L, const WeaponSettings & R)
        {
            return !(L == R);
        }

        inline bool operator<(const WeaponSettings & L, const WeaponSettings & R)
        {
            return std::tie(L.sword, L.axe, L.club, L.whip, L.proj, L.bstaff)
                < std::tie(R.sword, R.axe, R.club, R.whip, R.proj, R.bstaff);
        }

        struct IsSet
        {
            enum Enum : unsigned
            {
                None = 0,
                Pixie = 1 << 0,
                Aventail = 1 << 1,
                Bracer = 1 << 2,
                Shirt = 1 << 3,
                Boots = 1 << 4,
                Pants = 1 << 5,
                Gauntlets = 1 << 6,
                Knife = 1 << 7,
                Dagger = 1 << 8,
                Staff = 1 << 9,
                QStaff = 1 << 10,
            };
        };
    }

    // A collection of details about an item that uniquely idenify an item.
    // A "Thin" Profile is an incomplete profile used as a placeholder for a
    //"Fat" Profile.  A "Fat" Profile is a complete profile that is used as
    // a placeholder for an actual Item.  See ItemProfileWarehouse.  That is
    // the only place where thin profiles are used, and where the complete vec
    // of Fat Profiles are permanently stored as placeholders for actual items.
    class ItemProfile
    {
    public:
        // use this constructor with the Set() functions for creating specific ItemProfiles
        ItemProfile();

        // use this constructor to create a unique profile for an item
        ItemProfile(
            const std::string & BASE_NAME,
            const category::Enum CATEGORY,
            const armor_type::Enum ARMOR,
            const weapon_type::Enum WEAPON,
            const unique_type::Enum UNIQUE,
            const misc_type::Enum MISC,
            const set_type::Enum SET,
            const named_type::Enum NAMED);

        const std::string BaseName() const { return baseName_; }
        category::Enum Category() const { return category_; }
        armor_type::Enum ArmorType() const { return armor_; }
        weapon_type::Enum WeaponType() const { return weapon_; }
        unique_type::Enum UniqueType() const { return unique_; }
        misc_type::Enum MiscType() const { return misc_; }
        set_type::Enum SetType() const { return set_; }
        named_type::Enum NamedType() const { return named_; }
        element_type::Enum ElementType() const { return element_; }

        armor::shield_type::Enum ShieldType() const
        {
            return (
                (category_ & category::Armor) ? settings_.armor.shield : armor::shield_type::Count);
        }
        armor::helm_type::Enum HelmType() const
        {
            return ((category_ & category::Armor) ? settings_.armor.helm : armor::helm_type::Count);
        }
        armor::base_type::Enum BaseType() const
        {
            return ((category_ & category::Armor) ? settings_.armor.base : armor::base_type::Count);
        }
        armor::cover_type::Enum CoverType() const
        {
            return (
                (category_ & category::Armor) ? settings_.armor.cover : armor::cover_type::Count);
        }

        bool IsAventail() const { return (isSet_ & profile::IsSet::Aventail); }
        bool IsBracer() const { return (isSet_ & profile::IsSet::Bracer); }
        bool IsShirt() const { return (isSet_ & profile::IsSet::Shirt); }
        bool IsBoots() const { return (isSet_ & profile::IsSet::Boots); }
        bool IsPants() const { return (isSet_ & profile::IsSet::Pants); }
        bool IsGauntlets() const { return (isSet_ & profile::IsSet::Gauntlets); }
        bool IsKnife() const { return (isSet_ & profile::IsSet::Knife); }
        bool IsDagger() const { return (isSet_ & profile::IsSet::Dagger); }
        bool IsStaff() const { return (isSet_ & profile::IsSet::Staff); }
        bool IsQuarterStaff() const { return (isSet_ & profile::IsSet::QStaff); }
        bool IsPixie() const { return (isSet_ & profile::IsSet::Pixie); }

        weapon::sword_type::Enum SwordType() const
        {
            return (
                (category_ & category::Weapon) ? settings_.weapon.sword
                                               : weapon::sword_type::Count);
        }
        weapon::axe_type::Enum AxeType() const
        {
            return (
                (category_ & category::Weapon) ? settings_.weapon.axe : weapon::axe_type::Count);
        }
        weapon::club_type::Enum ClubType() const
        {
            return (
                (category_ & category::Weapon) ? settings_.weapon.club : weapon::club_type::Count);
        }
        weapon::whip_type::Enum WhipType() const
        {
            return (
                (category_ & category::Weapon) ? settings_.weapon.whip : weapon::whip_type::Count);
        }
        weapon::projectile_type::Enum ProjectileType() const
        {
            return (
                (category_ & category::Weapon) ? settings_.weapon.proj
                                               : weapon::projectile_type::Count);
        }
        weapon::bladedstaff_type::Enum BladedStaffType() const
        {
            return (
                (category_ & category::Weapon) ? settings_.weapon.bstaff
                                               : weapon::bladedstaff_type::Count);
        }

        sfml_util::Size::Enum Size() const { return size_; }

        material::Enum MaterialPrimary() const { return matPri_; }
        material::Enum MaterialSecondary() const { return matSec_; }

        creature::role::Enum Role() const { return role_; }

        Score_t TreasureScore() const { return score_; }

        Score_t ReligiousScore() const
        {
            return Score_t(static_cast<Score_t::type>(score_.As<float>() * religious_));
        }

        bool IsReligious() const { return (religious_ > 0.0f); }

        const creature::SummonInfo Summoning() const { return summonInfo_; }

        bool WillSummon() const { return summonInfo_.WillSummon(); }

        void SetSummoningAndAdjustScore(const creature::SummonInfo &);

        float Religious() const { return religious_; }
        void Religious(const float F) { religious_ = F; }

        bool IsWeapon() const { return (weapon_ != weapon_type::NotAWeapon); }

        bool IsArmor() const { return (armor_ != armor_type::NotArmor); }

        bool IsEquipment() const { return (IsWeapon() || IsArmor()); }

        bool IsMisc() const
        {
            return ((misc_ != misc_type::Count) && (misc_ != misc_type::NotMisc));
        }

        bool IsUnique() const
        {
            return ((unique_ != unique_type::Count) && (unique_ != unique_type::NotUnique));
        }

        bool IsSet() const { return ((set_ != set_type::Count) && (set_ != set_type::NotASet)); }

        bool IsNamed() const
        {
            return ((named_ != named_type::Count) && (named_ != named_type::NotNamed));
        }

        bool IsElemental() const { return (element_ != element_type::None); }

        bool IsStandard() const
        {
            return (
                (IsUnique() == false) && (IsSet() == false) && (IsNamed() == false)
                && (IsElemental() == false));
        }

        bool IsStandardEquipment() const { return (IsStandard() && IsEquipment()); }

        const MemberStrings ToMemberStrings() const;

        const std::string ToString() const { return ToMemberStrings().ToString(); }

        bool IsValid() const { return ToMemberStrings().IsValid(); }

        void SetUnique(
            const unique_type::Enum,
            const material::Enum MATERIAL_PRIMARY = material::Nothing,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const element_type::Enum ELEMENT_TYPE = element_type::None);

        void SetMisc(
            const misc_type::Enum,
            const bool IS_PIXIE = false,
            const material::Enum MATERIAL_PRIMARY = material::Nothing,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const set_type::Enum SET_TYPE = set_type::NotASet);

        void SetShield(
            const armor::shield_type::Enum,
            const material::Enum MATERIAL_PRIMARY = material::Nothing,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const named_type::Enum NAMED_TYPE = named_type::NotNamed,
            const set_type::Enum SET_TYPE = set_type::NotASet,
            const element_type::Enum ELEMENT_TYPE = element_type::None);

        void SetHelm(
            const armor::helm_type::Enum,
            const material::Enum MATERIAL_PRIMARY = material::Nothing,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const named_type::Enum NAMED_TYPE = named_type::NotNamed,
            const set_type::Enum SET_TYPE = set_type::NotASet,
            const element_type::Enum ELEMENT_TYPE = element_type::None);

        void SetCover(
            const armor::cover_type::Enum,
            const material::Enum MATERIAL_PRIMARY = material::Nothing,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const named_type::Enum NAMED_TYPE = named_type::NotNamed,
            const set_type::Enum SET_TYPE = set_type::NotASet,
            const element_type::Enum ELEMENT_TYPE = element_type::None,
            const bool IS_PIXIE = false);

        void SetAventail(
            const armor::base_type::Enum BASE_TYPE = armor::base_type::Plain,
            const material::Enum MATERIAL_PRIMARY = material::Nothing,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const named_type::Enum NAMED_TYPE = named_type::NotNamed,
            const set_type::Enum SET_TYPE = set_type::NotASet,
            const element_type::Enum ELEMENT_TYPE = element_type::None);

        void SetBracer(
            const armor::base_type::Enum BASE_TYPE = armor::base_type::Plain,
            const material::Enum MATERIAL_PRIMARY = material::Nothing,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const named_type::Enum NAMED_TYPE = named_type::NotNamed,
            const set_type::Enum SET_TYPE = set_type::NotASet,
            const element_type::Enum ELEMENT_TYPE = element_type::None,
            const bool IS_PIXIE = false);

        void SetShirt(
            const armor::base_type::Enum BASE_TYPE = armor::base_type::Plain,
            const material::Enum MATERIAL_PRIMARY = material::Nothing,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const named_type::Enum NAMED_TYPE = named_type::NotNamed,
            const set_type::Enum SET_TYPE = set_type::NotASet,
            const element_type::Enum ELEMENT_TYPE = element_type::None,
            const bool IS_PIXIE = false);

        void SetBoots(
            const armor::base_type::Enum BASE_TYPE = armor::base_type::Plain,
            const material::Enum MATERIAL_PRIMARY = material::Nothing,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const named_type::Enum NAMED_TYPE = named_type::NotNamed,
            const set_type::Enum SET_TYPE = set_type::NotASet,
            const element_type::Enum ELEMENT_TYPE = element_type::None,
            const bool IS_PIXIE = false);

        void SetPants(
            const armor::base_type::Enum BASE_TYPE = armor::base_type::Plain,
            const material::Enum MATERIAL_PRIMARY = material::Nothing,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const named_type::Enum NAMED_TYPE = named_type::NotNamed,
            const set_type::Enum SET_TYPE = set_type::NotASet,
            const element_type::Enum ELEMENT_TYPE = element_type::None,
            const bool IS_PIXIE = false);

        void SetGauntlets(
            const armor::base_type::Enum BASE_TYPE = armor::base_type::Plain,
            const material::Enum MATERIAL_PRIMARY = material::Nothing,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const named_type::Enum NAMED_TYPE = named_type::NotNamed,
            const set_type::Enum SET_TYPE = set_type::NotASet,
            const element_type::Enum ELEMENT_TYPE = element_type::None,
            const bool IS_PIXIE = false);

        void SetSword(
            const weapon::sword_type::Enum,
            const material::Enum MATERIAL_PRIMARY = material::Nothing,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const named_type::Enum NAMED_TYPE = named_type::NotNamed,
            const set_type::Enum SET_TYPE = set_type::NotASet,
            const element_type::Enum ELEMENT_TYPE = element_type::None);

        void SetAxe(
            const weapon::axe_type::Enum,
            const material::Enum MATERIAL_PRIMARY = material::Nothing,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const named_type::Enum NAMED_TYPE = named_type::NotNamed,
            const set_type::Enum SET_TYPE = set_type::NotASet,
            const element_type::Enum ELEMENT_TYPE = element_type::None);

        void SetClub(
            const weapon::club_type::Enum,
            const material::Enum MATERIAL_PRIMARY = material::Nothing,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const named_type::Enum NAMED_TYPE = named_type::NotNamed,
            const set_type::Enum SET_TYPE = set_type::NotASet,
            const element_type::Enum ELEMENT_TYPE = element_type::None);

        void SetWhip(
            const weapon::whip_type::Enum,
            const material::Enum MATERIAL_PRIMARY = material::Nothing,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const named_type::Enum NAMED_TYPE = named_type::NotNamed,
            const set_type::Enum SET_TYPE = set_type::NotASet,
            const element_type::Enum ELEMENT_TYPE = element_type::None);

        void SetProjectile(
            const weapon::projectile_type::Enum,
            const material::Enum MATERIAL_PRIMARY = material::Nothing,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const named_type::Enum NAMED_TYPE = named_type::NotNamed,
            const set_type::Enum SET_TYPE = set_type::NotASet,
            const element_type::Enum ELEMENT_TYPE = element_type::None);

        void SetBladedStaff(
            const weapon::bladedstaff_type::Enum,
            const material::Enum MATERIAL_PRIMARY = material::Nothing,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const named_type::Enum NAMED_TYPE = named_type::NotNamed,
            const set_type::Enum SET_TYPE = set_type::NotASet,
            const element_type::Enum ELEMENT_TYPE = element_type::None);

        void SetKnife(
            const sfml_util::Size::Enum SIZE = sfml_util::Size::Medium,
            const material::Enum MATERIAL_PRIMARY = material::Nothing,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const named_type::Enum NAMED_TYPE = named_type::NotNamed,
            const set_type::Enum SET_TYPE = set_type::NotASet,
            const element_type::Enum ELEMENT_TYPE = element_type::None,
            const bool IS_PIXIE = false);

        void SetDagger(
            const sfml_util::Size::Enum SIZE = sfml_util::Size::Medium,
            const material::Enum MATERIAL_PRIMARY = material::Nothing,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const named_type::Enum NAMED_TYPE = named_type::NotNamed,
            const set_type::Enum SET_TYPE = set_type::NotASet,
            const element_type::Enum ELEMENT_TYPE = element_type::None,
            const bool IS_PIXIE = false);

        void SetStaff(
            const material::Enum MATERIAL_PRIMARY = material::Nothing,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const named_type::Enum NAMED_TYPE = named_type::NotNamed,
            const set_type::Enum SET_TYPE = set_type::NotASet,
            const element_type::Enum ELEMENT_TYPE = element_type::None);

        void SetQuarterStaff(
            const material::Enum MATERIAL_PRIMARY = material::Nothing,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const named_type::Enum NAMED_TYPE = named_type::NotNamed,
            const set_type::Enum SET_TYPE = set_type::NotASet,
            const element_type::Enum ELEMENT_TYPE = element_type::None);

        armor::base_type::Enum ArmorTypeRestriction() const { return settings_.armor.restriction; }

        void SetArmorTypeRestriction(const armor::base_type::Enum E)
        {
            settings_.armor.restriction = E;
        }

        void Role(const creature::role::Enum E) { role_ = E; }

        friend bool operator==(const ItemProfile & L, const ItemProfile & R);
        friend bool operator<(const ItemProfile & L, const ItemProfile & R);

    private:
        void SetFlag(const bool WILL_SET, const profile::IsSet::Enum WHICH_BIT)
        {
            if (WILL_SET)
            {
                isSet_ = static_cast<profile::IsSet::Enum>(isSet_ | WHICH_BIT);
            }
            else
            {
                isSet_ = static_cast<profile::IsSet::Enum>(isSet_ & ~WHICH_BIT);
            }
        }

    private:
        void SetHelper(
            const material::Enum,
            const material::Enum,
            const named_type::Enum,
            const set_type::Enum,
            const element_type::Enum,
            const bool IS_PIXIE = false);

        Score_t ScoreHelper(
            const item::material::Enum MATERIAL_PRI,
            const item::material::Enum MATERIAL_SEC,
            const item::named_type::Enum NAMED_TYPE,
            const item::set_type::Enum SET_TYPE,
            const item::element_type::Enum ELEMENT_TYPE,
            const bool IS_WEAPON) const;

        void EquippableHelper(const int SCORE_BONUS);

    private:
        Score_t score_;

        // this is a ratio of how religious the item is.
        float religious_;

        category::Enum category_;
        armor_type::Enum armor_;
        weapon_type::Enum weapon_;
        unique_type::Enum unique_;
        misc_type::Enum misc_;
        set_type::Enum set_;
        named_type::Enum named_;
        element_type::Enum element_;

        profile::IsSet::Enum isSet_;

        union Settings
        {
            Settings()
                : weapon()
            {}

            profile::WeaponSettings weapon;
            profile::ArmorSettings armor;
        } settings_;

        sfml_util::Size::Enum size_;

        material::Enum matPri_;
        material::Enum matSec_;

        creature::role::Enum role_;

        creature::SummonInfo summonInfo_;

        std::string baseName_;
    };

    using ItemProfileVec_t = std::vector<ItemProfile>;

    inline bool operator==(const ItemProfile & L, const ItemProfile & R)
    {
        if (L.score_ != R.score_)
        {
            return false;
        }

        return std::tie(
                   L.category_,
                   L.armor_,
                   L.weapon_,
                   L.unique_,
                   L.misc_,
                   L.set_,
                   L.named_,
                   L.element_,
                   L.isSet_,
                   L.settings_.weapon,
                   L.size_,
                   L.matPri_,
                   L.matSec_,
                   L.role_,
                   L.baseName_,
                   L.summonInfo_,
                   L.religious_)
            == std::tie(
                   R.category_,
                   R.armor_,
                   R.weapon_,
                   R.unique_,
                   R.misc_,
                   R.set_,
                   R.named_,
                   R.element_,
                   R.isSet_,
                   R.settings_.weapon,
                   R.size_,
                   R.matPri_,
                   R.matSec_,
                   R.role_,
                   R.baseName_,
                   R.summonInfo_,
                   R.religious_);
    }

    inline bool operator!=(const ItemProfile & L, const ItemProfile & R) { return !(L == R); }

    inline bool operator<(const ItemProfile & L, const ItemProfile & R)
    {
        if (L.score_ != R.score_)
        {
            return L.score_ < R.score_;
        }

        return std::tie(
                   L.category_,
                   L.armor_,
                   L.weapon_,
                   L.unique_,
                   L.misc_,
                   L.set_,
                   L.named_,
                   L.element_,
                   L.isSet_,
                   L.settings_.weapon,
                   L.size_,
                   L.matPri_,
                   L.matSec_,
                   L.role_,
                   L.baseName_,
                   L.summonInfo_,
                   L.religious_)
            < std::tie(
                   R.category_,
                   R.armor_,
                   R.weapon_,
                   R.unique_,
                   R.misc_,
                   R.set_,
                   R.named_,
                   R.element_,
                   R.isSet_,
                   R.settings_.weapon,
                   R.size_,
                   R.matPri_,
                   R.matSec_,
                   R.role_,
                   R.baseName_,
                   R.summonInfo_,
                   R.religious_);
    }

} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_ITEMPROFILE_HPP_INCLUDE
