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
#include "game/game-data-file.hpp"
#include "game/loop-manager.hpp"
#include "item/item.hpp"
#include "log/log-macros.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/boost-string-includes.hpp"
#include "misc/enum-util.hpp"
#include "misc/filesystem-helpers.hpp"
#include "misc/random.hpp"

#include "sfml-util/loaders.hpp"

#include <boost/filesystem.hpp>

#include <cctype>
#include <exception>
#include <sstream>
#include <vector>

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        const std::string ItemImageLoader::FILE_EXT_STR_{ ".png" };
        const std::string ItemImageLoader::SEPARATOR_{ "-" };

        ItemImageLoader::ItemImageLoader()
            : imageDirectoryPath_("")
        {
            imageDirectoryPath_ = misc::filesystem::MakePathPretty(
                game::GameDataFile::Instance()->GetMediaPath("media-images-items-dir"));
        }

        bool ItemImageLoader::Test() const
        {
            using namespace item;

            static auto hasInitialPrompt{ false };
            if (false == hasInitialPrompt)
            {
                hasInitialPrompt = true;
                game::LoopManager::Instance()->TestingStrAppend(
                    "sfml_util::gui::ItemImageLoader::Test()  Starting tests...");
            }

            const std::string TEST_PRE_STR{ "ItemImageLoader Test " };

            static auto allFilenames{ misc::filesystem::FindFilesInDirectory(
                imageDirectoryPath_,
                "",
                FILE_EXT_STR_,
                misc::filesystem::FilenameText::TO_EXCLUDE_VEC_) };

            static auto const WEAPON_TYPE_WRAPPERS{ weapon::WeaponTypeWrapper::MakeCompleteSet() };

            static std::size_t weaponIndex{ 0 };
            if (weaponIndex < WEAPON_TYPE_WRAPPERS.size())
            {
                auto const & WEAPON_TYPE_WRAPPER{ WEAPON_TYPE_WRAPPERS.at(weaponIndex) };

                auto const FILENAME{ Filename(WEAPON_TYPE_WRAPPER, false) };

                sf::Texture texture;
                Load(texture, FILENAME);

                game::LoopManager::Instance()->TestingImageSet(
                    texture, true, "item", WEAPON_TYPE_WRAPPER.ReadableName());

                game::LoopManager::Instance()->TestingStrIncrement(
                    TEST_PRE_STR + WEAPON_TYPE_WRAPPER.ReadableName());

                EnsureValidDimmensions(texture, WEAPON_TYPE_WRAPPER.ReadableName());

                allFilenames.erase(
                    std::remove(std::begin(allFilenames), std::end(allFilenames), FILENAME),
                    std::end(allFilenames));

                ++weaponIndex;
                return false;
            }

            static auto const ARMOR_TYPE_WRAPPERS{ armor::ArmorTypeWrapper::MakeCompleteSet() };

            static std::size_t armorIndex{ 0 };
            if (armorIndex < ARMOR_TYPE_WRAPPERS.size())
            {
                auto const & ARMOR_TYPE_WRAPPER{ ARMOR_TYPE_WRAPPERS.at(armorIndex) };

                auto const FILENAME{ Filename(ARMOR_TYPE_WRAPPER) };

                // skip testing skin images because we need creatures for that, and skin images will
                // get tested when we test creature creation
                if (ARMOR_TYPE_WRAPPER.IsSkin() == false)
                {
                    sf::Texture texture;
                    Load(texture, FILENAME);

                    game::LoopManager::Instance()->TestingImageSet(
                        texture, true, "item", ARMOR_TYPE_WRAPPER.ReadableName());

                    game::LoopManager::Instance()->TestingStrIncrement(
                        TEST_PRE_STR + ARMOR_TYPE_WRAPPER.ReadableName());

                    EnsureValidDimmensions(texture, ARMOR_TYPE_WRAPPER.ReadableName());
                }

                allFilenames.erase(
                    std::remove(std::begin(allFilenames), std::end(allFilenames), FILENAME),
                    std::end(allFilenames));

                ++armorIndex;
                return false;
            }

            // test misc items
            static auto miscIndex{ 1 }; // start at 1 to avoid misc_type::Not
            static auto isJeweled{ true };
            static auto isBone{ false };
            if (miscIndex < static_cast<int>(misc_type::Count))
            {
                auto const ENUM{ static_cast<misc_type::Enum>(miscIndex) };

                // skip misc items that are weapons or armor because they will require a TypeWrapper
                // object to construct the image filename, and will have already been tested by
                // ItemFactory
                if (misc_type::IsWeapon(ENUM) || misc_type::IsArmor(ENUM))
                {
                    ++miscIndex;
                    return false;
                }

                auto const ENUM_STR{ misc_type::ToString(ENUM) };
                auto const FILENAMES_VEC = Filenames(ENUM, isJeweled, isBone);

                M_ASSERT_OR_LOGANDTHROW_SS(
                    (FILENAMES_VEC.empty() == false),
                    "sfml_util::gui::ItemImageLoader::Test() While testing misc item #"
                        << miscIndex << " \"" << ENUM_STR << "\", is_jeweled=" << std::boolalpha
                        << isJeweled << ", Filenames() returned an empty vector.");

                static std::size_t fileIndex{ 0 };
                if (fileIndex < FILENAMES_VEC.size())
                {
                    auto const NEXT_FILENAME{ FILENAMES_VEC[fileIndex] };

                    M_ASSERT_OR_LOGANDTHROW_SS(
                        (NEXT_FILENAME.empty() == false),
                        "sfml_util::gui::ItemImageLoader::Test() (rand)  "
                            << "While testing misc item #" << miscIndex << " \"" << ENUM_STR
                            << "\", filename #" << fileIndex << ", is_jeweled=" << std::boolalpha
                            << isJeweled << ", found an empty filename string.");

                    std::ostringstream ss;
                    ss << "ItemImageLoader Testing \"" << ENUM_STR << "\", file_index=" << fileIndex
                       << " (" << ((isJeweled) ? "jeweled" : "not-jeweled") << ")";

                    game::LoopManager::Instance()->TestingStrIncrement(ss.str());

                    sf::Texture texture;
                    Load(texture, NEXT_FILENAME);

                    game::LoopManager::Instance()->TestingImageSet(
                        texture, true, "items/misc", ENUM_STR, NEXT_FILENAME);

                    EnsureValidDimmensions(texture, ENUM_STR);

                    allFilenames.erase(
                        std::remove(
                            std::begin(allFilenames), std::end(allFilenames), NEXT_FILENAME),
                        std::end(allFilenames));

                    ++fileIndex;
                    return false;
                }

                if (false == isJeweled)
                {
                    isJeweled = true;
                    return false;
                }
                else
                {
                    isJeweled = false;
                }

                fileIndex = 0;
                ++miscIndex;
                return false;
            }

            for (auto const & FILENAME : allFilenames)
            {
                M_HP_LOG_WRN(
                    "sfml_util::gui::ItemImageLoader::Test() found the following item image "
                    "unused: "
                    << FILENAME);
            }

            game::LoopManager::Instance()->TestingStrAppend(
                "sfml_util::gui::ItemImageLoader::Test()  ALL TESTS PASSED.");

            return true;
        }

        void ItemImageLoader::Load(sf::Texture & texture, const item::ItemPtr_t ITEM_PTR) const
        {
            Load(texture, ITEM_PTR->ImageFilename());
        }

        const std::string ItemImageLoader::Path(const item::ItemPtr_t ITEM_PTR) const
        {
            return MakeFullPathFromFilename(ITEM_PTR->ImageFilename());
        }

        const std::string ItemImageLoader::Filename(
            const item::ItemPtr_t ITEM_PTR, const bool WILL_RANDOMIZE) const
        {
            // it is important to check for weapon/armor status first, because some misc_types are
            // weapons/armor and their image filenames will not be correctly identified by the
            // misc_type logic

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
            else if (ITEM_PTR->IsMisc())
            {
                return Filename(
                    ITEM_PTR->MiscType(),
                    ITEM_PTR->IsJeweled(),
                    (ITEM_PTR->MaterialPrimary() == item::material::Bone),
                    WILL_RANDOMIZE);
            }

            std::ostringstream ss;
            ss << "sfml_util::gui::ItemImageLoader::Filename(item={" << ITEM_PTR->ToString()
               << "}, will_randomize=" << std::boolalpha << WILL_RANDOMIZE
               << ") failed to find the image filename for that item because it was not weapon, "
                  "armor, or misc type.";

            throw std::runtime_error(ss.str());
        }

        bool ItemImageLoader::DoesFileExist(const item::ItemPtr_t ITEM_PTR) const
        {
            return misc::filesystem::DoesFileExist(ITEM_PTR->ImageFilename());
        }

        const std::vector<std::string> ItemImageLoader::Filenames(
            const item::misc_type::Enum MISC_TYPE, const bool IS_JEWELED, const bool IS_BONE) const
        {
            using namespace item;
            namespace ba = boost::algorithm;

            switch (MISC_TYPE)
            {
                // these misc_types are compound names where underscores need to be converted to
                // dashes
                case misc_type::Brooch_Crown:
                case misc_type::Brooch_Feather:
                case misc_type::Brooch_Fist:
                case misc_type::Brooch_Hourglass:
                case misc_type::Brooch_Key:
                case misc_type::Brooch_Mask:
                case misc_type::Charm_Crown:
                case misc_type::Charm_Feather:
                case misc_type::Charm_Fist:
                case misc_type::Charm_Hourglass:
                case misc_type::Charm_Key:
                case misc_type::Charm_Mask:
                case misc_type::Pin_Crown:
                case misc_type::Pin_Feather:
                case misc_type::Pin_Fist:
                case misc_type::Pin_Hourglass:
                case misc_type::Pin_Key:
                case misc_type::Pin_Mask:
                {
                    auto const TYPE_STR{ ba::replace_all_copy(
                        ba::to_lower_copy(misc_type::ToString(MISC_TYPE)), "_", SEPARATOR_) };

                    return { (TYPE_STR + FILE_EXT_STR_) };
                }

                    // these misc_types have specific filenames
                case misc_type::ManaAmulet:
                case misc_type::Pendant:
                {
                    return MakeFilenames("amulet", 23);
                }
                case misc_type::CapeCommanders:
                case misc_type::CapeGenerals:
                case misc_type::CapeKings:
                {
                    return { ("cape" + FILE_EXT_STR_) };
                }
                case misc_type::ShadeCloak:
                {
                    return { ("cloak" + FILE_EXT_STR_) };
                }
                case misc_type::SpecterRobe:
                {
                    return { ("robe" + FILE_EXT_STR_) };
                }
                case misc_type::Goblet:
                {
                    return MakeFilenames("goblet", 8);
                }
                case misc_type::Key:
                {
                    return MakeFilenames("key", 11);
                }
                case misc_type::LockPicks:
                {
                    return MakeFilenames("lockpicks", 7);
                }
                case misc_type::Mirror:
                {
                    return MakeFilenames("mirror", 10);
                }
                case misc_type::DrumLute:
                {
                    return MakeFilenames("drumlute", 21);
                }
                case misc_type::Orb:
                {
                    return MakeFilenames("orb", 9);
                }
                case misc_type::Ring:
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
                case misc_type::RingHobo:
                {
                    return { "ring" + FILE_EXT_STR_ };
                }
                case misc_type::Shard:
                {
                    return MakeFilenames("shard", 7);
                }
                case misc_type::Wand:
                {
                    return MakeFilenames("wand", 11);
                }
                case misc_type::Scepter:
                {
                    return MakeFilenames("scepter", 26);
                }
                case misc_type::DollBlessed:
                {
                    return MakeFilenames("doll", 5, 2);
                }
                case misc_type::DollCursed:
                {
                    return MakeFilenames("doll", 11, 6);
                }
                case misc_type::Doll:
                {
                    return { "doll-1" + FILE_EXT_STR_ };
                }
                case misc_type::FigurineBlessed:
                {
                    return MakeFilenames("figurine", 6);
                }
                case misc_type::FigurineCursed:
                {
                    return MakeFilenames("figurine-evil", 6);
                }
                case misc_type::Staff:
                {
                    return MakeFilenames("staff", 21);
                }
                case misc_type::SummoningStatue:
                {
                    return MakeFilenames("summoning-statue", 3);
                }
                case misc_type::BloodyDragonScale:
                {
                    return { "scales" + FILE_EXT_STR_ };
                }
                case misc_type::FlagFanatics:
                case misc_type::FlagRegalCaptains:
                case misc_type::FlagTribal:
                {
                    return { "flag" + FILE_EXT_STR_ };
                }
                case misc_type::MinotaurHide:
                {
                    return { "hide" + FILE_EXT_STR_ };
                }
                case misc_type::ReaperScythe:
                {
                    return { "bladedstaff-scythe" + FILE_EXT_STR_ };
                }

                // these misc_types are compound words whose filenames have dashes between each word
                case misc_type::AngelBraid:
                case misc_type::DevilHorn:
                case misc_type::GolemFinger:
                case misc_type::HurdyGurdy:
                case misc_type::LitchHand:
                case misc_type::MummyHand:
                case misc_type::PetrifiedSnake:
                case misc_type::PipeAndTabor:
                case misc_type::UnicornHorn:
                case misc_type::BasiliskTonge:
                case misc_type::BerserkersBeard:
                case misc_type::BishopsHanky:
                case misc_type::BleedingTrophy:
                case misc_type::BottleOfBansheeScreams:
                case misc_type::BronzeTroll:
                case misc_type::BurialShroud:
                case misc_type::ChimeraBone:
                case misc_type::CobraTooth:
                case misc_type::CrystalChimes:
                case misc_type::DemonDiary:
                case misc_type::DoveBloodVial:
                case misc_type::DragonToothWhistle:
                case misc_type::DriedFrog:
                case misc_type::DriedGecko:
                case misc_type::DriedIguana:
                case misc_type::DriedLizard:
                case misc_type::DriedSalamander:
                case misc_type::DriedSkink:
                case misc_type::DriedToad:
                case misc_type::DruidLeaf:
                case misc_type::EvilRabbitsFoot:
                case misc_type::ExoticGoldenGong:
                case misc_type::EyeCyclops:
                case misc_type::EyeGiantOwl:
                case misc_type::EyeHawk:
                case misc_type::FriarsChronicle:
                case misc_type::FuneralRecord:
                case misc_type::GhostSheet:
                case misc_type::GlassCat:
                case misc_type::GriffinFeather:
                case misc_type::HangmansNoose:
                case misc_type::HobgoblinNose:
                case misc_type::HolyEpic:
                case misc_type::HornOfTheHorde:
                case misc_type::ImpTail:
                case misc_type::IslanderHeaddress:
                case misc_type::JeweledArmband:
                case misc_type::JeweledHandbag:
                case misc_type::JeweledPrincessVeil:
                case misc_type::LastRitesScroll:
                case misc_type::MacabreManuscript:
                case misc_type::MadRatJuju:
                case misc_type::MagicHorseshoe:
                case misc_type::MagnifyingGlass:
                case misc_type::MaskMourners:
                case misc_type::MaskRascal:
                case misc_type::MortuaryOrnament:
                case misc_type::NecklaceJeweledAnkh:
                case misc_type::NecklaceSharkTooth:
                case misc_type::NecklaceVampiresTooth:
                case misc_type::PantherPaw:
                case misc_type::PixieBell:
                case misc_type::RattlesnakeTail:
                case misc_type::RavenClaw:
                case misc_type::RequiemRegister:
                case misc_type::RingMendicant:
                case misc_type::RingMonk:
                case misc_type::RingPriest:
                case misc_type::RoyalScoutSpyglass:
                case misc_type::SaintCameoPin:
                case misc_type::SaintsJournal:
                case misc_type::SanguineRelic:
                case misc_type::ScoundrelSack:
                case misc_type::SepultureDecoration:
                case misc_type::ShamanRainmaker:
                case misc_type::SirenConch:
                case misc_type::SpecterChains:
                case misc_type::SpiderEggs:
                case misc_type::SprintersLegtie:
                case misc_type::SwindlersBag:
                case misc_type::TricksterPouch:
                case misc_type::TuningFork:
                case misc_type::TurtleShell:
                case misc_type::VultureGizzard:
                case misc_type::WarhorseMarionette:
                case misc_type::WarTrumpet:
                case misc_type::WeaselTotem:
                case misc_type::WolfenFur:
                case misc_type::WraithTalisman:
                case misc_type::DriedHead:
                case misc_type::PuppetBlessed:
                case misc_type::PuppetCursed:
                {
                    std::string filename{ "" };

                    auto const MISC_TYPE_STR{ misc_type::ToString(MISC_TYPE) };
                    auto const MISC_TYPE_STR_SIZE{ MISC_TYPE_STR.size() };

                    for (std::size_t i(0); i < MISC_TYPE_STR_SIZE; ++i)
                    {
                        auto const CURRENT_CHAR{ MISC_TYPE_STR[i] };
                        filename.push_back(static_cast<char>(std::tolower(CURRENT_CHAR)));

                        if ((MISC_TYPE_STR_SIZE - 1) > i)
                        {
                            auto const NEXT_CHAR{ MISC_TYPE_STR[i + 1] };
                            if (std::islower(CURRENT_CHAR) && std::isupper(NEXT_CHAR))
                            {
                                filename.push_back(SEPARATOR_[0]);
                            }
                        }
                    }

                    return { filename + FILE_EXT_STR_ };
                }

                // these misc_types have single word names that are simply converted to lower-case
                case misc_type::Crumhorn:
                case misc_type::Icicle:
                case misc_type::Lyre:
                case misc_type::Recorder:
                case misc_type::Viol:
                case misc_type::Bust:
                case misc_type::Egg:
                case misc_type::Embryo:
                case misc_type::Seeds:
                {
                    return { ba::to_lower_copy(misc_type::ToString(MISC_TYPE)) + FILE_EXT_STR_ };
                }

                case misc_type::Not:
                case misc_type::Count:
                default:
                {
                    std::ostringstream ss;
                    ss << "sfml_util::gui::ItemImageLoader::Filenames(misc_type="
                       << ((MISC_TYPE == misc_type::Count) ? "Count"
                                                           : misc_type::ToString(MISC_TYPE))
                       << ", is_jeweled=" << std::boolalpha << IS_JEWELED << ", is_bone=" << IS_BONE
                       << ") but that misc_type is somehow invalid.";

                    throw std::range_error(ss.str());
                }
            }
        }

        const std::string ItemImageLoader::Filename(
            const item::misc_type::Enum MISC_TYPE,
            const bool IS_JEWELED,
            const bool IS_BONE,
            const bool WILL_RANDOMIZE) const
        {
            auto const FILENAMES(Filenames(MISC_TYPE, IS_JEWELED, IS_BONE));

            M_ASSERT_OR_LOGANDTHROW_SS(
                (FILENAMES.empty() == false),
                "sfml_util::gui::ItemImageLoader::Filename(misc_type="
                    << ((MISC_TYPE == item::misc_type::Count)
                            ? "Count"
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
            using namespace item;

            auto materialToUseForName{ PRIMARY_MATERIAL };
            if (PRIMARY_MATERIAL == material::Fur)
            {
                materialToUseForName = material::Hide;
            }

            return boost::algorithm::to_lower_copy(
                material::ToString(materialToUseForName) + FILE_EXT_STR_);
        }

        const std::string ItemImageLoader::Filename(
            const item::weapon::WeaponTypeWrapper & WEAPON_TYPE_WRAPPER,
            const bool IS_JEWELED) const
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

        const std::string ItemImageLoader::Filename(
            const item::armor::ArmorTypeWrapper & ARMOR_TYPE_WRAPPER) const
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
            sfml_util::Loaders::Texture(texture, MakeFullPathFromFilename(FILENAME));
        }

        const std::string
            ItemImageLoader::MakeFullPathFromFilename(const std::string & FILENAME) const
        {
            namespace bfs = boost::filesystem;
            const bfs::path PATH{ bfs::path(imageDirectoryPath_) / bfs::path(FILENAME) };
            return PATH.string();
        }

        void ItemImageLoader::EnsureValidDimmensions(
            const sf::Texture & TEXTURE, const std::string & ERROR_MSG) const
        {
            auto const SIZE{ TEXTURE.getSize() };

            auto const DIMMENSION{ static_cast<unsigned>(MaxDimmension()) };

            M_ASSERT_OR_LOGANDTHROW_SS(
                ((SIZE.x == DIMMENSION) || (SIZE.y == DIMMENSION)),
                "sfml_util::gui::ItemImageLoader::EnsureValidDimmensions() was given an image of "
                "size="
                    << SIZE.x << "x" << SIZE.y
                    << " but neither of those dimmensions was the required " << DIMMENSION
                    << ".  This error occured during testing \"" << ERROR_MSG << "\".");
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
} // namespace sfml_util
} // namespace heroespath
