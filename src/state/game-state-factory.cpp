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
// game-state-factory.cpp
//
#include "game-state-factory.hpp"

// This is the one and only place where this is included,
// and it needs to stay that way.  zTn 2017-3-30
#include "player/party-serialize-includes.hpp"

#include "creature/creature.hpp"
#include "game/game.hpp"
#include "log/log-macros.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/boost-string-includes.hpp"
#include "player/party.hpp"
#include "state/game-state.hpp"
#include "state/world-factory.hpp"
#include "state/world.hpp"

#include <boost/filesystem.hpp>

#include <vector>

namespace heroespath
{
namespace state
{

    std::unique_ptr<GameStateFactory> GameStateFactory::instanceUPtr_;
    //
    const std::string GameStateFactory::SAVED_HEROESPATH_DIR_NAME_("saved_games");
    const std::string GameStateFactory::SAVED_HEROESPATH_FILE_NAME_("game");
    const std::string GameStateFactory::SAVED_HEROESPATH_FILE_EXT_(".dat");
    //
    const std::string GameStateFactory::UNPLAYED_CHAR_DIR_NAME_("unplayed_characters");
    const std::string GameStateFactory::UNPLAYED_CHAR_FILE_EXT_(".dat");

    GameStateFactory::GameStateFactory()
    {
        M_HP_LOG_DBG("Singleton Construction: GameStateFactory");
    }

    GameStateFactory::~GameStateFactory()
    {
        M_HP_LOG_DBG("Singleton Destruction: GameStateFactory");
    }

    misc::NotNull<GameStateFactory *> GameStateFactory::Instance()
    {
        if (!instanceUPtr_)
        {
            M_HP_LOG_ERR("Singleton Instance() before Acquire(): GameStateFactory");
            Acquire();
        }

        return instanceUPtr_.get();
    }

    void GameStateFactory::Acquire()
    {
        if (!instanceUPtr_)
        {
            instanceUPtr_ = std::make_unique<GameStateFactory>();
        }
        else
        {
            M_HP_LOG_ERR("Singleton Acquire() after Construction: GameStateFactory");
        }
    }

    void GameStateFactory::Release()
    {
        M_ASSERT_OR_LOGANDTHROW_SS(
            (instanceUPtr_), "GameStateFactory::Release() found instanceUPtr that was null.");

        instanceUPtr_.reset();
    }

    void GameStateFactory::NewGame(player::PartyUPtr_t PARTY_UPTR) const
    {
        SaveGame(game::Game::Instance()->MakeNewGame(std::move(PARTY_UPTR)));
    }

    const GameStatePVec_t GameStateFactory::LoadAllGames() const
    {
        namespace bfs = boost::filesystem;

        const bfs::path DIR_OBJ(
            bfs::system_complete(bfs::current_path() / bfs::path(SAVED_HEROESPATH_DIR_NAME_)));

        GameStatePVec_t gameStatePVec;

        if (false == bfs::exists(DIR_OBJ))
        {
            return gameStatePVec;
        }

        if (false == bfs::is_directory(DIR_OBJ))
        {
            return gameStatePVec;
        }

        // create a vector of paths to saved games
        std::vector<bfs::path> pathVec;
        bfs::directory_iterator end_itr; // default construction yields past-the-end
        for (bfs::directory_iterator itr(DIR_OBJ); itr != end_itr; ++itr)
        {
            if ((boost::algorithm::starts_with(
                    itr->path().leaf().string(), SAVED_HEROESPATH_FILE_NAME_))
                && (boost::algorithm::ends_with(
                       itr->path().leaf().string(), SAVED_HEROESPATH_FILE_EXT_))
                && (bfs::is_regular_file(*itr)))
            {
                pathVec.emplace_back(DIR_OBJ / itr->path().leaf());
            }
        }

        // try and load each game file
        for (auto const & NEXT_PATH_OBJ : pathVec)
        {
            GameStatePtr_t nextGameStatePtr{ new GameState() };

            try
            {
                std::ifstream ifs(NEXT_PATH_OBJ.string());
                boost::archive::text_iarchive ia(ifs);
                ia >> *nextGameStatePtr;
                nextGameStatePtr->AfterDeserialize();
                gameStatePVec.emplace_back(nextGameStatePtr);
            }
            catch (const std::exception & E)
            {
                M_HP_LOG(
                    "state::GameStateFactory::LoadAll() while trying to "
                    << "de-serialize/load saved game file \"" << NEXT_PATH_OBJ.string()
                    << "\", threw std::exception(\"" << E.what() << "\")");
            }
            catch (...)
            {
                M_HP_LOG(
                    "state::GameStateFactory::LoadAll() while trying to "
                    << "de-serialize/load saved game file \"" << NEXT_PATH_OBJ.string()
                    << "\", threw UNKNOWN exception.");
            }
        }

        return gameStatePVec;
    }

