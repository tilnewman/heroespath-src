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

#include "gui/cached-texture.hpp"
#include "gui/content-images.hpp"
#include "item/item-profile.hpp"
#include "item/item-template.hpp"
#include "item/material-enum.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/config-file.hpp"
#include "misc/enum-common.hpp"
#include "misc/filesystem.hpp"
#include "misc/log-macros.hpp"
#include "misc/random.hpp"

#include <sstream>
#include <stdexcept>
#include <vector>

namespace heroespath
{
namespace gui
{

    std::string ItemImagePaths::itemImageDir_ { "" };

    void ItemImagePaths::SetupFilesystemPaths()
    {
        itemImageDir_ = misc::ConfigFile::Instance()->GetMediaPath("media-image-item-dir");

        M_HP_ASSERT_OR_LOG_AND_THROW(
            misc::filesystem::ExistsAndIsDirectory(itemImageDir_),
            "Item image directory does not exist or is not a directory."
                + M_HP_VAR_STR(itemImageDir_));
    }

    const std::string
        ItemImagePaths::Filename(const item::ItemProfile & PROFILE, const bool WILL_RANDOMIZE)
    {
        const auto FILENAMES = Filenames(PROFILE);

        if (FILENAMES.empty())
        {
            M_HP_LOG_ERR(
                "Failed to generate any image filenames for item profile: " << PROFILE.ToString());

            return std::string(ContentImage::FilenameError(true));
        }
        else
        {
            if (WILL_RANDOMIZE)
            {
                return misc::RandomSelect(FILENAMES);
            }
            else
            {
                return FILENAMES[0];
            }
        }
    }

    const std::vector<std::string> ItemImagePaths::Filenames(const item::ItemProfile & PROFILE)
    {
        std::vector<std::string> filenames;
        filenames.reserve(1);

        // Misc must come before weapon or armor because if an item is misc+weapon or
        // misc+armor then the image is based on the misc type.
        if (PROFILE.IsMisc())
        {
            filenames = MiscFilenames(PROFILE);
        }
        else if (PROFILE.IsWeapon())
        {
            filenames.emplace_back(WeaponFilename(PROFILE));
        }
        else
        {
            filenames.emplace_back(ArmorFilename(PROFILE));
        }

        if (filenames.empty())
        {
            M_HP_LOG_ERR(
                "Failed to generate any image filenames for item profile: " << PROFILE.ToString());

            filenames.emplace_back(ContentImage::FilenameError(false));
        }

        for (auto & rawFilename : filenames)
        {
            if (rawFilename.empty())
            {
                rawFilename = ContentImage::FilenameError(false);
            }

            rawFilename = misc::CamelTo(
                rawFilename, ContentImage::FilenameSeparator(), misc::CaseChange::LowerToUpper);

            misc::ToLower(rawFilename);

            for (char & c : rawFilename)
            {
                if (!misc::IsAlphaOrDigit(c))
                {
                    c = ContentImage::FilenameSeparatorChar();
                }
            }

            rawFilename += ContentImage::FilenameExtension();
        }

        return filenames;
    }

    const std::string ItemImagePaths::PathFromFilename(const std::string & FILE_NAME)
    {
        std::string path;
        path.reserve(64);

        if (FILE_NAME == ContentImage::FilenameTodo(true))
        {
            path += ContentImage::PathFullTodo();
        }
        else if (FILE_NAME == ContentImage::FilenameError(true))
        {
            path += ContentImage::PathFullError();
        }
        else
        {
            path += misc::filesystem::CombinePaths(itemImageDir_, FILE_NAME);
        }

        return path;
    }

