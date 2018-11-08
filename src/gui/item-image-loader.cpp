// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// item-image-loader.cpp
//
#include "item-image-loader.hpp"

#include "creature/dragon-class-enum.hpp"
#include "creature/nonplayer-inventory-types.hpp"
#include "creature/wolfen-class-enum.hpp"
#include "gui/loaders.hpp"
#include "item/item.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/boost-string-includes.hpp"
#include "misc/config-file.hpp"
#include "misc/enum-common.hpp"
#include "misc/filesystem.hpp"
#include "misc/log-macros.hpp"
#include "misc/random.hpp"
#include "stage/i-stage.hpp"

#include <exception>
#include <sstream>
#include <vector>

namespace heroespath
{
namespace gui
{

    const std::string ItemImageLoader::FILE_EXT_STR_ { ".png" };
    const std::string ItemImageLoader::SEPARATOR_ { "-" };

    ItemImageLoader::ItemImageLoader()
        : imageDirectoryPath_(misc::filesystem::CleanPath(
            misc::ConfigFile::Instance()->GetMediaPath("media-images-items-dir")))
    {}

    bool ItemImageLoader::Test(stage::IStagePtr_t iStagePtr) const
    {
        static auto hasInitialPrompt { false };
        if (false == hasInitialPrompt)
        {
            hasInitialPrompt = true;
            iStagePtr->TestingStrAppend("gui::ItemImageLoader::Test()  Starting tests...");
        }

        const std::string TEST_PRE_STR { "ItemImageLoader Test " };

        static auto allPaths { misc::filesystem::FindFiles(
            false, imageDirectoryPath_, "", FILE_EXT_STR_) };

        for (auto & pathStr : allPaths)
        {
            boost::algorithm::to_lower(pathStr);
        }

        static const auto WEAPON_TYPE_WRAPPERS {
            item::weapon::WeaponTypeWrapper::MakeCompleteSet()
        };

        static std::size_t weaponIndex { 0 };
        if (weaponIndex < WEAPON_TYPE_WRAPPERS.size())
        {
            const auto & WEAPON_TYPE_WRAPPER { WEAPON_TYPE_WRAPPERS.at(weaponIndex) };

            const auto FILENAME { boost::algorithm::to_lower_copy(
                Filename(WEAPON_TYPE_WRAPPER, false)) };

            sf::Texture texture;
            Load(texture, FILENAME);

            const auto IMAGE_PATH_STR { boost::algorithm::to_lower_copy(
                MakeFullPathFromFilename(FILENAME)) };

            auto imagePathFoundIter { std::find(
                std::begin(allPaths), std::end(allPaths), IMAGE_PATH_STR) };

            if (imagePathFoundIter != std::end(allPaths))
            {
                iStagePtr->TestingImageSet(IMAGE_PATH_STR, true);
                allPaths.erase(imagePathFoundIter);
            }

            iStagePtr->TestingStrIncrement(TEST_PRE_STR + WEAPON_TYPE_WRAPPER.ReadableName());
            EnsureValidDimmensions(texture, WEAPON_TYPE_WRAPPER.ReadableName());
            ++weaponIndex;
            return false;
        }

        static const auto ARMOR_TYPE_WRAPPERS { item::armor::ArmorTypeWrapper::MakeCompleteSet() };

        static std::size_t armorIndex { 0 };
        if (armorIndex < ARMOR_TYPE_WRAPPERS.size())
        {
            const auto & ARMOR_TYPE_WRAPPER { ARMOR_TYPE_WRAPPERS.at(armorIndex) };

            const auto FILENAME { boost::algorithm::to_lower_copy(Filename(ARMOR_TYPE_WRAPPER)) };

            // skip testing skin images until below
            if (ARMOR_TYPE_WRAPPER.IsSkin() == false)
            {
                sf::Texture texture;
                Load(texture, FILENAME);

                const auto IMAGE_PATH_STR { boost::algorithm::to_lower_copy(
                    MakeFullPathFromFilename(FILENAME)) };

                auto imagePathFoundIter { std::find(
                    std::begin(allPaths), std::end(allPaths), IMAGE_PATH_STR) };

                if (imagePathFoundIter != std::end(allPaths))
                {
                    iStagePtr->TestingImageSet(IMAGE_PATH_STR, true);
                    allPaths.erase(imagePathFoundIter);
                }

                iStagePtr->TestingStrIncrement(TEST_PRE_STR + ARMOR_TYPE_WRAPPER.ReadableName());
                EnsureValidDimmensions(texture, ARMOR_TYPE_WRAPPER.ReadableName());
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

            const auto FILENAME { boost::algorithm::to_lower_copy(
                GetSkinImageFilename(MATERIAL_ENUM)) };

            sf::Texture texture;
            Load(texture, FILENAME);

            const auto IMAGE_PATH_STR { boost::algorithm::to_lower_copy(
                MakeFullPathFromFilename(FILENAME)) };

            auto imagePathFoundIter { std::find(
                std::begin(allPaths), std::end(allPaths), IMAGE_PATH_STR) };

            if (imagePathFoundIter != std::end(allPaths))
            {
                iStagePtr->TestingImageSet(IMAGE_PATH_STR, true);
                allPaths.erase(imagePathFoundIter);
            }

            iStagePtr->TestingStrIncrement(TEST_PRE_STR + FILENAME);
            EnsureValidDimmensions(texture, FILENAME);

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
                const auto ENUM_STR { item::misc_type::ToString(ENUM) };
                const auto FILENAMES_VEC { Filenames(ENUM, IS_JEWELED, IS_BONE) };

                M_HP_ASSERT_OR_LOG_AND_THROW(
                    (FILENAMES_VEC.empty() == false),
                    "gui::ItemImageLoader::Test() While testing misc item #"
                        << miscIndex << " \"" << ENUM_STR << "\", is_jeweled=" << std::boolalpha
                        << IS_JEWELED << ", Filenames() returned an empty vector.");

                static std::size_t fileIndex { 0 };
                if (fileIndex < FILENAMES_VEC.size())
                {
                    const auto FILENAME { boost::algorithm::to_lower_copy(
                        FILENAMES_VEC[fileIndex]) };

                    M_HP_ASSERT_OR_LOG_AND_THROW(
                        (FILENAME.empty() == false),
                        "gui::ItemImageLoader::Test() (rand)  "
                            << "While testing misc item #" << miscIndex << " \"" << ENUM_STR
                            << "\", filename #" << fileIndex << ", is_jeweled=" << std::boolalpha
                            << IS_JEWELED << ", found an empty filename string.");

                    std::ostringstream ss;
                    ss << "ItemImageLoader Testing \"" << ENUM_STR << "\", file_index=" << fileIndex
                       << " (" << ((IS_JEWELED) ? "jeweled" : "not-jeweled") << ")";

                    iStagePtr->TestingStrIncrement(ss.str());

                    sf::Texture texture;
                    Load(texture, FILENAME);

                    const auto IMAGE_PATH_STR { boost::algorithm::to_lower_copy(
                        MakeFullPathFromFilename(FILENAME)) };

                    auto imagePathFoundIter { std::find(
                        std::begin(allPaths), std::end(allPaths), IMAGE_PATH_STR) };

                    if (imagePathFoundIter != std::end(allPaths))
                    {
                        iStagePtr->TestingImageSet(IMAGE_PATH_STR, true);
                        allPaths.erase(imagePathFoundIter);
                    }

                    EnsureValidDimmensions(texture, ENUM_STR);

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
                "gui::ItemImageLoader::Test() found the following item image "
                "unused: "
                << FILENAME);
        }

        iStagePtr->TestingStrAppend("gui::ItemImageLoader::Test()  ALL TESTS PASSED.");
        return true;
    }

    void ItemImageLoader::Load(sf::Texture & texture, const item::ItemPtr_t ITEM_PTR) const
    {
        Load(texture, ITEM_PTR->ImageFilename());
    }

    const std::string ItemImageLoader::Path(const item::ItemPtr_t ITEM_PTR) const
    {
        return Path(ITEM_PTR->ImageFilename());
    }

    const std::string ItemImageLoader::Path(const std::string & FILE_NAME) const
    {
        return MakeFullPathFromFilename(FILE_NAME);
    }

    const std::string
        ItemImageLoader::Filename(const item::ItemPtr_t ITEM_PTR, const bool WILL_RANDOMIZE) const
    {
        if (ITEM_PTR->IsMisc())
        {
            return Filename(
                ITEM_PTR->MiscType(),
                ITEM_PTR->IsJeweled(),
                (ITEM_PTR->MaterialPrimary() == item::material::Bone),
                WILL_RANDOMIZE);
        }
        else
        {
            if (ITEM_PTR->IsWeapon())
            {
                return Filename(ITEM_PTR->WeaponInfo(), ITEM_PTR->IsJeweled());
            }
            else if (ITEM_PTR->IsArmor())
            {
                if (ITEM_PTR->ArmorInfo().IsSkin())
                {
                    return GetSkinImageFilename(ITEM_PTR->MaterialPrimary());
                }
                else
                {
                    return Filename(ITEM_PTR->ArmorInfo());
                }
            }
        }

        std::ostringstream ss;
        ss << "gui::ItemImageLoader::Filename(item={" << ITEM_PTR->ToString()
           << "}, will_randomize=" << std::boolalpha << WILL_RANDOMIZE
           << ") failed to find the image filename for that item because it was not weapon, "
              "armor, or misc type.";

        throw std::runtime_error(ss.str());
    }

    bool ItemImageLoader::ExistsAndFile(const item::ItemPtr_t ITEM_PTR) const
    {
        return ExistsAndFile(ITEM_PTR->ImageFilename());
    }

    bool ItemImageLoader::ExistsAndFile(const std::string & IMAGE_FILE_NAME) const
    {
        return misc::filesystem::ExistsAndIsFile(MakeFullPathFromFilename(IMAGE_FILE_NAME));
    }

    const std::vector<std::string> ItemImageLoader::Filenames(
        const item::misc_type::Enum MISC_TYPE, const bool IS_JEWELED, const bool IS_BONE) const
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
                    ba::to_lower_copy(item::misc_type::ToString(MISC_TYPE)), "_", SEPARATOR_) };

