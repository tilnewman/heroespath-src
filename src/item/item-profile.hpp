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
#include "creature/summon-info.hpp"
#include "item/armor-details.hpp"
#include "item/item-profile-thin.hpp"
#include "item/item-score-helper.hpp"
#include "item/item-type-enum.hpp"
#include "item/weapon-details.hpp"
#include "item/weapon-types.hpp"
#include "log/log-macros.hpp"
#include "misc/handy-types.hpp"
#include "misc/types.hpp"

#include <string>
#include <tuple>
#include <vector>

namespace heroespath
{
namespace item
{

    // Responsible for wrapping information required to create an Item object.  An ItemProfile is
    // also responsible for storing the treasure score for the Item object it represents.  All
    // ItemProfiles include an ItemProfileThin member.
    //
    // To make weapon and armor items, the ItemFactory will need more than just one of these
    // ItemProfiles, it will also need to load details from the GameDataFile, such as min/max
    // damage, price, description, etc.  Actual Item instances also have a collection of Enchantment
    // objects, which ItemProfiles do not, although they do store treasure score information
    // regarding these enchantments.
    //
    class ItemProfile
    {
    public:
        ItemProfile();

        const std::string ReadableName() const { return thinProfile_.ReadableName(); }
        const std::string SystemName() const { return thinProfile_.SystemName(); }

        creature::role::Enum RoleRestriction() const;

        category::Enum Category() const { return category_; }
        armor_type::Enum ArmorType() const { return thinProfile_.ArmorInfo().Type(); }
        weapon_type::Enum WeaponType() const { return thinProfile_.WeaponInfo().Type(); }
        unique_type::Enum UniqueType() const { return unique_; }
        misc_type::Enum MiscType() const { return thinProfile_.MiscType(); }
        set_type::Enum SetType() const { return set_; }
        named_type::Enum NamedType() const { return named_; }
        element_type::Enum ElementType() const { return element_; }

        bool IsPixie() const { return isPixie_; }

        bool IsQuestItem() const { return misc_type::IsQuestItem(thinProfile_.MiscType()); }

        sfml_util::Size::Enum Size() const { return thinProfile_.WeaponInfo().Size(); }

        material::Enum MaterialPrimary() const { return matPri_; }
        material::Enum MaterialSecondary() const { return matSec_; }

        Score_t TreasureScore() const { return score_; }

        Score_t ReligiousScore() const
        {
            return Score_t(static_cast<Score_t::type>(score_.As<float>() * religiousRatio_));
        }

        bool IsReligious() const { return (0.0f < religiousRatio_); }

        const creature::SummonInfo SummonInfo() const { return summonInfo_; }

        void SetSummonInfoAndAdjustScore(const creature::SummonInfo &);

        bool IsWeapon() const { return thinProfile_.IsWeapon(); }
        bool IsArmor() const { return thinProfile_.IsArmor(); }

        name_material_type::Enum NameMaterialType() const
        {
            return thinProfile_.NameMaterialType();
        }

        bool IsNonMagicalWeaponOrArmor() const
        {
            return (IsWeapon() || IsArmor()) && (IsMagical() == false) && (IsMisc() == false);
        }

        bool IsMagical() const
        {
            return (summonInfo_.CanSummon() || IsElemental() || IsSet() || IsNamed() || IsUnique());
        }

        bool IsMisc() const
        {
            return ((MiscType() != misc_type::Count) && (MiscType() != misc_type::NotMisc));
        }

        bool IsUnique() const
        {
            return ((unique_ != unique_type::Count) && (unique_ != unique_type::NotUnique));
        }

        bool IsNamed() const
        {
            return ((named_ != named_type::Count) && (named_ != named_type::NotNamed));
        }

        bool IsSet() const { return ((set_ != set_type::Count) && (set_ != set_type::NotASet)); }

        bool IsElemental() const { return (element_ != element_type::None); }

        const weapon::WeaponTypeWrapper & WeaponInfo() const { return thinProfile_.WeaponInfo(); }

        const armor::ArmorTypeWrapper & ArmorInfo() const { return thinProfile_.ArmorInfo(); }

