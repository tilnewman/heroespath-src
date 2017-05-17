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
#include "utilz/assertlogandthrow.hpp"
#include "game/log-macros.hpp"

#include <boost/filesystem.hpp>


namespace game
{
namespace state
{

    GameStateFactorySPtr_t GameStateFactory::instanceSPtr_;
    //
    const std::string GameStateFactory::SAVED_GAME_DIR_NAME_("saved_games");
    const std::string GameStateFactory::SAVED_GAME_FILE_NAME_("game");
    const std::string GameStateFactory::SAVED_GAME_FILE_EXT_(".dat");
    //
    const std::string GameStateFactory::SAVED_CHAR_DIR_NAME_("unplayed_characters");
    const std::string GameStateFactory::SAVED_CHAR_FILE_EXT_(".dat");


    GameStateFactory::GameStateFactory()
    {}


    GameStateFactory::~GameStateFactory()
    {}


    GameStateFactorySPtr_t GameStateFactory::Instance()
    {
        if (instanceSPtr_.get() == nullptr)
            instanceSPtr_.reset( new GameStateFactory );

        return instanceSPtr_;
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
        const std::string DIR_OBJ_STR(DIR_OBJ.string());

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
        const std::string DIR_OBJ_STR(DIR_OBJ.string());

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
        const std::string DIR_OBJ_STR(DIR_OBJ.string());

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
        const std::string DIR_OBJ_STR(DIR_OBJ.string());

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
