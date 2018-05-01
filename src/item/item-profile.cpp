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
// item-profile.cpp
//
#include "item-profile.hpp"

#include "creature/enchantment-factory.hpp"
#include "creature/race-enum.hpp"
#include "item/armor-factory.hpp"
#include "item/item-profile-warehouse.hpp"
#include "item/weapon-factory.hpp"
#include "log/log-macros.hpp"
#include "misc/boost-string-includes.hpp"

#include <sstream>

namespace heroespath
{
namespace item
{

    ItemProfile::ItemProfile()
        : score_(0)
        , religiousRatio_(0.0f)
        , category_(category::None)
        , thinProfile_()
        , weaponType_(weapon_type::NotAWeapon)
        , unique_(unique_type::NotUnique)
        , set_(set_type::NotASet)
        , named_(named_type::NotNamed)
        , element_(element_type::None)
        , matPri_(material::Nothing)
        , matSec_(material::Nothing)
        , summonInfo_()
        , isPixie_(false)
    {}

    creature::role::Enum ItemProfile::RoleRestriction() const
    {
        if ((WeaponInfo().SwordType() == weapon::sword_type::Knightlysword)
            || (WeaponInfo().SwordType() == weapon::sword_type::Claymore))
        {
            return creature::role::Knight;
        }
        else if (
            (ArmorInfo().ShieldType() == armor::shield_type::Pavis)
            || (ArmorInfo().HelmType() == armor::helm_type::Great))
        {
            return creature::role::Knight;
        }
        else if (
            (ArmorInfo().BaseType() == armor::base_type::Plate)
            && (ArmorInfo().IsGauntlets() || ArmorInfo().IsPants() || ArmorInfo().IsBoots()
                || ArmorInfo().IsShirt() || ArmorInfo().IsBracers() || ArmorInfo().IsAventail()))
        {
            return creature::role::Knight;
        }
        else if (misc_type::IsMusicalInstrument(thinProfile_.MiscType()))
        {
            return creature::role::Bard;
        }
        else
        {
            return set_type::Role(set_);
        }
    }

    void ItemProfile::SetSummonInfoAndAdjustScore(const creature::SummonInfo & SUMMON_INFO)
    {
        summonInfo_ = SUMMON_INFO;

        // Use a creature's rank min/max to establish a kind of combined power/worth/value
        // summon score, then append that to the summoning item's score.

        auto const CREATURE_RANK_RANGE{ creature::race::RaceRoleRanks(
            SUMMON_INFO.Race(), SUMMON_INFO.Role()) };

        auto const SUMMON_COUNT_D{ static_cast<double>(SUMMON_INFO.Count()) };

        auto const CREATURE_RANK_AVG{
            (CREATURE_RANK_RANGE.A().As<double>() + CREATURE_RANK_RANGE.B().As<double>()) * 0.5
        };

        score_ += Score_t(
            static_cast<Score_t::type>(std::sqrt((CREATURE_RANK_AVG * SUMMON_COUNT_D) * 150.0)));
    }