        const std::string ToString() const;

        void SetUniqueThenSetMisc(
            const unique_type::Enum UNIQUE_TYPE,
            const misc_type::Enum MISC_TYPE,
            const material::Enum MATERIAL_PRIMARY,
            const material::Enum MATERIAL_SECONDARY,
            const element_type::Enum ELEMENT_TYPE,
            const bool IS_PIXIE);

        void SetMisc(
            const misc_type::Enum MISC_TYPE,
            const bool IS_PIXIE,
            const material::Enum MATERIAL_PRIMARY = material::Nothing,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const set_type::Enum SET_TYPE = set_type::NotASet,
            const element_type::Enum ELEMENT_TYPE = element_type::None,
            const creature::SummonInfo & SUMMON_INFO = creature::SummonInfo());

        void SetMisc(
            const ItemProfileThin & THIN_PROFILE,
            const bool IS_PIXIE,
            const material::Enum MATERIAL_PRIMARY = material::Nothing,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const set_type::Enum SET_TYPE = set_type::NotASet,
            const element_type::Enum ELEMENT_TYPE = element_type::None,
            const creature::SummonInfo & SUMMON_INFO = creature::SummonInfo());

        void SetShield(
            const armor::shield_type::Enum SHIELD_TYPE,
            const material::Enum MATERIAL_PRIMARY = material::Nothing,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const named_type::Enum NAMED_TYPE = named_type::NotNamed,
            const set_type::Enum SET_TYPE = set_type::NotASet,
            const element_type::Enum ELEMENT_TYPE = element_type::None);

        void SetShield(
            const ItemProfileThin & THIN_PROFILE,
            const armor::shield_type::Enum SHIELD_TYPE,
            const material::Enum MATERIAL_PRIMARY = material::Nothing,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const named_type::Enum NAMED_TYPE = named_type::NotNamed,
            const set_type::Enum SET_TYPE = set_type::NotASet,
            const element_type::Enum ELEMENT_TYPE = element_type::None)
        {
            SetArmorHelperSpecific(
                THIN_PROFILE,
                SHIELD_TYPE,
                MATERIAL_PRIMARY,
                MATERIAL_SECONDARY,
                NAMED_TYPE,
                SET_TYPE,
                ELEMENT_TYPE,
                false,
                category::OneHanded);
        }

        void SetHelm(
            const armor::helm_type::Enum HELM_TYPE,
            const material::Enum MATERIAL_PRIMARY = material::Nothing,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const named_type::Enum NAMED_TYPE = named_type::NotNamed,
            const set_type::Enum SET_TYPE = set_type::NotASet,
            const element_type::Enum ELEMENT_TYPE = element_type::None);

        void SetHelm(
            const ItemProfileThin & THIN_PROFILE,
            const armor::helm_type::Enum HELM_TYPE,
            const material::Enum MATERIAL_PRIMARY = material::Nothing,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const named_type::Enum NAMED_TYPE = named_type::NotNamed,
            const set_type::Enum SET_TYPE = set_type::NotASet,
            const element_type::Enum ELEMENT_TYPE = element_type::None)
        {
            SetArmorHelperSpecific(
                THIN_PROFILE,
                HELM_TYPE,
                MATERIAL_PRIMARY,
                MATERIAL_SECONDARY,
                NAMED_TYPE,
                SET_TYPE,
                ELEMENT_TYPE,
                false);
        }

        void SetCover(
            const armor::cover_type::Enum COVER_TYPE,
            const material::Enum MATERIAL_PRIMARY = material::Nothing,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const named_type::Enum NAMED_TYPE = named_type::NotNamed,
            const set_type::Enum SET_TYPE = set_type::NotASet,
            const element_type::Enum ELEMENT_TYPE = element_type::None,
            const bool IS_PIXIE = false,
            const misc_type::Enum MISC_TYPE = misc_type::NotMisc);

