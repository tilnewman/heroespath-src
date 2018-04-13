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
// item-factory.cpp
//
#include "item-factory.hpp"

#include "game/loop-manager.hpp"
#include "item/armor-factory.hpp"
#include "item/item-profile-warehouse.hpp"
#include "item/item-profile.hpp"
#include "item/item-warehouse.hpp"
#include "item/item.hpp"
#include "item/misc-item-factory.hpp"
#include "item/weapon-factory.hpp"
#include "log/log-macros.hpp"
#include "misc/assertlogandthrow.hpp"

#include <algorithm>
#include <exception>
#include <sstream>

namespace heroespath
{
namespace item
{

    bool ItemFactory::Test()
    {
        static auto didPostInitial{ false };
        if (false == didPostInitial)
        {
            didPostInitial = true;

            game::LoopManager::Instance()->TestingStrAppend(
                "item::ItemFactory::Test() Starting Tests...");

            game::LoopManager::Instance()->TestingStrAppend(
                "item::ItemFactory::Test() Starting Named Equipment Tests.  Please wait...");

            return false;
        }

        static auto allItemProfilesVec{ ItemProfileWarehouse::Instance()->GetNormalProfiles() };

        if (allItemProfilesVec.size()
            == ItemProfileWarehouse::Instance()->GetNormalProfiles().size())
        {
            for (auto const & PROFILE : ItemProfileWarehouse::Instance()->GetReligiousProfiles())
            {
                allItemProfilesVec.emplace_back(PROFILE);
            }
        }

        static auto hasTestedForDuplicates{ false };
        if (false == hasTestedForDuplicates)
        {
            game::LoopManager::Instance()->TestingStrAppend(
                "item::ItemFactory::Test() Starting Duplicate Test.  Please wait...");

            std::sort(std::begin(allItemProfilesVec), std::end(allItemProfilesVec));

            auto const UNIQUE_ITER{ std::unique(
                std::begin(allItemProfilesVec), std::end(allItemProfilesVec)) };

            if (UNIQUE_ITER != std::end(allItemProfilesVec))
            {
                std::ostringstream ss;
                ss << "item::ItemFactory::Test() failed duplicate item profiles test:  Duplicate "
                      "item profiles:  ";

                for (auto iter(UNIQUE_ITER); iter != std::end(allItemProfilesVec); ++iter)
                {
                    ss << "{" << iter->ToString() << "}   ";
                }
                throw std::runtime_error(ss.str());
            }

            hasTestedForDuplicates = true;
            return false;
        }

        static auto hasTestedForInvalid{ false };
        if (false == hasTestedForInvalid)
        {
            game::LoopManager::Instance()->TestingStrAppend(
                "item::ItemFactory::Test() Starting Invalid Test.  Please wait...");

            ItemProfileVec_t invalidProfiles;
            for (auto const & PROFILE : allItemProfilesVec)
            {
                if (PROFILE.IsValid() == false)
                {
                    invalidProfiles.emplace_back(PROFILE);
                }
            }

            if (invalidProfiles.empty() == false)
            {
                std::ostringstream ss;
                ss << "item::ItemFactory::Test() failed the invalid test.  Invalid Profiles:  ";
                for (auto const & PROFILE : invalidProfiles)
                {
                    ss << "{" << PROFILE.ToString() << "}   ";
                }
                throw std::runtime_error(ss.str());
            }

            hasTestedForInvalid = true;
            return false;
        }

        static auto hasTestedMakingItems_NamedEquipment{ false };
        if (false == hasTestedMakingItems_NamedEquipment)
        {
            auto NAMED_TYPE_END_ITER{ std::partition(
                allItemProfilesVec.begin(), allItemProfilesVec.end(), [](const ItemProfile & P) {
                    return (
                        (P.NamedType() != item::named_type::Count)
                        && (P.NamedType() != item::named_type::NotNamed));
                }) };

            M_ASSERT_OR_LOGANDTHROW_SS(
                (NAMED_TYPE_END_ITER != allItemProfilesVec.begin()),
                "item::ItemFactory::Test() found no named_type ItemProfiles.");

            for (auto iter{ allItemProfilesVec.begin() }; iter != NAMED_TYPE_END_ITER; ++iter)
            {
                auto itemPtr{ Make(*iter) };
                TestItem("named_type", itemPtr, *iter);
                ItemWarehouse::Access().Free(itemPtr);
            }

            game::LoopManager::Instance()->TestingStrAppend(
                std::string("item::ItemFactory::Test() All named_type tests PASSED.")
                + "  Starting Unique Item Tests.  Please wait...");

            allItemProfilesVec.erase(allItemProfilesVec.begin(), NAMED_TYPE_END_ITER);
            hasTestedMakingItems_NamedEquipment = true;
            return false;
        }

        static auto hasTestedMakingItems_Unique{ false };
        if (false == hasTestedMakingItems_Unique)
        {
            auto UNIQUE_TYPE_END_ITER{ std::partition(
                allItemProfilesVec.begin(), allItemProfilesVec.end(), [](const ItemProfile & P) {
                    return (
                        (P.UniqueType() != item::unique_type::Count)
                        && (P.UniqueType() != item::unique_type::NotUnique));
                }) };

            M_ASSERT_OR_LOGANDTHROW_SS(
                (UNIQUE_TYPE_END_ITER != allItemProfilesVec.begin()),
                "item::ItemFactory::Test() found no unique_type ItemProfiles.");

            for (auto iter{ allItemProfilesVec.begin() }; iter != UNIQUE_TYPE_END_ITER; ++iter)
            {
                auto itemPtr{ Make(*iter) };
                TestItem("unique_type", itemPtr, *iter);
                ItemWarehouse::Access().Free(itemPtr);
            }

            game::LoopManager::Instance()->TestingStrAppend(
                std::string("item::ItemFactory::Test() All unique_type tests PASSED.")
                + "  Starting Set Item Tests.  Please wait...");

            allItemProfilesVec.erase(allItemProfilesVec.begin(), UNIQUE_TYPE_END_ITER);
            hasTestedMakingItems_Unique = true;
            return false;
        }

        static auto hasTestedMakingItems_Set{ false };
        if (false == hasTestedMakingItems_Set)
        {
            auto SET_TYPE_END_ITER{ std::partition(
                allItemProfilesVec.begin(), allItemProfilesVec.end(), [](const ItemProfile & P) {
                    return (
                        (P.SetType() != item::set_type::Count)
                        && (P.SetType() != item::set_type::NotASet));
                }) };

            M_ASSERT_OR_LOGANDTHROW_SS(
                (SET_TYPE_END_ITER != allItemProfilesVec.begin()),
                "item::ItemFactory::Test() found no unique_type ItemProfiles.");

            for (auto iter{ allItemProfilesVec.begin() }; iter != SET_TYPE_END_ITER; ++iter)
            {
                auto itemPtr{ Make(*iter) };
                TestItem("set_type", itemPtr, *iter);
                ItemWarehouse::Access().Free(itemPtr);
            }

            game::LoopManager::Instance()->TestingStrAppend(
                std::string("item::ItemFactory::Test() All set_type tests PASSED.")
                + "  Starting Misc Item Tests.  Please wait...");

            allItemProfilesVec.erase(allItemProfilesVec.begin(), SET_TYPE_END_ITER);
            hasTestedMakingItems_Set = true;
            return false;
        }

        static auto hasTestedMakingItems_Misc{ false };
        if (false == hasTestedMakingItems_Misc)
        {
            auto MISC_TYPE_END_ITER{ std::partition(
                allItemProfilesVec.begin(), allItemProfilesVec.end(), [](const ItemProfile & P) {
                    return (
                        (P.MiscType() != item::misc_type::Count)
                        && (P.MiscType() != item::misc_type::NotMisc));
                }) };

            M_ASSERT_OR_LOGANDTHROW_SS(
                (MISC_TYPE_END_ITER != allItemProfilesVec.begin()),
                "item::ItemFactory::Test() found no misc_type ItemProfiles.");

            for (auto iter{ allItemProfilesVec.begin() }; iter != MISC_TYPE_END_ITER; ++iter)
            {
                auto itemPtr{ Make(*iter) };
                TestItem("misc_type", itemPtr, *iter);
                ItemWarehouse::Access().Free(itemPtr);
            }

            game::LoopManager::Instance()->TestingStrAppend(
                std::string("item::ItemFactory::Test() All stand-alone misc_type ")
                + "tests PASSED.  Starting Equipment with Element Type Tests. Please wait...");

            allItemProfilesVec.erase(allItemProfilesVec.begin(), MISC_TYPE_END_ITER);
            hasTestedMakingItems_Misc = true;
            return false;
        }

        static auto hasTestedMakingItems_ElementEquipment{ false };
        if (false == hasTestedMakingItems_ElementEquipment)
        {
            auto ELEMENT_TYPE_END_ITER{ std::partition(
                allItemProfilesVec.begin(), allItemProfilesVec.end(), [](const ItemProfile & P) {
                    return (P.ElementType() != item::element_type::None);
                }) };

            M_ASSERT_OR_LOGANDTHROW_SS(
                (allItemProfilesVec.begin() != ELEMENT_TYPE_END_ITER),
                "item::ItemFactory::Test() found no element_type ItemProfiles.");

            for (auto iter{ allItemProfilesVec.begin() }; iter != ELEMENT_TYPE_END_ITER; ++iter)
            {
                auto itemPtr{ Make(*iter) };

                M_ASSERT_OR_LOGANDTHROW_SS(
                    (itemPtr->IsWeapon() || itemPtr->IsArmor()),
                    "item::ItemFactory::Test() found an element_type that was not equipment:"
                        << " Item={" << itemPtr->ToString() << "}, ItemProfile=["
                        << iter->ToString()
                        << "], element_type=" << element_type::ToString(itemPtr->ElementType()));

                TestItem("elemet_type", itemPtr, *iter);
                ItemWarehouse::Access().Free(itemPtr);
            }

            game::LoopManager::Instance()->TestingStrAppend(
                std::string("item::ItemFactory::Test() All remaining element_type ")
                + "equipment tests PASSED.  Starting Standard Equipment Tests.  Please wait...");

            allItemProfilesVec.erase(allItemProfilesVec.begin(), ELEMENT_TYPE_END_ITER);
            hasTestedMakingItems_ElementEquipment = true;
            return false;
        }

        static auto hasTestedMakingItems_StandardEquipment{ false };
        if (false == hasTestedMakingItems_StandardEquipment)
        {
            for (auto iter{ allItemProfilesVec.begin() }; iter != allItemProfilesVec.end(); ++iter)
            {
                auto itemPtr{ Make(*iter) };

                M_ASSERT_OR_LOGANDTHROW_SS(
                    (itemPtr->IsWeapon() || itemPtr->IsArmor()),
                    "item::ItemFactory::Test() found standard equipment that was "
                        << "not weapon or armor:"
                        << " Item={" << itemPtr->ToString() << "}, ItemProfile=["
                        << iter->ToString()
                        << "], item_category=" << category::ToString(itemPtr->Category(), true));

                M_ASSERT_OR_LOGANDTHROW_SS(
                    (((itemPtr->SetType() == set_type::Count)
                      || (itemPtr->SetType() == set_type::NotASet))
                     && ((itemPtr->UniqueType() == unique_type::Count)
                         || (itemPtr->UniqueType() == unique_type::NotUnique))
                     && ((itemPtr->NamedType() == named_type::Count)
                         || (itemPtr->NamedType() == named_type::NotNamed))
                     && (itemPtr->ElementType() == element_type::None)),
                    "item::ItemFactory::Test() found standard equipment that was "
                        << "not standard:"
                        << " Item={" << itemPtr->ToString() << "}, ItemProfile=["
                        << iter->ToString());

                TestItem("standard_equipment", itemPtr, *iter);
                ItemWarehouse::Access().Free(itemPtr);
            }

            game::LoopManager::Instance()->TestingStrAppend(
                std::string("item::ItemFactory::Test() All Standard Equipment ")
                + " (All Remaining Profiles) Tests PASSED.");

            allItemProfilesVec.clear();
            hasTestedMakingItems_StandardEquipment = true;
            return false;
        }

        M_ASSERT_OR_LOGANDTHROW_SS(
            (allItemProfilesVec.empty()),
            "item::ItemFactory::Test() failed to extract all from allItemProfilesVec.");

        static auto didPostFinal{ false };
        if (false == didPostFinal)
        {
            didPostFinal = true;

            game::LoopManager::Instance()->TestingStrAppend(
                "item::ItemFactory::Test() Testing Complete:  ALL TESTS PASSED");

            return false;
        }

        return true;
    }

