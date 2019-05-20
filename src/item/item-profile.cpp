// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// item-profile.cpp
//
#include "item-profile.hpp"

#include "creature/enchantment-factory.hpp"
#include "creature/race-enum.hpp"
#include "item/item-profile-thin-factory.hpp"
#include "item/item-profile-warehouse.hpp"
#include "misc/log-macros.hpp"

namespace heroespath
{
namespace item
{

    ItemProfile::ItemProfile()
        : score_(0_score)
        , religiousRatio_(0.0f)
        , category_(category::None)
        , thinProfile_()
        , set_(set_type::Not)
        , named_(named_type::Not)
        , element_(element_type::None)
        , matPri_(material::Nothing)
        , matSec_(material::Nothing)
        , summonInfo_()
        , isPixie_(false)
    {}

    creature::role::Enum ItemProfile::RoleRestriction() const
    {
        if ((WeaponInfo().SwordType() == weapon::sword_type::KnightlySword)
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

    bool ItemProfile::MustBePixieVersionForPixiesToEquip() const
    {
        if (IsWeapon() || IsArmor())
        {
            return true;
        }
        else // assume must be misc
        {
            return misc_type::MustBePixieVersionForPixiesToEquip(MiscType());
        }
    }

    void ItemProfile::SetSummonInfoAndAdjustScore(const creature::SummonInfo & SUMMON_INFO)
    {
        summonInfo_ = SUMMON_INFO;

        // Use a creature's rank min/max to establish a kind of combined power/worth/value
        // summon score, then append that to the summoning item's score.

        const auto CREATURE_RANK_RANGE { creature::race::RaceRoleRanks(
            SUMMON_INFO.Race(), SUMMON_INFO.Role()) };

        const auto SUMMON_COUNT_D { static_cast<double>(SUMMON_INFO.Count()) };

        const auto CREATURE_RANK_AVG { (CREATURE_RANK_RANGE.A().GetAs<double>()
                                        + CREATURE_RANK_RANGE.B().GetAs<double>())
                                       * 0.5 };

        score_ += Score_t::Make(std::sqrt((CREATURE_RANK_AVG * SUMMON_COUNT_D) * 150.0));
    }

    const std::string ItemProfile::ToString() const
    {
        std::string str;
        str.reserve(128);

        if (category::None != category_)
        {
            str += "category=";
            str += EnumUtil<category>::ToString(category_, EnumStringHow(Wrap::Yes));
        }

        if (!str.empty())
        {
            str += ", ";
        }

        str += thinProfile_.ToString();

        if (isPixie_)
        {
            str += ", is_pixie=";
            str += misc::ToString(isPixie_);
        }

        const auto ROLE_RESTRICTION { RoleRestriction() };

        if (ROLE_RESTRICTION != creature::role::Count)
        {
            str += ", role_restriction=";
            str += NAMEOF_ENUM(ROLE_RESTRICTION);
        }

        str += ", mat_pri=";
        str += NAMEOF_ENUM(matPri_);

        if (material::Nothing != matSec_)
        {
            str += ", mat_sec=";
            str += NAMEOF_ENUM(matSec_);
        }

        if (set_type::Not != set_)
        {
            str += ", set_type=";
            str += ((set_type::Count == set_) ? "Count" : NAMEOF_ENUM(set_));
        }

        if (named_type::Not != named_)
        {
            str += ", named_type=";
            str += ((named_type::Count == named_) ? "Count" : NAMEOF_ENUM(named_));
        }

        if (element_type::None != element_)
        {
            str += ", element_type=";
            str += EnumUtil<element_type>::ToString(element_, EnumStringHow(Wrap::Yes));
        }

        if (summonInfo_.CanSummon())
        {
            str += ", summonInfo=";
            str += summonInfo_.ToString();
        }
        else if (summonInfo_.IsDefaultAndCompletelyInvalid() == false)
        {
            str += ", summonInfo=";
            str += summonInfo_.ToString();
            str += "(but CanSummon()=false?)";
        }

        return str;
    }

    void ItemProfile::SetMisc(
        const misc_type::Enum MISC_TYPE,
        const bool IS_PIXIE,
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const set_type::Enum SET_TYPE,
        const element_type::Enum ELEMENT_TYPE,
        const creature::SummonInfo & SUMMON_INFO)
    {
        SetMisc(
            ItemProfileThinFactory::MakeMisc(MISC_TYPE),
            IS_PIXIE,
            MATERIAL_PRIMARY,
            MATERIAL_SECONDARY,
            SET_TYPE,
            ELEMENT_TYPE,
            SUMMON_INFO);
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
        const auto MISC_TYPE { THIN_PROFILE.MiscType() };

        element_ = ELEMENT_TYPE;

        if (MISC_TYPE == misc_type::MagnifyingGlass)
        {
            category_ = static_cast<category::Enum>(category_ | category::ShowsEnemyInfo);
        }

        if (misc_type::IsUseable(MISC_TYPE))
        {
            category_ = static_cast<category::Enum>(category_ | category::Useable);
        }

        category_ = static_cast<category::Enum>(category_ | misc_type::EquipCategory(MISC_TYPE));

        thinProfile_ = THIN_PROFILE;
        matPri_ = MATERIAL_PRIMARY;
        matSec_ = MATERIAL_SECONDARY;
        isPixie_ = IS_PIXIE;

        if (misc::IsRealZero(religiousRatio_))
        {
            religiousRatio_ = misc_type::ReligiousRatio(MISC_TYPE);
        }

        item::ScoreHelper scoreHelper;
        score_ += scoreHelper.Score(MATERIAL_PRIMARY, MATERIAL_SECONDARY);

        score_ += creature::EnchantmentFactory::TreasureScore(SET_TYPE);

        score_ += creature::EnchantmentFactory::TreasureScore(
            MISC_TYPE, MATERIAL_PRIMARY, MATERIAL_SECONDARY);

        if (misc_type::IsUnique(MISC_TYPE))
        {
            score_ += 200_score;
        }

        if (ELEMENT_TYPE != element_type::None)
        {
            M_HP_ASSERT_OR_LOG_AND_THROW(
                ((misc_type::EquipCategory(MISC_TYPE) & category::Equipable) > 0),
                "item::ItemProfile::SetMisc(misc_type="
                    << NAMEOF_ENUM(MISC_TYPE) << ", is_pixie=" << std::boolalpha << IS_PIXIE
                    << ", mat_pri=" << NAMEOF_ENUM(MATERIAL_PRIMARY)
                    << ", mat_sec=" << NAMEOF_ENUM(MATERIAL_SECONDARY) << ", set_type="
                    << ((SET_TYPE == set_type::Count) ? "Count" : NAMEOF_ENUM(SET_TYPE))
                    << ", element_type="
                    << EnumUtil<element_type>::ToString(ELEMENT_TYPE, EnumStringHow(Wrap::Yes))
                    << ") element_type wasn't None but the misc_type was not equipable.");

            score_ += creature::EnchantmentFactory::TreasureScore(
                ELEMENT_TYPE, misc_type::IsWeapon(MISC_TYPE), MATERIAL_PRIMARY, MATERIAL_SECONDARY);
        }

        if ((MISC_TYPE == misc_type::CapeCommanders) || (MISC_TYPE == misc_type::CapeGenerals)
            || (MISC_TYPE == misc_type::CapeKings))
        {
            SetCover(
                armor::cover_type::Cape,
                MATERIAL_PRIMARY,
                MATERIAL_SECONDARY,
                named_type::Not,
                SET_TYPE,
                ELEMENT_TYPE,
                IS_PIXIE,
                MISC_TYPE);
        }
        else if (
            (MISC_TYPE == misc_type::ShadeCloak) || (MISC_TYPE == misc_type::GhostSheet)
            || (MISC_TYPE == misc_type::BurialShroud))
        {
            SetCover(
                armor::cover_type::Cloak,
                MATERIAL_PRIMARY,
                MATERIAL_SECONDARY,
                named_type::Not,
                SET_TYPE,
                ELEMENT_TYPE,
                IS_PIXIE,
                MISC_TYPE);
        }
        else if (MISC_TYPE == misc_type::SpecterRobe)
        {
            SetCover(
                armor::cover_type::Robe,
                MATERIAL_PRIMARY,
                MATERIAL_SECONDARY,
                named_type::Not,
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
                named_type::Not,
                SET_TYPE,
                ELEMENT_TYPE,
                MISC_TYPE,
                IS_PIXIE);
        }
        else if (MISC_TYPE == misc_type::ReaperScythe)
        {
            SetBladedStaff(
                weapon::bladedstaff_type::Scythe,
                MATERIAL_PRIMARY,
                MATERIAL_SECONDARY,
                named_type::Not,
                SET_TYPE,
                ELEMENT_TYPE,
                MISC_TYPE);
        }

        if (SUMMON_INFO.CanSummon())
        {
            SetSummonInfoAndAdjustScore(SUMMON_INFO);
        }
    }

    void ItemProfile::SetShield(
        const armor::shield_type::Enum SHIELD_TYPE,
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE,
        const element_type::Enum ELEMENT_TYPE)
    {
        SetShield(
            ItemProfileThinFactory::MakeArmorSpecific(SHIELD_TYPE),
            SHIELD_TYPE,
            MATERIAL_PRIMARY,
            MATERIAL_SECONDARY,
            NAMED_TYPE,
            SET_TYPE,
            ELEMENT_TYPE);
    }

    void ItemProfile::SetHelm(
        const armor::helm_type::Enum HELM_TYPE,
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE,
        const element_type::Enum ELEMENT_TYPE)
    {
        SetHelm(
            ItemProfileThinFactory::MakeArmorSpecific(HELM_TYPE),
            HELM_TYPE,
            MATERIAL_PRIMARY,
            MATERIAL_SECONDARY,
            NAMED_TYPE,
            SET_TYPE,
            ELEMENT_TYPE);
    }

    void ItemProfile::SetCover(
        const armor::cover_type::Enum COVER_TYPE,
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE,
        const element_type::Enum ELEMENT_TYPE,
        const bool IS_PIXIE,
        const misc_type::Enum MISC_TYPE)
    {
        SetCover(
            ItemProfileThinFactory::MakeArmorSpecific(COVER_TYPE, MISC_TYPE),
            COVER_TYPE,
            MATERIAL_PRIMARY,
            MATERIAL_SECONDARY,
            NAMED_TYPE,
            SET_TYPE,
            ELEMENT_TYPE,
            IS_PIXIE,
            MISC_TYPE);
    }

    void ItemProfile::SetAventail(
        const armor::base_type::Enum BASE_TYPE,
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE,
        const element_type::Enum ELEMENT_TYPE)
    {
        SetArmorWithBaseTypeHelper(
            ItemProfileThinFactory::MakeArmorNonSpecific(armor_type::Aventail, BASE_TYPE),
            MATERIAL_PRIMARY,
            MATERIAL_SECONDARY,
            NAMED_TYPE,
            SET_TYPE,
            ELEMENT_TYPE,
            false);
    }

    void ItemProfile::SetBracer(
        const armor::base_type::Enum BASE_TYPE,
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE,
        const element_type::Enum ELEMENT_TYPE,
        const bool IS_PIXIE)
    {
        SetArmorWithBaseTypeHelper(
            ItemProfileThinFactory::MakeArmorNonSpecific(armor_type::Bracers, BASE_TYPE),
            MATERIAL_PRIMARY,
            MATERIAL_SECONDARY,
            NAMED_TYPE,
            SET_TYPE,
            ELEMENT_TYPE,
            IS_PIXIE);
    }

    void ItemProfile::SetShirt(
        const armor::base_type::Enum BASE_TYPE,
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE,
        const element_type::Enum ELEMENT_TYPE,
        const bool IS_PIXIE)
    {
        SetArmorWithBaseTypeHelper(
            ItemProfileThinFactory::MakeArmorNonSpecific(armor_type::Shirt, BASE_TYPE),
            MATERIAL_PRIMARY,
            MATERIAL_SECONDARY,
            NAMED_TYPE,
            SET_TYPE,
            ELEMENT_TYPE,
            IS_PIXIE);
    }

    void ItemProfile::SetBoots(
        const armor::base_type::Enum BASE_TYPE,
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE,
        const element_type::Enum ELEMENT_TYPE,
        const bool IS_PIXIE)
    {
        SetArmorWithBaseTypeHelper(
            ItemProfileThinFactory::MakeArmorNonSpecific(armor_type::Boots, BASE_TYPE),
            MATERIAL_PRIMARY,
            MATERIAL_SECONDARY,
            NAMED_TYPE,
            SET_TYPE,
            ELEMENT_TYPE,
            IS_PIXIE);
    }

    void ItemProfile::SetPants(
        const armor::base_type::Enum BASE_TYPE,
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE,
        const element_type::Enum ELEMENT_TYPE,
        const bool IS_PIXIE)
    {
        SetArmorWithBaseTypeHelper(
            ItemProfileThinFactory::MakeArmorNonSpecific(armor_type::Pants, BASE_TYPE),
            MATERIAL_PRIMARY,
            MATERIAL_SECONDARY,
            NAMED_TYPE,
            SET_TYPE,
            ELEMENT_TYPE,
            IS_PIXIE);
    }

    void ItemProfile::SetGauntlets(
        const armor::base_type::Enum BASE_TYPE,
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE,
        const element_type::Enum ELEMENT_TYPE,
        const bool IS_PIXIE)
    {
        SetArmorWithBaseTypeHelper(
            ItemProfileThinFactory::MakeArmorNonSpecific(armor_type::Gauntlets, BASE_TYPE),
            MATERIAL_PRIMARY,
            MATERIAL_SECONDARY,
            NAMED_TYPE,
            SET_TYPE,
            ELEMENT_TYPE,
            IS_PIXIE);
    }

    void ItemProfile::SetSword(
        const weapon::sword_type::Enum SWORD_TYPE,
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE,
        const element_type::Enum ELEMENT_TYPE)
    {
        SetSword(
            ItemProfileThinFactory::MakeWeaponSpecific(SWORD_TYPE),
            SWORD_TYPE,
            MATERIAL_PRIMARY,
            MATERIAL_SECONDARY,
            NAMED_TYPE,
            SET_TYPE,
            ELEMENT_TYPE);
    }

    void ItemProfile::SetAxe(
        const weapon::axe_type::Enum AXE_TYPE,
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE,
        const element_type::Enum ELEMENT_TYPE)
    {
        SetAxe(
            ItemProfileThinFactory::MakeWeaponSpecific(AXE_TYPE),
            AXE_TYPE,
            MATERIAL_PRIMARY,
            MATERIAL_SECONDARY,
            NAMED_TYPE,
            SET_TYPE,
            ELEMENT_TYPE);
    }

    void ItemProfile::SetClub(
        const weapon::club_type::Enum CLUB_TYPE,
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE,
        const element_type::Enum ELEMENT_TYPE)
    {
        SetClub(
            ItemProfileThinFactory::MakeWeaponSpecific(CLUB_TYPE),
            CLUB_TYPE,
            MATERIAL_PRIMARY,
            MATERIAL_SECONDARY,
            NAMED_TYPE,
            SET_TYPE,
            ELEMENT_TYPE);
    }

    void ItemProfile::SetWhip(
        const weapon::whip_type::Enum WHIP_TYPE,
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE,
        const element_type::Enum ELEMENT_TYPE)
    {
        SetWhip(
            ItemProfileThinFactory::MakeWeaponSpecific(WHIP_TYPE),
            WHIP_TYPE,
            MATERIAL_PRIMARY,
            MATERIAL_SECONDARY,
            NAMED_TYPE,
            SET_TYPE,
            ELEMENT_TYPE);
    }

    void ItemProfile::SetProjectile(
        const weapon::projectile_type::Enum PROJECTILE_TYPE,
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE,
        const element_type::Enum ELEMENT_TYPE)
    {
        SetProjectile(
            ItemProfileThinFactory::MakeWeaponSpecific(PROJECTILE_TYPE),
            PROJECTILE_TYPE,
            MATERIAL_PRIMARY,
            MATERIAL_SECONDARY,
            NAMED_TYPE,
            SET_TYPE,
            ELEMENT_TYPE);
    }

    void ItemProfile::SetKnife(
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE,
        const element_type::Enum ELEMENT_TYPE,
        const bool IS_PIXIE)
    {
        SetKnife(
            ItemProfileThinFactory::MakeWeaponKnifeOrDagger(false),
            MATERIAL_PRIMARY,
            MATERIAL_SECONDARY,
            NAMED_TYPE,
            SET_TYPE,
            ELEMENT_TYPE,
            IS_PIXIE);
    }

    void ItemProfile::SetDagger(
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE,
        const element_type::Enum ELEMENT_TYPE,
        const bool IS_PIXIE)
    {
        SetDagger(
            ItemProfileThinFactory::MakeWeaponKnifeOrDagger(true),
            MATERIAL_PRIMARY,
            MATERIAL_SECONDARY,
            NAMED_TYPE,
            SET_TYPE,
            ELEMENT_TYPE,
            IS_PIXIE);
    }

    void ItemProfile::SetQuarterStaff(
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE,
        const element_type::Enum ELEMENT_TYPE)
    {
        SetQuarterStaff(
            ItemProfileThinFactory::MakeWeaponStaffOrQuarterstaff(true),
            MATERIAL_PRIMARY,
            MATERIAL_SECONDARY,
            NAMED_TYPE,
            SET_TYPE,
            ELEMENT_TYPE);
    }

    void ItemProfile::SetStaff(
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE,
        const element_type::Enum ELEMENT_TYPE,
        const misc_type::Enum MISC_TYPE,
        const bool IS_PIXIE)
    {
        SetStaff(
            ItemProfileThinFactory::MakeWeaponStaffOrQuarterstaff(false, MISC_TYPE),
            MATERIAL_PRIMARY,
            MATERIAL_SECONDARY,
            NAMED_TYPE,
            SET_TYPE,
            ELEMENT_TYPE,
            MISC_TYPE,
            IS_PIXIE);
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
        SetBladedStaff(
            ItemProfileThinFactory::MakeWeaponSpecific(BLADEDSTAFF_TYPE, MISC_TYPE),
            BLADEDSTAFF_TYPE,
            MATERIAL_PRIMARY,
            MATERIAL_SECONDARY,
            NAMED_TYPE,
            SET_TYPE,
            ELEMENT_TYPE,
            MISC_TYPE);
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
        category_
            = static_cast<category::Enum>(category_ | category::Equipable | category::Wearable);
        thinProfile_ = THIN_PROFILE;

        SetHelperForWeaponsAndArmor(
            MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE, IS_PIXIE);

        item::ScoreHelper scoreHelper;
        score_ += scoreHelper.Score(
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
        const auto DETAILS { weapon::WeaponDetailLoader::LookupWeaponDetails(
            THIN_PROFILE.WeaponInfo().DetailsKeyName()) };

        category_
            = static_cast<category::Enum>(category_ | category::Equipable | DETAILS.handedness);

        thinProfile_ = THIN_PROFILE;

        if ((MISC_TYPE == misc_type::Not) || (MISC_TYPE == misc_type::Count))
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
            return static_cast<category::Enum>(category::Equipable | category::BodyPart);
        }
        else
        {
            return static_cast<category::Enum>(
                category::Equipable | category::TwoHanded | category::BodyPart);
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
            item::ScoreHelper scoreHelper;

            return (
                scoreHelper.Score(MATERIAL_PRI, MATERIAL_SEC)
                + creature::EnchantmentFactory::TreasureScore(
                    NAMED_TYPE, MATERIAL_PRI, MATERIAL_SEC, IS_WEAPON, !IS_WEAPON)
                + creature::EnchantmentFactory::TreasureScore(SET_TYPE)
                + creature::EnchantmentFactory::TreasureScore(
                    ELEMENT_TYPE, IS_WEAPON, MATERIAL_PRI, MATERIAL_SEC));
        }
    }

} // namespace item
} // namespace heroespath