        void SetCover(
            const ItemProfileThin & THIN_PROFILE,
            const armor::cover_type::Enum COVER_TYPE,
            const material::Enum MATERIAL_PRIMARY = material::Nothing,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const named_type::Enum NAMED_TYPE = named_type::NotNamed,
            const set_type::Enum SET_TYPE = set_type::NotASet,
            const element_type::Enum ELEMENT_TYPE = element_type::None,
            const bool IS_PIXIE = false,
            const misc_type::Enum MISC_TYPE = misc_type::NotMisc)
        {
            SetArmorHelperSpecific(
                THIN_PROFILE,
                COVER_TYPE,
                MATERIAL_PRIMARY,
                MATERIAL_SECONDARY,
                NAMED_TYPE,
                SET_TYPE,
                ELEMENT_TYPE,
                IS_PIXIE,
                category::None,
                MISC_TYPE);
        }

        void SetAventail(
            const ItemProfileThin & THIN_PROFILE,
            const material::Enum MATERIAL_PRIMARY,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const named_type::Enum NAMED_TYPE = named_type::NotNamed,
            const set_type::Enum SET_TYPE = set_type::NotASet,
            const element_type::Enum ELEMENT_TYPE = element_type::None)
        {
            SetArmorWithBaseTypeHelper(
                THIN_PROFILE,
                MATERIAL_PRIMARY,
                MATERIAL_SECONDARY,
                NAMED_TYPE,
                SET_TYPE,
                ELEMENT_TYPE,
                false);
        }

        void SetAventail(
            const armor::base_type::Enum BASE_TYPE,
            const material::Enum MATERIAL_PRIMARY,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const named_type::Enum NAMED_TYPE = named_type::NotNamed,
            const set_type::Enum SET_TYPE = set_type::NotASet,
            const element_type::Enum ELEMENT_TYPE = element_type::None);

        void SetBracer(
            const ItemProfileThin & THIN_PROFILE,
            const material::Enum MATERIAL_PRIMARY,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const named_type::Enum NAMED_TYPE = named_type::NotNamed,
            const set_type::Enum SET_TYPE = set_type::NotASet,
            const element_type::Enum ELEMENT_TYPE = element_type::None,
            const bool IS_PIXIE = false)
        {
            SetArmorWithBaseTypeHelper(
                THIN_PROFILE,
                MATERIAL_PRIMARY,
                MATERIAL_SECONDARY,
                NAMED_TYPE,
                SET_TYPE,
                ELEMENT_TYPE,
                IS_PIXIE);
        }

        void SetBracer(
            const armor::base_type::Enum BASE_TYPE,
            const material::Enum MATERIAL_PRIMARY,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const named_type::Enum NAMED_TYPE = named_type::NotNamed,
            const set_type::Enum SET_TYPE = set_type::NotASet,
            const element_type::Enum ELEMENT_TYPE = element_type::None,
            const bool IS_PIXIE = false);

        void SetShirt(
            const ItemProfileThin & THIN_PROFILE,
            const material::Enum MATERIAL_PRIMARY = material::Nothing,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const named_type::Enum NAMED_TYPE = named_type::NotNamed,
            const set_type::Enum SET_TYPE = set_type::NotASet,
            const element_type::Enum ELEMENT_TYPE = element_type::None,
            const bool IS_PIXIE = false)
        {
            SetArmorWithBaseTypeHelper(
                THIN_PROFILE,
                MATERIAL_PRIMARY,
                MATERIAL_SECONDARY,
                NAMED_TYPE,
                SET_TYPE,
                ELEMENT_TYPE,
                IS_PIXIE);
        }

        void SetShirt(
            const armor::base_type::Enum BASE_TYPE = armor::base_type::Plain,
            const material::Enum MATERIAL_PRIMARY = material::Nothing,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const named_type::Enum NAMED_TYPE = named_type::NotNamed,
            const set_type::Enum SET_TYPE = set_type::NotASet,
            const element_type::Enum ELEMENT_TYPE = element_type::None,
            const bool IS_PIXIE = false);

