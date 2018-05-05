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

        for (auto const & ARMOR_THIN_PROFILE : ThinProfilesArmor())
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

    void ItemProfileWarehouse::MakeLoopOverMaterialsForThinProfile(
        const ItemProfileThin & THIN_PROFILE,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE)
    {
        auto const MATERIALS{ GetMaterialsFromThinProfile(THIN_PROFILE, NAMED_TYPE, SET_TYPE) };

        M_ASSERT_OR_LOGANDTHROW_SS(
            (MATERIALS.first.empty() == false),
            "item::ItemProfileWarehouse::MakeLoopOverMaterialsForThinProfile(thin_profile="
                << THIN_PROFILE.ToString() << ", named_type=" << named_type::ToString(NAMED_TYPE)
                << ", set_type=" << set_type::ToString(SET_TYPE)
                << ") GetMaterialsFromThinProfile() returned an empty primary materials vector, "
                   "the secondary had "
                << MATERIALS.second.size() << ".");

        for (auto const MATERIAL_PRI : MATERIALS.first)
        {
            if (MATERIALS.second.empty())
            {
                MakeLoopOverElementTypesForThinProfile(
                    THIN_PROFILE, NAMED_TYPE, SET_TYPE, MATERIAL_PRI, material::Nothing);
            }
            else
            {
                for (auto const MATERIAL_SEC : MATERIALS.second)
                {
                    if (MATERIAL_SEC != item::material::Nothing)
                    {
                        MakeLoopOverElementTypesForThinProfile(
                            THIN_PROFILE, NAMED_TYPE, SET_TYPE, MATERIAL_PRI, MATERIAL_SEC);
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
        const material::Enum MATERIAL_SECONDARY)
    {
        for (auto const ELEMENT_TYPE :
             ElementTypesIncludingNone(THIN_PROFILE, NAMED_TYPE, SET_TYPE))
        {
            MakeFromThinProfile(
                THIN_PROFILE,
                NAMED_TYPE,
                SET_TYPE,
                ELEMENT_TYPE,
                MATERIAL_PRIMARY,
                MATERIAL_SECONDARY);
        }
    }

    void ItemProfileWarehouse::MakeFromThinProfile(
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
                "ItemProfileWarehouse::MakeFromThinProfile(thin_profile="
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
        ss << "ItemProfileWarehouse::MakeFromThinProfile(thin_profile=" << THIN_PROFILE.ToString()
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

    const MaterialVecPair_t ItemProfileWarehouse::GetMaterialsFromThinProfile(
        const ItemProfileThin & THIN_PROFILE,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE)
    {
        if ((NAMED_TYPE != named_type::NotNamed) && (NAMED_TYPE != named_type::Count))
        {
            auto const NAMED_MATERIALS_VEC_PAIR{ matColl_.RemoveLameMaterialsForSpecialItems(
                matColl_.VectorPairByNamedType(NAMED_TYPE)) };

            // often there are no materials for a named_type and so we need to check and
            // fall-through here
            if (NAMED_MATERIALS_VEC_PAIR.first.empty() == false)
            {
                return NAMED_MATERIALS_VEC_PAIR;
            }
        }

        auto const IS_MAGICAL_ITEM{
            ((NAMED_TYPE != named_type::NotNamed) && (NAMED_TYPE != named_type::Count))
            || ((SET_TYPE != set_type::Count) && (SET_TYPE != set_type::NotASet))
        };

        MaterialVecPair_t materials;

        if (THIN_PROFILE.IsMisc())
        {
            materials = matColl_.VectorPairByMiscType(THIN_PROFILE.MiscType());
        }
        else if (THIN_PROFILE.IsArmor())
        {
            materials = GetMaterialsFromThinProfileForArmor(THIN_PROFILE);
        }
        else if (THIN_PROFILE.IsWeapon())
        {
            materials = GetMaterialsFromThinProfileForWeapons(THIN_PROFILE);
        }

        M_ASSERT_OR_LOGANDTHROW_SS(
            (materials.first.empty() == false),
            "item::ItemProfileWarehouse::GetMaterialsFromThinProfile(thin_profile = "
                << THIN_PROFILE.ToString() << ") "
                << "unable to create primary materials for that thin profile.  Secondary had "
                << materials.second.size() << ".");

        if (IS_MAGICAL_ITEM)
        {
            matColl_.RemoveLameMaterialsForSpecialItems(materials);
        }

        M_ASSERT_OR_LOGANDTHROW_SS(
            (materials.first.empty() == false),
            "item::ItemProfileWarehouse::GetMaterialsFromThinProfile(thin_profile = "
                << THIN_PROFILE.ToString() << ") "
                << "was to create primary materials for that thin profile, but it was magical and "
                   "so lame materials were removed, after that, there were no more primary "
                   "materials.  Secondary had "
                << materials.second.size() << ".");

        return materials;
    }

    const MaterialVecPair_t ItemProfileWarehouse::GetMaterialsFromThinProfileForArmor(
        const ItemProfileThin & THIN_PROFILE)
    {
        if ((THIN_PROFILE.ArmorInfo().Type() != armor_type::Covering)
            && (THIN_PROFILE.ArmorInfo().Type() != armor_type::Shield)
            && (THIN_PROFILE.ArmorInfo().Type() != armor_type::Helm))
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (THIN_PROFILE.ArmorInfo().BaseType() != armor::base_type::Count),
                "item::ItemProfileWarehouse::GetMaterialsFromThinProfileForArmor(thin_profile={"
                    << THIN_PROFILE.ToString()
                    << "}) -which is a 'non-specific' armor_type that requires a valid base_type "
                       "but the base_type was Count.");
        }

        MaterialVecPair_t materials;

        switch (THIN_PROFILE.ArmorInfo().Type())
        {
            case armor_type::Covering:
            {
                return Materials(THIN_PROFILE.ArmorInfo().CoverType());
            }
            case armor_type::Shield:
            {
                return Materials(THIN_PROFILE.ArmorInfo().ShieldType());
            }
            case armor_type::Helm:
            {
                return Materials(THIN_PROFILE.ArmorInfo().HelmType());
            }
            case armor_type::Gauntlets:
            {
                return MaterialsGauntlets(THIN_PROFILE.ArmorInfo().BaseType());
            }
            case armor_type::Pants:
            {
                return MaterialsPants(THIN_PROFILE.ArmorInfo().BaseType());
            }
            case armor_type::Boots:
            {
                return MaterialsBoots(THIN_PROFILE.ArmorInfo().BaseType());
            }
            case armor_type::Shirt:
            {
                return MaterialsShirt(THIN_PROFILE.ArmorInfo().BaseType());
            }
            case armor_type::Bracers:
            {
                return MaterialsBracer(THIN_PROFILE.ArmorInfo().BaseType());
            }
            case armor_type::Aventail:
            {
                return MaterialsAventail(THIN_PROFILE.ArmorInfo().BaseType());
            }
            case armor_type::Skin:
            case armor_type::NotArmor:
            case armor_type::Count:
            default:
            {
                std::ostringstream ss;
                ss << "ItemProfileWarehouse::GetMaterialsFromThinProfile(thin_profile="
                   << THIN_PROFILE.ToString() << ") "
                   << "-was armor but THIN_PROFILE.ArmorInfo().Type() was invalid.";

                throw std::runtime_error(ss.str());
            }
        }
    }

    const MaterialVecPair_t ItemProfileWarehouse::GetMaterialsFromThinProfileForWeapons(
        const ItemProfileThin & THIN_PROFILE)
    {
        if (THIN_PROFILE.WeaponInfo().IsSword())
        {
            return Materials(THIN_PROFILE.WeaponInfo().SwordType());
        }
        else if (THIN_PROFILE.WeaponInfo().IsAxe())
        {
            return Materials(THIN_PROFILE.WeaponInfo().AxeType());
        }
        else if (THIN_PROFILE.WeaponInfo().IsClub())
        {
            return Materials(THIN_PROFILE.WeaponInfo().ClubType());
        }
        else if (THIN_PROFILE.WeaponInfo().IsWhip())
        {
            return Materials(THIN_PROFILE.WeaponInfo().WhipType());
        }
        else if (THIN_PROFILE.WeaponInfo().IsProjectile())
        {
            return Materials(THIN_PROFILE.WeaponInfo().ProjectileType());
        }
        else if (THIN_PROFILE.WeaponInfo().IsBladedStaff())
        {
            return Materials(THIN_PROFILE.WeaponInfo().BladedStaffType());
        }
        else if (THIN_PROFILE.WeaponInfo().IsKnife())
        {
            return MaterialsKnife();
        }
        else if (THIN_PROFILE.WeaponInfo().IsDagger())
        {
            return MaterialsDagger();
        }
        else if (THIN_PROFILE.WeaponInfo().IsStaff())
        {
            return MaterialsStaff();
        }
        else if (THIN_PROFILE.WeaponInfo().IsQuarterstaff())
        {
            return MaterialsQuarterStaff();
        }
        else
        {
            std::ostringstream ss;
            ss << "ItemProfileWarehouse::GetMaterialsFromThinProfileForWeapons(thin_profile="
               << THIN_PROFILE.ToString() << ") "
               << "-was armor but that WeaponTypeWrapper's type was not supported.";

            throw std::runtime_error(ss.str());
        }
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

    const MaterialVecPair_t
        ItemProfileWarehouse::Materials(const armor::shield_type::Enum SHIELD_TYPE)
    {
        // when it comes to the name a shield is either material::IsJewel()="jeweled" or
        // material::IsRigid()="reinforced with"

        const MaterialVec_t EXTRA_MATERIALS_FOR_BUCKLERS{ material::Obsidian, material::Bone,
                                                          material::Tin,      material::Iron,
                                                          material::Silver,   material::Gold,
                                                          material::Platinum };

        auto const PRIMARY_MATERIALS{ matColl_.MakeVectorSortedAndUnique(
            { material::Wood, material::Scales, material::Bronze },
            ((SHIELD_TYPE == armor::shield_type::Buckler) ? EXTRA_MATERIALS_FOR_BUCKLERS
                                                          : MaterialVec_t())) };

        return matColl_.MakeVectorPairSortedAndUnique(PRIMARY_MATERIALS, matColl_.CoreSecondary());
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

    const ItemProfileThinVec_t ItemProfileWarehouse::ThinProfilesArmor()
    {
        ItemProfileThinVec_t thinProfiles;

        misc::Vector::AppendMove(
            ItemProfileThin::MakeArmorSpecificAll<armor::cover_type>(), thinProfiles);

        misc::Vector::AppendMove(
            ItemProfileThin::MakeArmorSpecificAll<armor::helm_type>(), thinProfiles);

        misc::Vector::AppendMove(
            ItemProfileThin::MakeArmorSpecificAll<armor::shield_type>(), thinProfiles);

        misc::Vector::AppendMove(
            ItemProfileThin::MakeArmorNonSpecificAll(armor_type::Aventail), thinProfiles);

        misc::Vector::AppendMove(
            ItemProfileThin::MakeArmorNonSpecificAll(armor_type::Boots), thinProfiles);

        misc::Vector::AppendMove(
            ItemProfileThin::MakeArmorNonSpecificAll(armor_type::Bracers), thinProfiles);

        misc::Vector::AppendMove(
            ItemProfileThin::MakeArmorNonSpecificAll(armor_type::Gauntlets), thinProfiles);

        misc::Vector::AppendMove(
            ItemProfileThin::MakeArmorNonSpecificAll(armor_type::Pants), thinProfiles);

        misc::Vector::AppendMove(
            ItemProfileThin::MakeArmorNonSpecificAll(armor_type::Shirt), thinProfiles);

        return thinProfiles;
    }

    const ItemProfileThinVec_t ItemProfileWarehouse::ThinProfiles(const named_type::Enum NAMED_TYPE)
    {
        using namespace armor;
        using namespace weapon;

        auto removeIf{ [](std::vector<ItemProfileThin> & thinProfiles, auto ifLambda) {
            thinProfiles.erase(
                std::remove_if(std::begin(thinProfiles), std::end(thinProfiles), ifLambda),
                std::end(thinProfiles));
        } };

        auto removeIfArmorType{ [&](std::vector<ItemProfileThin> & thinProfiles,
                                    const armor_type::Enum ARMOR_TYPE) {
            removeIf(thinProfiles, [&](auto const & PROFILE) {
                return (PROFILE.ArmorInfo().Type() == ARMOR_TYPE);
            });
        } };

        switch (NAMED_TYPE)
        {
            case named_type::Wicked:
            {
                return ItemProfileThin::MakeWeaponOfTypeAll(weapon_type::Pointed);
            }
            case named_type::Fiendish:
            {
                return ItemProfileThin::MakeWeaponOfTypeAll(weapon_type::Bladed);
            }

            case named_type::Infernal:
            {
                ItemProfileThinVec_t thinProfiles{
                    ItemProfileThin::MakeWeaponSpecific(projectile_type::Longbow),
                    ItemProfileThin::MakeWeaponSpecific(projectile_type::CompositeBow)
                };

                misc::Vector::AppendMove(
                    ItemProfileThin::MakeWeaponOfTypeAll(weapon_type::Sword), thinProfiles);

                misc::Vector::AppendMove(
                    ItemProfileThin::MakeWeaponOfTypeAll(weapon_type::Sling), thinProfiles);

                return thinProfiles;
            }

            case named_type::Raging:
            {
                return ItemProfileThin::MakeWeaponOfTypeAll(weapon_type::Club);
            }

            case named_type::Icy:
            case named_type::Winter:
            case named_type::Frigid:
            case named_type::Diabolic:
            {
                return ItemProfileThin::MakeWeaponOfTypeAll(weapon_type::Melee);
            }

            case named_type::Dancing:
            {
                return ItemProfileThin::MakeWeaponOfTypeAll(weapon_type::Sword);
            }

            case named_type::Marauder:
            {
                ItemProfileThinVec_t thinProfiles{ ItemProfileThin::MakeWeaponOfTypeAll(
                    weapon_type::Sword) };

                removeIf(thinProfiles, [](auto const & PROFILE) {
                    return (
                        (PROFILE.WeaponInfo().SwordType() == sword_type::Shortsword)
                        || (PROFILE.WeaponInfo().SwordType() == sword_type::Falcata)
                        || (PROFILE.WeaponInfo().SwordType() == sword_type::Gladius));
                });

                return thinProfiles;
            }

            case named_type::Wardens:
            {
                ItemProfileThinVec_t thinProfiles{ ItemProfileThin::MakeWeaponSpecific(
                    sword_type::Shortsword) };

                misc::Vector::AppendMove(
                    ItemProfileThin::MakeArmorNonSpecificAll(armor::base_type::Plain),
                    thinProfiles);

                removeIfArmorType(thinProfiles, armor_type::Shield);
                removeIfArmorType(thinProfiles, armor_type::Helm);

                thinProfiles.emplace_back(ItemProfileThin::MakeArmorSpecific(helm_type::Bascinet));
                thinProfiles.emplace_back(ItemProfileThin::MakeArmorSpecific(shield_type::Buckler));

                return thinProfiles;
            }

            case named_type::Princes:
            {
                ItemProfileThinVec_t thinProfiles{ ItemProfileThin::MakeArmorNonSpecificAll(
                    armor::base_type::Mail) };

                removeIfArmorType(thinProfiles, armor_type::Shield);
                removeIfArmorType(thinProfiles, armor_type::Helm);

                thinProfiles.emplace_back(ItemProfileThin::MakeArmorSpecific(helm_type::MailCoif));
                thinProfiles.emplace_back(ItemProfileThin::MakeArmorSpecific(shield_type::Kite));
                thinProfiles.emplace_back(ItemProfileThin::MakeArmorSpecific(cover_type::Cape));

                thinProfiles.emplace_back(ItemProfileThin::MakeWeaponSpecific(sword_type::Rapier));

                return thinProfiles;
            }

            case named_type::Ranger:
            {
                ItemProfileThinVec_t thinProfiles{ ItemProfileThin::MakeArmorNonSpecificAll(
                    armor::base_type::Plain) };

                removeIfArmorType(thinProfiles, armor_type::Shield);
                removeIfArmorType(thinProfiles, armor_type::Helm);

                thinProfiles.emplace_back(ItemProfileThin::MakeArmorSpecific(cover_type::Cloak));
                thinProfiles.emplace_back(ItemProfileThin::MakeArmorSpecific(cover_type::Vest));

                thinProfiles.emplace_back(
                    ItemProfileThin::MakeWeaponSpecific(projectile_type::Longbow));

                thinProfiles.emplace_back(
                    ItemProfileThin::MakeWeaponSpecific(projectile_type::Shortbow));

                thinProfiles.emplace_back(
                    ItemProfileThin::MakeWeaponSpecific(projectile_type::CompositeBow));

                thinProfiles.emplace_back(
                    ItemProfileThin::MakeWeaponSpecific(projectile_type::Sling));

                return thinProfiles;
            }

            case named_type::Samurai:
            {
                ItemProfileThinVec_t thinProfiles{ ItemProfileThin::MakeArmorNonSpecificAll(
                    armor::base_type::Scale) };

                removeIfArmorType(thinProfiles, armor_type::Shield);
                removeIfArmorType(thinProfiles, armor_type::Helm);

                thinProfiles.emplace_back(ItemProfileThin::MakeArmorSpecific(helm_type::Leather));
                thinProfiles.emplace_back(ItemProfileThin::MakeArmorSpecific(cover_type::Cape));

                thinProfiles.emplace_back(
                    ItemProfileThin::MakeWeaponSpecific(sword_type::Longsword));

                return thinProfiles;
            }

            case named_type::Thors:
            {
                ItemProfileThinVec_t thinProfiles{ ItemProfileThin::MakeArmorNonSpecificAll(
                    armor::base_type::Plate) };

                removeIfArmorType(thinProfiles, armor_type::Shield);
                removeIfArmorType(thinProfiles, armor_type::Helm);

                thinProfiles.emplace_back(ItemProfileThin::MakeArmorSpecific(helm_type::Great));
                thinProfiles.emplace_back(ItemProfileThin::MakeArmorSpecific(shield_type::Pavis));
                thinProfiles.emplace_back(ItemProfileThin::MakeArmorSpecific(cover_type::Cape));

                thinProfiles.emplace_back(
                    ItemProfileThin::MakeWeaponSpecific(sword_type::Claymore));

                return thinProfiles;
            }

            case named_type::Nile:
            {
                return { ItemProfileThin::MakeWeaponSpecific(sword_type::Falcata),
                         ItemProfileThin::MakeArmorSpecific(shield_type::Buckler),
                         ItemProfileThin::MakeArmorSpecific(shield_type::Kite),
                         ItemProfileThin::MakeArmorSpecific(helm_type::Leather),
                         ItemProfileThin::MakeArmorNonSpecific(
                             armor_type::Bracers, armor::base_type::Plain),
                         ItemProfileThin::MakeArmorNonSpecific(
                             armor_type::Shirt, armor::base_type::Plain),
                         ItemProfileThin::MakeArmorSpecific(cover_type::Cloak) };
            }

            case named_type::Searing:
            {
                return ItemProfileThin::MakeWeaponOfTypeAll(weapon_type::Bladed);
            }

            case named_type::Burning:
            {
                ItemProfileThinVec_t thinProfiles;

                misc::Vector::AppendMove(
                    ItemProfileThin::MakeWeaponOfTypeAll(weapon_type::Sword), thinProfiles);

                misc::Vector::AppendMove(
                    ItemProfileThin::MakeWeaponOfTypeAll(weapon_type::Axe), thinProfiles);

                misc::Vector::AppendMove(
                    ItemProfileThin::MakeWeaponOfTypeAll(weapon_type::BladedStaff), thinProfiles);

                misc::Vector::AppendMove(
                    ItemProfileThin::MakeWeaponOfTypeAll(weapon_type::Club), thinProfiles);

                misc::Vector::AppendMove(
                    ItemProfileThin::MakeWeaponOfTypeAll(weapon_type::Staff), thinProfiles);

                return thinProfiles;
            }

            case named_type::Fiery:
            {
                ItemProfileThinVec_t thinProfiles;

                misc::Vector::AppendMove(
                    ItemProfileThin::MakeWeaponOfTypeAll(weapon_type::Bow), thinProfiles);

                misc::Vector::AppendMove(
                    ItemProfileThin::MakeWeaponOfTypeAll(weapon_type::Crossbow), thinProfiles);

                misc::Vector::AppendMove(
                    ItemProfileThin::MakeWeaponOfTypeAll(weapon_type::Whip), thinProfiles);

                misc::Vector::AppendMove(
                    ItemProfileThin::MakeWeaponOfTypeAll(weapon_type::BladedStaff), thinProfiles);

                misc::Vector::AppendMove(
                    ItemProfileThin::MakeWeaponOfTypeAll(weapon_type::Sword), thinProfiles);

                return thinProfiles;
            }

            case named_type::Soldiers:
            {
                ItemProfileThinVec_t thinProfiles{
                    ItemProfileThin::MakeWeaponSpecific(projectile_type::Shortbow),
                    ItemProfileThin::MakeWeaponSpecific(projectile_type::Longbow),
                    ItemProfileThin::MakeWeaponSpecific(sword_type::Broadsword),
                    ItemProfileThin::MakeWeaponSpecific(sword_type::Longsword),
                    ItemProfileThin::MakeWeaponSpecific(sword_type::Knightlysword),
                    ItemProfileThin::MakeWeaponSpecific(sword_type::Claymore),
                    ItemProfileThin::MakeWeaponSpecific(axe_type::Battleaxe),
                    ItemProfileThin::MakeWeaponSpecific(axe_type::Waraxe)
                };

                misc::Vector::AppendMove(
                    ItemProfileThin::MakeWeaponOfTypeAll(weapon_type::BladedStaff), thinProfiles);

                misc::Vector::AppendMove(
                    ItemProfileThin::MakeArmorSpecificAll<shield_type>(), thinProfiles);

                misc::Vector::AppendMove(
                    ItemProfileThin::MakeArmorSpecificAll<helm_type>(), thinProfiles);

                misc::Vector::AppendMove(
                    ItemProfileThin::MakeArmorNonSpecificAll(armor_type::Pants), thinProfiles);

                misc::Vector::AppendMove(
                    ItemProfileThin::MakeArmorNonSpecificAll(armor_type::Boots), thinProfiles);

                misc::Vector::AppendMove(
                    ItemProfileThin::MakeArmorNonSpecificAll(armor_type::Shirt), thinProfiles);

                removeIf(thinProfiles, [](auto const & PROFILE) {
                    return (
                        (PROFILE.ArmorInfo().BaseType() == base_type::Plate)
                        || (PROFILE.ArmorInfo().ShieldType() == shield_type::Pavis));
                });

                return thinProfiles;
            }

            case named_type::Dark:
            {
                ItemProfileThinVec_t thinProfiles{
                    ItemProfileThin::MakeWeaponSpecific(projectile_type::Sling),
                    ItemProfileThin::MakeWeaponSpecific(projectile_type::Shortbow),
                    ItemProfileThin::MakeWeaponSpecific(projectile_type::Longbow),
                    ItemProfileThin::MakeWeaponSpecific(projectile_type::CompositeBow),
                    ItemProfileThin::MakeWeaponSpecific(sword_type::Falcata),
                    ItemProfileThin::MakeWeaponSpecific(sword_type::Rapier),
                    ItemProfileThin::MakeWeaponSpecific(sword_type::Flamberg),
                    ItemProfileThin::MakeWeaponSpecific(axe_type::Sickle),
                    ItemProfileThin::MakeWeaponSpecific(axe_type::Battleaxe),
                    ItemProfileThin::MakeWeaponSpecific(whip_type::Bullwhip),
                    ItemProfileThin::MakeWeaponSpecific(whip_type::Flail),
                    ItemProfileThin::MakeWeaponSpecific(whip_type::MaceAndChain),
                    ItemProfileThin::MakeWeaponSpecific(club_type::Spiked),
                    ItemProfileThin::MakeWeaponSpecific(club_type::Maul),
                    ItemProfileThin::MakeWeaponSpecific(bladedstaff_type::Spear),
                    ItemProfileThin::MakeWeaponSpecific(bladedstaff_type::Pike),
                    ItemProfileThin::MakeWeaponSpecific(bladedstaff_type::Scythe)
                };

                misc::Vector::AppendMove(
                    ItemProfileThin::MakeWeaponKnifeOrDaggerAll(false), thinProfiles);

                misc::Vector::AppendMove(
                    ItemProfileThin::MakeWeaponKnifeOrDaggerAll(true), thinProfiles);

                return thinProfiles;
            }

            case named_type::Betrayer:
            {
                ItemProfileThinVec_t thinProfiles{
                    ItemProfileThin::MakeWeaponSpecific(projectile_type::Longbow),
                    ItemProfileThin::MakeWeaponSpecific(projectile_type::CompositeBow),
                    ItemProfileThin::MakeWeaponSpecific(sword_type::Falcata),
                    ItemProfileThin::MakeWeaponSpecific(sword_type::Flamberg),
                    ItemProfileThin::MakeWeaponSpecific(axe_type::Sickle),
                    ItemProfileThin::MakeWeaponSpecific(whip_type::MaceAndChain),
                    ItemProfileThin::MakeWeaponSpecific(club_type::Spiked),
                    ItemProfileThin::MakeWeaponSpecific(bladedstaff_type::Scythe)
                };

                misc::Vector::AppendMove(
                    ItemProfileThin::MakeWeaponKnifeOrDaggerAll(true), thinProfiles);

                return thinProfiles;
            }

            case named_type::Robbers:
            case named_type::Thugs:
            case named_type::Knaves:
            {
                return { ItemProfileThin::MakeWeaponKnifeOrDagger(false, sfml_util::Size::Large) };
            }

            case named_type::Thief:
            case named_type::Muggers:
            case named_type::Pirate:
            {
                return { ItemProfileThin::MakeWeaponKnifeOrDagger(true, sfml_util::Size::Large) };
            }

            case named_type::Focus:
            {
                ItemProfileThinVec_t thinProfiles{ ItemProfileThin::MakeWeaponOfTypeAll(
                    weapon_type::Projectile) };

                removeIf(thinProfiles, [](auto const & PROFILE) {
                    return PROFILE.WeaponInfo().ProjectileType() == projectile_type::Blowpipe;
                });

                return thinProfiles;
            }

            case named_type::Proud:
            case named_type::Glory:
            case named_type::Pure:
            {
                ItemProfileThinVec_t thinProfiles{
                    ItemProfileThin::MakeWeaponSpecific(projectile_type::Longbow),
                    ItemProfileThin::MakeWeaponSpecific(projectile_type::Crossbow),
                    ItemProfileThin::MakeWeaponSpecific(projectile_type::CompositeBow),
                    ItemProfileThin::MakeWeaponSpecific(sword_type::Broadsword),
                    ItemProfileThin::MakeWeaponSpecific(sword_type::Longsword),
                    ItemProfileThin::MakeWeaponSpecific(sword_type::Knightlysword),
                    ItemProfileThin::MakeWeaponSpecific(sword_type::Claymore),
                    ItemProfileThin::MakeWeaponSpecific(axe_type::Battleaxe),
                    ItemProfileThin::MakeWeaponSpecific(axe_type::Waraxe),
                    ItemProfileThin::MakeWeaponSpecific(whip_type::MaceAndChain),
                    ItemProfileThin::MakeWeaponSpecific(club_type::Warhammer)
                };

                misc::Vector::AppendMove(
                    ItemProfileThin::MakeWeaponOfTypeAll(weapon_type::Staff), thinProfiles);

                misc::Vector::AppendMove(
                    ItemProfileThin::MakeWeaponOfTypeAll(weapon_type::BladedStaff), thinProfiles);

                removeIf(thinProfiles, [](auto const & PROFILE) {
                    return PROFILE.WeaponInfo().Type() & weapon_type::Spear;
                });

                return thinProfiles;
            }

            case named_type::Gloom:
            case named_type::Twilight:
            case named_type::Dusk:
            {
                ItemProfileThinVec_t thinProfiles{
                    ItemProfileThin::MakeWeaponKnifeOrDagger(true, sfml_util::Size::Large),
                    ItemProfileThin::MakeWeaponKnifeOrDagger(false, sfml_util::Size::Large),
                    ItemProfileThin::MakeWeaponSpecific(sword_type::Cutlass),
                    ItemProfileThin::MakeWeaponSpecific(sword_type::Falcata),
                    ItemProfileThin::MakeWeaponSpecific(sword_type::Rapier),
                    ItemProfileThin::MakeWeaponSpecific(sword_type::Saber),
                    ItemProfileThin::MakeWeaponSpecific(sword_type::Flamberg),
                    ItemProfileThin::MakeWeaponSpecific(axe_type::Handaxe),
                    ItemProfileThin::MakeWeaponSpecific(axe_type::Sickle),
                    ItemProfileThin::MakeWeaponSpecific(axe_type::Battleaxe),
                    ItemProfileThin::MakeWeaponSpecific(axe_type::Waraxe),
                    ItemProfileThin::MakeWeaponSpecific(club_type::Spiked),
                    ItemProfileThin::MakeWeaponSpecific(club_type::Maul),
                    ItemProfileThin::MakeWeaponSpecific(bladedstaff_type::ShortSpear),
                    ItemProfileThin::MakeWeaponSpecific(bladedstaff_type::Spear),
                    ItemProfileThin::MakeWeaponSpecific(bladedstaff_type::Pike),
                    ItemProfileThin::MakeWeaponSpecific(bladedstaff_type::Scythe)
                };

                misc::Vector::AppendMove(
                    ItemProfileThin::MakeWeaponOfTypeAll(weapon_type::Projectile), thinProfiles);

                misc::Vector::AppendMove(
                    ItemProfileThin::MakeWeaponOfTypeAll(weapon_type::Whip), thinProfiles);

                return thinProfiles;
            }

            case named_type::Honest:
            {
                ItemProfileThinVec_t thinProfiles{ ItemProfileThin::MakeArmorNonSpecificAll() };

                removeIf(thinProfiles, [](auto const & PROFILE) {
                    return (PROFILE.ArmorInfo().CoverType() == cover_type::Cloak)
                        || (PROFILE.ArmorInfo().CoverType() == cover_type::Robe);
                });

                return thinProfiles;
            }

            case named_type::Noble:
            {
                ItemProfileThinVec_t thinProfiles{ ItemProfileThin::MakeArmorNonSpecificAll() };

                removeIf(thinProfiles, [](auto const & PROFILE) {
                    return (PROFILE.ArmorInfo().CoverType() == cover_type::Cloak)
                        || (PROFILE.ArmorInfo().CoverType() == cover_type::Robe)
                        || (PROFILE.ArmorInfo().BaseType() == base_type::Plain);
                });

                return thinProfiles;
            }

            case named_type::Daring:
            {
                ItemProfileThinVec_t thinProfiles{ ItemProfileThin::MakeArmorNonSpecificAll() };

                removeIf(thinProfiles, [](auto const & PROFILE) {
                    return (PROFILE.ArmorInfo().CoverType() == cover_type::Cloak)
                        || (PROFILE.ArmorInfo().CoverType() == cover_type::Robe)
                        || (PROFILE.ArmorInfo().BaseType() == base_type::Plain)
                        || (PROFILE.ArmorInfo().ShieldType() == shield_type::Buckler)
                        || (PROFILE.ArmorInfo().HelmType() == helm_type::Leather);
                });

                return thinProfiles;
            }

            case named_type::Elite:
            {
                ItemProfileThinVec_t thinProfiles{ ItemProfileThin::MakeArmorNonSpecificAll() };

                removeIf(thinProfiles, [](auto const & PROFILE) {
                    return (PROFILE.ArmorInfo().CoverType() == cover_type::Cloak)
                        || (PROFILE.ArmorInfo().CoverType() == cover_type::Robe)
                        || (PROFILE.ArmorInfo().BaseType() == base_type::Plain)
                        || (PROFILE.ArmorInfo().BaseType() == base_type::Scale)
                        || (PROFILE.ArmorInfo().ShieldType() == shield_type::Buckler)
                        || (PROFILE.ArmorInfo().HelmType() == helm_type::Leather)
                        || (PROFILE.ArmorInfo().HelmType() == helm_type::MailCoif)
                        || (PROFILE.ArmorInfo().HelmType() == helm_type::Kettle);
                });

                return thinProfiles;
            }

            case named_type::Valiant:
            {
                ItemProfileThinVec_t thinProfiles{
                    ItemProfileThin::MakeArmorSpecific(shield_type::Heater),
                    ItemProfileThin::MakeArmorSpecific(shield_type::Pavis),
                    ItemProfileThin::MakeArmorSpecific(helm_type::Bascinet),
                    ItemProfileThin::MakeArmorSpecific(helm_type::Great),
                    ItemProfileThin::MakeArmorSpecific(cover_type::Cape),
                    ItemProfileThin::MakeArmorSpecific(cover_type::Vest)
                };

                misc::Vector::AppendMove(
                    ItemProfileThin::MakeArmorNonSpecificAll(base_type::Plate), thinProfiles);

                misc::Vector::AppendMove(
                    ItemProfileThin::MakeArmorNonSpecificAll(base_type::Mail), thinProfiles);

                return thinProfiles;
            }

            case named_type::Heros:
            {
                ItemProfileThinVec_t thinProfiles{
                    ItemProfileThin::MakeArmorSpecific(shield_type::Pavis),
                    ItemProfileThin::MakeArmorSpecific(helm_type::Great),
                    ItemProfileThin::MakeArmorSpecific(cover_type::Cape),
                    ItemProfileThin::MakeArmorSpecific(cover_type::Vest)
                };

                misc::Vector::AppendMove(
                    ItemProfileThin::MakeArmorNonSpecificAll(base_type::Plate), thinProfiles);

                removeIf(thinProfiles, [](auto const & PROFILE) {
                    return (
                        (PROFILE.ArmorInfo().Type() == armor_type::Bracers)
                        || (PROFILE.ArmorInfo().Type() == armor_type::Aventail));
                });

                return thinProfiles;
            }

            case named_type::Army:
            {
                ItemProfileThinVec_t thinProfiles{};

                misc::Vector::AppendMove(
                    ItemProfileThin::MakeArmorSpecificAll<shield_type>(), thinProfiles);

                misc::Vector::AppendMove(
                    ItemProfileThin::MakeArmorSpecificAll<helm_type>(), thinProfiles);

                misc::Vector::AppendMove(
                    ItemProfileThin::MakeArmorNonSpecificAll(armor_type::Pants), thinProfiles);

                misc::Vector::AppendMove(
                    ItemProfileThin::MakeArmorNonSpecificAll(armor_type::Boots), thinProfiles);

                misc::Vector::AppendMove(
                    ItemProfileThin::MakeArmorNonSpecificAll(armor_type::Shirt), thinProfiles);

                removeIf(thinProfiles, [](auto const & PROFILE) {
                    return (
                        (PROFILE.ArmorInfo().BaseType() == base_type::Plate)
                        || (PROFILE.ArmorInfo().HelmType() == helm_type::Leather)
                        || (PROFILE.ArmorInfo().HelmType() == helm_type::MailCoif)
                        || (PROFILE.ArmorInfo().ShieldType() == shield_type::Pavis));
                });

                return thinProfiles;
            }

            case named_type::Gladiator:
            {
                return { ItemProfileThin::MakeArmorSpecific(shield_type::Buckler),
                         ItemProfileThin::MakeArmorSpecific(shield_type::Kite),
                         ItemProfileThin::MakeArmorSpecific(helm_type::Great),
                         ItemProfileThin::MakeArmorSpecific(helm_type::Bascinet),
                         ItemProfileThin::MakeArmorSpecific(helm_type::Kettle),
                         ItemProfileThin::MakeWeaponSpecific(axe_type::Handaxe),
                         ItemProfileThin::MakeWeaponSpecific(bladedstaff_type::ShortSpear),
                         ItemProfileThin::MakeWeaponSpecific(bladedstaff_type::Spear),
                         ItemProfileThin::MakeWeaponSpecific(sword_type::Gladius),
                         ItemProfileThin::MakeWeaponSpecific(sword_type::Shortsword),
                         ItemProfileThin::MakeWeaponSpecific(sword_type::Broadsword) };
            }

            case named_type::Charred:
            {
                ItemProfileThinVec_t thinProfiles{
                    ItemProfileThin::MakeArmorSpecific(cover_type::Vest),
                };

                misc::Vector::AppendMove(
                    ItemProfileThin::MakeArmorSpecificAll<shield_type>(), thinProfiles);

                misc::Vector::AppendMove(
                    ItemProfileThin::MakeArmorSpecificAll<helm_type>(), thinProfiles);

                misc::Vector::AppendMove(
                    ItemProfileThin::MakeArmorNonSpecificAll(armor_type::Gauntlets), thinProfiles);

                misc::Vector::AppendMove(
                    ItemProfileThin::MakeArmorNonSpecificAll(armor_type::Shirt), thinProfiles);

                misc::Vector::AppendMove(
                    ItemProfileThin::MakeArmorNonSpecificAll(armor_type::Bracers), thinProfiles);

                removeIf(thinProfiles, [](auto const & PROFILE) {
                    return (PROFILE.ArmorInfo().BaseType() != base_type::Plain);
                });

                return thinProfiles;
            }

            case named_type::Moon:
            {
                ItemProfileThinVec_t thinProfiles{
                    ItemProfileThin::MakeArmorSpecific(cover_type::Robe),
                    ItemProfileThin::MakeArmorSpecific(cover_type::Cloak),
                    ItemProfileThin::MakeArmorSpecific(cover_type::Vest)
                };

                misc::Vector::AppendMove(
                    ItemProfileThin::MakeArmorSpecificAll<shield_type>(), thinProfiles);

                misc::Vector::AppendMove(
                    ItemProfileThin::MakeArmorSpecificAll<helm_type>(), thinProfiles);

                misc::Vector::AppendMove(
                    ItemProfileThin::MakeArmorNonSpecificAll(armor_type::Pants), thinProfiles);

                misc::Vector::AppendMove(
                    ItemProfileThin::MakeArmorNonSpecificAll(armor_type::Shirt), thinProfiles);

                misc::Vector::AppendMove(
                    ItemProfileThin::MakeArmorNonSpecificAll(armor_type::Boots), thinProfiles);

                removeIf(thinProfiles, [](auto const & PROFILE) {
                    return (
                        (PROFILE.ArmorInfo().BaseType() == base_type::Plain)
                        || (PROFILE.ArmorInfo().HelmType() == helm_type::Kettle)
                        || (PROFILE.ArmorInfo().HelmType() == helm_type::Leather));
                });

                return thinProfiles;
            }

            case named_type::Imposters:
            case named_type::Pickpocket:
            {
                return { ItemProfileThin::MakeArmorSpecific(cover_type::Robe) };
            }

            case named_type::Chill:
            case named_type::Frozen:
            case named_type::Arctic:
            {
                ItemProfileThinVec_t thinProfiles{ ItemProfileThin::MakeArmorNonSpecificAll() };

                misc::Vector::AppendMove(
                    ItemProfileThin::MakeArmorSpecificAll<cover_type>(), thinProfiles);

                removeIf(thinProfiles, [](auto const & PROFILE) {
                    return (
                        (PROFILE.ArmorInfo().Type() == armor_type::Pants)
                        || (PROFILE.ArmorInfo().Type() == armor_type::Boots)
                        || (PROFILE.ArmorInfo().Type() == armor_type::Shirt)
                        || (PROFILE.ArmorInfo().Type() == armor_type::Aventail)
                        || (PROFILE.ArmorInfo().HelmType() == helm_type::MailCoif)
                        || (PROFILE.ArmorInfo().HelmType() == helm_type::Leather)
                        || (PROFILE.ArmorInfo().CoverType() == cover_type::Cape)
                        || (PROFILE.ArmorInfo().ShieldType() == shield_type::Buckler));
                });

                return thinProfiles;
            }

            case named_type::Light:
            case named_type::Dawn:
            case named_type::Sun:
            {
                ItemProfileThinVec_t thinProfiles{
                    ItemProfileThin::MakeArmorSpecific(cover_type::Robe),
                    ItemProfileThin::MakeArmorSpecific(cover_type::Cape),
                    ItemProfileThin::MakeArmorSpecific(cover_type::Vest),
                    ItemProfileThin::MakeArmorSpecific(shield_type::Kite),
                    ItemProfileThin::MakeArmorSpecific(shield_type::Heater),
                    ItemProfileThin::MakeArmorSpecific(shield_type::Pavis),
                    ItemProfileThin::MakeArmorSpecific(helm_type::Archers),
                    ItemProfileThin::MakeArmorSpecific(helm_type::Bascinet),
                    ItemProfileThin::MakeArmorSpecific(helm_type::Great),
                };

                misc::Vector::AppendMove(
                    ItemProfileThin::MakeArmorNonSpecificAll(armor_type::Gauntlets), thinProfiles);

                misc::Vector::AppendMove(
                    ItemProfileThin::MakeArmorNonSpecificAll(armor_type::Pants), thinProfiles);

                misc::Vector::AppendMove(
                    ItemProfileThin::MakeArmorNonSpecificAll(armor_type::Boots), thinProfiles);

                misc::Vector::AppendMove(
                    ItemProfileThin::MakeArmorNonSpecificAll(armor_type::Shirt), thinProfiles);

                removeIf(thinProfiles, [](auto const & PROFILE) {
                    return (
                        (PROFILE.ArmorInfo().BaseType() == base_type::Plain)
                        || (PROFILE.ArmorInfo().BaseType() == base_type::Scale));
                });

                return thinProfiles;
            }

            case named_type::Sorrow:
            case named_type::Woe:
            case named_type::Misery:
            {
                ItemProfileThinVec_t thinProfiles{
                    ItemProfileThin::MakeArmorSpecific(cover_type::Cloak),
                    ItemProfileThin::MakeArmorSpecific(cover_type::Vest),
                    ItemProfileThin::MakeArmorSpecific(shield_type::Buckler),
                    ItemProfileThin::MakeArmorSpecific(shield_type::Kite),
                    ItemProfileThin::MakeArmorSpecific(shield_type::Heater),
                    ItemProfileThin::MakeArmorSpecific(helm_type::Leather),
                    ItemProfileThin::MakeArmorSpecific(helm_type::MailCoif),
                    ItemProfileThin::MakeArmorSpecific(helm_type::Archers),
                    ItemProfileThin::MakeArmorSpecific(helm_type::Bascinet),
                };

                misc::Vector::AppendMove(
                    ItemProfileThin::MakeArmorNonSpecificAll(armor_type::Gauntlets), thinProfiles);

                misc::Vector::AppendMove(
                    ItemProfileThin::MakeArmorNonSpecificAll(armor_type::Bracers), thinProfiles);

                removeIf(thinProfiles, [](auto const & PROFILE) {
                    return (PROFILE.ArmorInfo().BaseType() == base_type::Plain);
                });

                return thinProfiles;
            }

            case named_type::Charlatans:
            {
                return { ItemProfileThin::MakeArmorSpecific(cover_type::Vest) };
            }

            case named_type::Burglar:
            case named_type::Mountebank:
            {
                return { ItemProfileThin::MakeArmorSpecific(cover_type::Cloak) };
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
                return { ItemProfileThin::MakeArmorNonSpecific(
                             armor_type::Boots, armor::base_type::Plain),
                         ItemProfileThin::MakeArmorNonSpecific(
                             armor_type::Pants, armor::base_type::Plain),
                         ItemProfileThin::MakeArmorNonSpecific(
                             armor_type::Shirt, armor::base_type::Plain),
                         ItemProfileThin::MakeArmorNonSpecific(
                             armor_type::Gauntlets, armor::base_type::Plain),
                         ItemProfileThin::MakeArmorSpecific(armor::cover_type::Cloak),
                         ItemProfileThin::MakeWeaponKnifeOrDagger(true),
                         ItemProfileThin::MakeMisc(misc_type::LockPicks) };
            }

            case set_type::TheMagus:
            case set_type::TheNecromancers:
            case set_type::TheWarlocks:
            {
                return { ItemProfileThin::MakeArmorNonSpecific(
                             armor_type::Boots, armor::base_type::Plain),
                         ItemProfileThin::MakeArmorNonSpecific(
                             armor_type::Pants, armor::base_type::Plain),
                         ItemProfileThin::MakeArmorNonSpecific(
                             armor_type::Shirt, armor::base_type::Plain),
                         ItemProfileThin::MakeArmorNonSpecific(
                             armor_type::Gauntlets, armor::base_type::Plain),
                         ItemProfileThin::MakeArmorSpecific(armor::cover_type::Robe),
                         ItemProfileThin::MakeMisc(misc_type::Wand) };
            }

            case set_type::TheLichKings:
            {
                return { ItemProfileThin::MakeArmorNonSpecific(
                             armor_type::Boots, armor::base_type::Plain),
                         ItemProfileThin::MakeArmorNonSpecific(
                             armor_type::Pants, armor::base_type::Plain),
                         ItemProfileThin::MakeArmorNonSpecific(
                             armor_type::Shirt, armor::base_type::Plain),
                         ItemProfileThin::MakeArmorNonSpecific(
                             armor_type::Gauntlets, armor::base_type::Plain),
                         ItemProfileThin::MakeArmorSpecific(armor::cover_type::Robe),
                         ItemProfileThin::MakeMisc(misc_type::Litch_Hand) };
            }

            case set_type::TheSages:
            case set_type::TheShamans:
            case set_type::TheOracles:
            {
                return { ItemProfileThin::MakeArmorNonSpecific(
                             armor_type::Boots, armor::base_type::Plain),
                         ItemProfileThin::MakeArmorNonSpecific(
                             armor_type::Pants, armor::base_type::Plain),
                         ItemProfileThin::MakeArmorNonSpecific(
                             armor_type::Shirt, armor::base_type::Plain),
                         ItemProfileThin::MakeArmorNonSpecific(
                             armor_type::Gauntlets, armor::base_type::Plain),
                         ItemProfileThin::MakeArmorSpecific(armor::cover_type::Robe),
                         ItemProfileThin::MakeMisc(misc_type::Wand) };
            }

            case set_type::TheAngelic:
            {
                return { ItemProfileThin::MakeArmorNonSpecific(
                             armor_type::Boots, armor::base_type::Plain),
                         ItemProfileThin::MakeArmorNonSpecific(
                             armor_type::Pants, armor::base_type::Plain),
                         ItemProfileThin::MakeArmorNonSpecific(
                             armor_type::Shirt, armor::base_type::Plain),
                         ItemProfileThin::MakeArmorNonSpecific(
                             armor_type::Gauntlets, armor::base_type::Plain),
                         ItemProfileThin::MakeArmorSpecific(armor::cover_type::Robe),
                         ItemProfileThin::MakeMisc(misc_type::Angel_Braid) };
            }

            case set_type::TheBalladeers:
            case set_type::TheTroubadours:
            case set_type::TheMuses:
            {
                return { ItemProfileThin::MakeArmorNonSpecific(
                             armor_type::Boots, armor::base_type::Mail),
                         ItemProfileThin::MakeArmorNonSpecific(
                             armor_type::Pants, armor::base_type::Mail),
                         ItemProfileThin::MakeArmorNonSpecific(
                             armor_type::Shirt, armor::base_type::Mail),
                         ItemProfileThin::MakeArmorNonSpecific(
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
                return { ItemProfileThin::MakeArmorNonSpecific(
                             armor_type::Boots, armor::base_type::Plate),
                         ItemProfileThin::MakeArmorNonSpecific(
                             armor_type::Pants, armor::base_type::Plate),
                         ItemProfileThin::MakeArmorNonSpecific(
                             armor_type::Shirt, armor::base_type::Plate),
                         ItemProfileThin::MakeArmorNonSpecific(
                             armor_type::Gauntlets, armor::base_type::Plate),
                         ItemProfileThin::MakeArmorNonSpecific(
                             armor_type::Bracers, armor::base_type::Plate),
                         ItemProfileThin::MakeArmorSpecific(armor::cover_type::Cape),
                         ItemProfileThin::MakeArmorSpecific(armor::shield_type::Pavis),
                         ItemProfileThin::MakeArmorSpecific(armor::helm_type::Great),
                         ItemProfileThin::MakeArmorNonSpecific(
                             armor_type::Aventail, armor::base_type::Plate) };
            }

            case set_type::TheHunters:
            case set_type::TheSureShot:
            case set_type::TheMarksmans:
            case set_type::TheDeadeye:
            case set_type::TheDuskRanger:
            case set_type::TheVenomBow:
            {
                return { ItemProfileThin::MakeArmorNonSpecific(
                             armor_type::Boots, armor::base_type::Mail),
                         ItemProfileThin::MakeArmorNonSpecific(
                             armor_type::Pants, armor::base_type::Mail),
                         ItemProfileThin::MakeArmorNonSpecific(
                             armor_type::Shirt, armor::base_type::Mail),
                         ItemProfileThin::MakeArmorNonSpecific(
                             armor_type::Gauntlets, armor::base_type::Plain),
                         ItemProfileThin::MakeArmorNonSpecific(
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
                return { ItemProfileThin::MakeArmorNonSpecific(
                             armor_type::Boots, armor::base_type::Scale),
                         ItemProfileThin::MakeArmorNonSpecific(
                             armor_type::Pants, armor::base_type::Scale),
                         ItemProfileThin::MakeArmorNonSpecific(
                             armor_type::Shirt, armor::base_type::Scale),
                         ItemProfileThin::MakeArmorNonSpecific(
                             armor_type::Bracers, armor::base_type::Scale),
                         ItemProfileThin::MakeArmorNonSpecific(
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
