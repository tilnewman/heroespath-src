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

#include "creature/creature.hpp"
//#include "creature/nonplayer-inventory-factory.hpp"
#include "game/setup-teardown.hpp"
#include "gui/cached-texture.hpp"
#include "gui/creature-image-paths.hpp"
#include "gui/display.hpp"
#include "gui/item-image-paths.hpp"
#include "item/armor-ratings.hpp"
#include "item/inventory.hpp"
#include "item/item-cache.hpp"
#include "item/item-factory.hpp"
#include "item/item-profile-warehouse.hpp"
#include "item/item-profiles-reporter.hpp"
#include "item/item-template-factory.hpp"
#include "item/item-warehouse.hpp"
#include "item/item.hpp"
#include "misc/config-file.hpp"
#include "misc/enum-util.hpp"
#include "misc/filesystem.hpp"
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

class ImageFilenames
{
public:
    ImageFilenames(const std::string & DIR_CONFIG_KEY)
        : original()
        , remaining()
    {
        original.reserve(1024);

        const auto FULL_PATHS = misc::filesystem::FindFiles(
            false,
            misc::ConfigFile::Instance()->GetMediaPath(DIR_CONFIG_KEY),
            "",
            "",
            misc::filesystem::COMMON_FILE_NAME_PARTS_TO_EXCLUDE_VEC_);

        for (const auto & FULL_PATH : FULL_PATHS)
        {
            original.emplace_back(misc::filesystem::Filename(FULL_PATH));
        }

        std::sort(std::begin(original), std::end(original));
        remaining = original;
    }

    ~ImageFilenames()
    {
        BOOST_TEST(
            (remaining.empty()),
            "Out of " << totalCount() << " images, " << remaining.size()
                      << " were NEVER used during what is supposed to be an exhaustive pass: \n\t"
                      << misc::Join(remaining, misc::JoinHow("\n\t")));
    }

    void remove(const std::vector<std::string> & FILENAMES)
    {
        for (const auto & FILENAME : FILENAMES)
        {
            BOOST_TEST(
                (std::find(std::begin(original), std::end(original), FILENAME)
                 != std::end(original)),
                "asked a generated image filename does not exist in the dir: \"" << FILENAME
                                                                                 << "\"");

            remaining.erase(
                std::remove(
                    std::begin(remaining), std::end(remaining), misc::ToLowerCopy(FILENAME)),
                std::end(remaining));
        }
    }

    std::size_t totalCount() const { return original.size(); }

private:
    std::vector<std::string> original;
    std::vector<std::string> remaining;
};

