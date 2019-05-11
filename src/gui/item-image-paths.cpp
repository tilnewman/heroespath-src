// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// item-image-paths.cpp
//
#include "item-image-paths.hpp"

#include "creature/dragon-class-enum.hpp"
#include "creature/nonplayer-inventory-types.hpp"
#include "creature/race-enum.hpp"
#include "creature/role-enum.hpp"
#include "creature/wolfen-class-enum.hpp"
#include "gui/cached-texture.hpp"
#include "gui/content-images.hpp"
#include "item/armor-type-wrapper.hpp"
#include "item/item.hpp"
#include "item/weapon-type-wrapper.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/boost-string-includes.hpp"
#include "misc/config-file.hpp"
#include "misc/enum-common.hpp"
#include "misc/filesystem.hpp"
#include "misc/log-macros.hpp"
#include "misc/random.hpp"
#include "stage/i-stage.hpp"

#include <SFML/Graphics/Texture.hpp>

#include <sstream>
#include <stdexcept>
#include <vector>

namespace heroespath
{
namespace gui
{

    std::string ItemImagePaths::imageDirectoryPath_ { "" };

    void ItemImagePaths::SetupFilesystemPaths()
    {
        imageDirectoryPath_ = misc::ConfigFile::Instance()->GetMediaPath("media-image-item-dir");

        M_HP_ASSERT_OR_LOG_AND_THROW(
            misc::filesystem::ExistsAndIsDirectory(imageDirectoryPath_),
            "Item image directory does not exist or is not a directory."
                + M_HP_VAR_STR(imageDirectoryPath_));
    }

