// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// creature-image-manager.cpp
//
#include "creature-image-paths.hpp"

#include "creature/body-type.hpp"
#include "creature/creature.hpp"
#include "gui/cached-texture.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/config-file.hpp"
#include "misc/filesystem.hpp"
#include "misc/log-macros.hpp"
#include "misc/random.hpp"
#include "sfutil/image-manip.hpp"
#include "stage/i-stage.hpp"

#include <sstream>

namespace heroespath
{
namespace gui
{

    std::string CreatureImagePaths::imageDirectoryPath_ { "" };

    void CreatureImagePaths::SetupFilesystemPaths()
    {
        imageDirectoryPath_
            = misc::ConfigFile::Instance()->GetMediaPath("media-image-creature-dir");

        M_HP_ASSERT_OR_LOG_AND_THROW(
            misc::filesystem::ExistsAndIsDirectory(imageDirectoryPath_),
            "Creature image directory does not exist or is not a directory."
                + M_HP_VAR_STR(imageDirectoryPath_));
    }

    const std::string CreatureImagePaths::PathFromFilename(const std::string & FILENAME)
    {
        return misc::filesystem::CombinePaths(imageDirectoryPath_, FILENAME);
    }

    const std::string CreatureImagePaths::FilenameRandom(const creature::Creature & CREATURE)
    {
        const auto FILENAMES { Filenames(
            CREATURE.Race(),
            CREATURE.Role(),
            CREATURE.Sex(),
            CREATURE.WolfenClassType(),
            CREATURE.DragonClass()) };

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (FILENAMES.empty() == false),
            "gui::CreatureImagePaths::GetRandomFilename(creature={"
                << CREATURE.ToString()
                << "}) (which actually calls GetFilenames()) returned no filenames.");

        return misc::RandomSelect(FILENAMES);
    }

    bool CreatureImagePaths::WillHorizFlipToFaceRight(const creature::Creature & CREATURE)
    {
        return CREATURE.IsPlayerCharacter();
    }

