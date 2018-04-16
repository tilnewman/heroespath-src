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
// creature-image-manager.cpp
//
#include "creature-image-manager.hpp"

#include "creature/body-type.hpp"
#include "creature/creature.hpp"
#include "game/game-data-file.hpp"
#include "game/loop-manager.hpp"
#include "log/log-macros.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/random.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/sfml-util.hpp"

#include <boost/filesystem.hpp>

#include <exception>
#include <sstream>

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        bool CreatureImageManager::Test()
        {
            static auto didPostInitial{ false };
            if (false == didPostInitial)
            {
                didPostInitial = true;
                game::LoopManager::Instance()->TestingStrAppend(
                    "sfml_util::gui::CreatureImageManager::Test() Starting Tests...");
            }

            static auto raceIndex{ 0 };
            static auto roleIndex{ 0 };
            static auto sexIndex{ 0 };
            static auto classIndex{ 0 };

            if (raceIndex < static_cast<int>(creature::race::Count))
            {
                auto const RACE_ENUM{ static_cast<creature::race::Enum>(raceIndex) };
                auto const RACE_STR{ creature::race::ToString(RACE_ENUM) };
                auto const ROLE_VEC{ creature::race::Roles(RACE_ENUM) };

                if (roleIndex < static_cast<int>(ROLE_VEC.size()))
                {
                    auto const ROLE_ENUM{ ROLE_VEC[static_cast<std::size_t>(roleIndex)] };
                    auto const ROLE_STR{ creature::role::ToString(ROLE_ENUM) };

                    if (sexIndex < static_cast<int>(creature::sex::Count))
                    {
                        static std::size_t fileIndex{ 0 };

                        auto const SEX_ENUM{ static_cast<creature::sex::Enum>(sexIndex) };
                        auto const SEX_STR{ creature::sex::ToString(SEX_ENUM) };

                        // test to ensure that BodyType maker will not throw
                        creature::BodyType::Make_FromRaceAndRole(RACE_ENUM, ROLE_ENUM);

                        if (RACE_ENUM == creature::race::Wolfen)
                        {
                            if (classIndex < static_cast<int>(creature::wolfen_class::Count))
                            {
                                auto const CLASS_ENUM{ static_cast<creature::wolfen_class::Enum>(
                                    classIndex) };

                                auto const CLASS_STR{ creature::wolfen_class::ToString(
                                    CLASS_ENUM) };

                                auto const FILENAMES{ GetFilenames(
                                    RACE_ENUM,
                                    ROLE_ENUM,
                                    SEX_ENUM,
                                    CLASS_ENUM,
                                    creature::dragon_class::Count) };

                                M_ASSERT_OR_LOGANDTHROW_SS(
                                    (FILENAMES.empty() == false),
                                    "sfml_util::gui::CreatureImageManager() (wolfen_classes) race="
                                        << RACE_STR << ", role=" << ROLE_STR << ", sex=" << SEX_STR
                                        << ", wolfen_class=" << CLASS_STR
                                        << ", GetFilenames() failed to return anything.");

                                if (fileIndex < FILENAMES.size())
                                {
                                    sf::Texture texture;
                                    GetImageFromFilename(texture, FILENAMES.at(fileIndex), false);

                                    game::LoopManager::Instance()->TestingImageSet(
                                        texture,
                                        true,
                                        "Creature",
                                        "(no type)",
                                        FILENAMES.at(fileIndex));

                                    std::ostringstream ss;
                                    ss << " CreatureImageManager Tested race=" << RACE_STR
                                       << " role=" << ROLE_STR << " sex=" << SEX_STR
                                       << " wolfen_class=" << CLASS_STR
                                       << " filename=" << FILENAMES.at(fileIndex);

                                    game::LoopManager::Instance()->TestingStrAppend(ss.str());

                                    ++fileIndex;
                                    return false;
                                }

                                fileIndex = 0;
                                ++classIndex;
                                return false;
                            }
                        }
                        else if (RACE_ENUM == creature::race::Dragon)
                        {
                            if (classIndex < static_cast<int>(creature::dragon_class::Count))
                            {
                                auto const CLASS_ENUM{ static_cast<creature::dragon_class::Enum>(
                                    classIndex) };

                                auto const CLASS_STR{ creature::dragon_class::ToString(
                                    CLASS_ENUM) };

                                auto const FILENAMES{ GetFilenames(
                                    RACE_ENUM,
                                    ROLE_ENUM,
                                    SEX_ENUM,
                                    creature::wolfen_class::Count,
                                    CLASS_ENUM) };

                                M_ASSERT_OR_LOGANDTHROW_SS(
                                    (FILENAMES.empty() == false),
                                    "sfml_util::gui::CreatureImageManager() (dragon_classes) race="
                                        << RACE_STR << ", role=" << ROLE_STR << ", sex=" << SEX_STR
                                        << ", dragon_class=" << CLASS_STR
                                        << ", GetFilenames() failed to return anything.");

                                if (fileIndex < FILENAMES.size())
                                {
                                    sf::Texture texture;
                                    GetImageFromFilename(texture, FILENAMES.at(fileIndex), false);

                                    game::LoopManager::Instance()->TestingImageSet(
                                        texture,
                                        true,
                                        "Creature",
                                        "(no type)",
                                        FILENAMES.at(fileIndex));

                                    std::ostringstream ss;
                                    ss << " CreatureImageManager Tested race=" << RACE_STR
                                       << " role=" << ROLE_STR << " sex=" << SEX_STR
                                       << " dragon_class=" << CLASS_STR
                                       << " filename=" << FILENAMES.at(fileIndex);

                                    game::LoopManager::Instance()->TestingStrAppend(ss.str());

                                    ++fileIndex;
                                    return false;
                                }

                                fileIndex = 0;
                                ++classIndex;
                                return false;
                            }
                        }
                        else
                        {
                            auto const FILENAMES{ GetFilenames(
                                RACE_ENUM,
                                ROLE_ENUM,
                                SEX_ENUM,
                                creature::wolfen_class::Count,
                                creature::dragon_class::Count) };

                            M_ASSERT_OR_LOGANDTHROW_SS(
                                (FILENAMES.empty() == false),
                                "sfml_util::gui::CreatureImageManager() race="
                                    << RACE_STR << ", role=" << ROLE_STR << ", sex=" << SEX_STR
                                    << ", GetFilenames() failed to return anything.");

                            if (fileIndex < FILENAMES.size())
                            {
                                sf::Texture texture;
                                GetImageFromFilename(texture, FILENAMES.at(fileIndex), false);

                                game::LoopManager::Instance()->TestingImageSet(
                                    texture,
                                    true,
                                    "Creature",
                                    "(no type)",
                                    FILENAMES.at(fileIndex));

                                std::ostringstream ss;
                                ss << " CreatureImageManager Tested race=" << RACE_STR
                                   << " role=" << ROLE_STR << " sex=" << SEX_STR
                                   << " filename=" << FILENAMES.at(fileIndex);

                                game::LoopManager::Instance()->TestingStrAppend(ss.str());

                                ++fileIndex;
                                return false;
                            }
                        }

                        fileIndex = 0;
                        classIndex = 0;
                        ++sexIndex;
                        return false;
                    }

                    sexIndex = 0;
                    ++roleIndex;
                    return false;
                }

                sexIndex = 0;
                roleIndex = 0;
                ++raceIndex;
                return false;
            }

            game::LoopManager::Instance()->TestingStrAppend(
                "sfml_util::gui::CreatureImageManager::Test()  ALL TESTS PASSED.");

            return true;
        }

        void CreatureImageManager::GetImage(
            sf::Texture & texture, const creature::CreaturePtr_t CREATURE_PTR)
        {
            return GetImageFromFilename(
                texture, CREATURE_PTR->ImageFilename(), CREATURE_PTR->IsPlayerCharacter());
        }

        void CreatureImageManager::GetImageFromFilename(
            sf::Texture & texture, const std::string & FILENAME, const bool WILL_FACE_RIGHT)
        {
            return LoadImage(texture, FILENAME, WILL_FACE_RIGHT);
        }

        const std::string
            CreatureImageManager::GetRandomFilename(const creature::CreaturePtr_t CREATURE_PTR)
        {
            auto const FILENAMES{ GetFilenames(
                CREATURE_PTR->Race(),
                CREATURE_PTR->Role(),
                CREATURE_PTR->Sex(),
                CREATURE_PTR->WolfenClass(),
                CREATURE_PTR->DragonClass()) };

            M_ASSERT_OR_LOGANDTHROW_SS(
                (FILENAMES.empty() == false),
                "sfml_util::gui::CreatureImageManager::GetRandomFilename(creature={"
                    << CREATURE_PTR->ToString()
                    << "}) (which actually calls GetFilenames()) returned no filenames.");

            return misc::Vector::SelectRandom(FILENAMES);
        }

        void CreatureImageManager::LoadImage(
            sf::Texture & texture, const std::string & IMAGE_FILE_NAME, const bool WILL_FACE_RIGHT)
        {
            namespace bfs = boost::filesystem;

            auto const IMAGES_DIR{ game::GameDataFile::Instance()->GetMediaPath(
                "media-images-creatures-dir") };

            const bfs::path PATH_OBJ(
                bfs::system_complete(bfs::path(IMAGES_DIR) / bfs::path(IMAGE_FILE_NAME)));

            sfml_util::LoadTexture(texture, PATH_OBJ.string());

            if (WILL_FACE_RIGHT)
            {
                sfml_util::FlipHoriz(texture);
            }
        }

        const std::vector<std::string> CreatureImageManager::GetFilenames(
            const creature::race::Enum RACE,
            const creature::role::Enum ROLE,
            const creature::sex::Enum SEX,
            const creature::wolfen_class::Enum WOLFEN_CLASS,
            const creature::dragon_class::Enum DRAGON_CLASS)
        {
            using namespace creature;

            if (RACE == race::Troll)
            {
                if (ROLE == role::Berserker)
                {
                    return { "troll-berserker.png" };
                }

                if (ROLE == role::Brute)
                {
                    return { "troll-brute.png" };
                }

                if (ROLE == role::Chieftain)
                {
                    return { "troll-chieftain.png" };
                }

                if (ROLE == role::Grunt)
                {
                    return { "troll-grunt-1.png" };
                }

                if (ROLE == role::Mountain)
                {
                    return { "troll-mountain.png" };
                }

                if (ROLE == role::Shaman)
                {
                    return { "troll-shaman.png" };
                }

                if (ROLE == role::Smasher)
                {
                    return { "troll-smasher.png" };
                }

                if (ROLE == role::Strangler)
                {
                    return { "troll-strangler.png" };
                }

                if (ROLE == role::Warlord)
                {
                    return { "troll-warlord.png" };
                }

                if (ROLE == role::Grunt)
                {
                    return { "troll-1.png" };
                }
            }

            if (RACE == race::Orc)
            {
                if (ROLE == role::Berserker)
                {
                    return { "orc-berserker.png" };
                }

                if (ROLE == role::Chieftain)
                {
                    return { "orc-chieftain.png" };
                }

                if (ROLE == role::Elder)
                {
                    return { "orc-elder.png" };
                }

                if (ROLE == role::FourArmed)
                {
                    return { "orc-four-armed.png" };
                }

                if (ROLE == role::Captain)
                {
                    return { "orc-captain.png" };
                }

                if (ROLE == role::Mountain)
                {
                    return { "orc-mountain.png" };
                }

                if (ROLE == role::Spike)
                {
                    return { "orc-spike.png" };
                }

                if (ROLE == role::Grunt)
                {
                    std::vector<std::string> filenames;
                    for (std::size_t i(1); i <= 13; ++i)
                    {
                        std::ostringstream ss;
                        ss << "orc-grunt-" << i << ".png";
                        filenames.emplace_back(ss.str());
                    }

                    return filenames;
                }

                if (ROLE == role::Shaman)
                {
                    return { "orc-shaman.png" };
                }

                if (ROLE == role::Smasher)
                {
                    return { "orc-smasher.png" };
                }

                if (ROLE == role::Warlord)
                {
                    return { "orc-warlord.png" };
                }

                if (ROLE == role::Grunt)
                {
                    return { "orc-grunt-1.png" };
                }
            }

            if (RACE == race::Newt)
            {
                if (ROLE == role::Chieftain)
                {
                    return { "newt-chieftain.png" };
                }

                if (ROLE == role::Cleric)
                {
                    return { "newt-cleric.png" };
                }

                if (ROLE == role::Elder)
                {
                    return { "newt-elder.png" };
                }

                if (ROLE == role::Sorcerer)
                {
                    return { "newt-sorcerer.png" };
                }

                if (ROLE == role::Trader)
                {
                    return { "newt-trader.png" };
                }

                if (ROLE == role::Grunt)
                {
                    std::vector<std::string> filenames;
                    for (std::size_t i(1); i <= 6; ++i)
                    {
                        std::ostringstream ss;
                        ss << "newt-" << i << ".png";
                        filenames.emplace_back(ss.str());
                    }
                    return filenames;
                }
            }

            if ((RACE == race::Spider) && (ROLE == role::Spider))
            {
                std::vector<std::string> filenames;
                for (std::size_t i(1); i <= 9; ++i)
                {
                    std::ostringstream ss;
                    ss << "spider-giant-" << i << ".png";
                    filenames.emplace_back(ss.str());
                }
                return filenames;
            }

            if (RACE == race::Bog)
            {
                if (ROLE == role::Chieftain)
                {
                    return { "bog-chieftain.png" };
                }

                if (ROLE == role::Smasher)
                {
                    return { "bog-smasher.png" };
                }

                if (ROLE == role::Sorcerer)
                {
                    return { "bog-sorcerer.png" };
                }

                if (ROLE == role::Shaman)
                {
                    return { "bog-shaman.png" };
                }

                if (ROLE == role::Spike)
                {
                    return { "bog-spike.png" };
                }

                if (ROLE == role::Tendrilus)
                {
                    return { "bog-tendrilus.png" };
                }

                if (ROLE == role::Wing)
                {
                    return { "bog-wing.png" };
                }

                if (ROLE == role::Whelp)
                {
                    return { "bog-whelp.png" };
                }

                if (ROLE == role::Grunt)
                {
                    return { "bog-grunt-1.png", "bog-grunt-2.png", "bog-grunt-3.png" };
                }
            }

            if ((RACE == race::CaveCrawler) && (ROLE == role::Mountain))
            {
                return { "cave-crawler-1.png", "cave-crawler-2.png", "cave-crawler-3.png" };
            }

            if ((RACE == race::Hydra) && (ROLE == role::Wing))
            {
                return { "hydra-1.png" };
            }

            if (RACE == race::LizardWalker)
            {
                if (ROLE == role::Whelp)
                {
                    return { "lizard-walker-whelp-1.png", "lizard-walker-whelp-2.png" };
                }

                if (ROLE == role::Sorcerer)
                {
                    if (SEX == sex::Female)
                    {
                        return { "lizard-walker-sorcerer-female.png" };
                    }
                    else
                    {
                        return { "lizard-walker-sorcerer-male.png" };
                    }
                }

                if (ROLE == role::Captain)
                {
                    return { "lizard-walker-captain.png" };
                }

                if (ROLE == role::Chieftain)
                {
                    return { "lizard-walker-chieftain.png" };
                }

                if (ROLE == role::Spike)
                {
                    return { "lizard-walker-spike.png" };
                }

                if (ROLE == role::Shaman)
                {
                    if (SEX == sex::Female)
                    {
                        return { "lizard-walker-shaman-female.png" };
                    }
                    else
                    {
                        return { "lizard-walker-shaman-male.png" };
                    }
                }

                if (ROLE == role::Grunt)
                {
                    std::vector<std::string> filenames;
                    for (std::size_t i(1); i <= 12; ++i)
                    {
                        std::ostringstream ss;
                        ss << "lizard-walker-" << i << ".png";
                        filenames.emplace_back(ss.str());
                    }
                    return filenames;
                }
            }

            if (RACE == race::Minotaur)
            {
                if (ROLE == role::Grunt)
                {
                    return {
                        "minotaur-grunt-1.png", "minotaur-grunt-2.png", "minotaur-grunt-3.png"
                    };
                }

                if (ROLE == role::Brute)
                {
                    return { "minotaur-brute.png" };
                }

                if (ROLE == role::FourArmed)
                {
                    return { "minotaur-fourarmed.png" };
                }

                if (ROLE == role::Mountain)
                {
                    return { "minotaur-mountain.png" };
                }

                if (ROLE == role::Shaman)
                {
                    return { "minotaur-shaman.png" };
                }

                if (ROLE == role::Warlord)
                {
                    return { "minotaur-warlord.png" };
                }

                if (ROLE == role::Grunt)
                {
                    return { "minotaur-1.png", "minotaur-2.png" };
                }
            }

            if (RACE == race::Ogre)
            {
                if (ROLE == role::Berserker)
                {
                    return { "ogre-berserker.png" };
                }

                if (ROLE == role::Brute)
                {
                    return { "ogre-brute.png" };
                }

                if (ROLE == role::Elder)
                {
                    return { "ogre-elder.png" };
                }

                if (ROLE == role::FourArmed)
                {
                    return { "ogre-four-armed.png" };
                }

                if (ROLE == role::Giant)
                {
                    return { "ogre-giant.png" };
                }

                if (ROLE == role::Mountain)
                {
                    return { "ogre-mountain.png" };
                }

                if (ROLE == role::Warlord)
                {
                    return { "ogre-warlord.png" };
                }

                if (ROLE == role::Captain)
                {
                    return { "ogre-captain.png" };
                }

                if (ROLE == role::Chieftain)
                {
                    return { "ogre-chieftain.png" };
                }

                if (ROLE == role::Shaman)
                {
                    return { "ogre-shaman.png" };
                }

                if (ROLE == role::Spike)
                {
                    return { "ogre-spike.png" };
                }

                if (ROLE == role::Grunt)
                {
                    return { "ogre-1.png", "ogre-2.png", "ogre-3.png", "ogre-4.png" };
                }
            }

            if (RACE == race::Plant)
            {
                if (ROLE == role::Smasher)
                {
                    return { "plant-smasher.png" };
                }

                if (ROLE == role::Strangler)
                {
                    return { "plant-strangler.png" };
                }

                if (ROLE == role::Tendrilus)
                {
                    return { "plant-tendrilus-1.png", "plant-tendrilus-2.png" };
                }

                if (ROLE == role::Pod)
                {
                    return { "plant-pod-1.png", "plant-pod-2.png" };
                }
            }

            if (RACE == race::Skeleton)
            {
                if (ROLE == role::Chieftain)
                {
                    return { "skeleton-chieftain.png" };
                }

                if (ROLE == role::FourArmed)
                {
                    return { "skeleton-four-armed.png" };
                }

                if (ROLE == role::Mountain)
                {
                    return { "skeleton-mountain.png" };
                }

                if (ROLE == role::Grunt)
                {
                    std::vector<std::string> filenames;
                    for (std::size_t i(1); i <= 8; ++i)
                    {
                        std::ostringstream ss;
                        ss << "skeleton-" << i << ".png";
                        filenames.emplace_back(ss.str());
                    }
                    return filenames;
                }
            }

            if (RACE == race::Werebear)
            {
                if (ROLE == role::Elder)
                {
                    return { "werebear-elder.png" };
                }

                if (ROLE == role::Grunt)
                {
                    return { "werebear-grunt-1.png", "werebear-grunt-2.png" };
                }

                if (ROLE == role::Mountain)
                {
                    return { "werebear-mountain.png" };
                }
            }

            if ((RACE == race::Beetle) && (ROLE == role::Beetle))
            {
                return { "beetle-giant.png" };
            }

            if ((RACE == race::Boar) && (ROLE == role::Boar))
            {
                return { "boar-1.png" };
            }

            if (RACE == race::Demon)
            {
                if (ROLE == role::Skeleton)
                {
                    return { "demon-skeleton.png" };
                }

                if (ROLE == role::Spike)
                {
                    return { "demon-spike.png" };
                }

                if (ROLE == role::Strangler)
                {
                    return { "demon-strangler.png" };
                }

                if (ROLE == role::Whelp)
                {
                    std::vector<std::string> filenames;
                    for (std::size_t i(1); i <= 4; ++i)
                    {
                        std::ostringstream ss;
                        ss << "demon-whelp-" << i << ".png";
                        filenames.emplace_back(ss.str());
                    }
                    return filenames;
                }

                if (ROLE == role::Wing)
                {
                    return { "demon-wing-1.png", "demon-wing-2.png" };
                }

                if (ROLE == role::Grunt)
                {
                    std::vector<std::string> filenames;
                    for (std::size_t i(1); i <= 9; ++i)
                    {
                        std::ostringstream ss;
                        ss << "demon-" << i << ".png";
                        filenames.emplace_back(ss.str());
                    }
                    return filenames;
                }
            }

            if (RACE == race::Ghoul)
            {
                if (ROLE == role::Strangler)
                {
                    return { "ghoul-strangler.png" };
                }

                if (ROLE == role::Spike)
                {
                    return { "ghoul-spike.png" };
                }

                if (ROLE == role::Mountain)
                {
                    return { "ghoul-mountain.png" };
                }

                if (ROLE == role::Grunt)
                {
                    return { "ghoul-1.png", "ghoul-2.png" };
                }
            }

            if (RACE == race::Griffin)
            {
                if (ROLE == role::Whelp)
                {
                    return { "griffin-whelp.png" };
                }

                if (ROLE == role::Wing)
                {
                    return { "griffin-1.png" };
                }
            }

            if (RACE == race::Giant)
            {
                if (ROLE == role::Smasher)
                {
                    return { "giant-smasher.png" };
                }

                if (ROLE == role::Strangler)
                {
                    return { "giant-strangler.png" };
                }

                if (ROLE == role::Warlord)
                {
                    return { "giant-warlord.png" };
                }

                if (ROLE == role::Mountain)
                {
                    return { "giant-mountain.png" };
                }

                if (ROLE == role::Grunt)
                {
                    return { "giant-grunt-1.png", "giant-grunt-2.png", "giant-grunt-3.png" };
                }

                if (ROLE == role::Brute)
                {
                    return { "giant-brute.png" };
                }
            }

            if (RACE == race::Goblin)
            {
                if (ROLE == role::Thief)
                {
                    return { "goblin-thief.png" };
                }

                if (ROLE == role::Sorcerer)
                {
                    return { "goblin-sorcerer.png" };
                }

                if (ROLE == role::Brute)
                {
                    return { "goblin-brute.png" };
                }

                if (ROLE == role::Archer)
                {
                    return { "goblin-archer.png" };
                }

                if (ROLE == role::Captain)
                {
                    return { "goblin-captain.png" };
                }

                if (ROLE == role::Chieftain)
                {
                    return { "goblin-chieftain.png" };
                }

                if (ROLE == role::Shaman)
                {
                    return { "goblin-shaman.png" };
                }

                if (ROLE == role::Trader)
                {
                    if (SEX == sex::Female)
                    {
                        return { "goblin-trader-female-1.png", "goblin-trader-female-2.png" };
                    }
                    else
                    {
                        return { "goblin-trader-male-1.png", "goblin-trader-male-2.png" };
                    }
                }

                if (ROLE == role::Grunt)
                {
                    if (SEX == sex::Female)
                    {
                        return { "goblin-female-1.png", "goblin-female-2.png",
                                 "goblin-female-3.png", "goblin-female-4.png",
                                 "goblin-grunt-2.png",  "goblin-grunt-4.png",
                                 "goblin-grunt-5.png",  "goblin-grunt-6.png",
                                 "goblin-grunt-7.png",  "goblin-grunt-8.png",
                                 "goblin-grunt-9.png",  "goblin-grunt-10.png",
                                 "goblin-grunt-12.png" };
                    }
                    else
                    {
                        std::vector<std::string> filenames;
                        for (std::size_t i(1); i <= 13; ++i)
                        {
                            std::ostringstream ss;
                            ss << "goblin-grunt-" << i << ".png";
                            filenames.emplace_back(ss.str());
                        }
                        return filenames;
                    }
                }
            }

            if (RACE == race::Dragon)
            {
                if (ROLE == role::Firebrand)
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

                if (ROLE == role::Sylavin)
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

            if (RACE == race::Gnome)
            {
                if (ROLE == role::Bard)
                {
                    std::vector<std::string> filenames;

                    if (SEX == sex::Female)
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

                    if (SEX == sex::Male)
                    {
                        filenames.emplace_back("gnome-male-1.png");
                        filenames.emplace_back("gnome-male-2.png");
                    }

                    return filenames;
                }

                if (ROLE == role::Cleric)
                {
                    return { "gnome-4.png", "gnome-8.png" };
                }

                if (ROLE == role::Sorcerer)
                {
                    return { "gnome-5.png", "gnome-8.png" };
                }

                if (ROLE == role::Knight)
                {
                    std::vector<std::string> filenames{ "knight-gnome-1.png",
                                                        "knight-gnome-2.png" };

                    if (SEX == sex::Male)
                    {
                        filenames.emplace_back("knight-gnome-male-1.png");
                        filenames.emplace_back("gnome-male-1.png");
                        filenames.emplace_back("gnome-male-2.png");
                    }

                    return filenames;
                }

                if (ROLE == role::Beastmaster)
                {
                    std::vector<std::string> filenames{
                        "gnome-2.png", "gnome-3.png", "gnome-4.png", "gnome-6.png", "gnome-7.png"
                    };

                    if (SEX == sex::Female)
                    {
                        filenames.emplace_back("gnome-female-1.png");
                    }
                    else if (SEX == sex::Male)
                    {
                        filenames.emplace_back("gnome-male-1.png");
                        filenames.emplace_back("gnome-male-2.png");
                    }

                    return filenames;
                }

                if (ROLE == role::Archer)
                {
                    return { "archer-gnome-1.png", "gnome-2.png", "gnome-female-1.png" };
                }

                if (ROLE == role::Thief)
                {
                    std::vector<std::string> filenames{ "gnome-grunt.png",
                                                        "gnome-2.png",
                                                        "gnome-3.png",
                                                        "gnome-6.png",
                                                        "gnome-7.png" };

                    if (SEX == sex::Female)
                    {
                        filenames.emplace_back("gnome-female-1.png");
                    }

                    return filenames;
                }

                if (ROLE == role::Grunt)
                {
                    return { "gnome-grunt.png" };
                }
            }

            if (RACE == race::Human)
            {
                if (ROLE == role::Knight)
                {
                    std::vector<std::string> filenames{ "knight-human-1.png",
                                                        "knight-human-2.png" };

                    if (SEX == sex::Male)
                    {
                        filenames.emplace_back("knight-human-male-1.png");
                        filenames.emplace_back("knight-human-male-2.png");
                    }

                    return filenames;
                }

                if (ROLE == role::Archer)
                {
                    std::vector<std::string> filenames{ "archer-1.png", "archer-2.png" };

                    if (SEX == sex::Female)
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

                if (ROLE == role::Bard)
                {
                    std::vector<std::string> filenames;

                    if (SEX == sex::Female)
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

                if (ROLE == role::Beastmaster)
                {
                    std::vector<std::string> filenames;

                    if (SEX == sex::Female)
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

                if (ROLE == role::Cleric)
                {
                    std::vector<std::string> filenames{ "cleric-human-1.png",
                                                        "cleric-human-2.png" };

                    if (SEX == sex::Female)
                    {
                        filenames.emplace_back("cleric-human-female-1.png");
                    }
                    else
                    {
                        filenames.emplace_back("cleric-human-male-1.png");
                        filenames.emplace_back("cleric-human-male-2.png");
                        filenames.emplace_back("cleric-human-male-3.png");
                    }

                    return filenames;
                }

                if (ROLE == role::Knight)
                {
                    std::vector<std::string> filenames{ "knight-human-1.png",
                                                        "knight-human-2.png" };

                    if (SEX == sex::Male)
                    {
                        filenames.emplace_back("knight-human-male-1.png");
                        filenames.emplace_back("knight-human-male-2.png");
                    }

                    return filenames;
                }

                if (ROLE == role::Thief)
                {
                    std::vector<std::string> filenames{ "thief-human-1.png",
                                                        "thief-human-2.png",
                                                        "thief-human-3.png",
                                                        "thief-human-4.png",
                                                        "thief-human-5.png" };

                    if (SEX == sex::Female)
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

                if (ROLE == role::Sorcerer)
                {
                    if (SEX == sex::Female)
                    {
                        return { "sorcerer-human-female-1.png",  "sorcerer-human-female-2.png",
                                 "sorcerer-human-female-2b.png", "sorcerer-human-female-3.png",
                                 "sorcerer-human-female-4.png",  "sorcerer-human-female-5.png",
                                 "sorcerer-human-female-6.png" };
                    }
                    else
                    {
                        return { "sorcerer-human-male-1.png",
                                 "sorcerer-human-male-2.png",
                                 "sorcerer-human-male-2b.png",
                                 "sorcerer-human-male-3.png",
                                 "sorcerer-human-male-4.png" };
                    }
                }

                if (ROLE == role::Ranger)
                {
                    if (SEX == sex::Female)
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

                if (ROLE == role::Trader)
                {
                    if (SEX == sex::Female)
                    {
                        return { "trader-human-1.png", "trader-human-2.png", "trader-human-3.png" };
                    }
                    else
                    {
                        return { "trader-human-1.png",
                                 "trader-human-2.png",
                                 "trader-human-3.png",
                                 "trader-human-4.png" };
                    }
                }

                if (ROLE == role::Drunk)
                {
                    return { "drunk-1.png" };
                }

                if (ROLE == role::Brute)
                {
                    return { "soldier-brute.png" };
                }

                if (ROLE == role::Smasher)
                {
                    return { "soldier-smasher-1.png", "soldier-smasher-2.png" };
                }

                if (ROLE == role::Soldier)
                {
                    if (SEX == sex::Female)
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

                if (ROLE == role::Blacksmith)
                {
                    return { "human-blacksmith-male-1.png", "human-blacksmith-male-2.png" };
                }

                if (ROLE == role::Shaman)
                {
                    return { "human-shaman.png" };
                }

                if (ROLE == role::Grunt)
                {
                    if (SEX == sex::Female)
                    {
                        return {
                            "human-1.png",         "human-2.png",         "human-3.png",
                            "human-4.png",         "human-5.png",         "human-6.png",
                            "human-8.png",         "human-9.png",         "human-10.png",
                            "human-12.png",        "human-13.png",        "human-female-1.png",
                            "human-female-2.png",  "human-female-3.png",  "human-female-4.png",
                            "human-female-5.png",  "human-female-6.png",  "human-female-7.png",
                            "human-female-8.png",  "human-female-9.png",  "human-female-10.png",
                            "human-female-11.png", "human-female-12.png", "human-female-13.png",
                            "human-female-14.png"
                        };
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

            if (RACE == race::Pixie)
            {
                if (SEX == sex::Female)
                {
                    if (ROLE == role::Beastmaster)
                    {
                        return { "pixie-beastmaster-female.png" };
                    }
                    else if (ROLE == role::Sorcerer)
                    {
                        return { "pixie-female-3.png" };
                    }
                    else if (ROLE == role::Cleric)
                    {
                        return { "pixie-female-5.png" };
                    }
                    else if (ROLE == role::Bard)
                    {
                        return { "pixie-female-4.png" };
                    }
                    else
                    {
                        return { "pixie-1.png",        "pixie-2.png",        "pixie-female-1.png",
                                 "pixie-female-2.png", "pixie-female-3.png", "pixie-female-4.png",
                                 "pixie-female-5.png" };
                    }
                }
                else
                {
                    if (ROLE == role::Beastmaster)
                    {
                        return { "pixie-beastmaster-male.png" };
                    }
                    else if (
                        (ROLE == role::Sorcerer) || (ROLE == role::Cleric) || (ROLE == role::Bard))
                    {
                        return { "pixie-1.png", "pixie-2.png", "pixie-male-1.png" };
                    }
                }
            }

            if (RACE == race::Wolfen)
            {
                if (WOLFEN_CLASS == wolfen_class::Count)
                {
                    if (ROLE == role::TwoHeaded)
                    {
                        return { "wolfen-twoheaded.png" };
                    }

                    if (ROLE == role::Wolfen)
                    {
                        return { "wolfen-1.png", "wolfen-2.png", "wolfen-3.png" };
                    }
                }
                else
                {
                    switch (WOLFEN_CLASS)
                    {
                        case creature::wolfen_class::Pup:
                        {
                            return { "wolfen-pup.png" };
                        }
                        case creature::wolfen_class::Juvenile:
                        {
                            return { "wolfen-juvenile.png" };
                        }
                        case creature::wolfen_class::Adult:
                        {
                            return { "wolfen-adult.png" };
                        }
                        case creature::wolfen_class::Noble:
                        {
                            return { "wolfen-noble.png" };
                        }
                        case creature::wolfen_class::Highborn:
                        {
                            return { "wolfen-highborn.png" };
                        }
                        case creature::wolfen_class::Elder:
                        {
                            return { "wolfen-elder.png" };
                        }
                        case creature::wolfen_class::Count:
                        default:
                        {
                            break;
                        }
                    }
                }
            }

            if (RACE == race::Halfling)
            {
                if (ROLE == role::Shaman)
                {
                    return { "halfling-shaman.png" };
                }
                else if (ROLE == role::Chieftain)
                {
                    return { "halfling-chieftain.png" };
                }
                else if (ROLE == role::Captain)
                {
                    return { "halfling-captain.png" };
                }
                else if (ROLE == role::Grunt)
                {
                    return { "halfling-1.png" };
                }
            }

            if ((RACE == race::Lion) && (ROLE == role::Lion))
            {
                return { "lion.png" };
            }

            if ((RACE == race::LionBoar) && (ROLE == role::Lion))
            {
                return { "lion-boar.png" };
            }

            if ((RACE == race::Naga) && (ROLE == role::Grunt))
            {
                return { "naga.png" };
            }

            if (RACE == race::Pug)
            {
                if (ROLE == role::Archer)
                {
                    return { "pug-archer.png" };
                }
                else if (ROLE == role::Cleric)
                {
                    return { "pug-cleric.png" };
                }
                else if (ROLE == role::Grunt)
                {
                    return { "pug-grunt.png" };
                }
            }

            if ((RACE == race::Ramonaut) && (ROLE == role::Ramonaut))
            {
                return { "ramonaut.png" };
            }

            if (RACE == race::Serpent)
            {
                if (ROLE == role::Water)
                {
                    return { "serpent-water.png" };
                }
                else if (ROLE == role::Serpent)
                {
                    return { "serpent-desert.png" };
                }
            }

            if ((RACE == race::Shade) && (ROLE == role::Ghost))
            {
                return { "shade-1.png", "shade-2.png" };
            }

            if ((RACE == race::Cobra) && (ROLE == role::Serpent))
            {
                return { "cobra-1.png", "cobra-2.png" };
            }

            if ((RACE == race::Werebat) && (ROLE == role::Bat))
            {
                return { "werebat-1.png", "werebat-2.png" };
            }

            if ((RACE == race::Bat) && (ROLE == role::Bat))
            {
                return { "bat-1.png", "bat-2.png", "bat-3.png" };
            }

            if (RACE == race::Witch)
            {
                if (ROLE == role::Elder)
                {
                    return { "witch-elder.png" };
                }
                else if (ROLE == role::Ghost)
                {
                    return { "witch-ghost.png" };
                }
                else if (ROLE == role::Strangler)
                {
                    return { "witch-strangler.png" };
                }
                else if (ROLE == role::Mountain)
                {
                    return { "witch-mountain.png" };
                }
                else if (ROLE == role::Sorcerer)
                {
                    return { "witch-1.png" };
                }
            }

            if ((RACE == race::Golem) && (ROLE == role::Brute))
            {
                return { "golem.png" };
            }

            if ((RACE == race::Harpy) && (ROLE == role::Wing))
            {
                return { "harpy.png" };
            }

            if ((RACE == race::ThreeHeadedHound) && (ROLE == role::Mountain))
            {
                return { "hound-three-headed.png" };
            }

            if ((RACE == race::Wereboar) && (ROLE == role::Boar))
            {
                return { "wereboar.png" };
            }

            if ((RACE == race::Werecat) && (ROLE == role::Cat))
            {
                return { "werecat.png" };
            }

            if ((RACE == race::Werewolf) && (ROLE == role::Wolf))
            {
                return { "werewolf.png" };
            }

            if (RACE == race::Wyvern)
            {
                if (ROLE == role::Whelp)
                {
                    return { "wyvern-whelp.png" };
                }
                else if (ROLE == role::Wing)
                {
                    return { "wyvern-1.png", "wyvern-2.png" };
                }
            }

            std::ostringstream ss;
            ss << "sfml_util::gui::CreatureImageManager::GetFilenames(race="
               << ((RACE == creature::race::Count) ? "(count)" : creature::race::ToString(RACE))
               << ", role="
               << ((ROLE == creature::role::Count) ? "(count)" : creature::role::ToString(ROLE))
               << ", sex="
               << ((SEX == creature::sex::Count) ? "(count)" : creature::sex::ToString(SEX))
               << ", wolfen_class=" << WOLFEN_CLASS << ", dragon_class=" << DRAGON_CLASS
               << ") -No filenames found for that creature information.";

            throw std::runtime_error(ss.str());
        }

    } // namespace gui
} // namespace sfml_util
} // namespace heroespath