    bool ItemImagePaths::Test(stage::IStagePtr_t iStagePtr)
    {
        static auto hasInitialPrompt { false };
        if (false == hasInitialPrompt)
        {
            hasInitialPrompt = true;
            iStagePtr->TestingStrAppend("gui::ItemImagePaths::Test()  Starting tests...");
        }

        const std::string TEST_PRE_STR { "ItemImagePaths Test " };

        static auto allPaths { misc::filesystem::FindFiles(
            false, imageDirectoryPath_, "", ContentImage::FilenameExtension()) };

        for (auto & pathStr : allPaths)
        {
            misc::ToLower(pathStr);
        }

        static const auto WEAPON_TYPE_WRAPPERS {
            item::weapon::WeaponTypeWrapper::MakeCompleteSet()
        };

        static std::size_t weaponIndex { 0 };
        if (weaponIndex < WEAPON_TYPE_WRAPPERS.size())
        {
            const auto & WEAPON_TYPE_WRAPPER { WEAPON_TYPE_WRAPPERS.at(weaponIndex) };

            const auto FILENAME { misc::ToLowerCopy(Filename(WEAPON_TYPE_WRAPPER, false)) };

            const auto IMAGE_PATH_STR { misc::ToLowerCopy(PathFromFilename(FILENAME)) };

            gui::CachedTexture cachedTexture { PathWrapper(IMAGE_PATH_STR) };

            auto imagePathFoundIter { std::find(
                std::begin(allPaths), std::end(allPaths), IMAGE_PATH_STR) };

            if (imagePathFoundIter != std::end(allPaths))
            {
                iStagePtr->TestingImageSet(IMAGE_PATH_STR);
                allPaths.erase(imagePathFoundIter);
            }

            iStagePtr->TestingStrIncrement(TEST_PRE_STR + WEAPON_TYPE_WRAPPER.ReadableName());
            ++weaponIndex;
            return false;
        }

        static const auto ARMOR_TYPE_WRAPPERS { item::armor::ArmorTypeWrapper::MakeCompleteSet() };

        static std::size_t armorIndex { 0 };
        if (armorIndex < ARMOR_TYPE_WRAPPERS.size())
        {
            const auto & ARMOR_TYPE_WRAPPER { ARMOR_TYPE_WRAPPERS.at(armorIndex) };

            const auto FILENAME { misc::ToLowerCopy(Filename(ARMOR_TYPE_WRAPPER)) };

            // skip testing skin images until below
            if (ARMOR_TYPE_WRAPPER.IsSkin() == false)
            {
                const auto IMAGE_PATH_STR { misc::ToLowerCopy(PathFromFilename(FILENAME)) };

                gui::CachedTexture cachedTexture { PathWrapper(IMAGE_PATH_STR) };

                auto imagePathFoundIter { std::find(
                    std::begin(allPaths), std::end(allPaths), IMAGE_PATH_STR) };

                if (imagePathFoundIter != std::end(allPaths))
                {
                    iStagePtr->TestingImageSet(IMAGE_PATH_STR);
                    allPaths.erase(imagePathFoundIter);
                }

                iStagePtr->TestingStrIncrement(TEST_PRE_STR + ARMOR_TYPE_WRAPPER.ReadableName());
            }

            ++armorIndex;
            return false;
        }

        static std::size_t skinIndex { 0 };
        if (skinIndex <= 2)
        {
            const auto MATERIAL_ENUM { [&]() {
                // keep in sync with item::material::SkinMaterial()
                if (0 == skinIndex)
                {
                    return item::material::Hide;
                }
                else if (1 == skinIndex)
                {
                    return item::material::Plant;
                }
                else
                {
                    return item::material::Scales;
                }
            }() };

            const auto FILENAME { misc::ToLowerCopy(GetSkinImageFilename(MATERIAL_ENUM)) };

            const auto IMAGE_PATH_STR { misc::ToLowerCopy(PathFromFilename(FILENAME)) };

            gui::CachedTexture cachedTexture { PathWrapper(IMAGE_PATH_STR) };

            auto imagePathFoundIter { std::find(
                std::begin(allPaths), std::end(allPaths), IMAGE_PATH_STR) };

            if (imagePathFoundIter != std::end(allPaths))
            {
                iStagePtr->TestingImageSet(IMAGE_PATH_STR);
                allPaths.erase(imagePathFoundIter);
            }

            iStagePtr->TestingStrIncrement(TEST_PRE_STR + FILENAME);

            ++skinIndex;
            return false;
        }

        // test misc items
        static auto miscIndex { 1 }; // start at 1 to avoid item::misc_type::Not
        if (miscIndex < static_cast<int>(item::misc_type::Count))
        {
            static auto extrasIndex { 0 };

            if (extrasIndex < 4)
            {
                const auto IS_JEWELED { ((1 == extrasIndex) || (3 == extrasIndex)) };
                const auto IS_BONE { (extrasIndex >= 2) };
                const auto ENUM { static_cast<item::misc_type::Enum>(miscIndex) };
                const auto ENUM_STR { NAMEOF_ENUM(ENUM) };
                const auto FILENAMES_VEC { Filenames(ENUM, IS_JEWELED, IS_BONE) };

                M_HP_ASSERT_OR_LOG_AND_THROW(
                    (FILENAMES_VEC.empty() == false),
                    "gui::ItemImagePaths::Test() While testing misc item #"
                        << miscIndex << " \"" << ENUM_STR << "\", is_jeweled=" << std::boolalpha
                        << IS_JEWELED << ", Filenames() returned an empty vector.");

                static std::size_t fileIndex { 0 };
                if (fileIndex < FILENAMES_VEC.size())
                {
                    const auto FILENAME { misc::ToLowerCopy(FILENAMES_VEC[fileIndex]) };

                    M_HP_ASSERT_OR_LOG_AND_THROW(
                        (FILENAME.empty() == false),
                        "gui::ItemImagePaths::Test() (rand)  "
                            << "While testing misc item #" << miscIndex << " \"" << ENUM_STR
                            << "\", filename #" << fileIndex << ", is_jeweled=" << std::boolalpha
                            << IS_JEWELED << ", found an empty filename string.");

                    std::string str;
                    str.reserve(128);
                    str += "ItemImagePaths Testing \"";
                    str += ENUM_STR;
                    str += "\", file_index=";
                    str += std::to_string(fileIndex);
                    str += " (";
                    str += ((IS_JEWELED) ? "jeweled" : "not-jeweled");
                    str += ')';

                    iStagePtr->TestingStrIncrement(str);

                    const auto IMAGE_PATH_STR { misc::ToLowerCopy(PathFromFilename(FILENAME)) };

                    gui::CachedTexture cachedTexture { PathWrapper(IMAGE_PATH_STR) };

                    auto imagePathFoundIter { std::find(
                        std::begin(allPaths), std::end(allPaths), IMAGE_PATH_STR) };

                    if (imagePathFoundIter != std::end(allPaths))
                    {
                        iStagePtr->TestingImageSet(IMAGE_PATH_STR);
                        allPaths.erase(imagePathFoundIter);
                    }

                    ++fileIndex;
                    return false;
                }

                fileIndex = 0;
                ++extrasIndex;
                return false;
            }

            extrasIndex = 0;
            ++miscIndex;
            return false;
        }

        std::sort(std::begin(allPaths), std::end(allPaths));

        for (const auto & FILENAME : allPaths)
        {
            M_HP_LOG_WRN(
                "gui::ItemImagePaths::Test() found the following item image "
                "unused: "
                << FILENAME);
        }

        iStagePtr->TestingStrAppend("gui::ItemImagePaths::Test()  ALL TESTS PASSED.");
        return true;
    }

