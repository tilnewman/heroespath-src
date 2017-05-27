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

#include "sfml-util/sfml-util.hpp"
#include "sfml-util/loaders.hpp"

#include "game/log-macros.hpp"
#include "game/loop-manager.hpp"
#include "game/creature/body-type.hpp"

#include "misc/random.hpp"
#include "misc/assertlogandthrow.hpp"

#include <boost/filesystem.hpp>

#include <exception>
#include <sstream>


namespace sfml_util
{
namespace gui
{

    std::string CreatureImageManager::imagesDirectoryPath_;
    std::unique_ptr<CreatureImageManager> CreatureImageManager::instanceUPtr_{ nullptr };


    CreatureImageManager::CreatureImageManager()
    {}


    CreatureImageManager::~CreatureImageManager()
    {}


    CreatureImageManager * CreatureImageManager::Instance()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            Acquire();
        }

        return instanceUPtr_.get();
    }


    void CreatureImageManager::Acquire()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            instanceUPtr_.reset(new CreatureImageManager);
        }
    }


    void CreatureImageManager::Release()
    {
        M_ASSERT_OR_LOGANDTHROW_SS((instanceUPtr_.get() != nullptr), "sfml_util::gui::CreatureImageManager::Release() found instanceUPtr that was null.");
        instanceUPtr_.reset();
    }


    void CreatureImageManager::SetCreatureImageDirectory(const std::string & PATH)
    {
        imagesDirectoryPath_ = PATH;
    }


    bool CreatureImageManager::Test()
    {
        static auto didPostInitial{ false };
        if (false == didPostInitial)
        {
            didPostInitial = true;
            game::LoopManager::Instance()->TestingStrAppend("sfml_util::gui::CreatureImageManager::Test() Starting Tests...");
        }

        auto cimSPtr{ CreatureImageManager::Instance() };

        static auto raceIndex { 0 };
        static auto roleIndex { 0 };
        static auto sexIndex  { 0 };
        static auto classIndex{ 0 };

        if (raceIndex < static_cast<int>(game::creature::race::Count))
        {
            auto const RACE_ENUM{ static_cast<game::creature::race::Enum>(raceIndex) };
            auto const RACE_STR { game::creature::race::ToString(RACE_ENUM) };

            if (roleIndex < static_cast<int>(game::creature::role::Count))
            {
                auto const ROLE_ENUM{ static_cast<game::creature::role::Enum>(roleIndex) };
                auto const ROLE_STR { game::creature::role::ToString(ROLE_ENUM) };

                if (sexIndex < static_cast<int>(game::creature::sex::Count))
                {
                    static std::size_t i{ 0 };

                    auto const SEX_ENUM{ static_cast<game::creature::sex::Enum>(sexIndex) };
                    auto const SEX_STR { game::creature::sex::ToString(SEX_ENUM) };

                    //test to ensure that BodyType maker will not throw
                    auto const BODY_TYPE{ game::creature::BodyType::Make_FromRaceAndRole(RACE_ENUM, ROLE_ENUM) };

                    //skip sex unknown for Gnomes and Dragons
                    if (SEX_ENUM == game::creature::sex::Unknown)
                    {
                        if ((RACE_ENUM == game::creature::race::Gnome) ||
                            (RACE_ENUM == game::creature::race::Dragon))
                        {
                            i = 0;
                            classIndex = 0;
                            ++sexIndex;
                            return false;
                        }
                    }

                    if (RACE_ENUM == game::creature::race::Wolfen)
                    {
                        if (classIndex < static_cast<int>(game::creature::wolfen_class::Count))
                        {
                            auto const CLASS_ENUM{ static_cast<game::creature::wolfen_class::Enum>(classIndex) };
                            auto const CLASS_STR { game::creature::wolfen_class::ToString(CLASS_ENUM) };

                            std::vector<std::string> filenamesVec;
                            cimSPtr->GetFilenames(filenamesVec, RACE_ENUM, ROLE_ENUM, SEX_ENUM, CLASS_ENUM, game::creature::dragon_class::Count);

                            M_ASSERT_OR_LOGANDTHROW_SS((filenamesVec.empty() == false), "sfml_util::gui::CreatureImageManager() (wolfen_classes) race=" << RACE_STR << ", role=" << ROLE_STR << ", sex=" << SEX_STR << ", wolfen_class=" << CLASS_STR << ", GetFilenames() failed to return anything.");

                            if (i < filenamesVec.size())
                            {
                                sf::Texture texture;
                                cimSPtr->GetImage(texture, filenamesVec.at(i));
                                game::LoopManager::Instance()->TestingImageSet(texture);

                                std::ostringstream ss;
                                ss << " CreatureImageManager Tested race=" << RACE_STR << " role=" << ROLE_STR << " sex=" << SEX_STR << " wolfen_class=" << CLASS_STR << " filename=" << filenamesVec.at(i);
                                game::LoopManager::Instance()->TestingStrAppend(ss.str());

                                ++i;
                                return false;
                            }

                            i = 0;
                            ++classIndex;
                            return false;
                        }
                    }
                    else if (RACE_ENUM == game::creature::race::Dragon)
                    {
                        if ((ROLE_ENUM == game::creature::role::Firebrand) || (ROLE_ENUM == game::creature::role::Sylavin))
                        {
                            if (classIndex < static_cast<int>(game::creature::dragon_class::Count))
                            {
                                auto const CLASS_ENUM{ static_cast<game::creature::dragon_class::Enum>(classIndex) };
                                auto const CLASS_STR { game::creature::dragon_class::ToString(CLASS_ENUM) };

                                std::vector<std::string> filenamesVec;
                                cimSPtr->GetFilenames(filenamesVec, RACE_ENUM, ROLE_ENUM, SEX_ENUM, game::creature::wolfen_class::Count, CLASS_ENUM);

                                M_ASSERT_OR_LOGANDTHROW_SS((filenamesVec.empty() == false), "sfml_util::gui::CreatureImageManager() (dragon_classes) race=" << RACE_STR << ", role=" << ROLE_STR << ", sex=" << SEX_STR << ", dragon_class=" << CLASS_STR << ", GetFilenames() failed to return anything.");

                                if (i < filenamesVec.size())
                                {
                                    sf::Texture texture;
                                    cimSPtr->GetImage(texture, filenamesVec.at(i));
                                    game::LoopManager::Instance()->TestingImageSet(texture);

                                    std::ostringstream ss;
                                    ss << " CreatureImageManager Tested race=" << RACE_STR << " role=" << ROLE_STR << " sex=" << SEX_STR << " dragon_class=" << CLASS_STR << " filename=" << filenamesVec.at(i);
                                    game::LoopManager::Instance()->TestingStrAppend(ss.str());

                                    ++i;
                                    return false;
                                }

                                i = 0;
                                ++classIndex;
                                return false;
                            }
                        }
                    }
                    else
                    {
                        std::vector<std::string> filenamesVec;
                        cimSPtr->GetFilenames(filenamesVec, RACE_ENUM, ROLE_ENUM, SEX_ENUM, game::creature::wolfen_class::Count, game::creature::dragon_class::Count);

                        M_ASSERT_OR_LOGANDTHROW_SS((filenamesVec.empty() == false), "sfml_util::gui::CreatureImageManager() race=" << RACE_STR << ", role=" << ROLE_STR << ", sex=" << SEX_STR << ", GetFilenames() failed to return anything.");

                        if (i < filenamesVec.size())
                        {
                            sf::Texture texture;
                            cimSPtr->GetImage(texture, filenamesVec.at(i));
                            game::LoopManager::Instance()->TestingImageSet(texture);

                            std::ostringstream ss;
                            ss << " CreatureImageManager Tested race=" << RACE_STR << " role=" << ROLE_STR << " sex=" << SEX_STR << " filename=" << filenamesVec.at(i);
                            game::LoopManager::Instance()->TestingStrAppend(ss.str());

                            ++i;
                            return false;
                        }
                    }

                    i = 0;
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

        game::LoopManager::Instance()->TestingStrAppend("sfml_util::gui::CreatureImageManager::Test()  ALL TESTS PASSED.");
        return true;
    }


    void CreatureImageManager::GetImage(sf::Texture &       texture,
                                        const std::string & FILENAME,
                                        const bool          WILL_FACE_RIGHT) const
    {
        return LoadImage(texture, FILENAME, WILL_FACE_RIGHT);
    }


    void CreatureImageManager::GetImage(sf::Texture &                            texture,
                                        const game::creature::race::Enum         RACE,
                                        const game::creature::role::Enum         ROLE,
                                        const game::creature::sex::Enum          SEX,
                                        const bool                               WILL_PICK_RANDOM,
                                        const bool                               WILL_FACE_RIGHT,
                                        const game::creature::wolfen_class::Enum WOLFEN_CLASS,
                                        const game::creature::dragon_class::Enum DRAGON_CLASS) const
    {
        const std::string FILENAME(GetFilename(RACE, ROLE, SEX, WILL_PICK_RANDOM, WOLFEN_CLASS, DRAGON_CLASS));
        M_ASSERT_OR_LOGANDTHROW_SS((FILENAME.empty() == false), "sfml_util::gui::CreatureImageManager::GetImage()  GetFilename() call return no filename.  (race=" << game::creature::race::ToString(RACE) << ", role=" << game::creature::role::ToString(ROLE) << ", sex=" << game::creature::sex::ToString(SEX) << ", wolfen_class=" << WOLFEN_CLASS << ", dragon_class=" << DRAGON_CLASS << ")");
        return LoadImage(texture, FILENAME, WILL_FACE_RIGHT);
    }


    void CreatureImageManager::LoadImage(sf::Texture &       texture,
                                         const std::string & IMAGE_FILE_NAME,
                                         const bool          WILL_FACE_RIGHT) const
    {
        namespace bfs = boost::filesystem;
        const bfs::path PATH_OBJ(bfs::system_complete(bfs::path(imagesDirectoryPath_) / bfs::path(IMAGE_FILE_NAME)));

        sfml_util::LoadImageOrTexture(texture, PATH_OBJ.string());

        if (WILL_FACE_RIGHT)
        {
            sfml_util::FlipHoriz(texture);
        }
    }


    const std::string CreatureImageManager::GetFilename(const game::creature::race::Enum         RACE,
                                                        const game::creature::role::Enum         ROLE,
                                                        const game::creature::sex::Enum          SEX,
                                                        const bool                                     WILL_PICK_RANDOM,
                                                        const game::creature::wolfen_class::Enum WOLFEN_CLASS,
                                                        const game::creature::dragon_class::Enum DRAGON_CLASS) const
    {
        using namespace game::creature;

        std::vector<std::string> filenameVec;
        GetFilenames(filenameVec, RACE, ROLE, SEX, WOLFEN_CLASS, DRAGON_CLASS);

        M_ASSERT_OR_LOGANDTHROW_SS((filenameVec.empty() == false), "sfml_util::gui::CreatureImageManager::GetFilename()  GetFilenames() call return no filenames.  (race=" << game::creature::race::ToString(RACE) << ", role=" << game::creature::role::ToString(ROLE) << ", sex=" << game::creature::sex::ToString(SEX) << ", wolfen_class=" << WOLFEN_CLASS << ", dragon_class=" << DRAGON_CLASS << ")");

        if (WILL_PICK_RANDOM && (filenameVec.size() > 1))
        {
            return filenameVec[static_cast<std::size_t>(misc::random::Int(0, static_cast<int>(filenameVec.size()) - 1))];
        }
        else
        {
            return filenameVec[0];
        }
    }


    void CreatureImageManager::GetFilenames(std::vector<std::string> &               outputVec,
                                            const game::creature::race::Enum         RACE,
                                            const game::creature::role::Enum         ROLE,
                                            const game::creature::sex::Enum          SEX,
                                            const game::creature::wolfen_class::Enum WOLFEN_CLASS,
                                            const game::creature::dragon_class::Enum DRAGON_CLASS) const
    {
        using namespace game::creature;

        if (RACE == race::Troll)
        {
            if (ROLE == role::Berserker)
            {
                outputVec.push_back("troll-berserker.png");
                return;
            }

            if (ROLE == role::Brute)
            {
                outputVec.push_back("troll-brute.png");
                return;
            }

            if (ROLE == role::Chieftain)
            {
                outputVec.push_back("troll-chieftain.png");
                return;
            }

            if (ROLE == role::Grunt)
            {
                outputVec.push_back("troll-grunt-1.png");
                return;
            }

            if (ROLE == role::Mountain)
            {
                outputVec.push_back("troll-mountain.png");
                return;
            }

            if (ROLE == role::Shaman)
            {
                outputVec.push_back("troll-shaman.png");
                return;
            }

            if (ROLE == role::Smasher)
            {
                outputVec.push_back("troll-smasher.png");
                return;
            }

            if (ROLE == role::Strangler)
            {
                outputVec.push_back("troll-strangler.png");
                return;
            }

            if (ROLE == role::Warlord)
            {
                outputVec.push_back("troll-warlord.png");
                return;
            }

            outputVec.push_back("troll-1.png");
            return;
        }

        if (RACE == race::Orc)
        {
            if (ROLE == role::Berserker)
            {
                outputVec.push_back("orc-berserker.png");
                return;
            }

            if (ROLE == role::Chieftain)
            {
                outputVec.push_back("orc-chieftain.png");
                return;
            }

            if (ROLE == role::Elder)
            {
                outputVec.push_back("orc-elder.png");
                return;
            }

            if (ROLE == role::FourArmed)
            {
                outputVec.push_back("orc-four-armed.png");
                return;
            }

            if (ROLE == role::Captain)
            {
                outputVec.push_back("orc-captain.png");
                return;
            }

            if (ROLE == role::Mountain)
            {
                outputVec.push_back("orc-mountain.png");
                return;
            }

            if (ROLE == role::Spike)
            {
                outputVec.push_back("orc-spike.png");
                return;
            }

            if (ROLE == role::Grunt)
            {
                for(std::size_t i(1); i<=13; ++i)
                {
                    std::ostringstream ss;
                    ss << "orc-grunt-" << i << ".png";
                    outputVec.push_back(ss.str());
                }

                return;
            }

            if (ROLE == role::Shaman)
            {
                outputVec.push_back("orc-shaman.png");
                return;
            }

            if (ROLE == role::Smasher)
            {
                outputVec.push_back("orc-smasher.png");
                return;
            }

            if (ROLE == role::Warlord)
            {
                outputVec.push_back("orc-warlord.png");
                return;
            }

            outputVec.push_back("orc-grunt-1.png");
            return;
        }

        if (RACE == race::Newt)
        {
            if (ROLE == role::Chieftain)
            {
                outputVec.push_back("newt-chieftain.png");
                return;
            }

            if (ROLE == role::Cleric)
            {
                outputVec.push_back("newt-cleric.png");
                return;
            }

            if (ROLE == role::Elder)
            {
                outputVec.push_back("newt-elder.png");
                return;
            }

            if (ROLE == role::Sorcerer)
            {
                outputVec.push_back("newt-sorcerer.png");
                return;
            }

            if (ROLE == role::Trader)
            {
                outputVec.push_back("newt-trader.png");
                return;
            }

            for(std::size_t i(1); i<=6; ++i)
            {
                std::ostringstream ss;
                ss << "newt-" << i << ".png";
                outputVec.push_back(ss.str());
            }
            return;
        }

        if (RACE == race::Spider)
        {
            for (std::size_t i(1); i <= 9; ++i)
            {
                std::ostringstream ss;
                ss << "spider-giant-" << i << ".png";
                outputVec.push_back(ss.str());
            }
            return;
        }

        if (RACE == race::Bog)
        {
            if (ROLE == role::Chieftain)
            {
                outputVec.push_back("bog-chieftain.png");
                return;
            }

            if (ROLE == role::Smasher)
            {
                outputVec.push_back("bog-smasher.png");
                return;
            }

            if (ROLE == role::Sorcerer)
            {
                outputVec.push_back("bog-sorcerer.png");
                return;
            }

            if (ROLE == role::Shaman)
            {
                outputVec.push_back("bog-shaman.png");
                return;
            }

            if (ROLE == role::Spike)
            {
                outputVec.push_back("bog-spike.png");
                return;
            }

            if (ROLE == role::Tendrilus)
            {
                outputVec.push_back("bog-tendrilus.png");
                return;
            }

            if (ROLE == role::Wing)
            {
                outputVec.push_back("bog-wing.png");
                return;
            }

            if (ROLE == role::Whelp)
            {
                outputVec.push_back("bog-whelp.png");
                return;
            }

            outputVec.push_back("bog-grunt-1.png");
            outputVec.push_back("bog-grunt-2.png");
            outputVec.push_back("bog-grunt-3.png");
            return;
        }

        if (RACE == race::CaveCrawler)
        {
            outputVec.push_back("cave-crawler-1.png");
            outputVec.push_back("cave-crawler-2.png");
            outputVec.push_back("cave-crawler-3.png");
            return;
        }

        if (RACE == race::Hydra)
        {
            outputVec.push_back("hydra-1.png");
            return;
        }

        if (RACE == race::LizardWalker)
        {
            if (ROLE == role::Whelp)
            {
                outputVec.push_back("lizard-walker-whelp-1.png");
                outputVec.push_back("lizard-walker-whelp-2.png");
                return;
            }

            if (ROLE == role::Sorcerer)
            {
                if (SEX == sex::Female)
                {
                    outputVec.push_back("lizard-walker-sorcerer-female.png");
                    return;
                }
                else
                {
                    outputVec.push_back("lizard-walker-sorcerer-male.png");
                    return;
                }
            }

            if (ROLE == role::Captain)
            {
                outputVec.push_back("lizard-walker-captain.png");
                return;
            }

            if (ROLE == role::Chieftain)
            {
                outputVec.push_back("lizard-walker-chieftain.png");
                return;
            }

            if (ROLE == role::Spike)
            {
                outputVec.push_back("lizard-walker-spike.png");
                return;
            }

            if (ROLE == role::Shaman)
            {
                if (SEX == sex::Female)
                {
                    outputVec.push_back("lizard-walker-shaman-female.png");
                    return;
                }
                else
                {
                    outputVec.push_back("lizard-walker-shaman-male.png");
                    return;
                }
            }

            for(std::size_t i(1); i<=12; ++i)
            {
                std::ostringstream ss;
                ss << "lizard-walker-" << i << ".png";
                outputVec.push_back(ss.str());
            }
            return;
        }

        if (RACE == race::Minotaur)
        {
            if (ROLE == role::Grunt)
            {
                outputVec.push_back("minotaur-grunt-1.png");
                outputVec.push_back("minotaur-grunt-2.png");
                outputVec.push_back("minotaur-grunt-3.png");
                return;
            }

            if (ROLE == role::Brute)
            {
                outputVec.push_back("minotaur-brute.png");
                return;
            }

            if (ROLE == role::FourArmed)
            {
                outputVec.push_back("minotaur-fourarmed.png");
                return;
            }

            if (ROLE == role::Mountain)
            {
                outputVec.push_back("minotaur-mountain.png");
                return;
            }

            if (ROLE == role::Shaman)
            {
                outputVec.push_back("minotaur-shaman.png");
                return;
            }

            if (ROLE == role::Warlord)
            {
                outputVec.push_back("minotaur-warlord.png");
                return;
            }

            outputVec.push_back("minotaur-1.png");
            outputVec.push_back("minotaur-2.png");
            return;
        }

        if (RACE == race::Ogre)
        {
            if (ROLE == role::Berserker)
            {
                outputVec.push_back("ogre-berserker.png");
                return;
            }

            if (ROLE == role::Brute)
            {
                outputVec.push_back("ogre-brute.png");
                return;
            }

            if (ROLE == role::Elder)
            {
                outputVec.push_back("ogre-elder.png");
                return;
            }

            if (ROLE == role::FourArmed)
            {
                outputVec.push_back("ogre-four-armed.png");
                return;
            }

            if (ROLE == role::Giant)
            {
                outputVec.push_back("ogre-giant.png");
                return;
            }

            if (ROLE == role::Mountain)
            {
                outputVec.push_back("ogre-mountain.png");
                return;
            }

            if (ROLE == role::Warlord)
            {
                outputVec.push_back("ogre-warlord.png");
                return;
            }

            if (ROLE == role::Captain)
            {
                outputVec.push_back("ogre-captain.png");
                return;
            }

            if (ROLE == role::Chieftain)
            {
                outputVec.push_back("ogre-chieftain.png");
                return;
            }

            if (ROLE == role::Shaman)
            {
                outputVec.push_back("ogre-shaman.png");
                return;
            }

            if (ROLE == role::Whelp)
            {
                outputVec.push_back("ogre-whelp.png");
                return;
            }

            outputVec.push_back("ogre-1.png");
            outputVec.push_back("ogre-2.png");
            outputVec.push_back("ogre-3.png");
            outputVec.push_back("ogre-4.png");
            return;
        }

        if (RACE == race::Plant)
        {
            if (ROLE == role::Smasher)
            {
                outputVec.push_back("plant-smasher.png");
                return;
            }

            if (ROLE == role::Tendrilus)
            {
                outputVec.push_back("plant-tendrilus-1.png");
                outputVec.push_back("plant-tendrilus-2.png");
                return;
            }

            if (ROLE == role::Whelp)
            {
                outputVec.push_back("plant-whelp.png");
                return;
            }

            outputVec.push_back("plant-pod-1.png");
            outputVec.push_back("plant-pod-2.png");
            return;
        }

        if (RACE == race::Skeleton)
        {
            if (ROLE == role::Chieftain)
            {
                outputVec.push_back("skeleton-chieftain.png");
                return;
            }

            if (ROLE == role::FourArmed)
            {
                outputVec.push_back("skeleton-four-armed.png");
                return;
            }

            if (ROLE == role::Mountain)
            {
                outputVec.push_back("skeleton-mountain.png");
                return;
            }

            for (std::size_t i(1); i <= 8; ++i)
            {
                std::ostringstream ss;
                ss << "skeleton-" << i << ".png";
                outputVec.push_back(ss.str());
            }
            return;
        }

        if (RACE == race::Werebear)
        {
            if (ROLE == role::Elder)
            {
                outputVec.push_back("werebear-elder.png");
                return;
            }

            if (ROLE == role::Grunt)
            {
                outputVec.push_back("werebear-grunt-1.png");
                outputVec.push_back("werebear-grunt-2.png");
                return;
            }

            outputVec.push_back("werebear-1.png");
            return;
        }

        if (RACE == race::Beetle)
        {
            outputVec.push_back("beetle-giant.png");
            return;
        }

        if (RACE == race::Boar)
        {
            outputVec.push_back("boar-1.png");
            return;
        }

        if (RACE == race::Demon)
        {
            if (ROLE == role::Skeleton)
            {
                outputVec.push_back("demon-skeleton.png");
                return;
            }

            if (ROLE == role::Spike)
            {
                outputVec.push_back("demon-spike.png");
                return;
            }

            if (ROLE == role::Strangler)
            {
                outputVec.push_back("demon-strangler.png");
                return;
            }

            if (ROLE == role::Whelp)
            {
                for(std::size_t i(1); i<=4; ++i)
                {
                    std::ostringstream ss;
                    ss << "demon-whelp-" << i << ".png";
                    outputVec.push_back(ss.str());
                }
                return;
            }

            if (ROLE == role::Wing)
            {
                outputVec.push_back("demon-wing-1.png");
                outputVec.push_back("demon-wing-2.png");
                return;
            }

            for (std::size_t i(1); i <= 9; ++i)
            {
                std::ostringstream ss;
                ss << "demon-" << i << ".png";
                outputVec.push_back(ss.str());
            }
            return;
        }

        if (RACE == race::Ghoul)
        {
            if (ROLE == role::Strangler)
            {
                outputVec.push_back("ghoul-strangler.png");
                return;
            }

            if (ROLE == role::Spike)
            {
                outputVec.push_back("ghoul-spike.png");
                return;
            }

            outputVec.push_back("ghoul-1.png");
            outputVec.push_back("ghoul-2.png");
            outputVec.push_back("ghoul-3.png");
            return;
        }

        if (RACE == race::Griffin)
        {
            if (ROLE == role::Whelp)
            {
                outputVec.push_back("griffin-whelp.png");
                return;
            }

            outputVec.push_back("griffin-1.png");
            return;
        }

        if (RACE == race::Giant)
        {
            if (ROLE == role::Smasher)
            {
                outputVec.push_back("giant-smasher.png");
                return;
            }

            if (ROLE == role::Strangler)
            {
                outputVec.push_back("giant-strangler.png");
                return;
            }

            if (ROLE == role::Warlord)
            {
                outputVec.push_back("giant-warlord.png");
                return;
            }

            if (ROLE == role::Mountain)
            {
                outputVec.push_back("giant-mountain.png");
                return;
            }

            if (ROLE == role::Grunt)
            {
                outputVec.push_back("giant-grunt-1.png");
                outputVec.push_back("giant-grunt-2.png");
                outputVec.push_back("giant-grunt-3.png");
                return;
            }

            outputVec.push_back("giant-1.png");
            return;
        }

        if (RACE == race::Goblin)
        {
            if (ROLE == role::Thief)
            {
                outputVec.push_back("goblin-thief.png");
                return;
            }

            if (ROLE == role::Sorcerer)
            {
                outputVec.push_back("goblin-sorcerer.png");
                return;
            }

            if (ROLE == role::Brute)
            {
                outputVec.push_back("goblin-brute.png");
                return;
            }

            if (ROLE == role::Archer)
            {
                outputVec.push_back("goblin-archer.png");
                return;
            }

            if (ROLE == role::Captain)
            {
                outputVec.push_back("goblin-captain.png");
                return;
            }

            if (ROLE == role::Chieftain)
            {
                outputVec.push_back("goblin-chieftain.png");
                return;
            }

            if (ROLE == role::Shaman)
            {
                outputVec.push_back("goblin-shaman.png");
                return;
            }

            if (ROLE == role::Trader)
            {
                if (SEX == sex::Female)
                {
                    outputVec.push_back("goblin-trader-female.png");
                    return;
                }
                else
                {
                    outputVec.push_back("goblin-trader-male.png");
                    return;
                }
            }

            if (ROLE == role::Grunt)
            {
                if (SEX == sex::Female)
                {
                    outputVec.push_back("goblin-female-1.png");
                    outputVec.push_back("goblin-female-2.png");
                    outputVec.push_back("goblin-female-3.png");
                    outputVec.push_back("goblin-female-4.png");
                    outputVec.push_back("goblin-grunt-2.png");
                    outputVec.push_back("goblin-grunt-4.png");
                    outputVec.push_back("goblin-grunt-5.png");
                    outputVec.push_back("goblin-grunt-6.png");
                    outputVec.push_back("goblin-grunt-7.png");
                    outputVec.push_back("goblin-grunt-8.png");
                    outputVec.push_back("goblin-grunt-9.png");
                    outputVec.push_back("goblin-grunt-10.png");
                    outputVec.push_back("goblin-grunt-12.png");
                    return;
                }
                else
                {
                    for(std::size_t i(1); i<=13; ++i)
                    {
                        std::ostringstream ss;
                        ss << "goblin-grunt-" << i << ".png";
                        outputVec.push_back(ss.str());
                    }
                    return;
                }
            }

            if (SEX == sex::Female)
            {
                outputVec.push_back("goblin-1.png");
                outputVec.push_back("goblin-2.png");
                outputVec.push_back("goblin-female-1.png");
                outputVec.push_back("goblin-female-2.png");
                outputVec.push_back("goblin-female-3.png");
                outputVec.push_back("goblin-female-4.png");
                return;
            }
            else
            {
                 outputVec.push_back("goblin-1.png");
                 outputVec.push_back("goblin-2.png");
                 outputVec.push_back("goblin-3.png");
                 outputVec.push_back("goblin-male-1.png");
                 outputVec.push_back("goblin-male-2.png");
                 outputVec.push_back("goblin-male-3.png");
                 outputVec.push_back("goblin-male-4.png");
                 outputVec.push_back( "goblin-male-5.png");
                 return;
            }
        }

        if (RACE == race::Dragon)
        {
            if (ROLE == role::Firebrand)
            {
                switch (DRAGON_CLASS)
                {
                    case game::creature::dragon_class::Hatchling: { outputVec.push_back("dragon-fb-hatchling.png"); return; }
                    case game::creature::dragon_class::Whelp:     { outputVec.push_back("dragon-fb-whelp.png"); return;}
                    case game::creature::dragon_class::Fledgling: { outputVec.push_back("dragon-fb-fledgling.png"); return; }
                    case game::creature::dragon_class::Juvenile:  { outputVec.push_back("dragon-fb-juvenile.png"); return; }
                    case game::creature::dragon_class::Adult:     { outputVec.push_back("dragon-fb-adult.png"); return; }
                    case game::creature::dragon_class::Wyrm:      { outputVec.push_back("dragon-fb-wyrm.png"); return; }
                    case game::creature::dragon_class::Skycaster: { outputVec.push_back("dragon-fb-skycaster.png"); return; }
                    case game::creature::dragon_class::Elder:     { outputVec.push_back("dragon-fb-elder.png"); return; }
                    case game::creature::dragon_class::Count:
                    default: { break; }
                }
            }

            if (ROLE == role::Sylavin)
            {
                switch (DRAGON_CLASS)
                {
                    case game::creature::dragon_class::Hatchling: { outputVec.push_back("dragon-syl-hatchling.png"); return; }
                    case game::creature::dragon_class::Whelp:     { outputVec.push_back("dragon-syl-whelp.png"); return; }
                    case game::creature::dragon_class::Fledgling: { outputVec.push_back("dragon-syl-fledgling.png"); return; }
                    case game::creature::dragon_class::Juvenile:  { outputVec.push_back("dragon-syl-juvenile.png"); return; }
                    case game::creature::dragon_class::Adult:     { outputVec.push_back("dragon-syl-adult.png"); return; }
                    case game::creature::dragon_class::Wyrm:      { outputVec.push_back("dragon-syl-wyrm.png"); return; }
                    case game::creature::dragon_class::Skycaster: { outputVec.push_back("dragon-syl-skycaster.png"); return; }
                    case game::creature::dragon_class::Elder:     { outputVec.push_back("dragon-syl-elder.png"); return; }
                    case game::creature::dragon_class::Count:
                    default: { break; }
                }
            }
        }

        if (RACE == race::Gnome)
        {
            if (ROLE == role::Bard)
            {
                if (SEX == sex::Female)
                {
                    outputVec.push_back("bard-gnome-female-1.png");
                    outputVec.push_back("bard-female-1.png");
                }

                outputVec.push_back("bard-gnome-1.png");
                outputVec.push_back("gnome-female-1.png");
                outputVec.push_back("gnome-2.png");
                outputVec.push_back("gnome-3.png");
                outputVec.push_back("gnome-4.png");
                outputVec.push_back("gnome-7.png");

                if (SEX == sex::Male)
                {
                    outputVec.push_back("gnome-male-1.png");
                    outputVec.push_back("gnome-male-2.png");
                }

                return;
            }

            if (ROLE == role::Cleric)
            {
                outputVec.push_back("gnome-4.png");
                outputVec.push_back("gnome-8.png");
                return;
            }

            if (ROLE == role::Sorcerer)
            {
                outputVec.push_back("gnome-5.png");
                outputVec.push_back("gnome-8.png");
                return;
            }

            if (ROLE == role::Knight)
            {
                outputVec.push_back("knight-gnome-1.png");
                outputVec.push_back("knight-gnome-2.png");

                if (SEX == sex::Male)
                {
                    outputVec.push_back("knight-gnome-male-1.png");
                    outputVec.push_back("gnome-male-1.png");
                    outputVec.push_back("gnome-male-2.png");
                }

                return;
            }

            if (ROLE == role::Beastmaster)
            {
                outputVec.push_back("gnome-2.png");
                outputVec.push_back("gnome-3.png");
                outputVec.push_back("gnome-4.png");
                outputVec.push_back("gnome-6.png");
                outputVec.push_back("gnome-7.png");

                if (SEX == sex::Female)
                {
                    outputVec.push_back("gnome-female-1.png");
                }
                else if (SEX == sex::Male)
                {
                    outputVec.push_back("gnome-male-1.png");
                    outputVec.push_back("gnome-male-2.png");
                }

                return;
            }

            if (ROLE == role::Archer)
            {
                outputVec.push_back("archer-gnome-1.png");
                outputVec.push_back("gnome-2.png");
                outputVec.push_back("gnome-female-1.png");
                return;
            }

            if (ROLE == role::Thief)
            {
                outputVec.push_back("gnome-1.png");
                outputVec.push_back("gnome-2.png");
                outputVec.push_back("gnome-3.png");
                outputVec.push_back("gnome-6.png");
                outputVec.push_back("gnome-7.png");

                if (SEX == sex::Female)
                {
                    outputVec.push_back("gnome-female-1.png");
                }

                return;
            }

            outputVec.push_back("gnome-1.png");
            return;
        }

        if (RACE == race::Human)
        {
            if (ROLE == role::Knight)
            {
                outputVec.push_back("knight-human-1.png");
                outputVec.push_back("knight-human-2.png");

                if (SEX == sex::Male)
                {
                    outputVec.push_back("knight-human-male-1.png");
                    outputVec.push_back("knight-human-male-2.png");
                }

                return;
            }

            if (ROLE == role::Archer)
            {
                outputVec.push_back("archer-1.png");
                outputVec.push_back("archer-2.png");

                if (SEX == sex::Female)
                {
                    outputVec.push_back("archer-female-1.png");
                    outputVec.push_back("archer-female-2.png");
                    outputVec.push_back("archer-female-3.png");
                    outputVec.push_back("archer-female-4.png");
                    outputVec.push_back("archer-female-5.png");
                }
                else
                {
                    outputVec.push_back("archer-male-1.png");
                    outputVec.push_back("archer-male-2.png");
                    outputVec.push_back("archer-male-3.png");
                    outputVec.push_back("archer-male-4.png");
                }

                return;
            }

            if (ROLE == role::Bard)
            {
                if (SEX == sex::Female)
                {
                    outputVec.push_back("bard-human-female-1.png");
                    outputVec.push_back("bard-human-female-2.png");
                    outputVec.push_back("bard-human-female-3.png");
                    outputVec.push_back("bard-female-1.png");
                }
                else
                {
                    outputVec.push_back("bard-human-male-1.png");
                    outputVec.push_back("bard-human-male-2.png");
                    outputVec.push_back("bard-human-male-3.png");
                    outputVec.push_back("bard-human-male-4.png");
                }

                outputVec.push_back("bard-1.png");
                outputVec.push_back("bard-2.png");
                outputVec.push_back("bard-human-1.png");
                outputVec.push_back("bard-human-2.png");

                return;
            }

            if (ROLE == role::Beastmaster)
            {
                if (SEX == sex::Female)
                {
                    outputVec.push_back("beastmaster-human-female-1.png");
                }
                else
                {
                    outputVec.push_back("beastmaster-human-male-1.png");
                    outputVec.push_back("beastmaster-human-male-2.png");
                    outputVec.push_back("beastmaster-human-male-3.png");
                }

                return;
            }

            if (ROLE == role::Cleric)
            {
                outputVec.push_back("cleric-human-1.png");
                outputVec.push_back("cleric-human-2.png");

                if (SEX == sex::Female)
                {
                    outputVec.push_back("cleric-human-female-1.png");
                }
                else
                {
                    outputVec.push_back("cleric-human-male-1.png");
                    outputVec.push_back("cleric-human-male-2.png");
                    outputVec.push_back("cleric-human-male-3.png");
                }

                return;
            }

            if (ROLE == role::Knight)
            {
                outputVec.push_back("knight-human-1.png");
                outputVec.push_back("knight-human-2.png");

                if (SEX == sex::Male)
                {
                    outputVec.push_back("knight-human-male-1.png");
                    outputVec.push_back("knight-human-male-2.png");
                }

                return;
            }

            if (ROLE == role::Thief)
            {
                outputVec.push_back("thief-human-1.png");
                outputVec.push_back("thief-human-2.png");
                outputVec.push_back("thief-human-3.png");
                outputVec.push_back("thief-human-4.png");
                outputVec.push_back("thief-human-5.png");

                if (SEX == sex::Female)
                {
                    outputVec.push_back("thief-human-female-1.png");
                    outputVec.push_back("thief-human-female-2.png");
                }
                else
                {
                    outputVec.push_back("thief-human-male-1.png");
                    outputVec.push_back("thief-human-male-2.png");
                }

                return;
            }

            if (ROLE == role::Sorcerer)
            {
                if (SEX == sex::Female)
                {
                    outputVec.push_back("sorcerer-human-female-1.png");
                    outputVec.push_back("sorcerer-human-female-2.png");
                    outputVec.push_back("sorcerer-human-female-2b.png");
                    outputVec.push_back("sorcerer-human-female-3.png");
                    outputVec.push_back("sorcerer-human-female-4.png");
                    outputVec.push_back("sorcerer-human-female-5.png");
                    outputVec.push_back("sorcerer-human-female-6.png");
                }
                else
                {
                    outputVec.push_back("sorcerer-human-male-1.png");
                    outputVec.push_back("sorcerer-human-male-2.png");
                    outputVec.push_back("sorcerer-human-male-2b.png");
                    outputVec.push_back("sorcerer-human-male-3.png");
                    outputVec.push_back("sorcerer-human-male-4.png");
                }

                return;
            }

            if (ROLE == role::Ranger)
            {
                if (SEX == sex::Female)
                {
                    outputVec.push_back("ranger-human-female-1.png");
                    outputVec.push_back("ranger-human-female-2.png");
                    outputVec.push_back("ranger-human-female-3.png");
                    return;
                }
                else
                {
                    outputVec.push_back("ranger-human-male-1.png");
                    outputVec.push_back("ranger-human-male-2.png");
                    outputVec.push_back("ranger-human-male-3.png");
                    outputVec.push_back("ranger-human-male-4.png");
                    outputVec.push_back("ranger-human-male-5.png");
                    outputVec.push_back("ranger-human-male-6.png");
                     return;
                }
            }

            if (ROLE == role::Trader)
            {
                if (SEX == sex::Female)
                {
                    outputVec.push_back("trader-human-1.png");
                    outputVec.push_back("trader-human-2.png");
                    outputVec.push_back("trader-human-3.png");
                    return;
                }
                else
                {
                    outputVec.push_back("trader-human-1.png");
                    outputVec.push_back("trader-human-2.png");
                    outputVec.push_back("trader-human-3.png");
                    outputVec.push_back("trader-human-4.png");
                    return;
                }
            }

            if (ROLE == role::Drunk)
            {
                outputVec.push_back("drunk-1.png");
                return;
            }

            if (ROLE == role::Brute)
            {
                outputVec.push_back("soldier-brute.png");
                return;
            }

            if (ROLE == role::Smasher)
            {
                outputVec.push_back("soldier-smasher-1.png");
                outputVec.push_back("soldier-smasher-2.png");
                return;
            }

            if (ROLE == role::Soldier)
            {
                if (SEX == sex::Female)
                {
                    outputVec.push_back("soldier-2.png");
                    outputVec.push_back("soldier-6.png");
                    outputVec.push_back("soldier-7.png");
                    outputVec.push_back("soldier-11.png");
                    outputVec.push_back("soldier-13.png");
                    outputVec.push_back("soldier-14.png");
                    outputVec.push_back("soldier-female-1.png");
                    outputVec.push_back("soldier-swordsman.png");
                    return;
                }
                else
                {
                    outputVec.push_back("soldier-1.png");
                    outputVec.push_back("soldier-2.png");
                    outputVec.push_back("soldier-3.png");
                    outputVec.push_back("soldier-4.png");
                    outputVec.push_back("soldier-5.png");
                    outputVec.push_back("soldier-6.png");
                    outputVec.push_back("soldier-7.png");
                    outputVec.push_back("soldier-8.png");
                    outputVec.push_back("soldier-9.png");
                    outputVec.push_back("soldier-10.png");
                    outputVec.push_back("soldier-11.png");
                    outputVec.push_back("soldier-12.png");
                    outputVec.push_back("soldier-axeman.png");
                    outputVec.push_back("soldier-swordsman.png");
                    return;
                }
            }

            if (ROLE == role::Blacksmith)
            {
                outputVec.push_back("human-blacksmith-male-1.png");
                outputVec.push_back("human-blacksmith-male-2.png");
                return;
            }

            if (ROLE == role::Shaman)
            {
                outputVec.push_back("human-shaman.png");
                return;
            }

            if (ROLE == role::Witch)
            {
                outputVec.push_back("witch-1.png");
                outputVec.push_back("witch-2.png");
                outputVec.push_back("witch-3.png");
                outputVec.push_back("witch-4.png");
                outputVec.push_back("witch-5.png");
                return;
            }

            if (SEX == sex::Female)
            {
                outputVec.push_back("human-1.png");
                outputVec.push_back("human-2.png");
                outputVec.push_back("human-3.png");
                outputVec.push_back("human-4.png");
                outputVec.push_back("human-5.png");
                outputVec.push_back("human-6.png");
                outputVec.push_back("human-8.png");
                outputVec.push_back("human-9.png");
                outputVec.push_back("human-10.png");
                outputVec.push_back("human-12.png");
                outputVec.push_back("human-13.png");
                outputVec.push_back("human-female-1.png");
                outputVec.push_back("human-female-2.png");
                outputVec.push_back("human-female-3.png");
                outputVec.push_back("human-female-4.png");
                outputVec.push_back("human-female-5.png");
                outputVec.push_back("human-female-6.png");
                outputVec.push_back("human-female-7.png");
                outputVec.push_back("human-female-8.png");
                outputVec.push_back("human-female-9.png");
                outputVec.push_back("human-female-10.png");
                outputVec.push_back("human-female-11.png");
                outputVec.push_back("human-female-12.png");
                outputVec.push_back("human-female-13.png");
                outputVec.push_back("human-female-14.png");
                return;
            }
            else
            {
                outputVec.push_back("human-1.png");
                outputVec.push_back("human-2.png");
                outputVec.push_back("human-3.png");
                outputVec.push_back("human-4.png");
                outputVec.push_back("human-5.png");
                outputVec.push_back("human-6.png");
                outputVec.push_back("human-7.png");
                outputVec.push_back("human-8.png");
                outputVec.push_back("human-9.png");
                outputVec.push_back("human-10.png");
                outputVec.push_back("human-11.png");
                outputVec.push_back("human-12.png");
                outputVec.push_back("human-13.png");
                outputVec.push_back("human-male-1.png");
                outputVec.push_back("human-male-2.png");
                outputVec.push_back("human-male-3.png");
                outputVec.push_back("human-male-4.png");
                outputVec.push_back("human-male-5.png");
                outputVec.push_back("human-male-6.png");
                outputVec.push_back("human-male-7.png");
                return;
            }
        }

        if (RACE == race::Pixie)
        {
            if (SEX == sex::Female)
            {
                if (ROLE == role::Beastmaster)
                    outputVec.push_back("pixie-beastmaster-female.png");
                else if (ROLE == role::Sorcerer)
                    outputVec.push_back("pixie-female-3.png");
                else if (ROLE == role::Cleric)
                    outputVec.push_back("pixie-female-5.png");
                else if (ROLE == role::Bard)
                    outputVec.push_back("pixie-female-4.png");
                else
                {
                    outputVec.push_back("pixie-1.png");
                    outputVec.push_back("pixie-2.png");
                    outputVec.push_back("pixie-female-1.png");
                    outputVec.push_back("pixie-female-2.png");
                    outputVec.push_back("pixie-female-3.png");
                    outputVec.push_back("pixie-female-4.png");
                    outputVec.push_back("pixie-female-5.png");
                }
            }
            else
            {
                if (ROLE == role::Beastmaster)
                    outputVec.push_back("pixie-beastmaster-male.png");
                else if (ROLE == role::Sorcerer)
                    outputVec.push_back("pixie-1.png");
                else
                {
                    outputVec.push_back("pixie-1.png");
                    outputVec.push_back("pixie-2.png");
                    outputVec.push_back("pixie-male-1.png");
                }
            }

            return;
        }

        if (RACE == race::Wolfen)
        {
            if (WOLFEN_CLASS == wolfen_class::Count)
            {
                if (ROLE == role::TwoHeaded)
                {
                    outputVec.push_back("wolfen-twoheaded.png");
                    return;
                }

                outputVec.push_back("wolfen-1.png");
                outputVec.push_back("wolfen-2.png");
                outputVec.push_back("wolfen-3.png");
                return;
            }
            else
            {
                switch (WOLFEN_CLASS)
                {
                    case game::creature::wolfen_class::Pup:       { outputVec.push_back("wolfen-pup.png"); return; }
                    case game::creature::wolfen_class::Juvenile:  { outputVec.push_back("wolfen-juvenile.png"); return; }
                    case game::creature::wolfen_class::Adult:     { outputVec.push_back("wolfen-adult.png"); return; }
                    case game::creature::wolfen_class::Noble:     { outputVec.push_back("wolfen-noble.png"); return; }
                    case game::creature::wolfen_class::Highborn:  { outputVec.push_back("wolfen-highborn.png"); return; }
                    case game::creature::wolfen_class::Elder:     { outputVec.push_back("wolfen-elder.png"); return; }
                    case game::creature::wolfen_class::Count:
                    default: { break; }
                }
            }
        }

        if (RACE == race::Halfling)
        {
            if (ROLE == role::Shaman) { outputVec.push_back("halfling-shaman.png"); return; }
            if (ROLE == role::Chieftain) { outputVec.push_back("halfling-chieftain.png"); return; }
            if (ROLE == role::Captain) { outputVec.push_back("halfling-captain.png"); return; }

            outputVec.push_back("halfling-1.png");
            return;
        }

        if (RACE == race::Lion)
        {
            outputVec.push_back("lion.png");
            return;
        }

        if (RACE == race::LionBoar)
        {
            outputVec.push_back("lion-boar.png");
            return;
        }

        if (RACE == race::Naga)
        {
            outputVec.push_back("naga.png");
            return;
        }

        if (RACE == race::Pug)
        {
            if (ROLE == role::Archer) { outputVec.push_back("pug-archer.png"); return; }
            if (ROLE == role::Cleric) { outputVec.push_back("pug-cleric.png"); return; }

            outputVec.push_back("pug-grunt.png");
            return;
        }

        if (RACE == race::Ramonaut)
        {
            outputVec.push_back("ramonaut.png");
            return;
        }

        if (RACE == race::Serpent)
        {
            if (ROLE == role::Water) { outputVec.push_back("serpent-water.png"); return; }

            outputVec.push_back("serpent-desert.png");
            return;
        }

        if (RACE == race::Shade)
        {
            outputVec.push_back("shade-1.png");
            return;
        }

        if (RACE == race::Cobra)
        {
            outputVec.push_back("cobra-1.png");
            outputVec.push_back("cobra-2.png");
            return;
        }

        if (RACE == race::Werebat)
        {
            for (std::size_t i(1); i <= 5; ++i)
            {
                std::ostringstream ss;
                ss << "werebat-" << i << ".png";
                outputVec.push_back(ss.str());
            }
            return;
        }

        if (RACE == race::Wereboar)
        {
            outputVec.push_back("wereboar.png");
            return;
        }

        if (RACE == race::Werecat)
        {
            outputVec.push_back("werecat.png");
            return;
        }

        if (RACE == race::Werewolf)
        {
            outputVec.push_back("werewolf.png");
            return;
        }

        if (RACE == race::Wyvern)
        {
            if (ROLE == role::Whelp)
            {
                outputVec.push_back("wyvern-whelp.png");
                return;
            }

            outputVec.push_back("wyvern-1.png");
            outputVec.push_back("wyvern-2.png");
            return;
        }

        std::ostringstream ss;
        ss  << "CreatureImageManager::GetCharacterFilenames(race=" << game::creature::race::ToString(RACE)
            << ", role=" << game::creature::role::ToString(ROLE)
            << ", sex=" << game::creature::sex::ToString(SEX)
            << ", wolfen_class=" << WOLFEN_CLASS
            << ", dragon_class=" << DRAGON_CLASS
            << ") -No filenames found for that creature information.";
        throw std::runtime_error(ss.str());
    }

}
}