    const std::string ItemProfile::ToString() const
    {
        std::ostringstream ss;

        if (category::None != category_)
        {
            ss << ", category=" << category::ToString(category_, true);
        }

        if (armor::ArmorTypeWrapper() != thinProfile_.ArmorInfo())
        {
            ss << ", armor_info=" << thinProfile_.ArmorInfo().ToString();
        }

        auto const ARMOR_TYPE_RESTRICTION{ thinProfile_.ArmorBaseTypeRestriction() };

        if (ARMOR_TYPE_RESTRICTION != armor::base_type::Count)
        {
            ss << ", armor_base_type_restriction="
               << armor::base_type::ToString(ARMOR_TYPE_RESTRICTION);
        }

        if (weapon_type::NotAWeapon != weaponType_)
        {
            ss << ", weapon_type=" << weapon_type::ToString(weaponType_, true);
        }

        if (weapon::WeaponTypeWrapper() != thinProfile_.WeaponInfo())
        {
            ss << ", weapon_info=" << thinProfile_.WeaponInfo().ToString();
        }

        if (misc_type::NotMisc != thinProfile_.MiscType())
        {
            ss << ", misc_type="
               << ((misc_type::Count == thinProfile_.MiscType())
                       ? "Count"
                       : misc_type::ToString(thinProfile_.MiscType()));
        }

        if (isPixie_)
        {
            ss << ", is_pixie=" << std::boolalpha << isPixie_;
        }

        auto const ROLE_RESTRICTION{ RoleRestriction() };

        if (ROLE_RESTRICTION != creature::role::Count)
        {
            ss << creature::role::ToString(ROLE_RESTRICTION);
        }

        ss << ", mat_pri=" << material::ToString(matPri_);

        if (material::Nothing != matSec_)
        {
            ss << ", mat_sec=" << material::ToString(matSec_);
        }

        if (unique_type::NotUnique != unique_)
        {
            ss << ", unique_type="
               << ((unique_type::Count == unique_) ? "Count" : unique_type::ToString(unique_));
        }

        if (set_type::NotASet != set_)
        {
            ss << ", set_type=" << ((set_type::Count == set_) ? "Count" : set_type::ToString(set_));
        }

        if (named_type::NotNamed != named_)
        {
            ss << ", named_type="
               << ((named_type::Count == named_) ? "Count" : named_type::ToString(named_));
        }

        if (element_type::None != element_)
        {
            ss << ", element_type=" << element_type::ToString(element_, true);
        }

        if (summonInfo_.CanSummon())
        {
            ss << ", summonInfo=" << summonInfo_.ToString();
        }
        else if (summonInfo_.IsValid() == false)
        {
            ss << ", summonInfo=INVALID_ERROR";
        }

        return ss.str();
    }

    void ItemProfile::SetUnique(
        const unique_type::Enum UNIQUE_TYPE,
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const element_type::Enum ELEMENT_TYPE)
    {
        M_ASSERT_OR_LOGANDTHROW_SS(
            ((UNIQUE_TYPE != unique_type::Count) && (UNIQUE_TYPE != unique_type::NotUnique)),
            "item::ItemProfile::SetUnique(unique_type=Count/NotMisc, mat_pri="
                << material::ToString(MATERIAL_PRIMARY)
                << ", mat_sec=" << material::ToString(MATERIAL_SECONDARY)
                << ", element_type=" << element_type::ToString(ELEMENT_TYPE, true)
                << ") the unique_type was not valid.");

        unique_ = UNIQUE_TYPE;
        matPri_ = MATERIAL_PRIMARY;
        matSec_ = MATERIAL_SECONDARY;

        // there are no pixie unique items
        isPixie_ = false;

        religiousRatio_ = unique_type::ReligiousRatio(UNIQUE_TYPE);
        element_ = ELEMENT_TYPE;

        if (UNIQUE_TYPE == unique_type::MagnifyingGlass)
        {
            category_ = static_cast<category::Enum>(category_ | category::ShowsEnemyInfo);
        }

        if (unique_type::IsUseable(UNIQUE_TYPE))
        {
            category_ = static_cast<category::Enum>(category_ | category::Useable);
        }

        // TODO these items are not really made into weapons yet
        auto const IS_WEAPON{ (
            (UNIQUE_TYPE == unique_type::ViperFangFingerclaw)
            || (UNIQUE_TYPE == unique_type::ScorpionStingerFingerclaw)
            || (UNIQUE_TYPE == unique_type::RazorFingerclaw)) };

        // score based on material is not added here because all unique items are also misc items
        // and SetMisc() will add to the score based on materials.
        score_ += creature::EnchantmentFactory::Instance()->TreasureScore(
            UNIQUE_TYPE, MATERIAL_PRIMARY);

        score_ += creature::EnchantmentFactory::Instance()->TreasureScore(
            ELEMENT_TYPE, IS_WEAPON, MATERIAL_PRIMARY);

        score_ += 750_score;
    }

