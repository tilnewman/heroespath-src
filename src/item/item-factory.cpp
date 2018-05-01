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
#include "sfml-util/gui/item-image-machine.hpp"

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

        static auto const & NORMAL_PROFILES{
            ItemProfileWarehouse::Instance()->GetNormalProfiles()
        };

        static auto const & RELIGIOUS_PROFILES{
            ItemProfileWarehouse::Instance()->GetReligiousProfiles()
        };

        static auto didLogTotalItemProfileCount{ false };
        if (false == didLogTotalItemProfileCount)
        {
            didLogTotalItemProfileCount = true;

            std::ostringstream totalCountReportSS;

            totalCountReportSS
                << "item::ItemFactory::Test() ItemProfile creation finished.  Created "
                << NORMAL_PROFILES.size() + RELIGIOUS_PROFILES.size() << " profiles.";

            game::LoopManager::Instance()->TestingStrAppend(totalCountReportSS.str());
        }

        static item::ItemProfileVec_t duplicates;

        static auto hasTestedForDuplicatesNormal{ false };
        if (false == hasTestedForDuplicatesNormal)
        {
            game::LoopManager::Instance()->TestingStrAppend(
                "item::ItemFactory::Test() Starting Normal Profiles Duplicate Test.  Please "
                "wait...");

            for (auto iter(std::begin(NORMAL_PROFILES)); iter != std::end(NORMAL_PROFILES); ++iter)
            {
                auto const NEXT_ITER{ std::next(iter, 1) };
                if (NEXT_ITER == std::end(NORMAL_PROFILES))
                {
                    break;
                }
                else
                {
                    if ((*iter) == (*NEXT_ITER))
                    {
                        M_HP_LOG_ERR(
                            "item::ItemFactory::Test() Normal Profiles Duplicate Test failed for "
                            "profile1={"
                            << iter->ToString() << "} and profile2=" << NEXT_ITER->ToString()
                            << "}.");
                    }
                }
            }

            game::LoopManager::Instance()->TestingStrAppend(
                "item::ItemFactory::Test() Normal Profiles Duplicate Test finished.");

            hasTestedForDuplicatesNormal = true;
            return false;
        }

        static auto hasTestedForDuplicatesReligious{ false };
        if (false == hasTestedForDuplicatesReligious)
        {
            game::LoopManager::Instance()->TestingStrAppend(
                "item::ItemFactory::Test() Starting Religious Profiles Duplicate Test.  Please "
                "wait...");

            for (auto iter(std::begin(RELIGIOUS_PROFILES)); iter != std::end(RELIGIOUS_PROFILES);
                 ++iter)
            {
                auto const NEXT_ITER{ std::next(iter, 1) };
                if (NEXT_ITER == std::end(RELIGIOUS_PROFILES))
                {
                    break;
                }
                else
                {
                    M_ASSERT_OR_LOGANDTHROW_SS(
                        ((*iter) != (*NEXT_ITER)),
                        "item::ItemFactory::Test() Religious Profiles Duplicate Test failed for "
                        "profile1={"
                            << iter->ToString() << "} and profile2=" << NEXT_ITER->ToString()
                            << "}.");
                }
            }

            game::LoopManager::Instance()->TestingStrAppend(
                "item::ItemFactory::Test() Religious Profiles Duplicate Test finished.");

            hasTestedForDuplicatesReligious = true;
            return false;
        }

        static const std::size_t REPORT_STATUS_EVERY{ 10000 };
        static std::size_t testIndex{ 0 };

        static auto didTestNormal{ false };
        if (false == didTestNormal)
        {
            for (; testIndex < NORMAL_PROFILES.size();)
            {
                auto const & PROFILE{ NORMAL_PROFILES[testIndex] };

                auto itemPtr{ Make(PROFILE) };
                TestItem(itemPtr, PROFILE);
                ItemWarehouse::Access().Free(itemPtr);

                if ((++testIndex % REPORT_STATUS_EVERY) == 0)
                {
                    std::ostringstream statusReportSS;
                    statusReportSS
                        << "item::ItemFactory::Test() Making and Testing each normal item.  "
                        << std::setprecision(2)
                        << 100.0
                            * (static_cast<double>(testIndex)
                               / static_cast<double>(NORMAL_PROFILES.size()))
                        << "%.  Please wait...";

                    game::LoopManager::Instance()->TestingStrAppend(statusReportSS.str());

                    return false;
                }
            }

            testIndex = 0;
            didTestNormal = true;
            return false;
        }

        static auto didTestReligious{ false };
        if (false == didTestReligious)
        {
            for (; testIndex < RELIGIOUS_PROFILES.size();)
            {
                auto const & PROFILE{ RELIGIOUS_PROFILES[testIndex] };

                auto itemPtr{ Make(PROFILE) };
                TestItem(itemPtr, PROFILE);
                ItemWarehouse::Access().Free(itemPtr);

                if ((++testIndex % REPORT_STATUS_EVERY) == 0)
                {
                    std::ostringstream statusReportSS;
                    statusReportSS
                        << "item::ItemFactory::Test() Making and Testing each religious item.  "
                        << std::setprecision(2)
                        << 100.0
                            * (static_cast<double>(testIndex)
                               / static_cast<double>(RELIGIOUS_PROFILES.size()))
                        << "%.  Please wait...";

                    game::LoopManager::Instance()->TestingStrAppend(statusReportSS.str());
                    return false;
                }
            }

            didTestReligious = true;
            return false;
        }

        static auto didLogRandomItemsForNameAndDescriptionCheck{ false };
        if (false == didLogRandomItemsForNameAndDescriptionCheck)
        {
            didLogRandomItemsForNameAndDescriptionCheck = true;

            std::ostringstream randNameAndDescSS;
            randNameAndDescSS << "\n\n~~~~~~~~~~~~~~~~~~~~ Random Item Names and Descriptions To "
                                 "Verify ~~~~~~~~~~~~~~~~~~~~";

            auto const COUNT{ 10 };
            for (int i(0); i < COUNT; ++i)
            {
                auto const & PROFILE{ misc::Vector::SelectRandom(NORMAL_PROFILES) };
                auto itemPtr{ Make(PROFILE) };

                randNameAndDescSS << "\n\n\tName=\"" << itemPtr->Name() << "\"\n\tDesc=\""
                                  << itemPtr->Desc() << "\"\n\tItem={" << itemPtr->ToString()
                                  << "}";

                ItemWarehouse::Access().Free(itemPtr);
            }

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
        std::ostringstream ss;
        ss << std::boolalpha << "item::ItemFactory::TestItem(\nitem={" << ITEM_PTR->ToString()
           << "}\nprofile={" << ITEM_PROFILE.ToString() << "})\n *** ERROR: ";

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ITEM_PTR->MaterialPrimary() != ITEM_PTR->MaterialSecondary()),
            ss.str() << "made an item with duplicate materials.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ITEM_PTR->Category() == ITEM_PROFILE.Category()),
            ss.str() << "categories did not match.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ITEM_PTR->MiscType() == ITEM_PROFILE.MiscType()),
            ss.str() << "misc_types did not match.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ITEM_PTR->MiscType() != misc_type::Count), ss.str() << "misc_type was Count.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ITEM_PTR->SetType() == ITEM_PROFILE.SetType()),
            ss.str() << "set_types did not match.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ITEM_PTR->SetType() != set_type::Count), ss.str() << "set_type was Count.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ITEM_PTR->ElementType() == ITEM_PROFILE.ElementType()),
            ss.str() << "element_types did not match.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (element_type::IsValid(ITEM_PTR->ElementType())), ss.str() << "invalid element_type.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ITEM_PTR->NamedType() == ITEM_PROFILE.NamedType()),
            ss.str() << "named_types did not match.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ITEM_PTR->NamedType() != named_type::Count), ss.str() << "named_type was Count.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ITEM_PTR->UniqueType() == ITEM_PROFILE.UniqueType()),
            ss.str() << "unique_types did not match.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ITEM_PTR->UniqueType() != unique_type::Count), ss.str() << "unique_type was Count.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ITEM_PTR->MaterialPrimary() == ITEM_PROFILE.MaterialPrimary()),
            ss.str() << "primary materials did not match.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            ((ITEM_PTR->MaterialPrimary() != material::Count)
             && (ITEM_PTR->MaterialPrimary() != material::Nothing)),
            ss.str() << "primary material was either Count or Nothing.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ITEM_PTR->MaterialSecondary() == ITEM_PROFILE.MaterialSecondary()),
            ss.str() << "secondary materials did not match.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ITEM_PTR->MaterialSecondary() != material::Count),
            ss.str() << "secondary material was Count instead of Nothing.");

        if (ITEM_PTR->IsWeapon() || ITEM_PTR->IsArmor())
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                ((ITEM_PTR->IsWeapon()) != (ITEM_PTR->IsArmor())),
                ss.str() << "both weapon and armor at the same time.");
        }

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ITEM_PTR->IsBodypart() == false),
            ss.str() << "bodypart, but the ItemProfileWarehouse should not create any bodypart "
                        "items since they cannot be found in treasure chests.");

        M_ASSERT_OR_LOGANDTHROW_SS((ITEM_PTR->IsBroken() == false), ss.str() << "broken.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ITEM_PTR->ImageFilename().empty() == false), ss.str() << "no/empty image filename.");

        sfml_util::gui::ItemImageMachine itemImageMachine;
        itemImageMachine.EnsureFileExists(ITEM_PTR);

        if (ITEM_PTR->ArmorInfo().HelmType() == armor::helm_type::Leather)
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (ITEM_PTR->MaterialPrimary() == material::HardLeather),
                ss.str() << "Leather Helm had a primary material that was not hard leather.");
        }

        if (ITEM_PTR->WeaponInfo().WhipType() == weapon::whip_type::Bullwhip)
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (ITEM_PTR->MaterialPrimary() == material::HardLeather),
                ss.str() << "bull whip with a primary material that was not hard leather.");
        }

        // every unique_type is also a misc_type
        if (ITEM_PTR->IsUnique())
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (ITEM_PTR->IsMisc()),
                ss.str() << "unique but not misc.  That should never happen.  "
                            "Every unique_type should also be a misc_type.");
        }

        // unique_types cannot be set
        if (ITEM_PTR->IsUnique())
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (ITEM_PTR->IsSet() == false),
                ss.str() << "unique_type but also either element_type or set_type.");
        }

        // set_types cannot be elemental
        if (ITEM_PTR->IsSet())
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (ITEM_PTR->IsElemental() == false),
                ss.str() << "set_type but also either element_type or unique_type.");
        }

        // named_types must be weapons or armor, and cannot be misc
        if (ITEM_PTR->IsNamed())
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                ((ITEM_PTR->IsWeapon() || ITEM_PTR->IsArmor()) && (ITEM_PTR->IsMisc() == false)),
                ss.str() << "named_type but either not weapon(" << ITEM_PTR->IsWeapon()
                         << ")/armor(" << ITEM_PTR->IsArmor() << ") or misc_type("
                         << ITEM_PTR->IsMisc() << ").");
        }

        auto const SUMMON_INFO{ ITEM_PTR->SummonInfo() };

        // all summoning items are also misc items
        if (SUMMON_INFO.CanSummon())
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (ITEM_PTR->IsMisc()),
                ss.str() << "CanSummon() but not misc_type, which should never "
                            "happen.  All summoning items are also misc_type items.");

            M_ASSERT_OR_LOGANDTHROW_SS(
                (misc_type::IsSummoning(ITEM_PTR->MiscType())),
                ss.str() << "CanSummon() but not misc_type::IsSummoning().");
        }

        // invalid summoning info
        if (SUMMON_INFO.IsDefaultAndCompletelyInvalid() == false)
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (SUMMON_INFO.CanSummon()), ss.str() << "summoning but summoning_info was invalid.");
        }

        // if summoning type then must be able to summon, and vice versa
        M_ASSERT_OR_LOGANDTHROW_SS(
            (item::misc_type::IsSummoning(ITEM_PTR->MiscType()) == SUMMON_INFO.CanSummon()),
            ss.str() << "misc_type=summoning_type but it is not able to summon.");

        // enforce armor base_type restriction
        auto const ARMOR_BASE_TYPE_RESTRICTION{ ITEM_PROFILE.ArmorTypeRestriction() };
        if (ARMOR_BASE_TYPE_RESTRICTION != armor::base_type::Count)
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (ITEM_PTR->IsArmor()),
                ss.str() << "the profile specified an armor base_type restriction of "
                         << armor::base_type::ToString(ARMOR_BASE_TYPE_RESTRICTION)
                         << ", but the item created as not armor.");

            M_ASSERT_OR_LOGANDTHROW_SS(
                (ITEM_PTR->ArmorInfo().BaseType() == ARMOR_BASE_TYPE_RESTRICTION),
                ss.str() << "the profile specified an armor base_type restriction of "
                         << armor::base_type::ToString(ARMOR_BASE_TYPE_RESTRICTION)
                         << ", but the item created had an armor base_type of "
                         << ((ITEM_PTR->ArmorInfo().BaseType() == armor::base_type::Count)
                                 ? "Count"
                                 : armor::base_type::ToString(ITEM_PTR->ArmorInfo().BaseType()))
                         << ".");
        }

        // all items must be weapon, armor, or misc
        M_ASSERT_OR_LOGANDTHROW_SS(
            (ITEM_PTR->IsWeapon() || ITEM_PTR->IsArmor()
             || (ITEM_PTR->MiscType() != misc_type::NotMisc)),
            ss.str() << "the item was not a weapon, armor, or misc_type.  All items must be one of "
                        "these three.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ITEM_PTR->BaseName().empty() == false),
            ss.str() << "BaseName (\"" << ITEM_PTR->BaseName() << "\") was empty or invalid.");

        if (ITEM_PTR->IsWeapon())
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (ITEM_PTR->WeaponInfo().IsTypeValid()),
                ss.str() << "IsWeapon() but WeaponInfo().IsValid()==false.");
        }

        if (ITEM_PTR->IsArmor())
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (ITEM_PTR->ArmorInfo().IsTypeValid()),
                ss.str() << "IsArmor() but ArmorInfo().IsValid()==false.");
        }

        if (ITEM_PTR->ArmorType() == armor_type::Gauntlets)
        {
            if ((ITEM_PTR->MaterialPrimary() == material::Cloth)
                || (ITEM_PTR->MaterialPrimary() == material::SoftLeather)
                || (ITEM_PTR->MaterialPrimary() == material::HardLeather))
            {
                M_ASSERT_OR_LOGANDTHROW_SS(
                    (ITEM_PTR->ArmorInfo().SpecificName() == armor::ArmorTypeWrapper::GLOVES_NAME_),
                    ss.str() << "gauntles with primary material " << ITEM_PTR->MaterialPrimary()
                             << " which means gloves but all the names did not equal \""
                             << armor::ArmorTypeWrapper::GLOVES_NAME_
                             << "\".  ArmorTypeWrapper::SpecificName()=\""
                             << ITEM_PTR->ArmorInfo().SpecificName() << "\".");

                M_ASSERT_OR_LOGANDTHROW_SS(
                    (ITEM_PTR->ArmorInfo().SpecificName() == armor::ArmorTypeWrapper::GLOVES_NAME_),
                    ss.str() << "gauntles with primary material " << ITEM_PTR->MaterialPrimary()
                             << " which means gloves but all the names did not equal \""
                             << armor::ArmorTypeWrapper::GLOVES_NAME_
                             << "\".  ArmorTypeWrapper::SpecificName()=\""
                             << ITEM_PTR->ArmorInfo().SpecificName() << "\".");
            }
            else
            {
                M_ASSERT_OR_LOGANDTHROW_SS(
                    (ITEM_PTR->ArmorInfo().SpecificName() != armor::ArmorTypeWrapper::GLOVES_NAME_),
                    ss.str() << "gauntles with primary material " << ITEM_PTR->MaterialPrimary()
                             << " which means gauntlets and NOT gloves but the "
                                "ArmorTypeWrapper::SpecificName()=\""
                             << ITEM_PTR->ArmorInfo().SpecificName() << "\".");
            }
        }

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ITEM_PTR->Weight() >= 1_weight),
            ss.str() << "weight (" << ITEM_PTR->Weight() << ") was less than one.");

        if (ITEM_PTR->WeaponType() != weapon_type::NotAWeapon)
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (ITEM_PTR->WeaponInfo().SingleType() & ITEM_PTR->WeaponType()),
                ss.str() << " WeaponType()=" << weapon_type::ToString(ITEM_PTR->WeaponType(), true)
                         << " but ITEM_PTR->WeaponInfo().Type()="
                         << weapon_type::ToString(ITEM_PTR->WeaponInfo().SingleType(), true)
                         << " is not included in that.");
        }

        if (ITEM_PTR->IsMisc())
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (misc_type::IsWeapon(ITEM_PTR->MiscType()) == ITEM_PTR->IsWeapon()),
                ss.str() << "misc_type::IsWeapon()=" << misc_type::IsWeapon(ITEM_PTR->MiscType())
                         << " but ITEM_PTR->IsWeapon()=" << ITEM_PTR->IsWeapon() << ".");

            M_ASSERT_OR_LOGANDTHROW_SS(
                (misc_type::IsArmor(ITEM_PTR->MiscType()) == ITEM_PTR->IsArmor()),
                ss.str() << "misc_type::IsArmor()=" << misc_type::IsArmor(ITEM_PTR->MiscType())
                         << " but ITEM_PTR->IsArmor()=" << ITEM_PTR->IsArmor()
                         << " -these should match.");

            const bool IS_ITEM_EQUIPPABLE{ (ITEM_PTR->Category() & category::Equippable) != 0 };
            const bool IS_TYPE_EQUIPPABLE{ misc_type::IsEquippable(ITEM_PTR->MiscType()) };

            M_ASSERT_OR_LOGANDTHROW_SS(
                (IS_TYPE_EQUIPPABLE == IS_ITEM_EQUIPPABLE),
                ss.str() << "misc_type::IsEquippable(" << misc_type::ToString(ITEM_PTR->MiscType())
                         << ")=" << IS_TYPE_EQUIPPABLE
                         << " != (ITEM_PTR->Category() & category::Equippable)="
                         << IS_ITEM_EQUIPPABLE << ".");
        }

        auto const IS_MISC{ ITEM_PTR->IsMisc() };
        auto const IS_NONMISC_WEAPON{ ITEM_PTR->IsWeapon() && (ITEM_PTR->IsMisc() == false) };
        auto const IS_NONMISC_ARMOR{ ITEM_PTR->IsArmor() && (ITEM_PTR->IsMisc() == false) };

        ss << "(misc=" << IS_MISC << ", nonmisc_weapon=" << IS_NONMISC_WEAPON
           << ", nonmisc_armor=" << IS_NONMISC_ARMOR << ") is an invalid combination.";

        if (IS_MISC)
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                ((IS_NONMISC_WEAPON == false) && (IS_NONMISC_ARMOR == false)), ss.str());
        }
        else if (IS_NONMISC_WEAPON)
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                ((IS_MISC == false) && (IS_NONMISC_ARMOR == false)), ss.str());
        }
        else if (IS_NONMISC_ARMOR)
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                ((IS_MISC == false) && (IS_NONMISC_WEAPON == false)), ss.str());
        }
        else
        {
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
