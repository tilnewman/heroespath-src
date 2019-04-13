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
#include "item/armor-details.hpp"
#include "item/armor-type-wrapper.hpp"
#include "item/item-profile-warehouse.hpp"
#include "item/item-profile.hpp"
#include "item/item-warehouse.hpp"
#include "item/item.hpp"
#include "item/weapon-details.hpp"
#include "item/weapon-type-wrapper.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/enum-util.hpp"
#include "misc/filesystem.hpp"
#include "misc/log-macros.hpp"
#include "misc/vector-map.hpp"
#include "stage/i-stage.hpp"

#include <algorithm>
#include <exception>
#include <sstream>
#include <utility>

namespace heroespath
{
namespace item
{

    bool ItemFactory::Test(stage::IStagePtr_t iStagePtr) const
    {
        static auto didPostInitial { false };
        if (false == didPostInitial)
        {
            didPostInitial = true;

            iStagePtr->TestingStrAppend("item::ItemFactory::Test() Starting Tests...");

            return false;
        }

        static auto didPostItemProfileCreationStart { false };
        if (false == didPostItemProfileCreationStart)
        {
            iStagePtr->TestingStrAppend(
                "item::ItemFactory::Test() Starting ItemProfile creation.  Please wait...");

            didPostItemProfileCreationStart = true;
        }

        static item::ItemProfileVec_t profiles;

        static auto hasCreatedAllProfilesVector { false };
        if (false == hasCreatedAllProfilesVector)
        {
            M_HP_ASSERT_OR_LOG_AND_THROW(
                (ItemProfileWarehouse::Instance()->GetNormalProfiles().empty() == false),
                "item::ItemFactory::Test() found ItemProfieWarehouse's normal profiles empty.");

            for (const auto & PROFILE : ItemProfileWarehouse::Instance()->GetNormalProfiles())
            {
                profiles.emplace_back(PROFILE);
            }

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (ItemProfileWarehouse::Instance()->GetReligiousProfiles().empty() == false),
                "item::ItemFactory::Test() found ItemProfieWarehouse's religious profiles empty.");

            for (const auto & PROFILE : ItemProfileWarehouse::Instance()->GetReligiousProfiles())
            {
                profiles.emplace_back(PROFILE);
            }

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (ItemProfileWarehouse::Instance()->GetQuestProfiles().Empty() == false),
                "item::ItemFactory::Test() found ItemProfieWarehouse's quest profiles empty.");

            for (const auto & MISCENUM_PROFILE_PAIR :
                 ItemProfileWarehouse::Instance()->GetQuestProfiles())
            {
                profiles.emplace_back(MISCENUM_PROFILE_PAIR.second);
            }

            hasCreatedAllProfilesVector = true;
        }