    void ItemProfile::SetMisc(
        const misc_type::Enum MISC_TYPE,
        const bool IS_PIXIE,
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const set_type::Enum SET_TYPE,
        const element_type::Enum ELEMENT_TYPE)
    {
        using namespace item;

        M_ASSERT_OR_LOGANDTHROW_SS(
            ((MISC_TYPE != misc_type::Count) && (MISC_TYPE != misc_type::NotMisc)),
            "item::ItemProfile::SetMisc(misc_type=Count/NotMisc, is_pixie="
                << std::boolalpha << IS_PIXIE
                << ", mat_pri=" << material::ToString(MATERIAL_PRIMARY)
                << ", mat_sec=" << material::ToString(MATERIAL_SECONDARY) << ", set_type="
                << ((SET_TYPE == set_type::Count) ? "Count" : set_type::ToString(SET_TYPE))
                << ") the misc_type was not valid.");

        if (misc_type::IsUseable(MISC_TYPE))
        {
            category_ = static_cast<category::Enum>(category_ | category::Useable);
        }

        if (misc_type::IsEquippable(MISC_TYPE))
        {
            category_ = static_cast<category::Enum>(category_ | category::Equippable);
        }

        thinProfile_ = ItemProfileThin::MakeMisc(MISC_TYPE);
        matPri_ = MATERIAL_PRIMARY;
        matSec_ = MATERIAL_SECONDARY;
        isPixie_ = IS_PIXIE;

        if (misc::IsRealZero(religiousRatio_))
        {
            religiousRatio_ = misc_type::ReligiousRatio(MISC_TYPE);
        }

        score_ += ScoreHelper::Score(MATERIAL_PRIMARY, MATERIAL_SECONDARY);
        score_ += creature::EnchantmentFactory::Instance()->TreasureScore(SET_TYPE);

        score_ += creature::EnchantmentFactory::Instance()->TreasureScore(
            MISC_TYPE, MATERIAL_PRIMARY, MATERIAL_SECONDARY);

        if (MISC_TYPE == misc_type::Cape)
        {
            SetCover(
                armor::cover_type::Cape,
                MATERIAL_PRIMARY,
                MATERIAL_SECONDARY,
                named_type::NotNamed,
                SET_TYPE,
                ELEMENT_TYPE,
                IS_PIXIE,
                MISC_TYPE);

            if (ELEMENT_TYPE != element_type::None)
            {
                score_ += creature::EnchantmentFactory::Instance()->TreasureScore(
                    ELEMENT_TYPE, false, MATERIAL_PRIMARY);
            }
        }
        else if (
            (MISC_TYPE == misc_type::Cloak) || (MISC_TYPE == misc_type::Ghost_Sheet)
            || (MISC_TYPE == misc_type::Robe) || (MISC_TYPE == misc_type::Shroud))
        {
            SetCover(
                armor::cover_type::Cloak,
                MATERIAL_PRIMARY,
                MATERIAL_SECONDARY,
                named_type::NotNamed,
                SET_TYPE,
                ELEMENT_TYPE,
                IS_PIXIE,
                MISC_TYPE);

            if (ELEMENT_TYPE != element_type::None)
            {
                score_ += creature::EnchantmentFactory::Instance()->TreasureScore(
                    ELEMENT_TYPE, false, MATERIAL_PRIMARY);
            }
        }
        else if (MISC_TYPE == misc_type::Staff)
        {
            SetStaff(
                MATERIAL_PRIMARY,
                MATERIAL_SECONDARY,
                named_type::NotNamed,
                SET_TYPE,
                ELEMENT_TYPE,
                MISC_TYPE);

            if (ELEMENT_TYPE != element_type::None)
            {
                score_ += creature::EnchantmentFactory::Instance()->TreasureScore(
                    ELEMENT_TYPE, true, MATERIAL_PRIMARY);
            }
        }
        else if (MISC_TYPE == misc_type::Scythe)
        {
            SetBladedStaff(
                weapon::bladedstaff_type::Scythe,
                MATERIAL_PRIMARY,
                MATERIAL_SECONDARY,
                named_type::NotNamed,
                SET_TYPE,
                ELEMENT_TYPE,
                MISC_TYPE);

            if (ELEMENT_TYPE != element_type::None)
            {
                score_ += creature::EnchantmentFactory::Instance()->TreasureScore(
                    ELEMENT_TYPE, true, MATERIAL_PRIMARY);
            }
        }
    }