    const std::vector<std::string> ItemImagePaths::MiscFilenames(const item::ItemProfile & PROFILE)
    {
        const auto MISC = PROFILE.MiscType();

        switch (MISC)
        {
            // these Miscs have specific filenames
            case item::Misc::FlagFanatics:
            case item::Misc::FlagRegalCaptains:
            case item::Misc::FlagTribal: return { "flag" };

            case item::Misc::CapeCommanders:
            case item::Misc::CapeGenerals:
            case item::Misc::CapeKings: return { "cape" };

            case item::Misc::ManaAmulet:
            case item::Misc::Pendant: return MakeNumberedFilenames("amulet", 23);

            case item::Misc::Ring:
            {
                if (PROFILE.MaterialPrimary() == item::Material::Bone)
                {
                    return { "ring-bone" };
                }
                else if (item::Material::IsJeweled(
                             PROFILE.MaterialPrimary(), PROFILE.MaterialSecondary()))
                {
                    return MakeNumberedFilenames("ring-jeweled", 13);
                }
                else
                {
                    return { "ring" };
                }
            }

            case item::Misc::ShadeCloak: return { "cloak" };
            case item::Misc::SpecterRobe: return { "robe" };
            case item::Misc::RingHobo: return { "ring" };
            case item::Misc::Doll: return { "doll-1" };
            case item::Misc::BloodyDragonScale: return { "scales" };
            case item::Misc::MinotaurHide: return { "hide" };
            case item::Misc::ReaperScythe: return { "bladedstaff-scythe" };
            case item::Misc::Goblet: return MakeNumberedFilenames("goblet", 8);
            case item::Misc::Key: return MakeNumberedFilenames("key", 11);
            case item::Misc::LockPicks: return MakeNumberedFilenames("lockpicks", 6);
            case item::Misc::Mirror: return MakeNumberedFilenames("mirror", 10);
            case item::Misc::DrumLute: return MakeNumberedFilenames("drumlute", 13);
            case item::Misc::Orb: return MakeNumberedFilenames("orb", 9);
            case item::Misc::Shard: return MakeNumberedFilenames("shard", 7);
            case item::Misc::Wand: return MakeNumberedFilenames("wand", 11);
            case item::Misc::Scepter: return MakeNumberedFilenames("scepter", 26);
            case item::Misc::DollBlessed: return MakeNumberedFilenames("doll", 4, 2);
            case item::Misc::DollCursed: return MakeNumberedFilenames("doll", 10, 5);
            case item::Misc::FigurineBlessed: return MakeNumberedFilenames("figurine", 6);
            case item::Misc::FigurineCursed: return MakeNumberedFilenames("figurine-evil", 6);
            case item::Misc::SummoningStatue: return MakeNumberedFilenames("summoning-statue", 3);

            // these Miscs have single word names that are simply converted to lower-case
            case item::Misc::Crumhorn:
            case item::Misc::Icicle:
            case item::Misc::Lyre:
            case item::Misc::Recorder:
            case item::Misc::Viol:
            case item::Misc::Bust:
            case item::Misc::Egg:
            case item::Misc::Embryo:
            case item::Misc::Seeds:
                // fallthrough

            // these Miscs are compound words whose filenames have dashes between each word but that
            // will be done by Filenames() above
            case item::Misc::AngelBraid:
            case item::Misc::DevilHorn:
            case item::Misc::GolemFinger:
            case item::Misc::HurdyGurdy:
            case item::Misc::LichHand:
            case item::Misc::MummyHand:
            case item::Misc::PetrifiedSnake:
            case item::Misc::PipeAndTabor:
            case item::Misc::UnicornHorn:
            case item::Misc::BasiliskTongue:
            case item::Misc::BerserkersBeard:
            case item::Misc::BishopsHanky:
            case item::Misc::BleedingTrophy:
            case item::Misc::BottleOfBansheeScreams:
            case item::Misc::BronzeTroll:
            case item::Misc::BurialShroud:
            case item::Misc::ChimeraBone:
            case item::Misc::CobraTooth:
            case item::Misc::CrystalChimes:
            case item::Misc::DemonDiary:
            case item::Misc::DoveBloodVial:
            case item::Misc::DragonToothWhistle:
            case item::Misc::DriedFrog:
            case item::Misc::DriedGecko:
            case item::Misc::DriedIguana:
            case item::Misc::DriedLizard:
            case item::Misc::DriedSalamander:
            case item::Misc::DriedSkink:
            case item::Misc::DriedToad:
            case item::Misc::DruidLeaf:
            case item::Misc::EvilRabbitsFoot:
            case item::Misc::ExoticGoldenGong:
            case item::Misc::EyeCyclops:
            case item::Misc::EyeGiantOwl:
            case item::Misc::EyeHawk:
            case item::Misc::FriarsChronicle:
            case item::Misc::FuneralRecord:
            case item::Misc::GhostSheet:
            case item::Misc::GlassCat:
            case item::Misc::GriffinFeather:
            case item::Misc::HangmansNoose:
            case item::Misc::HobgoblinNose:
            case item::Misc::HolyEpic:
            case item::Misc::HornOfTheHorde:
            case item::Misc::ImpTail:
            case item::Misc::IslanderHeaddress:
            case item::Misc::JeweledArmband:
            case item::Misc::JeweledHandbag:
            case item::Misc::JeweledPrincessVeil:
            case item::Misc::LastRitesScroll:
            case item::Misc::MacabreManuscript:
            case item::Misc::MadRatJuju:
            case item::Misc::MagicHorseshoe:
            case item::Misc::MagnifyingGlass:
            case item::Misc::MaskMourners:
            case item::Misc::MaskRascal:
            case item::Misc::MortuaryOrnament:
            case item::Misc::NecklaceJeweledAnkh:
            case item::Misc::NecklaceSharkTooth:
            case item::Misc::NecklaceVampiresTooth:
            case item::Misc::PantherPaw:
            case item::Misc::PixieBell:
            case item::Misc::RattlesnakeTail:
            case item::Misc::RavenClaw:
            case item::Misc::RequiemRegister:
            case item::Misc::RingMendicant:
            case item::Misc::RingMonk:
            case item::Misc::RingPriest:
            case item::Misc::RoyalScoutSpyglass:
            case item::Misc::SaintCameoPin:
            case item::Misc::SaintsJournal:
            case item::Misc::SanguineRelic:
            case item::Misc::ScoundrelSack:
            case item::Misc::SepultureDecoration:
            case item::Misc::ShamanRainmaker:
            case item::Misc::SirenConch:
            case item::Misc::SpecterChains:
            case item::Misc::SpiderEggs:
            case item::Misc::SprintersLegtie:
            case item::Misc::SwindlersBag:
            case item::Misc::TricksterPouch:
            case item::Misc::TuningFork:
            case item::Misc::TurtleShell:
            case item::Misc::VultureGizzard:
            case item::Misc::WarhorseMarionette:
            case item::Misc::WarTrumpet:
            case item::Misc::WeaselTotem:
            case item::Misc::WolfenFur:
            case item::Misc::WraithTalisman:
            case item::Misc::DriedHead:
            case item::Misc::PuppetBlessed:
            case item::Misc::PuppetCursed:
            case item::Misc::BroochCrown:
            case item::Misc::BroochFeather:
            case item::Misc::BroochFist:
            case item::Misc::BroochHourglass:
            case item::Misc::BroochKey:
            case item::Misc::BroochMask:
            case item::Misc::CharmCrown:
            case item::Misc::CharmFeather:
            case item::Misc::CharmFist:
            case item::Misc::CharmHourglass:
            case item::Misc::CharmKey:
            case item::Misc::CharmMask:
            case item::Misc::PinCrown:
            case item::Misc::PinFeather:
            case item::Misc::PinFist:
            case item::Misc::PinHourglass:
            case item::Misc::PinKey:
            case item::Misc::PinMask:
            {
                return { std::string(NAMEOF_ENUM(MISC)) };
            }

            case item::Misc::Count:
            default:
            {
                M_HP_LOG_ERR("Unable to generate a filename for: " << PROFILE.ToString());
                return { std::string(ContentImage::FilenameError(false)) };
            }
        }
    }