    void ItemFactory::TestItem(
        const std::string & WHICH_TEST,
        const ItemPtr_t & ITEM_PTR,
        const ItemProfile & ITEM_PROFILE)
    {
        M_ASSERT_OR_LOGANDTHROW_SS(
            (ITEM_PROFILE.IsValid()),
            "item::ItemFactory::TestItem(which_test="
                << WHICH_TEST << ", item={" << ITEM_PTR->ToString() << "}, profile={"
                << ITEM_PROFILE.ToString() << "}) tried to make an item with an invalid profile: \""
                << ITEM_PROFILE.ToMemberStrings().InvalidString() << "\"");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ITEM_PTR->MaterialPrimary() != ITEM_PTR->MaterialSecondary()),
            "item::ItemFactory::TestItem(which_test="
                << WHICH_TEST << ", item={" << ITEM_PTR->ToString() << "}, profile="
                << ITEM_PROFILE.ToString() << ") made an item with duplicate materials.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ITEM_PTR->Category() == ITEM_PROFILE.Category()),
            "item::ItemFactory::TestItem(which_test="
                << WHICH_TEST << ", item={" << ITEM_PTR->ToString() << "}, profile="
                << ITEM_PROFILE.ToString() << ") made an item where the categories did not match.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ITEM_PTR->MiscType() == ITEM_PROFILE.MiscType()),
            "item::ItemFactory::TestItem(which_test="
                << WHICH_TEST << ", item={" << ITEM_PTR->ToString() << "}, profile="
                << ITEM_PROFILE.ToString() << ") made an item where the misc_types did not match.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ITEM_PTR->SetType() == ITEM_PROFILE.SetType()),
            "item::ItemFactory::TestItem(which_test="
                << WHICH_TEST << ", item={" << ITEM_PTR->ToString() << "}, profile="
                << ITEM_PROFILE.ToString() << ") made an item where the set_types did not match.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ITEM_PTR->ElementType() == ITEM_PROFILE.ElementType()),
            "item::ItemFactory::TestItem(which_test="
                << WHICH_TEST << ", item={" << ITEM_PTR->ToString()
                << "}, profile=" << ITEM_PROFILE.ToString()
                << ") made an item where the element_types did not match.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ITEM_PTR->NamedType() == ITEM_PROFILE.NamedType()),
            "item::ItemFactory::TestItem(which_test="
                << WHICH_TEST << ", item={" << ITEM_PTR->ToString()
                << "}, profile=" << ITEM_PROFILE.ToString()
                << ") made an item where the named_types did not match.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ITEM_PTR->UniqueType() == ITEM_PROFILE.UniqueType()),
            "item::ItemFactory::TestItem(which_test="
                << WHICH_TEST << ", item={" << ITEM_PTR->ToString()
                << "}, profile=" << ITEM_PROFILE.ToString()
                << ") made an item where the unique_types did not match.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ITEM_PTR->MaterialPrimary() == ITEM_PROFILE.MaterialPrimary()),
            "item::ItemFactory::TestItem(which_test="
                << WHICH_TEST << ", item={" << ITEM_PTR->ToString()
                << "}, profile=" << ITEM_PROFILE.ToString()
                << ") made an item where the primary materials did not match.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            ((ITEM_PTR->MaterialPrimary() != material::Count)
             && (ITEM_PTR->MaterialPrimary() != material::Nothing)),
            "item::ItemFactory::TestItem(which_test="
                << WHICH_TEST << ", item={" << ITEM_PTR->ToString()
                << "}, profile=" << ITEM_PROFILE.ToString()
                << ") made an item where the primary material was either Count or Nothing.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ITEM_PTR->MaterialSecondary() == ITEM_PROFILE.MaterialSecondary()),
            "item::ItemFactory::TestItem(which_test="
                << WHICH_TEST << ", item={" << ITEM_PTR->ToString()
                << "}, profile=" << ITEM_PROFILE.ToString()
                << ") made an item where the secondary materials did not match.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ITEM_PTR->MaterialSecondary() != material::Count),
            "item::ItemFactory::TestItem(which_test="
                << WHICH_TEST << ", item={" << ITEM_PTR->ToString()
                << "}, profile=" << ITEM_PROFILE.ToString()
                << ") made an item where the secondary material was Count instead of Nothing.");

        if ((ITEM_PTR->IsWeapon()) && (ITEM_PTR->IsArmor()))
        {
            std::ostringstream ss;
            ss << "item::ItemFactory::TestItem(which_test=" << WHICH_TEST << ", item={"
               << ITEM_PTR->ToString() << "}, profile=" << ITEM_PROFILE.ToString()
               << ") item was both weapon and armor";

            throw std::runtime_error(ss.str());
        }

        if ((ITEM_PTR->IsWeapon()) && (ITEM_PTR->WeaponType() == weapon_type::NotAWeapon))
        {
            std::ostringstream ss;
            ss << "item::ItemFactory::TestItem(which_test=" << WHICH_TEST
               << ", item=" << ITEM_PTR->ToString() << "}, profile=" << ITEM_PROFILE.ToString()
               << ") item's category was weapon but weapon_type was NotAWeapon.";

            throw std::runtime_error(ss.str());
        }

        if ((ITEM_PTR->IsWeapon()) && (ITEM_PTR->WeaponType() & weapon_type::Breath))
        {
            std::ostringstream ss;
            ss << "item::ItemFactory::TestItem(which_test=" << WHICH_TEST << ", item={"
               << ITEM_PTR->ToString() << "}, profile=" << ITEM_PROFILE.ToString()
               << ") item's weapon_type was Breath.";

            throw std::runtime_error(ss.str());
        }

        if ((ITEM_PTR->IsArmor()) && (ITEM_PTR->ArmorType() == armor_type::NotArmor))
        {
            std::ostringstream ss;
            ss << "item::ItemFactory::TestItem(which_test=" << WHICH_TEST << ", item={"
               << ITEM_PTR->ToString() << "}, profile=" << ITEM_PROFILE.ToString()
               << ") item's category was armor but armor_type was NotArmor.";

            throw std::runtime_error(ss.str());
        }

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ITEM_PTR->IsBodypart() == false),
            "item::ItemFactory::TestItem(which_test="
                << WHICH_TEST << ", item={" << ITEM_PTR->ToString()
                << "}, profile=" << ITEM_PROFILE.ToString()
                << ") created a bodypart item but there should be no bodypart items.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ITEM_PTR->IsBroken() == false),
            "item::ItemFactory::TestItem(which_test="
                << WHICH_TEST << ", item={" << ITEM_PTR->ToString()
                << "}, profile=" << ITEM_PROFILE.ToString() << ") created a broken item.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ITEM_PTR->Category() != category::None),
            "item::ItemFactory::TestItem(which_test="
                << WHICH_TEST << ", item={" << ITEM_PTR->ToString() << "}, profile="
                << ITEM_PROFILE.ToString() << ") created an item with no/empty category.");

        if (((ITEM_PTR->SetType() != set_type::Count) && (ITEM_PTR->SetType() != set_type::NotASet))
            && (ITEM_PTR->ElementType() != element_type::None))
        {
            std::ostringstream ss;
            ss << "item::ItemFactory::TestItem(which_test=" << WHICH_TEST << ", item={"
               << ITEM_PTR->ToString() << "}, profile=" << ITEM_PROFILE.ToString()
               << ") item is a set_type but also an element_type.";

            throw std::runtime_error(ss.str());
        }

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ITEM_PTR->ImageFilename().empty() == false),
            "item::ItemFactory::TestItem(which_test="
                << WHICH_TEST << ", item={" << ITEM_PTR->ToString() << "}, profile="
                << ITEM_PROFILE.ToString() << ") created an item with no/empty image filename.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (element_type::IsValid(ITEM_PTR->ElementType())),
            "item::ItemFactory::TestItem(which_test="
                << WHICH_TEST << ", item={" << ITEM_PTR->ToString() << "}, profile="
                << ITEM_PROFILE.ToString() << ") created an item with an invalid element_type.");

        if ((ITEM_PTR->Armor_Info().helm == armor::helm_type::Leather)
            && (ITEM_PTR->MaterialPrimary() != material::HardLeather))
        {
            std::ostringstream ss;
            ss << "item::ItemFactory::TestItem(which_test=" << WHICH_TEST << ", item={"
               << ITEM_PTR->ToString() << "}, profile=" << ITEM_PROFILE.ToString()
               << ") created a leather helm with a primary material that was not hard leather.";

            throw std::runtime_error(ss.str());
        }

        if ((ITEM_PTR->Weapon_Info().whip == weapon::whip_type::Bullwhip)
            && (ITEM_PTR->MaterialPrimary() != material::HardLeather))
        {
            std::ostringstream ss;
            ss << "item::ItemFactory::TestItem(which_test=" << WHICH_TEST << ", item={"
               << ITEM_PTR->ToString() << "}, profile=" << ITEM_PROFILE.ToString()
               << ") created a bull whip with a primary material that was not hard leather.";

            throw std::runtime_error(ss.str());
        }
    }

    const ItemPtr_t ItemFactory::Make(const ItemProfile & PROFILE)
    {
        if ((PROFILE.MiscType() != misc_type::Count) && (PROFILE.MiscType() != misc_type::NotMisc))
        {
            return item::MiscItemFactory::Make(PROFILE);
        }
        else if (PROFILE.Category() & item::category::Armor)
        {
            if (PROFILE.IsBoots())
            {
                return armor::ArmorFactory::Instance()->Make_Boots(PROFILE);
            }
            else if (PROFILE.IsBracer())
            {
                return armor::ArmorFactory::Instance()->Make_Bracer(PROFILE);
            }
            else if (PROFILE.IsPants())
            {
                return armor::ArmorFactory::Instance()->Make_Pants(PROFILE);
            }
            else if (PROFILE.IsShirt())
            {
                return armor::ArmorFactory::Instance()->Make_Shirt(PROFILE);
            }
            else if (PROFILE.IsGauntlets())
            {
                return armor::ArmorFactory::Instance()->Make_Gauntlets(PROFILE);
            }
            else if (PROFILE.IsAventail())
            {
                return armor::ArmorFactory::Instance()->Make_Aventail(PROFILE);
            }
            else if (PROFILE.ShieldType() != armor::shield_type::Count)
            {
                return armor::ArmorFactory::Instance()->Make_Shield(PROFILE);
            }
            else if (PROFILE.CoverType() != armor::cover_type::Count)
            {
                return armor::ArmorFactory::Instance()->Make_Cover(PROFILE);
            }
            else if (PROFILE.HelmType() != armor::helm_type::Count)
            {
                return armor::ArmorFactory::Instance()->Make_Helm(PROFILE);
            }
        }
        else if (PROFILE.Category() & item::category::Weapon)
        {
            if (PROFILE.SwordType() != weapon::sword_type::Count)
            {
                return weapon::WeaponFactory::Instance()->Make_Sword(PROFILE);
            }
            else if (PROFILE.AxeType() != weapon::axe_type::Count)
            {
                return weapon::WeaponFactory::Instance()->Make_Axe(PROFILE);
            }
            else if (PROFILE.ClubType() != weapon::club_type::Count)
            {
                return weapon::WeaponFactory::Instance()->Make_Club(PROFILE);
            }
            else if (PROFILE.WhipType() != weapon::whip_type::Count)
            {
                return weapon::WeaponFactory::Instance()->Make_Whip(PROFILE);
            }
            else if (PROFILE.ProjectileType() != weapon::projectile_type::Count)
            {
                return weapon::WeaponFactory::Instance()->Make_Projectile(PROFILE);
            }
            else if (PROFILE.BladedStaffType() != weapon::bladedstaff_type::Count)
            {
                return weapon::WeaponFactory::Instance()->Make_BladedStaff(PROFILE);
            }
            else if (PROFILE.IsKnife())
            {
                return weapon::WeaponFactory::Instance()->Make_Knife(PROFILE);
            }
            else if (PROFILE.IsDagger())
            {
                return weapon::WeaponFactory::Instance()->Make_Knife(PROFILE);
            }
            else if (PROFILE.IsStaff())
            {
                return weapon::WeaponFactory::Instance()->Make_Staff(PROFILE);
            }
            else if (PROFILE.IsQuarterStaff())
            {
                return weapon::WeaponFactory::Instance()->Make_Staff(PROFILE);
            }
        }

        std::ostringstream ss;

        ss << "item::ItemFactory::Make(profile=" << PROFILE.ToString()
           << ") failed to create an item based on that profile.";

        throw std::runtime_error(ss.str());
    }

} // namespace item
} // namespace heroespath