    void ItemProfile::SetSword(
        const weapon::sword_type::Enum SWORD_TYPE,
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE,
        const element_type::Enum ELEMENT_TYPE)
    {
        SetWeaponHelper(
            weapon::WeaponTypeWrapper(SWORD_TYPE),
            static_cast<weapon_type::Enum>(
                weapon_type::Melee | weapon_type::Sword | weapon_type::Bladed
                | weapon_type::Pointed),
            ScoreHelper::Score(SWORD_TYPE),
            MATERIAL_PRIMARY,
            MATERIAL_SECONDARY,
            NAMED_TYPE,
            SET_TYPE,
            ELEMENT_TYPE,
            false);
    }

    void ItemProfile::SetAxe(
        const weapon::axe_type::Enum AXE_TYPE,
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE,
        const element_type::Enum ELEMENT_TYPE)
    {
        SetWeaponHelper(
            weapon::WeaponTypeWrapper(AXE_TYPE),
            static_cast<weapon_type::Enum>(
                weapon_type::Bladed | weapon_type::Axe | weapon_type::Melee),
            ScoreHelper::Score(AXE_TYPE),
            MATERIAL_PRIMARY,
            MATERIAL_SECONDARY,
            NAMED_TYPE,
            SET_TYPE,
            ELEMENT_TYPE,
            false);
    }

    void ItemProfile::SetClub(
        const weapon::club_type::Enum CLUB_TYPE,
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE,
        const element_type::Enum ELEMENT_TYPE)
    {
        SetWeaponHelper(
            weapon::WeaponTypeWrapper(CLUB_TYPE),
            static_cast<weapon_type::Enum>(weapon_type::Melee | weapon_type::Club),
            ScoreHelper::Score(CLUB_TYPE),
            MATERIAL_PRIMARY,
            MATERIAL_SECONDARY,
            NAMED_TYPE,
            SET_TYPE,
            ELEMENT_TYPE,
            false);
    }

    void ItemProfile::SetWhip(
        const weapon::whip_type::Enum WHIP_TYPE,
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE,
        const element_type::Enum ELEMENT_TYPE)
    {
        SetWeaponHelper(
            weapon::WeaponTypeWrapper(WHIP_TYPE),
            static_cast<weapon_type::Enum>(weapon_type::Melee | weapon_type::Whip),
            ScoreHelper::Score(WHIP_TYPE),
            MATERIAL_PRIMARY,
            MATERIAL_SECONDARY,
            NAMED_TYPE,
            SET_TYPE,
            ELEMENT_TYPE,
            false);
    }

    void ItemProfile::SetProjectile(
        const weapon::projectile_type::Enum PROJECTILE_TYPE,
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE,
        const element_type::Enum ELEMENT_TYPE)
    {
        using namespace weapon;

        auto const WEAPON_TYPE{ [PROJECTILE_TYPE]() {
            if (PROJECTILE_TYPE == projectile_type::Blowpipe)
            {
                return weapon_type::Blowpipe;
            }
            else if (PROJECTILE_TYPE == projectile_type::Sling)
            {
                return weapon_type::Sling;
            }
            else if (PROJECTILE_TYPE == projectile_type::Crossbow)
            {
                return weapon_type::Crossbow;
            }
            else
            {
                return weapon_type::Bow;
            }
        }() };

        SetWeaponHelper(
            WeaponTypeWrapper(PROJECTILE_TYPE),
            static_cast<weapon_type::Enum>(weapon_type::Projectile | WEAPON_TYPE),
            ScoreHelper::Score(PROJECTILE_TYPE),
            MATERIAL_PRIMARY,
            MATERIAL_SECONDARY,
            NAMED_TYPE,
            SET_TYPE,
            ELEMENT_TYPE,
            false);
    }

