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
// item-factory.hpp
//
#include "item-factory.hpp"

#include "creature/enchantment-factory.hpp"
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

    std::unique_ptr<ItemFactory> ItemFactory::instanceUPtr_;

    ItemFactory::ItemFactory() { M_HP_LOG_DBG("Singleton Construction: ItemFactory"); }

    ItemFactory::~ItemFactory() { M_HP_LOG_DBG("Singleton Destruction: ItemFactory"); }

    ItemFactory * ItemFactory::Instance()
    {
        if (!instanceUPtr_)
        {
            M_HP_LOG_ERR("Singleton Instance() before Acquire(): ItemFactory");
            Acquire();
        }

        return instanceUPtr_.get();
    }

    void ItemFactory::Acquire()
    {
        if (!instanceUPtr_)
        {
            instanceUPtr_ = std::make_unique<ItemFactory>();
        }
        else
        {
            M_HP_LOG_ERR("Singleton Acquire() after Construction: ItemFactory");
        }
    }

    void ItemFactory::Release()
    {
        M_ASSERT_OR_LOGANDTHROW_SS(
            (instanceUPtr_), "creature::ItemFactory::Release() found instanceUPtr that was null.");

        instanceUPtr_.reset();
    }

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
                auto itemPtrOpt{ Instance()->Make(*iter) };
                TestItem("named_type", itemPtrOpt, *iter);
                ItemWarehouse::Instance()->Free(itemPtrOpt);
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
                auto itemPtrOpt{ Instance()->Make(*iter) };
                TestItem("unique_type", itemPtrOpt, *iter);
                ItemWarehouse::Instance()->Free(itemPtrOpt);
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
                auto itemPtrOpt{ Instance()->Make(*iter) };
                TestItem("set_type", itemPtrOpt, *iter);
                ItemWarehouse::Instance()->Free(itemPtrOpt);
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
                auto itemPtrOpt{ Instance()->Make(*iter) };
                TestItem("misc_type", itemPtrOpt, *iter);
                ItemWarehouse::Instance()->Free(itemPtrOpt);
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
                auto itemPtrOpt{ Instance()->Make(*iter) };

                M_ASSERT_OR_LOGANDTHROW_SS(
                    (itemPtrOpt->IsWeapon() || itemPtrOpt->IsArmor()),
                    "item::ItemFactory::Test() found an element_type that was not equipment:"
                        << " Item=\"" << itemPtrOpt->Name() << "\", ItemProfile=["
                        << iter->ToString()
                        << "], element_type=" << element_type::ToString(itemPtrOpt->ElementType()));

                TestItem("elemet_type", itemPtrOpt, *iter);
                ItemWarehouse::Instance()->Free(itemPtrOpt);
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
                auto itemPtrOpt{ Instance()->Make(*iter) };

                M_ASSERT_OR_LOGANDTHROW_SS(
                    (itemPtrOpt->IsWeapon() || itemPtrOpt->IsArmor()),
                    "item::ItemFactory::Test() found standard equipment that was "
                        << "not weapon or armor:"
                        << " Item=\"" << itemPtrOpt->Name() << "\", ItemProfile=["
                        << iter->ToString()
                        << "], item_category=" << category::ToString(itemPtrOpt->Category(), true));

                M_ASSERT_OR_LOGANDTHROW_SS(
                    (((itemPtrOpt->SetType() == set_type::Count)
                      || (itemPtrOpt->SetType() == set_type::NotASet))
                     && ((itemPtrOpt->UniqueType() == unique_type::Count)
                         || (itemPtrOpt->UniqueType() == unique_type::NotUnique))
                     && ((itemPtrOpt->NamedType() == named_type::Count)
                         || (itemPtrOpt->NamedType() == named_type::NotNamed))
                     && (itemPtrOpt->ElementType() == element_type::None)),
                    "item::ItemFactory::Test() found standard equipment that was "
                        << "not standard:"
                        << " Item=\"" << itemPtrOpt->Name() << "\", ItemProfile=["
                        << iter->ToString());

                TestItem("standard_equipment", itemPtrOpt, *iter);
                ItemWarehouse::Instance()->Free(itemPtrOpt);
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
                << WHICH_TEST << ", item=\"" << ITEM_PTR->Name() << "\", profile="
                << ITEM_PROFILE.ToString() << ") tried to make an item with an invalid profile: \""
                << ITEM_PROFILE.ToMemberStrings().InvalidString() << "\"");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ITEM_PTR->MaterialPrimary() != ITEM_PTR->MaterialSecondary()),
            "item::ItemFactory::TestItem(which_test="
                << WHICH_TEST << ", item=\"" << ITEM_PTR->Name() << "\", profile="
                << ITEM_PROFILE.ToString() << ") made an item with duplicate materials.");

        if ((ITEM_PTR->IsWeapon()) && (ITEM_PTR->IsArmor()))
        {
            std::ostringstream ss;
            ss << "item::ItemFactory::TestItem(which_test=" << WHICH_TEST << ", item=\""
               << ITEM_PTR->Name() << "\", profile=" << ITEM_PROFILE.ToString()
               << ") item was both weapon and armor";

            throw std::runtime_error(ss.str());
        }

        if ((ITEM_PTR->IsWeapon()) && (ITEM_PTR->WeaponType() == weapon_type::NotAWeapon))
        {
            std::ostringstream ss;
            ss << "item::ItemFactory::TestItem(which_test=" << WHICH_TEST << ", item=\""
               << ITEM_PTR->Name() << "\", profile=" << ITEM_PROFILE.ToString()
               << ") item's category was weapon but weapon_type was NotAWeapon.";

            throw std::runtime_error(ss.str());
        }

        if ((ITEM_PTR->IsWeapon()) && (ITEM_PTR->WeaponType() & weapon_type::Breath))
        {
            std::ostringstream ss;
            ss << "item::ItemFactory::TestItem(which_test=" << WHICH_TEST << ", item=\""
               << ITEM_PTR->Name() << "\", profile=" << ITEM_PROFILE.ToString()
               << ") item's weapon_type was Breath.";

            throw std::runtime_error(ss.str());
        }

        if ((ITEM_PTR->IsArmor()) && (ITEM_PTR->ArmorType() == armor_type::NotArmor))
        {
            std::ostringstream ss;
            ss << "item::ItemFactory::TestItem(which_test=" << WHICH_TEST << ", item=\""
               << ITEM_PTR->Name() << "\", profile=" << ITEM_PROFILE.ToString()
               << ") item's category was armor but armor_type was NotArmor.";

            throw std::runtime_error(ss.str());
        }

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ITEM_PTR->IsBodypart() == false),
            "item::ItemFactory::TestItem(which_test="
                << WHICH_TEST << ", item=\"" << ITEM_PTR->Name()
                << "\", profile=" << ITEM_PROFILE.ToString()
                << ") created a bodypart item but there should be no bodypart items.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ITEM_PTR->IsBroken() == false),
            "item::ItemFactory::TestItem(which_test="
                << WHICH_TEST << ", item=\"" << ITEM_PTR->Name()
                << "\", profile=" << ITEM_PROFILE.ToString() << ") created a broken item.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ITEM_PTR->Category() != category::None),
            "item::ItemFactory::TestItem(which_test="
                << WHICH_TEST << ", item=\"" << ITEM_PTR->Name() << "\", profile="
                << ITEM_PROFILE.ToString() << ") created an item with no/empty category.");

        if (((ITEM_PTR->SetType() != set_type::Count) && (ITEM_PTR->SetType() != set_type::NotASet))
            && (ITEM_PTR->ElementType() != element_type::None))
        {
            std::ostringstream ss;
            ss << "item::ItemFactory::TestItem(which_test=" << WHICH_TEST << ", item=\""
               << ITEM_PTR->Name() << "\", profile=" << ITEM_PROFILE.ToString()
               << ") item is a set_type but also an element_type.";

            throw std::runtime_error(ss.str());
        }

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ITEM_PTR->ImageFilename().empty() == false),
            "item::ItemFactory::TestItem(which_test="
                << WHICH_TEST << ", item=\"" << ITEM_PTR->Name() << "\", profile="
                << ITEM_PROFILE.ToString() << ") created an item with no/empty image filename.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (element_type::IsValid(ITEM_PTR->ElementType())),
            "item::ItemFactory::TestItem(which_test="
                << WHICH_TEST << ", item=\"" << ITEM_PTR->Name() << "\", profile="
                << ITEM_PROFILE.ToString() << ") created an item with an invalid element_type.");
    }

    const ItemPtr_t ItemFactory::Make(const ItemProfile & PROFILE) const
    {
        ItemPtrOpt_t itemPtrOpt{ boost::none };

        if (PROFILE.IsAventail())
        {
            itemPtrOpt
                = creature::EnchantmentFactory::Instance()->MakeStoreAttachReturn(SetTypesAndReturn(
                    PROFILE,
                    armor::ArmorFactory::Instance()->Make_Aventail(
                        PROFILE.BaseType(),
                        PROFILE.MaterialPrimary(),
                        PROFILE.MaterialSecondary())));
        }

        if (PROFILE.IsBoots())
        {
            itemPtrOpt
                = creature::EnchantmentFactory::Instance()->MakeStoreAttachReturn(SetTypesAndReturn(
                    PROFILE,
                    armor::ArmorFactory::Instance()->Make_Boots(
                        PROFILE.BaseType(),
                        PROFILE.MaterialPrimary(),
                        PROFILE.MaterialSecondary(),
                        PROFILE.IsPixie())));
        }

        if (PROFILE.IsBracer())
        {
            itemPtrOpt
                = creature::EnchantmentFactory::Instance()->MakeStoreAttachReturn(SetTypesAndReturn(
                    PROFILE,
                    armor::ArmorFactory::Instance()->Make_Bracer(
                        PROFILE.BaseType(),
                        PROFILE.MaterialPrimary(),
                        PROFILE.MaterialSecondary(),
                        PROFILE.IsPixie())));
        }

        if (PROFILE.IsGauntlets())
        {
            itemPtrOpt
                = creature::EnchantmentFactory::Instance()->MakeStoreAttachReturn(SetTypesAndReturn(
                    PROFILE,
                    armor::ArmorFactory::Instance()->Make_Gauntlets(
                        PROFILE.BaseType(),
                        PROFILE.MaterialPrimary(),
                        PROFILE.MaterialSecondary(),
                        PROFILE.IsPixie())));
        }

        if (PROFILE.IsPants())
        {
            itemPtrOpt
                = creature::EnchantmentFactory::Instance()->MakeStoreAttachReturn(SetTypesAndReturn(
                    PROFILE,
                    armor::ArmorFactory::Instance()->Make_Pants(
                        PROFILE.BaseType(),
                        PROFILE.MaterialPrimary(),
                        PROFILE.MaterialSecondary(),
                        PROFILE.IsPixie())));
        }

        if (PROFILE.IsShirt())
        {
            itemPtrOpt
                = creature::EnchantmentFactory::Instance()->MakeStoreAttachReturn(SetTypesAndReturn(
                    PROFILE,
                    armor::ArmorFactory::Instance()->Make_Shirt(
                        PROFILE.BaseType(),
                        PROFILE.MaterialPrimary(),
                        PROFILE.MaterialSecondary(),
                        PROFILE.IsPixie())));
        }

        if (PROFILE.Category() & item::category::Armor)
        {
            if (PROFILE.ShieldType() != armor::shield_type::Count)
            {
                itemPtrOpt = creature::EnchantmentFactory::Instance()->MakeStoreAttachReturn(
                    SetTypesAndReturn(
                        PROFILE,
                        armor::ArmorFactory::Instance()->Make_Shield(
                            PROFILE.ShieldType(),
                            PROFILE.MaterialPrimary(),
                            PROFILE.MaterialSecondary())));
            }

            if (PROFILE.CoverType() != armor::cover_type::Count)
            {
                itemPtrOpt = creature::EnchantmentFactory::Instance()->MakeStoreAttachReturn(
                    SetTypesAndReturn(
                        PROFILE,
                        armor::ArmorFactory::Instance()->Make_Cover(
                            PROFILE.CoverType(),
                            PROFILE.MaterialPrimary(),
                            PROFILE.MaterialSecondary())));
            }

            if (PROFILE.HelmType() != armor::helm_type::Count)
            {
                itemPtrOpt = creature::EnchantmentFactory::Instance()->MakeStoreAttachReturn(
                    SetTypesAndReturn(
                        PROFILE,
                        armor::ArmorFactory::Instance()->Make_Helm(
                            PROFILE.HelmType(),
                            PROFILE.MaterialPrimary(),
                            PROFILE.MaterialSecondary())));
            }
        }

        if (PROFILE.Category() & item::category::Weapon)
        {
            if (PROFILE.SwordType() != weapon::sword_type::Count)
            {
                itemPtrOpt = creature::EnchantmentFactory::Instance()->MakeStoreAttachReturn(
                    SetTypesAndReturn(
                        PROFILE,
                        weapon::WeaponFactory::Instance()->Make_Sword(
                            PROFILE.SwordType(),
                            PROFILE.MaterialPrimary(),
                            PROFILE.MaterialSecondary())));
            }

            if (PROFILE.AxeType() != weapon::axe_type::Count)
            {
                itemPtrOpt = creature::EnchantmentFactory::Instance()->MakeStoreAttachReturn(
                    SetTypesAndReturn(
                        PROFILE,
                        weapon::WeaponFactory::Instance()->Make_Axe(
                            PROFILE.AxeType(),
                            PROFILE.MaterialPrimary(),
                            PROFILE.MaterialSecondary())));
            }

            if (PROFILE.ClubType() != weapon::club_type::Count)
            {
                itemPtrOpt = creature::EnchantmentFactory::Instance()->MakeStoreAttachReturn(
                    SetTypesAndReturn(
                        PROFILE,
                        weapon::WeaponFactory::Instance()->Make_Club(
                            PROFILE.ClubType(),
                            PROFILE.MaterialPrimary(),
                            PROFILE.MaterialSecondary())));
            }

            if (PROFILE.WhipType() != weapon::whip_type::Count)
            {
                itemPtrOpt = creature::EnchantmentFactory::Instance()->MakeStoreAttachReturn(
                    SetTypesAndReturn(
                        PROFILE,
                        weapon::WeaponFactory::Instance()->Make_Whip(
                            PROFILE.WhipType(),
                            PROFILE.MaterialPrimary(),
                            PROFILE.MaterialSecondary())));
            }

            if (PROFILE.ProjectileType() != weapon::projectile_type::Count)
            {
                itemPtrOpt = creature::EnchantmentFactory::Instance()->MakeStoreAttachReturn(
                    SetTypesAndReturn(
                        PROFILE,
                        weapon::WeaponFactory::Instance()->Make_Projectile(
                            PROFILE.ProjectileType(),
                            PROFILE.MaterialPrimary(),
                            PROFILE.MaterialSecondary())));
            }

            if (PROFILE.BladedStaffType() != weapon::bladedstaff_type::Count)
            {
                itemPtrOpt = creature::EnchantmentFactory::Instance()->MakeStoreAttachReturn(
                    SetTypesAndReturn(
                        PROFILE,
                        weapon::WeaponFactory::Instance()->Make_BladedStaff(
                            PROFILE.BladedStaffType(),
                            PROFILE.MaterialPrimary(),
                            PROFILE.MaterialSecondary())));
            }
        }

        if (PROFILE.IsKnife())
        {
            itemPtrOpt
                = creature::EnchantmentFactory::Instance()->MakeStoreAttachReturn(SetTypesAndReturn(
                    PROFILE,
                    weapon::WeaponFactory::Instance()->Make_Knife(
                        false,
                        PROFILE.Size(),
                        PROFILE.MaterialPrimary(),
                        PROFILE.MaterialSecondary(),
                        PROFILE.IsPixie())));
        }

        if (PROFILE.IsDagger())
        {
            itemPtrOpt
                = creature::EnchantmentFactory::Instance()->MakeStoreAttachReturn(SetTypesAndReturn(
                    PROFILE,
                    weapon::WeaponFactory::Instance()->Make_Knife(
                        true,
                        PROFILE.Size(),
                        PROFILE.MaterialPrimary(),
                        PROFILE.MaterialSecondary(),
                        PROFILE.IsPixie())));
        }

        if (PROFILE.IsStaff())
        {
            itemPtrOpt
                = creature::EnchantmentFactory::Instance()->MakeStoreAttachReturn(SetTypesAndReturn(
                    PROFILE,
                    weapon::WeaponFactory::Instance()->Make_Staff(
                        false,
                        PROFILE.MaterialPrimary(),
                        PROFILE.MaterialSecondary(),
                        PROFILE.IsPixie())));
        }

        if (PROFILE.IsQuarterStaff())
        {
            itemPtrOpt
                = creature::EnchantmentFactory::Instance()->MakeStoreAttachReturn(SetTypesAndReturn(
                    PROFILE,
                    weapon::WeaponFactory::Instance()->Make_Staff(
                        true,
                        PROFILE.MaterialPrimary(),
                        PROFILE.MaterialSecondary(),
                        PROFILE.IsPixie())));
        }

        if ((PROFILE.MiscType() != misc_type::Count) && (PROFILE.MiscType() != misc_type::NotMisc))
        {
            itemPtrOpt = creature::EnchantmentFactory::Instance()->MakeStoreAttachReturn(
                SetTypesAndReturn(PROFILE, item::MiscItemFactory::Instance()->Make(PROFILE)));
        }

        M_ASSERT_OR_LOGANDTHROW_SS(
            (!!itemPtrOpt),
            "item::ItemFactory::Make(profile="
                << PROFILE.ToString() << ") failed to create an item based on that profile.");

        auto const ITEM_PTR{ itemPtrOpt.value() };

        if (PROFILE.UniqueType() == unique_type::MagnifyingGlass)
        {
            ITEM_PTR->AddCategory(category::ShowsEnemyInfo);
        }

        if (item::unique_type::IsUseable(PROFILE.UniqueType()))
        {
            ITEM_PTR->AddCategory(category::Useable);
        }

        ITEM_PTR->SetSummonInfo(PROFILE.Summoning());

        AppendElementTypeToName(ITEM_PTR, PROFILE);

        return ITEM_PTR;
    }

    const ItemPtr_t ItemFactory::SetTypesAndReturn(
        const ItemProfile & PROFILE, const ItemPtr_t & ITEM_PTR) const
    {
        if (PROFILE.ElementType() != element_type::None)
        {
            ITEM_PTR->SetElementType(PROFILE.ElementType());
        }

        if ((PROFILE.UniqueType() != unique_type::Count)
            && (PROFILE.UniqueType() != unique_type::NotUnique))
        {
            ITEM_PTR->SetUniqueType(PROFILE.UniqueType());
        }

        if ((PROFILE.NamedType() != named_type::Count)
            && (PROFILE.NamedType() != named_type::NotNamed))
        {
            ITEM_PTR->SetNamedType(PROFILE.NamedType());
        }

        if ((PROFILE.SetType() != set_type::Count) && (PROFILE.SetType() != set_type::NotASet))
        {
            ITEM_PTR->SetSetType(PROFILE.SetType());
        }

        if ((PROFILE.MiscType() != misc_type::Count) && (PROFILE.MiscType() != misc_type::NotMisc))
        {
            ITEM_PTR->SetMiscType(PROFILE.MiscType());
        }

        if ((PROFILE.Role() != creature::role::Count)
            && (PROFILE.Role() != creature::role::PlayerRoleCount))
        {
            ITEM_PTR->SetRestrictedRole(PROFILE.Role());
        }

        return ITEM_PTR;
    }

    void ItemFactory::AppendElementTypeToName(
        const ItemPtr_t & ITEM_PTR, const ItemProfile & PROFILE) const
    {
        if (PROFILE.ElementType() != element_type::None)
        {
            ITEM_PTR->SetName(ITEM_PTR->Name() + " " + element_type::Name(PROFILE.ElementType()));
        }
    }

} // namespace item
} // namespace heroespath