    const std::vector<std::string> CreatureImagePaths::Filenames(
        const creature::race::Enum RACE,
        const creature::role::Enum ROLE,
        const creature::sex::Enum SEX,
        const creature::WolfenClass::Enum WOLFEN_CLASS,
        const creature::dragon_class::Enum DRAGON_CLASS)
    {
        if (RACE == creature::race::Troll)
        {
            if (ROLE == creature::role::Berserker)
            {
                return { "troll-berserker.png" };
            }

            if (ROLE == creature::role::Brute)
            {
                return { "troll-brute.png" };
            }

            if (ROLE == creature::role::Chieftain)
            {
                return { "troll-chieftain.png" };
            }

            if (ROLE == creature::role::Grunt)
            {
                return { "troll-grunt-1.png", "troll-grunt-2.png" };
            }

            if (ROLE == creature::role::Mountain)
            {
                return { "troll-mountain.png" };
            }

            if (ROLE == creature::role::Shaman)
            {
                return { "troll-shaman.png" };
            }

            if (ROLE == creature::role::Smasher)
            {
                return { "troll-smasher.png" };
            }

            if (ROLE == creature::role::Strangler)
            {
                return { "troll-strangler.png" };
            }

            if (ROLE == creature::role::Warlord)
            {
                return { "troll-warlord.png" };
            }

            if (ROLE == creature::role::TwoHeaded)
            {
                return { "troll-two-headed.png" };
            }
        }

        if (RACE == creature::race::Orc)
        {
            if (ROLE == creature::role::Berserker)
            {
                return { "orc-berserker.png" };
            }

            if (ROLE == creature::role::Chieftain)
            {
                return { "orc-chieftain.png" };
            }

            if (ROLE == creature::role::Elder)
            {
                return { "orc-elder.png" };
            }

            if (ROLE == creature::role::FourArmed)
            {
                return { "orc-four-armed.png" };
            }

            if (ROLE == creature::role::Captain)
            {
                return { "orc-captain.png" };
            }

            if (ROLE == creature::role::Mountain)
            {
                return { "orc-mountain.png" };
            }

            if (ROLE == creature::role::Spike)
            {
                return { "orc-spike.png" };
            }

            if (ROLE == creature::role::Grunt)
            {
                const std::size_t COUNT(13);
                std::vector<std::string> filenames;
                filenames.reserve(COUNT + 1);
                for (std::size_t i(1); i <= COUNT; ++i)
                {
                    filenames.emplace_back("orc-grunt-" + std::to_string(i) + ".png");
                }

                return filenames;
            }

            if (ROLE == creature::role::Shaman)
            {
                return { "orc-shaman.png" };
            }

            if (ROLE == creature::role::Smasher)
            {
                return { "orc-smasher.png" };
            }

            if (ROLE == creature::role::Warlord)
            {
                return { "orc-warlord.png" };
            }

            if (ROLE == creature::role::Grunt)
            {
                return { "orc-grunt-1.png" };
            }
        }

        if (RACE == creature::race::Newt)
        {
            if (ROLE == creature::role::Chieftain)
            {
                return { "newt-chieftain.png" };
            }

            if (ROLE == creature::role::Cleric)
            {
                return { "newt-cleric.png" };
            }

            if (ROLE == creature::role::Elder)
            {
                return { "newt-elder.png" };
            }

            if (ROLE == creature::role::Sorcerer)
            {
                return { "newt-sorcerer.png" };
            }

            if (ROLE == creature::role::Trader)
            {
                return { "newt-trader.png" };
            }

            if (ROLE == creature::role::Grunt)
            {
                const std::size_t COUNT(6);
                std::vector<std::string> filenames;
                filenames.reserve(COUNT + 1);
                for (std::size_t i(1); i <= COUNT; ++i)
                {
                    filenames.emplace_back("newt-" + std::to_string(i) + ".png");
                }
                return filenames;
            }
        }

        if ((RACE == creature::race::Spider) && (ROLE == creature::role::Spider))
        {
            std::vector<std::string> filenames;
            for (std::size_t i(1); i <= 9; ++i)
            {
                filenames.emplace_back("spider-giant-" + std::to_string(i) + ".png");
            }
            return filenames;
        }

        if (RACE == creature::race::Bog)
        {
            if (ROLE == creature::role::Chieftain)
            {
                return { "bog-chieftain.png" };
            }

            if (ROLE == creature::role::Smasher)
            {
                return { "bog-smasher.png" };
            }

            if (ROLE == creature::role::Sorcerer)
            {
                return { "bog-sorcerer.png" };
            }

            if (ROLE == creature::role::Shaman)
            {
                return { "bog-shaman.png" };
            }

            if (ROLE == creature::role::Spike)
            {
                return { "bog-spike.png" };
            }

            if (ROLE == creature::role::Tendrilus)
            {
                return { "bog-tendrilus.png" };
            }

            if (ROLE == creature::role::Wing)
            {
                return { "bog-wing.png" };
            }

            if (ROLE == creature::role::Whelp)
            {
                return { "bog-whelp.png" };
            }

            if (ROLE == creature::role::Grunt)
            {
                return { "bog-grunt-1.png", "bog-grunt-2.png", "bog-grunt-3.png" };
            }
        }

        if ((RACE == creature::race::CaveCrawler) && (ROLE == creature::role::Mountain))
        {
            return { "cave-crawler-1.png", "cave-crawler-2.png", "cave-crawler-3.png" };
        }

        if ((RACE == creature::race::Hydra) && (ROLE == creature::role::Wing))
        {
            return { "hydra-1.png" };
        }

        if (RACE == creature::race::LizardWalker)
        {
            if (ROLE == creature::role::Whelp)
            {
                return { "lizard-walker-whelp-1.png", "lizard-walker-whelp-2.png" };
            }

            if (ROLE == creature::role::Sorcerer)
            {
                if (SEX == creature::sex::Female)
                {
                    return { "lizard-walker-sorcerer-female.png" };
                }
                else
                {
                    return { "lizard-walker-sorcerer-male.png" };
                }
            }

            if (ROLE == creature::role::Captain)
            {
                return { "lizard-walker-captain.png" };
            }

            if (ROLE == creature::role::Chieftain)
            {
                return { "lizard-walker-chieftain.png" };
            }

            if (ROLE == creature::role::Spike)
            {
                return { "lizard-walker-spike.png" };
            }

            if (ROLE == creature::role::Shaman)
            {
                if (SEX == creature::sex::Female)
                {
                    return { "lizard-walker-shaman-female.png" };
                }
                else
                {
                    return { "lizard-walker-shaman-male.png" };
                }
            }

            if (ROLE == creature::role::Grunt)
            {
                const std::size_t COUNT(12);
                std::vector<std::string> filenames;
                filenames.reserve(COUNT + 1);
                for (std::size_t i(1); i <= COUNT; ++i)
                {
                    filenames.emplace_back("lizard-walker-" + std::to_string(i) + ".png");
                }
                return filenames;
            }
        }

        if (RACE == creature::race::Minotaur)
        {
            if (ROLE == creature::role::Grunt)
            {
                return { "minotaur-grunt-1.png", "minotaur-grunt-2.png", "minotaur-grunt-3.png" };
            }

            if (ROLE == creature::role::Brute)
            {
                return { "minotaur-brute.png" };
            }

            if (ROLE == creature::role::FourArmed)
            {
                return { "minotaur-fourarmed.png" };
            }

            if (ROLE == creature::role::Mountain)
            {
                return { "minotaur-mountain.png" };
            }

            if (ROLE == creature::role::Shaman)
            {
                return { "minotaur-shaman.png" };
            }

            if (ROLE == creature::role::Warlord)
            {
                return { "minotaur-warlord.png" };
            }

            return { "minotaur-1.png", "minotaur-2.png" };
        }

        if (RACE == creature::race::Ogre)
        {
            if (ROLE == creature::role::Berserker)
            {
                return { "ogre-berserker.png" };
            }

            if (ROLE == creature::role::Brute)
            {
                return { "ogre-brute.png" };
            }

            if (ROLE == creature::role::Elder)
            {
                return { "ogre-elder.png" };
            }

            if (ROLE == creature::role::FourArmed)
            {
                return { "ogre-four-armed.png" };
            }

            if (ROLE == creature::role::Giant)
            {
                return { "ogre-giant.png" };
            }

            if (ROLE == creature::role::Mountain)
            {
                return { "ogre-mountain.png" };
            }

            if (ROLE == creature::role::Warlord)
            {
                return { "ogre-warlord.png" };
            }

            if (ROLE == creature::role::Captain)
            {
                return { "ogre-captain.png" };
            }

            if (ROLE == creature::role::Chieftain)
            {
                return { "ogre-chieftain.png" };
            }

            if (ROLE == creature::role::Shaman)
            {
                return { "ogre-shaman.png" };
            }

            if (ROLE == creature::role::Spike)
            {
                return { "ogre-spike.png" };
            }

            if (ROLE == creature::role::Grunt)
            {
                return { "ogre-1.png", "ogre-2.png", "ogre-3.png", "ogre-4.png" };
            }
        }

        if (RACE == creature::race::Plant)
        {
            if (ROLE == creature::role::Smasher)
            {
                return { "plant-smasher.png" };
            }

            if (ROLE == creature::role::Strangler)
            {
                return { "plant-strangler.png" };
            }

            if (ROLE == creature::role::Tendrilus)
            {
                return { "plant-tendrilus-1.png", "plant-tendrilus-2.png" };
            }

            if (ROLE == creature::role::Pod)
            {
                return { "plant-pod-1.png", "plant-pod-2.png" };
            }
        }

        if (RACE == creature::race::Skeleton)
        {
            if (ROLE == creature::role::Chieftain)
            {
                return { "skeleton-chieftain.png" };
            }

            if (ROLE == creature::role::FourArmed)
            {
                return { "skeleton-four-armed.png" };
            }

            if (ROLE == creature::role::Mountain)
            {
                return { "skeleton-mountain.png" };
            }

            if (ROLE == creature::role::Grunt)
            {
                const std::size_t COUNT(8);
                std::vector<std::string> filenames;
                filenames.reserve(COUNT + 1);
                for (std::size_t i(1); i <= COUNT; ++i)
                {
                    filenames.emplace_back("skeleton-" + std::to_string(i) + ".png");
                }
                return filenames;
            }
        }

        if (RACE == creature::race::Werebear)
        {
            if (ROLE == creature::role::Elder)
            {
                return { "werebear-elder.png" };
            }

            if (ROLE == creature::role::Grunt)
            {
                return { "werebear-grunt-1.png", "werebear-grunt-2.png" };
            }

            if (ROLE == creature::role::Mountain)
            {
                return { "werebear-mountain.png" };
            }
        }

        if ((RACE == creature::race::Beetle) && (ROLE == creature::role::Beetle))
        {
            return { "beetle-giant.png" };
        }

        if ((RACE == creature::race::Boar) && (ROLE == creature::role::Boar))
        {
            return { "boar-1.png" };
        }

        if (RACE == creature::race::Demon)
        {
            if (ROLE == creature::role::Skeleton)
            {
                return { "demon-skeleton.png" };
            }

            if (ROLE == creature::role::Spike)
            {
                return { "demon-spike.png" };
            }

            if (ROLE == creature::role::Strangler)
            {
                return { "demon-strangler.png" };
            }

            if (ROLE == creature::role::Whelp)
            {
                const std::size_t COUNT(4);
                std::vector<std::string> filenames;
                filenames.reserve(COUNT + 1);
                for (std::size_t i(1); i <= COUNT; ++i)
                {
                    filenames.emplace_back("demon-whelp-" + std::to_string(i) + ".png");
                }
                return filenames;
            }

            if (ROLE == creature::role::Wing)
            {
                return { "demon-wing-1.png", "demon-wing-2.png" };
            }

            if (ROLE == creature::role::Grunt)
            {
                const std::size_t COUNT(9);
                std::vector<std::string> filenames;
                filenames.reserve(COUNT + 1);
                for (std::size_t i(1); i <= COUNT; ++i)
                {
                    filenames.emplace_back("demon-" + std::to_string(i) + ".png");
                }
                return filenames;
            }
        }

        if (RACE == creature::race::Ghoul)
        {
            if (ROLE == creature::role::Strangler)
            {
                return { "ghoul-strangler.png" };
            }

            if (ROLE == creature::role::Spike)
            {
                return { "ghoul-spike.png" };
            }

            if (ROLE == creature::role::Mountain)
            {
                return { "ghoul-mountain.png" };
            }

            if (ROLE == creature::role::Grunt)
            {
                return { "ghoul-1.png", "ghoul-2.png" };
            }
        }

        if (RACE == creature::race::Griffin)
        {
            if (ROLE == creature::role::Whelp)
            {
                return { "griffin-whelp.png" };
            }

            if (ROLE == creature::role::Wing)
            {
                return { "griffin-1.png" };
            }
        }

        if (RACE == creature::race::Giant)
        {
            if (ROLE == creature::role::Smasher)
            {
                return { "giant-smasher.png" };
            }

            if (ROLE == creature::role::Strangler)
            {
                return { "giant-strangler.png" };
            }

            if (ROLE == creature::role::Warlord)
            {
                return { "giant-warlord.png" };
            }

            if (ROLE == creature::role::Mountain)
            {
                return { "giant-mountain.png" };
            }

            if (ROLE == creature::role::Grunt)
            {
                return { "giant-grunt-1.png", "giant-grunt-2.png", "giant-grunt-3.png" };
            }

            if (ROLE == creature::role::Brute)
            {
                return { "giant-brute.png" };
            }
        }

        if (RACE == creature::race::Goblin)
        {
            if (ROLE == creature::role::Thief)
            {
                return { "goblin-thief.png" };
            }

            if (ROLE == creature::role::Sorcerer)
            {
                return { "goblin-sorcerer.png" };
            }

            if (ROLE == creature::role::Brute)
            {
                return { "goblin-brute.png" };
            }

            if (ROLE == creature::role::Archer)
            {
                return { "goblin-archer.png" };
            }

            if (ROLE == creature::role::Captain)
            {
                return { "goblin-captain.png" };
            }

            if (ROLE == creature::role::Chieftain)
            {
                return { "goblin-chieftain.png" };
            }

            if (ROLE == creature::role::Shaman)
            {
                return { "goblin-shaman.png" };
            }

            if (ROLE == creature::role::Trader)
            {
                if (SEX == creature::sex::Female)
                {
                    return { "goblin-trader-female-1.png", "goblin-trader-female-2.png" };
                }
                else
                {
                    return { "goblin-trader-male-1.png", "goblin-trader-male-2.png",
                             "goblin-male-1.png",        "goblin-male-2.png",
                             "goblin-male-3.png",        "goblin-male-4.png",
                             "goblin-male-5.png" };
                }
            }

            if (ROLE == creature::role::Grunt)
            {
                if (SEX == creature::sex::Female)
                {
                    return { "goblin-female-1.png", "goblin-female-2.png", "goblin-female-3.png",
                             "goblin-female-4.png", "goblin-grunt-2.png",  "goblin-grunt-4.png",
                             "goblin-grunt-5.png",  "goblin-grunt-6.png",  "goblin-grunt-7.png",
                             "goblin-grunt-8.png",  "goblin-grunt-9.png",  "goblin-grunt-10.png",
                             "goblin-grunt-12.png" };
                }
                else
                {
                    const std::size_t COUNT(13);
                    std::vector<std::string> filenames;
                    filenames.reserve(COUNT + 1);
                    for (std::size_t i(1); i <= COUNT; ++i)
                    {
                        filenames.emplace_back("goblin-grunt-" + std::to_string(i) + ".png");
                    }
                    return filenames;
                }
            }
        }

        if (RACE == creature::race::Dragon)
        {
            if (ROLE == creature::role::Firebrand)
            {
                switch (DRAGON_CLASS)
                {
                    case creature::dragon_class::Hatchling:
                    {
                        return { "dragon-fb-hatchling.png" };
                    }
                    case creature::dragon_class::Whelp:
                    {
                        return { "dragon-fb-whelp.png" };
                    }
                    case creature::dragon_class::Fledgling:
                    {
                        return { "dragon-fb-fledgling.png" };
                    }
                    case creature::dragon_class::Juvenile:
                    {
                        return { "dragon-fb-juvenile.png" };
                    }
                    case creature::dragon_class::Adult:
                    {
                        return { "dragon-fb-adult.png" };
                    }
                    case creature::dragon_class::Wyrm:
                    {
                        return { "dragon-fb-wyrm.png" };
                    }
                    case creature::dragon_class::Skycaster:
                    {
                        return { "dragon-fb-skycaster.png" };
                    }
                    case creature::dragon_class::Elder:
                    {
                        return { "dragon-fb-elder.png" };
                    }
                    case creature::dragon_class::Count:
                    default:
                    {
                        break;
                    }
                }
            }

            if (ROLE == creature::role::Sylavin)
            {
                switch (DRAGON_CLASS)
                {
                    case creature::dragon_class::Hatchling:
                    {
                        return { "dragon-syl-hatchling.png" };
                    }
                    case creature::dragon_class::Whelp:
                    {
                        return { "dragon-syl-whelp.png" };
                    }
                    case creature::dragon_class::Fledgling:
                    {
                        return { "dragon-syl-fledgling.png" };
                    }
                    case creature::dragon_class::Juvenile:
                    {
                        return { "dragon-syl-juvenile.png" };
                    }
                    case creature::dragon_class::Adult:
                    {
                        return { "dragon-syl-adult.png" };
                    }
                    case creature::dragon_class::Wyrm:
                    {
                        return { "dragon-syl-wyrm.png" };
                    }
                    case creature::dragon_class::Skycaster:
                    {
                        return { "dragon-syl-skycaster.png" };
                    }
                    case creature::dragon_class::Elder:
                    {
                        return { "dragon-syl-elder.png" };
                    }
                    case creature::dragon_class::Count:
                    default:
                    {
                        break;
                    }
                }
            }
        }

        if (RACE == creature::race::Gnome)
        {
            if (ROLE == creature::role::Bard)
            {
                std::vector<std::string> filenames;

                if (SEX == creature::sex::Female)
                {
                    filenames.emplace_back("bard-gnome-female-1.png");
                    filenames.emplace_back("bard-female-1.png");
                }

                filenames.emplace_back("bard-gnome-1.png");
                filenames.emplace_back("gnome-female-1.png");
                filenames.emplace_back("gnome-2.png");
                filenames.emplace_back("gnome-3.png");
                filenames.emplace_back("gnome-4.png");
                filenames.emplace_back("gnome-7.png");

                if (SEX == creature::sex::Male)
                {
                    filenames.emplace_back("gnome-male-1.png");
                    filenames.emplace_back("gnome-male-2.png");
                }

                return filenames;
            }

            if (ROLE == creature::role::Cleric)
            {
                return { "gnome-4.png", "gnome-8.png" };
            }

            if (ROLE == creature::role::Sorcerer)
            {
                return { "gnome-5.png", "gnome-8.png" };
            }

            if (ROLE == creature::role::Knight)
            {
                std::vector<std::string> filenames { "knight-gnome-1.png", "knight-gnome-2.png" };

                if (SEX == creature::sex::Male)
                {
                    filenames.emplace_back("knight-gnome-male-1.png");
                    filenames.emplace_back("gnome-male-1.png");
                    filenames.emplace_back("gnome-male-2.png");
                }

                return filenames;
            }

            if (ROLE == creature::role::Beastmaster)
            {
                std::vector<std::string> filenames {
                    "gnome-2.png", "gnome-3.png", "gnome-4.png", "gnome-6.png", "gnome-7.png"
                };

                if (SEX == creature::sex::Female)
                {
                    filenames.emplace_back("gnome-female-1.png");
                }
                else if (SEX == creature::sex::Male)
                {
                    filenames.emplace_back("gnome-male-1.png");
                    filenames.emplace_back("gnome-male-2.png");
                }

                return filenames;
            }

            if (ROLE == creature::role::Archer)
            {
                return { "archer-gnome-1.png", "gnome-2.png", "gnome-female-1.png" };
            }

            if (ROLE == creature::role::Thief)
            {
                std::vector<std::string> filenames {
                    "gnome-grunt.png", "gnome-2.png", "gnome-3.png", "gnome-6.png", "gnome-7.png"
                };

                if (SEX == creature::sex::Female)
                {
                    filenames.emplace_back("gnome-female-1.png");
                }

                return filenames;
            }

            if (ROLE == creature::role::Grunt)
            {
                return { "gnome-grunt.png" };
            }
        }

        if (RACE == creature::race::Human)
        {
            if (ROLE == creature::role::Knight)
            {
                std::vector<std::string> filenames { "knight-human-1.png", "knight-human-2.png" };

                if (SEX == creature::sex::Male)
                {
                    filenames.emplace_back("knight-human-male-1.png");
                    filenames.emplace_back("knight-human-male-2.png");
                }

                return filenames;
            }

            if (ROLE == creature::role::Archer)
            {
                std::vector<std::string> filenames { "archer-1.png", "archer-2.png" };

                if (SEX == creature::sex::Female)
                {
                    filenames.emplace_back("archer-female-1.png");
                    filenames.emplace_back("archer-female-2.png");
                    filenames.emplace_back("archer-female-3.png");
                    filenames.emplace_back("archer-female-4.png");
                    filenames.emplace_back("archer-female-5.png");
                }
                else
                {
                    filenames.emplace_back("archer-male-1.png");
                    filenames.emplace_back("archer-male-2.png");
                    filenames.emplace_back("archer-male-3.png");
                    filenames.emplace_back("archer-male-4.png");
                }

                return filenames;
            }

            if (ROLE == creature::role::Bard)
            {
                std::vector<std::string> filenames;

                if (SEX == creature::sex::Female)
                {
                    filenames.emplace_back("bard-human-female-1.png");
                    filenames.emplace_back("bard-human-female-2.png");
                    filenames.emplace_back("bard-human-female-3.png");
                    filenames.emplace_back("bard-female-1.png");
                }
                else
                {
                    filenames.emplace_back("bard-human-male-1.png");
                    filenames.emplace_back("bard-human-male-2.png");
                    filenames.emplace_back("bard-human-male-3.png");
                    filenames.emplace_back("bard-human-male-4.png");
                }

                filenames.emplace_back("bard-1.png");
                filenames.emplace_back("bard-2.png");
                filenames.emplace_back("bard-human-1.png");
                filenames.emplace_back("bard-human-2.png");
                return filenames;
            }

            if (ROLE == creature::role::Beastmaster)
            {
                std::vector<std::string> filenames;

                if (SEX == creature::sex::Female)
                {
                    filenames.emplace_back("beastmaster-human-female-1.png");
                }
                else
                {
                    filenames.emplace_back("beastmaster-human-male-1.png");
                    filenames.emplace_back("beastmaster-human-male-2.png");
                    filenames.emplace_back("beastmaster-human-male-3.png");
                }

                return filenames;
            }

            if (ROLE == creature::role::Cleric)
            {
                std::vector<std::string> filenames { "cleric-human-1.png", "cleric-human-2.png" };

                if (SEX == creature::sex::Female)
                {
                    filenames.emplace_back("cleric-human-female-1.png");
                }
                else
                {
                    filenames.emplace_back("cleric-human-male-1.png");
                    filenames.emplace_back("cleric-human-male-2.png");
                    filenames.emplace_back("cleric-human-male-3.png");
                    filenames.emplace_back("cleric-human-male-4.png");
                }

                return filenames;
            }

            if (ROLE == creature::role::Thief)
            {
                std::vector<std::string> filenames { "thief-human-1.png",
                                                     "thief-human-2.png",
                                                     "thief-human-3.png",
                                                     "thief-human-4.png",
                                                     "thief-human-5.png" };

                if (SEX == creature::sex::Female)
                {
                    filenames.emplace_back("thief-human-female-1.png");
                    filenames.emplace_back("thief-human-female-2.png");
                }
                else
                {
                    filenames.emplace_back("thief-human-male-1.png");
                    filenames.emplace_back("thief-human-male-2.png");
                }

                return filenames;
            }

            if (ROLE == creature::role::Sorcerer)
            {
                if (SEX == creature::sex::Female)
                {
                    return { "sorcerer-human-female-1.png",  "sorcerer-human-female-2.png",
                             "sorcerer-human-female-2b.png", "sorcerer-human-female-3.png",
                             "sorcerer-human-female-4.png",  "sorcerer-human-female-5.png",
                             "sorcerer-human-female-6.png" };
                }
                else
                {
                    return { "sorcerer-human-male-1.png",  "sorcerer-human-male-2.png",
                             "sorcerer-human-male-2b.png", "sorcerer-human-male-3.png",
                             "sorcerer-human-male-4.png",  "sorcerer-human-male-5.png" };
                }
            }

            if (ROLE == creature::role::Ranger)
            {
                if (SEX == creature::sex::Female)
                {
                    return { "ranger-human-female-1.png",
                             "ranger-human-female-2.png",
                             "ranger-human-female-3.png" };
                }
                else
                {
                    return { "ranger-human-male-1.png", "ranger-human-male-2.png",
                             "ranger-human-male-3.png", "ranger-human-male-4.png",
                             "ranger-human-male-5.png", "ranger-human-male-6.png" };
                }
            }

            if (ROLE == creature::role::Trader)
            {
                if (SEX == creature::sex::Female)
                {
                    return { "trader-human-1.png", "trader-human-2.png", "trader-human-3.png" };
                }
                else
                {
                    return { "trader-human-1.png",
                             "trader-human-2.png",
                             "trader-human-3.png",
                             "trader-human-4.png",
                             "trader-human-5.png" };
                }
            }

            if (ROLE == creature::role::Drunk)
            {
                return { "drunk-1.png" };
            }

            if (ROLE == creature::role::Brute)
            {
                return { "soldier-brute.png" };
            }

            if (ROLE == creature::role::Smasher)
            {
                return { "soldier-smasher-1.png", "soldier-smasher-2.png" };
            }

            if (ROLE == creature::role::Soldier)
            {
                if (SEX == creature::sex::Female)
                {
                    return { "soldier-2.png",        "soldier-6.png",        "soldier-7.png",
                             "soldier-11.png",       "soldier-13.png",       "soldier-14.png",
                             "soldier-female-1.png", "soldier-swordsman.png" };
                }
                else
                {
                    return { "soldier-1.png",      "soldier-2.png",        "soldier-3.png",
                             "soldier-4.png",      "soldier-5.png",        "soldier-6.png",
                             "soldier-7.png",      "soldier-8.png",        "soldier-9.png",
                             "soldier-10.png",     "soldier-11.png",       "soldier-12.png",
                             "soldier-axeman.png", "soldier-swordsman.png" };
                }
            }

            if (ROLE == creature::role::Blacksmith)
            {
                if (SEX == creature::sex::Male)
                {
                    return { "human-blacksmith-male-1.png",
                             "human-blacksmith-male-2.png",
                             "human-blacksmith-male-3.png",
                             "human-blacksmith-male-4.png" };
                }
                else
                {
                    return { "human-blacksmith-female-1.png" };
                }
            }

            if (ROLE == creature::role::Shaman)
            {
                return { "human-shaman.png" };
            }

            if (ROLE == creature::role::Grunt)
            {
                if (SEX == creature::sex::Female)
                {
                    return { "human-1.png",         "human-2.png",         "human-3.png",
                             "human-4.png",         "human-5.png",         "human-6.png",
                             "human-8.png",         "human-9.png",         "human-10.png",
                             "human-12.png",        "human-13.png",        "human-female-1.png",
                             "human-female-2.png",  "human-female-3.png",  "human-female-4.png",
                             "human-female-5.png",  "human-female-6.png",  "human-female-7.png",
                             "human-female-8.png",  "human-female-9.png",  "human-female-10.png",
                             "human-female-11.png", "human-female-12.png", "human-female-13.png",
                             "human-female-14.png" };
                }
                else
                {
                    return { "human-1.png",      "human-2.png",      "human-3.png",
                             "human-4.png",      "human-5.png",      "human-6.png",
                             "human-7.png",      "human-8.png",      "human-9.png",
                             "human-10.png",     "human-11.png",     "human-12.png",
                             "human-13.png",     "human-male-1.png", "human-male-2.png",
                             "human-male-3.png", "human-male-4.png", "human-male-5.png",
                             "human-male-6.png", "human-male-7.png" };
                }
            }
        }

        if (RACE == creature::race::Pixie)
        {
            if (SEX == creature::sex::Female)
            {
                if (ROLE == creature::role::Beastmaster)
                {
                    return { "pixie-beastmaster-female.png" };
                }
                else if (ROLE == creature::role::Sorcerer)
                {
                    return { "pixie-female-1.png", "pixie-female-3.png" };
                }
                else if (ROLE == creature::role::Cleric)
                {
                    return { "pixie-female-2.png", "pixie-female-5.png" };
                }
                else if (ROLE == creature::role::Bard)
                {
                    return { "pixie-female-4.png", "pixie-3.png" };
                }
            }
            else
            {
                if (ROLE == creature::role::Beastmaster)
                {
                    return { "pixie-beastmaster-male.png" };
                }
                else if (
                    (ROLE == creature::role::Sorcerer) || (ROLE == creature::role::Cleric)
                    || (ROLE == creature::role::Bard))
                {
                    return { "pixie-1.png", "pixie-2.png", "pixie-3.png", "pixie-male-1.png" };
                }
            }
        }

        if (RACE == creature::race::Wolfen)
        {
            if (WOLFEN_CLASS >= creature::WolfenClass::Count)
            {
                if (ROLE == creature::role::TwoHeaded)
                {
                    return { "wolfen-twoheaded.png" };
                }

                if (ROLE == creature::role::Wolfen)
                {
                    return { "wolfen-1.png", "wolfen-2.png", "wolfen-3.png" };
                }
            }
            else
            {
                switch (WOLFEN_CLASS)
                {
                    case creature::WolfenClass::Pup:
                    {
                        return { "wolfen-pup.png" };
                    }
                    case creature::WolfenClass::Juvenile:
                    {
                        return { "wolfen-juvenile.png" };
                    }
                    case creature::WolfenClass::Adult:
                    {
                        return { "wolfen-adult.png" };
                    }
                    case creature::WolfenClass::Noble:
                    {
                        return { "wolfen-noble.png" };
                    }
                    case creature::WolfenClass::Highborn:
                    {
                        return { "wolfen-highborn.png" };
                    }
                    case creature::WolfenClass::Elder:
                    {
                        return { "wolfen-elder.png" };
                    }
                    case creature::WolfenClass::Count:
                    default:
                    {
                        break;
                    }
                }
            }
        }

        if (RACE == creature::race::Halfling)
        {
            if (ROLE == creature::role::Shaman)
            {
                return { "halfling-shaman.png" };
            }
            else if (ROLE == creature::role::Chieftain)
            {
                return { "halfling-chieftain.png" };
            }
            else if (ROLE == creature::role::Captain)
            {
                return { "halfling-captain.png" };
            }
            else if (ROLE == creature::role::Grunt)
            {
                return { "halfling-1.png" };
            }
        }

        if ((RACE == creature::race::Lion) && (ROLE == creature::role::Lion))
        {
            return { "lion.png" };
        }

        if ((RACE == creature::race::LionBoar) && (ROLE == creature::role::Lion))
        {
            return { "lion-boar.png" };
        }

        if ((RACE == creature::race::Naga) && (ROLE == creature::role::Grunt))
        {
            return { "naga.png" };
        }

        if (RACE == creature::race::Pug)
        {
            if (ROLE == creature::role::Archer)
            {
                return { "pug-archer.png" };
            }
            else if (ROLE == creature::role::Cleric)
            {
                return { "pug-cleric.png" };
            }
            else if (ROLE == creature::role::Grunt)
            {
                return { "pug-grunt.png" };
            }
        }

        if ((RACE == creature::race::Ramonaut) && (ROLE == creature::role::Ramonaut))
        {
            return { "ramonaut.png" };
        }

        if (RACE == creature::race::Serpent)
        {
            if (ROLE == creature::role::Water)
            {
                return { "serpent-water.png" };
            }
            else if (ROLE == creature::role::Serpent)
            {
                return { "serpent-desert.png" };
            }
        }

        if ((RACE == creature::race::Shade) && (ROLE == creature::role::Ghost))
        {
            return { "shade-1.png", "shade-2.png" };
        }

        if ((RACE == creature::race::Cobra) && (ROLE == creature::role::Serpent))
        {
            return { "cobra-1.png", "cobra-2.png" };
        }

        if ((RACE == creature::race::Werebat) && (ROLE == creature::role::Bat))
        {
            return { "werebat-1.png", "werebat-2.png" };
        }

        if ((RACE == creature::race::Bat) && (ROLE == creature::role::Bat))
        {
            return { "bat-1.png", "bat-2.png", "bat-3.png" };
        }

        if (RACE == creature::race::Witch)
        {
            if (ROLE == creature::role::Elder)
            {
                return { "witch-elder.png" };
            }
            else if (ROLE == creature::role::Ghost)
            {
                return { "witch-ghost.png" };
            }
            else if (ROLE == creature::role::Strangler)
            {
                return { "witch-strangler.png" };
            }
            else if (ROLE == creature::role::Mountain)
            {
                return { "witch-mountain.png" };
            }
            else if (ROLE == creature::role::Sorcerer)
            {
                return { "witch-1.png" };
            }
        }

        if ((RACE == creature::race::Golem) && (ROLE == creature::role::Brute))
        {
            return { "golem.png" };
        }

        if ((RACE == creature::race::Harpy) && (ROLE == creature::role::Wing))
        {
            return { "harpy.png" };
        }

        if ((RACE == creature::race::ThreeHeadedHound) && (ROLE == creature::role::Mountain))
        {
            return { "hound-three-headed.png" };
        }

        if ((RACE == creature::race::Wereboar) && (ROLE == creature::role::Boar))
        {
            return { "wereboar.png" };
        }

        if ((RACE == creature::race::Werecat) && (ROLE == creature::role::Cat))
        {
            return { "werecat.png" };
        }

        if ((RACE == creature::race::Werewolf) && (ROLE == creature::role::Wolf))
        {
            return { "werewolf.png" };
        }

        if (RACE == creature::race::Wyvern)
        {
            if (ROLE == creature::role::Whelp)
            {
                return { "wyvern-whelp.png" };
            }
            else if (ROLE == creature::role::Wing)
            {
                return { "wyvern-1.png", "wyvern-2.png" };
            }
        }

        std::ostringstream ss;
        ss << "gui::CreatureImagePaths::GetFilenames(race=" << NAMEOF_ENUM(RACE)
           << ", role=" << NAMEOF_ENUM(ROLE) << ", sex=" << NAMEOF_ENUM(SEX)
           << ", WolfenClass=" << WOLFEN_CLASS << ", dragon_class=" << DRAGON_CLASS
           << ") -No filenames found for that creature information.";

        throw std::runtime_error(ss.str());
    }

} // namespace gui
} // namespace heroespath