    void ItemProfile::SetBladedStaff(
        const weapon::bladedstaff_type::Enum BLADEDSTAFF_TYPE,
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE,
        const element_type::Enum ELEMENT_TYPE,
        const misc_type::Enum MISC_TYPE)
    {
        using namespace weapon;

        auto const POINTED_TYPE{ (
            (BLADEDSTAFF_TYPE == bladedstaff_type::Scythe) ? weapon_type::NotAWeapon
                                                           : weapon_type::Pointed) };

        auto const SPEAR_TYPE{ (
            ((BLADEDSTAFF_TYPE == bladedstaff_type::Spear)
             || (BLADEDSTAFF_TYPE == bladedstaff_type::ShortSpear))
                ? weapon_type::Spear
                : weapon_type::NotAWeapon) };

        SetWeaponHelper(
            WeaponTypeWrapper(BLADEDSTAFF_TYPE),
            static_cast<weapon_type::Enum>(
                weapon_type::BladedStaff | weapon_type::Melee | POINTED_TYPE | SPEAR_TYPE),
            ScoreHelper::Score(BLADEDSTAFF_TYPE),
            MATERIAL_PRIMARY,
            MATERIAL_SECONDARY,
            NAMED_TYPE,
            SET_TYPE,
            ELEMENT_TYPE,
            false,
            MISC_TYPE);
    }

    void ItemProfile::SetKnife(
        const sfml_util::Size::Enum SIZE,
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE,
        const element_type::Enum ELEMENT_TYPE,
        const bool IS_PIXIE)
    {
        SetWeaponHelper(
            weapon::WeaponTypeWrapper(false, SIZE),
            static_cast<weapon_type::Enum>(
                weapon_type::Bladed | weapon_type::Knife | weapon_type::Melee
                | weapon_type::Pointed),
            ScoreHelper::ScoreKnife(SIZE),
            MATERIAL_PRIMARY,
            MATERIAL_SECONDARY,
            NAMED_TYPE,
            SET_TYPE,
            ELEMENT_TYPE,
            IS_PIXIE);
    }

    void ItemProfile::SetDagger(
        const sfml_util::Size::Enum SIZE,
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE,
        const element_type::Enum ELEMENT_TYPE,
        const bool IS_PIXIE)
    {
        SetWeaponHelper(
            weapon::WeaponTypeWrapper(true, SIZE),
            static_cast<weapon_type::Enum>(
                weapon_type::Bladed | weapon_type::Knife | weapon_type::Melee
                | weapon_type::Pointed),
            ScoreHelper::ScoreDagger(SIZE),
            MATERIAL_PRIMARY,
            MATERIAL_SECONDARY,
            NAMED_TYPE,
            SET_TYPE,
            ELEMENT_TYPE,
            IS_PIXIE);
    }

    void ItemProfile::SetStaff(
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE,
        const element_type::Enum ELEMENT_TYPE,
        const misc_type::Enum MISC_TYPE)
    {
        SetWeaponHelper(
            weapon::WeaponTypeWrapper(false),
            static_cast<weapon_type::Enum>(weapon_type::Staff | weapon_type::Melee),
            ScoreHelper::ScoreStaff(),
            MATERIAL_PRIMARY,
            MATERIAL_SECONDARY,
            NAMED_TYPE,
            SET_TYPE,
            ELEMENT_TYPE,
            false,
            MISC_TYPE);
    }

    void ItemProfile::SetQuarterStaff(
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE,
        const element_type::Enum ELEMENT_TYPE)
    {
        SetWeaponHelper(
            weapon::WeaponTypeWrapper(true),
            static_cast<weapon_type::Enum>(weapon_type::Staff | weapon_type::Melee),
            ScoreHelper::ScoreQuarterStaff(),
            MATERIAL_PRIMARY,
            MATERIAL_SECONDARY,
            NAMED_TYPE,
            SET_TYPE,
            ELEMENT_TYPE,
            false);
    }