    const std::string ItemImagePaths::PathFromFilename(const std::string & FILE_NAME)
    {
        if (FILE_NAME == ContentImage::TodoFilename())
        {
            return ContentImage::TodoPath();
        }
        else if (FILE_NAME == ContentImage::ErrorFilename())
        {
            return ContentImage::ErrorPath();
        }
        else
        {
            return misc::filesystem::CombinePaths(imageDirectoryPath_, FILE_NAME);
        }
    }

    const std::string ItemImagePaths::Filename(const item::Item & ITEM, const bool WILL_RANDOMIZE)
    {
        if (ITEM.Name().empty())
        {
            return ContentImage::ErrorFilename();
        }

        if (ITEM.IsMisc())
        {
            return Filename(
                ITEM.MiscType(),
                ITEM.IsJeweled(),
                (ITEM.MaterialPrimary() == item::material::Bone),
                WILL_RANDOMIZE);
        }

        if (ITEM.IsWeapon())
        {
            return Filename(ITEM.WeaponInfo(), ITEM.IsJeweled());
        }

        if (ITEM.IsArmor())
        {
            if (ITEM.ArmorInfo().IsSkin())
            {
                return GetSkinImageFilename(ITEM.MaterialPrimary());
            }
            else
            {
                return Filename(ITEM.ArmorInfo());
            }
        }

        M_HP_LOG_ERR(
            "Failed to find the image filename for that item because it was not misc, weapon, or "
            "armor.  Returning the ContentImage::ErrorFilename()."
            + M_HP_VAR_STR(ITEM.ToString()) + M_HP_VAR_STR(WILL_RANDOMIZE));

        return ContentImage::ErrorFilename();
    }