    const std::string ItemImagePaths::WeaponFilename(const item::ItemProfile & PROFILE)
    {
        const auto & WEAPON_INFO = PROFILE.WeaponInfo();

        std::string str;
        str.reserve(32);
        str += WEAPON_INFO.TypeString();
        str += ContentImage::FilenameSeparator();
        str += WEAPON_INFO.SubTypeString();
        return str;
    }

    const std::string ItemImagePaths::ArmorFilename(const item::ItemProfile & PROFILE)
    {
        const auto & ARMOR_INFO = PROFILE.ArmorInfo();

        std::string str;
        str.reserve(32);

        if (ARMOR_INFO.IsSkin())
        {
            str += SkinArmorFilename(PROFILE);
        }
        else
        {
            str += ARMOR_INFO.TypeString();

            // these vary by item::Forms::Enum but I don't have images for those variations yet
            if (!ARMOR_INFO.IsAventail() && !ARMOR_INFO.IsBracer() && !ARMOR_INFO.IsPant())
            {
                str += ContentImage::FilenameSeparator();
                str += ARMOR_INFO.SubTypeString();

                if (ARMOR_INFO.HasForm())
                {
                    str += ContentImage::FilenameSeparator();
                    str += NAMEOF_ENUM(ARMOR_INFO.FormEnum());
                }
            }
        }

        return str;
    }

    const std::string ItemImagePaths::SkinArmorFilename(const item::ItemProfile & PROFILE)
    {
        switch (PROFILE.ArmorInfo().MinorAs<item::Skins>())
        {
            // still neeed to find an image for fur, so use hide until then
            case item::Skins::Fur:
            case item::Skins::Hide: return "hide";

            case item::Skins::Plant:
            {
                std::string plantSkinStr;
                plantSkinStr.reserve(16);
                plantSkinStr += NAMEOF_ENUM(item::Weapon::BodyPart);
                plantSkinStr += ContentImage::FilenameSeparator();
                plantSkinStr += NAMEOF_ENUM(PROFILE.MaterialPrimary());
                return plantSkinStr;
            }

            case item::Skins::Scale: return "scales";

            case item::Skins::Flesh:
            case item::Skins::Count:
            default: return std::string(ContentImage::FilenameError(false));
        }
    }

    const std::vector<std::string> ItemImagePaths::MakeNumberedFilenames(
        const std::string_view PREFIX, const int LAST_NUMBER, const int FIRST_NUMBER)
    {
        const int DIFF((LAST_NUMBER - FIRST_NUMBER) + 1);

        if (DIFF <= 0)
        {
            M_HP_LOG_ERR(
                "prefix=" << PREFIX << ", first_number=" << std::to_string(FIRST_NUMBER)
                          << ", last_number=" << std::to_string(LAST_NUMBER) << ":  first <= last");

            return { std::string(ContentImage::FilenameError(false)) };
        }
        else
        {
            std::vector<std::string> filenames;
            filenames.reserve(static_cast<std::size_t>(DIFF));

            std::string prefixWithSeparator;
            prefixWithSeparator += PREFIX;
            prefixWithSeparator += ContentImage::FilenameSeparator();

            for (int i(FIRST_NUMBER); i <= LAST_NUMBER; ++i)
            {
                filenames.emplace_back(prefixWithSeparator + std::to_string(i));
            }

            return filenames;
        }
    }

} // namespace gui
} // namespace heroespath