    void ItemProfile::SetArmorWithBaseTypeHelper(
        const armor_type::Enum ARMOR_TYPE,
        const armor::base_type::Enum BASE_TYPE,
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE,
        const element_type::Enum ELEMENT_TYPE,
        const bool IS_PIXIE)
    {
        category_ = static_cast<category::Enum>(category_ | CategoryArmor());
        thinProfile_ = ItemProfileThin::MakeArmor(ARMOR_TYPE, BASE_TYPE);

        SetHelperForWeaponsAndArmor(
            MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE, IS_PIXIE);

        score_ += ScoreHelper::Score(ARMOR_TYPE, BASE_TYPE);

        score_ += ScoreHelper(
            MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE, false);
    }

    void ItemProfile::SetWeaponHelper(
        const weapon::WeaponTypeWrapper & WEAPON_TYPE_WRAPPER,
        const weapon_type::Enum WEAPON_TYPE_TO_APPEND,
        const Score_t BASE_SCORE,
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE,
        const element_type::Enum ELEMENT_TYPE,
        const bool IS_PIXIE,
        const misc_type::Enum MISC_TYPE)
    {
        auto const DETAILS{ weapon::WeaponDetailLoader::LookupWeaponDetails(
            WEAPON_TYPE_WRAPPER.DetailsKeyName()) };

        category_
            = static_cast<category::Enum>(category_ | category::Equippable | DETAILS.handedness);

        thinProfile_ = ItemProfileThin::MakeWeapon(WEAPON_TYPE_WRAPPER, MISC_TYPE);

        weaponType_ = static_cast<weapon_type::Enum>(weaponType_ | WEAPON_TYPE_TO_APPEND);

        if ((MISC_TYPE == misc_type::NotMisc) || (MISC_TYPE == misc_type::Count))
        {
            SetHelperForWeaponsAndArmor(
                MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE, IS_PIXIE);

            score_ += BASE_SCORE
                + ScoreHelper(
                          MATERIAL_PRIMARY,
                          MATERIAL_SECONDARY,
                          NAMED_TYPE,
                          SET_TYPE,
                          ELEMENT_TYPE,
                          true);
        }
    }

    category::Enum ItemProfile::CategoryWeaponBodypart(const body_part::Enum BODY_PART)
    {
        if ((BODY_PART == body_part::Bite) || (BODY_PART == body_part::Breath))
        {
            return static_cast<category::Enum>(category::Equippable | category::BodyPart);
        }
        else
        {
            return static_cast<category::Enum>(
                category::Equippable | category::TwoHanded | category::BodyPart);
        }
    }

    void ItemProfile::SetHelperForWeaponsAndArmor(
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const named_type::Enum NAMED_ENUM,
        const set_type::Enum SET_ENUM,
        const element_type::Enum ELEMENT_TYPE,
        const bool IS_PIXIE)
    {
        matPri_ = MATERIAL_PRIMARY;
        matSec_ = MATERIAL_SECONDARY;
        named_ = NAMED_ENUM;
        set_ = SET_ENUM;
        element_ = ELEMENT_TYPE;
        isPixie_ = IS_PIXIE;
    }

    Score_t ItemProfile::ScoreHelper(
        const material::Enum MATERIAL_PRI,
        const material::Enum MATERIAL_SEC,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE,
        const element_type::Enum ELEMENT_TYPE,
        const bool IS_WEAPON) const
    {
        // This check for an invalid primary material is really a check if this call is based on a
        // thin profile. If this is for a thin profile then the score is ignored and the time
        // consuming calls to EnchantmentFactory can be skipped.
        if ((MATERIAL_PRI == material::Nothing) || (MATERIAL_PRI == material::Count))
        {
            return 0_score;
        }
        else
        {
            return (
                ScoreHelper::Score(MATERIAL_PRI, MATERIAL_SEC)
                + creature::EnchantmentFactory::Instance()->TreasureScore(
                      NAMED_TYPE, MATERIAL_PRI, IS_WEAPON, !IS_WEAPON)
                + creature::EnchantmentFactory::Instance()->TreasureScore(SET_TYPE)
                + creature::EnchantmentFactory::Instance()->TreasureScore(
                      ELEMENT_TYPE, IS_WEAPON, MATERIAL_PRI));
        }
    }

} // namespace item
} // namespace heroespath