    void GameStateFactory::SaveGame(const GameStatePtr_t GAMESTATE_PTR) const
    {
        Save(
            state::GameStatePtrOpt_t(GAMESTATE_PTR),
            boost::none,
            SAVED_HEROESPATH_DIR_NAME_,
            SAVED_HEROESPATH_FILE_NAME_,
            SAVED_HEROESPATH_FILE_EXT_);
    }

    const creature::CreaturePVec_t GameStateFactory::LoadAllUnplayedCharacters() const
    {
        namespace bfs = boost::filesystem;

        // configure the path
        const bfs::path DIR_OBJ(
            bfs::system_complete(bfs::current_path() / bfs::path(UNPLAYED_CHAR_DIR_NAME_)));

        creature::CreaturePVec_t characterPVec;

        // check for early exit cases
        if (false == bfs::exists(DIR_OBJ))
        {
            return characterPVec;
        }

        if (false == bfs::is_directory(DIR_OBJ))
        {
            return characterPVec;
        }

        // create a vector of paths to saved characters
        std::vector<bfs::path> pathVec;
        bfs::directory_iterator end_itr; // default construction yields past-the-end
        for (bfs::directory_iterator itr(DIR_OBJ); itr != end_itr; ++itr)
        {
            if ((boost::algorithm::ends_with(itr->path().leaf().string(), UNPLAYED_CHAR_FILE_EXT_))
                && (bfs::is_regular_file(*itr)))
            {
                pathVec.emplace_back(DIR_OBJ / itr->path().leaf());
            }
        }

        // try and load each character file
        const std::size_t NUM_FILES(pathVec.size());
        for (std::size_t i(0); i < NUM_FILES; ++i)
        {
            auto nextCharacterPtr{ new creature::Creature() };

            try
            {
                std::ifstream ifs(pathVec[i].string());
                boost::archive::text_iarchive ia(ifs);
                ia >> *nextCharacterPtr;
                nextCharacterPtr->AfterDeserialize();
                characterPVec.emplace_back(nextCharacterPtr);
            }
            catch (const std::exception & E)
            {
                M_HP_LOG(
                    "state::GameStateFactory::LoadAllUnplayedCharacters(), while trying to "
                    << "de-serialize/load saved game file \"" << pathVec[i].string()
                    << "\", threw std::exception(\"" << E.what() << "\")");
            }
            catch (...)
            {
                M_HP_LOG(
                    "state::GameStateFactory::LoadAllUnplayedCharacters(), while trying to "
                    << "de-serialize/load saved game file \"" << pathVec[i].string()
                    << "\", threw UNKNOWN exception.");
            }
        }

        return characterPVec;
    }

    void GameStateFactory::SaveCharacter(const creature::CreaturePtr_t CHARACTER_PTR) const
    {
        Save(
            boost::none,
            CHARACTER_PTR,
            UNPLAYED_CHAR_DIR_NAME_,
            CHARACTER_PTR->Name(),
            UNPLAYED_CHAR_FILE_EXT_);
    }