    const std::vector<std::string> ItemImagePaths::Filenames(
        const item::misc_type::Enum MISC_TYPE, const bool IS_JEWELED, const bool IS_BONE)
    {
        namespace ba = boost::algorithm;

        switch (MISC_TYPE)
        {
            // these misc_types are compound names where underscores need to be converted to
            // dashes
            case item::misc_type::Brooch_Crown:
            case item::misc_type::Brooch_Feather:
            case item::misc_type::Brooch_Fist:
            case item::misc_type::Brooch_Hourglass:
            case item::misc_type::Brooch_Key:
            case item::misc_type::Brooch_Mask:
            case item::misc_type::Charm_Crown:
            case item::misc_type::Charm_Feather:
            case item::misc_type::Charm_Fist:
            case item::misc_type::Charm_Hourglass:
            case item::misc_type::Charm_Key:
            case item::misc_type::Charm_Mask:
            case item::misc_type::Pin_Crown:
            case item::misc_type::Pin_Feather:
            case item::misc_type::Pin_Fist:
            case item::misc_type::Pin_Hourglass:
            case item::misc_type::Pin_Key:
            case item::misc_type::Pin_Mask:
            {
                const auto TYPE_STR { ba::replace_all_copy(
                    misc::ToLowerCopy(NAMEOF_ENUM(MISC_TYPE)),
                    "_",
                    ContentImage::FilenameSeparator()) };

                return { (TYPE_STR + ContentImage::FilenameExtension()) };
            }

                // these misc_types have specific filenames
            case item::misc_type::ManaAmulet:
            case item::misc_type::Pendant: { return MakeFilenames("amulet", 23);
            }
            case item::misc_type::CapeCommanders:
            case item::misc_type::CapeGenerals:
            case item::misc_type::CapeKings:
            {
                return { ("cape" + ContentImage::FilenameExtension()) };
            }
            case item::misc_type::ShadeCloak:
            {
                return { ("cloak" + ContentImage::FilenameExtension()) };
            }
            case item::misc_type::SpecterRobe:
            {
                return { ("robe" + ContentImage::FilenameExtension()) };
            }
            case item::misc_type::Goblet: { return MakeFilenames("goblet", 8);
            }
            case item::misc_type::Key: { return MakeFilenames("key", 11);
            }
            case item::misc_type::LockPicks: { return MakeFilenames("lockpicks", 6);
            }
            case item::misc_type::Mirror: { return MakeFilenames("mirror", 10);
            }
            case item::misc_type::DrumLute: { return MakeFilenames("drumlute", 13);
            }
            case item::misc_type::Orb: { return MakeFilenames("orb", 9);
            }
            case item::misc_type::Ring:
            {
                if (IS_JEWELED)
                {
                    return MakeFilenames("ring-jeweled", 13);
                }
                else if (IS_BONE)
                {
                    return { ("ring-bone" + ContentImage::FilenameExtension()) };
                }
                else
                {
                    return { "ring" + ContentImage::FilenameExtension() };
                }
            }
            case item::misc_type::RingHobo: { return { "ring" + ContentImage::FilenameExtension() };
            }
            case item::misc_type::Shard: { return MakeFilenames("shard", 7);
            }
            case item::misc_type::Wand: { return MakeFilenames("wand", 11);
            }
            case item::misc_type::Scepter: { return MakeFilenames("scepter", 26);
            }
            case item::misc_type::DollBlessed: { return MakeFilenames("doll", 4, 2);
            }
            case item::misc_type::DollCursed: { return MakeFilenames("doll", 10, 5);
            }
            case item::misc_type::Doll: { return { "doll-1" + ContentImage::FilenameExtension() };
            }
            case item::misc_type::FigurineBlessed: { return MakeFilenames("figurine", 6);
            }
            case item::misc_type::FigurineCursed: { return MakeFilenames("figurine-evil", 6);
            }
            case item::misc_type::Staff: { return MakeFilenames("staff", 21);
            }
            case item::misc_type::SummoningStatue: { return MakeFilenames("summoning-statue", 3);
            }
            case item::misc_type::BloodyDragonScale:
            {
                return { "scales" + ContentImage::FilenameExtension() };
            }
            case item::misc_type::FlagFanatics:
            case item::misc_type::FlagRegalCaptains:
            case item::misc_type::FlagTribal:
            {
                return { "flag" + ContentImage::FilenameExtension() };
            }
            case item::misc_type::MinotaurHide:
            {
                return { "hide" + ContentImage::FilenameExtension() };
            }
            case item::misc_type::ReaperScythe:
            {
                return { "bladedstaff-scythe" + ContentImage::FilenameExtension() };
            }

            // these misc_types are compound words whose filenames have dashes between each word
            case item::misc_type::AngelBraid:
            case item::misc_type::DevilHorn:
            case item::misc_type::GolemFinger:
            case item::misc_type::HurdyGurdy:
            case item::misc_type::LichHand:
            case item::misc_type::MummyHand:
            case item::misc_type::PetrifiedSnake:
            case item::misc_type::PipeAndTabor:
            case item::misc_type::UnicornHorn:
            case item::misc_type::BasiliskTongue:
            case item::misc_type::BerserkersBeard:
            case item::misc_type::BishopsHanky:
            case item::misc_type::BleedingTrophy:
            case item::misc_type::BottleOfBansheeScreams:
            case item::misc_type::BronzeTroll:
            case item::misc_type::BurialShroud:
            case item::misc_type::ChimeraBone:
            case item::misc_type::CobraTooth:
            case item::misc_type::CrystalChimes:
            case item::misc_type::DemonDiary:
            case item::misc_type::DoveBloodVial:
            case item::misc_type::DragonToothWhistle:
            case item::misc_type::DriedFrog:
            case item::misc_type::DriedGecko:
            case item::misc_type::DriedIguana:
            case item::misc_type::DriedLizard:
            case item::misc_type::DriedSalamander:
            case item::misc_type::DriedSkink:
            case item::misc_type::DriedToad:
            case item::misc_type::DruidLeaf:
            case item::misc_type::EvilRabbitsFoot:
            case item::misc_type::ExoticGoldenGong:
            case item::misc_type::EyeCyclops:
            case item::misc_type::EyeGiantOwl:
            case item::misc_type::EyeHawk:
            case item::misc_type::FriarsChronicle:
            case item::misc_type::FuneralRecord:
            case item::misc_type::GhostSheet:
            case item::misc_type::GlassCat:
            case item::misc_type::GriffinFeather:
            case item::misc_type::HangmansNoose:
            case item::misc_type::HobgoblinNose:
            case item::misc_type::HolyEpic:
            case item::misc_type::HornOfTheHorde:
            case item::misc_type::ImpTail:
            case item::misc_type::IslanderHeaddress:
            case item::misc_type::JeweledArmband:
            case item::misc_type::JeweledHandbag:
            case item::misc_type::JeweledPrincessVeil:
            case item::misc_type::LastRitesScroll:
            case item::misc_type::MacabreManuscript:
            case item::misc_type::MadRatJuju:
            case item::misc_type::MagicHorseshoe:
            case item::misc_type::MagnifyingGlass:
            case item::misc_type::MaskMourners:
            case item::misc_type::MaskRascal:
            case item::misc_type::MortuaryOrnament:
            case item::misc_type::NecklaceJeweledAnkh:
            case item::misc_type::NecklaceSharkTooth:
            case item::misc_type::NecklaceVampiresTooth:
            case item::misc_type::PantherPaw:
            case item::misc_type::PixieBell:
            case item::misc_type::RattlesnakeTail:
            case item::misc_type::RavenClaw:
            case item::misc_type::RequiemRegister:
            case item::misc_type::RingMendicant:
            case item::misc_type::RingMonk:
            case item::misc_type::RingPriest:
            case item::misc_type::RoyalScoutSpyglass:
            case item::misc_type::SaintCameoPin:
            case item::misc_type::SaintsJournal:
            case item::misc_type::SanguineRelic:
            case item::misc_type::ScoundrelSack:
            case item::misc_type::SepultureDecoration:
            case item::misc_type::ShamanRainmaker:
            case item::misc_type::SirenConch:
            case item::misc_type::SpecterChains:
            case item::misc_type::SpiderEggs:
            case item::misc_type::SprintersLegtie:
            case item::misc_type::SwindlersBag:
            case item::misc_type::TricksterPouch:
            case item::misc_type::TuningFork:
            case item::misc_type::TurtleShell:
            case item::misc_type::VultureGizzard:
            case item::misc_type::WarhorseMarionette:
            case item::misc_type::WarTrumpet:
            case item::misc_type::WeaselTotem:
            case item::misc_type::WolfenFur:
            case item::misc_type::WraithTalisman:
            case item::misc_type::DriedHead:
            case item::misc_type::PuppetBlessed:
            case item::misc_type::PuppetCursed:
            {
                const auto SEP_STR { misc::CamelTo(
                    NAMEOF_ENUM(MISC_TYPE),
                    ContentImage::FilenameSeparator(),
                    misc::CaseChange::LowerToUpper) };

                return { misc::ToLowerCopy(SEP_STR) + ContentImage::FilenameExtension() };
            }

            // these misc_types have single word names that are simply converted to lower-case
            case item::misc_type::Crumhorn:
            case item::misc_type::Icicle:
            case item::misc_type::Lyre:
            case item::misc_type::Recorder:
            case item::misc_type::Viol:
            case item::misc_type::Bust:
            case item::misc_type::Egg:
            case item::misc_type::Embryo:
            case item::misc_type::Seeds:
            {
                return { misc::ToLowerCopy(NAMEOF_ENUM(MISC_TYPE))
                         + ContentImage::FilenameExtension() };
            }

            case item::misc_type::Not:
            case item::misc_type::Count:
            default:
            {
                M_HP_LOG_ERR(
                    "MISC_TYPE enum is invalid, so returning the error image filename.  "
                    "(misc_type_enum_count="
                    << EnumUnderlying_t(item::misc_type::Count) << ")(MISC_TYPE="
                    << NAMEOF_ENUM(MISC_TYPE) << M_HP_VAR_STR(IS_JEWELED) << M_HP_VAR_STR(IS_BONE));

                return { ContentImage::ErrorFilename() };
            }
        }
    }

