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
#include <set>
#include <sstream>
#include <string>

namespace heroespath
{
namespace item
{

    std::unique_ptr<ItemProfileWarehouse> ItemProfileWarehouse::instanceUPtr_;

    ItemProfileWarehouse::ItemProfileWarehouse()
        : profiles_()
        , religiousProfiles_()
    {
        M_HP_LOG_DBG("Singleton Construction: ItemProfileWarehouse");
    }

    ItemProfileWarehouse::~ItemProfileWarehouse()
    {
        M_HP_LOG_DBG("Singleton Destruction: ItemProfileWarehouse");
    }

    misc::NotNull<ItemProfileWarehouse *> ItemProfileWarehouse::Instance()
    {
        if (!instanceUPtr_)
        {
            M_HP_LOG_ERR("Singleton Instance() before Acquire(): ItemProfileWarehouse");
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
            M_HP_LOG_ERR("Singleton Acquire() after Construction: ItemProfileWarehouse");
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
        profiles_.reserve(500000);

        // As of 2018-3-19 there were 251 religious profiles.
        religiousProfiles_.reserve(256);

        Setup_StandardEquipment();
        Setup_UniqueItems();
        Setup_MiscItems();
        Setup_NamedEquipment();
        Setup_SetEquipment();
        Setup_SummoningItems();

        profiles_.shrink_to_fit();
        religiousProfiles_.shrink_to_fit();

        std::sort(profiles_.begin(), profiles_.end(), [](const auto & A, const auto & B) {
            return A.TreasureScore() < B.TreasureScore();
        });

        std::sort(
            religiousProfiles_.begin(),
            religiousProfiles_.end(),
            [](const auto & A, const auto & B) { return A.ReligiousScore() < B.ReligiousScore(); });
    }

    void ItemProfileWarehouse::Setup_StandardEquipment()
    {
        auto setupThinProfile = [this](const auto & THIN_PROFILE) {
            this->SetupFromThinProfile(THIN_PROFILE, named_type::NotNamed, set_type::NotASet);
        };

        auto const THINPROFILES_WEAPONS_VEC{ ThinProfilesWeaponsAll() };

        std::for_each(
            THINPROFILES_WEAPONS_VEC.begin(), THINPROFILES_WEAPONS_VEC.end(), setupThinProfile);

        auto const THINPROFILES_ARMOR_VEC{ ThinProfilesArmor(true) };

        std::for_each(
            THINPROFILES_ARMOR_VEC.begin(), THINPROFILES_ARMOR_VEC.end(), setupThinProfile);
    }

    void ItemProfileWarehouse::Setup_UniqueItems()
    {
        for (int i(1); i < unique_type::Count; ++i)
        {
            auto const NEXT_UNIQUE_ENUM{ static_cast<unique_type::Enum>(i) };

            auto const ELEMENT_TYPE_COMBS_VEC{ element_type::Combinations(
                unique_type::ElementTypes(NEXT_UNIQUE_ENUM)) };

            for (auto const NEXT_ELEMENT_TYPE : ELEMENT_TYPE_COMBS_VEC)
            {
                using MatPair_t = std::pair<item::material::Enum, item::material::Enum>;
                std::set<MatPair_t> matPairSet;

                auto const MATERIALS{ unique_type::Materials(NEXT_UNIQUE_ENUM) };

                M_ASSERT_OR_LOGANDTHROW_SS(
                    (MATERIALS.first.empty() == false),
                    "ItemProfileWarehouse::Setup() failed to find any primary "
                        << "materials for unique_type=" << unique_type::ToString(NEXT_UNIQUE_ENUM));

                auto didAddProfile{ false };

                for (auto const NEXT_MATERIAL_PRIMARY : MATERIALS.first)
                {
                    if (MATERIALS.second.empty())
                    {
                        auto const MAT_PAIR{ std::make_pair(
                            NEXT_MATERIAL_PRIMARY, item::material::Nothing) };

                        if (matPairSet.find(MAT_PAIR) != std::end(matPairSet))
                        {
                            continue;
                        }

                        matPairSet.insert(MAT_PAIR);

                        ItemProfile p;

                        p.SetUnique(
                            NEXT_UNIQUE_ENUM,
                            NEXT_MATERIAL_PRIMARY,
                            material::Nothing,
                            NEXT_ELEMENT_TYPE);

                        p.SetMisc(
                            unique_type::MiscType(NEXT_UNIQUE_ENUM),
                            false,
                            NEXT_MATERIAL_PRIMARY,
                            material::Nothing);

                        AppendProfileToTheCorrectVector(p);
                        didAddProfile = true;
                    }
                    else
                    {
                        for (auto const NEXT_MATERIAL_SECONDARY : MATERIALS.second)
                        {
                            if ((NEXT_MATERIAL_PRIMARY == NEXT_MATERIAL_SECONDARY)
                                || (NEXT_MATERIAL_SECONDARY == item::material::Nothing))
                            {
                                continue;
                            }

                            auto const MAT_PAIR{ std::make_pair(
                                NEXT_MATERIAL_PRIMARY, item::material::Nothing) };

                            if (matPairSet.find(MAT_PAIR) != std::end(matPairSet))
                            {
                                continue;
                            }

                            matPairSet.insert(MAT_PAIR);

                            ItemProfile p;

                            p.SetUnique(
                                NEXT_UNIQUE_ENUM,
                                NEXT_MATERIAL_PRIMARY,
                                NEXT_MATERIAL_SECONDARY,
                                NEXT_ELEMENT_TYPE);

                            p.SetMisc(
                                unique_type::MiscType(NEXT_UNIQUE_ENUM),
                                false,
                                NEXT_MATERIAL_PRIMARY,
                                NEXT_MATERIAL_SECONDARY);

                            AppendProfileToTheCorrectVector(p);
                            didAddProfile = true;
                        }
                    }
                }

                M_ASSERT_OR_LOGANDTHROW_SS(
                    (didAddProfile),
                    "ItemProfileWarehouse::Setup() failed to find any valid "
                        << "material combinations for unique_type="
                        << unique_type::ToString(NEXT_UNIQUE_ENUM));
            }
        }
    }

    void ItemProfileWarehouse::Setup_MiscItems()
    {
        for (int i(1); i < misc_type::Count; ++i)
        {
            auto const NEXT_MISC_ENUM{ static_cast<misc_type::Enum>(i) };

            if (misc_type::IsStandaloneItem(NEXT_MISC_ENUM))
            {
                SetupProfilesForMiscType(NEXT_MISC_ENUM);
            }
        }
    }

    void ItemProfileWarehouse::Setup_NamedEquipment()
    {
        for (int i(1); i < named_type::Count; ++i)
        {
            auto const NEXT_NAMED_ENUM{ static_cast<named_type::Enum>(i) };

            auto const NEXT_NAMED_THINPROFILES_VEC{ ThinProfiles(NEXT_NAMED_ENUM) };

            for (auto const & NEXT_NAMED_THINPROFILE : NEXT_NAMED_THINPROFILES_VEC)
            {
                SetupFromThinProfile(NEXT_NAMED_THINPROFILE, NEXT_NAMED_ENUM, set_type::NotASet);
            }
        }
    }

    void ItemProfileWarehouse::Setup_SetEquipment()
    {
        for (int i(1); i < set_type::Count; ++i)
        {
            auto const NEXT_SET_ENUM{ static_cast<set_type::Enum>(i) };

            auto const NEXT_SET_THINPROFILES_VEC{ ThinProfiles(NEXT_SET_ENUM) };

            for (auto const & NEXT_SET_THINPROFILE : NEXT_SET_THINPROFILES_VEC)
            {
                SetupFromThinProfile(NEXT_SET_THINPROFILE, named_type::NotNamed, NEXT_SET_ENUM);
            }
        }
    }

    void ItemProfileWarehouse::Setup_SummoningItems()
    {
        std::set<ItemProfile> set;

        auto const MATERIALS_VEC{ material::CorePrimaryNoPearl() };

        using namespace creature;
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
                        for (auto const NEXT_MATERIAL : MATERIALS_VEC)
                        {
                            ItemProfile p;

                            p.SetMisc(
                                misc_type::Summoning_Statue,
                                false,
                                NEXT_MATERIAL,
                                material::Nothing,
                                set_type::NotASet);

                            p.SetSummoningAndAdjustScore(
                                SummonInfo(ORIGIN_ENUM, RACE_ENUM, ROLE_ENUM));

                            set.insert(p);
                        }
                    }
                    else
                    {
                        auto const MISC_TYPE{ [ORIGIN_ENUM]() {
                            if (ORIGIN_ENUM == origin_type::Egg)
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

                        ItemProfile p;
                        p.SetMisc(MISC_TYPE, false, MATERIAL, material::Nothing, set_type::NotASet);
                        p.SetSummoningAndAdjustScore(SummonInfo(ORIGIN_ENUM, RACE_ENUM, ROLE_ENUM));
                        set.insert(p);
                    }
                }
            }
        }

