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
#include "game/combat/treasure-factory.hpp"

#include "sfml-util/sfml-util.hpp"

#include "misc/assertlogandthrow.hpp"
#include "misc/vectors.hpp"

#include <sstream>
#include <exception>


namespace game
{
namespace item
{

    std::unique_ptr<ItemProfileWarehouse> ItemProfileWarehouse::instanceUPtr_{ nullptr };


    ItemProfileWarehouse::ItemProfileWarehouse()
    :
        vec_()
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
        vec_.clear();

        //items from standard equipment
        {
            ItemProfileVec_t v;

            auto const THINPROFILES_WEAPONS_VEC{ combat::TreasureFactory::ThinProfilesWeaponsAll() };
            std::copy(THINPROFILES_WEAPONS_VEC.begin(),
                      THINPROFILES_WEAPONS_VEC.end(),
                      std::back_inserter(v));

            auto const THINPROFILES_ARMOR_VEC{ combat::TreasureFactory::ThinProfilesArmor(true) };
            std::copy(THINPROFILES_ARMOR_VEC.begin(),
                      THINPROFILES_ARMOR_VEC.end(),
                      std::back_inserter(v));

            for (auto const & NEXT_THIN_PROFILE : v)
            {
                SetupFromThinProfile(NEXT_THIN_PROFILE,
                                     named_type::NotNamed,
                                     set_type::NotASet);
            }
        }

        //items from unique types
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

                        vec_.push_back(p);
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

                            vec_.push_back(p);
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
        
        //items from misc types
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
        
        //items from named equipment
        {
            for (int i(1); i < named_type::Count; ++i)
            {
                auto const NEXT_NAMED_ENUM{ static_cast<named_type::Enum>(i) };

                auto const NEXT_NAMED_THINPROFILES_VEC{
                    combat::TreasureFactory::ThinProfiles(NEXT_NAMED_ENUM) };

                for (auto const & NEXT_NAMED_THINPROFILE : NEXT_NAMED_THINPROFILES_VEC)
                {
                    SetupFromThinProfile(NEXT_NAMED_THINPROFILE,
                                         NEXT_NAMED_ENUM,
                                         set_type::NotASet);
                }
            }
        }

        //items from set equipment
        {
            for (int i(1); i < set_type::Count; ++i)
            {
                auto const NEXT_SET_ENUM{ static_cast<set_type::Enum>(i) };

                auto const NEXT_SET_THINPROFILES_VEC{
                    combat::TreasureFactory::ThinProfiles(NEXT_SET_ENUM) };

                for (auto const & NEXT_SET_THINPROFILE : NEXT_SET_THINPROFILES_VEC)
                {
                    SetupFromThinProfile(NEXT_SET_THINPROFILE,
                                         named_type::NotNamed,
                                         NEXT_SET_ENUM);
                }
            }
        }

        //okay...I can't find the bug in this ItemProfileWarehouse code that is creating duplicates
        //so I'll just remove them here.  There are ~834,000 items before this erase and ~523,000
        //after.  -zTn 2017-8-10
        std::sort(vec_.begin(), vec_.end());
        vec_.erase(std::unique(vec_.begin(), vec_.end()), vec_.end());

        //okay...I can't find the bug that allows items with the same material as pri and sec,
        //so I'll just remove them here.
        vec_.erase(std::remove_if(vec_.begin(), vec_.end(),
            [](const ItemProfile & P)
            {
                return (P.MaterialPrimary() == P.MaterialSecondary());
            }), vec_.end());

        M_HP_LOG_DBG("ItemProfileWarehouse::Setup() resulted in " << vec_.size() << " items.");
    }