inline void TestItem(const ItemPtr_t & ITEM_PTR, const ItemProfile & ITEM_PROFILE)
{
    BOOST_TEST(
        (ITEM_PTR->MaterialPrimary() != ITEM_PTR->MaterialSecondary()),
        "made an item with duplicate materials.");

    BOOST_TEST((ITEM_PTR->Category() == ITEM_PROFILE.Category()), "categories did not match.");
    BOOST_TEST((ITEM_PTR->MiscType() == ITEM_PROFILE.MiscType()), "Miscs did not match.");
    BOOST_TEST((ITEM_PTR->SetType() == ITEM_PROFILE.SetType()), "Sets did not match.");
    BOOST_TEST((ITEM_PTR->ElementType() == ITEM_PROFILE.ElementType()), "Elements did not match.");
    BOOST_TEST((EnumUtil<Element>::IsValid(ITEM_PTR->ElementType())), "invalid Element.");
    BOOST_TEST((ITEM_PTR->NamedType() == ITEM_PROFILE.NamedType()), "Nameds did not match.");

    BOOST_TEST(
        (ITEM_PTR->MaterialPrimary() == ITEM_PROFILE.MaterialPrimary()),
        "primary materials did not match.");

    BOOST_TEST(
        (ITEM_PTR->MaterialPrimary() < Material::Count),
        "primary material was invalid: " << EnumUnderlying_t(ITEM_PTR->MaterialPrimary()));

    BOOST_TEST(
        (ITEM_PTR->MaterialSecondary() == ITEM_PROFILE.MaterialSecondary()),
        "secondary materials did not match.");

    BOOST_TEST(
        (ITEM_PTR->IsWeapon() != ITEM_PTR->IsArmor()), "both weapon and armor at the same time.");

    BOOST_TEST(
        (!ITEM_PTR->IsBodyPart()),
        "bodypart, but the ItemProfileWarehouse should not create any bodypart "
        "items since they cannot be found in treasure chests.");

    BOOST_TEST(!ITEM_PTR->IsBroken(), "broken.");

    const auto & ARMOR_INFO = ITEM_PTR->ArmorInfo();

    if (ARMOR_INFO.IsMinor<item::Helms>(item::Helms::Leather))
    {
        BOOST_TEST(
            (ITEM_PTR->MaterialPrimary() == Material::Leather),
            "Leather Helm had a primary material that was not hard leather.");
    }

    // Sets cannot be elemental
    if (ITEM_PTR->IsSet())
    {
        BOOST_TEST(!ITEM_PTR->IsElemental(), "Set but also Element.");
    }

    // Nameds must be weapons or armor, and cannot be misc
    if (ITEM_PTR->IsNamed())
    {
        BOOST_TEST(
            ((ITEM_PTR->IsWeapon() || ITEM_PTR->IsArmor()) && !ITEM_PTR->IsMisc()),
            "Named but either not weapon(" << ITEM_PTR->IsWeapon() << ")/armor("
                                           << ITEM_PTR->IsArmor() << ") or Misc("
                                           << ITEM_PTR->IsMisc() << ").");
    }

    const auto & SUMMON_INFO = ITEM_PTR->SummoningInfo();

    // all summoning items are also misc items
    if (SUMMON_INFO.IsValid())
    {
        BOOST_TEST(
            ITEM_PTR->IsMisc(),
            "IsValid() but not Misc, which should never "
            "happen.  All summoning items are also Misc items.");

        BOOST_TEST(
            Misc::IsSummoning(ITEM_PTR->MiscType()), "IsValid() but not Misc::IsSummoning().");
    }

    // invalid summoning info
    if (!SUMMON_INFO.IsValid())
    {
        BOOST_TEST(SUMMON_INFO.IsValid(), "summoning but summoning_info was invalid.");
    }

    // if summoning type then must be able to summon, and vice versa
    BOOST_TEST(
        (Misc::IsSummoning(ITEM_PTR->MiscType()) == SUMMON_INFO.IsValid()),
        "Misc=summoning_type but it is not able to summon.");

    // all items must be weapon, armor, or misc
    BOOST_TEST(
        (ITEM_PTR->IsWeapon() || ITEM_PTR->IsArmor() || (ITEM_PTR->MiscType() < Misc::Count)),
        "the item was not a weapon, armor, or Misc.  All items must be one of "
        "these three.");

    BOOST_TEST(
        !ITEM_PTR->BaseName().empty(),
        "BaseName (\"" << ITEM_PTR->BaseName() << "\") was empty or invalid.");

    if (ITEM_PTR->IsArmor())
    {
        BOOST_TEST(ARMOR_INFO.IsValid(), "IsArmor() but ArmorInfo().IsValid()==false.");

        const auto MAT_NAME_TYPE = ITEM_PTR->GetMaterialNameStyle();

        BOOST_TEST(
            ((MAT_NAME_TYPE != MaterialNameStyle::Count)
             && (MAT_NAME_TYPE != MaterialNameStyle::Misc)),
            "armor had invalid MaterialNameStyle=" << NAMEOF_ENUM(MAT_NAME_TYPE));

        if (ITEM_PTR->ArmorType() != Armor::Skin)
        {
            const auto IS_SPECIFIC_ARMOR_TYPE { (
                (ITEM_PTR->ArmorType() == Armor::Cover) || (ITEM_PTR->ArmorType() == Armor::Helm)
                || (ITEM_PTR->ArmorType() == Armor::Shield)) };

            if (IS_SPECIFIC_ARMOR_TYPE)
            {
                BOOST_TEST(
                    (ARMOR_INFO.Form() >= Forms::Count),
                    "armor is specific Armor=" << NAMEOF_ENUM(ITEM_PTR->ArmorType())
                                               << " but had an invalid Form="
                                               << NAMEOF_ENUM(ARMOR_INFO.Form()) << ".");
            }
            else
            {
                BOOST_TEST(
                    (ARMOR_INFO.Form() < Forms::Count),
                    "armor is non-specific Armor=" << NAMEOF_ENUM(ITEM_PTR->ArmorType())
                                                   << " but had a Form="
                                                   << NAMEOF_ENUM(ARMOR_INFO.Form()) << ".");

                if (ARMOR_INFO.Form() == Forms::Plain)
                {
                    BOOST_TEST(
                        ((ITEM_PTR->MaterialPrimary() == Material::Cloth)
                         || (ITEM_PTR->MaterialPrimary() == Material::Leather)
                         || (ITEM_PTR->MaterialPrimary() == Material::Silk)
                         || (ITEM_PTR->MaterialPrimary() == Material::Fur)
                         || (ITEM_PTR->MaterialPrimary() == Material::Hide)),
                        "armor is non-specific Armor="
                            << NAMEOF_ENUM(ITEM_PTR->ArmorType())
                            << " with Form=Plain but the primary material was not one of "
                               "the "
                               "valid 'plain' materials.  It was "
                            << NAMEOF_ENUM(ITEM_PTR->MaterialPrimary()) << ".");
                }
                else if ((ARMOR_INFO.Form() == Forms::Mail) || (ARMOR_INFO.Form() == Forms::Plate))
                {
                    BOOST_TEST(
                        Material::IsMetal(ITEM_PTR->MaterialPrimary()),
                        "armor is non-specific Armor="
                            << NAMEOF_ENUM(ITEM_PTR->ArmorType())
                            << " with Form=" << NAMEOF_ENUM(ARMOR_INFO.Form())
                            << " but the primary material was not metal.  It was "
                            << NAMEOF_ENUM(ITEM_PTR->MaterialPrimary()) << ".");
                }
                else if (ARMOR_INFO.Form() == Forms::Scale)
                {
                    BOOST_TEST(
                        (ITEM_PTR->MaterialPrimary() == Material::Scales),
                        "armor is non-specific Armor="
                            << NAMEOF_ENUM(ITEM_PTR->ArmorType())
                            << " with Form=Scale but the primary material was not Scales. "
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
        " Weapons did not match.  profile=" << NAMEOF_ENUM(ITEM_PROFILE.WeaponType())
                                            << " but item=" << NAMEOF_ENUM(ITEM_PTR->WeaponType())
                                            << ".");

    BOOST_TEST(
        (ITEM_PROFILE.ArmorType() == ITEM_PTR->ArmorType()),
        " Armors did not match.  profile=" << NAMEOF_ENUM(ITEM_PROFILE.ArmorType()) << " but item="
                                           << NAMEOF_ENUM(ITEM_PTR->ArmorType()) << ".");

    const bool IS_ITEM_EQUIPABLE { ITEM_PTR->HasCategory(Category::Equipable) };

    if (IS_ITEM_EQUIPABLE)
    {
        const auto IS_WEARABLE { ITEM_PTR->HasCategory(Category::Wearable) };
        const auto IS_ONE_HANDED { ITEM_PTR->HasCategory(Category::OneHanded) };
        const auto IS_TWO_HANDED { ITEM_PTR->HasCategory(Category::TwoHanded) };

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
        BOOST_TEST(!ITEM_PTR->HasCategory(Category::Wearable), "not equipable but is wearable.");
        BOOST_TEST(!ITEM_PTR->HasCategory(Category::OneHanded), "not equipable but is one-handed.");
        BOOST_TEST(!ITEM_PTR->HasCategory(Category::TwoHanded), "not equipable but is two-handed.");
    }

    if (ITEM_PTR->IsMisc())
    {
        // unique_types cannot be set
        if (Misc::IsUnique(ITEM_PTR->MiscType()))
        {
            BOOST_TEST(!ITEM_PTR->IsSet(), "Misc unique but also Set.");
        }
    }

    if (ITEM_PTR->IsWeapon())
    {
        BOOST_TEST(ITEM_PTR->WeaponInfo().IsValid());
        const auto MAT_NAME_TYPE = ITEM_PTR->GetMaterialNameStyle();

        BOOST_TEST(
            ((MAT_NAME_TYPE != MaterialNameStyle::Count)
             && (MAT_NAME_TYPE != MaterialNameStyle::Misc)),
            "weapon had invalid MaterialNameStyle=" << NAMEOF_ENUM(MAT_NAME_TYPE));
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

    BOOST_TEST(ITEM_PROFILE.IsValid(), "item profile failed IsValid()");
}

inline const std::vector<std::string> getCreatureImageFilenames(
    const creature::race::Enum RACE_ENUM, const creature::role::Enum ROLE_ENUM)
{
    std::vector<std::string> imageFilenames;
    imageFilenames.reserve(32);

    for (EnumUnderlying_t sexIndex(0); sexIndex < creature::sex::Count; ++sexIndex)
    {
        const auto SEX_ENUM { static_cast<typename creature::sex::Enum>(sexIndex) };

        if (RACE_ENUM == creature::race::Dragon)
        {
            for (EnumUnderlying_t dragonIndex(0); dragonIndex < creature::dragon_class::Count;
                 ++dragonIndex)
            {
                const auto DRAGON_CLASS_ENUM { static_cast<typename creature::dragon_class::Enum>(
                    dragonIndex) };

                for (const auto & FILENAME_STR : gui::CreatureImagePaths::Filenames(
                         RACE_ENUM,
                         ROLE_ENUM,
                         SEX_ENUM,
                         creature::WolfenClass::Count,
                         DRAGON_CLASS_ENUM))
                {
                    imageFilenames.emplace_back(FILENAME_STR);
                }
            }
        }
        else if (RACE_ENUM == creature::race::Wolfen)
        {
            for (EnumUnderlying_t wolfenIndex(0); wolfenIndex < creature::WolfenClass::Count;
                 ++wolfenIndex)
            {
                const auto WOLFEN_CLASS_ENUM { static_cast<typename creature::WolfenClass::Enum>(
                    wolfenIndex) };

                for (const auto & FILENAME_STR : gui::CreatureImagePaths::Filenames(
                         RACE_ENUM,
                         ROLE_ENUM,
                         SEX_ENUM,
                         WOLFEN_CLASS_ENUM,
                         creature::dragon_class::Count))
                {
                    imageFilenames.emplace_back(FILENAME_STR);
                }
            }
        }
        else
        {
            for (const auto & FILENAME_STR : gui::CreatureImagePaths::Filenames(
                     RACE_ENUM,
                     ROLE_ENUM,
                     SEX_ENUM,
                     creature::WolfenClass::Count,
                     creature::dragon_class::Count))
            {
                imageFilenames.emplace_back(FILENAME_STR);
            }
        }
    }

    const std::string CONFIG_STR { "Creature configuration race="
                                   + std::string(NAMEOF_ENUM(RACE_ENUM))
                                   + ", role=" + std::string(NAMEOF_ENUM(ROLE_ENUM)) };

    BOOST_TEST(!imageFilenames.empty(), CONFIG_STR << " has no images.");

    const std::size_t ORIG_COUNT { imageFilenames.size() };

    std::sort(std::begin(imageFilenames), std::end(imageFilenames));

    imageFilenames.erase(
        std::unique(std::begin(imageFilenames), std::end(imageFilenames)),
        std::end(imageFilenames));

    const std::size_t UNIQUE_COUNT { imageFilenames.size() };

    M_HP_LOG(
        CONFIG_STR << " has " << UNIQUE_COUNT << " images with " << (ORIG_COUNT - UNIQUE_COUNT)
                   << " duplicates.");

    return imageFilenames;
}

BOOST_AUTO_TEST_CASE(item_profiles_and_individual_tests)
{
    ItemProfileVec_t profiles;
    ImageFilenames allItemImageFilenames("media-image-item-dir");

    // This code block puts the complete set of all items into the profiles vector EXCEPT for
    // body parts, which never have Profiles because they can never be acquired during game
    // play.  They can only be put in place when a creature is constructed.
    //
    // This means that when all tests are complete that were based on profiles we will still
    // need to test all weapon and armor items because they will include all the body parts.
    {
        SingleImageDisplayerScoped displayerScoped("Starting all item profile collection...", 0);
        displayerScoped.draw();

        auto & normalProfiles = ItemProfileWarehouse::Instance()->Normal();
        auto & relProfiles = ItemProfileWarehouse::Instance()->Religious();
        auto & questProfiles = ItemProfileWarehouse::Instance()->Quest();

        BOOST_TEST(!normalProfiles.empty());

        BOOST_TEST(
            !relProfiles.empty(),
            "ItemFactory::Test() found ItemProfieWarehouse's religious profiles empty.");

        BOOST_TEST(
            !questProfiles.empty(),
            "ItemFactory::Test() found ItemProfieWarehouse's quest profiles empty.");

        profiles.insert(std::end(profiles), std::begin(normalProfiles), std::end(normalProfiles));
        profiles.insert(std::end(profiles), std::begin(relProfiles), std::end(relProfiles));
        profiles.insert(std::end(profiles), std::begin(questProfiles), std::end(questProfiles));
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

    std::vector<ItemPtr_t> itemPtrs;
    itemPtrs.reserve(profiles.size());

    misc::VectorMap<std::string, ItemProfile> imageFilenameProfileMap;
    imageFilenameProfileMap.Reserve(1024); // there were 556 item images as of 2018-5-1

    {
        SingleImageDisplayerScoped displayerScoped(
            "Starting Individual Item Tests...", ((profiles.size() * 3) / iterationsBeforeDraw));

        displayerScoped.draw();

        resetCounter();

        for (const auto & PROFILE : profiles)
        {
            itemPtrs.emplace_back(ItemFactory::Make(PROFILE));
            countAndDraw(displayerScoped);
        }

        const auto ITEM_PTR_COUNT = itemPtrs.size();
        for (std::size_t i(0); i < ITEM_PTR_COUNT; ++i)
        {
            const auto IMAGE_FILENAMES = gui::ItemImagePaths::Filenames(itemPtrs[i]->Profile());

            allItemImageFilenames.remove(IMAGE_FILENAMES);

            for (const auto & IMAGE_FILENAME_STR : IMAGE_FILENAMES)
            {
                imageFilenameProfileMap.AppendIfKeyNotFound(IMAGE_FILENAME_STR, profiles[i]);
            }

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
            displayerScoped.get().appendImageToSeries(gui::CachedTexture(
                PathWrapper(gui::ItemImagePaths::PathFromFilename(FILENAME_PROFILE_PAIR.first))));

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

        std::vector<ItemPtr_t> selectedPtrs;
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
            [](const ItemPtr_t & PTR) { return PTR->WeaponInfo().IsBladedstaff(); },
            "Bladedstaff",
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
            [](const ItemPtr_t & PTR) {
                return PTR->WeaponInfo().IsMinor<item::Knifes>(item::Knifes::Knife);
            },
            "Knife",
            3);

        appendProfileNamesIf(
            [](const ItemPtr_t & PTR) {
                return PTR->WeaponInfo().IsMinor<item::Knifes>(item::Knifes::Dagger);
            },
            "Dagger",
            3);

        appendProfileNamesIf(
            [](const ItemPtr_t & PTR) { return PTR->WeaponInfo().IsAnyStaff(); }, "Staff", 3);

        appendProfileNamesIf([](const ItemPtr_t & PTR) { return PTR->IsArmor(); }, "Armor");

        appendProfileNamesIf(
            [](const ItemPtr_t & PTR) { return PTR->ArmorInfo().IsAventail(); }, "Aventail", 3);

        appendProfileNamesIf(
            [](const ItemPtr_t & PTR) { return PTR->ArmorInfo().IsBoot(); }, "Boot", 3);

        appendProfileNamesIf(
            [](const ItemPtr_t & PTR) { return PTR->ArmorInfo().IsBracer(); }, "Bracer", 3);

        appendProfileNamesIf(
            [](const ItemPtr_t & PTR) { return PTR->ArmorInfo().IsCover(); }, "Cover", 3);

        appendProfileNamesIf(
            [](const ItemPtr_t & PTR) { return PTR->ArmorInfo().IsGauntlet(); }, "Gauntlet", 3);

        appendProfileNamesIf(
            [](const ItemPtr_t & PTR) { return PTR->ArmorInfo().IsHelm(); }, "Helm", 10);

        appendProfileNamesIf(
            [](const ItemPtr_t & PTR) { return PTR->ArmorInfo().IsPant(); }, "Pant", 3);

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

    ItemProfilesReporter::LogReport();
    ArmorRatings::Instance()->LogCommonArmorRatings();
}

BOOST_AUTO_TEST_CASE(inventory_factory_tests)
{
    /*
    ImageFilenames allCreatureImageFilenames("media-image-creature-dir");

    creature::nonplayer::InventoryFactory inventoryFactory;

    const int RANK_MAX_OVERSHOOT { 10 };

    const int RANK_MAX_DRAGON { misc::ConfigFile::Instance()->ValueOrDefault<int>(
                                    "creature-dragon-class-rank-min-Elder")
                                + RANK_MAX_OVERSHOOT };

    const int RANK_MAX_WOLFEN { misc::ConfigFile::Instance()->ValueOrDefault<int>(
                                    "creature-wolfen-class-rank-min-Elder")
                                + RANK_MAX_OVERSHOOT };

    const int RANK_MAX_OTHER { misc::ConfigFile::Instance()->ValueOrDefault<int>(
                                   "rankclass-Master-rankmax")
                               + RANK_MAX_OVERSHOOT };

    const int RANK_MAX_MAX { misc::Max(RANK_MAX_DRAGON, RANK_MAX_WOLFEN, RANK_MAX_OTHER) };

    std::string raceRoleStr;
    raceRoleStr.reserve(64);

    std::vector<creature::CreatureUPtr_t> characterUPtrs;
    characterUPtrs.reserve(static_cast<std::size_t>(RANK_MAX_MAX + 2));

    std::vector<std::string> raceRoleStrings;
    raceRoleStrings.reserve(1024);

    const std::size_t iterationsBeforeDraw = 10;
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

    for (EnumUnderlying_t raceIndex(0); raceIndex < creature::race::Count; ++raceIndex)
    {
        const auto RACE_ENUM { static_cast<typename creature::race::Enum>(raceIndex) };

        const int RANK_MAX { [&]() {
            if (RACE_ENUM == creature::race::Dragon)
            {
                return RANK_MAX_DRAGON;
            }
            else if (RACE_ENUM == creature::race::Wolfen)
            {
                return RANK_MAX_WOLFEN;
            }
            else
            {
                return RANK_MAX_OTHER;
            }
        }() };

        for (const auto & ROLE_ENUM : creature::race::Roles(RACE_ENUM))
        {
            raceRoleStr.clear();
            raceRoleStr += NAMEOF_ENUM(RACE_ENUM);
            raceRoleStr += '_';
            raceRoleStr += NAMEOF_ENUM(ROLE_ENUM);
            raceRoleStr += '_';

            raceRoleStrings.emplace_back(raceRoleStr);

            SingleImageDisplayerScoped displayerScoped(
                "Inventory Factory Test___" + raceRoleStr + "___up to rank "
                    + std::to_string(RANK_MAX) + "...",
                (static_cast<std::size_t>(RANK_MAX * 3) / iterationsBeforeDraw));

            {
                const auto IMAGE_FILENAMES = getCreatureImageFilenames(RACE_ENUM, ROLE_ENUM);

                allCreatureImageFilenames.remove(IMAGE_FILENAMES);

                for (const auto & IMAGE_FILENAME : IMAGE_FILENAMES)
                {
                    displayerScoped.get().appendImageToSeries(gui::CachedTexture(
                        PathWrapper(gui::CreatureImagePaths::PathFromFilename(IMAGE_FILENAME))));

                    displayerScoped.draw();
                }
            }

            for (int rankIndex(1); rankIndex <= RANK_MAX; rankIndex += 1)
            {
                characterUPtrs.emplace_back(std::make_unique<creature::Creature>(
                    false,
                    raceRoleStr + std::to_string(rankIndex),
                    ((misc::RandomBool()) ? creature::sex::Female : creature::sex::Male),
                    RACE_ENUM,
                    ROLE_ENUM,
                    creature::StatSet(10_str, 10_acc, 10_cha, 10_lck, 10_spd, 10_int),
                    "",
                    10_health,
                    Rank_t::Make(rankIndex),
                    Experience_t::Make(rankIndex * 10000)));

                countAndDraw(displayerScoped);
            }

            for (const auto & CHARACTER_UPTR : characterUPtrs)
            {
                inventoryFactory.SetupCreatureInventory(misc::MakeNotNull(CHARACTER_UPTR.get()));
                countAndDraw(displayerScoped);
            }

            while (!characterUPtrs.empty())
            {
                characterUPtrs.pop_back();
                countAndDraw(displayerScoped);
            }
        }
    }

    // there were 166 race/role combinations on 2019-5-19
    {
        std::ostringstream ss;
        ss << "There are " << raceRoleStrings.size() << " total race/role combinations:";

        for (const auto & RACE_ROLE_STR : raceRoleStrings)
        {
            ss << "\n\t" << RACE_ROLE_STR;
        }

        M_HP_LOG(ss.str());
    }
    */
}
