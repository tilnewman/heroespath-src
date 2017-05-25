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

//This is the one and only place where this is included,
//and it needs to stay that way.  zTn 2017-3-30
#include "game/player/party-serialize-includes.hpp"

#include "game/game.hpp"
#include "game/state/game-state.hpp"
#include "game/state/world-state.hpp"
#include "game/location/location.hpp"
#include "game/player/party.hpp"
#include "game/player/character.hpp"
#include "game/log-macros.hpp"

#include "misc/assertlogandthrow.hpp"
#include "misc/boost-string-includes.hpp"

#include <boost/filesystem.hpp>

#include <vector>


namespace game
{
namespace state
{

    std::unique_ptr<GameStateFactory> GameStateFactory::instanceUPtr_{ nullptr };
    //
    const std::string GameStateFactory::SAVED_GAME_DIR_NAME_("saved_games");
    const std::string GameStateFactory::SAVED_GAME_FILE_NAME_("game");
    const std::string GameStateFactory::SAVED_GAME_FILE_EXT_(".dat");
    //
    const std::string GameStateFactory::SAVED_CHAR_DIR_NAME_("unplayed_characters");
    const std::string GameStateFactory::SAVED_CHAR_FILE_EXT_(".dat");


    GameStateFactory::GameStateFactory()
    {}


    GameStateFactory * GameStateFactory::Instance()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            Acquire();
        }