        std::copy(std::begin(set), std::end(set), std::back_inserter(profiles_));
    }

    const ItemProfileVec_t & ItemProfileWarehouse::GetNormalProfiles()
    {
        if (IsSetup() == false)
        {
            Setup();
        }

        return profiles_;
    }

    const ItemProfileVec_t & ItemProfileWarehouse::GetReligiousProfiles()
    {
        if (IsSetup() == false)
        {
            Setup();
        }

        return religiousProfiles_;
    }

    Score_t ItemProfileWarehouse::Score(const stats::TraitSet & TRAIT_SET)
    {
        using namespace stats;

        auto score{ 0_score };

        for (int i(1); i < Traits::Count; ++i)
        {
            auto const NEXT_TRAIT_ENUM{ static_cast<Traits::Enum>(i) };
            auto const NEXT_TRAIT_VALUE{ TRAIT_SET.GetCopy(NEXT_TRAIT_ENUM).Current() };
            auto traitScore{ [NEXT_TRAIT_VALUE]() {
                if (NEXT_TRAIT_VALUE >= 0)
                {
                    return 10 * NEXT_TRAIT_VALUE;
                }
                else
                {
                    return 5 * std::abs(NEXT_TRAIT_VALUE);
                }
            }() };

            if ((NEXT_TRAIT_ENUM == Traits::HealthGainAll)
                || (NEXT_TRAIT_ENUM == Traits::HealthGainMelee))
            {
                traitScore *= 10;
            }
            else if (
                (NEXT_TRAIT_ENUM == Traits::AnimalResist)
                || (NEXT_TRAIT_ENUM == Traits::ArmorRating) || (NEXT_TRAIT_ENUM == Traits::Backstab)
                || (NEXT_TRAIT_ENUM == Traits::CurseOnDamage)
                || (NEXT_TRAIT_ENUM == Traits::DamageBonusAll)
                || (NEXT_TRAIT_ENUM == Traits::DamageBonusMelee)
                || (NEXT_TRAIT_ENUM == Traits::DamageBonusProj)
                || (NEXT_TRAIT_ENUM == Traits::FindCoinsAmount)
                || (NEXT_TRAIT_ENUM == Traits::PoisonOnAll)
                || (NEXT_TRAIT_ENUM == Traits::PoisonOnMelee))
            {
                traitScore *= 2;
            }
            else if (NEXT_TRAIT_ENUM == Traits::DamageBonusFist)
            {
                traitScore /= 4;
            }

            score += Score_t(traitScore);
        }

        return score;
    }

