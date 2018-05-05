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
            ss << "category=" << category::ToString(category_, true);
        }

        if (ss.str().empty() == false)
        {
            ss << ", ";
        }

        ss << thinProfile_.ToString();

        if (isPixie_)
        {
            ss << ", is_pixie=" << std::boolalpha << isPixie_;
        }

        auto const ROLE_RESTRICTION{ RoleRestriction() };

        if (ROLE_RESTRICTION != creature::role::Count)
        {
            ss << ", role_restriction=" << creature::role::ToString(ROLE_RESTRICTION);
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
        else if (summonInfo_.IsDefaultAndCompletelyInvalid() == false)
        {
            ss << ", summonInfo=" << summonInfo_.ToString() << "(but CanSummon()=false?)";
        }

        return ss.str();
    }

    void ItemProfile::SetUniqueThenSetMisc(
        const unique_type::Enum UNIQUE_TYPE,
        const misc_type::Enum MISC_TYPE,
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const element_type::Enum ELEMENT_TYPE,
        const bool IS_PIXIE)
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

        /*
         * TODO these items are not really made into weapons yet
         *
        auto const IS_WEAPON{ (
            (UNIQUE_TYPE == unique_type::ViperFangFingerclaw)
            || (UNIQUE_TYPE == unique_type::ScorpionStingerFingerclaw)
            || (UNIQUE_TYPE == unique_type::RazorFingerclaw)) };
            */

        // score based on material and element_type is not added here because all unique items are
        // also misc items and SetMisc() will add those score bonuses
        score_ += creature::EnchantmentFactory::Instance()->TreasureScore(
            UNIQUE_TYPE, MATERIAL_PRIMARY);

        score_ += 750_score;

        SetMisc(
            MISC_TYPE,
            IS_PIXIE,
            MATERIAL_PRIMARY,
            MATERIAL_SECONDARY,
            set_type::NotASet,
            ELEMENT_TYPE);
    }

    void ItemProfile::SetMisc(
        const ItemProfileThin & THIN_PROFILE,
        const bool IS_PIXIE,
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const set_type::Enum SET_TYPE,
        const element_type::Enum ELEMENT_TYPE,
        const creature::SummonInfo & SUMMON_INFO)
    {
        using namespace item;

        auto const MISC_TYPE{ THIN_PROFILE.MiscType() };

        M_ASSERT_OR_LOGANDTHROW_SS(
            ((MISC_TYPE != misc_type::Count) && (MISC_TYPE != misc_type::NotMisc)),
            "item::ItemProfile::SetMisc(misc_type=Count/NotMisc, is_pixie="
                << std::boolalpha << IS_PIXIE
                << ", mat_pri=" << material::ToString(MATERIAL_PRIMARY)
                << ", mat_sec=" << material::ToString(MATERIAL_SECONDARY) << ", set_type="
                << ((SET_TYPE == set_type::Count) ? "Count" : set_type::ToString(SET_TYPE))
                << ", element_type=" << element_type::ToString(ELEMENT_TYPE, true)
                << ") the misc_type was not valid.");

        if (misc_type::IsUseable(MISC_TYPE))
        {
            category_ = static_cast<category::Enum>(category_ | category::Useable);
        }

        if (misc_type::IsEquippable(MISC_TYPE))
        {
            category_ = static_cast<category::Enum>(category_ | category::Equippable);
        }

        if (misc_type::IsWearable(MISC_TYPE))
        {
            category_ = static_cast<category::Enum>(category_ | category::Wearable);
        }

        thinProfile_ = THIN_PROFILE;
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

        if (ELEMENT_TYPE != element_type::None)
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (misc_type::IsEquippable(MISC_TYPE)),
                "item::ItemProfile::SetMisc(misc_type="
                    << misc_type::ToString(MISC_TYPE) << ", is_pixie=" << std::boolalpha << IS_PIXIE
                    << ", mat_pri=" << material::ToString(MATERIAL_PRIMARY)
                    << ", mat_sec=" << material::ToString(MATERIAL_SECONDARY) << ", set_type="
                    << ((SET_TYPE == set_type::Count) ? "Count" : set_type::ToString(SET_TYPE))
                    << ", element_type=" << element_type::ToString(ELEMENT_TYPE, true)
                    << ") element_type wasn't None but the misc_type was not equippable.");

            score_ += creature::EnchantmentFactory::Instance()->TreasureScore(
                ELEMENT_TYPE, misc_type::IsWeapon(MISC_TYPE), MATERIAL_PRIMARY);
        }

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
        }

        if (SUMMON_INFO.CanSummon())
        {
            SetSummonInfoAndAdjustScore(SUMMON_INFO);
        }
    }

    void ItemProfile::SetArmorWithBaseTypeHelper(
        const ItemProfileThin & THIN_PROFILE,
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE,
        const element_type::Enum ELEMENT_TYPE,
        const bool IS_PIXIE)
    {
        category_ = static_cast<category::Enum>(category_ | CategoryArmor());
        thinProfile_ = THIN_PROFILE;

        SetHelperForWeaponsAndArmor(
            MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE, IS_PIXIE);

        score_ += ScoreHelper::Score(
            THIN_PROFILE.ArmorInfo().Type(), THIN_PROFILE.ArmorInfo().BaseType());

        score_ += NonMiscScoreBasedOnMaterialsAndEnchantments(
            MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE, false);
    }

    void ItemProfile::SetWeaponHelper(
        const ItemProfileThin & THIN_PROFILE,
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
            THIN_PROFILE.WeaponInfo().DetailsKeyName()) };

        category_
            = static_cast<category::Enum>(category_ | category::Equippable | DETAILS.handedness);

        thinProfile_ = THIN_PROFILE;

        if ((MISC_TYPE == misc_type::NotMisc) || (MISC_TYPE == misc_type::Count))
        {
            SetHelperForWeaponsAndArmor(
                MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE, IS_PIXIE);

            score_ += BASE_SCORE
                + NonMiscScoreBasedOnMaterialsAndEnchantments(
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

    Score_t ItemProfile::NonMiscScoreBasedOnMaterialsAndEnchantments(
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
