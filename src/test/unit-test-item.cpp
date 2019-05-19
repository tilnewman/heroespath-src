// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// unit-test-item.cpp
//
#define BOOST_TEST_MODULE "item"

#include <boost/test/unit_test.hpp>

#include "game/setup-teardown.hpp"
#include "gui/cached-texture.hpp"
#include "gui/display.hpp"
#include "item/inventory.hpp"
#include "item/item-cache.hpp"
#include "item/item-factory.hpp"
#include "item/item-profile-warehouse.hpp"
#include "item/item-profiles-reporter.hpp"
#include "item/item-warehouse.hpp"
#include "item/item.hpp"
#include "misc/log-macros.hpp"
#include "test/util/common.hpp"
#include "test/util/drawables-displayer.hpp"
#include "test/util/game-engine-global-fixture.hpp"
#include "test/util/single-image-displayer-scoped.hpp"
#include "test/util/single-image-displayer.hpp"

using namespace heroespath;
using namespace heroespath::test;
using namespace heroespath::item;

void GameEngineGlobalFixture::setupBeforeAllTests()
{
    m_unitTestFilename = __FILE__;
    m_subsystemsToSetup = game::SubsystemCollection::TestAll;
    m_iDisplayerUPtr = std::make_unique<SingleImageDisplayer>();
}

BOOST_TEST_GLOBAL_FIXTURE(GameEngineGlobalFixture);