    void ItemProfileWarehouse::SetupFromThinProfile(
        const ItemProfile & THIN_PROFILE,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE)
    {
        auto const ELEMENT_TYPES{ [&]() {
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

        auto elementCombinationsVec{ element_type::Combinations(ELEMENT_TYPES) };
        elementCombinationsVec.emplace_back(element_type::None);

        SetupFromThinProfile(
            THIN_PROFILE,
            NAMED_TYPE,
            SET_TYPE,
            elementCombinationsVec,
            GetMaterialsFromThinProfile(THIN_PROFILE, NAMED_TYPE));
    }

    void ItemProfileWarehouse::SetupFromThinProfile(
        const ItemProfile & THIN_PROFILE,
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

        if ((THIN_PROFILE.MiscType() != misc_type::NotMisc)
            && (THIN_PROFILE.MiscType() != misc_type::Count))
        {
            ItemProfile fatProfile;
            fatProfile.SetMisc(
                THIN_PROFILE.MiscType(), false, MATERIAL_PRI, MATERIAL_SEC, SET_TYPE);
            AppendProfileToTheCorrectVector(fatProfile);

            if (misc_type::HasPixieVersion(THIN_PROFILE.MiscType()))
            {
                ItemProfile fatProfilePixie;
                fatProfilePixie.SetMisc(
                    THIN_PROFILE.MiscType(), true, MATERIAL_PRI, MATERIAL_SEC, SET_TYPE);
                AppendProfileToTheCorrectVector(fatProfilePixie);
            }

            return;
        }

        if (THIN_PROFILE.IsAventail())
        {
            ItemProfile fatProfile;
            fatProfile.SetAventail(
                BASE_TYPE, MATERIAL_PRI, MATERIAL_SEC, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE);
            AppendProfileToTheCorrectVector(fatProfile);
            return;
        }

        if (THIN_PROFILE.IsBoots())
        {
            ItemProfile fatProfile;
            fatProfile.SetBoots(
                BASE_TYPE, MATERIAL_PRI, MATERIAL_SEC, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE, false);
            AppendProfileToTheCorrectVector(fatProfile);

            ItemProfile fatProfilePixie;
            fatProfilePixie.SetBoots(
                BASE_TYPE, MATERIAL_PRI, MATERIAL_SEC, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE, true);
            AppendProfileToTheCorrectVector(fatProfilePixie);
            return;
        }

        if (THIN_PROFILE.IsBracer())
        {
            ItemProfile fatProfile;
            fatProfile.SetBracer(
                BASE_TYPE, MATERIAL_PRI, MATERIAL_SEC, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE, false);
            AppendProfileToTheCorrectVector(fatProfile);

            ItemProfile fatProfilePixie;
            fatProfilePixie.SetBracer(
                BASE_TYPE, MATERIAL_PRI, MATERIAL_SEC, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE, true);
            AppendProfileToTheCorrectVector(fatProfilePixie);
            return;
        }

        if (THIN_PROFILE.IsGauntlets())
        {
            ItemProfile fatProfile;
            fatProfile.SetGauntlets(
                BASE_TYPE, MATERIAL_PRI, MATERIAL_SEC, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE, false);
            AppendProfileToTheCorrectVector(fatProfile);

            ItemProfile fatProfilePixie;
            fatProfilePixie.SetGauntlets(
                BASE_TYPE, MATERIAL_PRI, MATERIAL_SEC, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE, true);
            AppendProfileToTheCorrectVector(fatProfilePixie);
            return;
        }

        if (THIN_PROFILE.IsPants())
        {
            ItemProfile fatProfile;
            fatProfile.SetPants(
                BASE_TYPE, MATERIAL_PRI, MATERIAL_SEC, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE, false);
            AppendProfileToTheCorrectVector(fatProfile);

            ItemProfile fatProfilePixie;
            fatProfilePixie.SetPants(
                BASE_TYPE, MATERIAL_PRI, MATERIAL_SEC, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE, true);
            AppendProfileToTheCorrectVector(fatProfilePixie);
            return;
        }

        if (THIN_PROFILE.IsShirt())
        {
            ItemProfile fatProfile;
            fatProfile.SetShirt(
                BASE_TYPE, MATERIAL_PRI, MATERIAL_SEC, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE, false);
            AppendProfileToTheCorrectVector(fatProfile);

            ItemProfile fatProfilePixie;
            fatProfilePixie.SetShirt(
                BASE_TYPE, MATERIAL_PRI, MATERIAL_SEC, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE, true);
            AppendProfileToTheCorrectVector(fatProfilePixie);
            return;
        }

        if (THIN_PROFILE.Category() & item::category::Armor)
        {

            if (THIN_PROFILE.ShieldType() != armor::shield_type::Count)
            {
                ItemProfile fatProfile;
                fatProfile.SetShield(
                    THIN_PROFILE.ShieldType(),
                    MATERIAL_PRI,
                    MATERIAL_SEC,
                    NAMED_TYPE,
                    SET_TYPE,
                    ELEMENT_TYPE);
                AppendProfileToTheCorrectVector(fatProfile);
                return;
            }

            if (THIN_PROFILE.CoverType() != armor::cover_type::Count)
            {
                ItemProfile fatProfile;
                fatProfile.SetCover(
                    THIN_PROFILE.CoverType(),
                    MATERIAL_PRI,
                    MATERIAL_SEC,
                    NAMED_TYPE,
                    SET_TYPE,
                    ELEMENT_TYPE,
                    false);
                AppendProfileToTheCorrectVector(fatProfile);

                ItemProfile fatProfilePixie;
                fatProfilePixie.SetCover(
                    THIN_PROFILE.CoverType(),
                    MATERIAL_PRI,
                    MATERIAL_SEC,
                    NAMED_TYPE,
                    SET_TYPE,
                    ELEMENT_TYPE,
                    true);
                AppendProfileToTheCorrectVector(fatProfilePixie);
                return;
            }

            if (THIN_PROFILE.HelmType() != armor::helm_type::Count)
            {
                ItemProfile fatProfile;
                fatProfile.SetHelm(
                    THIN_PROFILE.HelmType(),
                    MATERIAL_PRI,
                    MATERIAL_SEC,
                    NAMED_TYPE,
                    SET_TYPE,
                    ELEMENT_TYPE);
                AppendProfileToTheCorrectVector(fatProfile);
                return;
            }
        }

        if (THIN_PROFILE.Category() & item::category::Weapon)
        {

            if (THIN_PROFILE.SwordType() != weapon::sword_type::Count)
            {
                ItemProfile fatProfile;
                fatProfile.SetSword(
                    THIN_PROFILE.SwordType(),
                    MATERIAL_PRI,
                    MATERIAL_SEC,
                    NAMED_TYPE,
                    SET_TYPE,
                    ELEMENT_TYPE);
                AppendProfileToTheCorrectVector(fatProfile);
                return;
            }

            if (THIN_PROFILE.AxeType() != weapon::axe_type::Count)
            {
                ItemProfile fatProfile;
                fatProfile.SetAxe(
                    THIN_PROFILE.AxeType(),
                    MATERIAL_PRI,
                    MATERIAL_SEC,
                    NAMED_TYPE,
                    SET_TYPE,
                    ELEMENT_TYPE);
                AppendProfileToTheCorrectVector(fatProfile);
                return;
            }

            if (THIN_PROFILE.ClubType() != weapon::club_type::Count)
            {
                ItemProfile fatProfile;
                fatProfile.SetClub(
                    THIN_PROFILE.ClubType(),
                    MATERIAL_PRI,
                    MATERIAL_SEC,
                    NAMED_TYPE,
                    SET_TYPE,
                    ELEMENT_TYPE);
                AppendProfileToTheCorrectVector(fatProfile);
                return;
            }

            if (THIN_PROFILE.WhipType() != weapon::whip_type::Count)
            {
                ItemProfile fatProfile;
                fatProfile.SetWhip(
                    THIN_PROFILE.WhipType(),
                    MATERIAL_PRI,
                    MATERIAL_SEC,
                    NAMED_TYPE,
                    SET_TYPE,
                    ELEMENT_TYPE);
                AppendProfileToTheCorrectVector(fatProfile);
                return;
            }

            if (THIN_PROFILE.ProjectileType() != weapon::projectile_type::Count)
            {
                ItemProfile fatProfile;
                fatProfile.SetProjectile(
                    THIN_PROFILE.ProjectileType(),
                    MATERIAL_PRI,
                    MATERIAL_SEC,
                    NAMED_TYPE,
                    SET_TYPE,
                    ELEMENT_TYPE);
                AppendProfileToTheCorrectVector(fatProfile);
                return;
            }

            if (THIN_PROFILE.BladedStaffType() != weapon::bladedstaff_type::Count)
            {
                ItemProfile fatProfile;
                fatProfile.SetBladedStaff(
                    THIN_PROFILE.BladedStaffType(),
                    MATERIAL_PRI,
                    MATERIAL_SEC,
                    NAMED_TYPE,
                    SET_TYPE,
                    ELEMENT_TYPE);
                AppendProfileToTheCorrectVector(fatProfile);
                return;
            }

            if (THIN_PROFILE.IsKnife())
            {
                ItemProfile fatProfile;
                fatProfile.SetKnife(
                    THIN_PROFILE.Size(),
                    MATERIAL_PRI,
                    MATERIAL_SEC,
                    NAMED_TYPE,
                    SET_TYPE,
                    ELEMENT_TYPE,
                    false);
                AppendProfileToTheCorrectVector(fatProfile);

                ItemProfile fatProfilePixie;
                fatProfilePixie.SetKnife(
                    THIN_PROFILE.Size(),
                    MATERIAL_PRI,
                    MATERIAL_SEC,
                    NAMED_TYPE,
                    SET_TYPE,
                    ELEMENT_TYPE,
                    true);
                AppendProfileToTheCorrectVector(fatProfilePixie);

                return;
            }

            if (THIN_PROFILE.IsDagger())
            {
                ItemProfile fatProfile;
                fatProfile.SetDagger(
                    THIN_PROFILE.Size(),
                    MATERIAL_PRI,
                    MATERIAL_SEC,
                    NAMED_TYPE,
                    SET_TYPE,
                    ELEMENT_TYPE,
                    false);
                AppendProfileToTheCorrectVector(fatProfile);

                ItemProfile fatProfilePixie;
                fatProfilePixie.SetDagger(
                    THIN_PROFILE.Size(),
                    MATERIAL_PRI,
                    MATERIAL_SEC,
                    NAMED_TYPE,
                    SET_TYPE,
                    ELEMENT_TYPE,
                    true);
                AppendProfileToTheCorrectVector(fatProfilePixie);

                return;
            }

            if (THIN_PROFILE.IsStaff())
            {
                ItemProfile fatProfile;
                fatProfile.SetStaff(MATERIAL_PRI, MATERIAL_SEC, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE);
                AppendProfileToTheCorrectVector(fatProfile);
                return;
            }

            if (THIN_PROFILE.IsQuarterStaff())
            {
                ItemProfile fatProfile;
                fatProfile.SetQuarterStaff(
                    MATERIAL_PRI, MATERIAL_SEC, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE);
                AppendProfileToTheCorrectVector(fatProfile);
                return;
            }
        }

        std::ostringstream ss;
        ss << "ItemProfileWarehouse::SetupFromThinProfile(final version, thin_profile="
           << THIN_PROFILE.ToString() << ") "
           << "-was unable to find the type of the given ThinProfile.";
        throw std::runtime_error(ss.str());
    }

    void ItemProfileWarehouse::SetupFromThinProfile(
        const ItemProfile & THIN_PROFILE,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE,
        const ElementTypeVec_t & ELEMENT_TYPE_VEC,
        const BaseMaterialVecPairVec_t & BASEMATERIAL_VEC_PAIR_VEC)
    {
        M_ASSERT_OR_LOGANDTHROW_SS(
            (BASEMATERIAL_VEC_PAIR_VEC.empty() == false),
            "ItemProfileWarehouse(vector version, thin_profile=" << THIN_PROFILE.ToString()
                                                                 << ") was given an empty "
                                                                 << "BASEMATERIAL_VEC_PAIR_VEC.");

        for (auto const NEXT_ELEMENT_TYPE : ELEMENT_TYPE_VEC)
        {
            for (auto const & NEXT_BASEMATERIALVECPAIR : BASEMATERIAL_VEC_PAIR_VEC)
            {
                M_ASSERT_OR_LOGANDTHROW_SS(
                    (NEXT_BASEMATERIALVECPAIR.second.first.empty() == false),
                    "ItemProfileWarehouse(vector version, thin_profile="
                        << THIN_PROFILE.ToString()
                        << ", named_type=" << named_type::ToString(NAMED_TYPE)
                        << ", set_type=" << set_type::ToString(SET_TYPE)
                        << ", element_type=" << element_type::ToString(NEXT_ELEMENT_TYPE, true)
                        << ", BASEMATERIAL_VEC_PAIR_VEC.size()=" << BASEMATERIAL_VEC_PAIR_VEC.size()
                        << ") was given an empty "
                        << "NEXT_BASEMATERIALVECPAIR.second.first (primary material vec).");

                for (auto const NEXT_MATERIAL_PRI : NEXT_BASEMATERIALVECPAIR.second.first)
                {
                    if (NEXT_BASEMATERIALVECPAIR.second.second.empty())
                    {
                        SetupFromThinProfile(
                            THIN_PROFILE,
                            NAMED_TYPE,
                            SET_TYPE,
                            NEXT_ELEMENT_TYPE,
                            NEXT_MATERIAL_PRI,
                            material::Nothing,
                            NEXT_BASEMATERIALVECPAIR.first);
                    }
                    else
                    {
                        for (auto const NEXT_MATERIAL_SEC : NEXT_BASEMATERIALVECPAIR.second.second)
                        {
                            if (NEXT_MATERIAL_SEC != item::material::Nothing)
                            {
                                SetupFromThinProfile(
                                    THIN_PROFILE,
                                    NAMED_TYPE,
                                    SET_TYPE,
                                    NEXT_ELEMENT_TYPE,
                                    NEXT_MATERIAL_PRI,
                                    NEXT_MATERIAL_SEC,
                                    NEXT_BASEMATERIALVECPAIR.first);
                            }
                        }
                    }
                }
            }
        }
    }

    const BaseMaterialVecPairVec_t ItemProfileWarehouse::GetMaterialsFromThinProfile(
        const ItemProfile & THIN_PROFILE, const named_type::Enum NAMED_TYPE)
    {
        if ((NAMED_TYPE != named_type::NotNamed) && (NAMED_TYPE != named_type::Count))
        {
            auto const NAMED_MATERIALS_VEC_PAIR{ named_type::Materials(NAMED_TYPE) };

            if (NAMED_MATERIALS_VEC_PAIR.first.empty() == false)
            {
                return { std::make_pair(armor::base_type::Count, NAMED_MATERIALS_VEC_PAIR) };
            }
        }

        if ((THIN_PROFILE.MiscType() != misc_type::NotMisc)
            && (THIN_PROFILE.MiscType() != misc_type::Count))
        {
            return { std::make_pair(
                armor::base_type::Count, misc_type::Materials(THIN_PROFILE.MiscType())) };
        }
        else
        {
            if (THIN_PROFILE.IsAventail())
            {
                BaseMaterialVecPairVec_t v;

                for (int i(0); i < armor::base_type::Count; ++i)
                {
                    auto const NEXT_BASE_ENUM{ static_cast<armor::base_type::Enum>(i) };
                    if ((THIN_PROFILE.ArmorTypeRestriction() == NEXT_BASE_ENUM)
                        || (THIN_PROFILE.ArmorTypeRestriction() == armor::base_type::Count))
                    {
                        v.emplace_back(
                            std::make_pair(NEXT_BASE_ENUM, MaterialsAventail(NEXT_BASE_ENUM)));
                    }
                }

                return v;
            }

            if (THIN_PROFILE.IsBoots())
            {
                BaseMaterialVecPairVec_t v;

                for (int i(0); i < armor::base_type::Count; ++i)
                {
                    auto const NEXT_BASE_ENUM{ static_cast<armor::base_type::Enum>(i) };
                    if ((THIN_PROFILE.ArmorTypeRestriction() == NEXT_BASE_ENUM)
                        || (THIN_PROFILE.ArmorTypeRestriction() == armor::base_type::Count))
                    {
                        v.emplace_back(
                            std::make_pair(NEXT_BASE_ENUM, MaterialsBoots(NEXT_BASE_ENUM)));
                    }
                }

                return v;
            }

            if (THIN_PROFILE.IsBracer())
            {
                BaseMaterialVecPairVec_t v;

                for (int i(0); i < armor::base_type::Count; ++i)
                {
                    auto const NEXT_BASE_ENUM{ static_cast<armor::base_type::Enum>(i) };
                    if ((THIN_PROFILE.ArmorTypeRestriction() == NEXT_BASE_ENUM)
                        || (THIN_PROFILE.ArmorTypeRestriction() == armor::base_type::Count))
                    {
                        v.emplace_back(
                            std::make_pair(NEXT_BASE_ENUM, MaterialsBracer(NEXT_BASE_ENUM)));
                    }
                }

                return v;
            }

            if (THIN_PROFILE.IsGauntlets())
            {
                BaseMaterialVecPairVec_t v;

                for (int i(0); i < armor::base_type::Count; ++i)
                {
                    auto const NEXT_BASE_ENUM{ static_cast<armor::base_type::Enum>(i) };
                    if ((THIN_PROFILE.ArmorTypeRestriction() == NEXT_BASE_ENUM)
                        || (THIN_PROFILE.ArmorTypeRestriction() == armor::base_type::Count))
                    {
                        v.emplace_back(
                            std::make_pair(NEXT_BASE_ENUM, MaterialsGauntlets(NEXT_BASE_ENUM)));
                    }
                }

                return v;
            }

            if (THIN_PROFILE.IsPants())
            {
                BaseMaterialVecPairVec_t v;

                for (int i(0); i < armor::base_type::Count; ++i)
                {
                    auto const NEXT_BASE_ENUM{ static_cast<armor::base_type::Enum>(i) };
                    if ((THIN_PROFILE.ArmorTypeRestriction() == NEXT_BASE_ENUM)
                        || (THIN_PROFILE.ArmorTypeRestriction() == armor::base_type::Count))
                    {
                        v.emplace_back(
                            std::make_pair(NEXT_BASE_ENUM, MaterialsPants(NEXT_BASE_ENUM)));
                    }
                }

                return v;
            }

            if (THIN_PROFILE.IsShirt())
            {
                BaseMaterialVecPairVec_t v;

                for (int i(0); i < armor::base_type::Count; ++i)
                {
                    auto const NEXT_BASE_ENUM{ static_cast<armor::base_type::Enum>(i) };
                    if ((THIN_PROFILE.ArmorTypeRestriction() == NEXT_BASE_ENUM)
                        || (THIN_PROFILE.ArmorTypeRestriction() == armor::base_type::Count))
                    {
                        v.emplace_back(
                            std::make_pair(NEXT_BASE_ENUM, MaterialsShirt(NEXT_BASE_ENUM)));
                    }
                }

                return v;
            }

            if (THIN_PROFILE.Category() & item::category::Armor)
            {
                if (THIN_PROFILE.ShieldType() != armor::shield_type::Count)
                {
                    return { std::make_pair(
                        armor::base_type::Count, Materials(THIN_PROFILE.ShieldType())) };
                }

                if (THIN_PROFILE.CoverType() != armor::cover_type::Count)
                {
                    return { std::make_pair(
                        armor::base_type::Count, Materials(THIN_PROFILE.CoverType())) };
                }

                if (THIN_PROFILE.HelmType() != armor::helm_type::Count)
                {
                    return { std::make_pair(
                        armor::base_type::Count, Materials(THIN_PROFILE.HelmType())) };
                }
            }

            if (THIN_PROFILE.Category() & item::category::Weapon)
            {
                if (THIN_PROFILE.SwordType() != weapon::sword_type::Count)
                {
                    return { std::make_pair(
                        armor::base_type::Count, Materials(THIN_PROFILE.SwordType())) };
                }

                if (THIN_PROFILE.AxeType() != weapon::axe_type::Count)
                {
                    return { std::make_pair(
                        armor::base_type::Count, Materials(THIN_PROFILE.AxeType())) };
                }

                if (THIN_PROFILE.ClubType() != weapon::club_type::Count)
                {
                    return { std::make_pair(
                        armor::base_type::Count, Materials(THIN_PROFILE.ClubType())) };
                }

                if (THIN_PROFILE.WhipType() != weapon::whip_type::Count)
                {
                    return { std::make_pair(
                        armor::base_type::Count, Materials(THIN_PROFILE.WhipType())) };
                }

                if (THIN_PROFILE.ProjectileType() != weapon::projectile_type::Count)
                {
                    return { std::make_pair(
                        armor::base_type::Count, Materials(THIN_PROFILE.ProjectileType())) };
                }

                if (THIN_PROFILE.BladedStaffType() != weapon::bladedstaff_type::Count)
                {
                    return { std::make_pair(
                        armor::base_type::Count, Materials(THIN_PROFILE.BladedStaffType())) };
                }
            }

            if (THIN_PROFILE.IsKnife())
            {
                return { std::make_pair(armor::base_type::Count, MaterialsKnife()) };
            }

            if (THIN_PROFILE.IsDagger())
            {
                return { std::make_pair(armor::base_type::Count, MaterialsDagger()) };
            }

            if (THIN_PROFILE.IsStaff())
            {
                return { std::make_pair(armor::base_type::Count, MaterialsStaff()) };
            }

            if (THIN_PROFILE.IsQuarterStaff())
            {
                return { std::make_pair(armor::base_type::Count, MaterialsQuarterStaff()) };
            }

            std::ostringstream ss;
            ss << "ItemProfileWarehouse::GetMaterialsFromThinProfile(thin_profile="
               << THIN_PROFILE.ToString() << ") "
               << "-was unable to find the type of the given ThinProfile.";
            throw std::runtime_error(ss.str());
        }
    }

    void ItemProfileWarehouse::SetupProfilesForMiscType(const misc_type::Enum E)
    {
        auto const MATERIALS{ misc_type::Materials(E) };

        M_ASSERT_OR_LOGANDTHROW_SS(
            (MATERIALS.first.empty() == false),
            "ItemProfileWarehouse::SetupProfilesForMiscType() failed to find any "
                << " primary materials for misc_type=" << misc_type::ToString(E));

        auto const BEFORE_COUNT{ profiles_.size() + religiousProfiles_.size() };

        for (auto const NEXT_MATERIAL_PRIMARY : MATERIALS.first)
        {
            if (MATERIALS.second.empty())
            {
                ItemProfile p;
                p.SetMisc(E, false, NEXT_MATERIAL_PRIMARY, material::Nothing);
                AppendProfileToTheCorrectVector(p);

                if (misc_type::HasPixieVersion(E))
                {
                    ItemProfile pPixie;
                    pPixie.SetMisc(E, true, NEXT_MATERIAL_PRIMARY, material::Nothing);
                    AppendProfileToTheCorrectVector(pPixie);
                }
            }
            else
            {
                for (auto const NEXT_MATERIAL_SECONDARY : MATERIALS.second)
                {
                    if (NEXT_MATERIAL_PRIMARY == NEXT_MATERIAL_SECONDARY)
                    {
                        continue;
                    }

                    ItemProfile p;
                    p.SetMisc(E, false, NEXT_MATERIAL_PRIMARY, NEXT_MATERIAL_SECONDARY);
                    AppendProfileToTheCorrectVector(p);

                    if (misc_type::HasPixieVersion(E))
                    {
                        ItemProfile pPixie;
                        pPixie.SetMisc(E, true, NEXT_MATERIAL_PRIMARY, NEXT_MATERIAL_SECONDARY);
                        AppendProfileToTheCorrectVector(pPixie);
                    }
                }
            }
        }

        auto const AFTER_COUNT{ profiles_.size() + religiousProfiles_.size() };

        M_ASSERT_OR_LOGANDTHROW_SS(
            (BEFORE_COUNT != AFTER_COUNT),
            "ItemProfileWarehouse::SetupProfilesForMiscType() failed to find any"
                << " valid material combinations for misc_type=" << misc_type::ToString(E));
    }

    Score_t ItemProfileWarehouse::Score(const material::Enum PRI, const material::Enum SEC)
    {
        auto const BONUS_PRI{ [PRI]() {
            if ((PRI == material::Nothing) || (PRI == material::Count))
            {
                return 0;
            }
            else
            {
                return material::Bonus(PRI);
            }
        }() };

        auto const BONUS_SEC{ [SEC]() {
            if ((SEC == material::Nothing) || (SEC == material::Count))
            {
                return 0;
            }
            else
            {
                return material::Bonus(SEC);
            }
        }() };

        return Score_t(50 + (BONUS_PRI * BONUS_PRI) + ((BONUS_SEC * BONUS_SEC) / 3));
    }

    Score_t ItemProfileWarehouse::Score(const weapon::axe_type::Enum E)
    {
        using namespace weapon;

        switch (E)
        {
            case axe_type::Handaxe:
            {
                return 50_score;
            }
            case axe_type::Sickle:
            {
                return 80_score;
            }
            case axe_type::Battleaxe:
            {
                return 100_score;
            }
            case axe_type::Waraxe:
            {
                return 200_score;
            }
            case axe_type::Count:
            default:
            {
                return 0_score;
            }
        }
    }

    Score_t ItemProfileWarehouse::Score(const weapon::bladedstaff_type::Enum E)
    {
        using namespace weapon;

        switch (E)
        {
            case bladedstaff_type::Spear:
            {
                return 70_score;
            }
            case bladedstaff_type::ShortSpear:
            {
                return 50_score;
            }
            case bladedstaff_type::Scythe:
            {
                return 80_score;
            }
            case bladedstaff_type::Pike:
            {
                return 100_score;
            }
            case bladedstaff_type::Partisan:
            {
                return 150_score;
            }
            case bladedstaff_type::Halberd:
            {
                return 200_score;
            }
            case bladedstaff_type::Count:
            default:
            {
                return 0_score;
            }
        }
    }

    Score_t ItemProfileWarehouse::Score(const weapon::club_type::Enum E)
    {
        using namespace weapon;

        switch (E)
        {
            case club_type::Spiked:
            {
                return 50_score;
            }
            case club_type::Maul:
            {
                return 60_score;
            }
            case club_type::Mace:
            {
                return 80_score;
            }
            case club_type::Warhammer:
            {
                return 120_score;
            }
            case club_type::Count:
            default:
            {
                return 0_score;
            }
        }
    }

    Score_t ItemProfileWarehouse::Score(const weapon::projectile_type::Enum E)
    {
        using namespace weapon;

        switch (E)
        {
            case projectile_type::Blowpipe:
            {
                return 50_score;
            }
            case projectile_type::Sling:
            {
                return 60_score;
            }
            case projectile_type::Shortbow:
            {
                return 100_score;
            }
            case projectile_type::Longbow:
            {
                return 200_score;
            }
            case projectile_type::CompositeBow:
            {
                return 300_score;
            }
            case projectile_type::Crossbow:
            {
                return 300_score;
            }
            case projectile_type::Count:
            default:
            {
                return 0_score;
            }
        }
    }

    Score_t ItemProfileWarehouse::Score(const weapon::sword_type::Enum E)
    {
        using namespace weapon;

        switch (E)
        {
            case sword_type::Claymore:
            {
                return 200_score;
            }
            case sword_type::Longsword:
            {
                return 120_score;
            }
            case sword_type::Flamberg:
            {
                return 190_score;
            }
            case sword_type::Knightlysword:
            {
                return 110_score;
            }
            case sword_type::Broadsword:
            {
                return 100_score;
            }
            case sword_type::Falcata:
            {
                return 50_score;
            }
            case sword_type::Saber:
            {
                return 70_score;
            }
            case sword_type::Cutlass:
            {
                return 40_score;
            }
            case sword_type::Rapier:
            {
                return 70_score;
            }
            case sword_type::Gladius:
            {
                return 55_score;
            }
            case sword_type::Shortsword:
            {
                return 50_score;
            }
            case sword_type::Count:
            default:
            {
                return 0_score;
            }
        }
    }

    Score_t ItemProfileWarehouse::Score(const weapon::whip_type::Enum E)
    {
        using namespace weapon;

        switch (E)
        {
            case whip_type::Bullwhip:
            {
                return 30_score;
            }
            case whip_type::Flail:
            {
                return 60_score;
            }
            case whip_type::MaceAndChain:
            {
                return 90_score;
            }
            case whip_type::Count:
            default:
            {
                return 0_score;
            }
        }
    }

    Score_t ItemProfileWarehouse::ScoreKnife(const sfml_util::Size::Enum E)
    {
        if (E == sfml_util::Size::Small)
        {
            return 20_score;
        }

        return 25_score;
    }

    Score_t ItemProfileWarehouse::ScoreDagger(const sfml_util::Size::Enum E)
    {
        if (E == sfml_util::Size::Small)
        {
            return 30_score;
        }

        return 35_score;
    }

    Score_t ItemProfileWarehouse::ScoreStaff() { return 25_score; }

    Score_t ItemProfileWarehouse::ScoreQuarterStaff() { return 15_score; }

    Score_t ItemProfileWarehouse::Score(const armor::base_type::Enum E)
    {
        using namespace armor;

        switch (E)
        {
            case base_type::Mail:
            {
                return 75_score;
            }
            case base_type::Scale:
            {
                return 125_score;
            }
            case base_type::Plate:
            {
                return 175_score;
            }
            case base_type::Plain:
            case base_type::Count:
            default:
            {
                return 25_score;
            }
        }
    }

    Score_t ItemProfileWarehouse::Score(const armor::cover_type::Enum E)
    {
        using namespace armor;

        switch (E)
        {
            case cover_type::Cape:
            {
                return 25_score;
            }
            case cover_type::Vest:
            {
                return 50_score;
            }
            case cover_type::Robe:
            {
                return 75_score;
            }
            case cover_type::Cloak:
            {
                return 100_score;
            }
            case cover_type::Count:
            default:
            {
                return 0_score;
            }
        }
    }

    Score_t ItemProfileWarehouse::Score(const armor::helm_type::Enum E)
    {
        using namespace armor;

        switch (E)
        {
            case helm_type::Leather:
            {
                return 25_score;
            }
            case helm_type::MailCoif:
            {
                return 50_score;
            }
            case helm_type::Kettle:
            {
                return 75_score;
            }
            case helm_type::Archers:
            {
                return 100_score;
            }
            case helm_type::Bascinet:
            {
                return 125_score;
            }
            case helm_type::Great:
            {
                return 150_score;
            }
            case helm_type::Count:
            default:
            {
                return 0_score;
            }
        }
    }

    Score_t ItemProfileWarehouse::Score(const armor::shield_type::Enum E)
    {
        using namespace armor;

        switch (E)
        {
            case shield_type::Buckler:
            {
                return 50_score;
            }
            case shield_type::Kite:
            {
                return 100_score;
            }
            case shield_type::Heater:
            {
                return 150_score;
            }
            case shield_type::Pavis:
            {
                return 200_score;
            }
            case shield_type::Count:
            default:
            {
                return 0_score;
            }
        }
    }

    Score_t ItemProfileWarehouse::ScoreAventail(armor::base_type::Enum E)
    {
        return 34_score + Score(E);
    }

    Score_t ItemProfileWarehouse::ScoreBracer(armor::base_type::Enum E)
    {
        return 36_score + Score(E);
    }

    Score_t ItemProfileWarehouse::ScoreShirt(armor::base_type::Enum E)
    {
        return 44_score + Score(E);
    }

    Score_t ItemProfileWarehouse::ScoreBoots(armor::base_type::Enum E)
    {
        return 46_score + Score(E);
    }

    Score_t ItemProfileWarehouse::ScorePants(armor::base_type::Enum E)
    {
        return 55_score + Score(E);
    }

    Score_t ItemProfileWarehouse::ScoreGauntlets(armor::base_type::Enum E)
    {
        return 30_score + Score(E);
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

    const MaterialVecPair_t ItemProfileWarehouse::Materials(const weapon::projectile_type::Enum E)
    {
        using namespace item;
        using namespace weapon;

        switch (E)
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
            case projectile_type::Shortbow:
            case projectile_type::Longbow:
            {
                return MaterialVecPair_t({ material::Wood }, material::CoreSecondary());
            }
            case projectile_type::CompositeBow:
            {
                return MaterialVecPair_t({ material::Horn }, material::CoreSecondary());
            }
            case projectile_type::Crossbow:
            {
                return MaterialVecPair_t({ material::Wood }, material::CoreSecondary());
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

    const MaterialVecPair_t ItemProfileWarehouse::Materials(const weapon::whip_type::Enum E)
    {
        using namespace item;
        using namespace weapon;

        if (E == whip_type::Bullwhip)
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

    const MaterialVecPair_t ItemProfileWarehouse::Materials(const armor::helm_type::Enum E)
    {
        using namespace item;
        using namespace armor;

        if (E == helm_type::Leather)
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

    const MaterialVecPair_t ItemProfileWarehouse::MaterialsAventail(armor::base_type::Enum E)
    {
        return MaterialsFromBaseType(E);
    }

    const MaterialVecPair_t ItemProfileWarehouse::MaterialsBracer(armor::base_type::Enum E)
    {
        return MaterialsFromBaseType(E);
    }

    const MaterialVecPair_t ItemProfileWarehouse::MaterialsShirt(armor::base_type::Enum E)
    {
        return MaterialsFromBaseType(E);
    }

    const MaterialVecPair_t ItemProfileWarehouse::MaterialsBoots(armor::base_type::Enum E)
    {
        return MaterialsFromBaseTypeNoCloth(E);
    }

    const MaterialVecPair_t ItemProfileWarehouse::MaterialsPants(armor::base_type::Enum E)
    {
        return MaterialsFromBaseType(E);
    }

    const MaterialVecPair_t ItemProfileWarehouse::MaterialsGauntlets(armor::base_type::Enum E)
    {
        return MaterialsFromBaseType(E);
    }

    const MaterialVec_t
        ItemProfileWarehouse::MaterialsPrimaryFromArmorBaseType(const armor::base_type::Enum E)
    {
        using namespace item;
        using namespace armor;

        switch (E)
        {
            case base_type::Scale:
            {
                return { material::Scale };
            }
            case base_type::Mail:
            case base_type::Plate:
            {
                return { material::CoreMetal() };
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
        ItemProfileWarehouse::MaterialsFromBaseType(const armor::base_type::Enum E)
    {
        return MaterialVecPair_t(MaterialsPrimaryFromArmorBaseType(E), material::CoreSecondary());
    }

    const MaterialVecPair_t
        ItemProfileWarehouse::MaterialsFromBaseTypeNoCloth(const armor::base_type::Enum E)
    {
        using namespace item;
        using namespace armor;

        MaterialVec_t v;

        if (E == base_type::Plain)
        {
            v.emplace_back(material::SoftLeather);
            v.emplace_back(material::HardLeather);
        }
        else
        {
            v = MaterialsPrimaryFromArmorBaseType(E);
        }

        return MaterialVecPair_t(v, material::CoreSecondary());
    }

    const ItemProfileVec_t ItemProfileWarehouse::ThinProfilesWeaponsSwords()
    {
        using namespace item;
        using namespace weapon;

        ItemProfileVec_t v;

        for (int i(0); i < sword_type::Count; ++i)
        {
            ItemProfile thinProfile;
            thinProfile.SetSword(static_cast<sword_type::Enum>(i));
            v.emplace_back(thinProfile);
        }

        return v;
    }

    const ItemProfileVec_t ItemProfileWarehouse::ThinProfilesWeaponsProjectiles()
    {
        using namespace item;
        using namespace weapon;

        ItemProfileVec_t v;

        for (int i(0); i < projectile_type::Count; ++i)
        {
            ItemProfile thinProfile;
            thinProfile.SetProjectile(static_cast<projectile_type::Enum>(i));
            v.emplace_back(thinProfile);
        }

        return v;
    }

    const ItemProfileVec_t ItemProfileWarehouse::ThinProfilesWeaponsAll()
    {
        using namespace item;
        using namespace weapon;

        ItemProfileVec_t v;

        auto const SWORD_THIN_PROFILES_VEC{ ThinProfilesWeaponsSwords() };

        std::copy(
            SWORD_THIN_PROFILES_VEC.begin(), SWORD_THIN_PROFILES_VEC.end(), std::back_inserter(v));

        auto const PROJ_THIN_PROFILES_VEC{ ThinProfilesWeaponsProjectiles() };

        std::copy(
            PROJ_THIN_PROFILES_VEC.begin(), PROJ_THIN_PROFILES_VEC.end(), std::back_inserter(v));

        for (int i(0); i < axe_type::Count; ++i)
        {
            ItemProfile thinProfile;
            thinProfile.SetAxe(static_cast<axe_type::Enum>(i));
            v.emplace_back(thinProfile);
        }

        for (int i(0); i < bladedstaff_type::Count; ++i)
        {
            ItemProfile thinProfile;
            thinProfile.SetBladedStaff(static_cast<bladedstaff_type::Enum>(i));
            v.emplace_back(thinProfile);
        }

        for (int i(0); i < club_type::Count; ++i)
        {
            ItemProfile thinProfile;
            thinProfile.SetClub(static_cast<club_type::Enum>(i));
            v.emplace_back(thinProfile);
        }

        for (int i(0); i < whip_type::Count; ++i)
        {
            ItemProfile thinProfile;
            thinProfile.SetWhip(static_cast<whip_type::Enum>(i));
            v.emplace_back(thinProfile);
        }

        for (int i(0); i < sfml_util::Size::Count; ++i)
        {
            ItemProfile thinProfile;
            thinProfile.SetKnife(static_cast<sfml_util::Size::Enum>(i));
            v.emplace_back(thinProfile);
        }

        for (int i(0); i < sfml_util::Size::Count; ++i)
        {
            ItemProfile thinProfile;
            thinProfile.SetDagger(static_cast<sfml_util::Size::Enum>(i));
            v.emplace_back(thinProfile);
        }

        {
            ItemProfile thinProfile;
            thinProfile.SetStaff(material::Nothing);
            v.emplace_back(thinProfile);
        }

        {
            ItemProfile thinProfile;
            thinProfile.SetQuarterStaff(material::Nothing);
            v.emplace_back(thinProfile);
        }

        return v;
    }

    const ItemProfileVec_t
        ItemProfileWarehouse::ThinProfilesArmor(const bool WILL_INCLUDE_COVERINGS)
    {
        using namespace item;
        using namespace armor;

        ItemProfileVec_t v;

        if (WILL_INCLUDE_COVERINGS)
        {
            for (int i(0); i < cover_type::Count; ++i)
            {
                ItemProfile thinProfile;
                thinProfile.SetCover(static_cast<cover_type::Enum>(i));
                v.emplace_back(thinProfile);
            }
        }

        for (int i(0); i < helm_type::Count; ++i)
        {
            ItemProfile thinProfile;
            thinProfile.SetHelm(static_cast<helm_type::Enum>(i));
            v.emplace_back(thinProfile);
        }

        for (int i(0); i < shield_type::Count; ++i)
        {
            ItemProfile thinProfile;
            thinProfile.SetShield(static_cast<shield_type::Enum>(i));
            v.emplace_back(thinProfile);
        }

        {
            ItemProfile thinProfile;
            thinProfile.SetAventail(base_type::Count);
            v.emplace_back(thinProfile);
        }

        {
            ItemProfile thinProfile;
            thinProfile.SetBoots(base_type::Count);
            v.emplace_back(thinProfile);
        }

        {
            ItemProfile thinProfile;
            thinProfile.SetBracer(base_type::Count);
            v.emplace_back(thinProfile);
        }

        {
            ItemProfile thinProfile;
            thinProfile.SetGauntlets(base_type::Count);
            v.emplace_back(thinProfile);
        }

        {
            ItemProfile thinProfile;
            thinProfile.SetPants(base_type::Count);
            v.emplace_back(thinProfile);
        }

        {
            ItemProfile thinProfile;
            thinProfile.SetShirt(base_type::Count);
            v.emplace_back(thinProfile);
        }

        return v;
    }

    const ItemProfileVec_t ItemProfileWarehouse::ThinProfiles(const named_type::Enum E)
    {
        using namespace item;
        using namespace armor;
        using namespace weapon;

        switch (E)
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
                ItemProfileVec_t v;
                v.reserve(10);

                {
                    ItemProfile thinProfile;
                    thinProfile.SetShield(shield_type::Buckler);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetShield(shield_type::Kite);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetHelm(helm_type::Great);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetHelm(helm_type::Bascinet);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetHelm(helm_type::Kettle);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetAxe(axe_type::Handaxe);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetBladedStaff(bladedstaff_type::Spear);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetSword(sword_type::Gladius);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetSword(sword_type::Shortsword);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetSword(sword_type::Broadsword);
                    v.emplace_back(thinProfile);
                }

                return v;
            }

            case named_type::Soldiers:
            {
                ItemProfileVec_t thinProfiles{ ThinProfilesWeaponsAll() };

                auto const THINPROFILES_ARMOR_VEC{ ThinProfilesArmor(false) };

                thinProfiles.reserve(thinProfiles.size() + THINPROFILES_ARMOR_VEC.size());

                std::copy(
                    THINPROFILES_ARMOR_VEC.begin(),
                    THINPROFILES_ARMOR_VEC.end(),
                    std::back_inserter(thinProfiles));

                return thinProfiles;
            }

            case named_type::Wardens:
            {
                ItemProfileVec_t v{ ThinProfilesArmor(false) };

                for (auto nextThinProfileArmor : v)
                {
                    nextThinProfileArmor.SetArmorTypeRestriction(armor::base_type::Plain);
                }

                v.erase(
                    std::remove_if(
                        v.begin(),
                        v.end(),
                        [](const ItemProfile & P) { return (P.ArmorType() & armor_type::Sheild); }),
                    v.end());

                v.erase(
                    std::remove_if(
                        v.begin(),
                        v.end(),
                        [](const ItemProfile & P) { return (P.ArmorType() & armor_type::Helm); }),
                    v.end());

                {
                    ItemProfile thinProfile;
                    thinProfile.SetSword(sword_type::Shortsword);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetHelm(helm_type::Bascinet);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetShield(shield_type::Buckler);
                    v.emplace_back(thinProfile);
                }

                return v;
            }

            case named_type::Princes:
            {
                ItemProfileVec_t v{ ThinProfilesArmor(false) };

                for (auto nextThinProfileArmor : v)
                {
                    nextThinProfileArmor.SetArmorTypeRestriction(armor::base_type::Mail);
                }

                v.erase(
                    std::remove_if(
                        v.begin(),
                        v.end(),
                        [](const ItemProfile & P) { return (P.ArmorType() & armor_type::Sheild); }),
                    v.end());

                v.erase(
                    std::remove_if(
                        v.begin(),
                        v.end(),
                        [](const ItemProfile & P) { return (P.ArmorType() & armor_type::Helm); }),
                    v.end());

                {
                    ItemProfile thinProfile;
                    thinProfile.SetSword(sword_type::Rapier);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetHelm(helm_type::MailCoif);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetShield(shield_type::Kite);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetCover(cover_type::Cape);
                    v.emplace_back(thinProfile);
                }

                return v;
            }

            case named_type::Ranger:
            {
                ItemProfileVec_t v{ ThinProfilesArmor(false) };

                for (auto nextThinProfileArmor : v)
                {
                    nextThinProfileArmor.SetArmorTypeRestriction(armor::base_type::Plain);
                }

                v.erase(
                    std::remove_if(
                        v.begin(),
                        v.end(),
                        [](const ItemProfile & P) { return (P.ArmorType() & armor_type::Sheild); }),
                    v.end());

                v.erase(
                    std::remove_if(
                        v.begin(),
                        v.end(),
                        [](const ItemProfile & P) { return (P.ArmorType() & armor_type::Helm); }),
                    v.end());

                {
                    ItemProfile thinProfile;
                    thinProfile.SetProjectile(projectile_type::Longbow);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetCover(cover_type::Cloak);
                    v.emplace_back(thinProfile);
                }

                return v;
            }

            case named_type::Samurai:
            {
                ItemProfileVec_t v{ ThinProfilesArmor(false) };

                for (auto nextThinProfileArmor : v)
                {
                    nextThinProfileArmor.SetArmorTypeRestriction(armor::base_type::Scale);
                }

                v.erase(
                    std::remove_if(
                        v.begin(),
                        v.end(),
                        [](const ItemProfile & P) { return (P.ArmorType() & armor_type::Sheild); }),
                    v.end());

                v.erase(
                    std::remove_if(
                        v.begin(),
                        v.end(),
                        [](const ItemProfile & P) { return (P.ArmorType() & armor_type::Helm); }),
                    v.end());

                {
                    ItemProfile thinProfile;
                    thinProfile.SetSword(sword_type::Longsword);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetHelm(helm_type::Leather);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetCover(cover_type::Cape);
                    v.emplace_back(thinProfile);
                }

                return v;
            }

            case named_type::Thors:
            {
                ItemProfileVec_t v{ ThinProfilesArmor(false) };

                v.erase(
                    std::remove_if(
                        v.begin(),
                        v.end(),
                        [](const ItemProfile & P) { return (P.ArmorType() & armor_type::Sheild); }),
                    v.end());

                v.erase(
                    std::remove_if(
                        v.begin(),
                        v.end(),
                        [](const ItemProfile & P) { return (P.ArmorType() & armor_type::Helm); }),
                    v.end());

                {
                    ItemProfile thinProfile;
                    thinProfile.SetSword(sword_type::Broadsword);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetHelm(helm_type::Great);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetShield(shield_type::Pavis);
                    v.emplace_back(thinProfile);
                }

                return v;
            }
            case named_type::Nile:
            {
                ItemProfileVec_t v;

                {
                    ItemProfile thinProfile;
                    thinProfile.SetSword(sword_type::Falcata);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetShield(shield_type::Buckler);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetHelm(helm_type::Leather);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetBracer();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.emplace_back(thinProfile);
                }

                return v;
            }

            case named_type::Imposters:
            case named_type::Pickpocket:
            {
                ItemProfileVec_t v;

                {
                    ItemProfile thinProfile;
                    thinProfile.SetCover(cover_type::Robe);
                    v.emplace_back(thinProfile);
                }

                return v;
            }

            case named_type::Burglar:
            case named_type::Mountebank:
            {
                ItemProfileVec_t v;

                {
                    ItemProfile thinProfile;
                    thinProfile.SetCover(cover_type::Cloak);
                    v.emplace_back(thinProfile);
                }

                return v;
            }

            case named_type::Charlatans:
            {
                ItemProfileVec_t v;

                {
                    ItemProfile thinProfile;
                    thinProfile.SetCover(cover_type::Vest);
                    v.emplace_back(thinProfile);
                }

                return v;
            }

            case named_type::Robbers:
            case named_type::Thugs:
            case named_type::Knaves:
            {
                ItemProfileVec_t v;

                {
                    ItemProfile thinProfile;
                    thinProfile.SetKnife();
                    v.emplace_back(thinProfile);
                }

                return v;
            }

            case named_type::Muggers:
            case named_type::Thief:
            case named_type::Pirate:
            {
                ItemProfileVec_t v;

                {
                    ItemProfile thinProfile;
                    thinProfile.SetDagger();
                    v.emplace_back(thinProfile);
                }

                return v;
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

    const ItemProfileVec_t ItemProfileWarehouse::ThinProfiles(const set_type::Enum E)
    {
        using namespace item;

        switch (E)
        {
            case set_type::TheAssassins:
            case set_type::TheTickler:
            case set_type::TheNeverwinter:
            {
                ItemProfileVec_t v;

                {
                    ItemProfile thinProfile;
                    thinProfile.SetBoots();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetPants();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetShirt();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetGauntlets();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetCover(armor::cover_type::Cloak);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetDagger();
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetMisc(misc_type::LockPicks);
                    v.emplace_back(thinProfile);
                }

                return v;
            }

            case set_type::TheMagus:
            case set_type::TheNecromancers:
            case set_type::TheWarlocks:
            {
                ItemProfileVec_t v;

                {
                    ItemProfile thinProfile;
                    thinProfile.SetBoots();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetPants();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetShirt();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetGauntlets();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetCover(armor::cover_type::Robe);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetMisc(misc_type::Wand);
                    v.emplace_back(thinProfile);
                }

                return v;
            }

            case set_type::TheLichKings:
            {
                ItemProfileVec_t v;

                {
                    ItemProfile thinProfile;
                    thinProfile.SetBoots();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetPants();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetShirt();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetGauntlets();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetCover(armor::cover_type::Robe);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetMisc(misc_type::Litch_Hand);
                    v.emplace_back(thinProfile);
                }

                return v;
            }

            case set_type::TheSages:
            case set_type::TheShamans:
            case set_type::TheOracles:
            {
                ItemProfileVec_t v;

                {
                    ItemProfile thinProfile;
                    thinProfile.SetBoots();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetPants();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetShirt();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetGauntlets();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetCover(armor::cover_type::Robe);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetMisc(misc_type::Wand);
                    v.emplace_back(thinProfile);
                }

                return v;
            }

            case set_type::TheAngelic:
            {
                ItemProfileVec_t v;

                {
                    ItemProfile thinProfile;
                    thinProfile.SetBoots();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetPants();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetShirt();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetGauntlets();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetCover(armor::cover_type::Robe);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetMisc(misc_type::Braid);
                    v.emplace_back(thinProfile);
                }

                return v;
            }

            case set_type::TheBalladeers:
            case set_type::TheTroubadours:
            case set_type::TheMuses:
            {
                ItemProfileVec_t v;

                {
                    ItemProfile thinProfile;
                    thinProfile.SetBoots();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Mail);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetPants();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Mail);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetShirt();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Mail);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetGauntlets();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetCover(armor::cover_type::Vest);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetSword(weapon::sword_type::Longsword);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetMisc(misc_type::DrumLute);
                    v.emplace_back(thinProfile);
                }

                return v;
            }

            case set_type::TheCavaliers:
            case set_type::TheChampions:
            case set_type::ThePaladins:
            case set_type::TheBerserkers:
            case set_type::TheRosewood:
            case set_type::TheDragonslayers:
            case set_type::TheEventideRider:
            {
                ItemProfileVec_t v;

                {
                    ItemProfile thinProfile;
                    thinProfile.SetBoots();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plate);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetPants();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plate);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetShirt();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plate);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetGauntlets();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plate);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetBracer();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plate);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetCover(armor::cover_type::Cape);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetShield(armor::shield_type::Pavis);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetHelm(armor::helm_type::Great);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetAventail();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plate);
                    v.emplace_back(thinProfile);
                }

                return v;
            }

            case set_type::TheHunters:
            case set_type::TheSureShot:
            case set_type::TheMarksmans:
            case set_type::TheDeadeye:
            case set_type::TheDuskRanger:
            case set_type::TheVenomBow:
            {
                ItemProfileVec_t v;

                {
                    ItemProfile thinProfile;
                    thinProfile.SetBoots();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Mail);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetPants();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Mail);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetShirt();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Mail);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetGauntlets();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetBracer();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Mail);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetCover(armor::cover_type::Vest);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetHelm(armor::helm_type::Archers);
                    v.emplace_back(thinProfile);
                }

                return v;
            }

            case set_type::TheCritterChannelers:
            case set_type::TheMammalianHead:
            case set_type::TheSavageTaskmasters:
            case set_type::TheMonsterOverseers:
            case set_type::TheBeastRulers:
            {
                ItemProfileVec_t v;

                {
                    ItemProfile thinProfile;
                    thinProfile.SetBoots();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Scale);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetPants();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Scale);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetShirt();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Scale);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetBracer();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Scale);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetGauntlets();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Scale);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetShield(armor::shield_type::Kite);
                    v.emplace_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetHelm(armor::helm_type::Bascinet);
                    v.emplace_back(thinProfile);
                }

                return v;
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