                return { (TYPE_STR + FILE_EXT_STR_) };
            }

                // these misc_types have specific filenames
            case item::misc_type::ManaAmulet:
            case item::misc_type::Pendant:
            {
                return MakeFilenames("amulet", 23);
            }
            case item::misc_type::CapeCommanders:
            case item::misc_type::CapeGenerals:
            case item::misc_type::CapeKings:
            {
                return { ("cape" + FILE_EXT_STR_) };
            }
            case item::misc_type::ShadeCloak:
            {
                return { ("cloak" + FILE_EXT_STR_) };
            }
            case item::misc_type::SpecterRobe:
            {
                return { ("robe" + FILE_EXT_STR_) };
            }
            case item::misc_type::Goblet:
            {
                return MakeFilenames("goblet", 8);
            }
            case item::misc_type::Key:
            {
                return MakeFilenames("key", 11);
            }
            case item::misc_type::LockPicks:
            {
                return MakeFilenames("lockpicks", 6);
            }
            case item::misc_type::Mirror:
            {
                return MakeFilenames("mirror", 10);
            }
            case item::misc_type::DrumLute:
            {
                return MakeFilenames("drumlute", 13);
            }
            case item::misc_type::Orb:
            {
                return MakeFilenames("orb", 9);
            }
            case item::misc_type::Ring:
            {
                if (IS_JEWELED)
                {
                    return MakeFilenames("ring-jeweled", 13);
                }
                else if (IS_BONE)
                {
                    return { ("ring-bone" + FILE_EXT_STR_) };
                }
                else
                {
                    return { "ring" + FILE_EXT_STR_ };
                }
            }
            case item::misc_type::RingHobo:
            {
                return { "ring" + FILE_EXT_STR_ };
            }
            case item::misc_type::Shard:
            {
                return MakeFilenames("shard", 7);
            }
            case item::misc_type::Wand:
            {
                return MakeFilenames("wand", 11);
            }
            case item::misc_type::Scepter:
            {
                return MakeFilenames("scepter", 26);
            }
            case item::misc_type::DollBlessed:
            {
                return MakeFilenames("doll", 4, 2);
            }
            case item::misc_type::DollCursed:
            {
                return MakeFilenames("doll", 10, 5);
            }
            case item::misc_type::Doll:
            {
                return { "doll-1" + FILE_EXT_STR_ };
            }
            case item::misc_type::FigurineBlessed:
            {
                return MakeFilenames("figurine", 6);
            }
            case item::misc_type::FigurineCursed:
            {
                return MakeFilenames("figurine-evil", 6);
            }
            case item::misc_type::Staff:
            {
                return MakeFilenames("staff", 21);
            }
            case item::misc_type::SummoningStatue:
            {
                return MakeFilenames("summoning-statue", 3);
            }
            case item::misc_type::BloodyDragonScale:
            {
                return { "scales" + FILE_EXT_STR_ };
            }
            case item::misc_type::FlagFanatics:
            case item::misc_type::FlagRegalCaptains:
            case item::misc_type::FlagTribal:
            {
                return { "flag" + FILE_EXT_STR_ };
            }
            case item::misc_type::MinotaurHide:
            {
                return { "hide" + FILE_EXT_STR_ };
            }
            case item::misc_type::ReaperScythe:
            {
                return { "bladedstaff-scythe" + FILE_EXT_STR_ };
            }

            // these misc_types are compound words whose filenames have dashes between each word
            case item::misc_type::AngelBraid:
            case item::misc_type::DevilHorn:
            case item::misc_type::GolemFinger:
            case item::misc_type::HurdyGurdy:
            case item::misc_type::LitchHand:
            case item::misc_type::MummyHand:
            case item::misc_type::PetrifiedSnake:
            case item::misc_type::PipeAndTabor:
            case item::misc_type::UnicornHorn:
            case item::misc_type::BasiliskTonge:
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
                    item::misc_type::ToString(MISC_TYPE),
                    SEPARATOR_,
                    misc::CaseChange::LowerToUpper) };

                const auto SEP_STR_LOWER_CASE { misc::ToLowerCopy(SEP_STR) };

                return { SEP_STR_LOWER_CASE + FILE_EXT_STR_ };
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
                return { ba::to_lower_copy(item::misc_type::ToString(MISC_TYPE)) + FILE_EXT_STR_ };
            }

            case item::misc_type::Not:
            case item::misc_type::Count:
            default:
            {
                M_HP_LOG_ERR(
                    "(misc_type=" << item::misc_type::ToString(MISC_TYPE) << ", is_jeweled="
                                  << std::boolalpha << IS_JEWELED << ", is_bone=" << IS_BONE
                                  << ") but that misc_type is somehow invalid.");

                return {};
            }
        }
    }

    const std::string ItemImageLoader::Filename(
        const item::misc_type::Enum MISC_TYPE,
        const bool IS_JEWELED,
        const bool IS_BONE,
        const bool WILL_RANDOMIZE) const
    {
        const auto FILENAMES(Filenames(MISC_TYPE, IS_JEWELED, IS_BONE));

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (FILENAMES.empty() == false),
            "gui::ItemImageLoader::Filename(misc_type="
                << ((MISC_TYPE == item::misc_type::Count) ? "Count"
                                                          : item::misc_type::ToString(MISC_TYPE))
                << ", is_jeweled=" << std::boolalpha << IS_JEWELED << ", is_bone=" << IS_BONE
                << ", will_rand=" << WILL_RANDOMIZE
                << ") unable to get any filenames for those settings.");

        if (WILL_RANDOMIZE)
        {
            return misc::Vector::SelectRandom(FILENAMES);
        }
        else
        {
            return FILENAMES[0];
        }
    }

    const std::string
        ItemImageLoader::GetSkinImageFilename(const item::material::Enum PRIMARY_MATERIAL) const
    {
        auto materialToUseForName { PRIMARY_MATERIAL };
        if (PRIMARY_MATERIAL == item::material::Fur)
        {
            materialToUseForName = item::material::Hide;
        }

        return boost::algorithm::to_lower_copy(
            item::material::ToString(materialToUseForName) + FILE_EXT_STR_);
    }

    const std::string ItemImageLoader::Filename(
        const item::weapon::WeaponTypeWrapper & WEAPON_TYPE_WRAPPER, const bool IS_JEWELED) const
    {
        if (WEAPON_TYPE_WRAPPER.IsStaff())
        {
            if (IS_JEWELED)
            {
                return "staff-2" + FILE_EXT_STR_;
            }
            else
            {
                return "staff-plain" + FILE_EXT_STR_;
            }
        }
        else
        {
            return WEAPON_TYPE_WRAPPER.ImageFilename(SEPARATOR_, FILE_EXT_STR_);
        }
    }

    const std::string
        ItemImageLoader::Filename(const item::armor::ArmorTypeWrapper & ARMOR_TYPE_WRAPPER) const
    {
        if (ARMOR_TYPE_WRAPPER.IsPants() || ARMOR_TYPE_WRAPPER.IsBracers()
            || ARMOR_TYPE_WRAPPER.IsAventail())
        {
            return ARMOR_TYPE_WRAPPER.SpecificName() + FILE_EXT_STR_;
        }
        else
        {
            return ARMOR_TYPE_WRAPPER.ImageFilename(SEPARATOR_, FILE_EXT_STR_);
        }
    }

    void ItemImageLoader::Load(sf::Texture & texture, const std::string & FILENAME) const
    {
        gui::Loaders::Texture(texture, MakeFullPathFromFilename(FILENAME));
    }

    const std::string ItemImageLoader::MakeFullPathFromFilename(const std::string & FILENAME) const
    {
        return misc::filesystem::CombinePathsThenClean(imageDirectoryPath_, FILENAME);
    }

    void ItemImageLoader::EnsureValidDimmensions(
        const sf::Texture & TEXTURE, const std::string & ERROR_MSG) const
    {
        const auto SIZE { TEXTURE.getSize() };

        const auto DIMMENSION { static_cast<unsigned>(MaxDimmension()) };

        M_HP_ASSERT_OR_LOG_AND_THROW(
            ((SIZE.x == DIMMENSION) || (SIZE.y == DIMMENSION)),
            "gui::ItemImageLoader::EnsureValidDimmensions() was given an image of "
            "size="
                << SIZE.x << "x" << SIZE.y << " but neither of those dimmensions was the required "
                << DIMMENSION << ".  This error occured during testing \"" << ERROR_MSG << "\".");
    }

    const std::vector<std::string> ItemImageLoader::MakeFilenames(
        const std::string & PREFIX, const int LAST_NUMBER, const int FIRST_NUMBER) const
    {
        std::vector<std::string> filenames;
        filenames.reserve(static_cast<std::size_t>((LAST_NUMBER - FIRST_NUMBER) + 1));

        for (int i(FIRST_NUMBER); i <= LAST_NUMBER; ++i)
        {
            std::ostringstream ss;
            ss << PREFIX << "-" << i << FILE_EXT_STR_;
            filenames.emplace_back(ss.str());
        }

        return filenames;
    }

} // namespace gui
} // namespace heroespath