    const std::string ItemImagePaths::Filename(
        const item::misc_type::Enum MISC_TYPE,
        const bool IS_JEWELED,
        const bool IS_BONE,
        const bool WILL_RANDOMIZE)
    {
        const auto FILENAMES { Filenames(MISC_TYPE, IS_JEWELED, IS_BONE) };

        if (FILENAMES.empty())
        {
            M_HP_LOG_ERR(
                "The Filenames() function returned an empty string, so returning the error image "
                "filename."
                "(MISC_TYPE="
                << NAMEOF_ENUM(MISC_TYPE) << M_HP_VAR_STR(IS_JEWELED) << M_HP_VAR_STR(IS_BONE)
                << M_HP_VAR_STR(WILL_RANDOMIZE));

            return ContentImage::ErrorFilename();
        }

        if (WILL_RANDOMIZE)
        {
            return misc::RandomSelect(FILENAMES);
        }
        else
        {
            return FILENAMES[0];
        }
    }

    const std::string
        ItemImagePaths::GetSkinImageFilename(const item::material::Enum PRIMARY_MATERIAL)
    {
        auto materialToUseForName { PRIMARY_MATERIAL };
        if (PRIMARY_MATERIAL == item::material::Fur)
        {
            materialToUseForName = item::material::Hide;
        }

        return misc::ToLowerCopy(
            NAMEOF_ENUM_STR(materialToUseForName) + ContentImage::FilenameExtension());
    }