inline void TestItem(const ItemPtr_t & ITEM_PTR, const ItemProfile & ITEM_PROFILE)
{
    BOOST_TEST(
        (ITEM_PTR->MaterialPrimary() != ITEM_PTR->MaterialSecondary()),
        "made an item with duplicate materials.");

    BOOST_TEST((ITEM_PTR->Category() == ITEM_PROFILE.Category()), "categories did not match.");
    BOOST_TEST((ITEM_PTR->MiscType() == ITEM_PROFILE.MiscType()), "misc_types did not match.");
    BOOST_TEST((ITEM_PTR->MiscType() != misc_type::Count), "misc_type was Count.");
    BOOST_TEST((ITEM_PTR->SetType() == ITEM_PROFILE.SetType()), "set_types did not match.");
    BOOST_TEST((ITEM_PTR->SetType() != set_type::Count), "set_type was Count.");

    BOOST_TEST(
        (ITEM_PTR->ElementType() == ITEM_PROFILE.ElementType()), "element_types did not match.");

    BOOST_TEST((EnumUtil<element_type>::IsValid(ITEM_PTR->ElementType())), "invalid element_type.");
    BOOST_TEST((ITEM_PTR->NamedType() == ITEM_PROFILE.NamedType()), "named_types did not match.");
    BOOST_TEST((ITEM_PTR->NamedType() != named_type::Count), "named_type was Count.");

    BOOST_TEST(
        (ITEM_PTR->MaterialPrimary() == ITEM_PROFILE.MaterialPrimary()),
        "primary materials did not match.");

    BOOST_TEST(
        ((ITEM_PTR->MaterialPrimary() != material::Count)
         && (ITEM_PTR->MaterialPrimary() != material::Nothing)),
        "primary material was either Count or Nothing.");

    BOOST_TEST(
        (ITEM_PTR->MaterialSecondary() == ITEM_PROFILE.MaterialSecondary()),
        "secondary materials did not match.");

    BOOST_TEST(
        (ITEM_PTR->MaterialSecondary() != material::Count),
        "secondary material was Count instead of Nothing.");

    if (ITEM_PTR->IsWeapon() || ITEM_PTR->IsArmor())
    {
        BOOST_TEST(
            ((ITEM_PTR->IsWeapon()) != (ITEM_PTR->IsArmor())),
            "both weapon and armor at the same time.");
    }

    BOOST_TEST(
        (!ITEM_PTR->IsBodypart()),
        "bodypart, but the ItemProfileWarehouse should not create any bodypart "
        "items since they cannot be found in treasure chests.");

    BOOST_TEST(!ITEM_PTR->IsBroken(), "broken.");

    const auto & ARMOR_INFO = ITEM_PTR->ArmorInfo();

    if (ARMOR_INFO.HelmType() == armor::helm_type::Leather)
    {
        BOOST_TEST(
            (ITEM_PTR->MaterialPrimary() == material::Leather),
            "Leather Helm had a primary material that was not hard leather.");
    }

    // set_types cannot be elemental
    if (ITEM_PTR->IsSet())
    {
        BOOST_TEST(!ITEM_PTR->IsElemental(), "set_type but also element_type.");
    }

    // named_types must be weapons or armor, and cannot be misc
    if (ITEM_PTR->IsNamed())
    {
        BOOST_TEST(
            ((ITEM_PTR->IsWeapon() || ITEM_PTR->IsArmor()) && !ITEM_PTR->IsMisc()),
            "named_type but either not weapon(" << ITEM_PTR->IsWeapon() << ")/armor("
                                                << ITEM_PTR->IsArmor() << ") or misc_type("
                                                << ITEM_PTR->IsMisc() << ").");
    }

    const auto & SUMMON_INFO = ITEM_PTR->SummonInfo();

    // all summoning items are also misc items
    if (SUMMON_INFO.CanSummon())
    {
        BOOST_TEST(
            ITEM_PTR->IsMisc(),
            "CanSummon() but not misc_type, which should never "
            "happen.  All summoning items are also misc_type items.");

        BOOST_TEST(
            misc_type::IsSummoning(ITEM_PTR->MiscType()),
            "CanSummon() but not misc_type::IsSummoning().");
    }

    // invalid summoning info
    if (!SUMMON_INFO.IsDefaultAndCompletelyInvalid())
    {
        BOOST_TEST(SUMMON_INFO.CanSummon(), "summoning but summoning_info was invalid.");
    }

    // if summoning type then must be able to summon, and vice versa
    BOOST_TEST(
        (misc_type::IsSummoning(ITEM_PTR->MiscType()) == SUMMON_INFO.CanSummon()),
        "misc_type=summoning_type but it is not able to summon.");

    // all items must be weapon, armor, or misc
    BOOST_TEST(
        (ITEM_PTR->IsWeapon() || ITEM_PTR->IsArmor() || (ITEM_PTR->MiscType() != misc_type::Not)),
        "the item was not a weapon, armor, or misc_type.  All items must be one of "
        "these three.");

    BOOST_TEST(
        !ITEM_PTR->BaseName().empty(),
        "BaseName (\"" << ITEM_PTR->BaseName() << "\") was empty or invalid.");

    if (ITEM_PTR->IsArmor())
    {
        BOOST_TEST(ARMOR_INFO.IsTypeValid(), "IsArmor() but ArmorInfo().IsValid()==false.");

        BOOST_TEST(
            ((ARMOR_INFO.NameMaterialType() != name_material_type::Count)
             && (ARMOR_INFO.NameMaterialType() != name_material_type::Misc)),
            "armor had invalid name_material_type="
                << ((ARMOR_INFO.NameMaterialType() == name_material_type::Count)
                        ? "Count"
                        : NAMEOF_ENUM(ARMOR_INFO.NameMaterialType()))
                << ".");

        if (ITEM_PTR->ArmorType() == armor_type::Gauntlets)
        {
            if (material::IsGarmentType(ITEM_PTR->MaterialPrimary()))
            {
                BOOST_TEST(
                    (ARMOR_INFO.SpecificName() == armor::ArmorTypeWrapper::GLOVES_NAME_),
                    "gauntlets with primary material "
                        << NAMEOF_ENUM(ITEM_PTR->MaterialPrimary())
                        << " which means gloves but all the names did not equal \""
                        << armor::ArmorTypeWrapper::GLOVES_NAME_
                        << "\".  ArmorTypeWrapper::SpecificName()=\"" << ARMOR_INFO.SpecificName()
                        << "\".");
            }
            else
            {
                BOOST_TEST(
                    (ARMOR_INFO.SpecificName() != armor::ArmorTypeWrapper::GLOVES_NAME_),
                    "gauntlets with primary material "
                        << NAMEOF_ENUM(ITEM_PTR->MaterialPrimary())
                        << " which means gauntlets and NOT gloves but the "
                           "ArmorTypeWrapper::SpecificName()=\""
                        << ARMOR_INFO.SpecificName() << "\".");
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
                BOOST_TEST(
                    (ARMOR_INFO.BaseType() == armor::base_type::Count),
                    "armor is specific armor_type=" << NAMEOF_ENUM(ITEM_PTR->ArmorType())
                                                    << " but had an invalid base_type="
                                                    << NAMEOF_ENUM(ARMOR_INFO.BaseType()) << ".");
            }
            else
            {
                BOOST_TEST(
                    (ARMOR_INFO.BaseType() != armor::base_type::Count),
                    "armor is non-specific armor_type="
                        << NAMEOF_ENUM(ITEM_PTR->ArmorType())
                        << " but had a base_type=" << NAMEOF_ENUM(ARMOR_INFO.BaseType()) << ".");

                if (ARMOR_INFO.BaseType() == armor::base_type::Plain)
                {
                    BOOST_TEST(
                        ((ITEM_PTR->MaterialPrimary() == material::Cloth)
                         || (ITEM_PTR->MaterialPrimary() == material::Leather)
                         || (ITEM_PTR->MaterialPrimary() == material::Silk)
                         || (ITEM_PTR->MaterialPrimary() == material::Fur)
                         || (ITEM_PTR->MaterialPrimary() == material::Hide)),
                        "armor is non-specific armor_type="
                            << NAMEOF_ENUM(ITEM_PTR->ArmorType())
                            << " with base_type=Plain but the primary material was not one of "
                               "the "
                               "valid 'plain' materials.  It was "
                            << NAMEOF_ENUM(ITEM_PTR->MaterialPrimary()) << ".");
                }
                else if (
                    (ARMOR_INFO.BaseType() == armor::base_type::Mail)
                    || (ARMOR_INFO.BaseType() == armor::base_type::Plate))
                {
                    BOOST_TEST(
                        material::IsMetal(ITEM_PTR->MaterialPrimary()),
                        "armor is non-specific armor_type="
                            << NAMEOF_ENUM(ITEM_PTR->ArmorType())
                            << " with base_type=" << NAMEOF_ENUM(ARMOR_INFO.BaseType())
                            << " but the primary material was not metal.  It was "
                            << NAMEOF_ENUM(ITEM_PTR->MaterialPrimary()) << ".");
                }
                else if (ARMOR_INFO.BaseType() == armor::base_type::Scale)
                {
                    BOOST_TEST(
                        (ITEM_PTR->MaterialPrimary() == material::Scales),
                        "armor is non-specific armor_type="
                            << NAMEOF_ENUM(ITEM_PTR->ArmorType())
                            << " with base_type=Scale but the primary material was not Scales. "
                               " It "
                               "was "
                            << NAMEOF_ENUM(ITEM_PTR->MaterialPrimary()) << ".");
                }
            }
        }
    }

    BOOST_TEST((ITEM_PTR->Weight() >= 1_weight), ") was less than one.");

    BOOST_TEST(
        (ITEM_PROFILE.WeaponType() == ITEM_PTR->WeaponType()),
        " weapon_types did not match.  profile="
            << EnumUtil<weapon_type>::ToString(ITEM_PROFILE.WeaponType(), EnumStringHow(Wrap::Yes))
            << " but item="
            << EnumUtil<weapon_type>::ToString(ITEM_PTR->WeaponType(), EnumStringHow(Wrap::Yes))
            << ".");

    BOOST_TEST(
        (ITEM_PROFILE.ArmorType() == ITEM_PTR->ArmorType()),
        " armor_types did not match.  profile=" << NAMEOF_ENUM(ITEM_PROFILE.ArmorType())
                                                << " but item="
                                                << NAMEOF_ENUM(ITEM_PTR->ArmorType()) << ".");

    const bool IS_ITEM_EQUIPABLE { (ITEM_PTR->Category() & category::Equipable) > 0 };

    if (IS_ITEM_EQUIPABLE)
    {
        const auto IS_WEARABLE { (ITEM_PTR->Category() & category::Wearable) > 0 };
        const auto IS_ONE_HANDED { (ITEM_PTR->Category() & category::OneHanded) > 0 };
        const auto IS_TWO_HANDED { (ITEM_PTR->Category() & category::TwoHanded) > 0 };

        const auto EQUIP_TYPE_COUNT { ((IS_WEARABLE) ? 1 : 0) + ((IS_ONE_HANDED) ? 1 : 0)
                                      + ((IS_TWO_HANDED) ? 1 : 0) };

        BOOST_TEST(
            (EQUIP_TYPE_COUNT == 1),
            "equipable but not one and only one of the three equipable types: is_wearable="
                << std::boolalpha << IS_WEARABLE << ", is_one_handed=" << IS_ONE_HANDED
                << ", is_two_handed=" << IS_TWO_HANDED << ".");
    }
    else
    {
        BOOST_TEST(
            ((ITEM_PTR->Category() & category::Wearable) == false),
            "not equipable but is wearable.");

        BOOST_TEST(
            ((ITEM_PTR->Category() & category::OneHanded) == false),
            "not equipable but is one-handed.");

        BOOST_TEST(
            ((ITEM_PTR->Category() & category::TwoHanded) == false),
            "not equipable but is two-handed.");
    }

    if (ITEM_PTR->IsMisc())
    {
        const bool IS_MISCTYPE_EQUIPABLE {
            (misc_type::EquipCategory(ITEM_PTR->MiscType()) & category::Equipable) > 0
        };

        BOOST_TEST(
            (IS_MISCTYPE_EQUIPABLE == IS_ITEM_EQUIPABLE),
            "equipable, but the misc_type="
                << NAMEOF_ENUM(ITEM_PTR->MiscType())
                << " is NOT equipable.  misc_type_equip_category="
                << EnumUtil<category>::ToString(misc_type::EquipCategory(ITEM_PTR->MiscType())));

        // unique_types cannot be set
        if (misc_type::IsUnique(ITEM_PTR->MiscType()))
        {
            BOOST_TEST(!ITEM_PTR->IsSet(), "misc_type unique but also set_type.");
        }

        BOOST_TEST(
            (misc_type::IsWeapon(ITEM_PTR->MiscType()) == ITEM_PTR->IsWeapon()),
            "misc_type::IsWeapon()=" << misc_type::IsWeapon(ITEM_PTR->MiscType())
                                     << " but ITEM_PTR->IsWeapon()=" << ITEM_PTR->IsWeapon()
                                     << ".");

        BOOST_TEST(
            (misc_type::IsArmor(ITEM_PTR->MiscType()) == ITEM_PTR->IsArmor()),
            "misc_type::IsArmor()=" << misc_type::IsArmor(ITEM_PTR->MiscType())
                                    << " but ITEM_PTR->IsArmor()=" << ITEM_PTR->IsArmor()
                                    << " -these should match.");

        const auto IS_QUEST { misc_type::IsQuestItem(ITEM_PTR->MiscType()) };
        const auto IS_UNIQUE { misc_type::IsUnique(ITEM_PTR->MiscType()) };
        const auto IS_STANDALONE { misc_type::IsStandalone(ITEM_PTR->MiscType()) };

        const auto MISC_CATEGORY_COUNT { ((IS_QUEST) ? 1 : 0) + ((IS_UNIQUE) ? 1 : 0)
                                         + ((IS_STANDALONE) ? 1 : 0) };

        BOOST_TEST(
            (MISC_CATEGORY_COUNT == 1),
            "misc_type was not one and only one of the three misc_type sub-categories:  "
            "IsQuestItem="
                << std::boolalpha << IS_QUEST << ", IsUnique()=" << IS_UNIQUE
                << ", IsStandalone()=" << IS_STANDALONE << ".");
    }

    if (ITEM_PTR->IsWeapon())
    {
        BOOST_TEST(
            (ITEM_PTR->WeaponInfo().IsTypeValid()),
            "IsWeapon() but WeaponInfo().IsValid()==false.");

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

        BOOST_TEST(
            (1 == typeCounter),
            "weapon was not one of four states thought to be complete and mutually exclusive:  "
            "IsBodyPart()="
                << std::boolalpha << ITEM_PTR->WeaponInfo().IsBodyPart()
                << ", IsBlowpipe()=" << ITEM_PTR->WeaponInfo().IsBlowpipe()
                << ", HasSmallHandle()=" << ITEM_PTR->WeaponInfo().HasHandle()
                << ", HasLargeHandle()=" << ITEM_PTR->WeaponInfo().IsPole() << ".");

        BOOST_TEST(
            ((ITEM_PTR->WeaponInfo().NameMaterialType() != name_material_type::Count)
             && (ITEM_PTR->WeaponInfo().NameMaterialType() != name_material_type::Misc)),
            "weapon had invalid name_material_type="
                << ((ITEM_PTR->WeaponInfo().NameMaterialType() == name_material_type::Count)
                        ? "Count"
                        : NAMEOF_ENUM(ITEM_PTR->WeaponInfo().NameMaterialType()))
                << ".");
    }

    const auto IS_MISC { ITEM_PTR->IsMisc() };
    const auto IS_NONMISC_WEAPON { ITEM_PTR->IsWeapon() && (ITEM_PTR->IsMisc() == false) };
    const auto IS_NONMISC_ARMOR { ITEM_PTR->IsArmor() && (ITEM_PTR->IsMisc() == false) };

    if (IS_MISC)
    {
        BOOST_TEST(
            ((IS_NONMISC_WEAPON == false) && (IS_NONMISC_ARMOR == false)),
            "(misc=" << IS_MISC << ", nonmisc_weapon=" << IS_NONMISC_WEAPON
                     << ", nonmisc_armor=" << IS_NONMISC_ARMOR << ") is an invalid combination.");
    }
    else if (IS_NONMISC_WEAPON)
    {
        BOOST_TEST(
            ((IS_MISC == false) && (IS_NONMISC_ARMOR == false)),
            "(misc=" << IS_MISC << ", nonmisc_weapon=" << IS_NONMISC_WEAPON
                     << ", nonmisc_armor=" << IS_NONMISC_ARMOR << ") is an invalid combination.");
    }
    else if (IS_NONMISC_ARMOR)
    {
        BOOST_TEST(
            ((IS_MISC == false) && (IS_NONMISC_WEAPON == false)),
            "(misc=" << IS_MISC << ", nonmisc_weapon=" << IS_NONMISC_WEAPON
                     << ", nonmisc_armor=" << IS_NONMISC_ARMOR << ") is an invalid combination.");
    }
    else
    {
        std::ostringstream ss;
        ss << "(misc=" << IS_MISC << ", nonmisc_weapon=" << IS_NONMISC_WEAPON
           << ", nonmisc_armor=" << IS_NONMISC_ARMOR << ") is an invalid combination.";

        throw std::runtime_error(ss.str());
    }
}