        void SetBoots(
            const ItemProfileThin & THIN_PROFILE,
            const material::Enum MATERIAL_PRIMARY,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const named_type::Enum NAMED_TYPE = named_type::NotNamed,
            const set_type::Enum SET_TYPE = set_type::NotASet,
            const element_type::Enum ELEMENT_TYPE = element_type::None,
            const bool IS_PIXIE = false)
        {
            SetArmorWithBaseTypeHelper(
                THIN_PROFILE,
                MATERIAL_PRIMARY,
                MATERIAL_SECONDARY,
                NAMED_TYPE,
                SET_TYPE,
                ELEMENT_TYPE,
                IS_PIXIE);
        }

        void SetBoots(
            const armor::base_type::Enum BASE_TYPE,
            const material::Enum MATERIAL_PRIMARY,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const named_type::Enum NAMED_TYPE = named_type::NotNamed,
            const set_type::Enum SET_TYPE = set_type::NotASet,
            const element_type::Enum ELEMENT_TYPE = element_type::None,
            const bool IS_PIXIE = false);

        void SetPants(
            const ItemProfileThin & THIN_PROFILE,
            const material::Enum MATERIAL_PRIMARY = material::Nothing,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const named_type::Enum NAMED_TYPE = named_type::NotNamed,
            const set_type::Enum SET_TYPE = set_type::NotASet,
            const element_type::Enum ELEMENT_TYPE = element_type::None,
            const bool IS_PIXIE = false)
        {
            SetArmorWithBaseTypeHelper(
                THIN_PROFILE,
                MATERIAL_PRIMARY,
                MATERIAL_SECONDARY,
                NAMED_TYPE,
                SET_TYPE,
                ELEMENT_TYPE,
                IS_PIXIE);
        }

        void SetPants(
            const armor::base_type::Enum BASE_TYPE = armor::base_type::Plain,
            const material::Enum MATERIAL_PRIMARY = material::Nothing,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const named_type::Enum NAMED_TYPE = named_type::NotNamed,
            const set_type::Enum SET_TYPE = set_type::NotASet,
            const element_type::Enum ELEMENT_TYPE = element_type::None,
            const bool IS_PIXIE = false);

        void SetGauntlets(
            const ItemProfileThin & THIN_PROFILE,
            const material::Enum MATERIAL_PRIMARY = material::Nothing,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const named_type::Enum NAMED_TYPE = named_type::NotNamed,
            const set_type::Enum SET_TYPE = set_type::NotASet,
            const element_type::Enum ELEMENT_TYPE = element_type::None,
            const bool IS_PIXIE = false)
        {
            SetArmorWithBaseTypeHelper(
                THIN_PROFILE,
                MATERIAL_PRIMARY,
                MATERIAL_SECONDARY,
                NAMED_TYPE,
                SET_TYPE,
                ELEMENT_TYPE,
                IS_PIXIE);
        }

        void SetGauntlets(
            const armor::base_type::Enum BASE_TYPE = armor::base_type::Plain,
            const material::Enum MATERIAL_PRIMARY = material::Nothing,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const named_type::Enum NAMED_TYPE = named_type::NotNamed,
            const set_type::Enum SET_TYPE = set_type::NotASet,
            const element_type::Enum ELEMENT_TYPE = element_type::None,
            const bool IS_PIXIE = false);

        void SetSword(
            const weapon::sword_type::Enum SWORD_TYPE,
            const material::Enum MATERIAL_PRIMARY = material::Nothing,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const named_type::Enum NAMED_TYPE = named_type::NotNamed,
            const set_type::Enum SET_TYPE = set_type::NotASet,
            const element_type::Enum ELEMENT_TYPE = element_type::None);

        void SetSword(
            const ItemProfileThin & THIN_PROFILE,
            const weapon::sword_type::Enum SWORD_TYPE,
            const material::Enum MATERIAL_PRIMARY = material::Nothing,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const named_type::Enum NAMED_TYPE = named_type::NotNamed,
            const set_type::Enum SET_TYPE = set_type::NotASet,
            const element_type::Enum ELEMENT_TYPE = element_type::None)
        {
            SetWeaponHelper(
                THIN_PROFILE,
                ScoreHelper::Score(SWORD_TYPE),
                MATERIAL_PRIMARY,
                MATERIAL_SECONDARY,
                NAMED_TYPE,
                SET_TYPE,
                ELEMENT_TYPE);
        }