    bool GameStateFactory::DeleteUnplayedCharacterFile(
        const creature::CreaturePtr_t CHAR_TO_DELETE_PTR) const
    {
        namespace bfs = boost::filesystem;

        // configure the path
        const bfs::path DIR_OBJ(
            bfs::system_complete(bfs::current_path() / bfs::path(UNPLAYED_CHAR_DIR_NAME_)));

        // check for early exit cases
        if (false == bfs::exists(DIR_OBJ))
        {
            return false;
        }

        if (false == bfs::is_directory(DIR_OBJ))
        {
            return false;
        }

        // create a vector of paths to saved characters
        std::vector<bfs::path> pathVec;
        bfs::directory_iterator end_itr; // default construction yields past-the-end
        for (bfs::directory_iterator itr(DIR_OBJ); itr != end_itr; ++itr)
        {
            if ((boost::algorithm::ends_with(itr->path().leaf().string(), UNPLAYED_CHAR_FILE_EXT_))
                && (bfs::is_regular_file(*itr)))
            {
                pathVec.emplace_back(DIR_OBJ / itr->path().leaf());
            }
        }

        // loop over all paths to saved games and load then compare before deleting
        for (auto const & NEXT_PATH : pathVec)
        {
            try
            {
                auto creatureUPtr{ std::make_unique<creature::Creature>() };

                {
                    std::ifstream ifs(NEXT_PATH.string());
                    boost::archive::text_iarchive ia(ifs);
                    ia >> *creatureUPtr;
                    creatureUPtr->AfterDeserialize();
                }

                if ((*creatureUPtr) == (*CHAR_TO_DELETE_PTR))
                {
                    boost::system::error_code errorCode;
                    if (bfs::remove(NEXT_PATH, errorCode))
                    {
                        return true;
                    }
                    else
                    {
                        M_HP_LOG(
                            "state::GameStateFactory::DeleteUnplayedCharacterFile("
                            << CHAR_TO_DELETE_PTR->ToString() << "), while trying to delete "
                            << NEXT_PATH.string() << ", failed with error code=" << errorCode);

                        return false;
                    }
                }
            }
            catch (const std::exception & E)
            {
                M_HP_LOG(
                    "state::GameStateFactory::DeleteUnplayedCharacterFile("
                    << CHAR_TO_DELETE_PTR->ToString()
                    << "), while trying to de-serialize/load saved game file \""
                    << NEXT_PATH.string() << "\", threw std::exception(\"" << E.what() << "\")");
            }
            catch (...)
            {
                M_HP_LOG(
                    "state::GameStateFactory::DeleteUnplayedCharacterFile("
                    << CHAR_TO_DELETE_PTR->ToString()
                    << "), while trying to de-serialize/load saved game file \""
                    << NEXT_PATH.string() << "\", threw UNKNOWN exception.");
            }
        }

        return false;
    }

    void GameStateFactory::Save(
        const GameStatePtrOpt_t GAMESTATE_PTR_OPT,
        const creature::CreaturePtrOpt_t CHARACTER_PTR_OPT,
        const std::string & DIR_STR,
        const std::string & FILE_STR,
        const std::string & EXT_STR) const
    {
        namespace bfs = boost::filesystem;

        // establish the path
        const bfs::path DIR_OBJ(bfs::system_complete(bfs::current_path() / bfs::path(DIR_STR)));

        // create directory if missing
        if (false == bfs::exists(DIR_OBJ))
        {
            boost::system::error_code ec;

            M_ASSERT_OR_LOGANDTHROW_SS(
                (bfs::create_directory(DIR_OBJ, ec)),
                "GameStateFactory::Save() was unable to create the save game directory \""
                    << DIR_OBJ.string() << "\".  Error code=" << ec);
        }

        // find the next available save game filename
        std::ostringstream ss;
        bfs::path nextFilePathObj;
        for (std::size_t i(0);; ++i)
        {
            ss.str("");
            ss << FILE_STR << i << EXT_STR;

            nextFilePathObj = (DIR_OBJ / bfs::path(ss.str()));

            if (false == bfs::exists(nextFilePathObj))
            {
                break;
            }

            M_ASSERT_OR_LOGANDTHROW_SS(
                (i < 10000),
                "Save() failed to find an available save character filename after 10,000"
                    << " tries.  Something is very wrong...");
        }

        // set the date and time of last save
        if (GAMESTATE_PTR_OPT)
        {
            GAMESTATE_PTR_OPT->Obj().DateTimeOfLastSaveSet(sfml_util::DateTime::CurrentDateTime());
        }

        try
        {
            std::ofstream ofs(nextFilePathObj.string());
            boost::archive::text_oarchive oa(ofs);

            // save either, not both
            if (GAMESTATE_PTR_OPT)
            {
                oa << *GAMESTATE_PTR_OPT.value();
            }
            else
            {
                oa << CHARACTER_PTR_OPT->Obj();
            }
        }
        catch (const std::exception & E)
        {
            ss.str("");
            ss << "state::GameStateFactory::Save() while trying to serialize/save"
               << " game file \"" << nextFilePathObj.string() << "\", threw std::exception(\""
               << E.what() << "\".";

            M_HP_LOG(ss.str());
            throw std::runtime_error(ss.str());
        }
        catch (...)
        {
            ss.str("");
            ss << "state::GameStateFactory::Save() while trying to serialize/save"
               << " game file \"" << nextFilePathObj.string() << "\", threw an UNKNOWN exception.";

            M_HP_LOG(ss.str());
            throw std::runtime_error(ss.str());
        }
    }

} // namespace state
} // namespace heroespath