    void ItemProfileWarehouse::SetupFromThinProfile(
        const item::ItemProfile & THIN_PROFILE,
        const named_type::Enum    NAMED_TYPE,
        const set_type::Enum      SET_TYPE)
    {
        auto const ELEMENT_TYPE{ [&]()
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

        auto elementCombinationsVec{ element_type::Combinations(ELEMENT_TYPE) };
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
        if ((THIN_PROFILE.MiscType() != misc_type::NotMisc) &&
            (THIN_PROFILE.MiscType() != misc_type::Count))
        {
            ItemProfile fatProfile;
            fatProfile.SetMisc(THIN_PROFILE.MiscType(),
                               false,
                               MATERIAL_PRI,
                               MATERIAL_SEC,
                               SET_TYPE);
            vec_.push_back(fatProfile);

            if (misc_type::HasPixieVersion(THIN_PROFILE.MiscType()))
            {
                ItemProfile fatProfilePixie;
                fatProfilePixie.SetMisc(THIN_PROFILE.MiscType(),
                                        true,
                                        MATERIAL_PRI,
                                        MATERIAL_SEC,
                                        SET_TYPE);
                vec_.push_back(fatProfilePixie);
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
            vec_.push_back(fatProfile);
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
            vec_.push_back(fatProfile);

            ItemProfile fatProfilePixie;
            fatProfilePixie.SetBoots(BASE_TYPE,
                                     MATERIAL_PRI,
                                     MATERIAL_SEC,
                                     NAMED_TYPE,
                                     SET_TYPE,
                                     ELEMENT_TYPE,
                                     true);
            vec_.push_back(fatProfilePixie);
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
            vec_.push_back(fatProfile);

            ItemProfile fatProfilePixie;
            fatProfilePixie.SetBracer(BASE_TYPE,
                                      MATERIAL_PRI,
                                      MATERIAL_SEC,
                                      NAMED_TYPE,
                                      SET_TYPE,
                                      ELEMENT_TYPE,
                                      true);
            vec_.push_back(fatProfilePixie);
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
            vec_.push_back(fatProfile);

            ItemProfile fatProfilePixie;
            fatProfilePixie.SetGauntlets(BASE_TYPE,
                                         MATERIAL_PRI,
                                         MATERIAL_SEC,
                                         NAMED_TYPE,
                                         SET_TYPE,
                                         ELEMENT_TYPE,
                                         true);
            vec_.push_back(fatProfilePixie);
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
            vec_.push_back(fatProfile);

            ItemProfile fatProfilePixie;
            fatProfilePixie.SetPants(BASE_TYPE,
                                     MATERIAL_PRI,
                                     MATERIAL_SEC,
                                     NAMED_TYPE,
                                     SET_TYPE,
                                     ELEMENT_TYPE,
                                     true);
            vec_.push_back(fatProfilePixie);
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
            vec_.push_back(fatProfile);

            ItemProfile fatProfilePixie;
            fatProfilePixie.SetShirt(BASE_TYPE,
                                     MATERIAL_PRI,
                                     MATERIAL_SEC,
                                     NAMED_TYPE,
                                     SET_TYPE,
                                     ELEMENT_TYPE,
                                     true);
            vec_.push_back(fatProfilePixie);
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
            vec_.push_back(fatProfile);
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
            vec_.push_back(fatProfile);

            ItemProfile fatProfilePixie;
            fatProfilePixie.SetCover(THIN_PROFILE.CoverType(),
                                     MATERIAL_PRI,
                                     MATERIAL_SEC,
                                     NAMED_TYPE,
                                     SET_TYPE,
                                     ELEMENT_TYPE,
                                     true);
            vec_.push_back(fatProfilePixie);
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
            vec_.push_back(fatProfile);
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
            vec_.push_back(fatProfile);
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
            vec_.push_back(fatProfile);
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
            vec_.push_back(fatProfile);
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
            vec_.push_back(fatProfile);
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
            vec_.push_back(fatProfile);
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
            vec_.push_back(fatProfile);
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
                vec_.push_back(fatProfile);

                ItemProfile fatProfilePixie;
                fatProfilePixie.SetKnife(NEXT_SIZE_ENUM,
                                         MATERIAL_PRI,
                                         MATERIAL_SEC,
                                         NAMED_TYPE,
                                         SET_TYPE,
                                         ELEMENT_TYPE,
                                         true);
                vec_.push_back(fatProfilePixie);
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
                vec_.push_back(fatProfile);

                ItemProfile fatProfilePixie;
                fatProfilePixie.SetDagger(NEXT_SIZE_ENUM,
                                          MATERIAL_PRI,
                                          MATERIAL_SEC,
                                          NAMED_TYPE,
                                          SET_TYPE,
                                          ELEMENT_TYPE,
                                          true);
                vec_.push_back(fatProfilePixie);
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
            vec_.push_back(fatProfile);
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
            vec_.push_back(fatProfile);
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
                            combat::TreasureFactory::MaterialsAventail(NEXT_BASE_ENUM)) );
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
                            combat::TreasureFactory::MaterialsBoots(NEXT_BASE_ENUM)) );
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
                            combat::TreasureFactory::MaterialsBracer(NEXT_BASE_ENUM)) );
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
                            combat::TreasureFactory::MaterialsGauntlets(NEXT_BASE_ENUM)) );
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
                            combat::TreasureFactory::MaterialsPants(NEXT_BASE_ENUM)) );
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
                            combat::TreasureFactory::MaterialsShirt(NEXT_BASE_ENUM)) );
                    }
                }

                return v;
            }

            if (THIN_PROFILE.ShieldType() != armor::shield_type::Count)
            {
                return { std::make_pair(armor::base_type::Count,
                    combat::TreasureFactory::Materials(THIN_PROFILE.ShieldType())) };
            }

            if (THIN_PROFILE.CoverType() != armor::cover_type::Count)
            {
                return { std::make_pair(armor::base_type::Count,
                    combat::TreasureFactory::Materials(THIN_PROFILE.CoverType())) };
            }

            if (THIN_PROFILE.HelmType() != armor::helm_type::Count)
            {
                return { std::make_pair(armor::base_type::Count,
                    combat::TreasureFactory::Materials(THIN_PROFILE.HelmType())) };
            }

            if (THIN_PROFILE.SwordType() != weapon::sword_type::Count)
            {
                return { std::make_pair(armor::base_type::Count,
                    combat::TreasureFactory::Materials(THIN_PROFILE.SwordType())) };
            }

            if (THIN_PROFILE.AxeType() != weapon::axe_type::Count)
            {
                return { std::make_pair(armor::base_type::Count,
                    combat::TreasureFactory::Materials(THIN_PROFILE.AxeType())) };
            }

            if (THIN_PROFILE.ClubType() != weapon::club_type::Count)
            {
                return { std::make_pair(armor::base_type::Count,
                    combat::TreasureFactory::Materials(THIN_PROFILE.ClubType())) };
            }

            if (THIN_PROFILE.WhipType() != weapon::whip_type::Count)
            {
                return { std::make_pair(armor::base_type::Count,
                    combat::TreasureFactory::Materials(THIN_PROFILE.WhipType())) };
            }

            if (THIN_PROFILE.ProjectileType() != weapon::projectile_type::Count)
            {
                return { std::make_pair(armor::base_type::Count,
                    combat::TreasureFactory::Materials(THIN_PROFILE.ProjectileType())) };
            }

            if (THIN_PROFILE.BladedStaffType() != weapon::bladedstaff_type::Count)
            {
                return { std::make_pair(armor::base_type::Count,
                    combat::TreasureFactory::Materials(THIN_PROFILE.BladedStaffType())) };
            }

            if (THIN_PROFILE.IsKnife())
            {
                return { std::make_pair(armor::base_type::Count,
                    combat::TreasureFactory::MaterialsKnife()) };
            }

            if (THIN_PROFILE.IsDagger())
            {
                return { std::make_pair(armor::base_type::Count,
                    combat::TreasureFactory::MaterialsDagger()) };
            }

            if (THIN_PROFILE.IsStaff())
            {
                return { std::make_pair(armor::base_type::Count,
                    combat::TreasureFactory::MaterialsStaff()) };
            }

            if (THIN_PROFILE.IsQuarterStaff())
            {
                return { std::make_pair(armor::base_type::Count,
                    combat::TreasureFactory::MaterialsQuarterStaff()) };
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

        auto const COUNT{ vec_.size() };

        for (auto const NEXT_MATERIAL_PRIMARY : MATERIALS.first)
        {
            if (MATERIALS.second.empty())
            {
                ItemProfile p;
                p.SetMisc(E, false, NEXT_MATERIAL_PRIMARY, item::material::Nothing);
                vec_.push_back(p);

                if (misc_type::HasPixieVersion(E))
                {
                    ItemProfile pPixie;
                    pPixie.SetMisc(E, true, NEXT_MATERIAL_PRIMARY, item::material::Nothing);
                    vec_.push_back(pPixie);
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
                    vec_.push_back(p);

                    if (misc_type::HasPixieVersion(E))
                    {
                        ItemProfile pPixie;
                        pPixie.SetMisc(E, true, NEXT_MATERIAL_PRIMARY, NEXT_MATERIAL_SECONDARY);
                        vec_.push_back(pPixie);
                    }
                }
            }
        }

        M_ASSERT_OR_LOGANDTHROW_SS((vec_.size() != COUNT),
            "game::item::ItemProfileWarehouse::SetupProfilesForMiscType() failed to find any"
            << " valid material combinations for misc_type=" << item::misc_type::ToString(E));
    }

}
}