        void SetAxe(
            const weapon::axe_type::Enum AXE_TYPE,
            const material::Enum MATERIAL_PRIMARY = material::Nothing,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const named_type::Enum NAMED_TYPE = named_type::NotNamed,
            const set_type::Enum SET_TYPE = set_type::NotASet,
            const element_type::Enum ELEMENT_TYPE = element_type::None);

        void SetAxe(
            const ItemProfileThin & THIN_PROFILE,
            const weapon::axe_type::Enum AXE_TYPE,
            const material::Enum MATERIAL_PRIMARY = material::Nothing,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const named_type::Enum NAMED_TYPE = named_type::NotNamed,
            const set_type::Enum SET_TYPE = set_type::NotASet,
            const element_type::Enum ELEMENT_TYPE = element_type::None)
        {
            SetWeaponHelper(
                THIN_PROFILE,
                ScoreHelper::Score(AXE_TYPE),
                MATERIAL_PRIMARY,
                MATERIAL_SECONDARY,
                NAMED_TYPE,
                SET_TYPE,
                ELEMENT_TYPE);
        }

        void SetClub(
            const weapon::club_type::Enum CLUB_TYPE,
            const material::Enum MATERIAL_PRIMARY = material::Nothing,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const named_type::Enum NAMED_TYPE = named_type::NotNamed,
            const set_type::Enum SET_TYPE = set_type::NotASet,
            const element_type::Enum ELEMENT_TYPE = element_type::None);

        void SetClub(
            const ItemProfileThin & THIN_PROFILE,
            const weapon::club_type::Enum CLUB_TYPE,
            const material::Enum MATERIAL_PRIMARY = material::Nothing,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const named_type::Enum NAMED_TYPE = named_type::NotNamed,
            const set_type::Enum SET_TYPE = set_type::NotASet,
            const element_type::Enum ELEMENT_TYPE = element_type::None)
        {
            SetWeaponHelper(
                THIN_PROFILE,
                ScoreHelper::Score(CLUB_TYPE),
                MATERIAL_PRIMARY,
                MATERIAL_SECONDARY,
                NAMED_TYPE,
                SET_TYPE,
                ELEMENT_TYPE);
        }

        void SetWhip(
            const weapon::whip_type::Enum WHIP_TYPE,
            const material::Enum MATERIAL_PRIMARY = material::Nothing,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const named_type::Enum NAMED_TYPE = named_type::NotNamed,
            const set_type::Enum SET_TYPE = set_type::NotASet,
            const element_type::Enum ELEMENT_TYPE = element_type::None);

        void SetWhip(
            const ItemProfileThin & THIN_PROFILE,
            const weapon::whip_type::Enum WHIP_TYPE,
            const material::Enum MATERIAL_PRIMARY = material::Nothing,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const named_type::Enum NAMED_TYPE = named_type::NotNamed,
            const set_type::Enum SET_TYPE = set_type::NotASet,
            const element_type::Enum ELEMENT_TYPE = element_type::None)
        {
            SetWeaponHelper(
                THIN_PROFILE,
                ScoreHelper::Score(WHIP_TYPE),
                MATERIAL_PRIMARY,
                MATERIAL_SECONDARY,
                NAMED_TYPE,
                SET_TYPE,
                ELEMENT_TYPE);
        }

        void SetProjectile(
            const weapon::projectile_type::Enum PROJECTILE_TYPE,
            const material::Enum MATERIAL_PRIMARY = material::Nothing,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const named_type::Enum NAMED_TYPE = named_type::NotNamed,
            const set_type::Enum SET_TYPE = set_type::NotASet,
            const element_type::Enum ELEMENT_TYPE = element_type::None);

