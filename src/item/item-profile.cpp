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

    const std::string MemberStrings::InvalidString() const
    {
        if (normalStrings.empty())
        {
            return "no normal info";
        }

        auto const MATERIAL_STRING_ITER{ std::find_if(
            normalStrings.begin(), normalStrings.end(), [](const auto & S) {
                return boost::algorithm::contains(S, "mat=");
            }) };

        if (MATERIAL_STRING_ITER == normalStrings.end())
        {
            return "missing materials";
        }

        auto const MATERIAL_STRINGS{ boost::algorithm::erase_all_copy(
            *MATERIAL_STRING_ITER, "mat=") };

        auto const COMMA_POS{ MATERIAL_STRINGS.find(',') };

        auto const MATERIAL_STRING_PRI{ MATERIAL_STRINGS.substr(0, COMMA_POS) };

        if ((MATERIAL_STRING_PRI == "Nothing") || (MATERIAL_STRING_PRI == "(count)"))
        {
            return "primary material is Nothing or Count";
        }

        auto const MATERIAL_STRING_SEC{ MATERIAL_STRINGS.substr(COMMA_POS + 1) };

        if (MATERIAL_STRING_PRI == MATERIAL_STRING_SEC)
        {
            return "primary and secondary materials are the same";
        }

        if (weaponStrings.size() > 1)
        {
            return "multiple base weapon types";
        }

        if (armorStrings.size() > 1)
        {
            return "multiple base armor types";
        }

        if (std::find_if(
                normalStrings.begin(),
                normalStrings.end(),
                [](const auto & S) { return boost::algorithm::contains(S, "category"); })
            == normalStrings.end())
        {
            return "no category";
        }

        auto const HAS_WEAPONTYPE{ (
            std::find_if(
                normalStrings.begin(),
                normalStrings.end(),
                [](const auto & S) { return boost::algorithm::contains(S, "weapon_type"); })
            != normalStrings.end()) };

        auto const HAS_ARMORTYPE{ (
            std::find_if(
                normalStrings.begin(),
                normalStrings.end(),
                [](const auto & S) { return boost::algorithm::contains(S, "armor_type"); })
            != normalStrings.end()) };

        if (HAS_WEAPONTYPE && HAS_ARMORTYPE)
        {
            return "is both weapon and armor";
        }

        if (HAS_WEAPONTYPE && weaponStrings.empty())
        {
            return "weapon type with no specific weapon set";
        }

        if (HAS_ARMORTYPE && armorStrings.empty())
        {
            return "armor type with no specific armor set";
        }

        if (HAS_WEAPONTYPE && (armorStrings.empty() == false))
        {
            return "weapon type with specific armor set";
        }

        if (HAS_ARMORTYPE && (weaponStrings.empty() == false))
        {
            return "armor type with specifc weapon set";
        }

        return "";
    }

    const std::string MemberStrings::ToString() const
    {
        std::ostringstream ss;

        auto streamToSS
            = [&ss](const auto & ARG) { ss << ((ss.str().empty()) ? "" : ", ") << ARG; };

        std::for_each(normalStrings.begin(), normalStrings.end(), streamToSS);
        std::for_each(weaponStrings.begin(), weaponStrings.end(), streamToSS);
        std::for_each(armorStrings.begin(), armorStrings.end(), streamToSS);

        return "[" + ss.str() + "]";
    }

    ItemProfile::ItemProfile()
        : score_(0)
        , religious_(0.0f)
        , category_(category::None)
        , armor_(armor_type::NotArmor)
        , weapon_(weapon_type::NotAWeapon)
        , unique_(unique_type::NotUnique)
        , misc_(misc_type::NotMisc)
        , set_(set_type::NotASet)
        , named_(named_type::NotNamed)
        , element_(element_type::None)
        , isSet_(profile::IsSet::None)
        , settings_()
        , size_(sfml_util::Size::Medium)
        , matPri_(material::Nothing)
        , matSec_(material::Nothing)
        , summonInfo_()
        , baseName_("")
    {}

    creature::role::Enum ItemProfile::RoleRestrictionBasedOnMiscAndSetType() const
    {
        if ((misc_ == misc_type::DrumLute) || (misc_ == misc_type::Lyre)
            || (misc_ == misc_type::Hurdy_Gurdy) || (misc_ == misc_type::Pipe_And_Tabor)
            || (misc_ == misc_type::Viol) || (misc_ == misc_type::Recorder))
        {
            return creature::role::Bard;
        }
        else
        {
            return set_type::Role(set_);
        }
    }

    void ItemProfile::SetSummoningAndAdjustScore(const creature::SummonInfo & SUMMON_INFO)
    {
        summonInfo_ = SUMMON_INFO;

        // Use a creature's rank min/max to establish a kind of combined power/worth/value
        // summon score, then append that to the summoning item's score.

        auto const CREATURE_RANK_RANGE{ creature::race::RaceRoleRanks(
            SUMMON_INFO.Race(), SUMMON_INFO.Role()) };

        auto const SUMMON_COUNT_D{ static_cast<double>(SUMMON_INFO.Count()) };

        auto const CREATURE_RANK_AVG{
            (CREATURE_RANK_RANGE.A().As<double>() + CREATURE_RANK_RANGE.B().As<double>()) * 0.5f
        };

        auto const SUMMON_SCORE{ static_cast<int>(
            std::sqrt((CREATURE_RANK_AVG * SUMMON_COUNT_D) * 150.0)) };

        score_ += Score_t(SUMMON_SCORE);
    }

    const MemberStrings ItemProfile::ToMemberStrings() const
    {
        misc::StrVec_t normalStrings;
        misc::StrVec_t weaponStrings;
        misc::StrVec_t armorStrings;

        if (baseName_.empty() == false)
        {
            normalStrings.emplace_back("base_name=" + baseName_);
        }

        if (category_ != 0)
        {
            normalStrings.emplace_back("category=" + category::ToString(category_, true));
        }

        if (armor_ != 0)
        {
            normalStrings.emplace_back("armor_type=" + armor_type::ToString(armor_, true));
        }

        if (weapon_ != 0)
        {
            normalStrings.emplace_back("weapon_type=" + weapon_type::ToString(weapon_, true));
        }

        if ((unique_ != unique_type::NotUnique) && (unique_ != unique_type::Count))
        {
            normalStrings.emplace_back("unique_type=" + unique_type::ToString(unique_));
        }

        if ((misc_ != misc_type::NotMisc) && (misc_ != misc_type::Count))
        {
            normalStrings.emplace_back("misc_type=" + misc_type::ToString(misc_));
        }

        if ((set_ != set_type::NotASet) && (set_ != set_type::Count))
        {
            normalStrings.emplace_back("set_type=" + set_type::ToString(set_));
        }

        if ((named_ != named_type::NotNamed) && (named_ != named_type::Count))
        {
            normalStrings.emplace_back("named_type=" + named_type::ToString(named_));
        }

        if (element_ != element_type::None)
        {
            normalStrings.emplace_back("element_type=" + element_type::ToString(element_));
        }

        if (IsArmor())
        {
            if (IsPixie())
            {
                normalStrings.emplace_back("(Pixie)");
            }

            if (settings_.armor.shield != armor::shield_type::Count)
            {
                armorStrings.emplace_back(
                    "shield_type=" + armor::shield_type::ToString(settings_.armor.shield));
            }

            if (settings_.armor.helm != armor::helm_type::Count)
            {
                armorStrings.emplace_back(
                    "helm_type=" + armor::helm_type::ToString(settings_.armor.helm));
            }

            if (settings_.armor.base != armor::base_type::Count)
            {
                normalStrings.emplace_back(
                    "base_type=" + armor::base_type::ToString(settings_.armor.base));
            }

            if (settings_.armor.cover != armor::cover_type::Count)
            {
                armorStrings.emplace_back(
                    "cover_type=" + armor::cover_type::ToString(settings_.armor.cover));
            }

            if (settings_.armor.restriction != armor::base_type::Count)
            {
                normalStrings.emplace_back(
                    "armor_type_restriction="
                    + armor::base_type::ToString(settings_.armor.restriction));
            }
        }

        if (IsAventail())
        {
            armorStrings.emplace_back("(Aventail)");
        }

        if (IsBracer())
        {
            armorStrings.emplace_back("(Bracer)");
        }

        if (IsShirt())
        {
            armorStrings.emplace_back("(Shirt)");
        }

        if (IsBoots())
        {
            armorStrings.emplace_back("(Boots)");
        }

        if (IsPants())
        {
            armorStrings.emplace_back("(Pants)");
        }

        if (IsGauntlets())
        {
            armorStrings.emplace_back("(Gauntlets)");
        }

        if (IsWeapon())
        {
            if (settings_.weapon.sword != weapon::sword_type::Count)
            {
                weaponStrings.emplace_back(
                    "sword_type=" + weapon::sword_type::ToString(settings_.weapon.sword));
            }

            if (settings_.weapon.axe != weapon::axe_type::Count)
            {
                weaponStrings.emplace_back(
                    "axe_type=" + weapon::axe_type::ToString(settings_.weapon.axe));
            }

            if (settings_.weapon.club != weapon::club_type::Count)
            {
                weaponStrings.emplace_back(
                    "club_type=" + weapon::club_type::ToString(settings_.weapon.club));
            }

            if (settings_.weapon.whip != weapon::whip_type::Count)
            {
                weaponStrings.emplace_back(
                    "whip_type=" + weapon::whip_type::ToString(settings_.weapon.whip));
            }

            if (settings_.weapon.proj != weapon::projectile_type::Count)
            {
                weaponStrings.emplace_back(
                    "proj_type=" + weapon::projectile_type::ToString(settings_.weapon.proj));
            }

            if (settings_.weapon.bstaff != weapon::bladedstaff_type::Count)
            {
                weaponStrings.emplace_back(
                    "bstaff_type=" + weapon::bladedstaff_type::ToString(settings_.weapon.bstaff));
            }
        }

        if (IsKnife())
        {
            weaponStrings.emplace_back("(Knife)");
        }

        if (IsDagger())
        {
            weaponStrings.emplace_back("(Dagger)");
        }

        if (IsStaff())
        {
            weaponStrings.emplace_back("(Staff)");
        }

        if (IsQuarterStaff())
        {
            weaponStrings.emplace_back("(QuarterStaff)");
        }

        if (size_ != sfml_util::Size::Medium)
        {
            normalStrings.emplace_back("size=" + sfml_util::Size::ToString(size_));
        }

        {
            std::ostringstream ss;

            ss << ((ss.str().empty()) ? "" : ", ")
               << "mat=" << ((matPri_ == material::Count) ? "(count)" : material::ToString(matPri_))
               << "," << ((matSec_ == material::Count) ? "(count)" : material::ToString(matSec_));

            normalStrings.emplace_back(ss.str());
        }

        if (score_.IsNonZero())
        {
            std::ostringstream ss;
            ss << "score=" << score_;
            normalStrings.emplace_back(ss.str());
        }

        if (summonInfo_.OriginType() != creature::origin_type::Count)
        {
            std::ostringstream ss;

            ss << ((ss.str().empty()) ? "" : ", ")
               << creature::origin_type::ToString(summonInfo_.OriginType()) << " summoning a "
               << creature::race::RaceRoleName(summonInfo_.Race(), summonInfo_.Role());

            normalStrings.emplace_back(ss.str());
        }

        if (religious_ > 0.0f)
        {
            std::ostringstream ss;
            ss << ((ss.str().empty()) ? "" : ", ") << "religious_ratio=" << religious_;
            normalStrings.emplace_back(ss.str());
        }

        return MemberStrings(normalStrings, weaponStrings, armorStrings);
    }

    void ItemProfile::SetUnique(
        const unique_type::Enum E,
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const element_type::Enum ELEMENT_TYPE)
    {
        unique_ = E;
        matPri_ = MATERIAL_PRIMARY;
        matSec_ = MATERIAL_SECONDARY;
        misc_ = unique_type::MiscType(E);
        SetFlag(false, profile::IsSet::Pixie);
        religious_ = unique_type::ReligiousRatio(E);
        element_ = ELEMENT_TYPE;

        auto const IS_WEAPON{ (
            (E == unique_type::ViperFangFingerclaw) || (E == unique_type::ScorpionStingerFingerclaw)
            || (E == unique_type::RazorFingerclaw)) };

        score_
            = (ItemProfileWarehouse::Score(MATERIAL_PRIMARY, MATERIAL_SECONDARY)
               + creature::EnchantmentFactory::Instance()->TreasureScore(E, MATERIAL_PRIMARY)
               + creature::EnchantmentFactory::Instance()->TreasureScore(
                     ELEMENT_TYPE, IS_WEAPON, MATERIAL_PRIMARY));

        score_ += 750_score;
    }

    void ItemProfile::SetMisc(
        const misc_type::Enum E,
        const bool IS_PIXIE,
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const set_type::Enum SET_TYPE)
    {
        using namespace item;

        misc_ = E;
        matPri_ = MATERIAL_PRIMARY;
        matSec_ = MATERIAL_SECONDARY;
        SetFlag(IS_PIXIE, profile::IsSet::Pixie);

        if (misc::IsRealZero(religious_))
        {
            religious_ = misc_type::ReligiousRatio(E);
        }

        if (0_score == score_)
        {
            score_ += ItemProfileWarehouse::Score(MATERIAL_PRIMARY, MATERIAL_SECONDARY);
        }

        score_ += creature::EnchantmentFactory::Instance()->TreasureScore(SET_TYPE);

        if (E == misc_type::Cape)
        {
            category_ = CategoryArmor();
            armor_ = armor_type::Covering;
            settings_.armor = profile::ArmorSettings();
            settings_.armor.cover = armor::cover_type::Cape;
            score_ += ItemProfileWarehouse::Score(armor::cover_type::Cape);
        }
        else if (E == misc_type::Cloak)
        {
            category_ = CategoryArmor();
            armor_ = armor_type::Covering;
            settings_.armor = profile::ArmorSettings();
            settings_.armor.cover = armor::cover_type::Cloak;
            score_ += ItemProfileWarehouse::Score(armor::cover_type::Cloak);
        }
        else if (E == misc_type::LockPicks)
        {
            category_ = static_cast<category::Enum>(category_ | category::Equippable);
            score_ += Score_t(200);
        }
        else if (
            (E == misc_type::Fingerclaw) || (E == misc_type::Amulet) || (E == misc_type::Armband)
            || (E == misc_type::Veil) || (E == misc_type::Litch_Hand) || (E == misc_type::Braid))
        {
            if ((E == misc_type::Litch_Hand) || (E == misc_type::Braid))
            {
                category_ = static_cast<category::Enum>(category_ | category::QuestItem);
            }

            category_ = static_cast<category::Enum>(category_ | category::Equippable);
        }
        else if (E == misc_type::Spider_Eggs)
        {
            category_ = static_cast<category::Enum>(category_ | category::Useable);
            score_ += 500_score;
        }
        else if (E == misc_type::Summoning_Statue)
        {
            category_ = static_cast<category::Enum>(category_ | category::Useable);
            score_ += 100_score;
        }
        else if (
            (E == misc_type::Wand) || (E == misc_type::Petrified_Snake)
            || (E == misc_type::Mummy_Hand) || (E == misc_type::Shard) || (E == misc_type::Orb)
            || (E == misc_type::Scepter) || (E == misc_type::Icicle) || (E == misc_type::Finger)
            || (E == misc_type::Unicorn_Horn) || (E == misc_type::Devil_Horn)
            || (E == misc_type::Staff))
        {
            category_ = static_cast<category::Enum>(
                category_ | category::AllowsCasting | category::Equippable);

            if ((E == misc_type::Petrified_Snake) || (E == misc_type::Mummy_Hand)
                || (E == misc_type::Finger) || (E == misc_type::Unicorn_Horn)
                || (E == misc_type::Devil_Horn) || (E == misc_type::Icicle))
            {
                category_ = static_cast<category::Enum>(category_ | category::QuestItem);
            }

            score_ += 100_score;

            if (E == misc_type::Staff)
            {
                SetFlag(true, profile::IsSet::Staff);
                category_ = CategoryWeaponStaff(false);
                weapon_ = static_cast<weapon_type::Enum>(weapon_type::Staff | weapon_type::Melee);
            }
        }
        else if (
            (E == misc_type::DrumLute) || (E == misc_type::Lyre) || (E == misc_type::Hurdy_Gurdy)
            || (E == misc_type::Pipe_And_Tabor) || (E == misc_type::Viol)
            || (E == misc_type::Recorder))
        {
            category_ = static_cast<category::Enum>(category_ | category::Equippable);

            if ((E == misc_type::Lyre) || (E == misc_type::Hurdy_Gurdy)
                || (E == misc_type::Pipe_And_Tabor) || (E == misc_type::Viol)
                || (E == misc_type::Recorder))
            {
                category_ = static_cast<category::Enum>(category_ | category::QuestItem);
            }

            score_ += 100_score;
        }
        else if (
            (E == misc_type::Bust) || (E == misc_type::Figurine_Blessed)
            || (E == misc_type::Doll_Blessed) || (E == misc_type::Puppet_Blessed))
        {
            category_ = static_cast<category::Enum>(category_ | category::Blessed);
            score_ += 50_score;

            score_ += creature::EnchantmentFactory::Instance()->TreasureScore(
                E, MATERIAL_PRIMARY, MATERIAL_SECONDARY);
        }
        else if (
            (E == misc_type::Dried_Head) || (E == misc_type::Figurine_Cursed)
            || (E == misc_type::Doll_Cursed) || (E == misc_type::Puppet_Cursed))
        {
            category_ = static_cast<category::Enum>(category_ | category::Cursed);
            score_ += 50_score;

            score_ += creature::EnchantmentFactory::Instance()->TreasureScore(
                E, MATERIAL_PRIMARY, MATERIAL_SECONDARY);
        }
        else if (
            (E == misc_type::Goblet) || (E == misc_type::Balm_Pot) || (E == misc_type::Egg)
            || (E == misc_type::Embryo) || (E == misc_type::Seeds))
        {
            category_ = static_cast<category::Enum>(category_ | category::Useable);
            score_ += 150_score;
        }
        else if (E == misc_type::Ring)
        {
            category_ = static_cast<category::Enum>(
                category_ | category::Equippable | category::Wearable);

            score_ += 20_score;
        }
    }

    void ItemProfile::SetShield(
        const armor::shield_type::Enum E,
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE,
        const element_type::Enum ELEMENT_TYPE)
    {
        settings_.armor = profile::ArmorSettings();
        settings_.armor.shield = E;

        category_ = static_cast<category::Enum>(CategoryArmor() | category::OneHanded);

        armor_ = armor_type::Sheild;

        SetHelper(MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE);

        score_ = ItemProfileWarehouse::Score(E);

        score_ += ScoreHelper(
            MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE, false);
    }

    void ItemProfile::SetHelm(
        const armor::helm_type::Enum E,
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE,
        const element_type::Enum ELEMENT_TYPE)
    {
        settings_.armor = profile::ArmorSettings();
        settings_.armor.helm = E;

        category_ = CategoryArmor();

        armor_ = armor_type::Helm;

        SetHelper(MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE);

        score_ = ItemProfileWarehouse::Score(E);

        score_ += ScoreHelper(
            MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE, false);
    }

    void ItemProfile::SetCover(
        const armor::cover_type::Enum E,
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE,
        const element_type::Enum ELEMENT_TYPE,
        const bool IS_PIXIE)
    {
        settings_.armor = profile::ArmorSettings();
        settings_.armor.cover = E;

        category_ = CategoryArmor();

        armor_ = armor_type::Covering;

        SetHelper(
            MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE, IS_PIXIE);

        score_ = ItemProfileWarehouse::Score(E);

        score_ += ScoreHelper(
            MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE, false);
    }

    void ItemProfile::SetAventail(
        const armor::base_type::Enum E,
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE,
        const element_type::Enum ELEMENT_TYPE)
    {
        SetFlag(true, profile::IsSet::Aventail);
        settings_.armor = profile::ArmorSettings();
        settings_.armor.base = E;

        category_ = CategoryArmor();

        armor_ = armor_type::Aventail;

        SetHelper(MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE);

        score_ = ItemProfileWarehouse::Score(E);

        score_ += ScoreHelper(
            MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE, false);
    }

    void ItemProfile::SetBracer(
        const armor::base_type::Enum E,
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE,
        const element_type::Enum ELEMENT_TYPE,
        const bool IS_PIXIE)
    {
        SetFlag(true, profile::IsSet::Bracer);
        settings_.armor = profile::ArmorSettings();
        settings_.armor.base = E;

        category_ = CategoryArmor();

        armor_ = armor_type::Bracer;

        SetHelper(
            MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE, IS_PIXIE);

        score_ = ItemProfileWarehouse::Score(E);

        score_ += ScoreHelper(
            MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE, false);
    }

    void ItemProfile::SetShirt(
        const armor::base_type::Enum E,
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE,
        const element_type::Enum ELEMENT_TYPE,
        const bool IS_PIXIE)
    {
        SetFlag(true, profile::IsSet::Shirt);
        settings_.armor = profile::ArmorSettings();
        settings_.armor.base = E;

        category_ = CategoryArmor();

        armor_ = armor_type::Shirt;

        SetHelper(
            MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE, IS_PIXIE);

        score_ = ItemProfileWarehouse::Score(E);

        score_ += ScoreHelper(
            MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE, false);
    }

    void ItemProfile::SetBoots(
        const armor::base_type::Enum E,
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE,
        const element_type::Enum ELEMENT_TYPE,
        const bool IS_PIXIE)
    {
        SetFlag(true, profile::IsSet::Boots);
        settings_.armor = profile::ArmorSettings();
        settings_.armor.base = E;

        category_ = CategoryArmor();

        armor_ = armor_type::Boots;

        SetHelper(
            MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE, IS_PIXIE);

        score_ = ItemProfileWarehouse::Score(E);

        score_ += ScoreHelper(
            MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE, false);
    }

    void ItemProfile::SetPants(
        const armor::base_type::Enum E,
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE,
        const element_type::Enum ELEMENT_TYPE,
        const bool IS_PIXIE)
    {
        SetFlag(true, profile::IsSet::Pants);
        settings_.armor = profile::ArmorSettings();
        settings_.armor.base = E;

        category_ = CategoryArmor();

        armor_ = armor_type::Pants;

        SetHelper(
            MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE, IS_PIXIE);

        score_ = ItemProfileWarehouse::Score(E);

        score_ += ScoreHelper(
            MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE, false);
    }

    void ItemProfile::SetGauntlets(
        const armor::base_type::Enum E,
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE,
        const element_type::Enum ELEMENT_TYPE,
        const bool IS_PIXIE)
    {
        SetFlag(true, profile::IsSet::Gauntlets);
        settings_.armor = profile::ArmorSettings();
        settings_.armor.base = E;

        category_ = CategoryArmor();

        armor_ = armor_type::Gauntlets;

        SetHelper(
            MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE, IS_PIXIE);

        score_ = ItemProfileWarehouse::Score(E);

        score_ += ScoreHelper(
            MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE, false);
    }

    void ItemProfile::SetSword(
        const weapon::sword_type::Enum E,
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE,
        const element_type::Enum ELEMENT_TYPE)
    {
        using namespace item;
        using namespace weapon;

        settings_.weapon = profile::WeaponSettings();
        settings_.weapon.sword = E;

        category_ = CategoryWeapon<weapon::sword_type>(E);

        weapon_ = static_cast<weapon_type::Enum>(
            weapon_type::Melee | weapon_type::Sword | weapon_type::Bladed | weapon_type::Pointed);

        SetHelper(MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE);

        score_ = ItemProfileWarehouse::Score(E);

        score_ += ScoreHelper(
            MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE, true);
    }

    void ItemProfile::SetAxe(
        const weapon::axe_type::Enum E,
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE,
        const element_type::Enum ELEMENT_TYPE)
    {
        using namespace weapon;

        settings_.weapon = profile::WeaponSettings();
        settings_.weapon.axe = E;

        category_ = CategoryWeapon<weapon::axe_type>(E);

        weapon_ = static_cast<weapon_type::Enum>(
            weapon_type::Bladed | weapon_type::Axe | weapon_type::Melee);

        SetHelper(MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE);

        score_ = ItemProfileWarehouse::Score(E);

        score_ += ScoreHelper(
            MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE, true);
    }

    void ItemProfile::SetClub(
        const weapon::club_type::Enum E,
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE,
        const element_type::Enum ELEMENT_TYPE)
    {
        using namespace weapon;

        settings_.weapon = profile::WeaponSettings();
        settings_.weapon.club = E;

        category_ = CategoryWeapon<weapon::club_type>(E);

        weapon_ = static_cast<weapon_type::Enum>(weapon_type::Melee | weapon_type::Club);

        SetHelper(MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE);

        score_ = ItemProfileWarehouse::Score(E);

        score_ += ScoreHelper(
            MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE, true);
    }

    void ItemProfile::SetWhip(
        const weapon::whip_type::Enum E,
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE,
        const element_type::Enum ELEMENT_TYPE)
    {
        using namespace weapon;

        settings_.weapon = profile::WeaponSettings();
        settings_.weapon.whip = E;

        category_ = CategoryWeapon<weapon::whip_type>(E);

        weapon_ = static_cast<weapon_type::Enum>(weapon_type::Melee | weapon_type::Whip);

        SetHelper(MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE);

        score_ = ItemProfileWarehouse::Score(E);

        score_ += ScoreHelper(
            MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE, true);
    }

    void ItemProfile::SetProjectile(
        const weapon::projectile_type::Enum E,
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE,
        const element_type::Enum ELEMENT_TYPE)
    {
        using namespace item;
        using namespace weapon;

        settings_.weapon = profile::WeaponSettings();
        settings_.weapon.proj = E;

        category_ = CategoryWeapon<weapon::projectile_type>(E);

        auto const WEAPON_TYPE{ [E]() {
            if (E == projectile_type::Blowpipe)
            {
                return weapon_type::Blowpipe;
            }
            else if (E == projectile_type::Sling)
            {
                return weapon_type::Sling;
            }
            else if (E == projectile_type::Crossbow)
            {
                return weapon_type::Crossbow;
            }
            else
            {
                return weapon_type::Bow;
            }
        }() };

        weapon_ = static_cast<weapon_type::Enum>(weapon_type::Projectile | WEAPON_TYPE);

        SetHelper(MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE);

        score_ = ItemProfileWarehouse::Score(E);

        score_ += ScoreHelper(
            MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE, true);
    }

    void ItemProfile::SetBladedStaff(
        const weapon::bladedstaff_type::Enum E,
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE,
        const element_type::Enum ELEMENT_TYPE)
    {
        using namespace item;
        using namespace weapon;

        settings_.weapon = profile::WeaponSettings();
        settings_.weapon.bstaff = E;

        category_ = CategoryWeapon<weapon::bladedstaff_type>(E);

        auto const POINTED_TYPE{ (
            (E == bladedstaff_type::Scythe) ? weapon_type::NotAWeapon : weapon_type::Pointed) };

        auto const SPEAR_TYPE{ (
            ((E == bladedstaff_type::Spear) || (E == bladedstaff_type::ShortSpear))
                ? weapon_type::Spear
                : weapon_type::NotAWeapon) };

        weapon_ = static_cast<weapon_type::Enum>(
            weapon_type::BladedStaff | weapon_type::Melee | POINTED_TYPE | SPEAR_TYPE);

        SetHelper(MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE);

        score_ = ItemProfileWarehouse::Score(E);

        score_ += ScoreHelper(
            MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE, true);
    }

    void ItemProfile::SetKnife(
        const sfml_util::Size::Enum E,
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE,
        const element_type::Enum ELEMENT_TYPE,
        const bool IS_PIXIE)
    {
        SetFlag(true, profile::IsSet::Knife);
        size_ = E;

        category_ = static_cast<category::Enum>(
            category::Weapon | category::Equippable | category::OneHanded);

        weapon_ = static_cast<weapon_type::Enum>(
            weapon_type::Bladed | weapon_type::Knife | weapon_type::Melee | weapon_type::Pointed);

        SetHelper(
            MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE, IS_PIXIE);

        score_ = ItemProfileWarehouse::ScoreKnife(E);

        score_ += ScoreHelper(
            MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE, true);
    }

    void ItemProfile::SetDagger(
        const sfml_util::Size::Enum E,
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE,
        const element_type::Enum ELEMENT_TYPE,
        const bool IS_PIXIE)
    {
        SetFlag(true, profile::IsSet::Dagger);
        size_ = E;

        category_ = static_cast<category::Enum>(
            category::Weapon | category::Equippable | category::OneHanded);

        weapon_ = static_cast<weapon_type::Enum>(
            weapon_type::Bladed | weapon_type::Knife | weapon_type::Melee | weapon_type::Pointed);

        SetHelper(
            MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE, IS_PIXIE);

        score_ = ItemProfileWarehouse::ScoreDagger(E);

        score_ += ScoreHelper(
            MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE, true);
    }

    void ItemProfile::SetStaff(
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE,
        const element_type::Enum ELEMENT_TYPE)
    {
        SetFlag(true, profile::IsSet::Staff);

        category_ = CategoryWeaponStaff(false);

        weapon_ = static_cast<weapon_type::Enum>(weapon_type::Staff | weapon_type::Melee);

        SetHelper(MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE);

        score_ = ItemProfileWarehouse::ScoreStaff();

        score_ += ScoreHelper(
            MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE, true);
    }

    void ItemProfile::SetQuarterStaff(
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE,
        const element_type::Enum ELEMENT_TYPE)
    {
        SetFlag(true, profile::IsSet::QStaff);

        category_ = CategoryWeaponStaff(true);

        weapon_ = static_cast<weapon_type::Enum>(weapon_type::Staff | weapon_type::Melee);

        SetHelper(MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE);

        score_ = ItemProfileWarehouse::ScoreQuarterStaff();

        score_ += ScoreHelper(
            MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE, true);
    }

    category::Enum ItemProfile::CategoryWeaponBodypart(const body_part::Enum BODY_PART)
    {
        if ((BODY_PART == body_part::Bite) || (BODY_PART == body_part::Breath))
        {
            return static_cast<category::Enum>(
                category::Weapon | category::Equippable | category::BodyPart);
        }
        else
        {
            return static_cast<category::Enum>(
                category::Weapon | category::Equippable | category::TwoHanded | category::BodyPart);
        }
    }

    void ItemProfile::SetHelper(
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
        SetFlag(IS_PIXIE, profile::IsSet::Pixie);
    }

    Score_t ItemProfile::ScoreHelper(
        const material::Enum MATERIAL_PRI,
        const material::Enum MATERIAL_SEC,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE,
        const element_type::Enum ELEMENT_TYPE,
        const bool IS_WEAPON) const
    {
        if ((MATERIAL_PRI == material::Nothing) || (MATERIAL_PRI == material::Count))
        {
            return 0_score;
        }
        else
        {
            return (
                ItemProfileWarehouse::Score(MATERIAL_PRI, MATERIAL_SEC)
                + creature::EnchantmentFactory::Instance()->TreasureScore(
                      NAMED_TYPE, MATERIAL_PRI, IS_WEAPON, !IS_WEAPON)
                + creature::EnchantmentFactory::Instance()->TreasureScore(SET_TYPE)
                + creature::EnchantmentFactory::Instance()->TreasureScore(
                      ELEMENT_TYPE, IS_WEAPON, MATERIAL_PRI));
        }
    }

} // namespace item
} // namespace heroespath
