// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// item-profile-warehouse.cpp
//
#include "item-profile-warehouse.hpp"

#include "creature/race-enum.hpp"
#include "creature/role-enum.hpp"
#include "creature/traits-set.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/log-macros.hpp"
#include "misc/vectors.hpp"

#include <SFML/System/Clock.hpp>

#include <algorithm>
#include <exception>
#include <sstream>
#include <string>
#include <vector>

namespace heroespath
{
namespace item
{

    std::unique_ptr<ItemProfileWarehouse> ItemProfileWarehouse::instanceUPtr_;

    ItemProfileWarehouse::ItemProfileWarehouse()
        : materialFactory_()
        , thinProfileFactory_()
        , profiles_()
        , religiousProfiles_()
        , questItemProfilesMap_()
        , hasInitialized_(false)
    {
        M_HP_LOG_DBG("Subsystem Construction: ItemProfileWarehouse");
        questItemProfilesMap_.Reserve(1024);
    }

    ItemProfileWarehouse::~ItemProfileWarehouse()
    {
        M_HP_LOG_DBG("Subsystem Destruction: ItemProfileWarehouse");
    }

    misc::NotNull<ItemProfileWarehouse *> ItemProfileWarehouse::Instance()
    {
        if (!instanceUPtr_)
        {
            M_HP_LOG_ERR(
                "Subsystem Instance() called but instanceUPtr_ was null: ItemProfileWarehouse");
            Acquire();
        }

        return misc::NotNull<ItemProfileWarehouse *>(instanceUPtr_.get());
    }

    void ItemProfileWarehouse::Acquire()
    {
        if (!instanceUPtr_)
        {
            instanceUPtr_ = std::make_unique<ItemProfileWarehouse>();
        }
        else
        {
            M_HP_LOG_ERR("Subsystem Acquire() after Construction: ItemProfileWarehouse");
        }
    }

    void ItemProfileWarehouse::Release()
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (instanceUPtr_),
            "creature::ItemProfileWarehouse::Release() found instanceUPtr that was null.");