        void SetProjectile(
            const ItemProfileThin & THIN_PROFILE,
            const weapon::projectile_type::Enum PROJECTILE_TYPE,
            const material::Enum MATERIAL_PRIMARY = material::Nothing,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const named_type::Enum NAMED_TYPE = named_type::NotNamed,
            const set_type::Enum SET_TYPE = set_type::NotASet,
            const element_type::Enum ELEMENT_TYPE = element_type::None)
        {
            using namespace weapon;

            SetWeaponHelper(
                THIN_PROFILE,
                ScoreHelper::Score(PROJECTILE_TYPE),
                MATERIAL_PRIMARY,
                MATERIAL_SECONDARY,
                NAMED_TYPE,
                SET_TYPE,
                ELEMENT_TYPE);
        }

        void SetKnife(
            const sfml_util::Size::Enum SIZE = sfml_util::Size::Medium,
            const material::Enum MATERIAL_PRIMARY = material::Nothing,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const named_type::Enum NAMED_TYPE = named_type::NotNamed,
            const set_type::Enum SET_TYPE = set_type::NotASet,
            const element_type::Enum ELEMENT_TYPE = element_type::None,
            const bool IS_PIXIE = false);

        void SetKnife(
            const ItemProfileThin & THIN_PROFILE,
            const sfml_util::Size::Enum SIZE = sfml_util::Size::Medium,
            const material::Enum MATERIAL_PRIMARY = material::Nothing,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const named_type::Enum NAMED_TYPE = named_type::NotNamed,
            const set_type::Enum SET_TYPE = set_type::NotASet,
            const element_type::Enum ELEMENT_TYPE = element_type::None,
            const bool IS_PIXIE = false)
        {
            SetWeaponHelper(
                THIN_PROFILE,
                ScoreHelper::ScoreKnife(SIZE),
                MATERIAL_PRIMARY,
                MATERIAL_SECONDARY,
                NAMED_TYPE,
                SET_TYPE,
                ELEMENT_TYPE,
                IS_PIXIE);
        }

        void SetDagger(
            const sfml_util::Size::Enum SIZE = sfml_util::Size::Medium,
            const material::Enum MATERIAL_PRIMARY = material::Nothing,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const named_type::Enum NAMED_TYPE = named_type::NotNamed,
            const set_type::Enum SET_TYPE = set_type::NotASet,
            const element_type::Enum ELEMENT_TYPE = element_type::None,
            const bool IS_PIXIE = false);

        void SetDagger(
            const ItemProfileThin & THIN_PROFILE,
            const sfml_util::Size::Enum SIZE = sfml_util::Size::Medium,
            const material::Enum MATERIAL_PRIMARY = material::Nothing,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const named_type::Enum NAMED_TYPE = named_type::NotNamed,
            const set_type::Enum SET_TYPE = set_type::NotASet,
            const element_type::Enum ELEMENT_TYPE = element_type::None,
            const bool IS_PIXIE = false)
        {
            SetWeaponHelper(
                THIN_PROFILE,
                ScoreHelper::ScoreDagger(SIZE),
                MATERIAL_PRIMARY,
                MATERIAL_SECONDARY,
                NAMED_TYPE,
                SET_TYPE,
                ELEMENT_TYPE,
                IS_PIXIE);
        }

        void SetQuarterStaff(
            const material::Enum MATERIAL_PRIMARY = material::Nothing,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const named_type::Enum NAMED_TYPE = named_type::NotNamed,
            const set_type::Enum SET_TYPE = set_type::NotASet,
            const element_type::Enum ELEMENT_TYPE = element_type::None);

        void SetQuarterStaff(
            const ItemProfileThin & THIN_PROFILE,
            const material::Enum MATERIAL_PRIMARY = material::Nothing,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const named_type::Enum NAMED_TYPE = named_type::NotNamed,
            const set_type::Enum SET_TYPE = set_type::NotASet,
            const element_type::Enum ELEMENT_TYPE = element_type::None)
        {
            SetWeaponHelper(
                THIN_PROFILE,
                ScoreHelper::ScoreQuarterStaff(),
                MATERIAL_PRIMARY,
                MATERIAL_SECONDARY,
                NAMED_TYPE,
                SET_TYPE,
                ELEMENT_TYPE);
        }

