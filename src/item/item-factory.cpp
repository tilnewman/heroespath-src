// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// item-factory.cpp
//
#include "item-factory.hpp"

#include "creature/creature.hpp"
#include "creature/summon-info.hpp"
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
#include "misc/vector-map.hpp"
#include "sfml-util/gui/item-image-loader.hpp"

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

            return false;
        }

        static auto didPostItemProfileCreationStart{ false };
        if (false == didPostItemProfileCreationStart)
        {
            game::LoopManager::Instance()->TestingStrAppend(
                "item::ItemFactory::Test() Starting ItemProfile creation.  Please wait...");

            didPostItemProfileCreationStart = true;
        }

        static item::ItemProfileVec_t profiles;

        static auto hasCreatedAllProfilesVector{ false };
        if (false == hasCreatedAllProfilesVector)
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (ItemProfileWarehouse::Instance()->GetNormalProfiles().empty() == false),
                "item::ItemFactory::Test() found ItemProfieWarehouse's normal profiles empty.");

            for (auto const & PROFILE : ItemProfileWarehouse::Instance()->GetNormalProfiles())
            {
                profiles.emplace_back(PROFILE);
            }

            M_ASSERT_OR_LOGANDTHROW_SS(
                (ItemProfileWarehouse::Instance()->GetReligiousProfiles().empty() == false),
                "item::ItemFactory::Test() found ItemProfieWarehouse's normal profiles empty.");

            for (auto const & PROFILE : ItemProfileWarehouse::Instance()->GetReligiousProfiles())
            {
                profiles.emplace_back(PROFILE);
            }

            M_ASSERT_OR_LOGANDTHROW_SS(
                (ItemProfileWarehouse::Instance()->GetQuestProfiles().Empty() == false),
                "item::ItemFactory::Test() found ItemProfieWarehouse's normal profiles empty.");

            for (auto const & MISCENUM_PROFILE_PAIR :
                 ItemProfileWarehouse::Instance()->GetQuestProfiles())
            {
                profiles.emplace_back(MISCENUM_PROFILE_PAIR.second);
            }

            hasCreatedAllProfilesVector = true;
        }

        static auto hasTestedForDuplicates{ false };
        if (false == hasTestedForDuplicates)
        {
            game::LoopManager::Instance()->TestingStrAppend(
                "item::ItemFactory::Test() Starting Duplicate Test.  Please "
                "wait...");

            std::sort(std::begin(profiles), std::end(profiles));

            auto const FIRST_DUPLICATE_PROFILE_ITER{ std::unique(
                std::begin(profiles), std::end(profiles)) };

            item::ItemProfileVec_t duplicateProfiles;
            std::copy(
                FIRST_DUPLICATE_PROFILE_ITER,
                std::end(profiles),
                std::back_inserter(duplicateProfiles));

            if (duplicateProfiles.empty() == false)
            {
                const std::size_t DUPLICATE_COUNT_TO_DISPLAY{ 10 };

                M_HP_LOG_ERR(
                    "item::ItemFactory::Test() Normal Profiles Duplicate Test FAILED.  "
                    << duplicateProfiles.size() << " duplicates found.  Here are the first "
                    << DUPLICATE_COUNT_TO_DISPLAY << ":");

                for (std::size_t i(0); i < DUPLICATE_COUNT_TO_DISPLAY; ++i)
                {
                    if (i < duplicateProfiles.size())
                    {
                        M_HP_LOG_ERR(
                            "\t DUPLICATE PROFILE={" << duplicateProfiles.at(i).ToString()
                                                     << "}\n");
                    }
                    else
                    {
                        break;
                    }
                }
            }

            game::LoopManager::Instance()->TestingStrAppend(
                "item::ItemFactory::Test() Duplicate Test finished.");

            hasTestedForDuplicates = true;
            return false;
        }

        static misc::VectorMap<std::string, item::ItemProfile> imageFilenameProfileMap;
        imageFilenameProfileMap.Reserve(600); // there were 556 item images as of 2018-5-1

        static auto hasCreatedAndTestedAll{ false };
        if (false == hasCreatedAndTestedAll)
        {
            for (auto const & PROFILE : profiles)
            {
                auto itemPtr{ Make(PROFILE) };
                TestItem(itemPtr, PROFILE);
                imageFilenameProfileMap.AppendIfKeyNotFound(itemPtr->ImageFilename(), PROFILE);
                ItemWarehouse::Access().Free(itemPtr);
            }

            hasCreatedAndTestedAll = true;
            return false;
        }

        static auto didTestImages{ false };
        if (false == didTestImages)
        {
            std::ostringstream ss;
            ss << "item::ItemFactory::Test() Starting Images Test.  ("
               << imageFilenameProfileMap.Size() << ")  Please wait...";

            game::LoopManager::Instance()->TestingStrAppend(ss.str());

            sfml_util::gui::ItemImageLoader itemImageMachine;

            for (auto const & FILENAME_PROFILE_PAIR : imageFilenameProfileMap)
            {
                auto const DOES_FILE_EXIST{ itemImageMachine.DoesFileExists(
                    FILENAME_PROFILE_PAIR.first) };

                M_ASSERT_OR_LOGANDTHROW_SS(
                    (DOES_FILE_EXIST),
                    "item::ItemFactory::Test() Images Test found an image that did not exist:  "
                    "filename=\""
                        << FILENAME_PROFILE_PAIR.first << "\", profile={"
                        << FILENAME_PROFILE_PAIR.second.ToString() << "}");
            }

            game::LoopManager::Instance()->TestingStrAppend(
                "item::ItemFactory::Test() Starting Images Test Complete.");

            didTestImages = true;
            return false;
        }

        static auto didLogRandomItemsForNameAndDescriptionCheck{ false };
        if (false == didLogRandomItemsForNameAndDescriptionCheck)
        {
            didLogRandomItemsForNameAndDescriptionCheck = true;

            std::ostringstream randNameAndDescSS;
            randNameAndDescSS << "\n\n~~~~~~~~~~~~~~~~~~~~ Random Item Names and Descriptions To "
                                 "Verify ~~~~~~~~~~~~~~~~~~~~";

            auto appendProfileNamesIf{ [&](auto ifLambda,
                                           const std::string & CATEGORY_NAME = "",
                                           const std::size_t ORIG_COUNT_TO_DISPLAY = 10) {
                ItemProfileVec_t tempProfiles;
                tempProfiles.reserve(50000);

                for (auto const & PROFILE : profiles)
                {
                    if (ifLambda(PROFILE))
                    {
                        tempProfiles.emplace_back(PROFILE);
                    }
                }

                auto const ACTUAL_COUNT_TO_DISPLAY{ (
                    (ORIG_COUNT_TO_DISPLAY == 0) ? tempProfiles.size() : ORIG_COUNT_TO_DISPLAY) };

                if (ACTUAL_COUNT_TO_DISPLAY > 1)
                {
                    randNameAndDescSS << "\n~~~" << ACTUAL_COUNT_TO_DISPLAY << " " << CATEGORY_NAME
                                      << ":\n";
                }
                else
                {
                    randNameAndDescSS << '\n';
                }

                for (std::size_t i(0); i < ACTUAL_COUNT_TO_DISPLAY; ++i)
                {
                    auto const & PROFILE{ misc::Vector::SelectRandom(tempProfiles) };
                    auto itemPtr{ Make(PROFILE) };

                    randNameAndDescSS << "\tName=\"" << itemPtr->Name() << "\"\n\tDesc=\""
                                      << itemPtr->Desc() << "\"\n\tItem={" << itemPtr->ToString()
                                      << "}\n\n";

                    ItemWarehouse::Access().Free(itemPtr);
                }

                if (ACTUAL_COUNT_TO_DISPLAY > 1)
                {
                    randNameAndDescSS << "~~~\n\n";
                }
                else
                {
                    randNameAndDescSS << "\n\n";
                }
            } };

            appendProfileNamesIf(
                [](const ItemProfile & PROFILE) { return PROFILE.IsMisc(); }, "Misc");

            appendProfileNamesIf(
                [](const ItemProfile & PROFILE) { return PROFILE.IsSet(); }, "Sets");

            appendProfileNamesIf(
                [](const ItemProfile & PROFILE) { return PROFILE.IsNamed(); }, "Named");

            appendProfileNamesIf(
                [](const ItemProfile & PROFILE) { return PROFILE.IsUnique(); }, "Unique");

            appendProfileNamesIf(
                [](const ItemProfile & PROFILE) { return PROFILE.IsElemental(); }, "Element");

            appendProfileNamesIf(
                [](const ItemProfile & PROFILE) { return PROFILE.IsWeapon(); }, "Weapons");

            appendProfileNamesIf(
                [](const ItemProfile & PROFILE) { return PROFILE.WeaponInfo().IsAxe(); }, "Axe", 3);

            appendProfileNamesIf(
                [](const ItemProfile & PROFILE) { return PROFILE.WeaponInfo().IsBladedStaff(); },
                "BladedStaff",
                3);

            appendProfileNamesIf(
                [](const ItemProfile & PROFILE) { return PROFILE.WeaponInfo().IsClub(); },
                "Club",
                3);

            appendProfileNamesIf(
                [](const ItemProfile & PROFILE) { return PROFILE.WeaponInfo().IsProjectile(); },
                "Projectile",
                3);

            appendProfileNamesIf(
                [](const ItemProfile & PROFILE) { return PROFILE.WeaponInfo().IsSword(); },
                "Sword",
                3);

            appendProfileNamesIf(
                [](const ItemProfile & PROFILE) { return PROFILE.WeaponInfo().IsWhip(); },
                "Whip",
                3);

            appendProfileNamesIf(
                [](const ItemProfile & PROFILE) { return PROFILE.WeaponInfo().IsKnife(); },
                "Knife",
                3);

            appendProfileNamesIf(
                [](const ItemProfile & PROFILE) { return PROFILE.WeaponInfo().IsDagger(); },
                "Dagger",
                3);

            appendProfileNamesIf(
                [](const ItemProfile & PROFILE) { return PROFILE.WeaponInfo().IsStaff(); },
                "Staff",
                3);

            appendProfileNamesIf(
                [](const ItemProfile & PROFILE) { return PROFILE.IsArmor(); }, "Armor");

            appendProfileNamesIf(
                [](const ItemProfile & PROFILE) { return PROFILE.ArmorInfo().IsAventail(); },
                "Aventail",
                3);

            appendProfileNamesIf(
                [](const ItemProfile & PROFILE) { return PROFILE.ArmorInfo().IsBoots(); },
                "Boots",
                3);

            appendProfileNamesIf(
                [](const ItemProfile & PROFILE) { return PROFILE.ArmorInfo().IsBracers(); },
                "Bracers",
                3);

            appendProfileNamesIf(
                [](const ItemProfile & PROFILE) { return PROFILE.ArmorInfo().IsCover(); },
                "Covering",
                3);

            appendProfileNamesIf(
                [](const ItemProfile & PROFILE) { return PROFILE.ArmorInfo().IsGauntlets(); },
                "Gauntlets",
                3);

            appendProfileNamesIf(
                [](const ItemProfile & PROFILE) { return PROFILE.ArmorInfo().IsHelm(); },
                "Helm",
                10);

            appendProfileNamesIf(
                [](const ItemProfile & PROFILE) { return PROFILE.ArmorInfo().IsPants(); },
                "Pants",
                3);

            appendProfileNamesIf(
                [](const ItemProfile & PROFILE) { return PROFILE.ArmorInfo().IsShield(); },
                "Shield",
                10);

            appendProfileNamesIf(
                [](const ItemProfile & PROFILE) { return PROFILE.ArmorInfo().IsShirt(); },
                "Shirt",
                3);

            appendProfileNamesIf(
                [](const ItemProfile & PROFILE) { return PROFILE.IsUnique(); }, "Unique", 10);

            randNameAndDescSS << "\n\n~~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~~~~\n\n";

            M_HP_LOG_DBG(randNameAndDescSS.str());
        }

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

    void ItemFactory::TestItem(const ItemPtr_t & ITEM_PTR, const ItemProfile & ITEM_PROFILE)
    {
        auto makeErrorReportPrefix{ [&]() {
            std::ostringstream ss;

            ss << std::boolalpha << "item::ItemFactory::TestItem(\nitem={" << ITEM_PTR->ToString()
               << "}\nprofile={" << ITEM_PROFILE.ToString() << "})\n *** ERROR: ";

            return ss.str();
        } };

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ITEM_PTR->MaterialPrimary() != ITEM_PTR->MaterialSecondary()),
            makeErrorReportPrefix() << "made an item with duplicate materials.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ITEM_PTR->Category() == ITEM_PROFILE.Category()),
            makeErrorReportPrefix() << "categories did not match.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ITEM_PTR->MiscType() == ITEM_PROFILE.MiscType()),
            makeErrorReportPrefix() << "misc_types did not match.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ITEM_PTR->MiscType() != misc_type::Count),
            makeErrorReportPrefix() << "misc_type was Count.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ITEM_PTR->SetType() == ITEM_PROFILE.SetType()),
            makeErrorReportPrefix() << "set_types did not match.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ITEM_PTR->SetType() != set_type::Count),
            makeErrorReportPrefix() << "set_type was Count.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ITEM_PTR->ElementType() == ITEM_PROFILE.ElementType()),
            makeErrorReportPrefix() << "element_types did not match.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (element_type::IsValid(ITEM_PTR->ElementType())),
            makeErrorReportPrefix() << "invalid element_type.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ITEM_PTR->NamedType() == ITEM_PROFILE.NamedType()),
            makeErrorReportPrefix() << "named_types did not match.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ITEM_PTR->NamedType() != named_type::Count),
            makeErrorReportPrefix() << "named_type was Count.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ITEM_PTR->MaterialPrimary() == ITEM_PROFILE.MaterialPrimary()),
            makeErrorReportPrefix() << "primary materials did not match.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            ((ITEM_PTR->MaterialPrimary() != material::Count)
             && (ITEM_PTR->MaterialPrimary() != material::Nothing)),
            makeErrorReportPrefix() << "primary material was either Count or Nothing.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ITEM_PTR->MaterialSecondary() == ITEM_PROFILE.MaterialSecondary()),
            makeErrorReportPrefix() << "secondary materials did not match.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ITEM_PTR->MaterialSecondary() != material::Count),
            makeErrorReportPrefix() << "secondary material was Count instead of Nothing.");

        if (ITEM_PTR->IsWeapon() || ITEM_PTR->IsArmor())
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                ((ITEM_PTR->IsWeapon()) != (ITEM_PTR->IsArmor())),
                makeErrorReportPrefix() << "both weapon and armor at the same time.");
        }

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ITEM_PTR->IsBodypart() == false),
            makeErrorReportPrefix()
                << "bodypart, but the ItemProfileWarehouse should not create any bodypart "
                   "items since they cannot be found in treasure chests.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ITEM_PTR->IsBroken() == false), makeErrorReportPrefix() << "broken.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ITEM_PTR->ImageFilename().empty() == false),
            makeErrorReportPrefix() << "no/empty image filename.");

        if (ITEM_PTR->ArmorInfo().HelmType() == armor::helm_type::Leather)
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (ITEM_PTR->MaterialPrimary() == material::Leather),
                makeErrorReportPrefix()
                    << "Leather Helm had a primary material that was not hard leather.");
        }

        // set_types cannot be elemental
        if (ITEM_PTR->IsSet())
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (ITEM_PTR->IsElemental() == false),
                makeErrorReportPrefix() << "set_type but also element_type.");
        }

        // named_types must be weapons or armor, and cannot be misc
        if (ITEM_PTR->IsNamed())
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                ((ITEM_PTR->IsWeapon() || ITEM_PTR->IsArmor()) && (ITEM_PTR->IsMisc() == false)),
                makeErrorReportPrefix()
                    << "named_type but either not weapon(" << ITEM_PTR->IsWeapon() << ")/armor("
                    << ITEM_PTR->IsArmor() << ") or misc_type(" << ITEM_PTR->IsMisc() << ").");
        }

        auto const SUMMON_INFO{ ITEM_PTR->SummonInfo() };

        // all summoning items are also misc items
        if (SUMMON_INFO.CanSummon())
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (ITEM_PTR->IsMisc()),
                makeErrorReportPrefix()
                    << "CanSummon() but not misc_type, which should never "
                       "happen.  All summoning items are also misc_type items.");

            M_ASSERT_OR_LOGANDTHROW_SS(
                (misc_type::IsSummoning(ITEM_PTR->MiscType())),
                makeErrorReportPrefix() << "CanSummon() but not misc_type::IsSummoning().");
        }

        // invalid summoning info
        if (SUMMON_INFO.IsDefaultAndCompletelyInvalid() == false)
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (SUMMON_INFO.CanSummon()),
                makeErrorReportPrefix() << "summoning but summoning_info was invalid.");
        }

        // if summoning type then must be able to summon, and vice versa
        M_ASSERT_OR_LOGANDTHROW_SS(
            (item::misc_type::IsSummoning(ITEM_PTR->MiscType()) == SUMMON_INFO.CanSummon()),
            makeErrorReportPrefix() << "misc_type=summoning_type but it is not able to summon.");

        // all items must be weapon, armor, or misc
        M_ASSERT_OR_LOGANDTHROW_SS(
            (ITEM_PTR->IsWeapon() || ITEM_PTR->IsArmor()
             || (ITEM_PTR->MiscType() != misc_type::Not)),
            makeErrorReportPrefix()
                << "the item was not a weapon, armor, or misc_type.  All items must be one of "
                   "these three.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ITEM_PTR->BaseName().empty() == false),
            makeErrorReportPrefix()
                << "BaseName (\"" << ITEM_PTR->BaseName() << "\") was empty or invalid.");

        if (ITEM_PTR->IsArmor())
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (ITEM_PTR->ArmorInfo().IsTypeValid()),
                makeErrorReportPrefix() << "IsArmor() but ArmorInfo().IsValid()==false.");

            M_ASSERT_OR_LOGANDTHROW_SS(
                ((ITEM_PTR->ArmorInfo().NameMaterialType() != name_material_type::Count)
                 && (ITEM_PTR->ArmorInfo().NameMaterialType() != name_material_type::Misc)),
                makeErrorReportPrefix()
                    << "armor had invalid name_material_type="
                    << ((ITEM_PTR->ArmorInfo().NameMaterialType() == name_material_type::Count)
                            ? "Count"
                            : name_material_type::ToString(
                                  ITEM_PTR->ArmorInfo().NameMaterialType()))
                    << ".");

            if (ITEM_PTR->ArmorType() == armor_type::Gauntlets)
            {
                if (material::IsGarmentType(ITEM_PTR->MaterialPrimary()))
                {
                    M_ASSERT_OR_LOGANDTHROW_SS(
                        (ITEM_PTR->ArmorInfo().SpecificName()
                         == armor::ArmorTypeWrapper::GLOVES_NAME_),
                        makeErrorReportPrefix()
                            << "gauntles with primary material "
                            << material::ToString(ITEM_PTR->MaterialPrimary())
                            << " which means gloves but all the names did not equal \""
                            << armor::ArmorTypeWrapper::GLOVES_NAME_
                            << "\".  ArmorTypeWrapper::SpecificName()=\""
                            << ITEM_PTR->ArmorInfo().SpecificName() << "\".");
                }
                else
                {
                    M_ASSERT_OR_LOGANDTHROW_SS(
                        (ITEM_PTR->ArmorInfo().SpecificName()
                         != armor::ArmorTypeWrapper::GLOVES_NAME_),
                        makeErrorReportPrefix() << "gauntles with primary material "
                                                << material::ToString(ITEM_PTR->MaterialPrimary())
                                                << " which means gauntlets and NOT gloves but the "
                                                   "ArmorTypeWrapper::SpecificName()=\""
                                                << ITEM_PTR->ArmorInfo().SpecificName() << "\".");
                }
            }

            if (ITEM_PTR->ArmorType() != armor_type::Skin)
            {
                auto const IS_SPECIFIC_ARMOR_TYPE{ (
                    (ITEM_PTR->ArmorType() == armor_type::Covering)
                    || (ITEM_PTR->ArmorType() == armor_type::Helm)
                    || (ITEM_PTR->ArmorType() == armor_type::Shield)) };

                if (IS_SPECIFIC_ARMOR_TYPE)
                {
                    M_ASSERT_OR_LOGANDTHROW_SS(
                        (ITEM_PTR->ArmorInfo().BaseType() == armor::base_type::Count),
                        makeErrorReportPrefix()
                            << "armor is specific armor_type="
                            << armor_type::ToString(ITEM_PTR->ArmorType())
                            << " but had an invalid base_type="
                            << armor::base_type::ToString(ITEM_PTR->ArmorInfo().BaseType()) << ".");
                }
                else
                {
                    M_ASSERT_OR_LOGANDTHROW_SS(
                        (ITEM_PTR->ArmorInfo().BaseType() != armor::base_type::Count),
                        makeErrorReportPrefix()
                            << "armor is non-specific armor_type="
                            << armor_type::ToString(ITEM_PTR->ArmorType())
                            << " but had a base_type="
                            << armor::base_type::ToString(ITEM_PTR->ArmorInfo().BaseType()) << ".");

                    if (ITEM_PTR->ArmorInfo().BaseType() == armor::base_type::Plain)
                    {
                        M_ASSERT_OR_LOGANDTHROW_SS(
                            ((ITEM_PTR->MaterialPrimary() == material::Cloth)
                             || (ITEM_PTR->MaterialPrimary() == material::Leather)
                             || (ITEM_PTR->MaterialPrimary() == material::Silk)
                             || (ITEM_PTR->MaterialPrimary() == material::Fur)
                             || (ITEM_PTR->MaterialPrimary() == material::Hide)),
                            makeErrorReportPrefix()
                                << "armor is non-specific armor_type="
                                << armor_type::ToString(ITEM_PTR->ArmorType())
                                << " with base_type=Plain but the primary material was not one of "
                                   "the "
                                   "valid 'plain' materials.  It was "
                                << material::ToString(ITEM_PTR->MaterialPrimary()) << ".");
                    }
                    else if (
                        (ITEM_PTR->ArmorInfo().BaseType() == armor::base_type::Mail)
                        || (ITEM_PTR->ArmorInfo().BaseType() == armor::base_type::Plate))
                    {
                        M_ASSERT_OR_LOGANDTHROW_SS(
                            (material::IsMetal(ITEM_PTR->MaterialPrimary())),
                            makeErrorReportPrefix()
                                << "armor is non-specific armor_type="
                                << armor_type::ToString(ITEM_PTR->ArmorType()) << " with base_type="
                                << armor::base_type::ToString(ITEM_PTR->ArmorInfo().BaseType())
                                << " but the primary material was not metal.  It was "
                                << material::ToString(ITEM_PTR->MaterialPrimary()) << ".");
                    }
                    else if (ITEM_PTR->ArmorInfo().BaseType() == armor::base_type::Scale)
                    {
                        M_ASSERT_OR_LOGANDTHROW_SS(
                            (ITEM_PTR->MaterialPrimary() == material::Scales),
                            makeErrorReportPrefix()
                                << "armor is non-specific armor_type="
                                << armor_type::ToString(ITEM_PTR->ArmorType())
                                << " with base_type=Scale but the primary material was not Scales. "
                                   " It "
                                   "was "
                                << material::ToString(ITEM_PTR->MaterialPrimary()) << ".");
                    }
                }
            }
        }

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ITEM_PTR->Weight() >= 1_weight),
            makeErrorReportPrefix() << "weight (" << ITEM_PTR->Weight() << ") was less than one.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ITEM_PROFILE.WeaponType() == ITEM_PTR->WeaponType()),
            makeErrorReportPrefix()
                << " weapon_types did not match.  profile="
                << weapon_type::ToString(ITEM_PROFILE.WeaponType(), misc::Wrap::Yes) << " but item="
                << weapon_type::ToString(ITEM_PTR->WeaponType(), misc::Wrap::Yes) << ".");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ITEM_PROFILE.ArmorType() == ITEM_PTR->ArmorType()),
            makeErrorReportPrefix()
                << " armor_types did not match.  profile="
                << armor_type::ToString(ITEM_PROFILE.ArmorType())
                << " but item=" << armor_type::ToString(ITEM_PTR->ArmorType()) << ".");

        if (ITEM_PTR->IsMisc())
        {
            // unique_types cannot be set
            if (misc_type::IsUnique(ITEM_PTR->MiscType()))
            {
                M_ASSERT_OR_LOGANDTHROW_SS(
                    (ITEM_PTR->IsSet() == false),
                    makeErrorReportPrefix() << "misc_type unique but also set_type.");
            }

            M_ASSERT_OR_LOGANDTHROW_SS(
                (misc_type::IsWeapon(ITEM_PTR->MiscType()) == ITEM_PTR->IsWeapon()),
                makeErrorReportPrefix()
                    << "misc_type::IsWeapon()=" << misc_type::IsWeapon(ITEM_PTR->MiscType())
                    << " but ITEM_PTR->IsWeapon()=" << ITEM_PTR->IsWeapon() << ".");

            M_ASSERT_OR_LOGANDTHROW_SS(
                (misc_type::IsArmor(ITEM_PTR->MiscType()) == ITEM_PTR->IsArmor()),
                makeErrorReportPrefix()
                    << "misc_type::IsArmor()=" << misc_type::IsArmor(ITEM_PTR->MiscType())
                    << " but ITEM_PTR->IsArmor()=" << ITEM_PTR->IsArmor()
                    << " -these should match.");

            const bool IS_ITEM_EQUIPPABLE{ (ITEM_PTR->Category() & category::Equippable) != 0 };
            const bool IS_TYPE_EQUIPPABLE{ misc_type::IsEquippable(ITEM_PTR->MiscType()) };

            M_ASSERT_OR_LOGANDTHROW_SS(
                (IS_TYPE_EQUIPPABLE == IS_ITEM_EQUIPPABLE),
                makeErrorReportPrefix()
                    << "misc_type::IsEquippable(" << misc_type::ToString(ITEM_PTR->MiscType())
                    << ")=" << IS_TYPE_EQUIPPABLE
                    << " != (ITEM_PTR->Category() & category::Equippable)=" << IS_ITEM_EQUIPPABLE
                    << ".");

            auto const IS_QUEST{ misc_type::IsQuestItem(ITEM_PTR->MiscType()) };
            auto const IS_UNIQUE{ misc_type::IsUnique(ITEM_PTR->MiscType()) };
            auto const IS_STANDALONE{ misc_type::IsStandalone(ITEM_PTR->MiscType()) };

            auto const MISC_CATEGORY_COUNT{ ((IS_QUEST) ? 1 : 0) + ((IS_UNIQUE) ? 1 : 0)
                                            + ((IS_STANDALONE) ? 1 : 0) };

            M_ASSERT_OR_LOGANDTHROW_SS(
                (MISC_CATEGORY_COUNT == 1),
                makeErrorReportPrefix()
                    << "misc_type was not one and only one of the three "
                       "misc_type sub-categories:  IsQuestItem="
                    << std::boolalpha << IS_QUEST << ", IsUnique()=" << IS_UNIQUE
                    << ", IsStandalone()=" << IS_STANDALONE << ".");
        }

        if (ITEM_PTR->IsWeapon())
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (ITEM_PTR->WeaponInfo().IsTypeValid()),
                makeErrorReportPrefix() << "IsWeapon() but WeaponInfo().IsValid()==false.");

            int typeCounter{ 0 };
            auto incrementIf{ [&](const bool WILL_INCREMENT) {
                if (WILL_INCREMENT)
                {
                    ++typeCounter;
                }
            } };

            incrementIf(ITEM_PTR->WeaponInfo().IsBodyPart());
            incrementIf(ITEM_PTR->WeaponInfo().IsBlowpipe());
            incrementIf(ITEM_PTR->WeaponInfo().HasHandle());
            incrementIf(ITEM_PTR->WeaponInfo().IsPole());

            M_ASSERT_OR_LOGANDTHROW_SS(
                (1 == typeCounter),
                makeErrorReportPrefix()
                    << "weapon was not one of four states thought to be complete and mutually "
                       "exclusive:  IsBodyPart()="
                    << std::boolalpha << ITEM_PTR->WeaponInfo().IsBodyPart()
                    << ", IsBlowpipe()=" << ITEM_PTR->WeaponInfo().IsBlowpipe()
                    << ", HasSmallHandle()=" << ITEM_PTR->WeaponInfo().HasHandle()
                    << ", HasLargeHandle()=" << ITEM_PTR->WeaponInfo().IsPole() << ".");

            M_ASSERT_OR_LOGANDTHROW_SS(
                ((ITEM_PTR->WeaponInfo().NameMaterialType() != name_material_type::Count)
                 && (ITEM_PTR->WeaponInfo().NameMaterialType() != name_material_type::Misc)),
                makeErrorReportPrefix()
                    << "weapon had invalid name_material_type="
                    << ((ITEM_PTR->WeaponInfo().NameMaterialType() == name_material_type::Count)
                            ? "Count"
                            : name_material_type::ToString(
                                  ITEM_PTR->WeaponInfo().NameMaterialType()))
                    << ".");
        }

        auto const IS_MISC{ ITEM_PTR->IsMisc() };
        auto const IS_NONMISC_WEAPON{ ITEM_PTR->IsWeapon() && (ITEM_PTR->IsMisc() == false) };
        auto const IS_NONMISC_ARMOR{ ITEM_PTR->IsArmor() && (ITEM_PTR->IsMisc() == false) };

        if (IS_MISC)
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                ((IS_NONMISC_WEAPON == false) && (IS_NONMISC_ARMOR == false)),
                makeErrorReportPrefix()
                    << "(misc=" << IS_MISC << ", nonmisc_weapon=" << IS_NONMISC_WEAPON
                    << ", nonmisc_armor=" << IS_NONMISC_ARMOR << ") is an invalid combination.");
        }
        else if (IS_NONMISC_WEAPON)
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                ((IS_MISC == false) && (IS_NONMISC_ARMOR == false)),
                makeErrorReportPrefix()
                    << "(misc=" << IS_MISC << ", nonmisc_weapon=" << IS_NONMISC_WEAPON
                    << ", nonmisc_armor=" << IS_NONMISC_ARMOR << ") is an invalid combination.");
        }
        else if (IS_NONMISC_ARMOR)
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                ((IS_MISC == false) && (IS_NONMISC_WEAPON == false)),
                makeErrorReportPrefix()
                    << "(misc=" << IS_MISC << ", nonmisc_weapon=" << IS_NONMISC_WEAPON
                    << ", nonmisc_armor=" << IS_NONMISC_ARMOR << ") is an invalid combination.");
        }
        else
        {
            std::ostringstream ss;
            ss << makeErrorReportPrefix() << "(misc=" << IS_MISC
               << ", nonmisc_weapon=" << IS_NONMISC_WEAPON << ", nonmisc_armor=" << IS_NONMISC_ARMOR
               << ") is an invalid combination.";

            throw std::runtime_error(ss.str());
        }
    }

    const ItemPtr_t ItemFactory::Make(const ItemProfile & PROFILE)
    {
        // Note that some misc_type items are also weapons or armor.  In these cases item
        // construction is handled by WeaponFactory or ArmorFactory.
        if (PROFILE.IsArmor())
        {
            return armor::ArmorFactory::Make(PROFILE);
        }
        else if (PROFILE.IsWeapon())
        {
            return weapon::WeaponFactory::Make(PROFILE);
        }
        else if (PROFILE.IsMisc())
        {
            return item::MiscItemFactory::Make(PROFILE);
        }
        else
        {
            std::ostringstream ss;

            ss << "item::ItemFactory::Make(profile=" << PROFILE.ToString()
               << ") failed to create an item based on that profile because it was not misc, "
                  "weapon, "
                  "or armor.";

            throw std::runtime_error(ss.str());
        }
    }

    const ItemPtr_t ItemFactory::Make(
        const body_part::Enum BODY_PART, const creature::CreaturePtr_t CREATURE_PTR)
    {
        if (BODY_PART == body_part::Skin)
        {
            return armor::ArmorFactory::Make(BODY_PART, CREATURE_PTR);
        }
        else
        {
            return weapon::WeaponFactory::Make(BODY_PART, CREATURE_PTR);
        }
    }

} // namespace item
} // namespace heroespath
