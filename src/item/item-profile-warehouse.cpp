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
#include "creature/summon-info.hpp"
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
        : profiles_()
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
        profiles_.reserve(500000);

        // As of 2018-3-19 there were 251 religious profiles.
        // As of 2018-4-28 107
        religiousProfiles_.reserve(256);

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
        for (auto const & WEAPON_THIN_PROFILE : ThinProfilesWeaponsAll())
        {
            MakeLoopOverElementAndMaterialsForThinProfile(WEAPON_THIN_PROFILE);
        }

        for (auto const & ARMOR_THIN_PROFILE : ThinProfilesArmor(true))
        {
            MakeLoopOverElementAndMaterialsForThinProfile(ARMOR_THIN_PROFILE);
        }
    }

    void ItemProfileWarehouse::Setup_UniqueItems()
    {
        for (int i(1); i < unique_type::Count; ++i)
        {
            auto const UNIQUE_ENUM{ static_cast<unique_type::Enum>(i) };

            // currently there are only a few unique_type capes that are "of honor", but there are
            // some, so this loop on element type is required
            for (auto const ELEMENT_TYPE :
                 element_type::Combinations(unique_type::ElementTypes(UNIQUE_ENUM)))
            {
                auto didAddProfile{ false };

                auto appendPixieAndNonPixieProfilesAsNeeded{
                    [&](const material::Enum MATERIAL_PRIMARY,
                        const material::Enum MATERIAL_SECONDARY) {
                        auto const MISC_TYPE{ unique_type::MiscType(UNIQUE_ENUM) };

                        if (misc_type::HasOnlyPixieVersion(MISC_TYPE) == false)
                        {
                            ItemProfile profile;

                            profile.SetUnique(
                                UNIQUE_ENUM, MATERIAL_PRIMARY, MATERIAL_SECONDARY, ELEMENT_TYPE);

                            profile.SetMisc(MISC_TYPE, false, MATERIAL_PRIMARY, MATERIAL_SECONDARY);
                            AppendToEitherNormalOrReligiousVector(profile);
                            didAddProfile = true;
                        }

                        if (misc_type::HasPixieVersion(MISC_TYPE))
                        {
                            ItemProfile pixieProfile;

                            pixieProfile.SetUnique(
                                UNIQUE_ENUM, MATERIAL_PRIMARY, MATERIAL_SECONDARY, ELEMENT_TYPE);

                            pixieProfile.SetMisc(
                                MISC_TYPE, true, MATERIAL_PRIMARY, MATERIAL_SECONDARY);

                            AppendToEitherNormalOrReligiousVector(pixieProfile);
                            didAddProfile = true;
                        }
                    }
                };

                using MatPair_t = std::pair<item::material::Enum, item::material::Enum>;
                std::vector<MatPair_t> matPairsAlreadyMade;

                // this number found to be the max during test runs on 2018-4-28
                matPairsAlreadyMade.reserve(13);

                auto const MATERIALS{ unique_type::Materials(UNIQUE_ENUM) };

                M_ASSERT_OR_LOGANDTHROW_SS(
                    (MATERIALS.first.empty() == false),
                    "item::ItemProfileWarehouse::Setup_UniqueItems() failed to find any primary "
                        << "materials for unique_type=" << unique_type::ToString(UNIQUE_ENUM));

                for (auto const MATERIAL_PRIMARY : MATERIALS.first)
                {
                    if (MATERIALS.second.empty())
                    {
                        appendPixieAndNonPixieProfilesAsNeeded(MATERIAL_PRIMARY, material::Nothing);
                    }
                    else
                    {
                        for (auto const MATERIAL_SECONDARY : MATERIALS.second)
                        {
                            if (MATERIAL_PRIMARY == MATERIAL_SECONDARY)
                            {
                                M_HP_LOG_WRN(
                                    "item::ItemProfileWarehouse::Setup_Unique() pri==sec materials "
                                    "check "
                                    "found: "
                                    "pri="
                                    << material::ToString(MATERIAL_PRIMARY)
                                    << " and sec=" << material::ToString(MATERIAL_SECONDARY)
                                    << ", unique_type=" << unique_type::ToString(UNIQUE_ENUM)
                                    << ", element_type="
                                    << element_type::ToString(ELEMENT_TYPE, true));

                                continue;
                            }

                            auto const MAT_PAIR{ std::make_pair(
                                MATERIAL_PRIMARY, MATERIAL_SECONDARY) };

                            if (std::find(
                                    std::begin(matPairsAlreadyMade),
                                    std::end(matPairsAlreadyMade),
                                    MAT_PAIR)
                                != std::end(matPairsAlreadyMade))
                            {
                                M_HP_LOG_WRN(
                                    "item::ItemProfileWarehouse::Setup_Unique() repeat materials "
                                    "check "
                                    "found "
                                    " a pair that had already been made: pri="
                                    << material::ToString(MATERIAL_PRIMARY)
                                    << " and sec=" << material::ToString(MATERIAL_SECONDARY)
                                    << ", unique_type=" << unique_type::ToString(UNIQUE_ENUM)
                                    << ", element_type="
                                    << element_type::ToString(ELEMENT_TYPE, true));

                                // no need for continue here
                            }
                            else
                            {
                                matPairsAlreadyMade.emplace_back(MAT_PAIR);

                                appendPixieAndNonPixieProfilesAsNeeded(
                                    MATERIAL_PRIMARY, MATERIAL_SECONDARY);
                            }
                        }
                    }
                }

                M_ASSERT_OR_LOGANDTHROW_SS(
                    (didAddProfile),
                    "item::ItemProfileWarehouse::Setup_Unique() failed to find any valid "
                        << "material combinations for unique_type="
                        << unique_type::ToString(UNIQUE_ENUM));
            }
        }
    }

    void ItemProfileWarehouse::Setup_MiscItems()
    {
        for (int i(1); i < misc_type::Count; ++i)
        {
            auto const MISC_ENUM{ static_cast<misc_type::Enum>(i) };

            if (misc_type::IsStandalone(MISC_ENUM) && (misc_type::IsSummoning(MISC_ENUM) == false))
            {
                SetupProfilesForMiscType(MISC_ENUM);
            }
        }
    }

    void ItemProfileWarehouse::Setup_NamedEquipment()
    {
        for (int i(1); i < named_type::Count; ++i)
        {
            auto const NAMED_ENUM{ static_cast<named_type::Enum>(i) };
            auto const NAMED_THINPROFILES_VEC{ ThinProfiles(NAMED_ENUM) };

            for (auto const & NAMED_THINPROFILE : NAMED_THINPROFILES_VEC)
            {
                MakeLoopOverElementAndMaterialsForThinProfile(
                    NAMED_THINPROFILE, NAMED_ENUM, set_type::NotASet);
            }
        }
    }

    void ItemProfileWarehouse::Setup_SetEquipment()
    {
        for (int i(1); i < set_type::Count; ++i)
        {
            auto const SET_ENUM{ static_cast<set_type::Enum>(i) };
            auto const SET_THINPROFILES_VEC{ ThinProfiles(SET_ENUM) };

            for (auto const & SET_THINPROFILE : SET_THINPROFILES_VEC)
            {
                MakeLoopOverElementAndMaterialsForThinProfile(
                    SET_THINPROFILE, named_type::NotNamed, SET_ENUM);
            }
        }
    }

    void ItemProfileWarehouse::Setup_SummoningItems()
    {
        using namespace creature;

        std::vector<ItemProfile> profilesAlreadyMade;
        profilesAlreadyMade.reserve(2970); // found after testing on 2018-4-28

        auto addSummoningItemProfile{ [&](const misc_type::Enum MISC_TYPE,
                                          const material::Enum MATERIAL_PRIMARY,
                                          const origin_type::Enum ORIGIN_TYPE,
                                          const race::Enum RACE,
                                          const role::Enum ROLE) {
            ItemProfile p;

            p.SetMisc(MISC_TYPE, false, MATERIAL_PRIMARY, material::Nothing, set_type::NotASet);

            p.SetSummonInfoAndAdjustScore(SummonInfo(ORIGIN_TYPE, RACE, ROLE));

            if (std::find(std::begin(profilesAlreadyMade), std::end(profilesAlreadyMade), p)
                != std::end(profilesAlreadyMade))
            {
                M_HP_LOG_WRN(
                    "ItemProfileWarehouse::Setup_Summoning() duplicate profile "
                    "found: profile={"
                    << p.ToString() << "}");
            }
            else
            {
                profilesAlreadyMade.emplace_back(p);
            }
        } };

        for (int raceIndex(0); raceIndex < race::Count; ++raceIndex)
        {
            auto const RACE_ENUM{ static_cast<race::Enum>(raceIndex) };

            auto const ROLES_VEC{ race::Roles(RACE_ENUM) };

            for (auto const ROLE_ENUM : ROLES_VEC)
            {
                auto const ORIGINS_VEC{ race::OriginTypes(RACE_ENUM, ROLE_ENUM) };
                for (auto const ORIGIN_ENUM : ORIGINS_VEC)
                {
                    if (ORIGIN_ENUM == origin_type::Statue)
                    {
                        for (auto const MATERIAL :
                             misc_type::Materials(misc_type::Summoning_Statue).first)
                        {
                            addSummoningItemProfile(
                                misc_type::Summoning_Statue,
                                MATERIAL,
                                ORIGIN_ENUM,
                                RACE_ENUM,
                                ROLE_ENUM);
                        }
                    }
                    else
                    {
                        auto const MISC_TYPE{ [&]() {
                            if (RACE_ENUM == creature::race::Spider)
                            {
                                return misc_type::Spider_Eggs;
                            }
                            else if (ORIGIN_ENUM == origin_type::Egg)
                            {
                                return misc_type::Egg;
                            }
                            else if (ORIGIN_ENUM == origin_type::Embryo)
                            {
                                return misc_type::Embryo;
                            }
                            else
                            {
                                return misc_type::Seeds;
                            }
                        }() };

                        auto const MATERIAL{ [ORIGIN_ENUM]() {
                            if (ORIGIN_ENUM == origin_type::Seeds)
                            {
                                return material::Plant;
                            }
                            else
                            {
                                return material::Flesh;
                            }
                        }() };

                        addSummoningItemProfile(
                            MISC_TYPE, MATERIAL, ORIGIN_ENUM, RACE_ENUM, ROLE_ENUM);
                    }
                }
            }
        }

        std::copy(
            std::begin(profilesAlreadyMade),
            std::end(profilesAlreadyMade),
            std::back_inserter(profiles_));
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

    const ElementTypeVec_t ItemProfileWarehouse::ElementVecFromNamedAndSetTypes(
        const named_type::Enum NAMED_TYPE, const set_type::Enum SET_TYPE)
    {
        auto const ELEMENT_TYPE{ [&]() {
            if ((NAMED_TYPE != named_type::NotNamed) && (NAMED_TYPE != named_type::Count))
            {
                return named_type::ElementTypes(NAMED_TYPE);
            }
            else if ((SET_TYPE != set_type::NotASet) && (SET_TYPE != set_type::Count))
            {
                return element_type::None;
            }
            else
            {
                return static_cast<element_type::Enum>(
                    element_type::Fire | element_type::Frost | element_type::Honor
                    | element_type::Shadow);
            }
        }() };

        return element_type::Combinations(ELEMENT_TYPE, true);
    }

    void ItemProfileWarehouse::MakeLoopOverElementAndMaterialsForThinProfile(
        const ItemProfileThin & THIN_PROFILE,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE)
    {
        auto const BASEMATERIAL_VEC_PAIR_VEC{ GetMaterialsFromThinProfile(
            THIN_PROFILE, NAMED_TYPE) };

        for (auto const ELEMENT_TYPE : ElementVecFromNamedAndSetTypes(NAMED_TYPE, SET_TYPE))
        {
            for (auto const & NEXT_BASEMATERIALVECPAIR : BASEMATERIAL_VEC_PAIR_VEC)
            {
                M_ASSERT_OR_LOGANDTHROW_SS(
                    (NEXT_BASEMATERIALVECPAIR.second.first.empty() == false),
                    "ItemProfileWarehouse(vector version, thin_profile="
                        << THIN_PROFILE.ToString()
                        << ", named_type=" << named_type::ToString(NAMED_TYPE)
                        << ", set_type=" << set_type::ToString(SET_TYPE)
                        << ", element_type=" << element_type::ToString(ELEMENT_TYPE, true)
                        << ", BASEMATERIAL_VEC_PAIR_VEC.size()=" << BASEMATERIAL_VEC_PAIR_VEC.size()
                        << ") was given an empty "
                        << "NEXT_BASEMATERIALVECPAIR.second.first (primary material vec).");

                for (auto const MATERIAL_PRI : NEXT_BASEMATERIALVECPAIR.second.first)
                {
                    if (NEXT_BASEMATERIALVECPAIR.second.second.empty())
                    {
                        MakeFromThinProfile(
                            THIN_PROFILE,
                            NAMED_TYPE,
                            SET_TYPE,
                            ELEMENT_TYPE,
                            MATERIAL_PRI,
                            material::Nothing,
                            NEXT_BASEMATERIALVECPAIR.first);
                    }
                    else
                    {
                        for (auto const MATERIAL_SEC : NEXT_BASEMATERIALVECPAIR.second.second)
                        {
                            if (MATERIAL_SEC != item::material::Nothing)
                            {
                                MakeFromThinProfile(
                                    THIN_PROFILE,
                                    NAMED_TYPE,
                                    SET_TYPE,
                                    ELEMENT_TYPE,
                                    MATERIAL_PRI,
                                    MATERIAL_SEC,
                                    NEXT_BASEMATERIALVECPAIR.first);
                            }
                        }
                    }
                }
            }
        }
    }

    void ItemProfileWarehouse::MakeFromThinProfile(
        const ItemProfileThin & THIN_PROFILE,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE,
        const element_type::Enum ELEMENT_TYPE,
        const material::Enum MATERIAL_PRI,
        const material::Enum MATERIAL_SEC,
        const armor::base_type::Enum BASE_TYPE)
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
                    THIN_PROFILE.MiscType(),
                    false,
                    MATERIAL_PRI,
                    MATERIAL_SEC,
                    SET_TYPE,
                    ELEMENT_TYPE);

                AppendToEitherNormalOrReligiousVector(profile);
            }

            if (misc_type::HasPixieVersion(THIN_PROFILE.MiscType()))
            {
                ItemProfile pixieProfile;
                pixieProfile.SetMisc(
                    THIN_PROFILE.MiscType(),
                    true,
                    MATERIAL_PRI,
                    MATERIAL_SEC,
                    SET_TYPE,
                    ELEMENT_TYPE);

                AppendToEitherNormalOrReligiousVector(pixieProfile);
            }

            return;
        }
        else if (THIN_PROFILE.IsArmor())
        {
            if (THIN_PROFILE.ArmorInfo().IsAventail())
            {
                ItemProfile profile;

                profile.SetAventail(
                    BASE_TYPE, MATERIAL_PRI, MATERIAL_SEC, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE);

                AppendToEitherNormalOrReligiousVector(profile);
                return;
            }
            else if (THIN_PROFILE.ArmorInfo().IsBoots())
            {
                ItemProfile profile;

                profile.SetBoots(
                    BASE_TYPE,
                    MATERIAL_PRI,
                    MATERIAL_SEC,
                    NAMED_TYPE,
                    SET_TYPE,
                    ELEMENT_TYPE,
                    false);

                AppendToEitherNormalOrReligiousVector(profile);

                ItemProfile profilePixie;

                profilePixie.SetBoots(
                    BASE_TYPE,
                    MATERIAL_PRI,
                    MATERIAL_SEC,
                    NAMED_TYPE,
                    SET_TYPE,
                    ELEMENT_TYPE,
                    true);

                AppendToEitherNormalOrReligiousVector(profilePixie);
                return;
            }
            else if (THIN_PROFILE.ArmorInfo().IsBracers())
            {
                ItemProfile profile;

                profile.SetBracer(
                    BASE_TYPE,
                    MATERIAL_PRI,
                    MATERIAL_SEC,
                    NAMED_TYPE,
                    SET_TYPE,
                    ELEMENT_TYPE,
                    false);

                AppendToEitherNormalOrReligiousVector(profile);

                ItemProfile profilePixie;

                profilePixie.SetBracer(
                    BASE_TYPE,
                    MATERIAL_PRI,
                    MATERIAL_SEC,
                    NAMED_TYPE,
                    SET_TYPE,
                    ELEMENT_TYPE,
                    true);

                AppendToEitherNormalOrReligiousVector(profilePixie);
                return;
            }
            else if (THIN_PROFILE.ArmorInfo().IsGauntlets())
            {
                ItemProfile profile;

                profile.SetGauntlets(
                    BASE_TYPE,
                    MATERIAL_PRI,
                    MATERIAL_SEC,
                    NAMED_TYPE,
                    SET_TYPE,
                    ELEMENT_TYPE,
                    false);

                AppendToEitherNormalOrReligiousVector(profile);

                ItemProfile profilePixie;

                profilePixie.SetGauntlets(
                    BASE_TYPE,
                    MATERIAL_PRI,
                    MATERIAL_SEC,
                    NAMED_TYPE,
                    SET_TYPE,
                    ELEMENT_TYPE,
                    true);

                AppendToEitherNormalOrReligiousVector(profilePixie);
                return;
            }
            else if (THIN_PROFILE.ArmorInfo().IsPants())
            {
                ItemProfile profile;

                profile.SetPants(
                    BASE_TYPE,
                    MATERIAL_PRI,
                    MATERIAL_SEC,
                    NAMED_TYPE,
                    SET_TYPE,
                    ELEMENT_TYPE,
                    false);

                AppendToEitherNormalOrReligiousVector(profile);

                ItemProfile profilePixie;

                profilePixie.SetPants(
                    BASE_TYPE,
                    MATERIAL_PRI,
                    MATERIAL_SEC,
                    NAMED_TYPE,
                    SET_TYPE,
                    ELEMENT_TYPE,
                    true);

                AppendToEitherNormalOrReligiousVector(profilePixie);
                return;
            }
            else if (THIN_PROFILE.ArmorInfo().IsShirt())
            {
                ItemProfile profile;

                profile.SetShirt(
                    BASE_TYPE,
                    MATERIAL_PRI,
                    MATERIAL_SEC,
                    NAMED_TYPE,
                    SET_TYPE,
                    ELEMENT_TYPE,
                    false);

                AppendToEitherNormalOrReligiousVector(profile);

                ItemProfile profilePixie;

                profilePixie.SetShirt(
                    BASE_TYPE,
                    MATERIAL_PRI,
                    MATERIAL_SEC,
                    NAMED_TYPE,
                    SET_TYPE,
                    ELEMENT_TYPE,
                    true);

                AppendToEitherNormalOrReligiousVector(profilePixie);
                return;
            }
            else if (THIN_PROFILE.ArmorInfo().IsShield())
            {
                ItemProfile profile;

                profile.SetShield(
                    THIN_PROFILE.ArmorInfo().ShieldType(),
                    MATERIAL_PRI,
                    MATERIAL_SEC,
                    NAMED_TYPE,
                    SET_TYPE,
                    ELEMENT_TYPE);

                AppendToEitherNormalOrReligiousVector(profile);
                return;
            }
            else if (THIN_PROFILE.ArmorInfo().IsCover())
            {
                ItemProfile profile;

                profile.SetCover(
                    THIN_PROFILE.ArmorInfo().CoverType(),
                    MATERIAL_PRI,
                    MATERIAL_SEC,
                    NAMED_TYPE,
                    SET_TYPE,
                    ELEMENT_TYPE,
                    false);

                AppendToEitherNormalOrReligiousVector(profile);

                ItemProfile profilePixie;

                profilePixie.SetCover(
                    THIN_PROFILE.ArmorInfo().CoverType(),
                    MATERIAL_PRI,
                    MATERIAL_SEC,
                    NAMED_TYPE,
                    SET_TYPE,
                    ELEMENT_TYPE,
                    true);

                AppendToEitherNormalOrReligiousVector(profilePixie);
                return;
            }
            else if (THIN_PROFILE.ArmorInfo().IsHelm())
            {
                ItemProfile profile;

                profile.SetHelm(
                    THIN_PROFILE.ArmorInfo().HelmType(),
                    MATERIAL_PRI,
                    MATERIAL_SEC,
                    NAMED_TYPE,
                    SET_TYPE,
                    ELEMENT_TYPE);

                AppendToEitherNormalOrReligiousVector(profile);
                return;
            }
        }
        else if (THIN_PROFILE.IsWeapon())
        {
            if (THIN_PROFILE.WeaponInfo().IsSword())
            {
                ItemProfile profile;

                profile.SetSword(
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
                profile.SetStaff(MATERIAL_PRI, MATERIAL_SEC, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE);
                AppendToEitherNormalOrReligiousVector(profile);
                return;
            }
            else if (THIN_PROFILE.WeaponInfo().IsQuarterstaff())
            {
                ItemProfile profile;

                profile.SetQuarterStaff(
                    MATERIAL_PRI, MATERIAL_SEC, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE);

                AppendToEitherNormalOrReligiousVector(profile);
                return;
            }
        }

        std::ostringstream ss;
        ss << "ItemProfileWarehouse::MakeFromThinProfile(thin_profile=" << THIN_PROFILE.ToString()
           << ", named_type="
           << ((NAMED_TYPE == named_type::Count) ? "Count" : named_type::ToString(NAMED_TYPE))
           << ", set_type="
           << ((SET_TYPE == set_type::Count) ? "Count" : set_type::ToString(SET_TYPE))
           << ", element_type=" << element_type::ToString(ELEMENT_TYPE)
           << ", mat_pri=" << material::ToString(MATERIAL_PRI)
           << ", mat_sec=" << material::ToString(MATERIAL_SEC) << ", armor_base_type="
           << ((BASE_TYPE == armor::base_type::Count) ? "Count"
                                                      : armor::base_type::ToString(BASE_TYPE))
           << ") was unable to find the type of the given weapon or armor.";

        throw std::runtime_error(ss.str());
    }

    const BaseMaterialVecPairVec_t ItemProfileWarehouse::GetMaterialsFromThinProfile(
        const ItemProfileThin & THIN_PROFILE, const named_type::Enum NAMED_TYPE)
    {
        if ((NAMED_TYPE != named_type::NotNamed) && (NAMED_TYPE != named_type::Count))
        {
            auto const NAMED_MATERIALS_VEC_PAIR{ named_type::Materials(NAMED_TYPE) };

            if (NAMED_MATERIALS_VEC_PAIR.first.empty() == false)
            {
                return { std::make_pair(armor::base_type::Count, NAMED_MATERIALS_VEC_PAIR) };
            }
        }

        if (THIN_PROFILE.IsMisc())
        {
            return { std::make_pair(
                armor::base_type::Count, misc_type::Materials(THIN_PROFILE.MiscType())) };
        }
        else if (THIN_PROFILE.IsArmor())
        {
            if (THIN_PROFILE.ArmorInfo().IsAventail())
            {
                BaseMaterialVecPairVec_t v;

                for (int i(0); i < armor::base_type::Count; ++i)
                {
                    auto const NEXT_BASE_ENUM{ static_cast<armor::base_type::Enum>(i) };

                    if ((THIN_PROFILE.ArmorBaseTypeRestriction() == NEXT_BASE_ENUM)
                        || (THIN_PROFILE.ArmorBaseTypeRestriction() == armor::base_type::Count))
                    {
                        v.emplace_back(
                            std::make_pair(NEXT_BASE_ENUM, MaterialsAventail(NEXT_BASE_ENUM)));
                    }
                }

                return v;
            }
            else if (THIN_PROFILE.ArmorInfo().IsBoots())
            {
                BaseMaterialVecPairVec_t v;

                for (int i(0); i < armor::base_type::Count; ++i)
                {
                    auto const NEXT_BASE_ENUM{ static_cast<armor::base_type::Enum>(i) };

                    if ((THIN_PROFILE.ArmorBaseTypeRestriction() == NEXT_BASE_ENUM)
                        || (THIN_PROFILE.ArmorBaseTypeRestriction() == armor::base_type::Count))
                    {
                        v.emplace_back(
                            std::make_pair(NEXT_BASE_ENUM, MaterialsBoots(NEXT_BASE_ENUM)));
                    }
                }

                return v;
            }
            else if (THIN_PROFILE.ArmorInfo().IsBracers())
            {
                BaseMaterialVecPairVec_t v;

                for (int i(0); i < armor::base_type::Count; ++i)
                {
                    auto const NEXT_BASE_ENUM{ static_cast<armor::base_type::Enum>(i) };

                    if ((THIN_PROFILE.ArmorBaseTypeRestriction() == NEXT_BASE_ENUM)
                        || (THIN_PROFILE.ArmorBaseTypeRestriction() == armor::base_type::Count))
                    {
                        v.emplace_back(
                            std::make_pair(NEXT_BASE_ENUM, MaterialsBracer(NEXT_BASE_ENUM)));
                    }
                }

                return v;
            }
            else if (THIN_PROFILE.ArmorInfo().IsGauntlets())
            {
                BaseMaterialVecPairVec_t v;

                for (int i(0); i < armor::base_type::Count; ++i)
                {
                    auto const NEXT_BASE_ENUM{ static_cast<armor::base_type::Enum>(i) };
                    if ((THIN_PROFILE.ArmorBaseTypeRestriction() == NEXT_BASE_ENUM)
                        || (THIN_PROFILE.ArmorBaseTypeRestriction() == armor::base_type::Count))
                    {
                        v.emplace_back(
                            std::make_pair(NEXT_BASE_ENUM, MaterialsGauntlets(NEXT_BASE_ENUM)));
                    }
                }

                return v;
            }
            else if (THIN_PROFILE.ArmorInfo().IsPants())
            {
                BaseMaterialVecPairVec_t v;

                for (int i(0); i < armor::base_type::Count; ++i)
                {
                    auto const NEXT_BASE_ENUM{ static_cast<armor::base_type::Enum>(i) };
                    if ((THIN_PROFILE.ArmorBaseTypeRestriction() == NEXT_BASE_ENUM)
                        || (THIN_PROFILE.ArmorBaseTypeRestriction() == armor::base_type::Count))
                    {
                        v.emplace_back(
                            std::make_pair(NEXT_BASE_ENUM, MaterialsPants(NEXT_BASE_ENUM)));
                    }
                }

                return v;
            }
            else if (THIN_PROFILE.ArmorInfo().IsShirt())
            {
                BaseMaterialVecPairVec_t v;

                for (int i(0); i < armor::base_type::Count; ++i)
                {
                    auto const NEXT_BASE_ENUM{ static_cast<armor::base_type::Enum>(i) };
                    if ((THIN_PROFILE.ArmorBaseTypeRestriction() == NEXT_BASE_ENUM)
                        || (THIN_PROFILE.ArmorBaseTypeRestriction() == armor::base_type::Count))
                    {
                        v.emplace_back(
                            std::make_pair(NEXT_BASE_ENUM, MaterialsShirt(NEXT_BASE_ENUM)));
                    }
                }

                return v;
            }
            else if (THIN_PROFILE.ArmorInfo().IsShield())
            {
                return { std::make_pair(
                    armor::base_type::Count, Materials(THIN_PROFILE.ArmorInfo().ShieldType())) };
            }
            else if (THIN_PROFILE.ArmorInfo().IsCover())
            {
                return { std::make_pair(
                    armor::base_type::Count, Materials(THIN_PROFILE.ArmorInfo().CoverType())) };
            }
            else if (THIN_PROFILE.ArmorInfo().IsHelm())
            {
                return { std::make_pair(
                    armor::base_type::Count, Materials(THIN_PROFILE.ArmorInfo().HelmType())) };
            }
        }
        else if (THIN_PROFILE.IsWeapon())
        {
            if (THIN_PROFILE.WeaponInfo().IsSword())
            {
                return { std::make_pair(
                    armor::base_type::Count, Materials(THIN_PROFILE.WeaponInfo().SwordType())) };
            }
            else if (THIN_PROFILE.WeaponInfo().IsAxe())
            {
                return { std::make_pair(
                    armor::base_type::Count, Materials(THIN_PROFILE.WeaponInfo().AxeType())) };
            }
            else if (THIN_PROFILE.WeaponInfo().IsClub())
            {
                return { std::make_pair(
                    armor::base_type::Count, Materials(THIN_PROFILE.WeaponInfo().ClubType())) };
            }
            else if (THIN_PROFILE.WeaponInfo().IsWhip())
            {
                return { std::make_pair(
                    armor::base_type::Count, Materials(THIN_PROFILE.WeaponInfo().WhipType())) };
            }
            else if (THIN_PROFILE.WeaponInfo().IsProjectile())
            {
                return { std::make_pair(
                    armor::base_type::Count,
                    Materials(THIN_PROFILE.WeaponInfo().ProjectileType())) };
            }
            else if (THIN_PROFILE.WeaponInfo().IsBladedStaff())
            {
                return { std::make_pair(
                    armor::base_type::Count,
                    Materials(THIN_PROFILE.WeaponInfo().BladedStaffType())) };
            }
            else if (THIN_PROFILE.WeaponInfo().IsKnife())
            {
                return { std::make_pair(armor::base_type::Count, MaterialsKnife()) };
            }
            else if (THIN_PROFILE.WeaponInfo().IsDagger())
            {
                return { std::make_pair(armor::base_type::Count, MaterialsDagger()) };
            }
            else if (THIN_PROFILE.WeaponInfo().IsStaff())
            {
                return { std::make_pair(armor::base_type::Count, MaterialsStaff()) };
            }
            else if (THIN_PROFILE.WeaponInfo().IsQuarterstaff())
            {
                return { std::make_pair(armor::base_type::Count, MaterialsQuarterStaff()) };
            }
        }

        std::ostringstream ss;
        ss << "ItemProfileWarehouse::GetMaterialsFromThinProfile(thin_profile="
           << THIN_PROFILE.ToString() << ") "
           << "-was unable to find the type of the given ThinProfile to make materials from.";

        throw std::runtime_error(ss.str());
    }

    void ItemProfileWarehouse::SetupProfilesForMiscType(const misc_type::Enum MISC_TYPE)
    {
        auto appendMiscItemProfile{ [&](const material::Enum MATERIAL_PRIMARY,
                                        const material::Enum MATERIAL_SECONDARY) {
            if (misc_type::HasOnlyPixieVersion(MISC_TYPE) == false)
            {
                ItemProfile profile;
                profile.SetMisc(MISC_TYPE, false, MATERIAL_PRIMARY, MATERIAL_SECONDARY);
                AppendToEitherNormalOrReligiousVector(profile);
            }

            if (misc_type::HasPixieVersion(MISC_TYPE))
            {
                ItemProfile pixieProfile;
                pixieProfile.SetMisc(MISC_TYPE, true, MATERIAL_PRIMARY, MATERIAL_SECONDARY);
                AppendToEitherNormalOrReligiousVector(pixieProfile);
            }
        } };

        auto const MATERIALS{ misc_type::Materials(MISC_TYPE) };

        M_ASSERT_OR_LOGANDTHROW_SS(
            (MATERIALS.first.empty() == false),
            "ItemProfileWarehouse::SetupProfilesForMiscType() failed to find any "
                << " primary materials for misc_type=" << misc_type::ToString(MISC_TYPE));

        auto const BEFORE_COUNT{ profiles_.size() + religiousProfiles_.size() };

        for (auto const MATERIAL_PRIMARY : MATERIALS.first)
        {
            if (MATERIALS.second.empty())
            {
                appendMiscItemProfile(MATERIAL_PRIMARY, material::Nothing);
            }
            else
            {
                for (auto const MATERIAL_SECONDARY : MATERIALS.second)
                {
                    if (MATERIAL_PRIMARY == MATERIAL_SECONDARY)
                    {
                        continue;
                    }

                    appendMiscItemProfile(MATERIAL_PRIMARY, MATERIAL_SECONDARY);
                }
            }
        }

        auto const AFTER_COUNT{ profiles_.size() + religiousProfiles_.size() };

        M_ASSERT_OR_LOGANDTHROW_SS(
            (BEFORE_COUNT != AFTER_COUNT),
            "ItemProfileWarehouse::SetupProfilesForMiscType() failed to find any"
                << " valid material combinations for misc_type=" << misc_type::ToString(MISC_TYPE));
    }

    const MaterialVecPair_t ItemProfileWarehouse::Materials(const weapon::axe_type::Enum)
    {
        return material::CoreMetalAndCoreSecondary();
    }

    const MaterialVecPair_t ItemProfileWarehouse::Materials(const weapon::bladedstaff_type::Enum)
    {
        return MaterialVecPair_t({ material::Wood }, material::CoreSecondary());
    }

    const MaterialVecPair_t ItemProfileWarehouse::Materials(const weapon::club_type::Enum)
    {
        return material::CoreMetalAndCoreSecondary();
    }

    const MaterialVecPair_t
        ItemProfileWarehouse::Materials(const weapon::projectile_type::Enum PROJECTILE_TYPE)
    {

        using namespace weapon;

        switch (PROJECTILE_TYPE)
        {
            case projectile_type::Blowpipe:
            {
                return MaterialVecPair_t(
                    { material::Wood, material::Stone, material::Bone, material::Obsidian },
                    material::CoreSecondary());
            }

            case projectile_type::Sling:
            {
                return MaterialVecPair_t({ material::Rope }, material::CoreSecondary());
            }
            case projectile_type::Crossbow:
            case projectile_type::Shortbow:
            case projectile_type::Longbow:
            {
                return MaterialVecPair_t({ material::Wood }, material::CoreSecondary());
            }
            case projectile_type::CompositeBow:
            {
                return MaterialVecPair_t({ material::Horn }, material::CoreSecondary());
            }
            case projectile_type::Count:
            default:
            {
                return MaterialVecPair_t();
            }
        }
    }

    const MaterialVecPair_t ItemProfileWarehouse::Materials(const weapon::sword_type::Enum)
    {
        return material::CoreMetalAndCoreSecondary();
    }

    const MaterialVecPair_t ItemProfileWarehouse::Materials(const weapon::whip_type::Enum WHIP_TYPE)
    {

        using namespace weapon;

        if (WHIP_TYPE == whip_type::Bullwhip)
        {
            return MaterialVecPair_t({ material::HardLeather }, material::CoreSecondary());
        }
        else
        {
            return material::CoreMetalAndCoreSecondary();
        }
    }

    const MaterialVecPair_t ItemProfileWarehouse::MaterialsKnife()
    {
        return material::CorePrimaryNoPearlAndSecondary();
    }

    const MaterialVecPair_t ItemProfileWarehouse::MaterialsDagger() { return MaterialsKnife(); }

    const MaterialVecPair_t ItemProfileWarehouse::MaterialsStaff()
    {
        return MaterialVecPair_t({ material::Wood }, material::CoreSecondary());
    }

    const MaterialVecPair_t ItemProfileWarehouse::MaterialsQuarterStaff()
    {
        return MaterialsStaff();
    }

    const MaterialVecPair_t ItemProfileWarehouse::Materials(const armor::cover_type::Enum)
    {
        return MaterialVecPair_t(
            { material::Cloth, material::SoftLeather, material::HardLeather },
            material::CoreSecondary());
    }

    const MaterialVecPair_t ItemProfileWarehouse::Materials(const armor::helm_type::Enum HELM_TYPE)
    {
        using namespace armor;

        if (HELM_TYPE == helm_type::Leather)
        {
            return MaterialVecPair_t({ material::HardLeather }, material::CoreSecondary());
        }
        else
        {
            return MaterialVecPair_t(material::CoreMetal(), material::CoreSecondary());
        }
    }

    const MaterialVecPair_t ItemProfileWarehouse::Materials(const armor::shield_type::Enum)
    {
        MaterialVec_t v{ material::CoreMetal() };
        v.emplace_back(material::Wood);
        return MaterialVecPair_t(v, material::CoreSecondary());
    }

    const MaterialVecPair_t
        ItemProfileWarehouse::MaterialsAventail(armor::base_type::Enum BASE_TYPE)
    {
        return MaterialsFromBaseType(BASE_TYPE);
    }

    const MaterialVecPair_t ItemProfileWarehouse::MaterialsBracer(armor::base_type::Enum BASE_TYPE)
    {
        return MaterialsFromBaseType(BASE_TYPE);
    }

    const MaterialVecPair_t ItemProfileWarehouse::MaterialsShirt(armor::base_type::Enum BASE_TYPE)
    {
        return MaterialsFromBaseType(BASE_TYPE);
    }

    const MaterialVecPair_t ItemProfileWarehouse::MaterialsBoots(armor::base_type::Enum BASE_TYPE)
    {
        return MaterialsFromBaseTypeNoCloth(BASE_TYPE);
    }

    const MaterialVecPair_t ItemProfileWarehouse::MaterialsPants(armor::base_type::Enum BASE_TYPE)
    {
        return MaterialsFromBaseType(BASE_TYPE);
    }

    const MaterialVecPair_t
        ItemProfileWarehouse::MaterialsGauntlets(armor::base_type::Enum BASE_TYPE)
    {
        return MaterialsFromBaseType(BASE_TYPE);
    }

    const MaterialVec_t ItemProfileWarehouse::MaterialsPrimaryFromArmorBaseType(
        const armor::base_type::Enum BASE_TYPE)
    {
        using namespace armor;

        switch (BASE_TYPE)
        {
            case base_type::Scale:
            {
                return { material::Scales };
            }
            case base_type::Mail:
            case base_type::Plate:
            {
                return material::CoreMetal();
            }
            case base_type::Plain:
            {
                return { material::Cloth, material::SoftLeather, material::HardLeather };
            }
            case base_type::Count:
            default:
            {
                return {};
            }
        }
    }

    const MaterialVecPair_t
        ItemProfileWarehouse::MaterialsFromBaseType(const armor::base_type::Enum BASE_TYPE)
    {
        return MaterialVecPair_t(
            MaterialsPrimaryFromArmorBaseType(BASE_TYPE), material::CoreSecondary());
    }

    const MaterialVecPair_t
        ItemProfileWarehouse::MaterialsFromBaseTypeNoCloth(const armor::base_type::Enum BASE_TYPE)
    {
        MaterialVec_t primaryMaterials{ material::SoftLeather, material::HardLeather };

        if (BASE_TYPE != armor::base_type::Plain)
        {
            primaryMaterials = MaterialsPrimaryFromArmorBaseType(BASE_TYPE);
        }

        return MaterialVecPair_t(primaryMaterials, material::CoreSecondary());
    }

    const ItemProfileThinVec_t ItemProfileWarehouse::ThinProfilesWeaponsSwords()
    {
        return ItemProfileThin::MakeWeaponSpecificAll<weapon::sword_type>();
    }

    const ItemProfileThinVec_t ItemProfileWarehouse::ThinProfilesWeaponsProjectiles()
    {
        return ItemProfileThin::MakeWeaponSpecificAll<weapon::projectile_type>();
    }

    const ItemProfileThinVec_t ItemProfileWarehouse::ThinProfilesWeaponsAll()
    {
        ItemProfileThinVec_t thinProfiles;

        misc::Vector::AppendMove(ThinProfilesWeaponsSwords(), thinProfiles);
        misc::Vector::AppendMove(ThinProfilesWeaponsProjectiles(), thinProfiles);

        misc::Vector::AppendMove(
            ItemProfileThin::MakeWeaponSpecificAll<weapon::axe_type>(), thinProfiles);

        misc::Vector::AppendMove(
            ItemProfileThin::MakeWeaponSpecificAll<weapon::bladedstaff_type>(), thinProfiles);

        misc::Vector::AppendMove(
            ItemProfileThin::MakeWeaponSpecificAll<weapon::club_type>(), thinProfiles);

        misc::Vector::AppendMove(
            ItemProfileThin::MakeWeaponSpecificAll<weapon::whip_type>(), thinProfiles);

        misc::Vector::AppendMove(ItemProfileThin::MakeWeaponKnifeOrDaggerAll(false), thinProfiles);
        misc::Vector::AppendMove(ItemProfileThin::MakeWeaponKnifeOrDaggerAll(true), thinProfiles);
        thinProfiles.emplace_back(ItemProfileThin::MakeWeaponStaffOrQuarterstaff(false));
        thinProfiles.emplace_back(ItemProfileThin::MakeWeaponStaffOrQuarterstaff(true));

        return thinProfiles;
    }

    const ItemProfileThinVec_t ItemProfileWarehouse::ThinProfilesArmor(
        const bool WILL_INCLUDE_COVERINGS, const armor::base_type::Enum BASE_TYPE_RESTRICTION)
    {
        ItemProfileThinVec_t thinProfiles;

        if (WILL_INCLUDE_COVERINGS)
        {
            misc::Vector::AppendMove(
                ItemProfileThin::MakeArmorSpecificAll<armor::cover_type>(), thinProfiles);
        }

        misc::Vector::AppendMove(
            ItemProfileThin::MakeArmorSpecificAll<armor::helm_type>(), thinProfiles);

        misc::Vector::AppendMove(
            ItemProfileThin::MakeArmorSpecificAll<armor::shield_type>(), thinProfiles);

        thinProfiles.emplace_back(
            ItemProfileThin::MakeArmor(armor_type::Aventail, BASE_TYPE_RESTRICTION));

        thinProfiles.emplace_back(
            ItemProfileThin::MakeArmor(armor_type::Boots, BASE_TYPE_RESTRICTION));

        thinProfiles.emplace_back(
            ItemProfileThin::MakeArmor(armor_type::Bracers, BASE_TYPE_RESTRICTION));

        thinProfiles.emplace_back(
            ItemProfileThin::MakeArmor(armor_type::Gauntlets, BASE_TYPE_RESTRICTION));

        thinProfiles.emplace_back(
            ItemProfileThin::MakeArmor(armor_type::Pants, BASE_TYPE_RESTRICTION));

        thinProfiles.emplace_back(
            ItemProfileThin::MakeArmor(armor_type::Shirt, BASE_TYPE_RESTRICTION));

        return thinProfiles;
    }

    const ItemProfileThinVec_t ItemProfileWarehouse::ThinProfiles(const named_type::Enum NAMED_TYPE)
    {
        auto removeByArmorType{ [](std::vector<ItemProfileThin> & thinProfiles,
                                   const armor_type::Enum ARMOR_TYPE) {
            thinProfiles.erase(
                std::remove_if(
                    std::begin(thinProfiles),
                    std::end(thinProfiles),
                    [ARMOR_TYPE](auto const & THIN_PROFILE) {
                        return (THIN_PROFILE.ArmorInfo().Type() == ARMOR_TYPE);
                    }),
                std::end(thinProfiles));
        } };

        using namespace armor;
        using namespace weapon;

        switch (NAMED_TYPE)
        {
            case named_type::Wicked:
            case named_type::Fiendish:
            case named_type::Infernal:
            case named_type::Raging:
            case named_type::Diabolic:
            case named_type::Searing:
            case named_type::Burning:
            case named_type::Fiery:
            case named_type::Icy:
            case named_type::Winter:
            case named_type::Frigid:
            case named_type::Proud:
            case named_type::Glory:
            case named_type::Pure:
            case named_type::Gloom:
            case named_type::Twilight:
            case named_type::Dusk:
            case named_type::Dark:
            case named_type::Betrayer:
            {
                return ThinProfilesWeaponsAll();
            }

            case named_type::Dancing:
            case named_type::Marauder:
            {
                return ThinProfilesWeaponsSwords();
            }

            case named_type::Honest:
            case named_type::Noble:
            case named_type::Daring:
            case named_type::Elite:
            case named_type::Valiant:
            case named_type::Heros:
            case named_type::Light:
            case named_type::Dawn:
            case named_type::Sun:
            case named_type::Sorrow:
            case named_type::Woe:
            case named_type::Misery:
            case named_type::Moon:
            {
                return ThinProfilesArmor(true);
            }

            case named_type::Army:
            case named_type::Charred:
            case named_type::Chill:
            case named_type::Frozen:
            case named_type::Arctic:
            {
                return ThinProfilesArmor(false);
            }

            case named_type::Gladiator:
            {
                return { ItemProfileThin::MakeArmorSpecific(shield_type::Buckler),
                         ItemProfileThin::MakeArmorSpecific(shield_type::Kite),
                         ItemProfileThin::MakeArmorSpecific(helm_type::Great),
                         ItemProfileThin::MakeArmorSpecific(helm_type::Bascinet),
                         ItemProfileThin::MakeArmorSpecific(helm_type::Kettle),
                         ItemProfileThin::MakeWeaponSpecific(axe_type::Handaxe),
                         ItemProfileThin::MakeWeaponSpecific(bladedstaff_type::Spear),
                         ItemProfileThin::MakeWeaponSpecific(sword_type::Gladius),
                         ItemProfileThin::MakeWeaponSpecific(sword_type::Shortsword),
                         ItemProfileThin::MakeWeaponSpecific(sword_type::Broadsword) };
            }

            case named_type::Soldiers:
            {
                ItemProfileThinVec_t thinProfiles;
                misc::Vector::AppendMove(ThinProfilesWeaponsAll(), thinProfiles);
                misc::Vector::AppendMove(ThinProfilesArmor(false), thinProfiles);
                return thinProfiles;
            }

            case named_type::Wardens:
            {
                ItemProfileThinVec_t thinProfiles;

                misc::Vector::AppendMove(
                    ThinProfilesArmor(false, armor::base_type::Plain), thinProfiles);

                removeByArmorType(thinProfiles, armor_type::Shield);
                removeByArmorType(thinProfiles, armor_type::Helm);

                thinProfiles.emplace_back(
                    ItemProfileThin::MakeWeaponSpecific(sword_type::Shortsword));

                thinProfiles.emplace_back(ItemProfileThin::MakeArmorSpecific(helm_type::Bascinet));
                thinProfiles.emplace_back(ItemProfileThin::MakeArmorSpecific(shield_type::Buckler));

                return thinProfiles;
            }

            case named_type::Princes:
            {
                ItemProfileThinVec_t thinProfiles{ ThinProfilesArmor(
                    false, armor::base_type::Mail) };

                removeByArmorType(thinProfiles, armor_type::Shield);
                removeByArmorType(thinProfiles, armor_type::Helm);

                thinProfiles.emplace_back(ItemProfileThin::MakeWeaponSpecific(sword_type::Rapier));
                thinProfiles.emplace_back(ItemProfileThin::MakeArmorSpecific(helm_type::MailCoif));
                thinProfiles.emplace_back(ItemProfileThin::MakeArmorSpecific(shield_type::Kite));
                thinProfiles.emplace_back(ItemProfileThin::MakeArmorSpecific(cover_type::Cape));

                return thinProfiles;
            }

            case named_type::Ranger:
            {
                ItemProfileThinVec_t thinProfiles{ ThinProfilesArmor(
                    false, armor::base_type::Plain) };

                removeByArmorType(thinProfiles, armor_type::Shield);
                removeByArmorType(thinProfiles, armor_type::Helm);

                thinProfiles.emplace_back(
                    ItemProfileThin::MakeWeaponSpecific(projectile_type::Longbow));

                thinProfiles.emplace_back(ItemProfileThin::MakeArmorSpecific(cover_type::Cloak));

                return thinProfiles;
            }

            case named_type::Samurai:
            {
                ItemProfileThinVec_t thinProfiles{ ThinProfilesArmor(
                    false, armor::base_type::Scale) };

                removeByArmorType(thinProfiles, armor_type::Shield);
                removeByArmorType(thinProfiles, armor_type::Helm);

                thinProfiles.emplace_back(
                    ItemProfileThin::MakeWeaponSpecific(sword_type::Longsword));

                thinProfiles.emplace_back(ItemProfileThin::MakeArmorSpecific(helm_type::Leather));
                thinProfiles.emplace_back(ItemProfileThin::MakeArmorSpecific(cover_type::Cape));

                return thinProfiles;
            }

            case named_type::Thors:
            {
                ItemProfileThinVec_t thinProfiles{ ThinProfilesArmor(false) };

                removeByArmorType(thinProfiles, armor_type::Shield);
                removeByArmorType(thinProfiles, armor_type::Helm);

                thinProfiles.emplace_back(
                    ItemProfileThin::MakeWeaponSpecific(sword_type::Broadsword));

                thinProfiles.emplace_back(ItemProfileThin::MakeArmorSpecific(helm_type::Great));
                thinProfiles.emplace_back(ItemProfileThin::MakeArmorSpecific(shield_type::Pavis));

                return thinProfiles;
            }
            case named_type::Nile:
            {
                return { ItemProfileThin::MakeWeaponSpecific(sword_type::Falcata),
                         ItemProfileThin::MakeArmorSpecific(shield_type::Buckler),
                         ItemProfileThin::MakeArmorSpecific(helm_type::Leather),
                         ItemProfileThin::MakeArmor(armor_type::Bracers, armor::base_type::Plain) };
            }

            case named_type::Imposters:
            case named_type::Pickpocket:
            {
                return { ItemProfileThin::MakeArmorSpecific(cover_type::Robe) };
            }

            case named_type::Burglar:
            case named_type::Mountebank:
            {
                return { ItemProfileThin::MakeArmorSpecific(cover_type::Cloak) };
            }

            case named_type::Charlatans:
            {
                return { ItemProfileThin::MakeArmorSpecific(cover_type::Vest) };
            }

            case named_type::Robbers:
            case named_type::Thugs:
            case named_type::Knaves:
            {
                return { ItemProfileThin::MakeWeaponKnifeOrDagger(false) };
            }

            case named_type::Muggers:
            case named_type::Thief:
            case named_type::Pirate:
            {
                return { ItemProfileThin::MakeWeaponKnifeOrDagger(true) };
            }

            case named_type::Focus:
            {
                return ThinProfilesWeaponsProjectiles();
            }

            case named_type::NotNamed:
            case named_type::Count:
            default:
            {
                return {};
            }
        }
    }

    const ItemProfileThinVec_t ItemProfileWarehouse::ThinProfiles(const set_type::Enum SET_TYPE)
    {
        switch (SET_TYPE)
        {
            case set_type::TheAssassins:
            case set_type::TheTickler:
            case set_type::TheNeverwinter:
            {
                return { ItemProfileThin::MakeArmor(armor_type::Boots, armor::base_type::Plain),
                         ItemProfileThin::MakeArmor(armor_type::Pants, armor::base_type::Plain),
                         ItemProfileThin::MakeArmor(armor_type::Shirt, armor::base_type::Plain),
                         ItemProfileThin::MakeArmor(armor_type::Gauntlets, armor::base_type::Plain),
                         ItemProfileThin::MakeArmorSpecific(armor::cover_type::Cloak),
                         ItemProfileThin::MakeWeaponKnifeOrDagger(true),
                         ItemProfileThin::MakeMisc(misc_type::LockPicks) };
            }

            case set_type::TheMagus:
            case set_type::TheNecromancers:
            case set_type::TheWarlocks:
            {
                return { ItemProfileThin::MakeArmor(armor_type::Boots, armor::base_type::Plain),
                         ItemProfileThin::MakeArmor(armor_type::Pants, armor::base_type::Plain),
                         ItemProfileThin::MakeArmor(armor_type::Shirt, armor::base_type::Plain),
                         ItemProfileThin::MakeArmor(armor_type::Gauntlets, armor::base_type::Plain),
                         ItemProfileThin::MakeArmorSpecific(armor::cover_type::Robe),
                         ItemProfileThin::MakeMisc(misc_type::Wand) };
            }

            case set_type::TheLichKings:
            {
                return { ItemProfileThin::MakeArmor(armor_type::Boots, armor::base_type::Plain),
                         ItemProfileThin::MakeArmor(armor_type::Pants, armor::base_type::Plain),
                         ItemProfileThin::MakeArmor(armor_type::Shirt, armor::base_type::Plain),
                         ItemProfileThin::MakeArmor(armor_type::Gauntlets, armor::base_type::Plain),
                         ItemProfileThin::MakeArmorSpecific(armor::cover_type::Robe),
                         ItemProfileThin::MakeMisc(misc_type::Litch_Hand) };
            }

            case set_type::TheSages:
            case set_type::TheShamans:
            case set_type::TheOracles:
            {
                return { ItemProfileThin::MakeArmor(armor_type::Boots, armor::base_type::Plain),
                         ItemProfileThin::MakeArmor(armor_type::Pants, armor::base_type::Plain),
                         ItemProfileThin::MakeArmor(armor_type::Shirt, armor::base_type::Plain),
                         ItemProfileThin::MakeArmor(armor_type::Gauntlets, armor::base_type::Plain),
                         ItemProfileThin::MakeArmorSpecific(armor::cover_type::Robe),
                         ItemProfileThin::MakeMisc(misc_type::Wand) };
            }

            case set_type::TheAngelic:
            {
                return { ItemProfileThin::MakeArmor(armor_type::Boots),
                         ItemProfileThin::MakeArmor(armor_type::Pants, armor::base_type::Plain),
                         ItemProfileThin::MakeArmor(armor_type::Shirt, armor::base_type::Plain),
                         ItemProfileThin::MakeArmor(armor_type::Gauntlets, armor::base_type::Plain),
                         ItemProfileThin::MakeArmorSpecific(armor::cover_type::Robe),
                         ItemProfileThin::MakeMisc(misc_type::Angel_Braid) };
            }

            case set_type::TheBalladeers:
            case set_type::TheTroubadours:
            case set_type::TheMuses:
            {
                return { ItemProfileThin::MakeArmor(armor_type::Boots, armor::base_type::Mail),
                         ItemProfileThin::MakeArmor(armor_type::Pants, armor::base_type::Mail),
                         ItemProfileThin::MakeArmor(armor_type::Shirt, armor::base_type::Mail),
                         ItemProfileThin::MakeArmor(armor_type::Gauntlets, armor::base_type::Plain),
                         ItemProfileThin::MakeArmorSpecific(armor::cover_type::Vest),
                         ItemProfileThin::MakeWeaponSpecific(weapon::sword_type::Longsword),
                         ItemProfileThin::MakeMisc(misc_type::DrumLute) };
            }

            case set_type::TheCavaliers:
            case set_type::TheChampions:
            case set_type::ThePaladins:
            case set_type::TheBerserkers:
            case set_type::TheRosewood:
            case set_type::TheDragonslayers:
            case set_type::TheEventideRider:
            {
                return { ItemProfileThin::MakeArmor(armor_type::Boots, armor::base_type::Plate),
                         ItemProfileThin::MakeArmor(armor_type::Pants, armor::base_type::Plate),
                         ItemProfileThin::MakeArmor(armor_type::Shirt, armor::base_type::Plate),
                         ItemProfileThin::MakeArmor(armor_type::Gauntlets, armor::base_type::Plate),
                         ItemProfileThin::MakeArmor(armor_type::Bracers, armor::base_type::Plate),
                         ItemProfileThin::MakeArmorSpecific(armor::cover_type::Cape),
                         ItemProfileThin::MakeArmorSpecific(armor::shield_type::Pavis),
                         ItemProfileThin::MakeArmorSpecific(armor::helm_type::Great),
                         ItemProfileThin::MakeArmor(
                             armor_type::Aventail, armor::base_type::Plate) };
            }

            case set_type::TheHunters:
            case set_type::TheSureShot:
            case set_type::TheMarksmans:
            case set_type::TheDeadeye:
            case set_type::TheDuskRanger:
            case set_type::TheVenomBow:
            {
                return { ItemProfileThin::MakeArmor(armor_type::Boots, armor::base_type::Mail),
                         ItemProfileThin::MakeArmor(armor_type::Pants, armor::base_type::Mail),
                         ItemProfileThin::MakeArmor(armor_type::Shirt, armor::base_type::Mail),
                         ItemProfileThin::MakeArmor(armor_type::Gauntlets, armor::base_type::Plain),
                         ItemProfileThin::MakeArmor(armor_type::Bracers, armor::base_type::Mail),
                         ItemProfileThin::MakeArmorSpecific(armor::cover_type::Vest),
                         ItemProfileThin::MakeArmorSpecific(armor::helm_type::Archers) };
            }

            case set_type::TheCritterChannelers:
            case set_type::TheMammalianHead:
            case set_type::TheSavageTaskmasters:
            case set_type::TheMonsterOverseers:
            case set_type::TheBeastRulers:
            {
                return { ItemProfileThin::MakeArmor(armor_type::Boots, armor::base_type::Scale),
                         ItemProfileThin::MakeArmor(armor_type::Pants, armor::base_type::Scale),
                         ItemProfileThin::MakeArmor(armor_type::Shirt, armor::base_type::Scale),
                         ItemProfileThin::MakeArmor(armor_type::Bracers, armor::base_type::Scale),
                         ItemProfileThin::MakeArmor(armor_type::Gauntlets, armor::base_type::Scale),
                         ItemProfileThin::MakeArmorSpecific(armor::shield_type::Kite),
                         ItemProfileThin::MakeArmorSpecific(armor::helm_type::Bascinet) };
            }

            case set_type::NotASet:
            case set_type::Count:
            default:
            {
                return {};
            }
        }
    }

} // namespace item
} // namespace heroespath