        void SetStaff(
            const material::Enum MATERIAL_PRIMARY = material::Nothing,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const named_type::Enum NAMED_TYPE = named_type::NotNamed,
            const set_type::Enum SET_TYPE = set_type::NotASet,
            const element_type::Enum ELEMENT_TYPE = element_type::None,
            const misc_type::Enum MISC_TYPE = misc_type::NotMisc);

        void SetStaff(
            const ItemProfileThin & THIN_PROFILE,
            const material::Enum MATERIAL_PRIMARY = material::Nothing,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const named_type::Enum NAMED_TYPE = named_type::NotNamed,
            const set_type::Enum SET_TYPE = set_type::NotASet,
            const element_type::Enum ELEMENT_TYPE = element_type::None,
            const misc_type::Enum MISC_TYPE = misc_type::NotMisc)
        {
            SetWeaponHelper(
                THIN_PROFILE,
                ScoreHelper::ScoreStaff(),
                MATERIAL_PRIMARY,
                MATERIAL_SECONDARY,
                NAMED_TYPE,
                SET_TYPE,
                ELEMENT_TYPE,
                false,
                MISC_TYPE);
        }

        void SetBladedStaff(
            const weapon::bladedstaff_type::Enum BLADEDSTAFF_TYPE,
            const material::Enum MATERIAL_PRIMARY = material::Nothing,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const named_type::Enum NAMED_TYPE = named_type::NotNamed,
            const set_type::Enum SET_TYPE = set_type::NotASet,
            const element_type::Enum ELEMENT_TYPE = element_type::None,
            const misc_type::Enum MISC_TYPE = misc_type::NotMisc);

        void SetBladedStaff(
            const ItemProfileThin & THIN_PROFILE,
            const weapon::bladedstaff_type::Enum BLADEDSTAFF_TYPE,
            const material::Enum MATERIAL_PRIMARY = material::Nothing,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const named_type::Enum NAMED_TYPE = named_type::NotNamed,
            const set_type::Enum SET_TYPE = set_type::NotASet,
            const element_type::Enum ELEMENT_TYPE = element_type::None,
            const misc_type::Enum MISC_TYPE = misc_type::NotMisc)
        {
            SetWeaponHelper(
                THIN_PROFILE,
                ScoreHelper::Score(BLADEDSTAFF_TYPE),
                MATERIAL_PRIMARY,
                MATERIAL_SECONDARY,
                NAMED_TYPE,
                SET_TYPE,
                ELEMENT_TYPE,
                false,
                MISC_TYPE);
        }

        static category::Enum CategoryWeaponBodypart(const body_part::Enum);

        static category::Enum CategoryArmor()
        {
            return static_cast<category::Enum>(category::Equippable);
        }

    private:
        void SetArmorWithBaseTypeHelper(
            const ItemProfileThin & THIN_PROFILE,
            const material::Enum MATERIAL_PRIMARY,
            const material::Enum MATERIAL_SECONDARY,
            const named_type::Enum NAMED_TYPE,
            const set_type::Enum SET_TYPE,
            const element_type::Enum ELEMENT_TYPE,
            const bool IS_PIXIE);

        template <typename ArmorEnum_t>
        void SetArmorHelperSpecific(
            const ItemProfileThin & THIN_PROFILE,
            const ArmorEnum_t SPECIFIC_ARMOR_TYPE,
            const material::Enum MATERIAL_PRIMARY,
            const material::Enum MATERIAL_SECONDARY,
            const named_type::Enum NAMED_TYPE,
            const set_type::Enum SET_TYPE,
            const element_type::Enum ELEMENT_TYPE,
            const bool IS_PIXIE,
            const category::Enum CATEGORY_TO_APPEND = category::None,
            const misc_type::Enum MISC_TYPE = misc_type::NotMisc)
        {
            category_
                = static_cast<category::Enum>(category_ | CategoryArmor() | CATEGORY_TO_APPEND);

            thinProfile_ = THIN_PROFILE;

            if ((MISC_TYPE == misc_type::NotMisc) || (MISC_TYPE == misc_type::Count))
            {
                SetHelperForWeaponsAndArmor(
                    MATERIAL_PRIMARY,
                    MATERIAL_SECONDARY,
                    NAMED_TYPE,
                    SET_TYPE,
                    ELEMENT_TYPE,
                    IS_PIXIE);

                score_ += ScoreHelper::Score(SPECIFIC_ARMOR_TYPE);

                score_ += NonMiscScoreBasedOnMaterialsAndEnchantments(
                    MATERIAL_PRIMARY,
                    MATERIAL_SECONDARY,
                    NAMED_TYPE,
                    SET_TYPE,
                    ELEMENT_TYPE,
                    false);
            }
        }

