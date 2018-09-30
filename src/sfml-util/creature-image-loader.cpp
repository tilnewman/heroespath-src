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
#include "creature-image-loader.hpp"

#include "creature/body-type.hpp"
#include "creature/creature.hpp"
#include "game/game-data-file.hpp"
#include "game/loop-manager.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/filesystem-helpers.hpp"
#include "misc/log-macros.hpp"
#include "misc/random.hpp"
#include "sfml-util/loaders.hpp"
#include "sfutil/image-manip.hpp"

#include <boost/filesystem.hpp>

#include <exception>
#include <sstream>

namespace heroespath
{
namespace sfml_util
{

    CreatureImageLoader::CreatureImageLoader()
        : imageDirectoryPath_("")
    {
        imageDirectoryPath_ = misc::filesystem::MakePathPretty(
            game::GameDataFile::Instance()->GetMediaPath("media-images-creatures-dir"));
    }

    bool CreatureImageLoader::Test() const
    {
        static auto didPostInitial { false };
        if (false == didPostInitial)
        {
            didPostInitial = true;
            game::LoopManager::Instance()->TestingStrAppend(
                "sfml_util::CreatureImageLoader::Test() Starting Tests...");
        }

        static auto allPaths { misc::filesystem::FindFilesInDirectory(
            imageDirectoryPath_, "", ".png") };

        for (auto & pathStr : allPaths)
        {
            boost::algorithm::to_lower(pathStr);
        }

        static misc::EnumUnderlying_t raceIndex { 0 };
        if (raceIndex < creature::race::Count)
        {
            auto const RACE_ENUM { static_cast<creature::race::Enum>(raceIndex) };
            auto const RACE_STR { creature::race::ToString(RACE_ENUM) };
            auto const ROLE_VEC { creature::race::Roles(RACE_ENUM) };

            static misc::EnumUnderlying_t roleIndex { 0 };
            if (roleIndex < ROLE_VEC.size())
            {
                auto const ROLE_ENUM { ROLE_VEC[static_cast<std::size_t>(roleIndex)] };
                auto const ROLE_STR { creature::role::ToString(ROLE_ENUM) };

                static misc::EnumUnderlying_t sexIndex { 0 };
                if (sexIndex < creature::sex::Count)
                {
                    auto const SEX_ENUM { static_cast<creature::sex::Enum>(sexIndex) };
                    auto const SEX_STR { creature::sex::ToString(SEX_ENUM) };

                    // test to ensure that BodyType maker will not throw
                    creature::BodyType::Make_FromRaceAndRole(RACE_ENUM, ROLE_ENUM);

                    static misc::EnumUnderlying_t classIndex { 0 };
                    static std::size_t fileIndex { 0 };
                    if (RACE_ENUM == creature::race::Wolfen)
                    {
                        if (classIndex < creature::wolfen_class::Count)
                        {
                            auto const CLASS_ENUM { static_cast<creature::wolfen_class::Enum>(
                                classIndex) };

                            auto const CLASS_STR { creature::wolfen_class::ToString(CLASS_ENUM) };

                            auto const FILENAMES { Filenames(
                                RACE_ENUM,
                                ROLE_ENUM,
                                SEX_ENUM,
                                CLASS_ENUM,
                                creature::dragon_class::Count) };

                            M_HP_ASSERT_OR_LOG_AND_THROW(
                                (FILENAMES.empty() == false),
                                "sfml_util::CreatureImageLoader() (wolfen_classes) race="
                                    << RACE_STR << ", role=" << ROLE_STR << ", sex=" << SEX_STR
                                    << ", wolfen_class=" << CLASS_STR
                                    << ", GetFilenames() failed to return anything.");

                            if (fileIndex < FILENAMES.size())
                            {
                                auto const FILENAME { FILENAMES.at(fileIndex) };

                                auto const PATH { boost::algorithm::to_lower_copy(Path(FILENAME)) };

                                sf::Texture texture;
                                Load(texture, FILENAME, false);

                                auto imagePathFoundIter { std::find(
                                    std::begin(allPaths), std::end(allPaths), PATH) };

                                if (imagePathFoundIter != std::end(allPaths))
                                {
                                    game::LoopManager::Instance()->TestingImageSet(PATH, true);
                                    allPaths.erase(imagePathFoundIter);
                                }

                                std::ostringstream ss;
                                ss << " CreatureImageLoader Tested race=" << RACE_STR
                                   << " role=" << ROLE_STR << " sex=" << SEX_STR
                                   << " wolfen_class=" << CLASS_STR << " filename=" << FILENAME;

                                game::LoopManager::Instance()->TestingStrAppend(ss.str());

                                allPaths.erase(
                                    std::remove(std::begin(allPaths), std::end(allPaths), PATH),
                                    std::end(allPaths));

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
                        if (classIndex < creature::dragon_class::Count)
                        {
                            auto const CLASS_ENUM { static_cast<creature::dragon_class::Enum>(
                                classIndex) };

                            auto const CLASS_STR { creature::dragon_class::ToString(CLASS_ENUM) };

                            auto const FILENAMES { Filenames(
                                RACE_ENUM,
                                ROLE_ENUM,
                                SEX_ENUM,
                                creature::wolfen_class::Count,
                                CLASS_ENUM) };

                            M_HP_ASSERT_OR_LOG_AND_THROW(
                                (FILENAMES.empty() == false),
                                "sfml_util::CreatureImageLoader() (dragon_classes) race="
                                    << RACE_STR << ", role=" << ROLE_STR << ", sex=" << SEX_STR
                                    << ", dragon_class=" << CLASS_STR
                                    << ", GetFilenames() failed to return anything.");

                            if (fileIndex < FILENAMES.size())
                            {
                                auto const FILENAME { FILENAMES.at(fileIndex) };

                                auto const PATH { boost::algorithm::to_lower_copy(Path(FILENAME)) };

                                sf::Texture texture;
                                Load(texture, FILENAME, false);

                                auto imagePathFoundIter { std::find(
                                    std::begin(allPaths), std::end(allPaths), PATH) };

                                if (imagePathFoundIter != std::end(allPaths))
                                {
                                    game::LoopManager::Instance()->TestingImageSet(PATH, true);
                                    allPaths.erase(imagePathFoundIter);
                                }

                                std::ostringstream ss;
                                ss << " CreatureImageLoader Tested race=" << RACE_STR
                                   << " role=" << ROLE_STR << " sex=" << SEX_STR
                                   << " dragon_class=" << CLASS_STR << " filename=" << FILENAME;

                                game::LoopManager::Instance()->TestingStrAppend(ss.str());

                                allPaths.erase(
                                    std::remove(std::begin(allPaths), std::end(allPaths), PATH),
                                    std::end(allPaths));

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
                        auto const FILENAMES { Filenames(
                            RACE_ENUM,
                            ROLE_ENUM,
                            SEX_ENUM,
                            creature::wolfen_class::Count,
                            creature::dragon_class::Count) };

                        M_HP_ASSERT_OR_LOG_AND_THROW(
                            (FILENAMES.empty() == false),
                            "sfml_util::CreatureImageLoader() race="
                                << RACE_STR << ", role=" << ROLE_STR << ", sex=" << SEX_STR
                                << ", GetFilenames() failed to return anything.");

                        if (fileIndex < FILENAMES.size())
                        {
                            auto const FILENAME { FILENAMES.at(fileIndex) };

                            auto const PATH { boost::algorithm::to_lower_copy(Path(FILENAME)) };

                            sf::Texture texture;
                            Load(texture, FILENAME, false);

                            auto imagePathFoundIter { std::find(
                                std::begin(allPaths), std::end(allPaths), PATH) };

                            if (imagePathFoundIter != std::end(allPaths))
                            {
                                game::LoopManager::Instance()->TestingImageSet(PATH, true);
                                allPaths.erase(imagePathFoundIter);
                            }

                            std::ostringstream ss;
                            ss << " CreatureImageLoader Tested race=" << RACE_STR
                               << " role=" << ROLE_STR << " sex=" << SEX_STR
                               << " filename=" << FILENAME;

                            game::LoopManager::Instance()->TestingStrAppend(ss.str());

                            allPaths.erase(
                                std::remove(std::begin(allPaths), std::end(allPaths), PATH),
                                std::end(allPaths));

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

            roleIndex = 0;
            ++raceIndex;
            return false;
        }

        std::sort(std::begin(allPaths), std::end(allPaths));

        for (auto const & PATH : allPaths)
        {
            M_HP_LOG_WRN(
                "sfml_util::CreatureImageLoader::Test() found the following item image "
                "unused: "
                << PATH);
        }

        game::LoopManager::Instance()->TestingStrAppend(
            "sfml_util::CreatureImageLoader::Test()  ALL TESTS PASSED.");

        return true;
    }

    const std::string CreatureImageLoader::Path(const creature::CreaturePtr_t CREATURE_PTR) const
    {
        return Path(CREATURE_PTR->ImageFilename());
    }

    const std::string CreatureImageLoader::Path(const std::string & FILENAME) const
    {
        return misc::filesystem::CompletePath(imageDirectoryPath_, FILENAME);
    }

    void CreatureImageLoader::Load(
        sf::Texture & texture, const creature::CreaturePtr_t CREATURE_PTR) const
    {
        return Load(texture, CREATURE_PTR->ImageFilename(), WillHorizFlipToFaceRight(CREATURE_PTR));
    }

    void CreatureImageLoader::Load(
        sf::Texture & texture,
        const std::string & FILENAME,
        const bool WILL_HORIZ_FLIP_TO_FACE_RIGHT) const
    {
        sfml_util::Loaders::Texture(texture, Path(FILENAME));

        if (WILL_HORIZ_FLIP_TO_FACE_RIGHT)
        {
            sfutil::FlipHoriz(texture);
        }
    }

    const std::string
        CreatureImageLoader::FilenameRandom(const creature::CreaturePtr_t CREATURE_PTR) const
    {
        auto const FILENAMES { Filenames(
            CREATURE_PTR->Race(),
            CREATURE_PTR->Role(),
            CREATURE_PTR->Sex(),
            CREATURE_PTR->WolfenClass(),
            CREATURE_PTR->DragonClass()) };

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (FILENAMES.empty() == false),
            "sfml_util::CreatureImageLoader::GetRandomFilename(creature={"
                << CREATURE_PTR->ToString()
                << "}) (which actually calls GetFilenames()) returned no filenames.");

        return misc::Vector::SelectRandom(FILENAMES);
    }

    void CreatureImageLoader::EnsureFileExists(const std::string & FILENAME) const
    {
        auto const FULL_PATH_STR { Path(FILENAME) };

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (misc::filesystem::DoesFileExist(FULL_PATH_STR)),
            "sfml_util::CreatureImageLoader::EnsureFileExists(\""
                << FULL_PATH_STR
                << "\") but that file either does not exist or is not a regular file.");
    }

    bool CreatureImageLoader::WillHorizFlipToFaceRight(
        const creature::CreaturePtr_t CREATURE_PTR) const
    {
        return CREATURE_PTR->IsPlayerCharacter();
    }

    const std::vector<std::string> CreatureImageLoader::Filenames(
        const creature::race::Enum RACE,
        const creature::role::Enum ROLE,
        const creature::sex::Enum SEX,
        const creature::wolfen_class::Enum WOLFEN_CLASS,
        const creature::dragon_class::Enum DRAGON_CLASS) const
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
                return { "troll-grunt-1.png", "troll-grunt-2.png" };
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

            if (ROLE == role::TwoHeaded)
            {
                return { "troll-two-headed.png" };
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
                return { "minotaur-grunt-1.png", "minotaur-grunt-2.png", "minotaur-grunt-3.png" };
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

            return { "minotaur-1.png", "minotaur-2.png" };
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
                    return { "goblin-trader-male-1.png", "goblin-trader-male-2.png",
                             "goblin-male-1.png",        "goblin-male-2.png",
                             "goblin-male-3.png",        "goblin-male-4.png",
                             "goblin-male-5.png" };
                }
            }

            if (ROLE == role::Grunt)
            {
                if (SEX == sex::Female)
                {
                    return { "goblin-female-1.png", "goblin-female-2.png", "goblin-female-3.png",
                             "goblin-female-4.png", "goblin-grunt-2.png",  "goblin-grunt-4.png",
                             "goblin-grunt-5.png",  "goblin-grunt-6.png",  "goblin-grunt-7.png",
                             "goblin-grunt-8.png",  "goblin-grunt-9.png",  "goblin-grunt-10.png",
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
                std::vector<std::string> filenames { "knight-gnome-1.png", "knight-gnome-2.png" };

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
                std::vector<std::string> filenames {
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
                std::vector<std::string> filenames {
                    "gnome-grunt.png", "gnome-2.png", "gnome-3.png", "gnome-6.png", "gnome-7.png"
                };

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
                std::vector<std::string> filenames { "knight-human-1.png", "knight-human-2.png" };

                if (SEX == sex::Male)
                {
                    filenames.emplace_back("knight-human-male-1.png");
                    filenames.emplace_back("knight-human-male-2.png");
                }

                return filenames;
            }

            if (ROLE == role::Archer)
            {
                std::vector<std::string> filenames { "archer-1.png", "archer-2.png" };

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
                std::vector<std::string> filenames { "cleric-human-1.png", "cleric-human-2.png" };

                if (SEX == sex::Female)
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

            if (ROLE == role::Knight)
            {
                std::vector<std::string> filenames { "knight-human-1.png", "knight-human-2.png" };

                if (SEX == sex::Male)
                {
                    filenames.emplace_back("knight-human-male-1.png");
                    filenames.emplace_back("knight-human-male-2.png");
                }

                return filenames;
            }

            if (ROLE == role::Thief)
            {
                std::vector<std::string> filenames { "thief-human-1.png",
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
                    return { "sorcerer-human-male-1.png",  "sorcerer-human-male-2.png",
                             "sorcerer-human-male-2b.png", "sorcerer-human-male-3.png",
                             "sorcerer-human-male-4.png",  "sorcerer-human-male-5.png" };
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
                             "trader-human-4.png",
                             "trader-human-5.png" };
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
                if (SEX == sex::Male)
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

            if (ROLE == role::Shaman)
            {
                return { "human-shaman.png" };
            }

            if (ROLE == role::Grunt)
            {
                if (SEX == sex::Female)
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
                    return { "pixie-female-1.png", "pixie-female-3.png" };
                }
                else if (ROLE == role::Cleric)
                {
                    return { "pixie-female-2.png", "pixie-female-5.png" };
                }
                else if (ROLE == role::Bard)
                {
                    return { "pixie-female-4.png", "pixie-3.png" };
                }
            }
            else
            {
                if (ROLE == role::Beastmaster)
                {
                    return { "pixie-beastmaster-male.png" };
                }
                else if ((ROLE == role::Sorcerer) || (ROLE == role::Cleric) || (ROLE == role::Bard))
                {
                    return { "pixie-1.png", "pixie-2.png", "pixie-3.png", "pixie-male-1.png" };
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
        ss << "sfml_util::CreatureImageLoader::GetFilenames(race="
           << ((RACE == creature::race::Count) ? "(count)" : creature::race::ToString(RACE))
           << ", role="
           << ((ROLE == creature::role::Count) ? "(count)" : creature::role::ToString(ROLE))
           << ", sex=" << ((SEX == creature::sex::Count) ? "(count)" : creature::sex::ToString(SEX))
           << ", wolfen_class=" << WOLFEN_CLASS << ", dragon_class=" << DRAGON_CLASS
           << ") -No filenames found for that creature information.";

        throw std::runtime_error(ss.str());
    }

} // namespace sfml_util
} // namespace heroespath
