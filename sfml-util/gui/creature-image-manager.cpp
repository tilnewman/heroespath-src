//
// creature-image-manager.cpp
//
#include "creature-image-manager.hpp"

#include "sfml-util/sfml-util.hpp"
#include "sfml-util/loaders.hpp"
#include "utilz/random.hpp"

#include "game/log-macros.hpp"
#include "utilz/assertlogandthrow.hpp"
#include "game/loop-manager.hpp"

#include <boost/filesystem.hpp>

#include <exception>
#include <sstream>


namespace sfml_util
{
namespace gui
{

    std::string                CreatureImageManager::imagesDirectoryPath_;
    CreatureImageManagerSPtr_t CreatureImageManager::instanceSPtr_;


    CreatureImageManager::CreatureImageManager()
    {}


    CreatureImageManager::~CreatureImageManager()
    {}


    CreatureImageManagerSPtr_t CreatureImageManager::Instance()
    {
        if (nullptr == instanceSPtr_.get())
            instanceSPtr_.reset( new CreatureImageManager );

        return instanceSPtr_;
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
        static auto i         { 0 };

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
                    auto const SEX_ENUM{ static_cast<game::creature::sex::Enum>(sexIndex) };
                    auto const SEX_STR { game::creature::sex::ToString(SEX_ENUM) };

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

                            if (i < static_cast<int>(filenamesVec.size()))
                            {
                                auto const TEXTURE_SPTR{ cimSPtr->GetImage(filenamesVec.at(i)) };
                                M_ASSERT_OR_LOGANDTHROW_SS((TEXTURE_SPTR.get() != nullptr), "sfml_util::gui::CreatureImageManager() (wolfen_classes) race=" << RACE_STR << ", role=" << ROLE_STR << ", sex=" << SEX_STR << ", wolfen_class=" << CLASS_STR << ", GetImage(\"" << filenamesVec.at(i) << "\") returned a nullptr texture.");

                                game::LoopManager::Instance()->TestingImageSet(TEXTURE_SPTR);

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

                                if (i < static_cast<int>(filenamesVec.size()))
                                {
                                    auto const TEXTURE_SPTR{ cimSPtr->GetImage(filenamesVec.at(i)) };
                                    M_ASSERT_OR_LOGANDTHROW_SS((TEXTURE_SPTR.get() != nullptr), "sfml_util::gui::CreatureImageManager() (dragon_classes) race=" << RACE_STR << ", role=" << ROLE_STR << ", sex=" << SEX_STR << ", dragon_class=" << CLASS_STR << ", GetImage(\"" << filenamesVec.at(i) << "\") returned a nullptr texture.");

                                    game::LoopManager::Instance()->TestingImageSet(TEXTURE_SPTR);

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

                        if (i < static_cast<int>(filenamesVec.size()))
                        {
                            auto const TEXTURE_SPTR{ cimSPtr->GetImage(filenamesVec.at(i)) };
                            M_ASSERT_OR_LOGANDTHROW_SS((TEXTURE_SPTR.get() != nullptr), "sfml_util::gui::CreatureImageManager() (dragon_classes) race=" << RACE_STR << ", role=" << ROLE_STR << ", sex=" << SEX_STR << ", GetImage(\"" << filenamesVec.at(i) << "\") returned a nullptr texture.");

                            game::LoopManager::Instance()->TestingImageSet(TEXTURE_SPTR);

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


    const TextureSPtr_t CreatureImageManager::GetImage(const std::string & FILENAME,
                                                       const bool          WILL_FACE_RIGHT) const
    {
        return LoadImage(FILENAME, WILL_FACE_RIGHT);
    }


    const TextureSPtr_t CreatureImageManager::GetImage(const game::creature::race::Enum         RACE,
                                                       const game::creature::role::Enum         ROLE,
                                                       const game::creature::sex::Enum          SEX,
                                                       const bool                                     WILL_PICK_RANDOM,
                                                       const bool                                     WILL_FACE_RIGHT,
                                                       const game::creature::wolfen_class::Enum WOLFEN_CLASS,
                                                       const game::creature::dragon_class::Enum DRAGON_CLASS) const
    {
        const std::string FILENAME(GetFilename(RACE, ROLE, SEX, WILL_PICK_RANDOM, WOLFEN_CLASS, DRAGON_CLASS));
        M_ASSERT_OR_LOGANDTHROW_SS((FILENAME.empty() == false), "sfml_util::gui::CreatureImageManager::GetImage()  GetFilename() call return no filename.  (race=" << game::creature::race::ToString(RACE) << ", role=" << game::creature::role::ToString(ROLE) << ", sex=" << game::creature::sex::ToString(SEX) << ", wolfen_class=" << WOLFEN_CLASS << ", dragon_class=" << DRAGON_CLASS << ")");
        return LoadImage(FILENAME, WILL_FACE_RIGHT);
    }


    TextureSPtr_t CreatureImageManager::LoadImage(const std::string & IMAGE_FILE_NAME,
                                                  const bool          WILL_FACE_RIGHT) const
    {
        namespace bfs = boost::filesystem;
        const bfs::path PATH_OBJ(bfs::system_complete(bfs::path(imagesDirectoryPath_) / bfs::path(IMAGE_FILE_NAME)));

        TextureSPtr_t textureSPtr;
        sfml_util::LoadImageOrTextureSPtr(textureSPtr, PATH_OBJ.string());

        if (WILL_FACE_RIGHT)
            sfml_util::FlipHoriz( * textureSPtr );

        return textureSPtr;
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
            return filenameVec[utilz::random::Int(0, filenameVec.size() - 1)];
        }
        else
            return filenameVec[0];
    }


    void CreatureImageManager::GetFilenames(std::vector<std::string> &                     outputVec,
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
                outputVec.push_back("troll-berserker.gif");
                return;
            }

            if (ROLE == role::Brute)
            {
                outputVec.push_back("troll-brute.gif");
                return;
            }

            if (ROLE == role::Chieftain)
            {
                outputVec.push_back("troll-chieftain.gif");
                return;
            }

            if (ROLE == role::Grunt)
            {
                outputVec.push_back("troll-grunt-1.gif");
                return;
            }

            if (ROLE == role::Mountain)
            {
                outputVec.push_back("troll-mountain.gif");
                return;
            }

            if (ROLE == role::Shaman)
            {
                outputVec.push_back("troll-shaman.gif");
                return;
            }

            if (ROLE == role::Smasher)
            {
                outputVec.push_back("troll-smasher.gif");
                return;
            }

            if (ROLE == role::Strangler)
            {
                outputVec.push_back("troll-strangler.gif");
                return;
            }

            if (ROLE == role::Warlord)
            {
                outputVec.push_back("troll-warlord.gif");
                return;
            }

            outputVec.push_back("troll-1.gif");
            return;
        }

        if (RACE == race::Orc)
        {
            if (ROLE == role::Berserker)
            {
                outputVec.push_back("orc-berserker.gif");
                return;
            }

            if (ROLE == role::Chieftain)
            {
                outputVec.push_back("orc-chieftain.gif");
                return;
            }

            if (ROLE == role::Elder)
            {
                outputVec.push_back("orc-elder.gif");
                return;
            }

            if (ROLE == role::FourArmed)
            {
                outputVec.push_back("orc-four-armed.gif");
                return;
            }

            if (ROLE == role::Captain)
            {
                outputVec.push_back("orc-captain.gif");
                return;
            }

            if (ROLE == role::Mountain)
            {
                outputVec.push_back("orc-mountain.gif");
                return;
            }

            if (ROLE == role::Spike)
            {
                outputVec.push_back("orc-spike.gif");
                return;
            }

            if (ROLE == role::Grunt)
            {
                for(std::size_t i(1); i<=13; ++i)
                {
                    std::ostringstream ss;
                    ss << "orc-grunt-" << i << ".gif";
                    outputVec.push_back(ss.str());
                }

                return;
            }

            if (ROLE == role::Shaman)
            {
                outputVec.push_back("orc-shaman.gif");
                return;
            }

            if (ROLE == role::Smasher)
            {
                outputVec.push_back("orc-smasher.gif");
                return;
            }

            if (ROLE == role::Warlord)
            {
                outputVec.push_back("orc-warlord.gif");
                return;
            }

            outputVec.push_back("orc-grunt-1.gif");
            return;
        }

        if (RACE == race::Newt)
        {
            if (ROLE == role::Chieftain)
            {
                outputVec.push_back("newt-chieftain.gif");
                return;
            }

            if (ROLE == role::Cleric)
            {
                outputVec.push_back("newt-cleric.gif");
                return;
            }

            if (ROLE == role::Elder)
            {
                outputVec.push_back("newt-elder.gif");
                return;
            }

            if (ROLE == role::Sorcerer)
            {
                outputVec.push_back("newt-sorcerer.gif");
                return;
            }

            if (ROLE == role::Trader)
            {
                outputVec.push_back("newt-trader.gif");
                return;
            }

            for(std::size_t i(1); i<=6; ++i)
            {
                std::ostringstream ss;
                ss << "newt-" << i << ".gif";
                outputVec.push_back(ss.str());
            }
            return;
        }

        if (RACE == race::Spider)
        {
            for (std::size_t i(1); i <= 9; ++i)
            {
                std::ostringstream ss;
                ss << "spider-giant-" << i << ".gif";
                outputVec.push_back(ss.str());
            }
            return;
        }

        if (RACE == race::Bog)
        {
            if (ROLE == role::Chieftain)
            {
                outputVec.push_back("bog-chieftain.gif");
                return;
            }

            if (ROLE == role::Smasher)
            {
                outputVec.push_back("bog-smasher.gif");
                return;
            }

            if (ROLE == role::Sorcerer)
            {
                outputVec.push_back("bog-sorcerer.gif");
                return;
            }

            if (ROLE == role::Shaman)
            {
                outputVec.push_back("bog-shaman.gif");
                return;
            }

            if (ROLE == role::Spike)
            {
                outputVec.push_back("bog-spike.gif");
                return;
            }

            if (ROLE == role::Tendrilus)
            {
                outputVec.push_back("bog-tendrilus.gif");
                return;
            }

            if (ROLE == role::Wing)
            {
                outputVec.push_back("bog-wing.gif");
                return;
            }

            if (ROLE == role::Whelp)
            {
                outputVec.push_back("bog-whelp.gif");
                return;
            }

            outputVec.push_back("bog-grunt-1.gif");
            outputVec.push_back("bog-grunt-2.gif");
            outputVec.push_back("bog-grunt-3.gif");
            return;
        }

        if (RACE == race::CaveCrawler)
        {
            outputVec.push_back("cave-crawler-1.gif");
            outputVec.push_back("cave-crawler-2.gif");
            outputVec.push_back("cave-crawler-3.gif");
            return;
        }

        if (RACE == race::Hydra)
        {
            outputVec.push_back("hydra-1.gif");
            return;
        }

        if (RACE == race::LizardWalker)
        {
            if (ROLE == role::Whelp)
            {
                outputVec.push_back("lizard-walker-whelp-1.gif");
                outputVec.push_back("lizard-walker-whelp-2.gif");
                return;
            }

            if (ROLE == role::Sorcerer)
            {
                if (SEX == sex::Female)
                {
                    outputVec.push_back("lizard-walker-sorcerer-female.gif");
                    return;
                }
                else
                {
                    outputVec.push_back("lizard-walker-sorcerer-male.gif");
                    return;
                }
            }

            if (ROLE == role::Captain)
            {
                outputVec.push_back("lizard-walker-captain.gif");
                return;
            }

            if (ROLE == role::Chieftain)
            {
                outputVec.push_back("lizard-walker-chieftain.gif");
                return;
            }

            if (ROLE == role::Spike)
            {
                outputVec.push_back("lizard-walker-spike.gif");
                return;
            }

            if (ROLE == role::Shaman)
            {
                if (SEX == sex::Female)
                {
                    outputVec.push_back("lizard-walker-shaman-female.gif");
                    return;
                }
                else
                {
                    outputVec.push_back("lizard-walker-shaman-male.gif");
                    return;
                }
            }

            for(std::size_t i(1); i<=12; ++i)
            {
                std::ostringstream ss;
                ss << "lizard-walker-" << i << ".gif";
                outputVec.push_back(ss.str());
            }
            return;
        }

        if (RACE == race::Minotaur)
        {
            if (ROLE == role::Grunt)
            {
                outputVec.push_back("minotaur-grunt-1.gif");
                outputVec.push_back("minotaur-grunt-2.gif");
                outputVec.push_back("minotaur-grunt-3.gif");
                return;
            }

            if (ROLE == role::Brute)
            {
                outputVec.push_back("minotaur-brute.gif");
                return;
            }

            if (ROLE == role::FourArmed)
            {
                outputVec.push_back("minotaur-fourarmed.gif");
                return;
            }

            if (ROLE == role::Mountain)
            {
                outputVec.push_back("minotaur-mountain.gif");
                return;
            }

            if (ROLE == role::Shaman)
            {
                outputVec.push_back("minotaur-shaman.gif");
                return;
            }

            if (ROLE == role::Warlord)
            {
                outputVec.push_back("minotaur-warlord.gif");
                return;
            }

            outputVec.push_back("minotaur-1.gif");
            outputVec.push_back("minotaur-2.gif");
            return;
        }

        if (RACE == race::Ogre)
        {
            if (ROLE == role::Berserker)
            {
                outputVec.push_back("ogre-berserker.gif");
                return;
            }

            if (ROLE == role::Brute)
            {
                outputVec.push_back("ogre-brute.gif");
                return;
            }

            if (ROLE == role::Elder)
            {
                outputVec.push_back("ogre-elder.gif");
                return;
            }

            if (ROLE == role::FourArmed)
            {
                outputVec.push_back("ogre-four-armed.gif");
                return;
            }

            if (ROLE == role::Giant)
            {
                outputVec.push_back("ogre-giant.gif");
                return;
            }

            if (ROLE == role::Mountain)
            {
                outputVec.push_back("ogre-mountain.gif");
                return;
            }

            if (ROLE == role::Warlord)
            {
                outputVec.push_back("ogre-warlord.gif");
                return;
            }

            if (ROLE == role::Captain)
            {
                outputVec.push_back("ogre-captain.gif");
                return;
            }

            if (ROLE == role::Chieftain)
            {
                outputVec.push_back("ogre-chieftain.gif");
                return;
            }

            if (ROLE == role::Shaman)
            {
                outputVec.push_back("ogre-shaman.gif");
                return;
            }

            if (ROLE == role::Whelp)
            {
                outputVec.push_back("ogre-whelp.gif");
                return;
            }

            outputVec.push_back("ogre-1.gif");
            outputVec.push_back("ogre-2.gif");
            outputVec.push_back("ogre-3.gif");
            outputVec.push_back("ogre-4.gif");
            return;
        }

        if (RACE == race::Plant)
        {
            if (ROLE == role::Smasher)
            {
                outputVec.push_back("plant-smasher.gif");
                return;
            }

            if (ROLE == role::Tendrilus)
            {
                outputVec.push_back("plant-tendrilus-1.gif");
                outputVec.push_back("plant-tendrilus-2.gif");
                return;
            }

            if (ROLE == role::Whelp)
            {
                outputVec.push_back("plant-whelp.gif");
                return;
            }

            outputVec.push_back("plant-pod-1.gif");
            outputVec.push_back("plant-pod-2.gif");
            return;
        }

        if (RACE == race::Skeleton)
        {
            if (ROLE == role::Chieftain)
            {
                outputVec.push_back("skeleton-chieftain.gif");
                return;
            }

            if (ROLE == role::FourArmed)
            {
                outputVec.push_back("skeleton-four-armed.gif");
                return;
            }

            if (ROLE == role::Mountain)
            {
                outputVec.push_back("skeleton-mountain.gif");
                return;
            }

            for (std::size_t i(1); i <= 8; ++i)
            {
                std::ostringstream ss;
                ss << "skeleton-" << i << ".gif";
                outputVec.push_back(ss.str());
            }
            return;
        }

        if (RACE == race::Werebear)
        {
            if (ROLE == role::Elder)
            {
                outputVec.push_back("werebear-elder.gif");
                return;
            }

            if (ROLE == role::Grunt)
            {
                outputVec.push_back("werebear-grunt-1.gif");
                outputVec.push_back("werebear-grunt-2.gif");
                return;
            }

            outputVec.push_back("werebear-1.gif");
            return;
        }

        if (RACE == race::Beetle)
        {
            outputVec.push_back("beetle-giant.gif");
            return;
        }

        if (RACE == race::Boar)
        {
            outputVec.push_back("boar-1.gif");
            return;
        }

        if (RACE == race::Demon)
        {
            if (ROLE == role::Skeleton)
            {
                outputVec.push_back("demon-skeleton.gif");
                return;
            }

            if (ROLE == role::Spike)
            {
                outputVec.push_back("demon-spike.gif");
                return;
            }

            if (ROLE == role::Strangler)
            {
                outputVec.push_back("demon-strangler.gif");
                return;
            }

            if (ROLE == role::Whelp)
            {
                for(std::size_t i(1); i<=4; ++i)
                {
                    std::ostringstream ss;
                    ss << "demon-whelp-" << i << ".gif";
                    outputVec.push_back(ss.str());
                }
                return;
            }

            if (ROLE == role::Wing)
            {
                outputVec.push_back("demon-wing-1.gif");
                outputVec.push_back("demon-wing-2.gif");
                return;
            }

            for (std::size_t i(1); i <= 9; ++i)
            {
                std::ostringstream ss;
                ss << "demon-" << i << ".gif";
                outputVec.push_back(ss.str());
            }
            return;
        }

        if (RACE == race::Ghoul)
        {
            if (ROLE == role::Strangler)
            {
                outputVec.push_back("ghoul-strangler.gif");
                return;
            }

            if (ROLE == role::Spike)
            {
                outputVec.push_back("ghoul-spike.gif");
                return;
            }

            outputVec.push_back("ghoul-1.gif");
            outputVec.push_back("ghoul-2.gif");
            outputVec.push_back("ghoul-3.gif");
            return;
        }

        if (RACE == race::Griffin)
        {
            if (ROLE == role::Whelp)
            {
                outputVec.push_back("griffin-whelp.gif");
                return;
            }

            outputVec.push_back("griffin-1.gif");
            return;
        }

        if (RACE == race::Giant)
        {
            if (ROLE == role::Smasher)
            {
                outputVec.push_back("giant-smasher.gif");
                return;
            }

            if (ROLE == role::Strangler)
            {
                outputVec.push_back("giant-strangler.gif");
                return;
            }

            if (ROLE == role::Warlord)
            {
                outputVec.push_back("giant-warlord.gif");
                return;
            }

            if (ROLE == role::Mountain)
            {
                outputVec.push_back("giant-mountain.gif");
                return;
            }

            if (ROLE == role::Grunt)
            {
                outputVec.push_back("giant-grunt-1.gif");
                outputVec.push_back("giant-grunt-2.gif");
                outputVec.push_back("giant-grunt-3.gif");
                return;
            }

            outputVec.push_back("giant-1.gif");
            return;
        }

        if (RACE == race::Goblin)
        {
            if (ROLE == role::Thief)
            {
                outputVec.push_back("goblin-thief.gif");
                return;
            }

            if (ROLE == role::Sorcerer)
            {
                outputVec.push_back("goblin-sorcerer.gif");
                return;
            }

            if (ROLE == role::Brute)
            {
                outputVec.push_back("goblin-brute.gif");
                return;
            }

            if (ROLE == role::Archer)
            {
                outputVec.push_back("goblin-archer.gif");
                return;
            }

            if (ROLE == role::Captain)
            {
                outputVec.push_back("goblin-captain.gif");
                return;
            }

            if (ROLE == role::Chieftain)
            {
                outputVec.push_back("goblin-chieftain.gif");
                return;
            }

            if (ROLE == role::Shaman)
            {
                outputVec.push_back("goblin-shaman.gif");
                return;
            }

            if (ROLE == role::Trader)
            {
                if (SEX == sex::Female)
                {
                    outputVec.push_back("goblin-trader-female.gif");
                    return;
                }
                else
                {
                    outputVec.push_back("goblin-trader-male.gif");
                    return;
                }
            }

            if (ROLE == role::Grunt)
            {
                if (SEX == sex::Female)
                {
                    outputVec.push_back("goblin-female-1.gif");
                    outputVec.push_back("goblin-female-2.gif");
                    outputVec.push_back("goblin-female-3.gif");
                    outputVec.push_back("goblin-female-4.gif");
                    outputVec.push_back("goblin-grunt-2.gif");
                    outputVec.push_back("goblin-grunt-4.gif");
                    outputVec.push_back("goblin-grunt-5.gif");
                    outputVec.push_back("goblin-grunt-6.gif");
                    outputVec.push_back("goblin-grunt-7.gif");
                    outputVec.push_back("goblin-grunt-8.gif");
                    outputVec.push_back("goblin-grunt-9.gif");
                    outputVec.push_back("goblin-grunt-10.gif");
                    outputVec.push_back("goblin-grunt-12.gif");
                    return;
                }
                else
                {
                    for(std::size_t i(1); i<=13; ++i)
                    {
                        std::ostringstream ss;
                        ss << "goblin-grunt-" << i << ".gif";
                        outputVec.push_back(ss.str());
                    }
                    return;
                }
            }

            if (SEX == sex::Female)
            {
                outputVec.push_back("goblin-1.gif");
                outputVec.push_back("goblin-2.gif");
                outputVec.push_back("goblin-female-1.gif");
                outputVec.push_back("goblin-female-2.gif");
                outputVec.push_back("goblin-female-3.gif");
                outputVec.push_back("goblin-female-4.gif");
                return;
            }
            else
            {
                 outputVec.push_back("goblin-1.gif");
                 outputVec.push_back("goblin-2.gif");
                 outputVec.push_back("goblin-3.gif");
                 outputVec.push_back("goblin-male-1.gif");
                 outputVec.push_back("goblin-male-2.gif");
                 outputVec.push_back("goblin-male-3.gif");
                 outputVec.push_back("goblin-male-4.gif");
                 outputVec.push_back( "goblin-male-5.gif");
                 return;
            }
        }

        if (RACE == race::Dragon)
        {
            if (ROLE == role::Firebrand)
            {
                switch (DRAGON_CLASS)
                {
                    case game::creature::dragon_class::Hatchling: { outputVec.push_back("dragon-fb-hatchling.gif"); return; }
                    case game::creature::dragon_class::Whelp:     { outputVec.push_back("dragon-fb-whelp.gif"); return;}
                    case game::creature::dragon_class::Fledgling: { outputVec.push_back("dragon-fb-fledgling.gif"); return; }
                    case game::creature::dragon_class::Juvenile:  { outputVec.push_back("dragon-fb-juvenile.gif"); return; }
                    case game::creature::dragon_class::Adult:     { outputVec.push_back("dragon-fb-adult.gif"); return; }
                    case game::creature::dragon_class::Wyrm:      { outputVec.push_back("dragon-fb-wyrm.gif"); return; }
                    case game::creature::dragon_class::Skycaster: { outputVec.push_back("dragon-fb-skycaster.gif"); return; }
                    case game::creature::dragon_class::Elder:     { outputVec.push_back("dragon-fb-elder.gif"); return; }
                    case game::creature::dragon_class::Count:
                    default: { break; }
                }
            }

            if (ROLE == role::Sylavin)
            {
                switch (DRAGON_CLASS)
                {
                    case game::creature::dragon_class::Hatchling: { outputVec.push_back("dragon-syl-hatchling.gif"); return; }
                    case game::creature::dragon_class::Whelp:     { outputVec.push_back("dragon-syl-whelp.gif"); return; }
                    case game::creature::dragon_class::Fledgling: { outputVec.push_back("dragon-syl-fledgling.gif"); return; }
                    case game::creature::dragon_class::Juvenile:  { outputVec.push_back("dragon-syl-juvenile.gif"); return; }
                    case game::creature::dragon_class::Adult:     { outputVec.push_back("dragon-syl-adult.gif"); return; }
                    case game::creature::dragon_class::Wyrm:      { outputVec.push_back("dragon-syl-wyrm.gif"); return; }
                    case game::creature::dragon_class::Skycaster: { outputVec.push_back("dragon-syl-skycaster.gif"); return; }
                    case game::creature::dragon_class::Elder:     { outputVec.push_back("dragon-syl-elder.gif"); return; }
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
                    outputVec.push_back("bard-gnome-female-1.gif");
                    outputVec.push_back("bard-female-1.gif");
                }

                outputVec.push_back("bard-gnome-1.gif");
                outputVec.push_back("gnome-female-1.gif");
                outputVec.push_back("gnome-2.gif");
                outputVec.push_back("gnome-3.gif");
                outputVec.push_back("gnome-4.gif");
                outputVec.push_back("gnome-7.gif");

                if (SEX == sex::Male)
                {
                    outputVec.push_back("gnome-male-1.gif");
                    outputVec.push_back("gnome-male-2.gif");
                }

                return;
            }

            if (ROLE == role::Cleric)
            {
                outputVec.push_back("gnome-4.gif");
                outputVec.push_back("gnome-8.gif");
                return;
            }

            if (ROLE == role::Sorcerer)
            {
                outputVec.push_back("gnome-5.gif");
                outputVec.push_back("gnome-8.gif");
                return;
            }

            if (ROLE == role::Knight)
            {
                outputVec.push_back("knight-gnome-1.gif");
                outputVec.push_back("knight-gnome-2.gif");

                if (SEX == sex::Male)
                {
                    outputVec.push_back("knight-gnome-male-1.gif");
                    outputVec.push_back("gnome-male-1.gif");
                    outputVec.push_back("gnome-male-2.gif");
                }

                return;
            }

            if (ROLE == role::Beastmaster)
            {
                outputVec.push_back("gnome-2.gif");
                outputVec.push_back("gnome-3.gif");
                outputVec.push_back("gnome-4.gif");
                outputVec.push_back("gnome-6.gif");
                outputVec.push_back("gnome-7.gif");

                if (SEX == sex::Female)
                {
                    outputVec.push_back("gnome-female-1.gif");
                }
                else if (SEX == sex::Male)
                {
                    outputVec.push_back("gnome-male-1.gif");
                    outputVec.push_back("gnome-male-2.gif");
                }

                return;
            }

            if (ROLE == role::Archer)
            {
                outputVec.push_back("archer-gnome-1.gif");
                outputVec.push_back("gnome-2.gif");
                outputVec.push_back("gnome-female-1.gif");
                return;
            }

            if (ROLE == role::Thief)
            {
                outputVec.push_back("gnome-1.gif");
                outputVec.push_back("gnome-2.gif");
                outputVec.push_back("gnome-3.gif");
                outputVec.push_back("gnome-6.gif");
                outputVec.push_back("gnome-7.gif");

                if (SEX == sex::Female)
                {
                    outputVec.push_back("gnome-female-1.gif");
                }

                return;
            }

            outputVec.push_back("gnome-1.gif");
            return;
        }

        if (RACE == race::Human)
        {
            if (ROLE == role::Knight)
            {
                outputVec.push_back("knight-human-1.gif");
                outputVec.push_back("knight-human-2.gif");

                if (SEX == sex::Male)
                {
                    outputVec.push_back("knight-human-male-1.gif");
                    outputVec.push_back("knight-human-male-2.gif");
                }

                return;
            }

            if (ROLE == role::Archer)
            {
                outputVec.push_back("archer-1.gif");
                outputVec.push_back("archer-2.gif");

                if (SEX == sex::Female)
                {
                    outputVec.push_back("archer-female-1.gif");
                    outputVec.push_back("archer-female-2.gif");
                    outputVec.push_back("archer-female-3.gif");
                    outputVec.push_back("archer-female-4.gif");
                    outputVec.push_back("archer-female-5.gif");
                }
                else
                {
                    outputVec.push_back("archer-male-1.gif");
                    outputVec.push_back("archer-male-2.gif");
                    outputVec.push_back("archer-male-3.gif");
                    outputVec.push_back("archer-male-4.gif");
                }

                return;
            }

            if (ROLE == role::Bard)
            {
                if (SEX == sex::Female)
                {
                    outputVec.push_back("bard-human-female-1.gif");
                    outputVec.push_back("bard-human-female-2.gif");
                    outputVec.push_back("bard-human-female-3.gif");
                    outputVec.push_back("bard-female-1.gif");
                }
                else
                {
                    outputVec.push_back("bard-human-male-1.gif");
                    outputVec.push_back("bard-human-male-2.gif");
                    outputVec.push_back("bard-human-male-3.gif");
                    outputVec.push_back("bard-human-male-4.gif");
                }

                outputVec.push_back("bard-1.gif");
                outputVec.push_back("bard-2.gif");
                outputVec.push_back("bard-human-1.gif");
                outputVec.push_back("bard-human-2.gif");

                return;
            }

            if (ROLE == role::Beastmaster)
            {
                if (SEX == sex::Female)
                {
                    outputVec.push_back("beastmaster-human-female-1.gif");
                }
                else
                {
                    outputVec.push_back("beastmaster-human-male-1.gif");
                    outputVec.push_back("beastmaster-human-male-2.gif");
                    outputVec.push_back("beastmaster-human-male-3.gif");
                }

                return;
            }

            if (ROLE == role::Cleric)
            {
                outputVec.push_back("cleric-human-1.gif");
                outputVec.push_back("cleric-human-2.gif");

                if (SEX == sex::Female)
                {
                    outputVec.push_back("cleric-human-female-1.gif");
                }
                else
                {
                    outputVec.push_back("cleric-human-male-1.gif");
                    outputVec.push_back("cleric-human-male-2.gif");
                    outputVec.push_back("cleric-human-male-3.gif");
                }

                return;
            }

            if (ROLE == role::Knight)
            {
                outputVec.push_back("knight-human-1.gif");
                outputVec.push_back("knight-human-2.gif");

                if (SEX == sex::Male)
                {
                    outputVec.push_back("knight-human-male-1.gif");
                    outputVec.push_back("knight-human-male-2.gif");
                }

                return;
            }

            if (ROLE == role::Thief)
            {
                outputVec.push_back("thief-human-1.gif");
                outputVec.push_back("thief-human-2.gif");
                outputVec.push_back("thief-human-3.gif");
                outputVec.push_back("thief-human-4.gif");
                outputVec.push_back("thief-human-5.gif");

                if (SEX == sex::Female)
                {
                    outputVec.push_back("thief-human-female-1.gif");
                    outputVec.push_back("thief-human-female-2.gif");
                }
                else
                {
                    outputVec.push_back("thief-human-male-1.gif");
                    outputVec.push_back("thief-human-male-2.gif");
                }

                return;
            }

            if (ROLE == role::Sorcerer)
            {
                if (SEX == sex::Female)
                {
                    outputVec.push_back("sorcerer-human-female-1.gif");
                    outputVec.push_back("sorcerer-human-female-2.gif");
                    outputVec.push_back("sorcerer-human-female-2b.gif");
                    outputVec.push_back("sorcerer-human-female-3.gif");
                    outputVec.push_back("sorcerer-human-female-4.gif");
                    outputVec.push_back("sorcerer-human-female-5.gif");
                    outputVec.push_back("sorcerer-human-female-6.gif");
                }
                else
                {
                    outputVec.push_back("sorcerer-human-male-1.gif");
                    outputVec.push_back("sorcerer-human-male-2.gif");
                    outputVec.push_back("sorcerer-human-male-2b.gif");
                    outputVec.push_back("sorcerer-human-male-3.gif");
                    outputVec.push_back("sorcerer-human-male-4.gif");
                }

                return;
            }

            if (ROLE == role::Ranger)
            {
                if (SEX == sex::Female)
                {
                    outputVec.push_back("ranger-human-female-1.gif");
                    outputVec.push_back("ranger-human-female-2.gif");
                    outputVec.push_back("ranger-human-female-3.gif");
                    return;
                }
                else
                {
                    outputVec.push_back("ranger-human-male-1.gif");
                    outputVec.push_back("ranger-human-male-2.gif");
                    outputVec.push_back("ranger-human-male-3.gif");
                    outputVec.push_back("ranger-human-male-4.gif");
                    outputVec.push_back("ranger-human-male-5.gif");
                    outputVec.push_back("ranger-human-male-6.gif");
                     return;
                }
            }

            if (ROLE == role::Trader)
            {
                if (SEX == sex::Female)
                {
                    outputVec.push_back("trader-human-1.gif");
                    outputVec.push_back("trader-human-2.gif");
                    outputVec.push_back("trader-human-3.gif");
                    return;
                }
                else
                {
                    outputVec.push_back("trader-human-1.gif");
                    outputVec.push_back("trader-human-2.gif");
                    outputVec.push_back("trader-human-3.gif");
                    outputVec.push_back("trader-human-4.gif");
                    return;
                }
            }

            if (ROLE == role::Drunk)
            {
                outputVec.push_back("drunk-1.gif");
                return;
            }

            if (ROLE == role::Brute)
            {
                outputVec.push_back("soldier-brute.gif");
                return;
            }

            if (ROLE == role::Smasher)
            {
                outputVec.push_back("soldier-smasher-1.gif");
                outputVec.push_back("soldier-smasher-2.gif");
                return;
            }

            if (ROLE == role::Soldier)
            {
                if (SEX == sex::Female)
                {
                    outputVec.push_back("soldier-2.gif");
                    outputVec.push_back("soldier-6.gif");
                    outputVec.push_back("soldier-7.gif");
                    outputVec.push_back("soldier-11.gif");
                    outputVec.push_back("soldier-13.gif");
                    outputVec.push_back("soldier-14.gif");
                    outputVec.push_back("soldier-female-1.gif");
                    outputVec.push_back("soldier-swordsman.gif");
                    return;
                }
                else
                {
                    outputVec.push_back("soldier-1.gif");
                    outputVec.push_back("soldier-2.gif");
                    outputVec.push_back("soldier-3.gif");
                    outputVec.push_back("soldier-4.gif");
                    outputVec.push_back("soldier-5.gif");
                    outputVec.push_back("soldier-6.gif");
                    outputVec.push_back("soldier-7.gif");
                    outputVec.push_back("soldier-8.gif");
                    outputVec.push_back("soldier-9.gif");
                    outputVec.push_back("soldier-10.gif");
                    outputVec.push_back("soldier-11.gif");
                    outputVec.push_back("soldier-12.gif");
                    outputVec.push_back("soldier-axeman.gif");
                    outputVec.push_back("soldier-swordsman.gif");
                    return;
                }
            }

            if (ROLE == role::Blacksmith)
            {
                outputVec.push_back("human-blacksmith-male-1.gif");
                outputVec.push_back("human-blacksmith-male-2.gif");
                return;
            }

            if (ROLE == role::Shaman)
            {
                outputVec.push_back("human-shaman.gif");
                return;
            }

            if (ROLE == role::Witch)
            {
                outputVec.push_back("witch-1.gif");
                outputVec.push_back("witch-2.gif");
                outputVec.push_back("witch-3.gif");
                outputVec.push_back("witch-4.gif");
                outputVec.push_back("witch-5.gif");
                return;
            }

            if (SEX == sex::Female)
            {
                outputVec.push_back("human-1.gif");
                outputVec.push_back("human-2.gif");
                outputVec.push_back("human-3.gif");
                outputVec.push_back("human-4.gif");
                outputVec.push_back("human-5.gif");
                outputVec.push_back("human-6.gif");
                outputVec.push_back("human-8.gif");
                outputVec.push_back("human-9.gif");
                outputVec.push_back("human-10.gif");
                outputVec.push_back("human-12.gif");
                outputVec.push_back("human-13.gif");
                outputVec.push_back("human-female-1.gif");
                outputVec.push_back("human-female-2.gif");
                outputVec.push_back("human-female-3.gif");
                outputVec.push_back("human-female-4.gif");
                outputVec.push_back("human-female-5.gif");
                outputVec.push_back("human-female-6.gif");
                outputVec.push_back("human-female-7.gif");
                outputVec.push_back("human-female-8.gif");
                outputVec.push_back("human-female-9.gif");
                outputVec.push_back("human-female-10.gif");
                outputVec.push_back("human-female-11.gif");
                outputVec.push_back("human-female-12.gif");
                outputVec.push_back("human-female-13.gif");
                outputVec.push_back("human-female-14.gif");
                return;
            }
            else
            {
                outputVec.push_back("human-1.gif");
                outputVec.push_back("human-2.gif");
                outputVec.push_back("human-3.gif");
                outputVec.push_back("human-4.gif");
                outputVec.push_back("human-5.gif");
                outputVec.push_back("human-6.gif");
                outputVec.push_back("human-7.gif");
                outputVec.push_back("human-8.gif");
                outputVec.push_back("human-9.gif");
                outputVec.push_back("human-10.gif");
                outputVec.push_back("human-11.gif");
                outputVec.push_back("human-12.gif");
                outputVec.push_back("human-13.gif");
                outputVec.push_back("human-male-1.gif");
                outputVec.push_back("human-male-2.gif");
                outputVec.push_back("human-male-3.gif");
                outputVec.push_back("human-male-4.gif");
                outputVec.push_back("human-male-5.gif");
                outputVec.push_back("human-male-6.gif");
                outputVec.push_back("human-male-7.gif");
                return;
            }
        }

        if (RACE == race::Pixie)
        {
            if (SEX == sex::Female)
            {
                if (ROLE == role::Beastmaster)
                    outputVec.push_back("pixie-beastmaster-female.gif");
                else if (ROLE == role::Sorcerer)
                    outputVec.push_back("pixie-female-3.gif");
                else if (ROLE == role::Cleric)
                    outputVec.push_back("pixie-female-5.gif");
                else if (ROLE == role::Bard)
                    outputVec.push_back("pixie-female-4.gif");
                else
                {
                    outputVec.push_back("pixie-1.gif");
                    outputVec.push_back("pixie-2.gif");
                    outputVec.push_back("pixie-female-1.gif");
                    outputVec.push_back("pixie-female-2.gif");
                    outputVec.push_back("pixie-female-3.gif");
                    outputVec.push_back("pixie-female-4.gif");
                    outputVec.push_back("pixie-female-5.gif");
                }
            }
            else
            {
                if (ROLE == role::Beastmaster)
                    outputVec.push_back("pixie-beastmaster-male.gif");
                else if (ROLE == role::Sorcerer)
                    outputVec.push_back("pixie-1.gif");
                else
                {
                    outputVec.push_back("pixie-1.gif");
                    outputVec.push_back("pixie-2.gif");
                    outputVec.push_back("pixie-male-1.gif");
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
                    outputVec.push_back("wolfen-twoheaded.gif");
                    return;
                }

                outputVec.push_back("wolfen-1.gif");
                outputVec.push_back("wolfen-2.gif");
                outputVec.push_back("wolfen-3.gif");
                return;
            }
            else
            {
                switch (WOLFEN_CLASS)
                {
                    case game::creature::wolfen_class::Pup:       { outputVec.push_back("wolfen-pup.gif"); return; }
                    case game::creature::wolfen_class::Juvenile:  { outputVec.push_back("wolfen-juvenile.gif"); return; }
                    case game::creature::wolfen_class::Adult:     { outputVec.push_back("wolfen-adult.gif"); return; }
                    case game::creature::wolfen_class::Noble:     { outputVec.push_back("wolfen-noble.gif"); return; }
                    case game::creature::wolfen_class::Highborn:  { outputVec.push_back("wolfen-highborn.gif"); return; }
                    case game::creature::wolfen_class::Elder:     { outputVec.push_back("wolfen-elder.gif"); return; }
                    case game::creature::wolfen_class::Count:
                    default: { break; }
                }
            }
        }

        if (RACE == race::Halfling)
        {
            if (ROLE == role::Shaman) { outputVec.push_back("halfling-shaman.gif"); return; }
            if (ROLE == role::Chieftain) { outputVec.push_back("halfling-chieftain.gif"); return; }
            if (ROLE == role::Captain) { outputVec.push_back("halfling-captain.gif"); return; }

            outputVec.push_back("halfling-1.gif");
            return;
        }

        if (RACE == race::Lion)
        {
            outputVec.push_back("lion.gif");
            return;
        }

        if (RACE == race::LionBoar)
        {
            outputVec.push_back("lion-boar.gif");
            return;
        }

        if (RACE == race::Naga)
        {
            outputVec.push_back("naga.gif");
            return;
        }

        if (RACE == race::Pug)
        {
            if (ROLE == role::Archer) { outputVec.push_back("pug-archer.gif"); return; }
            if (ROLE == role::Cleric) { outputVec.push_back("pug-cleric.gif"); return; }

            outputVec.push_back("pug-grunt.gif");
            return;
        }

        if (RACE == race::Ramonaut)
        {
            outputVec.push_back("ramonaut.gif");
            return;
        }

        if (RACE == race::Serpent)
        {
            if (ROLE == role::Water) { outputVec.push_back("serpent-water.gif"); return; }

            outputVec.push_back("serpent-desert.gif");
            return;
        }

        if (RACE == race::Shade)
        {
            outputVec.push_back("shade-1.gif");
            return;
        }

        if (RACE == race::Cobra)
        {
            outputVec.push_back("cobra-1.gif");
            outputVec.push_back("cobra-2.gif");
            return;
        }

        if (RACE == race::Werebat)
        {
            for (std::size_t i(1); i <= 5; ++i)
            {
                std::ostringstream ss;
                ss << "werebat-" << i << ".gif";
                outputVec.push_back(ss.str());
            }
            return;
        }

        if (RACE == race::Wereboar)
        {
            outputVec.push_back("wereboar.gif");
            return;
        }

        if (RACE == race::Werecat)
        {
            outputVec.push_back("werecat.gif");
            return;
        }

        if (RACE == race::Werewolf)
        {
            outputVec.push_back("werewolf.gif");
            return;
        }

        if (RACE == race::Wyvern)
        {
            if (ROLE == role::Whelp)
            {
                outputVec.push_back("wyvern-whelp.gif");
                return;
            }

            outputVec.push_back("wyvern-1.gif");
            outputVec.push_back("wyvern-2.gif");
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
