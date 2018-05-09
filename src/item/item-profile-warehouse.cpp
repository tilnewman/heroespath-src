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
// item-profile-warehouse.cpp
//
#include "item-profile-warehouse.hpp"

#include "creature/race-enum.hpp"
#include "creature/role-enum.hpp"
#include "log/log-macros.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/vectors.hpp"
#include "sfml-util/sfml-util.hpp"
#include "stats/traits-set.hpp"

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
    {
        M_HP_LOG_DBG("Subsystem Construction: ItemProfileWarehouse");
    }

    ItemProfileWarehouse::~ItemProfileWarehouse()
    {
        M_HP_LOG_DBG("Subsystem Destruction: ItemProfileWarehouse");
    }

    misc::NotNull<ItemProfileWarehouse *> ItemProfileWarehouse::Instance()
    {
        if (!instanceUPtr_)
        {
            M_HP_LOG_ERR("Subsystem Instance() before Acquire(): ItemProfileWarehouse");
            Acquire();
        }

        return instanceUPtr_.get();
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
        M_ASSERT_OR_LOGANDTHROW_SS(
            (instanceUPtr_),
            "creature::ItemProfileWarehouse::Release() found instanceUPtr that was null.");

        instanceUPtr_.reset();
    }

    void ItemProfileWarehouse::Setup()
    {
        profiles_.clear();

        // As of 2017-8-17 there were 849299 raw item profiles created before cleanup.
        // As of 2018-3-17 there were 465091 profiles with no duplicates to cleanup.
        // As of 2018-4-28 457935
        // As of 2018-5-2 236585 (reduced the number of element enchantments)
        profiles_.reserve(240000);

        // As of 2018-3-19 there were 251 religious profiles.
        // As of 2018-4-28 107
        // As of 2018-5-2 392
        religiousProfiles_.reserve(512);

        Setup_StandardEquipment();
        Setup_UniqueItems();
        Setup_MiscItems();
        Setup_NamedEquipment();
        Setup_SetEquipment();
        Setup_SummoningItems();

        profiles_.shrink_to_fit();
        religiousProfiles_.shrink_to_fit();

        // correct operation of the Treasure Stage requires sorting by PROFILE.TreasureScore()
        // see operator< in ItemProfile.hpp
        std::sort(std::begin(profiles_), std::end(profiles_));
        std::sort(std::begin(religiousProfiles_), std::end(religiousProfiles_));
    }

    void ItemProfileWarehouse::Setup_StandardEquipment()
    {
        for (auto const & WEAPON_THIN_PROFILE : thinProfileFactory_.MakeAllWeapons())
        {
            MakeLoopOverMaterialsAndElementsForEquipment(WEAPON_THIN_PROFILE);
        }

        for (auto const & ARMOR_THIN_PROFILE : thinProfileFactory_.MakeAllArmor())
        {
            MakeLoopOverMaterialsAndElementsForEquipment(ARMOR_THIN_PROFILE);
        }
    }

    void ItemProfileWarehouse::Setup_UniqueItems()
    {
        for (int i(1); i < unique_type::Count; ++i)
        {
            auto const UNIQUE_TYPE{ static_cast<unique_type::Enum>(i) };

            // currently there are only a few unique_type capes that are "of honor", but there are
            // some, so this loop on element type is required
            for (auto const ELEMENT_TYPE : unique_type::ElementTypes(UNIQUE_TYPE, true))
            {
                MakeLoopOverMaterialsForUnique(UNIQUE_TYPE, ELEMENT_TYPE);
            }
        }
    }

    void ItemProfileWarehouse::Setup_MiscItems()
    {
        for (int i(1); i < misc_type::Count; ++i)
        {
            auto const MISC_TYPE{ static_cast<misc_type::Enum>(i) };

            if (misc_type::IsStandalone(MISC_TYPE) && (misc_type::IsSummoning(MISC_TYPE) == false))
            {
                for (auto const ELEMENT_TYPE : misc_type::ElementTypes(MISC_TYPE, true))
                {
                    MakeLoopOverMaterialsForMisc(MISC_TYPE, ELEMENT_TYPE);
                }
            }
        }
    }

    void ItemProfileWarehouse::Setup_NamedEquipment()
    {
        for (int i(1); i < named_type::Count; ++i)
        {
            auto const NAMED_TYPE{ static_cast<named_type::Enum>(i) };

            for (auto const & NAMED_THINPROFILE : thinProfileFactory_.MakeAllNamedTypes(NAMED_TYPE))
            {
                MakeLoopOverMaterialsAndElementsForEquipment(
                    NAMED_THINPROFILE, NAMED_TYPE, set_type::NotASet);
            }
        }
    }

    void ItemProfileWarehouse::Setup_SetEquipment()
    {
        for (int i(1); i < set_type::Count; ++i)
        {
            auto const SET_TYPE{ static_cast<set_type::Enum>(i) };

            for (auto const & SET_THINPROFILE : thinProfileFactory_.MakeAllSetTypes(SET_TYPE))
            {
                MakeLoopOverMaterialsAndElementsForEquipment(
                    SET_THINPROFILE, named_type::NotNamed, SET_TYPE);
            }
        }
    }

    void ItemProfileWarehouse::Setup_SummoningItems()
    {
        using namespace creature;

        for (int raceIndex(0); raceIndex < race::Count; ++raceIndex)
        {
            auto const RACE_TYPE{ static_cast<race::Enum>(raceIndex) };

            auto const ROLES_VEC{ race::Roles(RACE_TYPE) };

            for (auto const ROLE_TYPE : ROLES_VEC)
            {
                auto const ORIGINS_VEC{ race::OriginTypes(RACE_TYPE, ROLE_TYPE) };
                for (auto const ORIGIN_TYPE : ORIGINS_VEC)
                {
                    auto const SUMMON_INFO{ SummonInfo(ORIGIN_TYPE, RACE_TYPE, ROLE_TYPE) };

                    if (ORIGIN_TYPE == origin_type::Statue)
                    {
                        for (auto const ELEMENT_TYPE :
                             misc_type::ElementTypes(misc_type::Summoning_Statue, true))
                        {
                            MakeLoopOverMaterialsForMisc(
                                misc_type::Summoning_Statue, ELEMENT_TYPE, SUMMON_INFO);
                        }
                    }
                    else
                    {
                        auto const MISC_TYPE{ [&]() {
                            if (RACE_TYPE == creature::race::Spider)
                            {
                                return misc_type::Spider_Eggs;
                            }
                            else if (ORIGIN_TYPE == origin_type::Egg)
                            {
                                return misc_type::Egg;
                            }
                            else if (ORIGIN_TYPE == origin_type::Embryo)
                            {
                                return misc_type::Embryo;
                            }
                            else
                            {
                                return misc_type::Seeds;
                            }
                        }() };

                        for (auto const ELEMENT_TYPE : misc_type::ElementTypes(MISC_TYPE, true))
                        {
                            MakeLoopOverMaterialsForMisc(MISC_TYPE, ELEMENT_TYPE, SUMMON_INFO);
                        }
                    }
                }
            }
        }
    }

    const ItemProfileVec_t & ItemProfileWarehouse::GetNormalProfiles()
    {
        // TODO when done with testing put this at system startup or in a seperate thread during
        // intro stage
        if (IsSetup() == false)
        {
            Setup();
        }

        return profiles_;
    }

    const ItemProfileVec_t & ItemProfileWarehouse::GetReligiousProfiles()
    {
        // TODO when done with testing put this at system startup or in a seperate thread during
        // intro stage
        if (IsSetup() == false)
        {
            Setup();
        }

        return religiousProfiles_;
    }

    const ElementEnumVec_t ItemProfileWarehouse::ElementTypesIncludingNone(
        const ItemProfileThin & THIN_PROFILE,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE)
    {
        // note that unique_type is not handled here, see Setup_UniqueItems()

        ElementEnumVec_t elementTypes;

        if ((SET_TYPE != set_type::NotASet) && (SET_TYPE != set_type::Count))
        {
            elementTypes = { element_type::None };
        }
        else if ((NAMED_TYPE != named_type::NotNamed) && (NAMED_TYPE != named_type::Count))
        {
            elementTypes = named_type::ElementTypes(NAMED_TYPE, true);
        }
        else if (
            (THIN_PROFILE.MiscType() != misc_type::NotMisc)
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

        M_ASSERT_OR_LOGANDTHROW_SS(
            (elementTypes.empty() == false),
            "item::ItemProfileWarehouse::ElementTypesIncludingNone(thin_profile={"
                << THIN_PROFILE.ToString() << "}, named_type="
                << ((NAMED_TYPE == named_type::Count) ? "Count" : named_type::ToString(NAMED_TYPE))
                << ", set_type="
                << ((SET_TYPE == set_type::Count) ? "Count" : set_type::ToString(SET_TYPE))
                << ") resulted in an elementType vector that was empty.");

        M_ASSERT_OR_LOGANDTHROW_SS(
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
        for (auto const & MATERIAL_PAIR :
             materialFactory_.MakeForEquipment(THIN_PROFILE, NAMED_TYPE, SET_TYPE))
        {
            for (auto const ELEMENT_TYPE :
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

    void ItemProfileWarehouse::MakeLoopOverMaterialsForUnique(
        const unique_type::Enum UNIQUE_TYPE, const element_type::Enum ELEMENT_TYPE)
    {
        for (auto const & MATERIAL_PAIR : materialFactory_.MakeForUniqueType(UNIQUE_TYPE))
        {
            auto const MISC_TYPE{ unique_type::MiscType(UNIQUE_TYPE) };

            if (misc_type::HasOnlyPixieVersion(MISC_TYPE) == false)
            {
                ItemProfile profile;

                profile.SetUniqueThenSetMisc(
                    UNIQUE_TYPE,
                    MISC_TYPE,
                    MATERIAL_PAIR.first,
                    MATERIAL_PAIR.second,
                    ELEMENT_TYPE,
                    false);

                AppendToEitherNormalOrReligiousVector(profile);
            }

            if (misc_type::HasPixieVersion(MISC_TYPE))
            {
                ItemProfile pixieProfile;

                pixieProfile.SetUniqueThenSetMisc(
                    UNIQUE_TYPE,
                    MISC_TYPE,
                    MATERIAL_PAIR.first,
                    MATERIAL_PAIR.second,
                    ELEMENT_TYPE,
                    true);

                AppendToEitherNormalOrReligiousVector(pixieProfile);
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

        if (THIN_PROFILE.IsMisc())
        {
            if (misc_type::HasOnlyPixieVersion(THIN_PROFILE.MiscType()) == false)
            {
                ItemProfile profile;

                profile.SetMisc(
                    THIN_PROFILE, false, MATERIAL_PRI, MATERIAL_SEC, SET_TYPE, ELEMENT_TYPE);

                AppendToEitherNormalOrReligiousVector(profile);
            }

            if (misc_type::HasPixieVersion(THIN_PROFILE.MiscType()))
            {
                ItemProfile pixieProfile;

                pixieProfile.SetMisc(
                    THIN_PROFILE, true, MATERIAL_PRI, MATERIAL_SEC, SET_TYPE, ELEMENT_TYPE);

                AppendToEitherNormalOrReligiousVector(pixieProfile);
            }

            return;
        }
        else if (THIN_PROFILE.IsArmor())
        {
            ItemProfile profile;
            ItemProfile profilePixie;

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
                case armor_type::NotArmor:
                case armor_type::Count:
                default:
                {
                    break;
                }
            }

            M_ASSERT_OR_LOGANDTHROW_SS(
                (profile.ArmorType() != armor_type::NotArmor),
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

            AppendToEitherNormalOrReligiousVector(profile);

            if (profilePixie.ArmorType() != armor_type::NotArmor)
            {
                AppendToEitherNormalOrReligiousVector(profilePixie);
            }

            return;
        }
        else if (THIN_PROFILE.IsWeapon())
        {
            if (THIN_PROFILE.WeaponInfo().IsSword())
            {
                ItemProfile profile;

                profile.SetSword(
                    THIN_PROFILE,
                    THIN_PROFILE.WeaponInfo().SwordType(),
                    MATERIAL_PRI,
                    MATERIAL_SEC,
                    NAMED_TYPE,
                    SET_TYPE,
                    ELEMENT_TYPE);

                AppendToEitherNormalOrReligiousVector(profile);
                return;
            }
            else if (THIN_PROFILE.WeaponInfo().IsAxe())
            {
                ItemProfile profile;

                profile.SetAxe(
                    THIN_PROFILE,
                    THIN_PROFILE.WeaponInfo().AxeType(),
                    MATERIAL_PRI,
                    MATERIAL_SEC,
                    NAMED_TYPE,
                    SET_TYPE,
                    ELEMENT_TYPE);

                AppendToEitherNormalOrReligiousVector(profile);
                return;
            }
            else if (THIN_PROFILE.WeaponInfo().IsClub())
            {
                ItemProfile profile;

                profile.SetClub(
                    THIN_PROFILE,
                    THIN_PROFILE.WeaponInfo().ClubType(),
                    MATERIAL_PRI,
                    MATERIAL_SEC,
                    NAMED_TYPE,
                    SET_TYPE,
                    ELEMENT_TYPE);

                AppendToEitherNormalOrReligiousVector(profile);
                return;
            }
            else if (THIN_PROFILE.WeaponInfo().IsWhip())
            {
                ItemProfile profile;

                profile.SetWhip(
                    THIN_PROFILE,
                    THIN_PROFILE.WeaponInfo().WhipType(),
                    MATERIAL_PRI,
                    MATERIAL_SEC,
                    NAMED_TYPE,
                    SET_TYPE,
                    ELEMENT_TYPE);

                AppendToEitherNormalOrReligiousVector(profile);
                return;
            }
            else if (THIN_PROFILE.WeaponInfo().IsProjectile())
            {
                ItemProfile profile;

                profile.SetProjectile(
                    THIN_PROFILE,
                    THIN_PROFILE.WeaponInfo().ProjectileType(),
                    MATERIAL_PRI,
                    MATERIAL_SEC,
                    NAMED_TYPE,
                    SET_TYPE,
                    ELEMENT_TYPE);

                AppendToEitherNormalOrReligiousVector(profile);
                return;
            }
            else if (THIN_PROFILE.WeaponInfo().IsBladedStaff())
            {
                ItemProfile profile;

                profile.SetBladedStaff(
                    THIN_PROFILE,
                    THIN_PROFILE.WeaponInfo().BladedStaffType(),
                    MATERIAL_PRI,
                    MATERIAL_SEC,
                    NAMED_TYPE,
                    SET_TYPE,
                    ELEMENT_TYPE);

                AppendToEitherNormalOrReligiousVector(profile);
                return;
            }
            else if (THIN_PROFILE.WeaponInfo().IsKnife())
            {
                ItemProfile profile;

                profile.SetKnife(
                    THIN_PROFILE,
                    THIN_PROFILE.WeaponInfo().Size(),
                    MATERIAL_PRI,
                    MATERIAL_SEC,
                    NAMED_TYPE,
                    SET_TYPE,
                    ELEMENT_TYPE,
                    false);

                AppendToEitherNormalOrReligiousVector(profile);

                ItemProfile profilePixie;

                profilePixie.SetKnife(
                    THIN_PROFILE,
                    THIN_PROFILE.WeaponInfo().Size(),
                    MATERIAL_PRI,
                    MATERIAL_SEC,
                    NAMED_TYPE,
                    SET_TYPE,
                    ELEMENT_TYPE,
                    true);

                AppendToEitherNormalOrReligiousVector(profilePixie);

                return;
            }
            else if (THIN_PROFILE.WeaponInfo().IsDagger())
            {
                ItemProfile profile;

                profile.SetDagger(
                    THIN_PROFILE,
                    THIN_PROFILE.WeaponInfo().Size(),
                    MATERIAL_PRI,
                    MATERIAL_SEC,
                    NAMED_TYPE,
                    SET_TYPE,
                    ELEMENT_TYPE,
                    false);

                AppendToEitherNormalOrReligiousVector(profile);

                ItemProfile profilePixie;

                profilePixie.SetDagger(
                    THIN_PROFILE,
                    THIN_PROFILE.WeaponInfo().Size(),
                    MATERIAL_PRI,
                    MATERIAL_SEC,
                    NAMED_TYPE,
                    SET_TYPE,
                    ELEMENT_TYPE,
                    true);

                AppendToEitherNormalOrReligiousVector(profilePixie);

                return;
            }
            else if (THIN_PROFILE.WeaponInfo().IsStaff())
            {
                ItemProfile profile;

                profile.SetStaff(
                    THIN_PROFILE, MATERIAL_PRI, MATERIAL_SEC, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE);

                AppendToEitherNormalOrReligiousVector(profile);
                return;
            }
            else if (THIN_PROFILE.WeaponInfo().IsQuarterstaff())
            {
                ItemProfile profile;

                profile.SetQuarterStaff(
                    THIN_PROFILE, MATERIAL_PRI, MATERIAL_SEC, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE);

                AppendToEitherNormalOrReligiousVector(profile);
                return;
            }
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
           << ") was unable to find the type of the given weapon or armor.";

        throw std::runtime_error(ss.str());
    }

    void ItemProfileWarehouse::MakeLoopOverMaterialsForMisc(
        const misc_type::Enum MISC_TYPE,
        const element_type::Enum ELEMENT_TYPE,
        const creature::SummonInfo & SUMMON_INFO)
    {
        auto const IS_MAGICAL{ (ELEMENT_TYPE != element_type::None) || SUMMON_INFO.CanSummon() };

        for (auto const & MATERIAL_PAIR : materialFactory_.MakeForMiscType(MISC_TYPE, IS_MAGICAL))
        {
            if (misc_type::HasOnlyPixieVersion(MISC_TYPE) == false)
            {
                ItemProfile profile;

                profile.SetMisc(
                    MISC_TYPE,
                    false,
                    MATERIAL_PAIR.first,
                    MATERIAL_PAIR.second,
                    set_type::NotASet,
                    ELEMENT_TYPE,
                    SUMMON_INFO);

                AppendToEitherNormalOrReligiousVector(profile);
            }

            if (misc_type::HasPixieVersion(MISC_TYPE))
            {
                ItemProfile pixieProfile;

                pixieProfile.SetMisc(
                    MISC_TYPE,
                    true,
                    MATERIAL_PAIR.first,
                    MATERIAL_PAIR.second,
                    set_type::NotASet,
                    ELEMENT_TYPE,
                    SUMMON_INFO);

                AppendToEitherNormalOrReligiousVector(pixieProfile);
            }
        }
    }

} // namespace item
} // namespace heroespath
