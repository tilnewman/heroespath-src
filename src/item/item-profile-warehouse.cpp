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
    MaterialCollections ItemProfileWarehouse::matColl_;

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
        matColl_.Setup();

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
        for (auto const & WEAPON_THIN_PROFILE : ThinProfilesWeaponsAll())
        {
            MakeLoopOverMaterialsForThinProfile(WEAPON_THIN_PROFILE);
        }

        for (auto const & ARMOR_THIN_PROFILE : ThinProfilesArmor(true))
        {
            MakeLoopOverMaterialsForThinProfile(ARMOR_THIN_PROFILE);
        }
    }

    void ItemProfileWarehouse::Setup_UniqueItems()
    {
        for (int i(1); i < unique_type::Count; ++i)
        {
            auto const UNIQUE_ENUM{ static_cast<unique_type::Enum>(i) };

            // currently there are only a few unique_type capes that are "of honor", but there are
            // some, so this loop on element type is required
            for (auto const ELEMENT_TYPE : unique_type::ElementTypes(UNIQUE_ENUM, true))
            {
                auto didAddProfile{ false };

                auto appendPixieAndNonPixieProfilesAsNeeded{
                    [&](const material::Enum MATERIAL_PRIMARY,
                        const material::Enum MATERIAL_SECONDARY) {
                        auto const MISC_TYPE{ unique_type::MiscType(UNIQUE_ENUM) };

                        if (misc_type::HasOnlyPixieVersion(MISC_TYPE) == false)
                        {
                            ItemProfile profile;

                            profile.SetUniqueThenSetMisc(
                                UNIQUE_ENUM,
                                MISC_TYPE,
                                MATERIAL_PRIMARY,
                                MATERIAL_SECONDARY,
                                ELEMENT_TYPE,
                                false);

                            AppendToEitherNormalOrReligiousVector(profile);
                            didAddProfile = true;
                        }

                        if (misc_type::HasPixieVersion(MISC_TYPE))
                        {
                            ItemProfile pixieProfile;

                            pixieProfile.SetUniqueThenSetMisc(
                                UNIQUE_ENUM,
                                MISC_TYPE,
                                MATERIAL_PRIMARY,
                                MATERIAL_SECONDARY,
                                ELEMENT_TYPE,
                                true);

                            AppendToEitherNormalOrReligiousVector(pixieProfile);
                            didAddProfile = true;
                        }
                    }
                };

                using MatPair_t = std::pair<item::material::Enum, item::material::Enum>;
                std::vector<MatPair_t> matPairsAlreadyMade;

                // this number found to be the max during test runs on 2018-4-28
                matPairsAlreadyMade.reserve(50);

                auto const MATERIALS{ matColl_.RemoveLameMaterialsForSpecialItems(
                    matColl_.VectorPairByUniqueType(UNIQUE_ENUM)) };

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

                                continue;
                            }

                            matPairsAlreadyMade.emplace_back(MAT_PAIR);

                            appendPixieAndNonPixieProfilesAsNeeded(
                                MATERIAL_PRIMARY, MATERIAL_SECONDARY);
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
                for (auto const ELEMENT_TYPE : misc_type::ElementTypes(MISC_ENUM, true))
                {
                    MakeLoopOverMaterialsForMiscType(MISC_ENUM, ELEMENT_TYPE);
                }
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
                MakeLoopOverMaterialsForThinProfile(
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
                MakeLoopOverMaterialsForThinProfile(
                    SET_THINPROFILE, named_type::NotNamed, SET_ENUM);
            }
        }
    }

    void ItemProfileWarehouse::Setup_SummoningItems()
    {
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
                    auto const SUMMON_INFO{ SummonInfo(ORIGIN_ENUM, RACE_ENUM, ROLE_ENUM) };

                    if (ORIGIN_ENUM == origin_type::Statue)
                    {
                        for (auto const ELEMENT_TYPE :
                             misc_type::ElementTypes(misc_type::Summoning_Statue, true))
                        {
                            MakeLoopOverMaterialsForMiscType(
                                misc_type::Summoning_Statue, ELEMENT_TYPE, SUMMON_INFO);
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

                        for (auto const ELEMENT_TYPE : misc_type::ElementTypes(MISC_TYPE, true))
                        {
                            MakeLoopOverMaterialsForMiscType(MISC_TYPE, ELEMENT_TYPE, SUMMON_INFO);
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
        const set_type::Enum SET_TYPE,
        const armor::base_type::Enum MISSING_BASE_TYPE)
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
        else
        {
            if (THIN_PROFILE.ArmorInfo().WasMadeInvalidWithoutBaseType())
            {
                M_ASSERT_OR_LOGANDTHROW_SS(
                    (MISSING_BASE_TYPE != armor::base_type::Count),
                    "item::ItemProfileWarehouse::ElementTypesIncludingNone(thin_profile={"
                        << THIN_PROFILE.ToString() << "}, named_type="
                        << ((NAMED_TYPE == named_type::Count) ? "Count"
                                                              : named_type::ToString(NAMED_TYPE))
                        << ", set_type="
                        << ((SET_TYPE == set_type::Count) ? "Count" : set_type::ToString(SET_TYPE))
                        << ", base_type=Count) found "
                           "THIN_PROFILE.ArmorInfo().WasMadeInvalidWithoutBaseType() but the given "
                           "base_type was invalid.");

                elementTypes
                    = THIN_PROFILE.ArmorInfo().ElementTypesWithGivenBaseType(MISSING_BASE_TYPE);
            }
            else if (THIN_PROFILE.IsArmor())
            {
                elementTypes = THIN_PROFILE.ArmorInfo().ElementTypes();
            }
            else if (THIN_PROFILE.IsWeapon())
            {
                elementTypes = THIN_PROFILE.WeaponInfo().ElementTypes();
            }
        }

        M_ASSERT_OR_LOGANDTHROW_SS(
            (elementTypes.empty() == false),
            "item::ItemProfileWarehouse::ElementTypesIncludingNone(thin_profile={"
                << THIN_PROFILE.ToString() << "}, named_type="
                << ((NAMED_TYPE == named_type::Count) ? "Count" : named_type::ToString(NAMED_TYPE))
                << ", set_type="
                << ((SET_TYPE == set_type::Count) ? "Count" : set_type::ToString(SET_TYPE))
                << ", forced_base_type="
                << ((MISSING_BASE_TYPE == armor::base_type::Count)
                        ? "Count"
                        : armor::base_type::ToString(MISSING_BASE_TYPE))
                << ") resulted in an elementType vector that was empty.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (std::find(std::begin(elementTypes), std::end(elementTypes), element_type::None)
             != std::end(elementTypes)),
            "item::ItemProfileWarehouse::ElementTypesIncludingNone(thin_profile={"
                << THIN_PROFILE.ToString() << "}, named_type="
                << ((NAMED_TYPE == named_type::Count) ? "Count" : named_type::ToString(NAMED_TYPE))
                << ", set_type="
                << ((SET_TYPE == set_type::Count) ? "Count" : set_type::ToString(SET_TYPE))
                << ", forced_base_type="
                << ((MISSING_BASE_TYPE == armor::base_type::Count)
                        ? "Count"
                        : armor::base_type::ToString(MISSING_BASE_TYPE))
                << ") resulted in an elementType vector that did not contain "
                   "element_type::None.");

        return elementTypes;
    }

    void ItemProfileWarehouse::MakeLoopOverMaterialsForThinProfile(
        const ItemProfileThin & THIN_PROFILE,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE)
    {
        auto const BASEMATERIAL_VEC_PAIR_VEC{ GetMaterialsFromThinProfile(
            THIN_PROFILE, NAMED_TYPE, SET_TYPE) };

        M_ASSERT_OR_LOGANDTHROW_SS(
            (BASEMATERIAL_VEC_PAIR_VEC.empty() == false),
            "item::ItemProfileWarehouse::MakeLoopOverMaterialsForThinProfile(thin_profile="
                << THIN_PROFILE.ToString() << ", named_type=" << named_type::ToString(NAMED_TYPE)
                << ", set_type=" << set_type::ToString(SET_TYPE)
                << ") BASEMATERIAL_VEC_PAIR_VEC (the result of GetMaterialsFromThinProfile()) was "
                   "empty.");

        for (auto const & NEXT_BASEMATERIALVECPAIR : BASEMATERIAL_VEC_PAIR_VEC)
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (NEXT_BASEMATERIALVECPAIR.second.first.empty() == false),
                "item::ItemProfileWarehouse::MakeLoopOverMaterialsForThinProfile(thin_profile="
                    << THIN_PROFILE.ToString()
                    << ", named_type=" << named_type::ToString(NAMED_TYPE)
                    << ", set_type=" << set_type::ToString(SET_TYPE)
                    << ", BASEMATERIAL_VEC_PAIR_VEC.size()=" << BASEMATERIAL_VEC_PAIR_VEC.size()
                    << ") was given an empty "
                    << "NEXT_BASEMATERIALVECPAIR.second.first (primary material vec).");

            for (auto const MATERIAL_PRI : NEXT_BASEMATERIALVECPAIR.second.first)
            {
                if (NEXT_BASEMATERIALVECPAIR.second.second.empty())
                {
                    MakeLoopOverElementTypesForThinProfile(
                        THIN_PROFILE,
                        NAMED_TYPE,
                        SET_TYPE,
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
                            MakeLoopOverElementTypesForThinProfile(
                                THIN_PROFILE,
                                NAMED_TYPE,
                                SET_TYPE,
                                MATERIAL_PRI,
                                MATERIAL_SEC,
                                NEXT_BASEMATERIALVECPAIR.first);
                        }
                    }
                }
            }
        }
    }

    void ItemProfileWarehouse::MakeLoopOverElementTypesForThinProfile(
        const ItemProfileThin & THIN_PROFILE,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE,
        const material::Enum MATERIAL_PRIMARY,
        const material::Enum MATERIAL_SECONDARY,
        const armor::base_type::Enum MISSING_BASE_TYPE)
    {
        for (auto const ELEMENT_TYPE :
             ElementTypesIncludingNone(THIN_PROFILE, NAMED_TYPE, SET_TYPE, MISSING_BASE_TYPE))
        {
            MakeFromThinProfile(
                THIN_PROFILE,
                NAMED_TYPE,
                SET_TYPE,
                ELEMENT_TYPE,
                MATERIAL_PRIMARY,
                MATERIAL_SECONDARY,
                MISSING_BASE_TYPE);
        }
    }

    void ItemProfileWarehouse::MakeFromThinProfile(
        const ItemProfileThin & THIN_PROFILE,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE,
        const element_type::Enum ELEMENT_TYPE,
        const material::Enum MATERIAL_PRI,
        const material::Enum MATERIAL_SEC,
        const armor::base_type::Enum MISSING_BASE_TYPE)
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
                        MISSING_BASE_TYPE,
                        MATERIAL_PRI,
                        MATERIAL_SEC,
                        NAMED_TYPE,
                        SET_TYPE,
                        ELEMENT_TYPE,
                        false);

                    profilePixie.SetGauntlets(
                        THIN_PROFILE,
                        MISSING_BASE_TYPE,
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
                        MISSING_BASE_TYPE,
                        MATERIAL_PRI,
                        MATERIAL_SEC,
                        NAMED_TYPE,
                        SET_TYPE,
                        ELEMENT_TYPE,
                        false);

                    profilePixie.SetPants(
                        THIN_PROFILE,
                        MISSING_BASE_TYPE,
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
                        MISSING_BASE_TYPE,
                        MATERIAL_PRI,
                        MATERIAL_SEC,
                        NAMED_TYPE,
                        SET_TYPE,
                        ELEMENT_TYPE,
                        false);

                    profilePixie.SetBoots(
                        THIN_PROFILE,
                        MISSING_BASE_TYPE,
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
                        MISSING_BASE_TYPE,
                        MATERIAL_PRI,
                        MATERIAL_SEC,
                        NAMED_TYPE,
                        SET_TYPE,
                        ELEMENT_TYPE,
                        false);

                    profilePixie.SetShirt(
                        THIN_PROFILE,
                        MISSING_BASE_TYPE,
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
                        MISSING_BASE_TYPE,
                        MATERIAL_PRI,
                        MATERIAL_SEC,
                        NAMED_TYPE,
                        SET_TYPE,
                        ELEMENT_TYPE,
                        false);

                    profilePixie.SetBracer(
                        THIN_PROFILE,
                        MISSING_BASE_TYPE,
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
                        MISSING_BASE_TYPE,
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
                "ItemProfileWarehouse::MakeFromThinProfile(thin_profile="
                    << THIN_PROFILE.ToString() << ", named_type="
                    << ((NAMED_TYPE == named_type::Count) ? "Count"
                                                          : named_type::ToString(NAMED_TYPE))
                    << ", set_type="
                    << ((SET_TYPE == set_type::Count) ? "Count" : set_type::ToString(SET_TYPE))
                    << ", element_type=" << element_type::ToString(ELEMENT_TYPE)
                    << ", mat_pri=" << material::ToString(MATERIAL_PRI)
                    << ", mat_sec=" << material::ToString(MATERIAL_SEC) << ", missing_base_type="
                    << ((MISSING_BASE_TYPE == armor::base_type::Count)
                            ? "Count"
                            : armor::base_type::ToString(MISSING_BASE_TYPE))
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
        ss << "ItemProfileWarehouse::MakeFromThinProfile(thin_profile=" << THIN_PROFILE.ToString()
           << ", named_type="
           << ((NAMED_TYPE == named_type::Count) ? "Count" : named_type::ToString(NAMED_TYPE))
           << ", set_type="
           << ((SET_TYPE == set_type::Count) ? "Count" : set_type::ToString(SET_TYPE))
           << ", element_type=" << element_type::ToString(ELEMENT_TYPE)
           << ", mat_pri=" << material::ToString(MATERIAL_PRI)
           << ", mat_sec=" << material::ToString(MATERIAL_SEC) << ", armor_base_type="
           << ((MISSING_BASE_TYPE == armor::base_type::Count)
                   ? "Count"
                   : armor::base_type::ToString(MISSING_BASE_TYPE))
           << ") was unable to find the type of the given weapon or armor.";

        throw std::runtime_error(ss.str());
    }

    const BaseMaterialVecPairVec_t ItemProfileWarehouse::GetMaterialsFromThinProfile(
        const ItemProfileThin & THIN_PROFILE,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE)
    {
        auto const IS_MAGICAL_ITEM{
            ((NAMED_TYPE != named_type::NotNamed) && (NAMED_TYPE != named_type::Count))
            || ((SET_TYPE != set_type::Count) && (SET_TYPE != set_type::NotASet))
        };

        if ((NAMED_TYPE != named_type::NotNamed) && (NAMED_TYPE != named_type::Count))
        {
            auto const NAMED_MATERIALS_VEC_PAIR{ matColl_.RemoveLameMaterialsForSpecialItems(
                matColl_.VectorPairByNamedType(NAMED_TYPE)) };

            // often there are no materials for a named_type and so we need to check and
            // fall-through here
            if (NAMED_MATERIALS_VEC_PAIR.first.empty() == false)
            {
                return { std::make_pair(armor::base_type::Count, NAMED_MATERIALS_VEC_PAIR) };
            }
        }

        if (THIN_PROFILE.IsMisc())
        {
            auto materials{ matColl_.VectorPairByMiscType(THIN_PROFILE.MiscType()) };

            if (IS_MAGICAL_ITEM)
            {
                matColl_.RemoveLameMaterialsForSpecialItems(materials);
            }

            return { std::make_pair(armor::base_type::Count, materials) };
        }
        else if (THIN_PROFILE.IsArmor())
        {
            BaseMaterialVecPairVec_t baseTypeMaterialPairs;

            // can't use THIN_PROFILE.ArmorInfo().Is_() functions here because some thin profiles
            // are missing their base_types, so use THIN_PROFILE.ArmorInfo().Type() instead which is
            // always valid
            switch (THIN_PROFILE.ArmorInfo().Type())
            {
                case armor_type::Shield:
                {
                    baseTypeMaterialPairs = { std::make_pair(
                        armor::base_type::Count,
                        Materials(THIN_PROFILE.ArmorInfo().ShieldType())) };

                    break;
                }
                case armor_type::Helm:
                {
                    baseTypeMaterialPairs = { std::make_pair(
                        armor::base_type::Count, Materials(THIN_PROFILE.ArmorInfo().HelmType())) };

                    break;
                }
                case armor_type::Gauntlets:
                {
                    for (int i(0); i < armor::base_type::Count; ++i)
                    {
                        auto const NEXT_BASE_ENUM{ static_cast<armor::base_type::Enum>(i) };

                        if (IS_MAGICAL_ITEM && (NEXT_BASE_ENUM == armor::base_type::Plain)
                            && (THIN_PROFILE.ArmorBaseTypeRestriction() != armor::base_type::Plain))
                        {
                            continue;
                        }

                        if ((THIN_PROFILE.ArmorBaseTypeRestriction() == NEXT_BASE_ENUM)
                            || (THIN_PROFILE.ArmorBaseTypeRestriction() == armor::base_type::Count))
                        {
                            baseTypeMaterialPairs.emplace_back(
                                std::make_pair(NEXT_BASE_ENUM, MaterialsGauntlets(NEXT_BASE_ENUM)));
                        }
                    }

                    break;
                }
                case armor_type::Pants:
                {
                    for (int i(0); i < armor::base_type::Count; ++i)
                    {
                        auto const NEXT_BASE_ENUM{ static_cast<armor::base_type::Enum>(i) };

                        if (IS_MAGICAL_ITEM && (NEXT_BASE_ENUM == armor::base_type::Plain)
                            && (THIN_PROFILE.ArmorBaseTypeRestriction() != armor::base_type::Plain))
                        {
                            continue;
                        }

                        if ((THIN_PROFILE.ArmorBaseTypeRestriction() == NEXT_BASE_ENUM)
                            || (THIN_PROFILE.ArmorBaseTypeRestriction() == armor::base_type::Count))
                        {
                            baseTypeMaterialPairs.emplace_back(
                                std::make_pair(NEXT_BASE_ENUM, MaterialsPants(NEXT_BASE_ENUM)));
                        }
                    }

                    break;
                }
                case armor_type::Boots:
                {
                    for (int i(0); i < armor::base_type::Count; ++i)
                    {
                        auto const NEXT_BASE_ENUM{ static_cast<armor::base_type::Enum>(i) };

                        if (IS_MAGICAL_ITEM && (NEXT_BASE_ENUM == armor::base_type::Plain)
                            && (THIN_PROFILE.ArmorBaseTypeRestriction() != armor::base_type::Plain))
                        {
                            continue;
                        }

                        if ((THIN_PROFILE.ArmorBaseTypeRestriction() == NEXT_BASE_ENUM)
                            || (THIN_PROFILE.ArmorBaseTypeRestriction() == armor::base_type::Count))
                        {
                            baseTypeMaterialPairs.emplace_back(
                                std::make_pair(NEXT_BASE_ENUM, MaterialsBoots(NEXT_BASE_ENUM)));
                        }
                    }

                    break;
                }
                case armor_type::Shirt:
                {
                    for (int i(0); i < armor::base_type::Count; ++i)
                    {
                        auto const NEXT_BASE_ENUM{ static_cast<armor::base_type::Enum>(i) };

                        if (IS_MAGICAL_ITEM && (NEXT_BASE_ENUM == armor::base_type::Plain)
                            && (THIN_PROFILE.ArmorBaseTypeRestriction() != armor::base_type::Plain))
                        {
                            continue;
                        }

                        if ((THIN_PROFILE.ArmorBaseTypeRestriction() == NEXT_BASE_ENUM)
                            || (THIN_PROFILE.ArmorBaseTypeRestriction() == armor::base_type::Count))
                        {
                            baseTypeMaterialPairs.emplace_back(
                                std::make_pair(NEXT_BASE_ENUM, MaterialsShirt(NEXT_BASE_ENUM)));
                        }
                    }

                    break;
                }
                case armor_type::Bracers:
                {
                    for (int i(0); i < armor::base_type::Count; ++i)
                    {
                        auto const NEXT_BASE_ENUM{ static_cast<armor::base_type::Enum>(i) };

                        if (IS_MAGICAL_ITEM && (NEXT_BASE_ENUM == armor::base_type::Plain)
                            && (THIN_PROFILE.ArmorBaseTypeRestriction() != armor::base_type::Plain))
                        {
                            continue;
                        }

                        if ((THIN_PROFILE.ArmorBaseTypeRestriction() == NEXT_BASE_ENUM)
                            || (THIN_PROFILE.ArmorBaseTypeRestriction() == armor::base_type::Count))
                        {
                            baseTypeMaterialPairs.emplace_back(
                                std::make_pair(NEXT_BASE_ENUM, MaterialsBracer(NEXT_BASE_ENUM)));
                        }
                    }

                    break;
                }
                case armor_type::Aventail:
                {
                    for (int i(0); i < armor::base_type::Count; ++i)
                    {
                        auto const NEXT_BASE_ENUM{ static_cast<armor::base_type::Enum>(i) };

                        if (IS_MAGICAL_ITEM && (NEXT_BASE_ENUM == armor::base_type::Plain)
                            && (THIN_PROFILE.ArmorBaseTypeRestriction() != armor::base_type::Plain))
                        {
                            continue;
                        }

                        if ((THIN_PROFILE.ArmorBaseTypeRestriction() == NEXT_BASE_ENUM)
                            || (THIN_PROFILE.ArmorBaseTypeRestriction() == armor::base_type::Count))
                        {
                            baseTypeMaterialPairs.emplace_back(
                                std::make_pair(NEXT_BASE_ENUM, MaterialsAventail(NEXT_BASE_ENUM)));
                        }
                    }

                    break;
                }
                case armor_type::Covering:
                {
                    baseTypeMaterialPairs = { std::make_pair(
                        armor::base_type::Count, Materials(THIN_PROFILE.ArmorInfo().CoverType())) };

                    break;
                }
                case armor_type::Skin:
                case armor_type::NotArmor:
                case armor_type::Count:
                {
                    std::ostringstream ss;
                    ss << "ItemProfileWarehouse::GetMaterialsFromThinProfile(thin_profile="
                       << THIN_PROFILE.ToString() << ") "
                       << "-was armor but THIN_PROFILE.ArmorInfo().Type() was invalid.";

                    throw std::runtime_error(ss.str());
                }
            }

            if (IS_MAGICAL_ITEM)
            {
                for (auto & baseRestrictionMaterialVecsPair : baseTypeMaterialPairs)
                {
                    baseRestrictionMaterialVecsPair.second
                        = matColl_.RemoveLameMaterialsForSpecialItems(
                            baseRestrictionMaterialVecsPair.second);
                }
            }

            baseTypeMaterialPairs.erase(
                std::remove_if(
                    std::begin(baseTypeMaterialPairs),
                    std::end(baseTypeMaterialPairs),
                    [](auto const & BASE_RESTRICTION_MATERIAL_VECS_PAIR) {
                        return BASE_RESTRICTION_MATERIAL_VECS_PAIR.second.first.empty();
                    }),
                std::end(baseTypeMaterialPairs));

            return baseTypeMaterialPairs;
        }
        else if (THIN_PROFILE.IsWeapon())
        {
            MaterialVecPair_t materialVecPair;

            if (THIN_PROFILE.WeaponInfo().IsSword())
            {
                materialVecPair = Materials(THIN_PROFILE.WeaponInfo().SwordType());
            }
            else if (THIN_PROFILE.WeaponInfo().IsAxe())
            {
                materialVecPair = Materials(THIN_PROFILE.WeaponInfo().AxeType());
            }
            else if (THIN_PROFILE.WeaponInfo().IsClub())
            {
                materialVecPair = Materials(THIN_PROFILE.WeaponInfo().ClubType());
            }
            else if (THIN_PROFILE.WeaponInfo().IsWhip())
            {
                materialVecPair = Materials(THIN_PROFILE.WeaponInfo().WhipType());
            }
            else if (THIN_PROFILE.WeaponInfo().IsProjectile())
            {
                materialVecPair = Materials(THIN_PROFILE.WeaponInfo().ProjectileType());
            }
            else if (THIN_PROFILE.WeaponInfo().IsBladedStaff())
            {
                materialVecPair = Materials(THIN_PROFILE.WeaponInfo().BladedStaffType());
            }
            else if (THIN_PROFILE.WeaponInfo().IsKnife())
            {
                materialVecPair = MaterialsKnife();
            }
            else if (THIN_PROFILE.WeaponInfo().IsDagger())
            {
                materialVecPair = MaterialsDagger();
            }
            else if (THIN_PROFILE.WeaponInfo().IsStaff())
            {
                materialVecPair = MaterialsStaff();
            }
            else if (THIN_PROFILE.WeaponInfo().IsQuarterstaff())
            {
                materialVecPair = MaterialsQuarterStaff();
            }

            if (IS_MAGICAL_ITEM)
            {
                matColl_.RemoveLameMaterialsForSpecialItems(materialVecPair);
            }

            return { std::make_pair(armor::base_type::Count, materialVecPair) };
        }

        std::ostringstream ss;
        ss << "ItemProfileWarehouse::GetMaterialsFromThinProfile(thin_profile="
           << THIN_PROFILE.ToString() << ") "
           << "-was unable to find the type of the given ThinProfile to make materials from.";

        throw std::runtime_error(ss.str());
    }

    void ItemProfileWarehouse::MakeLoopOverMaterialsForMiscType(
        const misc_type::Enum MISC_TYPE,
        const element_type::Enum ELEMENT_TYPE,
        const creature::SummonInfo & SUMMON_INFO)
    {
        auto appendMiscItemProfile{ [&](const material::Enum MATERIAL_PRIMARY,
                                        const material::Enum MATERIAL_SECONDARY) {
            if (misc_type::HasOnlyPixieVersion(MISC_TYPE) == false)
            {
                ItemProfile profile;

                profile.SetMisc(
                    MISC_TYPE,
                    false,
                    MATERIAL_PRIMARY,
                    MATERIAL_SECONDARY,
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
                    MATERIAL_PRIMARY,
                    MATERIAL_SECONDARY,
                    set_type::NotASet,
                    ELEMENT_TYPE,
                    SUMMON_INFO);

                AppendToEitherNormalOrReligiousVector(pixieProfile);
            }
        } };

        auto const MATERIALS{ matColl_.VectorPairByMiscType(MISC_TYPE) };

        M_ASSERT_OR_LOGANDTHROW_SS(
            (MATERIALS.first.empty() == false),
            "ItemProfileWarehouse::MakeLoopOverMaterialsForMiscType() failed to find any "
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
            "ItemProfileWarehouse::MakeLoopOverMaterialsForMiscType() failed to find any"
                << " valid material combinations for misc_type=" << misc_type::ToString(MISC_TYPE));
    }

    const MaterialVecPair_t ItemProfileWarehouse::Materials(const weapon::axe_type::Enum)
    {
        return matColl_.CoreMetalAndCoreSecondary();
    }

    const MaterialVecPair_t ItemProfileWarehouse::Materials(const weapon::bladedstaff_type::Enum)
    {
        return matColl_.MakeVectorPairSortedAndUnique({ material::Wood }, matColl_.CoreSecondary());
    }

    const MaterialVecPair_t ItemProfileWarehouse::Materials(const weapon::club_type::Enum)
    {
        return matColl_.CoreMetalAndCoreSecondary();
    }

    const MaterialVecPair_t
        ItemProfileWarehouse::Materials(const weapon::projectile_type::Enum PROJECTILE_TYPE)
    {

        using namespace weapon;

        switch (PROJECTILE_TYPE)
        {
            case projectile_type::Blowpipe:
            {
                return matColl_.MakeVectorPairSortedAndUnique(
                    { material::Wood, material::Stone, material::Bone, material::Obsidian },
                    matColl_.CoreSecondary());
            }

            case projectile_type::Sling:
            {
                return matColl_.MakeVectorPairSortedAndUnique(
                    { material::Rope }, matColl_.CoreSecondary());
            }
            case projectile_type::Crossbow:
            case projectile_type::Shortbow:
            case projectile_type::Longbow:
            {
                return matColl_.MakeVectorPairSortedAndUnique(
                    { material::Wood }, matColl_.CoreSecondary());
            }
            case projectile_type::CompositeBow:
            {
                return matColl_.MakeVectorPairSortedAndUnique(
                    { material::Horn }, matColl_.CoreSecondary());
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
        return matColl_.CoreMetalAndCoreSecondary();
    }

    const MaterialVecPair_t ItemProfileWarehouse::Materials(const weapon::whip_type::Enum WHIP_TYPE)
    {

        using namespace weapon;

        if (WHIP_TYPE == whip_type::Bullwhip)
        {
            return matColl_.MakeVectorPairSortedAndUnique(
                { material::HardLeather }, matColl_.CoreSecondary());
        }
        else
        {
            return matColl_.CoreMetalAndCoreSecondary();
        }
    }

    const MaterialVecPair_t ItemProfileWarehouse::MaterialsKnife()
    {
        return matColl_.CorePrimaryWithoutPearlAndSecondary();
    }

    const MaterialVecPair_t ItemProfileWarehouse::MaterialsDagger() { return MaterialsKnife(); }

    const MaterialVecPair_t ItemProfileWarehouse::MaterialsStaff()
    {
        return matColl_.MakeVectorPairSortedAndUnique({ material::Wood }, matColl_.CoreSecondary());
    }

    const MaterialVecPair_t ItemProfileWarehouse::MaterialsQuarterStaff()
    {
        return MaterialsStaff();
    }

    const MaterialVecPair_t ItemProfileWarehouse::Materials(const armor::cover_type::Enum)
    {
        return matColl_.MakeVectorPairSortedAndUnique(
            { material::Cloth, material::SoftLeather, material::HardLeather },
            matColl_.CoreSecondary());
    }

    const MaterialVecPair_t ItemProfileWarehouse::Materials(const armor::helm_type::Enum HELM_TYPE)
    {
        using namespace armor;

        if (HELM_TYPE == helm_type::Leather)
        {
            return matColl_.MakeVectorPairSortedAndUnique(
                { material::HardLeather }, matColl_.CoreSecondary());
        }
        else
        {
            return matColl_.MakeVectorPairSortedAndUnique(
                matColl_.CoreMetal(), matColl_.CoreSecondary());
        }
    }

    const MaterialVecPair_t ItemProfileWarehouse::Materials(const armor::shield_type::Enum)
    {
        return matColl_.MakeVectorPairSortedAndUnique(
            matColl_.MakeVectorSortedAndUnique(matColl_.CoreMetal(), { material::Wood }),
            matColl_.CoreSecondary());
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
                return matColl_.CoreMetal();
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
        return matColl_.MakeVectorPairSortedAndUnique(
            MaterialsPrimaryFromArmorBaseType(BASE_TYPE), matColl_.CoreSecondary());
    }

    const MaterialVecPair_t
        ItemProfileWarehouse::MaterialsFromBaseTypeNoCloth(const armor::base_type::Enum BASE_TYPE)
    {
        MaterialVec_t primaryMaterials{ material::SoftLeather, material::HardLeather };

        if (BASE_TYPE != armor::base_type::Plain)
        {
            primaryMaterials = MaterialsPrimaryFromArmorBaseType(BASE_TYPE);
        }

        return matColl_.MakeVectorPairSortedAndUnique(primaryMaterials, matColl_.CoreSecondary());
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

        if (armor::base_type::Count == BASE_TYPE_RESTRICTION)
        {
            thinProfiles.emplace_back(ItemProfileThin::MakeArmorNonSpecificInvalidWithoutBaseType(
                armor_type::Aventail, BASE_TYPE_RESTRICTION));

            thinProfiles.emplace_back(ItemProfileThin::MakeArmorNonSpecificInvalidWithoutBaseType(
                armor_type::Boots, BASE_TYPE_RESTRICTION));

            thinProfiles.emplace_back(ItemProfileThin::MakeArmorNonSpecificInvalidWithoutBaseType(
                armor_type::Bracers, BASE_TYPE_RESTRICTION));

            thinProfiles.emplace_back(ItemProfileThin::MakeArmorNonSpecificInvalidWithoutBaseType(
                armor_type::Gauntlets, BASE_TYPE_RESTRICTION));

            thinProfiles.emplace_back(ItemProfileThin::MakeArmorNonSpecificInvalidWithoutBaseType(
                armor_type::Pants, BASE_TYPE_RESTRICTION));

            thinProfiles.emplace_back(ItemProfileThin::MakeArmorNonSpecificInvalidWithoutBaseType(
                armor_type::Shirt, BASE_TYPE_RESTRICTION));
        }
        else
        {
            thinProfiles.emplace_back(ItemProfileThin::MakeArmorNonSpecificWithBaseTypeRestriction(
                armor_type::Aventail, BASE_TYPE_RESTRICTION));

            thinProfiles.emplace_back(ItemProfileThin::MakeArmorNonSpecificWithBaseTypeRestriction(
                armor_type::Boots, BASE_TYPE_RESTRICTION));

            thinProfiles.emplace_back(ItemProfileThin::MakeArmorNonSpecificWithBaseTypeRestriction(
                armor_type::Bracers, BASE_TYPE_RESTRICTION));

            thinProfiles.emplace_back(ItemProfileThin::MakeArmorNonSpecificWithBaseTypeRestriction(
                armor_type::Gauntlets, BASE_TYPE_RESTRICTION));

            thinProfiles.emplace_back(ItemProfileThin::MakeArmorNonSpecificWithBaseTypeRestriction(
                armor_type::Pants, BASE_TYPE_RESTRICTION));

            thinProfiles.emplace_back(ItemProfileThin::MakeArmorNonSpecificWithBaseTypeRestriction(
                armor_type::Shirt, BASE_TYPE_RESTRICTION));
        }

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
                         ItemProfileThin::MakeArmorNonSpecificWithBaseTypeRestriction(
                             armor_type::Bracers, armor::base_type::Plain) };
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
                return { ItemProfileThin::MakeArmorNonSpecificWithBaseTypeRestriction(
                             armor_type::Boots, armor::base_type::Plain),
                         ItemProfileThin::MakeArmorNonSpecificWithBaseTypeRestriction(
                             armor_type::Pants, armor::base_type::Plain),
                         ItemProfileThin::MakeArmorNonSpecificWithBaseTypeRestriction(
                             armor_type::Shirt, armor::base_type::Plain),
                         ItemProfileThin::MakeArmorNonSpecificWithBaseTypeRestriction(
                             armor_type::Gauntlets, armor::base_type::Plain),
                         ItemProfileThin::MakeArmorSpecific(armor::cover_type::Cloak),
                         ItemProfileThin::MakeWeaponKnifeOrDagger(true),
                         ItemProfileThin::MakeMisc(misc_type::LockPicks) };
            }

            case set_type::TheMagus:
            case set_type::TheNecromancers:
            case set_type::TheWarlocks:
            {
                return { ItemProfileThin::MakeArmorNonSpecificWithBaseTypeRestriction(
                             armor_type::Boots, armor::base_type::Plain),
                         ItemProfileThin::MakeArmorNonSpecificWithBaseTypeRestriction(
                             armor_type::Pants, armor::base_type::Plain),
                         ItemProfileThin::MakeArmorNonSpecificWithBaseTypeRestriction(
                             armor_type::Shirt, armor::base_type::Plain),
                         ItemProfileThin::MakeArmorNonSpecificWithBaseTypeRestriction(
                             armor_type::Gauntlets, armor::base_type::Plain),
                         ItemProfileThin::MakeArmorSpecific(armor::cover_type::Robe),
                         ItemProfileThin::MakeMisc(misc_type::Wand) };
            }

            case set_type::TheLichKings:
            {
                return { ItemProfileThin::MakeArmorNonSpecificWithBaseTypeRestriction(
                             armor_type::Boots, armor::base_type::Plain),
                         ItemProfileThin::MakeArmorNonSpecificWithBaseTypeRestriction(
                             armor_type::Pants, armor::base_type::Plain),
                         ItemProfileThin::MakeArmorNonSpecificWithBaseTypeRestriction(
                             armor_type::Shirt, armor::base_type::Plain),
                         ItemProfileThin::MakeArmorNonSpecificWithBaseTypeRestriction(
                             armor_type::Gauntlets, armor::base_type::Plain),
                         ItemProfileThin::MakeArmorSpecific(armor::cover_type::Robe),
                         ItemProfileThin::MakeMisc(misc_type::Litch_Hand) };
            }

            case set_type::TheSages:
            case set_type::TheShamans:
            case set_type::TheOracles:
            {
                return { ItemProfileThin::MakeArmorNonSpecificWithBaseTypeRestriction(
                             armor_type::Boots, armor::base_type::Plain),
                         ItemProfileThin::MakeArmorNonSpecificWithBaseTypeRestriction(
                             armor_type::Pants, armor::base_type::Plain),
                         ItemProfileThin::MakeArmorNonSpecificWithBaseTypeRestriction(
                             armor_type::Shirt, armor::base_type::Plain),
                         ItemProfileThin::MakeArmorNonSpecificWithBaseTypeRestriction(
                             armor_type::Gauntlets, armor::base_type::Plain),
                         ItemProfileThin::MakeArmorSpecific(armor::cover_type::Robe),
                         ItemProfileThin::MakeMisc(misc_type::Wand) };
            }

            case set_type::TheAngelic:
            {
                return { ItemProfileThin::MakeArmorNonSpecificWithBaseTypeRestriction(
                             armor_type::Boots, armor::base_type::Plain),
                         ItemProfileThin::MakeArmorNonSpecificWithBaseTypeRestriction(
                             armor_type::Pants, armor::base_type::Plain),
                         ItemProfileThin::MakeArmorNonSpecificWithBaseTypeRestriction(
                             armor_type::Shirt, armor::base_type::Plain),
                         ItemProfileThin::MakeArmorNonSpecificWithBaseTypeRestriction(
                             armor_type::Gauntlets, armor::base_type::Plain),
                         ItemProfileThin::MakeArmorSpecific(armor::cover_type::Robe),
                         ItemProfileThin::MakeMisc(misc_type::Angel_Braid) };
            }

            case set_type::TheBalladeers:
            case set_type::TheTroubadours:
            case set_type::TheMuses:
            {
                return { ItemProfileThin::MakeArmorNonSpecificWithBaseTypeRestriction(
                             armor_type::Boots, armor::base_type::Mail),
                         ItemProfileThin::MakeArmorNonSpecificWithBaseTypeRestriction(
                             armor_type::Pants, armor::base_type::Mail),
                         ItemProfileThin::MakeArmorNonSpecificWithBaseTypeRestriction(
                             armor_type::Shirt, armor::base_type::Mail),
                         ItemProfileThin::MakeArmorNonSpecificWithBaseTypeRestriction(
                             armor_type::Gauntlets, armor::base_type::Plain),
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
                return { ItemProfileThin::MakeArmorNonSpecificWithBaseTypeRestriction(
                             armor_type::Boots, armor::base_type::Plate),
                         ItemProfileThin::MakeArmorNonSpecificWithBaseTypeRestriction(
                             armor_type::Pants, armor::base_type::Plate),
                         ItemProfileThin::MakeArmorNonSpecificWithBaseTypeRestriction(
                             armor_type::Shirt, armor::base_type::Plate),
                         ItemProfileThin::MakeArmorNonSpecificWithBaseTypeRestriction(
                             armor_type::Gauntlets, armor::base_type::Plate),
                         ItemProfileThin::MakeArmorNonSpecificWithBaseTypeRestriction(
                             armor_type::Bracers, armor::base_type::Plate),
                         ItemProfileThin::MakeArmorSpecific(armor::cover_type::Cape),
                         ItemProfileThin::MakeArmorSpecific(armor::shield_type::Pavis),
                         ItemProfileThin::MakeArmorSpecific(armor::helm_type::Great),
                         ItemProfileThin::MakeArmorNonSpecificWithBaseTypeRestriction(
                             armor_type::Aventail, armor::base_type::Plate) };
            }

            case set_type::TheHunters:
            case set_type::TheSureShot:
            case set_type::TheMarksmans:
            case set_type::TheDeadeye:
            case set_type::TheDuskRanger:
            case set_type::TheVenomBow:
            {
                return { ItemProfileThin::MakeArmorNonSpecificWithBaseTypeRestriction(
                             armor_type::Boots, armor::base_type::Mail),
                         ItemProfileThin::MakeArmorNonSpecificWithBaseTypeRestriction(
                             armor_type::Pants, armor::base_type::Mail),
                         ItemProfileThin::MakeArmorNonSpecificWithBaseTypeRestriction(
                             armor_type::Shirt, armor::base_type::Mail),
                         ItemProfileThin::MakeArmorNonSpecificWithBaseTypeRestriction(
                             armor_type::Gauntlets, armor::base_type::Plain),
                         ItemProfileThin::MakeArmorNonSpecificWithBaseTypeRestriction(
                             armor_type::Bracers, armor::base_type::Mail),
                         ItemProfileThin::MakeArmorSpecific(armor::cover_type::Vest),
                         ItemProfileThin::MakeArmorSpecific(armor::helm_type::Archers) };
            }

            case set_type::TheCritterChannelers:
            case set_type::TheMammalianHead:
            case set_type::TheSavageTaskmasters:
            case set_type::TheMonsterOverseers:
            case set_type::TheBeastRulers:
            {
                return { ItemProfileThin::MakeArmorNonSpecificWithBaseTypeRestriction(
                             armor_type::Boots, armor::base_type::Scale),
                         ItemProfileThin::MakeArmorNonSpecificWithBaseTypeRestriction(
                             armor_type::Pants, armor::base_type::Scale),
                         ItemProfileThin::MakeArmorNonSpecificWithBaseTypeRestriction(
                             armor_type::Shirt, armor::base_type::Scale),
                         ItemProfileThin::MakeArmorNonSpecificWithBaseTypeRestriction(
                             armor_type::Bracers, armor::base_type::Scale),
                         ItemProfileThin::MakeArmorNonSpecificWithBaseTypeRestriction(
                             armor_type::Gauntlets, armor::base_type::Scale),
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