    const std::string ItemImagePaths::Filename(
        const item::weapon::WeaponTypeWrapper & WEAPON_TYPE_WRAPPER, const bool IS_JEWELED)
    {
        if (WEAPON_TYPE_WRAPPER.IsStaff())
        {
            if (IS_JEWELED)
            {
                return "staff-2" + ContentImage::FilenameExtension();
            }
            else
            {
                return "staff-plain" + ContentImage::FilenameExtension();
            }
        }
        else
        {
            return WEAPON_TYPE_WRAPPER.ImageFilename(
                ContentImage::FilenameSeparator(), ContentImage::FilenameExtension());
        }
    }

    const std::string
        ItemImagePaths::Filename(const item::armor::ArmorTypeWrapper & ARMOR_TYPE_WRAPPER)
    {
        if (ARMOR_TYPE_WRAPPER.IsPants() || ARMOR_TYPE_WRAPPER.IsBracers()
            || ARMOR_TYPE_WRAPPER.IsAventail())
        {
            return ARMOR_TYPE_WRAPPER.SpecificName() + ContentImage::FilenameExtension();
        }
        else
        {
            return ARMOR_TYPE_WRAPPER.ImageFilename(
                ContentImage::FilenameSeparator(), ContentImage::FilenameExtension());
        }
    }

    const std::vector<std::string> ItemImagePaths::MakeFilenames(
        const std::string & PREFIX, const int LAST_NUMBER, const int FIRST_NUMBER)
    {
        const int DIFF((LAST_NUMBER - FIRST_NUMBER) + 1);
        if (DIFF > 0)
        {
            std::vector<std::string> filenames;
            filenames.reserve(static_cast<std::size_t>(DIFF));

            for (int i(FIRST_NUMBER); i <= LAST_NUMBER; ++i)
            {
                filenames.emplace_back(
                    PREFIX + ContentImage::FilenameSeparator() + std::to_string(i)
                    + ContentImage::FilenameExtension());
            }

            return filenames;
        }
        else
        {
            std::ostringstream ss;
            ss << "ItemImagePaths::MakeFilenames(prefix=" << PREFIX
               << ", last_number=" << LAST_NUMBER << ", first_number=" << FIRST_NUMBER
               << ")  first was <= last";

            throw std::runtime_error(ss.str());
        }
    }

} // namespace gui
} // namespace heroespath
