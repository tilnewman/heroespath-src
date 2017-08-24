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

#include "game/log-macros.hpp"
#include "game/creature/race-enum.hpp"
#include "game/creature/role-enum.hpp"
#include "game/stats/traits-set.hpp"

#include "sfml-util/sfml-util.hpp"

#include "misc/vectors.hpp"
#include "misc/assertlogandthrow.hpp"

#include <string>
#include <sstream>
#include <algorithm>
#include <exception>


namespace game
{
namespace item
{

    std::unique_ptr<ItemProfileWarehouse> ItemProfileWarehouse::instanceUPtr_{ nullptr };


    ItemProfileWarehouse::ItemProfileWarehouse()
    :
        profiles_()
    {
        M_HP_LOG_DBG("Singleton Construction: ItemProfileWarehouse");
    }


    ItemProfileWarehouse::~ItemProfileWarehouse()
    {
        M_HP_LOG_DBG("Singleton Destruction: ItemProfileWarehouse");
    }


    ItemProfileWarehouse * ItemProfileWarehouse::Instance()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            M_HP_LOG_WRN("Singleton Instance() before Acquire(): ItemProfileWarehouse");
            Acquire();
        }

        return instanceUPtr_.get();
    }


    void ItemProfileWarehouse::Acquire()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            instanceUPtr_.reset(new ItemProfileWarehouse);
        }
        else
        {
            M_HP_LOG_WRN("Singleton Acquire() after Construction: ItemProfileWarehouse");
        }
    }


    void ItemProfileWarehouse::Release()
    {
        M_ASSERT_OR_LOGANDTHROW_SS((instanceUPtr_.get() != nullptr),
            "game::creature::ItemProfileWarehouse::Release() found instanceUPtr that was null.");

        instanceUPtr_.reset();
    }


    void ItemProfileWarehouse::Setup()
    {
        profiles_.clear();

        //As of 2017-8-17 there were 849299 raw item profiles created before reductions.
        profiles_.reserve(1000000);

        Setup_StandardEquipmen();
        Setup_UniqueItems();
        Setup_MiscItems();
        Setup_NamedEquipment();
        Setup_SetEquipment();
        Setup_SummoningItems();

        Setup_EliminateDuplicates();
        Setup_LogAndThrowOnInvalid();

        //Sorting by treasure score will help speed up later uses of profiles_
        // during gameplay, and is required before Setup_LogStatistics().
        std::sort(profiles_.begin(),
                  profiles_.end(),
                  [](const auto & A, const auto & B)
                    {
                        return A.TreasureScore() < B.TreasureScore();
                    });

        //This line adds lots of verbose details about item profiles to the log,
        //which are not normally needed during a typical run of the game,
        //so this line will normally be commented out.
        //Setup_LogStatistics();
    }


    void ItemProfileWarehouse::Setup_StandardEquipmen()
    {
        auto setupThinProfile = [&](const auto & THIN_PROFILE)
            {
                SetupFromThinProfile(THIN_PROFILE,
                                     named_type::NotNamed,
                                     set_type::NotASet);
            };

        auto const THINPROFILES_WEAPONS_VEC{ ThinProfilesWeaponsAll() };
        
        std::for_each(THINPROFILES_WEAPONS_VEC.begin(),
                      THINPROFILES_WEAPONS_VEC.end(),
                      setupThinProfile);

        auto const THINPROFILES_ARMOR_VEC{ ThinProfilesArmor(true) };

        std::for_each(THINPROFILES_ARMOR_VEC.begin(),
                      THINPROFILES_ARMOR_VEC.end(),
                      setupThinProfile);
    }


    void ItemProfileWarehouse::Setup_UniqueItems()
    {
        for (int i(1); i < item::unique_type::Count; ++i)
        {
            auto const NEXT_UNIQUE_ENUM{ static_cast<item::unique_type::Enum>(i) };

            auto const ELEMENT_TYPE_COMBS_VEC{ item::element_type::Combinations(
                item::unique_type::ElementTypes(NEXT_UNIQUE_ENUM)) };

            for (auto const NEXT_ELEMENT_TYPE : ELEMENT_TYPE_COMBS_VEC)
            {
                auto const MATERIALS{ item::unique_type::Materials(NEXT_UNIQUE_ENUM) };

                M_ASSERT_OR_LOGANDTHROW_SS((MATERIALS.first.empty() == false),
                    "game::item::ItemProfileWarehouse::Setup() failed to find any primary "
                    << "materials for unique_type="
                    << item::unique_type::ToString(NEXT_UNIQUE_ENUM));

                auto didAddProfile{ false };

                for (auto const NEXT_MATERIAL_PRIMARY : MATERIALS.first)
                {
                    if (MATERIALS.second.empty())
                    {
                        ItemProfile p;

                        p.SetUnique(NEXT_UNIQUE_ENUM,
                                    NEXT_MATERIAL_PRIMARY,
                                    item::material::Nothing,
                                    NEXT_ELEMENT_TYPE);

                        p.SetMisc(item::unique_type::MiscType(NEXT_UNIQUE_ENUM),
                                  false,
                                  NEXT_MATERIAL_PRIMARY,
                                  item::material::Nothing);

                        profiles_.push_back(p);
                        didAddProfile = true;
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

                            p.SetUnique(NEXT_UNIQUE_ENUM,
                                        NEXT_MATERIAL_PRIMARY,
                                        NEXT_MATERIAL_SECONDARY,
                                        NEXT_ELEMENT_TYPE);

                            p.SetMisc(item::unique_type::MiscType(NEXT_UNIQUE_ENUM),
                                      false,
                                      NEXT_MATERIAL_PRIMARY,
                                      NEXT_MATERIAL_SECONDARY);

                            profiles_.push_back(p);
                            didAddProfile = true;
                        }
                    }
                }

                M_ASSERT_OR_LOGANDTHROW_SS((didAddProfile),
                    "game::item::ItemProfileWarehouse::Setup() failed to find any valid "
                    << "material combinations for unique_type="
                    << item::unique_type::ToString(NEXT_UNIQUE_ENUM));
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

            auto const NEXT_NAMED_THINPROFILES_VEC{
                ThinProfiles(NEXT_NAMED_ENUM) };

            for (auto const & NEXT_NAMED_THINPROFILE : NEXT_NAMED_THINPROFILES_VEC)
            {
                SetupFromThinProfile(NEXT_NAMED_THINPROFILE,
                                     NEXT_NAMED_ENUM,
                                     set_type::NotASet);
            }
        }
    }


    void ItemProfileWarehouse::Setup_SetEquipment()
    {
        for (int i(1); i < set_type::Count; ++i)
        {
            auto const NEXT_SET_ENUM{ static_cast<set_type::Enum>(i) };

            auto const NEXT_SET_THINPROFILES_VEC{
                ThinProfiles(NEXT_SET_ENUM) };

            for (auto const & NEXT_SET_THINPROFILE : NEXT_SET_THINPROFILES_VEC)
            {
                SetupFromThinProfile(NEXT_SET_THINPROFILE,
                                     named_type::NotNamed,
                                     NEXT_SET_ENUM);
            }
        }
    }


    void ItemProfileWarehouse::Setup_SummoningItems()
    {
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

                            p.SetMisc(misc_type::Summoning_Statue,
                                      false,
                                      NEXT_MATERIAL,
                                      material::Nothing,
                                      set_type::NotASet);

                            p.SetSummoningAndAdjustScore(
                                SummonInfo(ORIGIN_ENUM, RACE_ENUM, ROLE_ENUM));

                            profiles_.push_back(p);
                        }
                    }
                    else
                    {
                        auto const MISC_TYPE{ [ORIGIN_ENUM]()
                        {
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

                        auto const MATERIAL{ [ORIGIN_ENUM]()
                        {
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

                        p.SetMisc(MISC_TYPE,
                                  false,
                                  MATERIAL,
                                  material::Nothing,
                                  set_type::NotASet);

                        p.SetSummoningAndAdjustScore(
                            SummonInfo(ORIGIN_ENUM, RACE_ENUM, ROLE_ENUM));

                        profiles_.push_back(p);
                    }
                }
            }
        }
    }


    void ItemProfileWarehouse::Setup_EliminateDuplicates()
    {
        auto const RAW_COUNT{ profiles_.size() };
        
        //I can't find the bug in this code that is creating duplicates,
        //so I'll just remove those duplicates here.  -zTn 2017-8-10
        std::sort(profiles_.begin(), profiles_.end());
        profiles_.erase(std::unique(profiles_.begin(), profiles_.end()), profiles_.end());

        auto const DUPLICATE_PROFILE_COUNT{ RAW_COUNT - profiles_.size() };
        
        profiles_.shrink_to_fit();

        M_HP_LOG_DBG("ItemProfileWarehouse::Setup() resulted in " 
            << RAW_COUNT << " raw profiles, "
            << DUPLICATE_PROFILE_COUNT << " duplicate profiles, "
            << profiles_.size() << " final profiles, with a vector capacity that is "
            << ((profiles_.size() == profiles_.capacity()) ? "shrunk" : "NOT-shurnk")
            << ".");
    }


    void ItemProfileWarehouse::Setup_LogAndThrowOnInvalid() const
    {
        auto didFindInvalid{ false };

        for (auto const & PROFILE : profiles_)
        {
            if (PROFILE.IsValid() == false)
            {
                didFindInvalid = true;

                M_HP_LOG_DBG("ItemProfileWarehouse found invalid profiles during Setup():  \""
                    << PROFILE.ToMemberStrings().InvalidString()
                    << "\", profile=" << PROFILE.ToString());
            }
        }

        if (didFindInvalid)
        {
            throw std::runtime_error("ItemProfileWarehouse found invalid profiles during Setup().");
        }
    }


    void ItemProfileWarehouse::Setup_LogStatistics() const
    {
        struct ItemSet
        {
            ItemSet(const std::string & NAME, int DIVISION_SIZE, const std::vector<ItemProfile> & PROFILES)
                :
                name(NAME),
                division(DIVISION_SIZE),
                profiles(),
                scores(),
                divCountsScore(static_cast<std::size_t>((PROFILES[PROFILES.size() - 1].TreasureScore() / DIVISION_SIZE) + 1), 0),
                sum(0)
            {}

            std::string name;
            int division;
            std::vector<ItemProfile> profiles;
            std::vector<int> scores;
            std::vector<int> divCountsScore;
            long sum;

            void Add(const ItemProfile & P)
            {
                profiles.push_back(P);
                ++divCountsScore[static_cast<std::size_t>(P.TreasureScore() / division)];
                sum += P.TreasureScore();
                scores.push_back(P.TreasureScore());
            }

            void Log() const
            {
                M_HP_LOG_DBG("ItemProfileWarehouse::Setup() ItemSet \"" << name
                    << "\" **********************************");

                auto const MEAN{ static_cast<int>(sum / static_cast<long>(scores.size())) };

                M_HP_LOG_DBG("ItemProfileWarehouse::Setup() ItemSet \"" << name
                    << "\"\tcount=" << profiles.size()
                    << ", min=" << scores[0]
                    << ", mean=" << MEAN
                    << ", median=" << scores[scores.size() / 2]
                    << ", max=" << scores[scores.size() - 1]
                    << ", std_dev=" << misc::Vector::StandardDeviation(scores, scores.size(), MEAN));

                M_HP_LOG_DBG("ItemProfileWarehouse::Setup() ItemSet \"" << name
                    << "\" **********************************");

                for (int d(0); d < static_cast<int>(divCountsScore.size()); ++d)
                {
                    M_HP_LOG_DBG("ItemProfileWarehouse::Setup() ItemSet \"" << name
                        << "\" Treasure Score Division [" << d * division
                        << "-" << (d + 1) * division
                        << "]\t\t =" << divCountsScore[static_cast<std::size_t>(d)]);
                }

                M_HP_LOG_DBG("ItemProfileWarehouse::Setup() ItemSet \"" << name
                    << "\" **********************************");

                for (int d(0); d < static_cast<int>(divCountsScore.size()); ++d)
                {
                    auto const DIV_SCORE_START{ d * division };

                    auto const FIRST_WITH_SCORE_ITER{ std::find_if(profiles.begin(),
                        profiles.end(),
                        [DIV_SCORE_START](const auto & P)
                    {
                        return P.TreasureScore() >= DIV_SCORE_START;
                    }) };

                    int i{ 0 };
                    auto iter{ FIRST_WITH_SCORE_ITER };
                    for (; iter != profiles.end() && i < 10; ++iter, ++i)
                    {
                        M_HP_LOG_DBG("ItemProfileWarehouse::Setup() ItemSet \"" << name
                            << "\" Sample: " << iter->TreasureScore()
                            << "\t" << iter->ToString());
                    }
                }

                M_HP_LOG_DBG("ItemProfileWarehouse::Setup() ItemSet \"" << name
                    << "\" **********************************");
            }
        };

        ItemSet treasureScoreSet("TreasureScore", 500, profiles_);
        ItemSet equipmentSet("Equipment", 200, profiles_);
        ItemSet standardEquipmentSet("StandardEquipment", 200, profiles_);
        ItemSet uniqueSet("Unique", 500, profiles_);
        ItemSet setSet("Set", 500, profiles_);
        ItemSet namedSet("Named", 500, profiles_);
        ItemSet elementalSet("Elemental", 500, profiles_);

        for (auto const & PROFILE : profiles_)
        {
            treasureScoreSet.Add(PROFILE);

            if (PROFILE.IsEquipment())
            {
                equipmentSet.Add(PROFILE);

                if (PROFILE.IsStandard())
                {
                    standardEquipmentSet.Add(PROFILE);
                }
            }

            if ((PROFILE.IsUnique() == true) &&
                (PROFILE.IsSet() == false) &&
                (PROFILE.IsNamed() == false) &&
                (PROFILE.IsElemental() == false))
            {
                uniqueSet.Add(PROFILE);
            }

            if ((PROFILE.IsUnique() == false) &&
                (PROFILE.IsSet() == true) &&
                (PROFILE.IsNamed() == false) &&
                (PROFILE.IsElemental() == false))
            {
                setSet.Add(PROFILE);
            }

            if ((PROFILE.IsUnique() == false) &&
                (PROFILE.IsSet() == false) &&
                (PROFILE.IsNamed() == true) &&
                (PROFILE.IsElemental() == false))
            {
                namedSet.Add(PROFILE);
            }

            if ((PROFILE.IsUnique() == false) &&
                (PROFILE.IsSet() == false) &&
                (PROFILE.IsNamed() == false) &&
                (PROFILE.IsElemental() == true))
            {
                elementalSet.Add(PROFILE);
            }
        }

        treasureScoreSet.Log();
        equipmentSet.Log();
        standardEquipmentSet.Log();
        uniqueSet.Log();
        setSet.Log();
        namedSet.Log();
        elementalSet.Log();
    }


    int ItemProfileWarehouse::Score(const stats::TraitSet & TRAIT_SET)
    {
        using namespace stats;

        int score{ 0 };

        for (int i(1); i < Traits::Count; ++i)
        {
            auto const NEXT_TRAIT_ENUM{ static_cast<Traits::Enum>(i) };
            auto const NEXT_TRAIT_VALUE{ TRAIT_SET.GetCopy(NEXT_TRAIT_ENUM).Current() };
            auto traitScore{ [NEXT_TRAIT_VALUE]()
            {
                if (NEXT_TRAIT_VALUE >= 0)
                {
                    return 10 * NEXT_TRAIT_VALUE;
                }
                else
                {
                    return 5 * std::abs(NEXT_TRAIT_VALUE);
                }
            }() };

            if ((NEXT_TRAIT_ENUM == Traits::HealthGainAll) ||
                (NEXT_TRAIT_ENUM == Traits::HealthGainMelee))
            {
                traitScore *= 10;
            }
            else if ((NEXT_TRAIT_ENUM == Traits::AnimalResist) ||
                     (NEXT_TRAIT_ENUM == Traits::ArmorRating) ||
                     (NEXT_TRAIT_ENUM == Traits::Backstab) ||
                     (NEXT_TRAIT_ENUM == Traits::CurseOnDamage) ||
                     (NEXT_TRAIT_ENUM == Traits::DamageBonusAll) ||
                     (NEXT_TRAIT_ENUM == Traits::DamageBonusMelee) ||
                     (NEXT_TRAIT_ENUM == Traits::DamageBonusProj) ||
                     (NEXT_TRAIT_ENUM == Traits::FindCoinsAmount) ||
                     (NEXT_TRAIT_ENUM == Traits::PoisonOnAll) ||
                     (NEXT_TRAIT_ENUM == Traits::PoisonOnMelee))
            {
                traitScore *= 2;
            }
            else if (NEXT_TRAIT_ENUM == Traits::DamageBonusFist)
            {
                traitScore /= 4;
            }

            score += traitScore;
        }

        return score;
    }


    void ItemProfileWarehouse::SetupFromThinProfile(
        const item::ItemProfile & THIN_PROFILE,
        const named_type::Enum    NAMED_TYPE,
        const set_type::Enum      SET_TYPE)
    {
        auto const ELEMENT_TYPES{ [&]()
            {
                if ((NAMED_TYPE != named_type::NotNamed) &&
                    (NAMED_TYPE != named_type::Count))
                {
                    return named_type::ElementTypes(NAMED_TYPE);
                }
                else if ((SET_TYPE != set_type::NotASet) &&
                         (SET_TYPE != set_type::Count))
                {
                    return element_type::None;
                }
                else
                {
                    return static_cast<element_type::Enum>(element_type::Fire |
                                                           element_type::Frost |
                                                           element_type::Honor |
                                                           element_type::Shadow);
                }
            }() };

        auto elementCombinationsVec{ element_type::Combinations(ELEMENT_TYPES) };
        elementCombinationsVec.push_back(element_type::None);

        SetupFromThinProfile(THIN_PROFILE,
                             NAMED_TYPE,
                             SET_TYPE,
                             elementCombinationsVec,
                             GetMaterialsFromThinProfile(THIN_PROFILE,
                                                         NAMED_TYPE));
    }


    void ItemProfileWarehouse::SetupFromThinProfile(
        const ItemProfile &          THIN_PROFILE,
        const named_type::Enum       NAMED_TYPE,
        const set_type::Enum         SET_TYPE,
        const element_type::Enum     ELEMENT_TYPE,
        const material::Enum         MATERIAL_PRI,
        const material::Enum         MATERIAL_SEC,
        const armor::base_type::Enum BASE_TYPE)
    {
        if (MATERIAL_PRI == MATERIAL_SEC)
        {
            return;
        }

        if ((THIN_PROFILE.MiscType() != misc_type::NotMisc) &&
            (THIN_PROFILE.MiscType() != misc_type::Count))
        {
            ItemProfile fatProfile;
            fatProfile.SetMisc(THIN_PROFILE.MiscType(),
                               false,
                               MATERIAL_PRI,
                               MATERIAL_SEC,
                               SET_TYPE);
            profiles_.push_back(fatProfile);

            if (misc_type::HasPixieVersion(THIN_PROFILE.MiscType()))
            {
                ItemProfile fatProfilePixie;
                fatProfilePixie.SetMisc(THIN_PROFILE.MiscType(),
                                        true,
                                        MATERIAL_PRI,
                                        MATERIAL_SEC,
                                        SET_TYPE);
                profiles_.push_back(fatProfilePixie);
            }

            return;
        }

        if (THIN_PROFILE.IsAventail())
        {
            ItemProfile fatProfile;
            fatProfile.SetAventail(BASE_TYPE,
                                   MATERIAL_PRI,
                                   MATERIAL_SEC,
                                   NAMED_TYPE,
                                   SET_TYPE,
                                   ELEMENT_TYPE);
            profiles_.push_back(fatProfile);
            return;
        }

        if (THIN_PROFILE.IsBoots())
        {
            ItemProfile fatProfile;
            fatProfile.SetBoots(BASE_TYPE,
                                MATERIAL_PRI,
                                MATERIAL_SEC,
                                NAMED_TYPE,
                                SET_TYPE,
                                ELEMENT_TYPE,
                                false);
            profiles_.push_back(fatProfile);

            ItemProfile fatProfilePixie;
            fatProfilePixie.SetBoots(BASE_TYPE,
                                     MATERIAL_PRI,
                                     MATERIAL_SEC,
                                     NAMED_TYPE,
                                     SET_TYPE,
                                     ELEMENT_TYPE,
                                     true);
            profiles_.push_back(fatProfilePixie);
            return;
        }

        if (THIN_PROFILE.IsBracer())
        {
            ItemProfile fatProfile;
            fatProfile.SetBracer(BASE_TYPE,
                                 MATERIAL_PRI,
                                 MATERIAL_SEC,
                                 NAMED_TYPE,
                                 SET_TYPE,
                                 ELEMENT_TYPE,
                                 false);
            profiles_.push_back(fatProfile);

            ItemProfile fatProfilePixie;
            fatProfilePixie.SetBracer(BASE_TYPE,
                                      MATERIAL_PRI,
                                      MATERIAL_SEC,
                                      NAMED_TYPE,
                                      SET_TYPE,
                                      ELEMENT_TYPE,
                                      true);
            profiles_.push_back(fatProfilePixie);
            return;
        }

        if (THIN_PROFILE.IsGauntlets())
        {
            ItemProfile fatProfile;
            fatProfile.SetGauntlets(BASE_TYPE,
                                    MATERIAL_PRI,
                                    MATERIAL_SEC,
                                    NAMED_TYPE,
                                    SET_TYPE,
                                    ELEMENT_TYPE,
                                    false);
            profiles_.push_back(fatProfile);

            ItemProfile fatProfilePixie;
            fatProfilePixie.SetGauntlets(BASE_TYPE,
                                         MATERIAL_PRI,
                                         MATERIAL_SEC,
                                         NAMED_TYPE,
                                         SET_TYPE,
                                         ELEMENT_TYPE,
                                         true);
            profiles_.push_back(fatProfilePixie);
            return;
        }

        if (THIN_PROFILE.IsPants())
        {
            ItemProfile fatProfile;
            fatProfile.SetPants(BASE_TYPE,
                                MATERIAL_PRI,
                                MATERIAL_SEC,
                                NAMED_TYPE,
                                SET_TYPE,
                                ELEMENT_TYPE,
                                false);
            profiles_.push_back(fatProfile);

            ItemProfile fatProfilePixie;
            fatProfilePixie.SetPants(BASE_TYPE,
                                     MATERIAL_PRI,
                                     MATERIAL_SEC,
                                     NAMED_TYPE,
                                     SET_TYPE,
                                     ELEMENT_TYPE,
                                     true);
            profiles_.push_back(fatProfilePixie);
            return;
        }

        if (THIN_PROFILE.IsShirt())
        {
            ItemProfile fatProfile;
            fatProfile.SetShirt(BASE_TYPE,
                                MATERIAL_PRI,
                                MATERIAL_SEC,
                                NAMED_TYPE,
                                SET_TYPE,
                                ELEMENT_TYPE,
                                false);
            profiles_.push_back(fatProfile);

            ItemProfile fatProfilePixie;
            fatProfilePixie.SetShirt(BASE_TYPE,
                                     MATERIAL_PRI,
                                     MATERIAL_SEC,
                                     NAMED_TYPE,
                                     SET_TYPE,
                                     ELEMENT_TYPE,
                                     true);
            profiles_.push_back(fatProfilePixie);
            return;
        }

        if (THIN_PROFILE.ShieldType() != armor::shield_type::Count)
        {
            ItemProfile fatProfile;
            fatProfile.SetShield(THIN_PROFILE.ShieldType(),
                                 MATERIAL_PRI,
                                 MATERIAL_SEC,
                                 NAMED_TYPE,
                                 SET_TYPE,
                                 ELEMENT_TYPE);
            profiles_.push_back(fatProfile);
            return;
        }

        if (THIN_PROFILE.CoverType() != armor::cover_type::Count)
        {
            ItemProfile fatProfile;
            fatProfile.SetCover(THIN_PROFILE.CoverType(),
                                MATERIAL_PRI,
                                MATERIAL_SEC,
                                NAMED_TYPE,
                                SET_TYPE,
                                ELEMENT_TYPE,
                                false);
            profiles_.push_back(fatProfile);

            ItemProfile fatProfilePixie;
            fatProfilePixie.SetCover(THIN_PROFILE.CoverType(),
                                     MATERIAL_PRI,
                                     MATERIAL_SEC,
                                     NAMED_TYPE,
                                     SET_TYPE,
                                     ELEMENT_TYPE,
                                     true);
            profiles_.push_back(fatProfilePixie);
            return;
        }

        if (THIN_PROFILE.HelmType() != armor::helm_type::Count)
        {
            ItemProfile fatProfile;
            fatProfile.SetHelm(THIN_PROFILE.HelmType(),
                               MATERIAL_PRI,
                               MATERIAL_SEC,
                               NAMED_TYPE,
                               SET_TYPE,
                               ELEMENT_TYPE);
            profiles_.push_back(fatProfile);
            return;
        }

        if (THIN_PROFILE.SwordType() != weapon::sword_type::Count)
        {
            ItemProfile fatProfile;
            fatProfile.SetSword(THIN_PROFILE.SwordType(),
                                MATERIAL_PRI,
                                MATERIAL_SEC,
                                NAMED_TYPE,
                                SET_TYPE,
                                ELEMENT_TYPE);
            profiles_.push_back(fatProfile);
            return;
        }

        if (THIN_PROFILE.AxeType() != weapon::axe_type::Count)
        {
            ItemProfile fatProfile;
            fatProfile.SetAxe(THIN_PROFILE.AxeType(),
                              MATERIAL_PRI,
                              MATERIAL_SEC,
                              NAMED_TYPE,
                              SET_TYPE,
                              ELEMENT_TYPE);
            profiles_.push_back(fatProfile);
            return;
        }

        if (THIN_PROFILE.ClubType() != weapon::club_type::Count)
        {
            ItemProfile fatProfile;
            fatProfile.SetClub(THIN_PROFILE.ClubType(),
                               MATERIAL_PRI,
                               MATERIAL_SEC,
                               NAMED_TYPE,
                               SET_TYPE,
                               ELEMENT_TYPE);
            profiles_.push_back(fatProfile);
            return;
        }

        if (THIN_PROFILE.WhipType() != weapon::whip_type::Count)
        {
            ItemProfile fatProfile;
            fatProfile.SetWhip(THIN_PROFILE.WhipType(),
                               MATERIAL_PRI,
                               MATERIAL_SEC,
                               NAMED_TYPE,
                               SET_TYPE,
                               ELEMENT_TYPE);
            profiles_.push_back(fatProfile);
            return;
        }

        if (THIN_PROFILE.ProjectileType() != weapon::projectile_type::Count)
        {
            ItemProfile fatProfile;
            fatProfile.SetProjectile(THIN_PROFILE.ProjectileType(),
                                     MATERIAL_PRI,
                                     MATERIAL_SEC,
                                     NAMED_TYPE,
                                     SET_TYPE,
                                     ELEMENT_TYPE);
            profiles_.push_back(fatProfile);
            return;
        }

        if (THIN_PROFILE.BladedStaffType() != weapon::bladedstaff_type::Count)
        {
            ItemProfile fatProfile;
            fatProfile.SetBladedStaff(THIN_PROFILE.BladedStaffType(),
                                      MATERIAL_PRI,
                                      MATERIAL_SEC,
                                      NAMED_TYPE,
                                      SET_TYPE,
                                      ELEMENT_TYPE);
            profiles_.push_back(fatProfile);
            return;
        }

        if (THIN_PROFILE.IsKnife())
        {
            for(int i(0); i<sfml_util::Size::Count; ++i)
            {
                auto const NEXT_SIZE_ENUM{ static_cast<sfml_util::Size::Enum>(i) };

                ItemProfile fatProfile;
                fatProfile.SetKnife(NEXT_SIZE_ENUM,
                                    MATERIAL_PRI,
                                    MATERIAL_SEC,
                                    NAMED_TYPE,
                                    SET_TYPE,
                                    ELEMENT_TYPE,
                                    false);
                profiles_.push_back(fatProfile);

                ItemProfile fatProfilePixie;
                fatProfilePixie.SetKnife(NEXT_SIZE_ENUM,
                                         MATERIAL_PRI,
                                         MATERIAL_SEC,
                                         NAMED_TYPE,
                                         SET_TYPE,
                                         ELEMENT_TYPE,
                                         true);
                profiles_.push_back(fatProfilePixie);
            }
            return;
        }

        if (THIN_PROFILE.IsDagger())
        {
            for(int i(0); i<sfml_util::Size::Count; ++i)
            {
                auto const NEXT_SIZE_ENUM{ static_cast<sfml_util::Size::Enum>(i) };

                ItemProfile fatProfile;
                fatProfile.SetDagger(NEXT_SIZE_ENUM,
                                     MATERIAL_PRI,
                                     MATERIAL_SEC,
                                     NAMED_TYPE,
                                     SET_TYPE,
                                     ELEMENT_TYPE,
                                     false);
                profiles_.push_back(fatProfile);

                ItemProfile fatProfilePixie;
                fatProfilePixie.SetDagger(NEXT_SIZE_ENUM,
                                          MATERIAL_PRI,
                                          MATERIAL_SEC,
                                          NAMED_TYPE,
                                          SET_TYPE,
                                          ELEMENT_TYPE,
                                          true);
                profiles_.push_back(fatProfilePixie);
            }
            return;
        }

        if (THIN_PROFILE.IsStaff())
        {
            ItemProfile fatProfile;
            fatProfile.SetStaff(MATERIAL_PRI,
                                MATERIAL_SEC,
                                NAMED_TYPE,
                                SET_TYPE,
                                ELEMENT_TYPE);
            profiles_.push_back(fatProfile);
            return;
        }

        if (THIN_PROFILE.IsQuarterStaff())
        {
            ItemProfile fatProfile;
            fatProfile.SetQuarterStaff(MATERIAL_PRI,
                                       MATERIAL_SEC,
                                       NAMED_TYPE,
                                       SET_TYPE,
                                       ELEMENT_TYPE);
            profiles_.push_back(fatProfile);
            return;
        }

        std::ostringstream ss;
        ss << "game::item::ItemProfileWarehouse::SetupFromThinProfile(final version, thin_profile="
            << THIN_PROFILE.ToString() << ") "
            << "-was unable to find the type of the given ThinProfile.";
        throw std::runtime_error(ss.str());
    }


    void ItemProfileWarehouse::SetupFromThinProfile(
        const ItemProfile &              THIN_PROFILE,
        const named_type::Enum           NAMED_TYPE,
        const set_type::Enum             SET_TYPE,
        const ElementTypeVec_t &         ELEMENT_TYPE_VEC,
        const BaseMaterialVecPairVec_t & BASEMATERIAL_VEC_PAIR_VEC)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((BASEMATERIAL_VEC_PAIR_VEC.empty() == false),
            "game::item::ItemProfileWarehouse(vector version, thin_profile="
            << THIN_PROFILE.ToString() << ") was given an empty " << "BASEMATERIAL_VEC_PAIR_VEC.");

        for (auto const NEXT_ELEMENT_TYPE : ELEMENT_TYPE_VEC)
        {
            for (auto const & NEXT_BASEMATERIALVECPAIR : BASEMATERIAL_VEC_PAIR_VEC)
            {
                M_ASSERT_OR_LOGANDTHROW_SS((NEXT_BASEMATERIALVECPAIR.second.first.empty() == false),
                    "game::item::ItemProfileWarehouse(vector version, thin_profile="
                    << THIN_PROFILE.ToString() << ", named_type="
                    << item::named_type::ToString(NAMED_TYPE)
                    << ", set_type=" << item::set_type::ToString(SET_TYPE)
                    << ", element_type=" << item::element_type::ToString(NEXT_ELEMENT_TYPE, true)
                    << ", BASEMATERIAL_VEC_PAIR_VEC.size()=" << BASEMATERIAL_VEC_PAIR_VEC.size()
                    << ") was given an empty "
                    << "NEXT_BASEMATERIALVECPAIR.second.first (primary material vec).");

                for (auto const NEXT_MATERIAL_PRI : NEXT_BASEMATERIALVECPAIR.second.first)
                {
                    if (NEXT_BASEMATERIALVECPAIR.second.second.empty())
                    {
                        SetupFromThinProfile(THIN_PROFILE,
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
                            SetupFromThinProfile(THIN_PROFILE,
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


    const BaseMaterialVecPairVec_t ItemProfileWarehouse::GetMaterialsFromThinProfile(
        const ItemProfile &    THIN_PROFILE,
        const named_type::Enum NAMED_TYPE)
    {
        if ((NAMED_TYPE != named_type::NotNamed) &&
            (NAMED_TYPE != named_type::Count))
        {
            auto const NAMED_MATERIALS_VEC_PAIR{ named_type::Materials(NAMED_TYPE) };

            if (NAMED_MATERIALS_VEC_PAIR.first.empty() == false)
            {
                return { std::make_pair(armor::base_type::Count,
                    NAMED_MATERIALS_VEC_PAIR) };
            }
        }

        if ((THIN_PROFILE.MiscType() != misc_type::NotMisc) &&
                 (THIN_PROFILE.MiscType() != misc_type::Count))
        {
            return { std::make_pair(armor::base_type::Count,
                misc_type::Materials(THIN_PROFILE.MiscType())) };
        }
        else
        {
            if (THIN_PROFILE.IsAventail())
            {
                BaseMaterialVecPairVec_t v;

                for (int i(0); i < armor::base_type::Count; ++i)
                {
                    auto const NEXT_BASE_ENUM{ static_cast<armor::base_type::Enum>(i) };
                    if ((THIN_PROFILE.ArmorTypeRestriction() == NEXT_BASE_ENUM) ||
                        (THIN_PROFILE.ArmorTypeRestriction() == armor::base_type::Count))
                    {
                        v.push_back( std::make_pair(NEXT_BASE_ENUM,
                            MaterialsAventail(NEXT_BASE_ENUM)) );
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
                    if ((THIN_PROFILE.ArmorTypeRestriction() == NEXT_BASE_ENUM) ||
                        (THIN_PROFILE.ArmorTypeRestriction() == armor::base_type::Count))
                    {
                        v.push_back(std::make_pair(NEXT_BASE_ENUM,
                            MaterialsBoots(NEXT_BASE_ENUM)) );
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
                    if ((THIN_PROFILE.ArmorTypeRestriction() == NEXT_BASE_ENUM) ||
                        (THIN_PROFILE.ArmorTypeRestriction() == armor::base_type::Count))
                    {
                        v.push_back(std::make_pair(NEXT_BASE_ENUM,
                            MaterialsBracer(NEXT_BASE_ENUM)) );
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
                    if ((THIN_PROFILE.ArmorTypeRestriction() == NEXT_BASE_ENUM) ||
                        (THIN_PROFILE.ArmorTypeRestriction() == armor::base_type::Count))
                    {
                        v.push_back(std::make_pair(NEXT_BASE_ENUM,
                            MaterialsGauntlets(NEXT_BASE_ENUM)) );
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
                    if ((THIN_PROFILE.ArmorTypeRestriction() == NEXT_BASE_ENUM) ||
                        (THIN_PROFILE.ArmorTypeRestriction() == armor::base_type::Count))
                    {
                        v.push_back(std::make_pair(NEXT_BASE_ENUM,
                            MaterialsPants(NEXT_BASE_ENUM)) );
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
                    if ((THIN_PROFILE.ArmorTypeRestriction() == NEXT_BASE_ENUM) ||
                        (THIN_PROFILE.ArmorTypeRestriction() == armor::base_type::Count))
                    {
                        v.push_back(std::make_pair(NEXT_BASE_ENUM,
                            MaterialsShirt(NEXT_BASE_ENUM)) );
                    }
                }

                return v;
            }

            if (THIN_PROFILE.ShieldType() != armor::shield_type::Count)
            {
                return { std::make_pair(armor::base_type::Count,
                    Materials(THIN_PROFILE.ShieldType())) };
            }

            if (THIN_PROFILE.CoverType() != armor::cover_type::Count)
            {
                return { std::make_pair(armor::base_type::Count,
                    Materials(THIN_PROFILE.CoverType())) };
            }

            if (THIN_PROFILE.HelmType() != armor::helm_type::Count)
            {
                return { std::make_pair(armor::base_type::Count,
                    Materials(THIN_PROFILE.HelmType())) };
            }

            if (THIN_PROFILE.SwordType() != weapon::sword_type::Count)
            {
                return { std::make_pair(armor::base_type::Count,
                    Materials(THIN_PROFILE.SwordType())) };
            }

            if (THIN_PROFILE.AxeType() != weapon::axe_type::Count)
            {
                return { std::make_pair(armor::base_type::Count,
                    Materials(THIN_PROFILE.AxeType())) };
            }

            if (THIN_PROFILE.ClubType() != weapon::club_type::Count)
            {
                return { std::make_pair(armor::base_type::Count,
                    Materials(THIN_PROFILE.ClubType())) };
            }

            if (THIN_PROFILE.WhipType() != weapon::whip_type::Count)
            {
                return { std::make_pair(armor::base_type::Count,
                    Materials(THIN_PROFILE.WhipType())) };
            }

            if (THIN_PROFILE.ProjectileType() != weapon::projectile_type::Count)
            {
                return { std::make_pair(armor::base_type::Count,
                    Materials(THIN_PROFILE.ProjectileType())) };
            }

            if (THIN_PROFILE.BladedStaffType() != weapon::bladedstaff_type::Count)
            {
                return { std::make_pair(armor::base_type::Count,
                    Materials(THIN_PROFILE.BladedStaffType())) };
            }

            if (THIN_PROFILE.IsKnife())
            {
                return { std::make_pair(armor::base_type::Count,
                    MaterialsKnife()) };
            }

            if (THIN_PROFILE.IsDagger())
            {
                return { std::make_pair(armor::base_type::Count,
                    MaterialsDagger()) };
            }

            if (THIN_PROFILE.IsStaff())
            {
                return { std::make_pair(armor::base_type::Count,
                    MaterialsStaff()) };
            }

            if (THIN_PROFILE.IsQuarterStaff())
            {
                return { std::make_pair(armor::base_type::Count,
                    MaterialsQuarterStaff()) };
            }

            std::ostringstream ss;
            ss << "game::item::ItemProfileWarehouse::GetMaterialsFromThinProfile(thin_profile="
                << THIN_PROFILE.ToString() << ") "
                << "-was unable to find the type of the given ThinProfile.";
            throw std::runtime_error(ss.str());
        }
    }


    void ItemProfileWarehouse::SetupProfilesForMiscType(const item::misc_type::Enum E)
    {
        auto const MATERIALS{ item::misc_type::Materials(E) };

        M_ASSERT_OR_LOGANDTHROW_SS((MATERIALS.first.empty() == false),
            "game::item::ItemProfileWarehouse::SetupProfilesForMiscType() failed to find any "
            << " primary materials for misc_type=" << item::misc_type::ToString(E));

        auto const COUNT{ profiles_.size() };

        for (auto const NEXT_MATERIAL_PRIMARY : MATERIALS.first)
        {
            if (MATERIALS.second.empty())
            {
                ItemProfile p;
                p.SetMisc(E, false, NEXT_MATERIAL_PRIMARY, item::material::Nothing);
                profiles_.push_back(p);

                if (misc_type::HasPixieVersion(E))
                {
                    ItemProfile pPixie;
                    pPixie.SetMisc(E, true, NEXT_MATERIAL_PRIMARY, item::material::Nothing);
                    profiles_.push_back(pPixie);
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
                    profiles_.push_back(p);

                    if (misc_type::HasPixieVersion(E))
                    {
                        ItemProfile pPixie;
                        pPixie.SetMisc(E, true, NEXT_MATERIAL_PRIMARY, NEXT_MATERIAL_SECONDARY);
                        profiles_.push_back(pPixie);
                    }
                }
            }
        }

        M_ASSERT_OR_LOGANDTHROW_SS((profiles_.size() != COUNT),
            "game::item::ItemProfileWarehouse::SetupProfilesForMiscType() failed to find any"
            << " valid material combinations for misc_type=" << item::misc_type::ToString(E));
    }


    int ItemProfileWarehouse::Score(const item::material::Enum PRI,
                               const item::material::Enum SEC)
    {
        auto const BONUS_PRI{ [PRI]()
            {
                if ((PRI == item::material::Nothing) ||
                    (PRI == item::material::Count))
                {
                    return 0;
                }
                else
                {
                    return item::material::Bonus(PRI);
                }
            }() };

        auto const BONUS_SEC{ [SEC]()
            {
                if ((SEC == item::material::Nothing) ||
                    (SEC == item::material::Count))
                {
                    return 0;
                }
                else
                {
                    return item::material::Bonus(SEC);
                }
            }() };

        return 50 + (BONUS_PRI * BONUS_PRI) + ((BONUS_SEC * BONUS_SEC) / 3);
    }


    int ItemProfileWarehouse::Score(const item::weapon::axe_type::Enum E)
    {
        using namespace item::weapon;

        switch(E)
        {
            case axe_type::Handaxe:   { return 50; }
            case axe_type::Sickle:    { return 80; }
            case axe_type::Battleaxe: { return 100; }
            case axe_type::Waraxe:    { return 200; }
            case axe_type::Count:
            default:                  { return 0; }
        }
    }


    int ItemProfileWarehouse::Score(const item::weapon::bladedstaff_type::Enum E)
    {
        using namespace item::weapon;

        switch (E)
        {
            case bladedstaff_type::Spear:      { return 70; }
            case bladedstaff_type::ShortSpear: { return 50; }
            case bladedstaff_type::Scythe:     { return 80; }
            case bladedstaff_type::Pike:       { return 100; }
            case bladedstaff_type::Partisan:   { return 150; }
            case bladedstaff_type::Halberd:    { return 200; }
            case bladedstaff_type::Count:
            default:                           { return 0; }
        }
    }


    int ItemProfileWarehouse::Score(const item::weapon::club_type::Enum E)
    {
        using namespace item::weapon;

        switch(E)
        {
            case club_type::Spiked:    { return 50; }
            case club_type::Maul:      { return 60; }
            case club_type::Mace:      { return 80; }
            case club_type::Warhammer: { return 120; }
            case club_type::Count:
            default:                   { return 0; }
        }
    }


    int ItemProfileWarehouse::Score(const item::weapon::projectile_type::Enum E)
    {
        using namespace item::weapon;

        switch (E)
        {
            case projectile_type::Blowpipe:     { return 50; }
            case projectile_type::Sling:        { return 60; }
            case projectile_type::Shortbow:     { return 100; }
            case projectile_type::Longbow:      { return 200; }
            case projectile_type::CompositeBow: { return 300; }
            case projectile_type::Crossbow:     { return 300; }
            case projectile_type::Count:
            default:                            { return 0; }
        }
    }


    int ItemProfileWarehouse::Score(const item::weapon::sword_type::Enum E)
    {
        using namespace item::weapon;

        switch(E)
        {
            case sword_type::Claymore:      { return 200; }
            case sword_type::Longsword:     { return 120; }
            case sword_type::Flamberg:      { return 190; }
            case sword_type::Knightlysword: { return 110; }
            case sword_type::Broadsword:    { return 100; }
            case sword_type::Falcata:       { return 50; }
            case sword_type::Saber:         { return 70; }
            case sword_type::Cutlass:       { return 40; }
            case sword_type::Rapier:        { return 70; }
            case sword_type::Gladius:       { return 55; }
            case sword_type::Shortsword:    { return 50; }
            case sword_type::Count:
            default:                        { return 0; }
        }
    }


    int ItemProfileWarehouse::Score(const item::weapon::whip_type::Enum E)
    {
        using namespace item::weapon;

        switch(E)
        {
            case whip_type::Bullwhip:     { return 30; }
            case whip_type::Flail:        { return 60; }
            case whip_type::MaceAndChain: { return 90; }
            case whip_type::Count:
            default:                      { return 0; }
        }
    }


    int ItemProfileWarehouse::ScoreKnife(const sfml_util::Size::Enum E)
    {
        if (E == sfml_util::Size::Small)
        {
            return 20;
        }

        return 25;
    }


    int ItemProfileWarehouse::ScoreDagger(const sfml_util::Size::Enum E)
    {
        if (E == sfml_util::Size::Small)
        {
            return 30;
        }

        return 35;
    }


    int ItemProfileWarehouse::ScoreStaff()
    {
        return 25;
    }


    int ItemProfileWarehouse::ScoreQuarterStaff()
    {
        return 15;
    }


    int ItemProfileWarehouse::Score(const item::armor::base_type::Enum E)
    {
        using namespace item::armor;

        switch(E)
        {
            case base_type::Mail:  { return 75; }
            case base_type::Scale: { return 125; }
            case base_type::Plate: { return 175; }
            case base_type::Plain:
            case base_type::Count:
            default:               { return 25; }
        }
    }


    int ItemProfileWarehouse::Score(const item::armor::cover_type::Enum E)
    {
        using namespace item::armor;

        switch(E)
        {
            case cover_type::Cape:  { return 25; }
            case cover_type::Vest:  { return 50; }
            case cover_type::Robe:  { return 75; }
            case cover_type::Cloak: { return 100; }
            case cover_type::Count:
            default:                { return 0; }
        }
    }


    int ItemProfileWarehouse::Score(const item::armor::helm_type::Enum E)
    {
        using namespace item::armor;

        switch(E)
        {
            case helm_type::Leather:  { return 25; }
            case helm_type::MailCoif: { return 50; }
            case helm_type::Kettle:   { return 75; }
            case helm_type::Archers:  { return 100; }
            case helm_type::Bascinet: { return 125; }
            case helm_type::Great:    { return 150; }
            case helm_type::Count:
            default:                  { return 0; }
        }
    }


    int ItemProfileWarehouse::Score(const item::armor::shield_type::Enum E)
    {
        using namespace item::armor;

        switch(E)
        {
            case shield_type::Buckler: { return 50; }
            case shield_type::Kite:    { return 100; }
            case shield_type::Heater:  { return 150; }
            case shield_type::Pavis:   { return 200; }
            case shield_type::Count:
            default:                   { return 0; }
        }
    }


    int ItemProfileWarehouse::ScoreAventail(item::armor::base_type::Enum E)
    {
        return 34 + Score(E);
    }


    int ItemProfileWarehouse::ScoreBracer(item::armor::base_type::Enum E)
    {
        return 36 + Score(E);
    }


    int ItemProfileWarehouse::ScoreShirt(item::armor::base_type::Enum E)
    {
        return 44 + Score(E);
    }


    int ItemProfileWarehouse::ScoreBoots(item::armor::base_type::Enum E)
    {
        return 46 + Score(E);
    }


    int ItemProfileWarehouse::ScorePants(item::armor::base_type::Enum E)
    {
        return 55 + Score(E);
    }


    int ItemProfileWarehouse::ScoreGauntlets(item::armor::base_type::Enum E)
    {
        return 30 + Score(E);
    }


    const item::MaterialVecPair_t ItemProfileWarehouse::Materials(const item::weapon::axe_type::Enum)
    {
        return item::material::CoreMetalAndCoreSecondary();
    }


    const item::MaterialVecPair_t ItemProfileWarehouse::Materials(
        const item::weapon::bladedstaff_type::Enum)
    {
        return item::MaterialVecPair_t({ item::material::Wood }, item::material::CoreSecondary());
    }


    const item::MaterialVecPair_t ItemProfileWarehouse::Materials(const item::weapon::club_type::Enum)
    {
        return item::material::CoreMetalAndCoreSecondary();
    }


    const item::MaterialVecPair_t ItemProfileWarehouse::Materials(
        const item::weapon::projectile_type::Enum E)
    {
        using namespace item;
        using namespace item::weapon;

        switch (E)
        {
            case projectile_type::Blowpipe:
            {
                return MaterialVecPair_t({ material::Wood,
                                           material::Stone,
                                           material::Bone,
                                           material::Obsidian },
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


    const item::MaterialVecPair_t ItemProfileWarehouse::Materials(
        const item::weapon::sword_type::Enum)
    {
        return item::material::CoreMetalAndCoreSecondary();
    }


    const item::MaterialVecPair_t ItemProfileWarehouse::Materials(
        const item::weapon::whip_type::Enum E)
    {
        using namespace item;
        using namespace item::weapon;

        if (E == whip_type::Bullwhip)
        {
            return MaterialVecPair_t({ material::SoftLeather }, material::CoreSecondary());
        }
        else
        {
            return material::CoreMetalAndCoreSecondary();
        }
    }


    const item::MaterialVecPair_t ItemProfileWarehouse::MaterialsKnife()
    {
        return item::material::CorePrimaryNoPearlAndSecondary();
    }


    const item::MaterialVecPair_t ItemProfileWarehouse::MaterialsDagger()
    {
        return MaterialsKnife();
    }


    const item::MaterialVecPair_t ItemProfileWarehouse::MaterialsStaff()
    {
        return item::MaterialVecPair_t({ item::material::Wood }, item::material::CoreSecondary());
    }


    const item::MaterialVecPair_t ItemProfileWarehouse::MaterialsQuarterStaff()
    {
        return MaterialsStaff();
    }


    const item::MaterialVecPair_t ItemProfileWarehouse::Materials(const item::armor::cover_type::Enum)
    {
        return item::MaterialVecPair_t({ item::material::Cloth,
                                         item::material::SoftLeather,
                                         item::material::HardLeather},
                                       item::material::CoreSecondary());
    }


    const item::MaterialVecPair_t ItemProfileWarehouse::Materials(const item::armor::helm_type::Enum E)
    {
        using namespace item;
        using namespace item::armor;

        if (E == helm_type::Leather)
        {
            return item::MaterialVecPair_t({ material::HardLeather }, material::CoreSecondary());
        }
        else
        {
            return item::MaterialVecPair_t(material::CoreMetal(), material::CoreSecondary());
        }
    }


    const item::MaterialVecPair_t ItemProfileWarehouse::Materials(
        const item::armor::shield_type::Enum)
    {
        item::MaterialVec_t v{ item::material::CoreMetal() };
        v.push_back(item::material::Wood);
        return item::MaterialVecPair_t(v, item::material::CoreSecondary());
    }


    const item::MaterialVecPair_t ItemProfileWarehouse::MaterialsAventail(
        item::armor::base_type::Enum E)
    {
        return MaterialsFromBaseType(E);
    }


    const item::MaterialVecPair_t ItemProfileWarehouse::MaterialsBracer(item::armor::base_type::Enum E)
    {
        return MaterialsFromBaseType(E);
    }


    const item::MaterialVecPair_t ItemProfileWarehouse::MaterialsShirt(item::armor::base_type::Enum E)
    {
        return MaterialsFromBaseType(E);
    }


    const item::MaterialVecPair_t ItemProfileWarehouse::MaterialsBoots(item::armor::base_type::Enum E)
    {
        return MaterialsFromBaseTypeNoCloth(E);
    }


    const item::MaterialVecPair_t ItemProfileWarehouse::MaterialsPants(item::armor::base_type::Enum E)
    {
        return MaterialsFromBaseType(E);
    }


    const item::MaterialVecPair_t ItemProfileWarehouse::MaterialsGauntlets(
        item::armor::base_type::Enum E)
    {
        return MaterialsFromBaseType(E);
    }


    const item::MaterialVec_t ItemProfileWarehouse::MaterialsPrimaryFromArmorBaseType(
        const item::armor::base_type::Enum E)
    {
        using namespace item;
        using namespace item::armor;

        switch(E)
        {
            case base_type::Scale: { return { material::Scale }; }
            case base_type::Mail:
            case base_type::Plate: { return { material::CoreMetal() }; }
            case base_type::Plain: { return { material::Cloth,
                                              material::SoftLeather,
                                              material::HardLeather }; }
            case base_type::Count:
            default:               { return {}; }
        }
    }


    const item::MaterialVecPair_t ItemProfileWarehouse::MaterialsFromBaseType(
        const item::armor::base_type::Enum E)
    {
        return item::MaterialVecPair_t(MaterialsPrimaryFromArmorBaseType(E),
            item::material::CoreSecondary());
    }


    const item::MaterialVecPair_t ItemProfileWarehouse::MaterialsFromBaseTypeNoCloth(
        const item::armor::base_type::Enum E)
    {
        using namespace item;
        using namespace item::armor;

        item::MaterialVec_t v;

        if (E == base_type::Plain)
        {
            v.push_back(material::SoftLeather);
            v.push_back(material::HardLeather);
        }
        else
        {
            v = MaterialsPrimaryFromArmorBaseType(E);
        }

        return item::MaterialVecPair_t(v, item::material::CoreSecondary());
    }


    const item::ItemProfileVec_t ItemProfileWarehouse::ThinProfilesWeaponsSwords()
    {
        using namespace item;
        using namespace item::weapon;

        ItemProfileVec_t v;

        for (int i(0); i < sword_type::Count; ++i)
        {
            item::ItemProfile thinProfile;
            thinProfile.SetSword( static_cast<sword_type::Enum>(i) );
            v.push_back(thinProfile);
        }

        return v;
    }


    const item::ItemProfileVec_t ItemProfileWarehouse::ThinProfilesWeaponsProjectiles()
    {
        using namespace item;
        using namespace item::weapon;

        ItemProfileVec_t v;

        for (int i(0); i < projectile_type::Count; ++i)
        {
            item::ItemProfile thinProfile;
            thinProfile.SetProjectile( static_cast<projectile_type::Enum>(i) );
            v.push_back(thinProfile);
        }

        return v;
    }


    const item::ItemProfileVec_t ItemProfileWarehouse::ThinProfilesWeaponsAll()
    {
        using namespace item;
        using namespace item::weapon;

        ItemProfileVec_t v;

        auto const SWORD_THIN_PROFILES_VEC{ ThinProfilesWeaponsSwords() };

        std::copy(SWORD_THIN_PROFILES_VEC.begin(),
                  SWORD_THIN_PROFILES_VEC.end(),
                  std::back_inserter(v));

        auto const PROJ_THIN_PROFILES_VEC{ ThinProfilesWeaponsProjectiles() };

        std::copy(PROJ_THIN_PROFILES_VEC.begin(),
                  PROJ_THIN_PROFILES_VEC.end(),
                  std::back_inserter(v));

        for (int i(0); i < axe_type::Count; ++i)
        {
            item::ItemProfile thinProfile;
            thinProfile.SetAxe(static_cast<axe_type::Enum>(i));
            v.push_back(thinProfile);
        }

        for (int i(0); i < bladedstaff_type::Count; ++i)
        {
            item::ItemProfile thinProfile;
            thinProfile.SetBladedStaff(static_cast<bladedstaff_type::Enum>(i));
            v.push_back(thinProfile);
        }

        for (int i(0); i < club_type::Count; ++i)
        {
            item::ItemProfile thinProfile;
            thinProfile.SetClub(static_cast<club_type::Enum>(i));
            v.push_back(thinProfile);
        }

        for (int i(0); i < whip_type::Count; ++i)
        {
            item::ItemProfile thinProfile;
            thinProfile.SetWhip(static_cast<whip_type::Enum>(i));
            v.push_back(thinProfile);
        }

        for (int i(0); i < sfml_util::Size::Count; ++i)
        {
            item::ItemProfile thinProfile;
            thinProfile.SetKnife(static_cast<sfml_util::Size::Enum>(i));
            v.push_back(thinProfile);
        }

        for (int i(0); i < sfml_util::Size::Count; ++i)
        {
            item::ItemProfile thinProfile;
            thinProfile.SetDagger(static_cast<sfml_util::Size::Enum>(i));
            v.push_back(thinProfile);
        }

        {
            item::ItemProfile thinProfile;
            thinProfile.SetStaff(material::Nothing);
            v.push_back(thinProfile);
        }

        {
            item::ItemProfile thinProfile;
            thinProfile.SetQuarterStaff(material::Nothing);
            v.push_back(thinProfile);
        }

        return v;
    }


    const item::ItemProfileVec_t ItemProfileWarehouse::ThinProfilesArmor(
        const bool WILL_INCLUDE_COVERINGS)
    {
        using namespace item;
        using namespace item::armor;

        ItemProfileVec_t v;

        if (WILL_INCLUDE_COVERINGS)
        {
            for (int i(0); i < cover_type::Count; ++i)
            {
                item::ItemProfile thinProfile;
                thinProfile.SetCover(static_cast<cover_type::Enum>(i));
                v.push_back(thinProfile);
            }
        }

        for (int i(0); i < helm_type::Count; ++i)
        {
            item::ItemProfile thinProfile;
            thinProfile.SetHelm(static_cast<helm_type::Enum>(i));
            v.push_back(thinProfile);
        }

        for (int i(0); i < shield_type::Count; ++i)
        {
            item::ItemProfile thinProfile;
            thinProfile.SetShield(static_cast<shield_type::Enum>(i));
            v.push_back(thinProfile);
        }

        {
            item::ItemProfile thinProfile;
            thinProfile.SetAventail(base_type::Count);
            v.push_back(thinProfile);
        }

        {
            item::ItemProfile thinProfile;
            thinProfile.SetBoots(base_type::Count);
            v.push_back(thinProfile);
        }

        {
            item::ItemProfile thinProfile;
            thinProfile.SetBracer(base_type::Count);
            v.push_back(thinProfile);
        }

        {
            item::ItemProfile thinProfile;
            thinProfile.SetGauntlets(base_type::Count);
            v.push_back(thinProfile);
        }

        {
            item::ItemProfile thinProfile;
            thinProfile.SetPants(base_type::Count);
            v.push_back(thinProfile);
        }

        {
            item::ItemProfile thinProfile;
            thinProfile.SetShirt(base_type::Count);
            v.push_back(thinProfile);
        }

        {
            item::ItemProfile thinProfile;
            thinProfile.SetGauntlets(base_type::Count);
            v.push_back(thinProfile);
        }

        return v;
    }


    const item::ItemProfileVec_t ItemProfileWarehouse::ThinProfiles(const item::named_type::Enum E)
    {
        using namespace item;
        using namespace item::armor;
        using namespace item::weapon;

        switch (E)
        {
            case item::named_type::Wicked:
            case item::named_type::Fiendish:
            case item::named_type::Infernal:
            case item::named_type::Raging:
            case item::named_type::Diabolic:
            case item::named_type::Searing:
            case item::named_type::Burning:
            case item::named_type::Fiery:
            case item::named_type::Icy:
            case item::named_type::Winter:
            case item::named_type::Frigid:
            case item::named_type::Proud:
            case item::named_type::Glory:
            case item::named_type::Pure:
            case item::named_type::Gloom:
            case item::named_type::Twilight:
            case item::named_type::Dusk:
            case item::named_type::Dark:
            case item::named_type::Betrayer:
            {
                return ThinProfilesWeaponsAll();
            }

            case item::named_type::Dancing:
            case item::named_type::Marauder:
            {
                return ThinProfilesWeaponsSwords();
            }

            case item::named_type::Honest:
            case item::named_type::Noble:
            case item::named_type::Daring:
            case item::named_type::Elite:
            case item::named_type::Valiant:
            case item::named_type::Heros:
            case item::named_type::Light:
            case item::named_type::Dawn:
            case item::named_type::Sun:
            case item::named_type::Sorrow:
            case item::named_type::Woe:
            case item::named_type::Misery:
            case item::named_type::Moon:
            {
                return ThinProfilesArmor(true);
            }

            case item::named_type::Army:
            case item::named_type::Charred:
            case item::named_type::Chill:
            case item::named_type::Frozen:
            case item::named_type::Arctic:
            {
                return ThinProfilesArmor(false);
            }

            case item::named_type::Gladiator:
            {
                ItemProfileVec_t v;
                v.reserve(10);

                {
                    ItemProfile thinProfile;
                    thinProfile.SetShield(shield_type::Buckler);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetShield(shield_type::Kite);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetHelm(helm_type::Great);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetHelm(helm_type::Bascinet);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetHelm(helm_type::Kettle);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetAxe(axe_type::Handaxe);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetBladedStaff(bladedstaff_type::Spear);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetSword(sword_type::Gladius);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetSword(sword_type::Shortsword);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetSword(sword_type::Broadsword);
                    v.push_back(thinProfile);
                }

                return v;
            }

            case item::named_type::Soldiers:
            {
                item::ItemProfileVec_t thinProfiles{ ThinProfilesWeaponsAll() };

                auto const THINPROFILES_ARMOR_VEC{ ThinProfilesArmor(false) };

                thinProfiles.reserve(thinProfiles.size() + THINPROFILES_ARMOR_VEC.size());

                std::copy(THINPROFILES_ARMOR_VEC.begin(),
                          THINPROFILES_ARMOR_VEC.end(),
                          std::back_inserter(thinProfiles));

                return thinProfiles;
            }

            case item::named_type::Wardens:
            {
                ItemProfileVec_t v{ ThinProfilesArmor(false) };

                for (auto nextThinProfileArmor : v)
                {
                    nextThinProfileArmor.SetArmorTypeRestriction(armor::base_type::Plain);
                }
                
                v.erase(std::remove_if(v.begin(), v.end(),
                    []
                    (const ItemProfile & P)
                    { return (P.ArmorType() & armor_type::Sheild); } ), v.end());

                v.erase(std::remove_if(v.begin(), v.end(),
                    []
                    (const ItemProfile & P)
                    { return (P.ArmorType() & armor_type::Helm); } ), v.end());

                {
                    ItemProfile thinProfile;
                    thinProfile.SetSword(sword_type::Shortsword);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetHelm(helm_type::Bascinet);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetShield(shield_type::Buckler);
                    v.push_back(thinProfile);
                }

                return v;
            }

            case item::named_type::Princes:
            {
                ItemProfileVec_t v{ ThinProfilesArmor(false) };

                for (auto nextThinProfileArmor : v)
                {
                    nextThinProfileArmor.SetArmorTypeRestriction(armor::base_type::Mail);
                }

                v.erase(std::remove_if(v.begin(), v.end(),
                    []
                    (const ItemProfile & P)
                    { return (P.ArmorType() & armor_type::Sheild); } ), v.end());

                v.erase(std::remove_if(v.begin(), v.end(),
                    []
                    (const ItemProfile & P)
                    { return (P.ArmorType() & armor_type::Helm); } ), v.end());

                {
                    ItemProfile thinProfile;
                    thinProfile.SetSword(sword_type::Rapier);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetHelm(helm_type::MailCoif);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetShield(shield_type::Kite);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetCover(cover_type::Cape);
                    v.push_back(thinProfile);
                }

                return v;
            }

            case item::named_type::Ranger:
            {
                ItemProfileVec_t v{ ThinProfilesArmor(false) };

                for (auto nextThinProfileArmor : v)
                {
                    nextThinProfileArmor.SetArmorTypeRestriction(armor::base_type::Plain);
                }

                v.erase(std::remove_if(v.begin(), v.end(),
                    []
                    (const ItemProfile & P)
                    { return (P.ArmorType() & armor_type::Sheild); } ), v.end());

                v.erase(std::remove_if(v.begin(), v.end(),
                    []
                    (const ItemProfile & P)
                    { return (P.ArmorType() & armor_type::Helm); } ), v.end());

                {
                    ItemProfile thinProfile;
                    thinProfile.SetProjectile(projectile_type::Longbow);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetCover(cover_type::Cloak);
                    v.push_back(thinProfile);
                }

                return v;
            }

            case item::named_type::Samurai:
            {
                ItemProfileVec_t v{ ThinProfilesArmor(false) };

                for (auto nextThinProfileArmor : v)
                {
                    nextThinProfileArmor.SetArmorTypeRestriction(armor::base_type::Scale);
                }

                v.erase(std::remove_if(v.begin(), v.end(),
                    []
                    (const ItemProfile & P)
                    { return (P.ArmorType() & armor_type::Sheild); } ), v.end());

                v.erase(std::remove_if(v.begin(), v.end(),
                    []
                    (const ItemProfile & P)
                    { return (P.ArmorType() & armor_type::Helm); } ), v.end());

                {
                    ItemProfile thinProfile;
                    thinProfile.SetSword(sword_type::Longsword);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetHelm(helm_type::Leather);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetCover(cover_type::Cape);
                    v.push_back(thinProfile);
                }

                return v;
            }

            case item::named_type::Thors:
            {
                ItemProfileVec_t v{ ThinProfilesArmor(false) };

                v.erase(std::remove_if(v.begin(), v.end(),
                    []
                    (const ItemProfile & P)
                    { return (P.ArmorType() & armor_type::Sheild); } ), v.end());

                v.erase(std::remove_if(v.begin(), v.end(),
                    []
                    (const ItemProfile & P)
                    { return (P.ArmorType() & armor_type::Helm); } ), v.end());

                {
                    ItemProfile thinProfile;
                    thinProfile.SetSword(sword_type::Broadsword);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetHelm(helm_type::Great);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetShield(shield_type::Pavis);
                    v.push_back(thinProfile);
                }

                return v;
            }
            case item::named_type::Nile:
            {
                ItemProfileVec_t v;

                {
                    ItemProfile thinProfile;
                    thinProfile.SetSword(sword_type::Falcata);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetShield(shield_type::Buckler);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetHelm(helm_type::Leather);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetBracer();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.push_back(thinProfile);
                }

                return v;
            }

            case item::named_type::Imposters:
            case item::named_type::Pickpocket:
            {
                ItemProfileVec_t v;

                {
                    ItemProfile thinProfile;
                    thinProfile.SetCover(cover_type::Robe);
                    v.push_back(thinProfile);
                }

                return v;
            }

            case item::named_type::Burglar:
            case item::named_type::Mountebank:
            {
                ItemProfileVec_t v;

                {
                    ItemProfile thinProfile;
                    thinProfile.SetCover(cover_type::Cloak);
                    v.push_back(thinProfile);
                }

                return v;
            }

            case item::named_type::Charlatans:
            {
                ItemProfileVec_t v;

                {
                    ItemProfile thinProfile;
                    thinProfile.SetCover(cover_type::Vest);
                    v.push_back(thinProfile);
                }

                return v;
            }

            case item::named_type::Robbers:
            case item::named_type::Thugs:
            case item::named_type::Knaves:
            {
                ItemProfileVec_t v;

                {
                    ItemProfile thinProfile;
                    thinProfile.SetKnife();
                    v.push_back(thinProfile);
                }

                return v;
            }

            case item::named_type::Muggers:
            case item::named_type::Thief:
            case item::named_type::Pirate:
            {
                ItemProfileVec_t v;

                {
                    ItemProfile thinProfile;
                    thinProfile.SetDagger();
                    v.push_back(thinProfile);
                }

                return v;
            }

            case item::named_type::Focus:
            {
                return ThinProfilesWeaponsProjectiles();
            }

            case item::named_type::NotNamed:
            case item::named_type::Count:
            default:
            {
                return {};
            }
        }
    }


    const item::ItemProfileVec_t ItemProfileWarehouse::ThinProfiles(const item::set_type::Enum E)
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
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetPants();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetShirt();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetGauntlets();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetCover(armor::cover_type::Cloak);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetDagger();
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetMisc(misc_type::LockPicks);
                    v.push_back(thinProfile);
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
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetPants();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetShirt();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetGauntlets();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetCover(armor::cover_type::Robe);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetMisc(misc_type::Wand);
                    v.push_back(thinProfile);
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
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetPants();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetShirt();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetGauntlets();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetCover(armor::cover_type::Robe);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetMisc(misc_type::Litch_Hand);
                    v.push_back(thinProfile);
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
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetPants();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetShirt();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetGauntlets();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetCover(armor::cover_type::Robe);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetMisc(misc_type::Wand);
                    v.push_back(thinProfile);
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
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetPants();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetShirt();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetGauntlets();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetCover(armor::cover_type::Robe);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetMisc(misc_type::Braid);
                    v.push_back(thinProfile);
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
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetPants();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Mail);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetShirt();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Mail);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetGauntlets();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetCover(armor::cover_type::Vest);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetSword(weapon::sword_type::Longsword);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetMisc(misc_type::DrumLute);
                    v.push_back(thinProfile);
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
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetPants();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plate);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetShirt();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plate);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetGauntlets();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plate);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetBracer();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plate);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetCover(armor::cover_type::Cape);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetShield(armor::shield_type::Pavis);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetHelm(armor::helm_type::Great);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetAventail();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plate);
                    v.push_back(thinProfile);
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
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetPants();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Mail);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetShirt();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Mail);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetGauntlets();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetBracer();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Mail);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetCover(armor::cover_type::Vest);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetHelm(armor::helm_type::Archers);
                    v.push_back(thinProfile);
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
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetPants();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Scale);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetShirt();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Scale);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetBracer();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Scale);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetGauntlets();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Scale);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetShield(armor::shield_type::Kite);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetHelm(armor::helm_type::Bascinet);
                    v.push_back(thinProfile);
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

}
}