BOOST_AUTO_TEST_CASE(all_item_tests)
{
    ItemProfileVec_t profiles;

    {
        SingleImageDisplayerScoped displayerScoped("Starting all item profile collection...", 0);
        displayerScoped.draw();

        auto & normalProfiles = ItemProfileWarehouse::Instance()->GetNormalProfiles();
        auto & relProfiles = ItemProfileWarehouse::Instance()->GetReligiousProfiles();
        auto & questProfiles = ItemProfileWarehouse::Instance()->GetQuestProfiles();

        BOOST_TEST(!normalProfiles.empty());

        BOOST_TEST(
            !relProfiles.empty(),
            "ItemFactory::Test() found ItemProfieWarehouse's religious profiles empty.");

        BOOST_TEST(
            !questProfiles.Empty(),
            "ItemFactory::Test() found ItemProfieWarehouse's quest profiles empty.");

        profiles.insert(std::end(profiles), std::begin(normalProfiles), std::end(normalProfiles));
        profiles.insert(std::end(profiles), std::begin(relProfiles), std::end(relProfiles));

        for (const auto & ENUM_PROFILE_PAIR : questProfiles)
        {
            profiles.push_back(ENUM_PROFILE_PAIR.second);
        }
    }

    {
        SingleImageDisplayerScoped displayerScoped(
            "Starting duplicate item profiles test...basically a big sort...", 0);

        displayerScoped.draw();

        std::sort(std::begin(profiles), std::end(profiles));

        ItemProfileVec_t duplicateProfiles;

        std::copy(
            std::unique(std::begin(profiles), std::end(profiles)),
            std::end(profiles),
            std::back_inserter(duplicateProfiles));

        if (duplicateProfiles.empty() == false)
        {
            const std::size_t DUPLICATE_COUNT_TO_DISPLAY { 10 };

            M_HP_LOG_ERR(
                "FAIL: " << duplicateProfiles.size()
                         << " duplicate item profiles found.  Here are the first "
                         << DUPLICATE_COUNT_TO_DISPLAY << ":");

            for (std::size_t i(0); i < DUPLICATE_COUNT_TO_DISPLAY; ++i)
            {
                if (i < duplicateProfiles.size())
                {
                    M_HP_LOG_ERR(
                        "\t DUPLICATE ITEM PROFILE={" << duplicateProfiles.at(i).ToString()
                                                      << "}\n");
                }
                else
                {
                    break;
                }
            }
        }
    }

    const std::size_t iterationsBeforeDraw = 500;
    std::size_t counter = 0;

    auto resetCounter = [&]() { counter = 0; };

    auto countAndDraw = [&](SingleImageDisplayerScoped & displayer) {
        if (++counter >= iterationsBeforeDraw)
        {
            displayer.get().incrememntProgress();
            displayer.draw();
            resetCounter();
        }
    };

    std::vector<item::ItemPtr_t> itemPtrs;
    itemPtrs.reserve(profiles.size());

    misc::VectorMap<std::string, ItemProfile> imageFilenameProfileMap;
    imageFilenameProfileMap.Reserve(1024); // there were 556 item images as of 2018-5-1

    {
        SingleImageDisplayerScoped displayerScoped(
            "Starting Individual Item Tests...", ((profiles.size() * 3) / iterationsBeforeDraw));

        displayerScoped.draw();

        resetCounter();

        ItemFactory factory;

        for (const auto & PROFILE : profiles)
        {
            itemPtrs.emplace_back(factory.Make(PROFILE));
            countAndDraw(displayerScoped);
        }

        const auto ITEM_PTR_COUNT = itemPtrs.size();
        for (std::size_t i(0); i < ITEM_PTR_COUNT; ++i)
        {
            imageFilenameProfileMap.AppendIfKeyNotFound(itemPtrs[i]->ImagePath(), profiles[i]);
            countAndDraw(displayerScoped);
        }

        for (std::size_t i(0); i < ITEM_PTR_COUNT; ++i)
        {
            TestItem(itemPtrs[i], profiles[i]);
            countAndDraw(displayerScoped);
        }
    }

    {
        SingleImageDisplayerScoped displayerScoped(
            "Starting Images Test...", imageFilenameProfileMap.Size());

        for (const auto & FILENAME_PROFILE_PAIR : imageFilenameProfileMap)
        {
            displayerScoped.get().appendImageToSeries(
                gui::CachedTexture(PathWrapper(FILENAME_PROFILE_PAIR.first)));

            displayerScoped.draw();
        }
    }

    {
        SingleImageDisplayerScoped displayerScoped(
            "Starting Log of Random Item Names and Descriptions for manual check...if you ever "
            "do...",
            27);

        displayerScoped.draw();

        M_HP_LOG("\n\n~~~~~~~~~~~~~~~~~~~~ Random Item Names and Descriptions To Verify "
                 "~~~~~~~~~~~~~~~~~~~~");

        std::string str;
        str.reserve(4096);

        std::vector<item::ItemPtr_t> selectedPtrs;
        selectedPtrs.reserve(profiles.size());

        auto appendProfileNamesIf { [&](auto ifLambda,
                                        const std::string & CATEGORY_NAME = "",
                                        const std::size_t ORIG_COUNT_TO_DISPLAY = 10) {
            str.clear();
            selectedPtrs.clear();

            for (const auto & PTR : itemPtrs)
            {
                if (ifLambda(PTR))
                {
                    selectedPtrs.emplace_back(PTR);
                }
            }

            const auto ACTUAL_COUNT_TO_DISPLAY { (
                (ORIG_COUNT_TO_DISPLAY == 0) ? selectedPtrs.size() : ORIG_COUNT_TO_DISPLAY) };

            if (ACTUAL_COUNT_TO_DISPLAY > 1)
            {
                str += "\n~~~";
                str += std::to_string(ACTUAL_COUNT_TO_DISPLAY);
                str += ' ';
                str += CATEGORY_NAME;
                str += ":\n";
            }
            else
            {
                str += '\n';
            }

            for (std::size_t i(0); i < ACTUAL_COUNT_TO_DISPLAY; ++i)
            {
                const auto itemPtr = misc::RandomSelect(selectedPtrs);
                str += "\tName=\"";
                str += itemPtr->Name();
                str += "\"\n\tDesc=\"";
                str += itemPtr->Desc();
                str += "\"\n\tItem={";
                str += itemPtr->ToString();
                str += "}\n\n";
            }

            if (ACTUAL_COUNT_TO_DISPLAY > 1)
            {
                str += "~~~\n\n";
            }
            else
            {
                str += "\n\n";
            }

            M_HP_LOG(str);
            displayerScoped.get().incrememntProgress();
            displayerScoped.draw();
        } };

        appendProfileNamesIf([](const ItemPtr_t & PTR) { return PTR->IsMisc(); }, "Misc");
        appendProfileNamesIf([](const ItemPtr_t & PTR) { return PTR->IsSet(); }, "Sets");
        appendProfileNamesIf([](const ItemPtr_t & PTR) { return PTR->IsNamed(); }, "Named");
        appendProfileNamesIf([](const ItemPtr_t & PTR) { return PTR->IsUnique(); }, "Unique");
        appendProfileNamesIf([](const ItemPtr_t & PTR) { return PTR->IsElemental(); }, "Element");
        appendProfileNamesIf([](const ItemPtr_t & PTR) { return PTR->IsWeapon(); }, "Weapons");

        appendProfileNamesIf(
            [](const ItemPtr_t & PTR) { return PTR->WeaponInfo().IsAxe(); }, "Axe", 3);

        appendProfileNamesIf(
            [](const ItemPtr_t & PTR) { return PTR->WeaponInfo().IsBladedStaff(); },
            "BladedStaff",
            3);

        appendProfileNamesIf(
            [](const ItemPtr_t & PTR) { return PTR->WeaponInfo().IsClub(); }, "Club", 3);

        appendProfileNamesIf(
            [](const ItemPtr_t & PTR) { return PTR->WeaponInfo().IsProjectile(); },
            "Projectile",
            3);

        appendProfileNamesIf(
            [](const ItemPtr_t & PTR) { return PTR->WeaponInfo().IsSword(); }, "Sword", 3);

        appendProfileNamesIf(
            [](const ItemPtr_t & PTR) { return PTR->WeaponInfo().IsWhip(); }, "Whip", 3);

        appendProfileNamesIf(
            [](const ItemPtr_t & PTR) { return PTR->WeaponInfo().IsKnife(); }, "Knife", 3);

        appendProfileNamesIf(
            [](const ItemPtr_t & PTR) { return PTR->WeaponInfo().IsDagger(); }, "Dagger", 3);

        appendProfileNamesIf(
            [](const ItemPtr_t & PTR) { return PTR->WeaponInfo().IsStaff(); }, "Staff", 3);

        appendProfileNamesIf([](const ItemPtr_t & PTR) { return PTR->IsArmor(); }, "Armor");

        appendProfileNamesIf(
            [](const ItemPtr_t & PTR) { return PTR->ArmorInfo().IsAventail(); }, "Aventail", 3);

        appendProfileNamesIf(
            [](const ItemPtr_t & PTR) { return PTR->ArmorInfo().IsBoots(); }, "Boots", 3);

        appendProfileNamesIf(
            [](const ItemPtr_t & PTR) { return PTR->ArmorInfo().IsBracers(); }, "Bracers", 3);

        appendProfileNamesIf(
            [](const ItemPtr_t & PTR) { return PTR->ArmorInfo().IsCover(); }, "Covering", 3);

        appendProfileNamesIf(
            [](const ItemPtr_t & PTR) { return PTR->ArmorInfo().IsGauntlets(); }, "Gauntlets", 3);

        appendProfileNamesIf(
            [](const ItemPtr_t & PTR) { return PTR->ArmorInfo().IsHelm(); }, "Helm", 10);

        appendProfileNamesIf(
            [](const ItemPtr_t & PTR) { return PTR->ArmorInfo().IsPants(); }, "Pants", 3);

        appendProfileNamesIf(
            [](const ItemPtr_t & PTR) { return PTR->ArmorInfo().IsShield(); }, "Shield", 10);

        appendProfileNamesIf(
            [](const ItemPtr_t & PTR) { return PTR->ArmorInfo().IsShirt(); }, "Shirt", 3);

        appendProfileNamesIf([](const ItemPtr_t & PTR) { return PTR->IsUnique(); }, "Unique", 10);

        M_HP_LOG("\n\n~~~~~~~~~~~~~~~~~~~~ Random Item Names and Descriptions To Verify "
                 "~~~~~~~~~~~~~~~~~~~~");
    }

    imageFilenameProfileMap.Clear();
    profiles.clear();

    {
        SingleImageDisplayerScoped displayerScoped(
            "Starting to Free All Items...", (itemPtrs.size() / iterationsBeforeDraw));

        resetCounter();

        for (const auto & PTR : itemPtrs)
        {
            ItemWarehouse::Access().Free(PTR);
            countAndDraw(displayerScoped);
        }

        itemPtrs.clear();
    }
}