        static auto hasTestedForDuplicates { false };
        if (false == hasTestedForDuplicates)
        {
            iStagePtr->TestingStrAppend(
                "item::ItemFactory::Test() Starting Duplicate Test.  Please "
                "wait...");

            std::sort(std::begin(profiles), std::end(profiles));

            item::ItemProfileVec_t duplicateProfiles;
            std::copy(
                std::unique(std::begin(profiles), std::end(profiles)),
                std::end(profiles),
                std::back_inserter(duplicateProfiles));

            if (duplicateProfiles.empty() == false)
            {
                const std::size_t DUPLICATE_COUNT_TO_DISPLAY { 10 };

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

            iStagePtr->TestingStrAppend("item::ItemFactory::Test() Duplicate Test finished.");

            hasTestedForDuplicates = true;
            return false;
        }

        static misc::VectorMap<std::string, item::ItemProfile> imageFilenameProfileMap;
        imageFilenameProfileMap.Reserve(600); // there were 556 item images as of 2018-5-1

        static auto hasCreatedAndTestedAll { false };
        if (false == hasCreatedAndTestedAll)
        {
            iStagePtr->TestingStrAppend(
                "item::ItemFactory::Test() Starting Individual Item Tests....");

            for (const auto & PROFILE : profiles)
            {
                auto itemPtr { Make(PROFILE) };
                TestItem(itemPtr, PROFILE);
                imageFilenameProfileMap.AppendIfKeyNotFound(itemPtr->ImagePath(), PROFILE);
                ItemWarehouse::Access().Free(itemPtr);
            }

            iStagePtr->TestingStrAppend(
                "item::ItemFactory::Test() Finished Individual Item Tests.");

            hasCreatedAndTestedAll = true;
            return false;
        }

        static auto didTestImages { false };
        if (false == didTestImages)
        {
            std::ostringstream ss;
            ss << "item::ItemFactory::Test() Starting Images Test.  ("
               << imageFilenameProfileMap.Size() << ")  Please wait...";

            iStagePtr->TestingStrAppend(ss.str());

            for (const auto & FILENAME_PROFILE_PAIR : imageFilenameProfileMap)
            {
                M_HP_ASSERT_OR_LOG_AND_THROW(
                    (misc::filesystem::ExistsAndIsFile(FILENAME_PROFILE_PAIR.first)),
                    "item::ItemFactory::Test() Images Test found an image that did not exist:  "
                    "filename=\""
                        << FILENAME_PROFILE_PAIR.first << "\", profile={"
                        << FILENAME_PROFILE_PAIR.second.ToString() << "}");
            }

            iStagePtr->TestingStrAppend("item::ItemFactory::Test() Starting Images Test Complete.");

            didTestImages = true;
            return false;
        }

        static auto didLogRandomItemsForNameAndDescriptionCheck { false };
        if (false == didLogRandomItemsForNameAndDescriptionCheck)
        {
            didLogRandomItemsForNameAndDescriptionCheck = true;

            std::ostringstream randNameAndDescSS;
            randNameAndDescSS << "\n\n~~~~~~~~~~~~~~~~~~~~ Random Item Names and Descriptions To "
                                 "Verify ~~~~~~~~~~~~~~~~~~~~";

            auto appendProfileNamesIf { [&](auto ifLambda,
                                            const std::string & CATEGORY_NAME = "",
                                            const std::size_t ORIG_COUNT_TO_DISPLAY = 10) {
                ItemProfileVec_t tempProfiles;
                tempProfiles.reserve(50000);

                for (const auto & PROFILE : profiles)
                {
                    if (ifLambda(PROFILE))
                    {
                        tempProfiles.emplace_back(PROFILE);
                    }
                }

                const auto ACTUAL_COUNT_TO_DISPLAY { (
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
                    const auto & PROFILE { misc::RandomSelect(tempProfiles) };
                    auto itemPtr { Make(PROFILE) };

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

        static auto didPostFinal { false };
        if (false == didPostFinal)
        {
            didPostFinal = true;

            iStagePtr->TestingStrAppend(
                "item::ItemFactory::Test() Testing Complete:  ALL TESTS PASSED");

            return false;
        }

        return true;
    }

    void ItemFactory::TestItem(const ItemPtr_t & ITEM_PTR, const ItemProfile & ITEM_PROFILE) const
    {
        auto makeErrorReportPrefix { [&]() {
            std::ostringstream ss;

            ss << std::boolalpha << "item::ItemFactory::TestItem(\nitem={" << ITEM_PTR->ToString()
               << "}\nprofile={" << ITEM_PROFILE.ToString() << "})\nERROR: ";

            return ss.str();
        } };

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (ITEM_PTR->MaterialPrimary() != ITEM_PTR->MaterialSecondary()),
            makeErrorReportPrefix() << "made an item with duplicate materials.");

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (ITEM_PTR->Category() == ITEM_PROFILE.Category()),
            makeErrorReportPrefix() << "categories did not match.");

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (ITEM_PTR->MiscType() == ITEM_PROFILE.MiscType()),
            makeErrorReportPrefix() << "misc_types did not match.");

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (ITEM_PTR->MiscType() != misc_type::Count),
            makeErrorReportPrefix() << "misc_type was Count.");

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (ITEM_PTR->SetType() == ITEM_PROFILE.SetType()),
            makeErrorReportPrefix() << "set_types did not match.");

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (ITEM_PTR->SetType() != set_type::Count),
            makeErrorReportPrefix() << "set_type was Count.");

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (ITEM_PTR->ElementType() == ITEM_PROFILE.ElementType()),
            makeErrorReportPrefix() << "element_types did not match.");

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (EnumUtil<element_type>::IsValid(ITEM_PTR->ElementType())),
            makeErrorReportPrefix() << "invalid element_type.");

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (ITEM_PTR->NamedType() == ITEM_PROFILE.NamedType()),
            makeErrorReportPrefix() << "named_types did not match.");

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (ITEM_PTR->NamedType() != named_type::Count),
            makeErrorReportPrefix() << "named_type was Count.");

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (ITEM_PTR->MaterialPrimary() == ITEM_PROFILE.MaterialPrimary()),
            makeErrorReportPrefix() << "primary materials did not match.");

        M_HP_ASSERT_OR_LOG_AND_THROW(
            ((ITEM_PTR->MaterialPrimary() != material::Count)
             && (ITEM_PTR->MaterialPrimary() != material::Nothing)),
            makeErrorReportPrefix() << "primary material was either Count or Nothing.");

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (ITEM_PTR->MaterialSecondary() == ITEM_PROFILE.MaterialSecondary()),
            makeErrorReportPrefix() << "secondary materials did not match.");

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (ITEM_PTR->MaterialSecondary() != material::Count),
            makeErrorReportPrefix() << "secondary material was Count instead of Nothing.");

        if (ITEM_PTR->IsWeapon() || ITEM_PTR->IsArmor())
        {
            M_HP_ASSERT_OR_LOG_AND_THROW(
                ((ITEM_PTR->IsWeapon()) != (ITEM_PTR->IsArmor())),
                makeErrorReportPrefix() << "both weapon and armor at the same time.");
        }

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (ITEM_PTR->IsBodypart() == false),
            makeErrorReportPrefix()
                << "bodypart, but the ItemProfileWarehouse should not create any bodypart "
                   "items since they cannot be found in treasure chests.");

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (ITEM_PTR->IsBroken() == false), makeErrorReportPrefix() << "broken.");

        if (ITEM_PTR->ArmorInfo().HelmType() == armor::helm_type::Leather)
        {
            M_HP_ASSERT_OR_LOG_AND_THROW(
                (ITEM_PTR->MaterialPrimary() == material::Leather),
                makeErrorReportPrefix()
                    << "Leather Helm had a primary material that was not hard leather.");
        }

        // set_types cannot be elemental
        if (ITEM_PTR->IsSet())
        {
            M_HP_ASSERT_OR_LOG_AND_THROW(
                (ITEM_PTR->IsElemental() == false),
                makeErrorReportPrefix() << "set_type but also element_type.");
        }

        // named_types must be weapons or armor, and cannot be misc
        if (ITEM_PTR->IsNamed())
        {
            M_HP_ASSERT_OR_LOG_AND_THROW(
                ((ITEM_PTR->IsWeapon() || ITEM_PTR->IsArmor()) && (ITEM_PTR->IsMisc() == false)),
                makeErrorReportPrefix()
                    << "named_type but either not weapon(" << ITEM_PTR->IsWeapon() << ")/armor("
                    << ITEM_PTR->IsArmor() << ") or misc_type(" << ITEM_PTR->IsMisc() << ").");
        }

        const auto SUMMON_INFO { ITEM_PTR->SummonInfo() };

        // all summoning items are also misc items
        if (SUMMON_INFO.CanSummon())
        {
            M_HP_ASSERT_OR_LOG_AND_THROW(
                (ITEM_PTR->IsMisc()),
                makeErrorReportPrefix()
                    << "CanSummon() but not misc_type, which should never "
                       "happen.  All summoning items are also misc_type items.");

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (misc_type::IsSummoning(ITEM_PTR->MiscType())),
                makeErrorReportPrefix() << "CanSummon() but not misc_type::IsSummoning().");
        }

        // invalid summoning info
        if (SUMMON_INFO.IsDefaultAndCompletelyInvalid() == false)
        {
            M_HP_ASSERT_OR_LOG_AND_THROW(
                (SUMMON_INFO.CanSummon()),
                makeErrorReportPrefix() << "summoning but summoning_info was invalid.");
        }

        // if summoning type then must be able to summon, and vice versa
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (item::misc_type::IsSummoning(ITEM_PTR->MiscType()) == SUMMON_INFO.CanSummon()),
            makeErrorReportPrefix() << "misc_type=summoning_type but it is not able to summon.");

        // all items must be weapon, armor, or misc
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (ITEM_PTR->IsWeapon() || ITEM_PTR->IsArmor()
             || (ITEM_PTR->MiscType() != misc_type::Not)),
            makeErrorReportPrefix()
                << "the item was not a weapon, armor, or misc_type.  All items must be one of "
                   "these three.");

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (ITEM_PTR->BaseName().empty() == false),
            makeErrorReportPrefix()
                << "BaseName (\"" << ITEM_PTR->BaseName() << "\") was empty or invalid.");

        if (ITEM_PTR->IsArmor())
        {
            M_HP_ASSERT_OR_LOG_AND_THROW(
                (ITEM_PTR->ArmorInfo().IsTypeValid()),
                makeErrorReportPrefix() << "IsArmor() but ArmorInfo().IsValid()==false.");

            M_HP_ASSERT_OR_LOG_AND_THROW(
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
                    M_HP_ASSERT_OR_LOG_AND_THROW(
                        (ITEM_PTR->ArmorInfo().SpecificName()
                         == armor::ArmorTypeWrapper::GLOVES_NAME_),
                        makeErrorReportPrefix()
                            << "gauntlets with primary material "
                            << material::ToString(ITEM_PTR->MaterialPrimary())
                            << " which means gloves but all the names did not equal \""
                            << armor::ArmorTypeWrapper::GLOVES_NAME_
                            << "\".  ArmorTypeWrapper::SpecificName()=\""
                            << ITEM_PTR->ArmorInfo().SpecificName() << "\".");
                }
                else
                {
                    M_HP_ASSERT_OR_LOG_AND_THROW(
                        (ITEM_PTR->ArmorInfo().SpecificName()
                         != armor::ArmorTypeWrapper::GLOVES_NAME_),
                        makeErrorReportPrefix() << "gauntlets with primary material "
                                                << material::ToString(ITEM_PTR->MaterialPrimary())
                                                << " which means gauntlets and NOT gloves but the "
                                                   "ArmorTypeWrapper::SpecificName()=\""
                                                << ITEM_PTR->ArmorInfo().SpecificName() << "\".");
                }
            }

            if (ITEM_PTR->ArmorType() != armor_type::Skin)
            {
                const auto IS_SPECIFIC_ARMOR_TYPE { (
                    (ITEM_PTR->ArmorType() == armor_type::Covering)
                    || (ITEM_PTR->ArmorType() == armor_type::Helm)
                    || (ITEM_PTR->ArmorType() == armor_type::Shield)) };

                if (IS_SPECIFIC_ARMOR_TYPE)
                {
                    M_HP_ASSERT_OR_LOG_AND_THROW(
                        (ITEM_PTR->ArmorInfo().BaseType() == armor::base_type::Count),
                        makeErrorReportPrefix()
                            << "armor is specific armor_type="
                            << armor_type::ToString(ITEM_PTR->ArmorType())
                            << " but had an invalid base_type="
                            << armor::base_type::ToString(ITEM_PTR->ArmorInfo().BaseType()) << ".");
                }
                else
                {
                    M_HP_ASSERT_OR_LOG_AND_THROW(
                        (ITEM_PTR->ArmorInfo().BaseType() != armor::base_type::Count),
                        makeErrorReportPrefix()
                            << "armor is non-specific armor_type="
                            << armor_type::ToString(ITEM_PTR->ArmorType())
                            << " but had a base_type="
                            << armor::base_type::ToString(ITEM_PTR->ArmorInfo().BaseType()) << ".");

                    if (ITEM_PTR->ArmorInfo().BaseType() == armor::base_type::Plain)
                    {
                        M_HP_ASSERT_OR_LOG_AND_THROW(
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
                        M_HP_ASSERT_OR_LOG_AND_THROW(
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
                        M_HP_ASSERT_OR_LOG_AND_THROW(
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

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (ITEM_PTR->Weight() >= 1_weight),
            makeErrorReportPrefix() << "weight (" << ITEM_PTR->Weight() << ") was less than one.");

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (ITEM_PROFILE.WeaponType() == ITEM_PTR->WeaponType()),
            makeErrorReportPrefix()
                << " weapon_types did not match.  profile="
                << weapon_type::ToString(ITEM_PROFILE.WeaponType(), EnumStringHow(Wrap::Yes))
                << " but item="
                << weapon_type::ToString(ITEM_PTR->WeaponType(), EnumStringHow(Wrap::Yes)) << ".");

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (ITEM_PROFILE.ArmorType() == ITEM_PTR->ArmorType()),
            makeErrorReportPrefix()
                << " armor_types did not match.  profile="
                << armor_type::ToString(ITEM_PROFILE.ArmorType())
                << " but item=" << armor_type::ToString(ITEM_PTR->ArmorType()) << ".");

        const bool IS_ITEM_EQUIPPABLE { (ITEM_PTR->Category() & category::Equipable) > 0 };

        if (IS_ITEM_EQUIPPABLE)
        {
            const auto IS_WEARABLE { (ITEM_PTR->Category() & category::Wearable) > 0 };
            const auto IS_ONE_HANDED { (ITEM_PTR->Category() & category::OneHanded) > 0 };
            const auto IS_TWO_HANDED { (ITEM_PTR->Category() & category::TwoHanded) > 0 };

            const auto EQUIP_TYPE_COUNT { ((IS_WEARABLE) ? 1 : 0) + ((IS_ONE_HANDED) ? 1 : 0)
                                          + ((IS_TWO_HANDED) ? 1 : 0) };

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (EQUIP_TYPE_COUNT == 1),
                makeErrorReportPrefix()
                    << "equipable but not one and only one of the three equipable types:"
                       "is_wearable="
                    << std::boolalpha << IS_WEARABLE << ", is_one_handed=" << IS_ONE_HANDED
                    << ", is_two_handed=" << IS_TWO_HANDED << ".");
        }
        else
        {
            M_HP_ASSERT_OR_LOG_AND_THROW(
                ((ITEM_PTR->Category() & category::Wearable) == false),
                makeErrorReportPrefix() << "not equipable but is wearable.");

            M_HP_ASSERT_OR_LOG_AND_THROW(
                ((ITEM_PTR->Category() & category::OneHanded) == false),
                makeErrorReportPrefix() << "not equipable but is one-handed.");

            M_HP_ASSERT_OR_LOG_AND_THROW(
                ((ITEM_PTR->Category() & category::TwoHanded) == false),
                makeErrorReportPrefix() << "not equipable but is two-handed.");
        }

        if (ITEM_PTR->IsMisc())
        {
            const bool IS_MISCTYPE_EQUIPPABLE {
                (misc_type::EquipCategory(ITEM_PTR->MiscType()) & category::Equipable) > 0
            };

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (IS_MISCTYPE_EQUIPPABLE == IS_ITEM_EQUIPPABLE),
                makeErrorReportPrefix()
                    << "equipable, but the misc_type=" << misc_type::ToString(ITEM_PTR->MiscType())
                    << " is NOT equipable.  misc_type_equip_category="
                    << category::ToString(misc_type::EquipCategory(ITEM_PTR->MiscType())));

            // unique_types cannot be set
            if (misc_type::IsUnique(ITEM_PTR->MiscType()))
            {
                M_HP_ASSERT_OR_LOG_AND_THROW(
                    (ITEM_PTR->IsSet() == false),
                    makeErrorReportPrefix() << "misc_type unique but also set_type.");
            }

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (misc_type::IsWeapon(ITEM_PTR->MiscType()) == ITEM_PTR->IsWeapon()),
                makeErrorReportPrefix()
                    << "misc_type::IsWeapon()=" << misc_type::IsWeapon(ITEM_PTR->MiscType())
                    << " but ITEM_PTR->IsWeapon()=" << ITEM_PTR->IsWeapon() << ".");

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (misc_type::IsArmor(ITEM_PTR->MiscType()) == ITEM_PTR->IsArmor()),
                makeErrorReportPrefix()
                    << "misc_type::IsArmor()=" << misc_type::IsArmor(ITEM_PTR->MiscType())
                    << " but ITEM_PTR->IsArmor()=" << ITEM_PTR->IsArmor()
                    << " -these should match.");

            const auto IS_QUEST { misc_type::IsQuestItem(ITEM_PTR->MiscType()) };
            const auto IS_UNIQUE { misc_type::IsUnique(ITEM_PTR->MiscType()) };
            const auto IS_STANDALONE { misc_type::IsStandalone(ITEM_PTR->MiscType()) };

            const auto MISC_CATEGORY_COUNT { ((IS_QUEST) ? 1 : 0) + ((IS_UNIQUE) ? 1 : 0)
                                             + ((IS_STANDALONE) ? 1 : 0) };

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (MISC_CATEGORY_COUNT == 1),
                makeErrorReportPrefix()
                    << "misc_type was not one and only one of the three "
                       "misc_type sub-categories:  IsQuestItem="
                    << std::boolalpha << IS_QUEST << ", IsUnique()=" << IS_UNIQUE
                    << ", IsStandalone()=" << IS_STANDALONE << ".");
        }

        if (ITEM_PTR->IsWeapon())
        {
            M_HP_ASSERT_OR_LOG_AND_THROW(
                (ITEM_PTR->WeaponInfo().IsTypeValid()),
                makeErrorReportPrefix() << "IsWeapon() but WeaponInfo().IsValid()==false.");

            int typeCounter { 0 };
            auto incrementIf { [&](const bool WILL_INCREMENT) {
                if (WILL_INCREMENT)
                {
                    ++typeCounter;
                }
            } };

            incrementIf(ITEM_PTR->WeaponInfo().IsBodyPart());
            incrementIf(ITEM_PTR->WeaponInfo().IsBlowpipe());
            incrementIf(ITEM_PTR->WeaponInfo().HasHandle());
            incrementIf(ITEM_PTR->WeaponInfo().IsPole());

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (1 == typeCounter),
                makeErrorReportPrefix()
                    << "weapon was not one of four states thought to be complete and mutually "
                       "exclusive:  IsBodyPart()="
                    << std::boolalpha << ITEM_PTR->WeaponInfo().IsBodyPart()
                    << ", IsBlowpipe()=" << ITEM_PTR->WeaponInfo().IsBlowpipe()
                    << ", HasSmallHandle()=" << ITEM_PTR->WeaponInfo().HasHandle()
                    << ", HasLargeHandle()=" << ITEM_PTR->WeaponInfo().IsPole() << ".");

            M_HP_ASSERT_OR_LOG_AND_THROW(
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

        const auto IS_MISC { ITEM_PTR->IsMisc() };
        const auto IS_NONMISC_WEAPON { ITEM_PTR->IsWeapon() && (ITEM_PTR->IsMisc() == false) };
        const auto IS_NONMISC_ARMOR { ITEM_PTR->IsArmor() && (ITEM_PTR->IsMisc() == false) };

        if (IS_MISC)
        {
            M_HP_ASSERT_OR_LOG_AND_THROW(
                ((IS_NONMISC_WEAPON == false) && (IS_NONMISC_ARMOR == false)),
                makeErrorReportPrefix()
                    << "(misc=" << IS_MISC << ", nonmisc_weapon=" << IS_NONMISC_WEAPON
                    << ", nonmisc_armor=" << IS_NONMISC_ARMOR << ") is an invalid combination.");
        }
        else if (IS_NONMISC_WEAPON)
        {
            M_HP_ASSERT_OR_LOG_AND_THROW(
                ((IS_MISC == false) && (IS_NONMISC_ARMOR == false)),
                makeErrorReportPrefix()
                    << "(misc=" << IS_MISC << ", nonmisc_weapon=" << IS_NONMISC_WEAPON
                    << ", nonmisc_armor=" << IS_NONMISC_ARMOR << ") is an invalid combination.");
        }
        else if (IS_NONMISC_ARMOR)
        {
            M_HP_ASSERT_OR_LOG_AND_THROW(
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

    const ItemPtr_t ItemFactory::Make(const ItemProfile & PROFILE) const
    {
        // Note that some misc_type items are also weapons or armor.  In these cases item
        // construction is handled by WeaponFactory or ArmorFactory.
        if (PROFILE.IsArmor())
        {
            return MakeArmor(PROFILE);
        }
        else if (PROFILE.IsWeapon())
        {
            return MakeWeapon(PROFILE);
        }
        else if (PROFILE.IsMisc())
        {
            return MakeMisc(PROFILE);
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
        const body_part::Enum BODY_PART, const creature::CreaturePtr_t CREATURE_PTR) const
    {
        if (BODY_PART == body_part::Skin)
        {
            return MakeArmor(BODY_PART, CREATURE_PTR);
        }
        else
        {
            return MakeWeapon(BODY_PART, CREATURE_PTR);
        }
    }

    const ItemPtr_t ItemFactory::MakeArmor(const ItemProfile & PROFILE) const
    {
        const auto & ARMOR_TYPE_WRAPPER { PROFILE.ArmorInfo() };

        const auto ARMOR_DETAILS { armor::ArmorDetailLoader::LookupArmorDetails(
            ARMOR_TYPE_WRAPPER.DetailsKeyName()) };

        return ItemWarehouse::Access().Store(std::make_unique<Item>(
            nameFactory_.MakeNonBodyPartName(PROFILE),
            nameFactory_.MakeNonBodyPartDescription(PROFILE, ARMOR_DETAILS.description),
            PROFILE.Category(),
            PROFILE.MaterialPrimary(),
            PROFILE.MaterialSecondary(),
            CalculatePrice(PROFILE, ARMOR_DETAILS.price),
            CalculateWeight(PROFILE, ARMOR_DETAILS.weight),
            0_health,
            0_health,
            CalculateArmorRating(PROFILE, ARMOR_DETAILS.armor_rating),
            TypeWrapper(PROFILE),
            PROFILE.WeaponInfo(),
            ARMOR_TYPE_WRAPPER,
            PROFILE.IsPixie(),
            PROFILE.RoleRestriction()));
    }

    const ItemPtr_t ItemFactory::MakeArmor(
        const body_part::Enum BODY_PART, const creature::CreaturePtr_t CREATURE_PTR) const
    {
        // TODO move to MaterialFactory
        const auto MATERIALS_PAIR { material::SkinMaterial(CREATURE_PTR->Race()) };

        M_HP_ASSERT_OR_LOG_AND_THROW(
            ((MATERIALS_PAIR.first != material::Count)
             && (MATERIALS_PAIR.first != material::Nothing)),
            "item::armor::ArmorFactory::Make(body_part="
                << ((BODY_PART == body_part::Count) ? "Count" : body_part::ToString(BODY_PART))
                << ", creature={" << CREATURE_PTR->ToString()
                << "}) but that creature's skin material (material::SkinMaterial()) was "
                   "invalid: pri="
                << ((MATERIALS_PAIR.first == material::Count)
                        ? "Count"
                        : material::ToString(MATERIALS_PAIR.first))
                << ", sec="
                << ((MATERIALS_PAIR.second == material::Count)
                        ? "Count"
                        : material::ToString(MATERIALS_PAIR.second))
                << ".");

        const armor::ArmorTypeWrapper ARMOR_TYPE_WRAPPER { BODY_PART };

        const auto ARMOR_RATING { Armor_t::Make(
            material::ArmorRatingBonusPri(MATERIALS_PAIR.first).As<int>()
            + CREATURE_PTR->Rank().As<int>()) };

        const auto WEIGHT { Weight_t::Make(
            10.0f * material::WeightMult(MATERIALS_PAIR.first, MATERIALS_PAIR.second)) };

        return ItemWarehouse::Access().Store(std::make_unique<Item>(
            nameFactory_.MakeArmorBodyPartName(MATERIALS_PAIR, CREATURE_PTR),
            nameFactory_.MakeArmorBodyPartDescription(MATERIALS_PAIR),
            static_cast<category::Enum>(category::Equipable | category::BodyPart),
            MATERIALS_PAIR.first,
            MATERIALS_PAIR.second,
            0_coin,
            WEIGHT,
            0_health,
            0_health,
            ARMOR_RATING,
            TypeWrapper(),
            weapon::WeaponTypeWrapper(),
            ARMOR_TYPE_WRAPPER,
            CREATURE_PTR->IsPixie()));
    }

    const ItemPtr_t ItemFactory::MakeWeapon(const ItemProfile & PROFILE) const
    {
        const auto & WEAPON_TYPE_WRAPPER { PROFILE.WeaponInfo() };

        const auto WEAPON_DETAILS { weapon::WeaponDetailLoader::LookupWeaponDetails(
            WEAPON_TYPE_WRAPPER.DetailsKeyName()) };

        return ItemWarehouse::Access().Store(std::make_unique<Item>(
            nameFactory_.MakeNonBodyPartName(PROFILE),
            nameFactory_.MakeNonBodyPartDescription(PROFILE, WEAPON_DETAILS.description),
            PROFILE.Category(),
            PROFILE.MaterialPrimary(),
            PROFILE.MaterialSecondary(),
            CalculatePrice(PROFILE, WEAPON_DETAILS.price),
            CalculateWeight(PROFILE, WEAPON_DETAILS.weight),
            WEAPON_DETAILS.damage_min,
            WEAPON_DETAILS.damage_max,
            0_armor,
            TypeWrapper(PROFILE),
            WEAPON_TYPE_WRAPPER,
            PROFILE.ArmorInfo(),
            PROFILE.IsPixie(),
            PROFILE.RoleRestriction()));
    }

    const ItemPtr_t ItemFactory::MakeWeapon(
        const body_part::Enum BODY_PART, const creature::CreaturePtr_t CREATURE_PTR) const
    {
        const weapon::WeaponTypeWrapper WEAPON_TYPE_WRAPPER(BODY_PART);

        if (BODY_PART == body_part::Breath)
        {
            M_HP_ASSERT_OR_LOG_AND_THROW(
                ((CREATURE_PTR->Role() == creature::role::Sylavin)
                 || (CREATURE_PTR->Role() == creature::role::Firebrand)),
                "WeaponFactory::Make(body_part=Breath, creature=\n{"
                    << CREATURE_PTR->ToString()
                    << "}\nbody part was breath but the creature's role was not Sylavin or "
                       "Firebrand.");
        }

        const auto WEAPON_DETAILS_NAME { (
            (body_part::Breath == BODY_PART)
                ? (WEAPON_TYPE_WRAPPER.DetailsKeyName()
                   + creature::role::ToString(CREATURE_PTR->Role()))
                : WEAPON_TYPE_WRAPPER.DetailsKeyName()) };

        const weapon::WeaponDetails WEAPON_DETAILS {
            weapon::WeaponDetailLoader::LookupWeaponDetails(WEAPON_DETAILS_NAME)
        };

        // TODO move into MaterialsFactory
        const auto MATERIALS { [BODY_PART]() {
            if (BODY_PART == body_part::Bite)
            {
                return std::make_pair(material::Tooth, material::Nothing);
            }
            else if (BODY_PART == body_part::Breath)
            {
                return std::make_pair(material::Gas, material::Nothing);
            }
            else if (BODY_PART == body_part::Claws)
            {
                return std::make_pair(material::Claw, material::Nothing);
            }
            else if (BODY_PART == body_part::Fists)
            {
                return std::make_pair(material::Bone, material::Hide);
            }
            else // tentacles
            {
                return std::make_pair(material::Plant, material::Nothing);
            }
        }() };

        return ItemWarehouse::Access().Store(std::make_unique<Item>(
            nameFactory_.MakeWeaponBodyPartName(CREATURE_PTR, WEAPON_TYPE_WRAPPER.ReadableName()),
            nameFactory_.MakeWeaponBodyPartDescription(WEAPON_DETAILS.description, CREATURE_PTR),
            ItemProfile::CategoryWeaponBodypart(BODY_PART),
            MATERIALS.first,
            MATERIALS.second,
            WEAPON_DETAILS.price,
            WEAPON_DETAILS.weight,
            WEAPON_DETAILS.damage_min,
            WEAPON_DETAILS.damage_max,
            0_armor,
            TypeWrapper(),
            WEAPON_TYPE_WRAPPER,
            armor::ArmorTypeWrapper(),
            CREATURE_PTR->IsPixie()));
    }

    const ItemPtr_t ItemFactory::MakeMisc(const ItemProfile & PROFILE) const
    {
        return ItemWarehouse::Access().Store(std::make_unique<Item>(
            // "" is okay in both places because there is special misc_type logic inside
            nameFactory_.MakeNonBodyPartName(PROFILE),
            nameFactory_.MakeNonBodyPartDescription(PROFILE, PROFILE.ReadableName()),
            PROFILE.Category(),
            PROFILE.MaterialPrimary(),
            PROFILE.MaterialSecondary(),
            // zero is okay in both places because there is special misc_type logic inside
            CalculatePrice(PROFILE),
            CalculateWeight(PROFILE),
            0_health,
            0_health,
            0_armor,
            TypeWrapper(PROFILE),
            PROFILE.WeaponInfo(),
            PROFILE.ArmorInfo(),
            PROFILE.IsPixie(),
            PROFILE.RoleRestriction()));
    }

    Coin_t ItemFactory::CalculatePrice(
        const ItemProfile & PROFILE, const Coin_t BASE_PRICE_PARAM) const
    {
        const auto BASE_PRICE_FINAL { (
            (BASE_PRICE_PARAM > 0_coin) ? BASE_PRICE_PARAM
                                        : TreasureScoreToCoins(PROFILE.TreasureScore())) };

        Coin_t price { BASE_PRICE_FINAL
                       + material::PriceAdj(
                           PROFILE.MaterialPrimary(), PROFILE.MaterialSecondary()) };

        if (PROFILE.IsPixie())
        {
            price = Coin_t::Make(price.As<float>() * 1.5f);
        }

        return price;
    }

    Weight_t ItemFactory::CalculateWeight(
        const ItemProfile & PROFILE, const Weight_t BASE_WEIGHT_PARAM) const
    {
        auto weight { (
            (BASE_WEIGHT_PARAM > 0_weight) ? BASE_WEIGHT_PARAM
                                           : misc_type::Weight(PROFILE.MiscType())) };

        weight = Weight_t::Make(
            weight.As<float>()
            * material::WeightMult(PROFILE.MaterialPrimary(), PROFILE.MaterialSecondary()));

        if (PROFILE.IsPixie())
        {
            weight /= 250_weight;
        }

        if (weight < 1_weight)
        {
            weight = 1_weight;
        }

        return weight;
    }

    Armor_t ItemFactory::CalculateArmorRating(
        const ItemProfile & PROFILE, const Armor_t & BASE_ARMOR_RATING) const
    {
        // armor items with a clasp will not consider the secondary material in terms of armor
        // rating because that is just the material of the clasp

        return BASE_ARMOR_RATING
            + material::ArmorRatingBonus(
                   PROFILE.MaterialPrimary(),
                   (((PROFILE.NameMaterialType() == name_material_type::Clasped)
                     || (PROFILE.NameMaterialType() == name_material_type::ClaspedWithBase))
                        ? material::Nothing
                        : PROFILE.MaterialSecondary()));
    }

    Coin_t ItemFactory::TreasureScoreToCoins(const Score_t & TREASURE_SCORE) const
    {
        // For now Treasure Score equals the price in coins
        return Coin_t::Make(TREASURE_SCORE);
    }

} // namespace item
} // namespace heroespath