        instanceUPtr_.reset();
    }

    void ItemProfileWarehouse::Initialize()
    {
        M_HP_LOG_DBG("ItemProfileWarehouse::Initialize() start");
        sf::Clock totalClock;

        profiles_.clear();

        // As of 2017-8-17 there were 849299 raw item profiles created before cleanup.
        // As of 2018-3-17 there were 465091 profiles with no duplicates to cleanup.
        // As of 2018-4-28 457935
        // As of 2018-5-2 236585 (reduced the number of element enchantments)
        // As oF 2018-5-8 59370
        // 2018-5-17 44057 (fixed a bug in materials factory that was allowing invalid materials)
        profiles_.reserve(45000);

        // As of 2018-3-19 there were 251 religious profiles.
        // As of 2018-4-28 107
        // As of 2018-5-2 392
        // As of 2018-5-8 313
        // 2018-5-17 317
        religiousProfiles_.reserve(320);

        sf::Clock jobClock;

        auto logJobDuration = [&](const std::string & NAME) {
            M_HP_LOG_DBG(
                "ItemProfileWarehouse::Initialize() Job \""
                << NAME << "\" took " << jobClock.getElapsedTime().asMilliseconds() << "ms.");
            jobClock.restart();
        };

        Setup_StandardEquipment();
        logJobDuration("Setup_StandardEquipment");

        Setup_MiscItems();
        logJobDuration("Setup_MiscItems");

        Setup_NamedEquipment();
        logJobDuration("Setup_NamedEquipment");

        Setup_SetEquipment();
        logJobDuration("Setup_SetEquipment");

        Setup_SummoningItems();
        logJobDuration("Setup_SummoningItems");

        profiles_.shrink_to_fit();
        religiousProfiles_.shrink_to_fit();
        logJobDuration("shrink_to_fit");

        // correct operation of the Treasure Stage requires sorting by PROFILE.TreasureScore()
        // see operator< in ItemProfile.hpp

        std::sort(std::begin(profiles_), std::end(profiles_));
        logJobDuration("sort normal profiles");

        std::sort(std::begin(religiousProfiles_), std::end(religiousProfiles_));
        logJobDuration("sort religious profiles");

        hasInitialized_ = true;

        M_HP_LOG_DBG(
            "ItemProfileWarehouse::Initialize() end, duration="
            << totalClock.getElapsedTime().asMilliseconds() << "ms, normal_profile_count="
            << profiles_.size() << ", religious_profile_count=" << religiousProfiles_.size());
    }

    void ItemProfileWarehouse::EnsureInitialized()
    {
        if (!hasInitialized_)
        {
            Initialize();
        }
    }

    void ItemProfileWarehouse::Setup_StandardEquipment()
    {
        for (const auto & WEAPON_THIN_PROFILE : thinProfileFactory_.MakeAllWeapons())
        {
            MakeLoopOverMaterialsAndElementsForEquipment(WEAPON_THIN_PROFILE);
        }

        for (const auto & ARMOR_THIN_PROFILE : thinProfileFactory_.MakeAllArmor())
        {
            MakeLoopOverMaterialsAndElementsForEquipment(ARMOR_THIN_PROFILE);
        }
    }

    void ItemProfileWarehouse::Setup_MiscItems()
    {
        for (EnumUnderlying_t i(1); i < misc_type::Count; ++i)
        {
            const auto MISC_TYPE { static_cast<misc_type::Enum>(i) };

            if (misc_type::IsSummoning(MISC_TYPE) == false)
            {
                for (const auto ELEMENT_TYPE : misc_type::ElementTypes(MISC_TYPE, true))
                {
                    MakeLoopOverMaterialsForMisc(MISC_TYPE, ELEMENT_TYPE);
                }
            }
        }
    }

    void ItemProfileWarehouse::Setup_NamedEquipment()
    {
        for (EnumUnderlying_t i(1); i < named_type::Count; ++i)
        {
            const auto NAMED_TYPE { static_cast<named_type::Enum>(i) };

            for (const auto & NAMED_THINPROFILE : thinProfileFactory_.MakeAllNamedTypes(NAMED_TYPE))
            {
                MakeLoopOverMaterialsAndElementsForEquipment(
                    NAMED_THINPROFILE, NAMED_TYPE, set_type::Not);
            }
        }
    }

    void ItemProfileWarehouse::Setup_SetEquipment()
    {
        for (EnumUnderlying_t i(1); i < set_type::Count; ++i)
        {
            const auto SET_TYPE { static_cast<set_type::Enum>(i) };

            for (const auto & SET_THINPROFILE : thinProfileFactory_.MakeAllSetTypes(SET_TYPE))
            {
                MakeLoopOverMaterialsAndElementsForEquipment(
                    SET_THINPROFILE, named_type::Not, SET_TYPE);
            }
        }
    }

    void ItemProfileWarehouse::Setup_SummoningItems()
    {
        for (EnumUnderlying_t raceIndex(0); raceIndex < creature::race::Count; ++raceIndex)
        {
            const auto RACE_TYPE { static_cast<creature::race::Enum>(raceIndex) };

            const auto ROLES_VEC { creature::race::Roles(RACE_TYPE) };

            for (const auto ROLE_TYPE : ROLES_VEC)
            {
                const auto ORIGINS_VEC { creature::race::OriginTypes(RACE_TYPE, ROLE_TYPE) };
                for (const auto ORIGIN_TYPE : ORIGINS_VEC)
                {
                    const auto SUMMON_INFO { creature::SummonInfo(
                        ORIGIN_TYPE, RACE_TYPE, ROLE_TYPE) };

                    if (ORIGIN_TYPE == creature::origin_type::Statue)
                    {
                        for (const auto ELEMENT_TYPE :
                             misc_type::ElementTypes(misc_type::SummoningStatue, true))
                        {
                            MakeLoopOverMaterialsForMisc(
                                misc_type::SummoningStatue, ELEMENT_TYPE, SUMMON_INFO);
                        }
                    }
                    else
                    {
                        const auto MISC_TYPE { [&]() {
                            if (RACE_TYPE == creature::race::Spider)
                            {
                                return misc_type::SpiderEggs;
                            }
                            else if (ORIGIN_TYPE == creature::origin_type::Egg)
                            {
                                return misc_type::Egg;
                            }
                            else if (ORIGIN_TYPE == creature::origin_type::Embryo)
                            {
                                return misc_type::Embryo;
                            }
                            else
                            {
                                return misc_type::Seeds;
                            }
                        }() };

                        for (const auto ELEMENT_TYPE : misc_type::ElementTypes(MISC_TYPE, true))
                        {
                            MakeLoopOverMaterialsForMisc(MISC_TYPE, ELEMENT_TYPE, SUMMON_INFO);
                        }
                    }
                }
            }
        }
    }

    const ElementEnumVec_t ItemProfileWarehouse::ElementTypesIncludingNone(
        const ItemProfileThin & THIN_PROFILE,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE)
    {
        // note that unique_type is not handled here, see Setup_UniqueItems()

        ElementEnumVec_t elementTypes;

        if ((SET_TYPE != set_type::Not) && (SET_TYPE != set_type::Count))
        {
            elementTypes = { element_type::None };
        }
        else if ((NAMED_TYPE != named_type::Not) && (NAMED_TYPE != named_type::Count))
        {
            elementTypes = named_type::ElementTypes(NAMED_TYPE, true);
        }
        else if (
            (THIN_PROFILE.MiscType() != misc_type::Not)
            && (THIN_PROFILE.MiscType() != misc_type::Count))
        {
            elementTypes = misc_type::ElementTypes(THIN_PROFILE.MiscType(), true);
        }
        else if (THIN_PROFILE.IsArmor())
        {
            elementTypes = THIN_PROFILE.ArmorInfo().ElementTypes();
        }
        else if (THIN_PROFILE.IsWeapon())
        {
            elementTypes = THIN_PROFILE.WeaponInfo().ElementTypes();
        }

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (elementTypes.empty() == false),
            "item::ItemProfileWarehouse::ElementTypesIncludingNone(thin_profile={"
                << THIN_PROFILE.ToString() << "}, named_type="
                << ((NAMED_TYPE == named_type::Count) ? "Count" : named_type::ToString(NAMED_TYPE))
                << ", set_type="
                << ((SET_TYPE == set_type::Count) ? "Count" : set_type::ToString(SET_TYPE))
                << ") resulted in an elementType vector that was empty.");

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (std::find(std::begin(elementTypes), std::end(elementTypes), element_type::None)
             != std::end(elementTypes)),
            "item::ItemProfileWarehouse::ElementTypesIncludingNone(thin_profile={"
                << THIN_PROFILE.ToString() << "}, named_type="
                << ((NAMED_TYPE == named_type::Count) ? "Count" : named_type::ToString(NAMED_TYPE))
                << ", set_type="
                << ((SET_TYPE == set_type::Count) ? "Count" : set_type::ToString(SET_TYPE))
                << ") resulted in an elementType vector that did not contain "
                   "element_type::None.");

        return elementTypes;
    }

    void ItemProfileWarehouse::MakeLoopOverMaterialsAndElementsForEquipment(
        const ItemProfileThin & THIN_PROFILE,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE)
    {
        for (const auto & MATERIAL_PAIR :
             materialFactory_.MakeForEquipment(THIN_PROFILE, NAMED_TYPE, SET_TYPE))
        {
            for (const auto ELEMENT_TYPE :
                 ElementTypesIncludingNone(THIN_PROFILE, NAMED_TYPE, SET_TYPE))
            {
                MakeForEquipment(
                    THIN_PROFILE,
                    NAMED_TYPE,
                    SET_TYPE,
                    ELEMENT_TYPE,
                    MATERIAL_PAIR.first,
                    MATERIAL_PAIR.second);
            }
        }
    }

    void ItemProfileWarehouse::MakeForEquipment(
        const ItemProfileThin & THIN_PROFILE,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE,
        const element_type::Enum ELEMENT_TYPE,
        const material::Enum MATERIAL_PRI,
        const material::Enum MATERIAL_SEC)
    {
        if (MATERIAL_PRI == MATERIAL_SEC)
        {
            return;
        }

        ItemProfile profile;
        ItemProfile profilePixie;

        if (THIN_PROFILE.IsMisc())
        {
            if (misc_type::HasOnlyPixieVersion(THIN_PROFILE.MiscType()) == false)
            {
                profile.SetMisc(
                    THIN_PROFILE, false, MATERIAL_PRI, MATERIAL_SEC, SET_TYPE, ELEMENT_TYPE);

                AppendToCorrectProfileCollection(profile);
            }

            if (misc_type::HasPixieVersion(THIN_PROFILE.MiscType()))
            {
                profilePixie.SetMisc(
                    THIN_PROFILE, true, MATERIAL_PRI, MATERIAL_SEC, SET_TYPE, ELEMENT_TYPE);

                AppendToCorrectProfileCollection(profilePixie);
            }

            return;
        }

        if (THIN_PROFILE.IsArmor())
        {
            // can't use THIN_PROFILE.ArmorInfo().Is_() functions here because some thin profiles
            // are missing their base_types, so use THIN_PROFILE.ArmorInfo().Type() instead which is
            // always valid
            switch (THIN_PROFILE.ArmorInfo().Type())
            {
                case armor_type::Shield:
                {
                    profile.SetShield(
                        THIN_PROFILE,
                        THIN_PROFILE.ArmorInfo().ShieldType(),
                        MATERIAL_PRI,
                        MATERIAL_SEC,
                        NAMED_TYPE,
                        SET_TYPE,
                        ELEMENT_TYPE);

                    break;
                }
                case armor_type::Helm:
                {
                    profile.SetHelm(
                        THIN_PROFILE,
                        THIN_PROFILE.ArmorInfo().HelmType(),
                        MATERIAL_PRI,
                        MATERIAL_SEC,
                        NAMED_TYPE,
                        SET_TYPE,
                        ELEMENT_TYPE);

                    break;
                }
                case armor_type::Gauntlets:
                {
                    profile.SetGauntlets(
                        THIN_PROFILE,
                        MATERIAL_PRI,
                        MATERIAL_SEC,
                        NAMED_TYPE,
                        SET_TYPE,
                        ELEMENT_TYPE,
                        false);

                    profilePixie.SetGauntlets(
                        THIN_PROFILE,
                        MATERIAL_PRI,
                        MATERIAL_SEC,
                        NAMED_TYPE,
                        SET_TYPE,
                        ELEMENT_TYPE,
                        true);

                    break;
                }
                case armor_type::Pants:
                {
                    profile.SetPants(
                        THIN_PROFILE,
                        MATERIAL_PRI,
                        MATERIAL_SEC,
                        NAMED_TYPE,
                        SET_TYPE,
                        ELEMENT_TYPE,
                        false);

                    profilePixie.SetPants(
                        THIN_PROFILE,
                        MATERIAL_PRI,
                        MATERIAL_SEC,
                        NAMED_TYPE,
                        SET_TYPE,
                        ELEMENT_TYPE,
                        true);

                    break;
                }
                case armor_type::Boots:
                {
                    profile.SetBoots(
                        THIN_PROFILE,
                        MATERIAL_PRI,
                        MATERIAL_SEC,
                        NAMED_TYPE,
                        SET_TYPE,
                        ELEMENT_TYPE,
                        false);

                    profilePixie.SetBoots(
                        THIN_PROFILE,
                        MATERIAL_PRI,
                        MATERIAL_SEC,
                        NAMED_TYPE,
                        SET_TYPE,
                        ELEMENT_TYPE,
                        true);

                    break;
                }
                case armor_type::Shirt:
                {
                    profile.SetShirt(
                        THIN_PROFILE,
                        MATERIAL_PRI,
                        MATERIAL_SEC,
                        NAMED_TYPE,
                        SET_TYPE,
                        ELEMENT_TYPE,
                        false);

                    profilePixie.SetShirt(
                        THIN_PROFILE,
                        MATERIAL_PRI,
                        MATERIAL_SEC,
                        NAMED_TYPE,
                        SET_TYPE,
                        ELEMENT_TYPE,
                        true);

                    break;
                }
                case armor_type::Bracers:
                {
                    profile.SetBracer(
                        THIN_PROFILE,
                        MATERIAL_PRI,
                        MATERIAL_SEC,
                        NAMED_TYPE,
                        SET_TYPE,
                        ELEMENT_TYPE,
                        false);

                    profilePixie.SetBracer(
                        THIN_PROFILE,
                        MATERIAL_PRI,
                        MATERIAL_SEC,
                        NAMED_TYPE,
                        SET_TYPE,
                        ELEMENT_TYPE,
                        true);

                    break;
                }
                case armor_type::Aventail:
                {
                    profile.SetAventail(
                        THIN_PROFILE,
                        MATERIAL_PRI,
                        MATERIAL_SEC,
                        NAMED_TYPE,
                        SET_TYPE,
                        ELEMENT_TYPE);

                    break;
                }
                case armor_type::Covering:
                {
                    profile.SetCover(
                        THIN_PROFILE,
                        THIN_PROFILE.ArmorInfo().CoverType(),
                        MATERIAL_PRI,
                        MATERIAL_SEC,
                        NAMED_TYPE,
                        SET_TYPE,
                        ELEMENT_TYPE,
                        false);

                    profilePixie.SetCover(
                        THIN_PROFILE,
                        THIN_PROFILE.ArmorInfo().CoverType(),
                        MATERIAL_PRI,
                        MATERIAL_SEC,
                        NAMED_TYPE,
                        SET_TYPE,
                        ELEMENT_TYPE,
                        true);

                    break;
                }
                case armor_type::Skin:
                case armor_type::Not:
                case armor_type::Count:
                default: { break;
                }
            }

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (profile.IsArmor()),
                "ItemProfileWarehouse::MakeForEquipment(thin_profile="
                    << THIN_PROFILE.ToString() << ", named_type="
                    << ((NAMED_TYPE == named_type::Count) ? "Count"
                                                          : named_type::ToString(NAMED_TYPE))
                    << ", set_type="
                    << ((SET_TYPE == set_type::Count) ? "Count" : set_type::ToString(SET_TYPE))
                    << ", element_type=" << element_type::ToString(ELEMENT_TYPE)
                    << ", mat_pri=" << material::ToString(MATERIAL_PRI)
                    << ", mat_sec=" << material::ToString(MATERIAL_SEC)
                    << ") was unable to find the type of the given armor.");

            AppendToCorrectProfileCollection(profile);

            // only knights can equip plate armor, and knights can't be pixies...
            if (profilePixie.IsArmor()
                && (profilePixie.ArmorInfo().BaseType() != armor::base_type::Plate))
            {
                AppendToCorrectProfileCollection(profilePixie);
            }

            return;
        }

        if (THIN_PROFILE.IsWeapon())
        {
            if (THIN_PROFILE.WeaponInfo().IsSword())
            {
                profile.SetSword(
                    THIN_PROFILE,
                    THIN_PROFILE.WeaponInfo().SwordType(),
                    MATERIAL_PRI,
                    MATERIAL_SEC,
                    NAMED_TYPE,
                    SET_TYPE,
                    ELEMENT_TYPE);
            }
            else if (THIN_PROFILE.WeaponInfo().IsAxe())
            {
                profile.SetAxe(
                    THIN_PROFILE,
                    THIN_PROFILE.WeaponInfo().AxeType(),
                    MATERIAL_PRI,
                    MATERIAL_SEC,
                    NAMED_TYPE,
                    SET_TYPE,
                    ELEMENT_TYPE);
            }
            else if (THIN_PROFILE.WeaponInfo().IsClub())
            {
                profile.SetClub(
                    THIN_PROFILE,
                    THIN_PROFILE.WeaponInfo().ClubType(),
                    MATERIAL_PRI,
                    MATERIAL_SEC,
                    NAMED_TYPE,
                    SET_TYPE,
                    ELEMENT_TYPE);
            }
            else if (THIN_PROFILE.WeaponInfo().IsWhip())
            {
                profile.SetWhip(
                    THIN_PROFILE,
                    THIN_PROFILE.WeaponInfo().WhipType(),
                    MATERIAL_PRI,
                    MATERIAL_SEC,
                    NAMED_TYPE,
                    SET_TYPE,
                    ELEMENT_TYPE);
            }
            else if (THIN_PROFILE.WeaponInfo().IsProjectile())
            {
                profile.SetProjectile(
                    THIN_PROFILE,
                    THIN_PROFILE.WeaponInfo().ProjectileType(),
                    MATERIAL_PRI,
                    MATERIAL_SEC,
                    NAMED_TYPE,
                    SET_TYPE,
                    ELEMENT_TYPE);
            }
            else if (THIN_PROFILE.WeaponInfo().IsBladedStaff())
            {
                profile.SetBladedStaff(
                    THIN_PROFILE,
                    THIN_PROFILE.WeaponInfo().BladedStaffType(),
                    MATERIAL_PRI,
                    MATERIAL_SEC,
                    NAMED_TYPE,
                    SET_TYPE,
                    ELEMENT_TYPE);
            }
            else if (THIN_PROFILE.WeaponInfo().IsKnife())
            {
                profile.SetKnife(
                    THIN_PROFILE,
                    MATERIAL_PRI,
                    MATERIAL_SEC,
                    NAMED_TYPE,
                    SET_TYPE,
                    ELEMENT_TYPE,
                    false);

                profilePixie.SetKnife(
                    THIN_PROFILE,
                    MATERIAL_PRI,
                    MATERIAL_SEC,
                    NAMED_TYPE,
                    SET_TYPE,
                    ELEMENT_TYPE,
                    true);
            }
            else if (THIN_PROFILE.WeaponInfo().IsDagger())
            {
                profile.SetDagger(
                    THIN_PROFILE,
                    MATERIAL_PRI,
                    MATERIAL_SEC,
                    NAMED_TYPE,
                    SET_TYPE,
                    ELEMENT_TYPE,
                    false);

                profilePixie.SetDagger(
                    THIN_PROFILE,
                    MATERIAL_PRI,
                    MATERIAL_SEC,
                    NAMED_TYPE,
                    SET_TYPE,
                    ELEMENT_TYPE,
                    true);
            }
            else if (THIN_PROFILE.WeaponInfo().IsStaff())
            {
                profile.SetStaff(
                    THIN_PROFILE,
                    MATERIAL_PRI,
                    MATERIAL_SEC,
                    NAMED_TYPE,
                    SET_TYPE,
                    ELEMENT_TYPE,
                    misc_type::Not,
                    false);

                profilePixie.SetStaff(
                    THIN_PROFILE,
                    MATERIAL_PRI,
                    MATERIAL_SEC,
                    NAMED_TYPE,
                    SET_TYPE,
                    ELEMENT_TYPE,
                    misc_type::Not,
                    true);
            }
            else if (THIN_PROFILE.WeaponInfo().IsQuarterstaff())
            {
                profile.SetQuarterStaff(
                    THIN_PROFILE, MATERIAL_PRI, MATERIAL_SEC, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE);
            }

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (profile.IsWeapon()),
                "ItemProfileWarehouse::MakeForEquipment(thin_profile="
                    << THIN_PROFILE.ToString() << ", named_type="
                    << ((NAMED_TYPE == named_type::Count) ? "Count"
                                                          : named_type::ToString(NAMED_TYPE))
                    << ", set_type="
                    << ((SET_TYPE == set_type::Count) ? "Count" : set_type::ToString(SET_TYPE))
                    << ", element_type=" << element_type::ToString(ELEMENT_TYPE)
                    << ", mat_pri=" << material::ToString(MATERIAL_PRI)
                    << ", mat_sec=" << material::ToString(MATERIAL_SEC)
                    << ") was unable to find the type of the given weapon.");

            AppendToCorrectProfileCollection(profile);

            if (profilePixie.IsWeapon())
            {
                AppendToCorrectProfileCollection(profilePixie);
            }

            return;
        }

        std::ostringstream ss;
        ss << "ItemProfileWarehouse::MakeForEquipment(thin_profile=" << THIN_PROFILE.ToString()
           << ", named_type="
           << ((NAMED_TYPE == named_type::Count) ? "Count" : named_type::ToString(NAMED_TYPE))
           << ", set_type="
           << ((SET_TYPE == set_type::Count) ? "Count" : set_type::ToString(SET_TYPE))
           << ", element_type=" << element_type::ToString(ELEMENT_TYPE)
           << ", mat_pri=" << material::ToString(MATERIAL_PRI)
           << ", mat_sec=" << material::ToString(MATERIAL_SEC)
           << ") was unable to find the type of the given item profile.";

        throw std::runtime_error(ss.str());
    }

    void ItemProfileWarehouse::MakeLoopOverMaterialsForMisc(
        const misc_type::Enum MISC_TYPE,
        const element_type::Enum ELEMENT_TYPE,
        const creature::SummonInfo & SUMMON_INFO)
    {
        const auto IS_MAGICAL { (ELEMENT_TYPE != element_type::None) || SUMMON_INFO.CanSummon()
                                || misc_type::IsUnique(MISC_TYPE)
                                || misc_type::IsQuestItem(MISC_TYPE) };

        for (const auto & MATERIAL_PAIR : materialFactory_.MakeForMiscType(MISC_TYPE, IS_MAGICAL))
        {
            if (misc_type::HasOnlyPixieVersion(MISC_TYPE) == false)
            {
                ItemProfile profile;

                profile.SetMisc(
                    MISC_TYPE,
                    false,
                    MATERIAL_PAIR.first,
                    MATERIAL_PAIR.second,
                    set_type::Not,
                    ELEMENT_TYPE,
                    SUMMON_INFO);

                AppendToCorrectProfileCollection(profile);
            }

            if (misc_type::HasPixieVersion(MISC_TYPE))
            {
                ItemProfile profilePixie;

                profilePixie.SetMisc(
                    MISC_TYPE,
                    true,
                    MATERIAL_PAIR.first,
                    MATERIAL_PAIR.second,
                    set_type::Not,
                    ELEMENT_TYPE,
                    SUMMON_INFO);

                AppendToCorrectProfileCollection(profilePixie);
            }
        }
    }

} // namespace item
} // namespace heroespath