        return instanceUPtr_.get();
    }


    void GameStateFactory::Acquire()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            instanceUPtr_.reset(new GameStateFactory);
        }
    }


    void GameStateFactory::Release()
    {
        M_ASSERT_OR_LOGANDTHROW_SS((instanceUPtr_.get() != nullptr), "game::GameStateFactory::Release() found instanceUPtr that was null.");
        instanceUPtr_.reset();
    }


    void GameStateFactory::NewGame(const player::PartySPtr_t & PARTY_SPTR) const
    {
        const WorldStateSPtr_t WORLDSTATE_SPTR( new WorldState );
        GameStateSPtr_t gameStateSPtr( new GameState(PARTY_SPTR, WORLDSTATE_SPTR) );
        gameStateSPtr->IsNewGameSet(true);
        gameStateSPtr->DateTimeStartedSet(sfml_util::DateTime::CurrentDateTime());

        location::LocationSPtr_t locationSPtr( new location::Location(location::map::Thornberry) );
        gameStateSPtr->LocationSet( locationSPtr );

        Game::Instance()->StateSet(gameStateSPtr);
        SaveGame(gameStateSPtr);
    }


    GameStateSSet_t GameStateFactory::LoadAllGames() const
    {
        namespace bfs = boost::filesystem;

        //configure the path
        const bfs::path   DIR_OBJ(bfs::system_complete(bfs::current_path() / bfs::path(SAVED_GAME_DIR_NAME_)));

        GameStateSSet_t gameStateSSet;

        //check for early exit cases
        if (false == bfs::exists(DIR_OBJ))
            return gameStateSSet;

        if (false == bfs::is_directory(DIR_OBJ))
            return gameStateSSet;

        //create a vector of paths to saved games
        std::vector<bfs::path> pathVec;
        bfs::directory_iterator end_itr; // default construction yields past-the-end
        for (bfs::directory_iterator itr(DIR_OBJ); itr != end_itr; ++itr)
        {
            if ((boost::algorithm::starts_with(itr->path().leaf().string(), SAVED_GAME_FILE_NAME_)) &&
                (boost::algorithm::ends_with(itr->path().leaf().string(), SAVED_GAME_FILE_EXT_)) &&
                (bfs::is_regular_file(*itr)))
            {
                pathVec.push_back(DIR_OBJ / itr->path().leaf());
            }
        }

        //try and load each game file
        for (auto const & NEXT_PATH_OBJ : pathVec)
        {
            GameStateSPtr_t nextGameStateSPtr( new GameState() );

            try
            {
                std::ifstream ifs(NEXT_PATH_OBJ.string());
                boost::archive::text_iarchive ia(ifs);
                ia >> * nextGameStateSPtr;

                gameStateSSet.insert(nextGameStateSPtr);
            }
            catch (const std::exception & E)
            {
                M_HP_LOG("game::state::GameStateFactory::LoadAll() while trying to de-serialize/load saved game file \"" << NEXT_PATH_OBJ.string() << "\", threw std::exception(\"" << E.what() << "\")");
            }
            catch (...)
            {
                M_HP_LOG("game::state::GameStateFactory::LoadAll() while trying to de-serialize/load saved game file \"" << NEXT_PATH_OBJ.string() << "\", threw UNKNOWN exception.");
            }
        }

        return gameStateSSet;
    }


    void GameStateFactory::SaveGame(const GameStateSPtr_t & GAME_SPTR) const
    {
        Save(GAME_SPTR,
             player::CharacterSPtr_t(),
             SAVED_GAME_DIR_NAME_,
             SAVED_GAME_FILE_NAME_,
             SAVED_GAME_FILE_EXT_);
    }


    player::CharacterSSet_t GameStateFactory::LoadAllCompanions() const
    {
        namespace bfs = boost::filesystem;

        //configure the path
        const bfs::path   DIR_OBJ(bfs::system_complete(bfs::current_path() / bfs::path(SAVED_CHAR_DIR_NAME_)));

        player::CharacterSSet_t characterSSet;

        //check for early exit cases
        if (false == bfs::exists(DIR_OBJ))
            return characterSSet;

        if (false == bfs::is_directory(DIR_OBJ))
            return characterSSet;

        //create a vector of paths to saved characters
        std::vector<bfs::path> pathVec;
        bfs::directory_iterator end_itr; // default construction yields past-the-end
        for (bfs::directory_iterator itr(DIR_OBJ); itr != end_itr; ++itr)
        {
            if ((boost::algorithm::ends_with(itr->path().leaf().string(), SAVED_CHAR_FILE_EXT_)) &&
                (bfs::is_regular_file(*itr)))
            {
                pathVec.push_back(DIR_OBJ / itr->path().leaf());
            }
        }

        //try and load each character file
        const std::size_t NUM_FILES(pathVec.size());
        for (std::size_t i(0); i < NUM_FILES; ++i)
        {
            player::CharacterSPtr_t nextCharacterSPtr( new player::Character() );

            try
            {
                std::ifstream ifs(pathVec[i].string());
                boost::archive::text_iarchive ia(ifs);
                ia >> * nextCharacterSPtr;

                characterSSet.insert(nextCharacterSPtr);
            }
            catch (const std::exception & E)
            {
                M_HP_LOG("game::state::GameStateFactory::LoadAllCompanions(), while trying to de-serialize/load saved game file \"" << pathVec[i].string() << "\", threw std::exception(\"" << E.what() << "\")");
            }
            catch (...)
            {
                M_HP_LOG("game::state::GameStateFactory::LoadAllCompanions(), while trying to de-serialize/load saved game file \"" << pathVec[i].string() << "\", threw UNKNOWN exception.");
            }
        }

        return characterSSet;
    }


    void GameStateFactory::SaveCharacter(const player::CharacterSPtr_t & CHARACTER_SPTR) const
    {
        Save(GameStateSPtr_t(),
             CHARACTER_SPTR,
             SAVED_CHAR_DIR_NAME_,
             CHARACTER_SPTR->Name(),
             SAVED_CHAR_FILE_EXT_);
    }


    bool GameStateFactory::DeleteCharacter(const player::CharacterSPtr_t & CHAR_TO_DELETE_SPTR) const
    {
        namespace bfs = boost::filesystem;

        //configure the path
        const bfs::path   DIR_OBJ(bfs::system_complete(bfs::current_path() / bfs::path(SAVED_CHAR_DIR_NAME_)));

        //check for early exit cases
        if (false == bfs::exists(DIR_OBJ))
            return false;

        if (false == bfs::is_directory(DIR_OBJ))
            return false;

        //create a vector of paths to saved characters
        std::vector<bfs::path> pathVec;
        bfs::directory_iterator end_itr; // default construction yields past-the-end
        for (bfs::directory_iterator itr(DIR_OBJ); itr != end_itr; ++itr)
        {
            if ((boost::algorithm::ends_with(itr->path().leaf().string(), SAVED_CHAR_FILE_EXT_)) &&
                (bfs::is_regular_file(*itr)))
            {
                pathVec.push_back(DIR_OBJ / itr->path().leaf());
            }
        }

        //loop over all paths to saved games and load then compare before deleting
        for (auto const & NEXT_PATH : pathVec)
        {
            player::CharacterSPtr_t nextCharacterSPtr( new player::Character() );

            try
            {
                {
                    std::ifstream ifs(NEXT_PATH.string());
                    boost::archive::text_iarchive ia(ifs);
                    ia >> *nextCharacterSPtr;
                }

                if ((*nextCharacterSPtr) == (*CHAR_TO_DELETE_SPTR))
                {
                    boost::system::error_code errorCode;
                    if (bfs::remove(NEXT_PATH, errorCode))
                    {
                        return true;
                    }
                    else
                    {
                        M_HP_LOG("game::state::GameStateFactory::DeleteCharacter(" << CHAR_TO_DELETE_SPTR->Name() << "), while trying to delete " << NEXT_PATH.string() << ", failed with error code=" << errorCode);
                        return false;
                    }
                }
            }
            catch (const std::exception & E)
            {
                M_HP_LOG("game::state::GameStateFactory::DeleteCharacter(" << CHAR_TO_DELETE_SPTR->Name() << "), while trying to de-serialize/load saved game file \"" << NEXT_PATH.string() << "\", threw std::exception(\"" << E.what() << "\")");
            }
            catch (...)
            {
                M_HP_LOG("game::state::GameStateFactory::DeleteCharacter(" << CHAR_TO_DELETE_SPTR->Name() << "), while trying to de-serialize/load saved game file \"" << NEXT_PATH.string() << "\", threw UNKNOWN exception.");
            }
        }

        return false;
    }


    void GameStateFactory::Save(const GameStateSPtr_t &         GAME_SPTR,
                                const player::CharacterSPtr_t & CHARACTER_SPTR,
                                const std::string &             DIR_STR,
                                const std::string &             FILE_STR,
                                const std::string &             EXT_STR) const
    {
        namespace bfs = boost::filesystem;

        //establish the path
        const bfs::path   DIR_OBJ(bfs::system_complete(bfs::current_path() / bfs::path(DIR_STR)));

        //create directory if missing
        if (false == bfs::exists(DIR_OBJ))
        {
            boost::system::error_code ec;
            M_ASSERT_OR_LOGANDTHROW_SS((bfs::create_directory(DIR_OBJ, ec)), "game::GameStateFactory::Save() was unable to create the save game directory \"" << DIR_OBJ.string() << "\".  Error code=" << ec);
        }

        //find the next available save game filename
        std::ostringstream ss;
        bfs::path nextFilePathObj;
        for (std::size_t i(0); ; ++i)
        {
            ss.str("");
            ss << FILE_STR << i << EXT_STR;

            nextFilePathObj = (DIR_OBJ / bfs::path(ss.str()));

            if (false == bfs::exists(nextFilePathObj))
                break;

            M_ASSERT_OR_LOGANDTHROW_SS((i < 10000), "game::Save() failed to find an available save character filename after 10,000 tries.  Something is very wrong...");
        }

        //set the date and time of last save
        if (GAME_SPTR.get() != nullptr)
            GAME_SPTR->DateTimeOfLastSaveSet(sfml_util::DateTime::CurrentDateTime());

        try
        {
            std::ofstream ofs(nextFilePathObj.string());
            boost::archive::text_oarchive oa(ofs);

            //save either, not both
            if (GAME_SPTR.get() != nullptr)
                oa << * GAME_SPTR;
            else
                if (CHARACTER_SPTR.get() != nullptr)
                    oa << * CHARACTER_SPTR;
        }
        catch (const std::exception & E)
        {
            ss.str("");
            ss << "game::state::GameStateFactory::Save() while trying to serialize/save game file \"" << nextFilePathObj.string() << "\", threw std::exception(\"" << E.what() << "\".";
            M_HP_LOG(ss.str());
            throw std::runtime_error(ss.str());
        }
        catch (...)
        {
            ss.str("");
            ss << "game::state::GameStateFactory::Save() while trying to serialize/save game file \"" << nextFilePathObj.string() << "\", threw an UNKNOWN exception.";
            M_HP_LOG(ss.str());
            throw std::runtime_error(ss.str());
        }
    }

}
}