        void SetWeaponHelper(
            const ItemProfileThin & THIN_PROFILE,
            const Score_t BASE_SCORE,
            const material::Enum MATERIAL_PRIMARY,
            const material::Enum MATERIAL_SECONDARY,
            const named_type::Enum NAMED_TYPE,
            const set_type::Enum SET_TYPE,
            const element_type::Enum ELEMENT_TYPE,
            const bool IS_PIXIE = false,
            const misc_type::Enum MISC_TYPE = misc_type::NotMisc);

        friend bool operator==(const ItemProfile & L, const ItemProfile & R);
        friend bool operator<(const ItemProfile & L, const ItemProfile & R);

    private:
        void SetHelperForWeaponsAndArmor(
            const material::Enum,
            const material::Enum,
            const named_type::Enum,
            const set_type::Enum,
            const element_type::Enum,
            const bool IS_PIXIE = false);

        Score_t NonMiscScoreBasedOnMaterialsAndEnchantments(
            const item::material::Enum MATERIAL_PRI,
            const item::material::Enum MATERIAL_SEC,
            const item::named_type::Enum NAMED_TYPE,
            const item::set_type::Enum SET_TYPE,
            const item::element_type::Enum ELEMENT_TYPE,
            const bool IS_WEAPON) const;

    private:
        Score_t score_;

        // this is a ratio of how religious the item is
        float religiousRatio_;

        category::Enum category_;
        ItemProfileThin thinProfile_;
        unique_type::Enum unique_;
        set_type::Enum set_;
        named_type::Enum named_;
        element_type::Enum element_;
        material::Enum matPri_;
        material::Enum matSec_;
        creature::SummonInfo summonInfo_;
        bool isPixie_;
    };

    using ItemProfileVec_t = std::vector<ItemProfile>;

    inline bool operator==(const ItemProfile & L, const ItemProfile & R)
    {
        // shortcut to member that most often varies
        if (L.score_ != R.score_)
        {
            return false;
        }

        return std::tie(
                   L.category_,
                   L.thinProfile_,
                   L.unique_,
                   L.set_,
                   L.named_,
                   L.element_,
                   L.matPri_,
                   L.matSec_,
                   L.summonInfo_,
                   L.isPixie_,
                   L.religiousRatio_)
            == std::tie(
                   R.category_,
                   R.thinProfile_,
                   R.unique_,
                   R.set_,
                   R.named_,
                   R.element_,
                   R.matPri_,
                   R.matSec_,
                   R.summonInfo_,
                   R.isPixie_,
                   R.religiousRatio_);
    }

    inline bool operator!=(const ItemProfile & L, const ItemProfile & R) { return !(L == R); }

    inline bool operator<(const ItemProfile & L, const ItemProfile & R)
    {
        // shortcut to member that most often varies
        if (L.score_ != R.score_)
        {
            return (L.score_ < R.score_);
        }

        return std::tie(
                   L.thinProfile_,
                   L.category_,
                   L.unique_,
                   L.set_,
                   L.named_,
                   L.element_,
                   L.isPixie_,
                   L.matPri_,
                   L.matSec_,
                   L.summonInfo_,
                   L.religiousRatio_)
            < std::tie(
                   R.thinProfile_,
                   R.category_,
                   R.unique_,
                   R.set_,
                   R.named_,
                   R.element_,
                   R.isPixie_,
                   R.matPri_,
                   R.matSec_,
                   R.summonInfo_,
                   R.religiousRatio_);
    }

} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_ITEMPROFILE_HPP_INCLUDE
