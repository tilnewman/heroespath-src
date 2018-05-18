// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// game-state-factory.cpp
//
#include "game-state-factory.hpp"

// This is the one and only place where this is included,
// and it needs to stay that way.  zTn 2017-3-30
#include "creature/player-party-serialize-includes.hpp"

#include "creature/creature-factory.hpp"
#include "creature/creature-warehouse.hpp"
#include "creature/creature.hpp"
#include "creature/player-party.hpp"
#include "game/game-state.hpp"
#include "game/game.hpp"
#include "game/world-factory.hpp"
#include "game/world.hpp"
#include "log/log-macros.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/boost-string-includes.hpp"

#include <limits>
#include <vector>

namespace heroespath
{
namespace game
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
        M_HP_LOG_DBG("Subsystem Construction: GameStateFactory");
    }

    GameStateFactory::~GameStateFactory()
    {
        M_HP_LOG_DBG("Subsystem Destruction: GameStateFactory");
    }

    misc::NotNull<GameStateFactory *> GameStateFactory::Instance()
    {
        if (!instanceUPtr_)
        {
            M_HP_LOG_ERR("Subsystem Instance() before Acquire(): GameStateFactory");
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
            M_HP_LOG_ERR("Subsystem Acquire() after Construction: GameStateFactory");
        }
    }

    void GameStateFactory::Release()
    {
        M_ASSERT_OR_LOGANDTHROW_SS(
            (instanceUPtr_), "GameStateFactory::Release() found instanceUPtr that was null.");

        instanceUPtr_.reset();
    }

    void GameStateFactory::NewGame(creature::PlayerPartyUPtr_t PARTY_UPTR) const
    {
        game::Game::Instance()->MakeNewGame(std::move(PARTY_UPTR));
    }

    const GameStatePVec_t GameStateFactory::LoadAllGames() const
    {
        auto const DIR_PATH{ CreateDirectoryPathFromCurrent(SAVED_HEROESPATH_DIR_NAME_) };

        GameStatePVec_t gameStatePVec;

        // it is possible that during normal operation the saved games dir won't exist, so don't log
        // an error here
        if (DoesDirectoryExist(DIR_PATH) == false)
        {
            return gameStatePVec;
        }

        auto const PATHS_TO_SAVED_GAME_FILES{ CreateVectorOfMatchingFilePathsInDir(
            DIR_PATH, SAVED_HEROESPATH_FILE_NAME_, SAVED_HEROESPATH_FILE_EXT_) };

        // it is possible that during normal operation the saved games dir will be empty, so don't
        // bother checking PATHS_TO_SAVED_GAME_FILES.empty()

        for (auto const & FILE_PATH : PATHS_TO_SAVED_GAME_FILES)
        {
            auto gameStatePtr{ new GameState() };

            try
            {
                std::ifstream inputFileStream(FILE_PATH.string());
                boost::archive::text_iarchive inputTextArchive(inputFileStream);
                inputTextArchive >> *gameStatePtr;
            }
            catch (const std::exception & E)
            {
                M_HP_LOG_ERR(
                    "GameStateFactory::LoadAllGames() while trying to "
                    << "de-serialize saved game file \"" << FILE_PATH.string()
                    << "\", threw exception \"" << E.what()
                    << "\".  That saved game file is somehow invalid and cannot be loaded.");

                delete gameStatePtr;
                continue;
            }

            gameStatePtr->AfterDeserialize();
            gameStatePVec.emplace_back(gameStatePtr);
        }

        return gameStatePVec;
    }

    void GameStateFactory::SaveGame(const GameStatePtr_t GAMESTATE_PTR) const
    {
        Save(
            GameStatePtrOpt_t(GAMESTATE_PTR),
            boost::none,
            SAVED_HEROESPATH_DIR_NAME_,
            SAVED_HEROESPATH_FILE_NAME_,
            SAVED_HEROESPATH_FILE_EXT_);
    }

    const creature::CreaturePVec_t GameStateFactory::LoadAllUnplayedCharacters() const
    {
        auto const DIR_PATH{ CreateDirectoryPathFromCurrent(UNPLAYED_CHAR_DIR_NAME_) };

        creature::CreaturePVec_t characterPVec;

        // it is possible that during normal operation that the unplayed characters save dir won't
        // exist, so don't log an error here
        if (DoesDirectoryExist(DIR_PATH) == false)
        {
            return characterPVec;
        }

        auto const SAVED_UNPLAYED_CHARACTER_FILE_PATHS{ CreateVectorOfMatchingFilePathsInDir(
            DIR_PATH, "", UNPLAYED_CHAR_FILE_EXT_) };

        // it is possible that during normal operation that the unplayed characters save dir will be
        // empty, so don't bother checking if SAVED_UNPLAYED_CHARACTER_FILE_PATHS.empty()

        creature::CreatureFactory creatureFactory;

        // try and load each character file
        for (auto const & FILE_PATH : SAVED_UNPLAYED_CHARACTER_FILE_PATHS)
        {
            auto const CREATURE_PTR{ creatureFactory.MakeDefaultForDeserialization() };

            try
            {
                std::ifstream inputFileStream(FILE_PATH.string());
                boost::archive::text_iarchive inputTextArchive(inputFileStream);
                inputTextArchive >> *CREATURE_PTR;
            }
            catch (const std::exception & E)
            {
                M_HP_LOG_ERR(
                    "GameStateFactory::LoadAllUnplayedCharacters(), while trying to "
                    << "de-serialize/load saved unplayed character file \"" << FILE_PATH.string()
                    << "\" threw exception \"" << E.what()
                    << "\".  That character file is somehow invalid and cannot be loaded.");

                creature::CreatureWarehouse::Access().Free(CREATURE_PTR);
                continue;
            }

            CREATURE_PTR->AfterDeserialize();
            characterPVec.emplace_back(CREATURE_PTR);
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

    void GameStateFactory::DeleteUnplayedCharacterFile(
        const creature::CreaturePtr_t CHAR_TO_DELETE_PTR) const
    {
        auto const DIR_PATH{ CreateDirectoryPathFromCurrent(UNPLAYED_CHAR_DIR_NAME_) };

        // it is possible that if no new characters have been saved that the dir isn't there, but if
        // that is the case then this function should never be called, so log an error here.
        if (DoesDirectoryExist(DIR_PATH) == false)
        {
            M_HP_LOG_ERR(
                "GameStateFactory::DeleteUnplayedCharacterFile(character={"
                << CHAR_TO_DELETE_PTR->ToString()
                << "}) failed because the saved unplayed character directory (" << DIR_PATH.string()
                << ") was not there.  This function should never be called in this case.");

            return;
        }

        auto const SAVED_UNPLAYED_CHARACTER_FILE_PATHS{ CreateVectorOfMatchingFilePathsInDir(
            DIR_PATH, "", UNPLAYED_CHAR_FILE_EXT_) };

        if (SAVED_UNPLAYED_CHARACTER_FILE_PATHS.empty())
        {
            M_HP_LOG_ERR(
                "GameStateFactory::DeleteUnplayedCharacterFile(character={"
                << CHAR_TO_DELETE_PTR->ToString()
                << "}) failed because the saved unplayed character directory (" << DIR_PATH.string()
                << ") was empty.  This function should never be called in this case.");

            return;
        }

        for (auto const & FILE_PATH : SAVED_UNPLAYED_CHARACTER_FILE_PATHS)
        {
            // intentionally not using the CreatureFactory here, to avoid interactions with
            // CreatureWarehouse that are not needed
            creature::Creature loadedCharacter;

            try
            {
                std::ifstream inputFileStream(FILE_PATH.string());
                boost::archive::text_iarchive inputTextArchive(inputFileStream);
                inputTextArchive >> loadedCharacter;
            }
            catch (const std::exception & EX)
            {
                M_HP_LOG_ERR(
                    "GameStateFactory::DeleteUnplayedCharacterFile(character={"
                    << CHAR_TO_DELETE_PTR->ToString() << "}) threw exception \"" << EX.what()
                    << "\" when trying to deserialize file \"" << FILE_PATH.string()
                    << "\".  This file is somehow invalid and cannot be loaded.  The partially "
                    << "deserialized character={" << loadedCharacter.ToString() << "}");

                continue;
            }

            loadedCharacter.AfterDeserialize();

            if (loadedCharacter == (*CHAR_TO_DELETE_PTR))
            {
                boost::system::error_code errorCode;
                if (boost::filesystem::remove(FILE_PATH, errorCode) == false)
                {
                    M_HP_LOG_ERR(
                        "GameStateFactory::DeleteUnplayedCharacterFile(character={"
                        << CHAR_TO_DELETE_PTR->ToString() << "}) failed to delete file \""
                        << FILE_PATH.string() << "\" with error_code=" << errorCode
                        << ".  This file can safely be deleted manually.");
                }

                return;
            }
        }

        M_HP_LOG_ERR(
            "GameStateFactory::DeleteUnplayedCharacterFile(character={"
            << CHAR_TO_DELETE_PTR->ToString() << "}) failed to find a saved file (out of "
            << SAVED_UNPLAYED_CHARACTER_FILE_PATHS.size() << ") matcheing that character.");

        return;
    }

    void GameStateFactory::Save(
        const GameStatePtrOpt_t GAMESTATE_PTR_OPT,
        const creature::CreaturePtrOpt_t CHARACTER_PTR_OPT,
        const std::string & DIR_STR,
        const std::string & FILE_STR,
        const std::string & EXT_STR) const
    {
        namespace bfs = boost::filesystem;

        auto const DIR_PATH{ CreateDirectoryPathFromCurrent(DIR_STR) };

        auto makeFunctionDescStr{ [&]() {
            std::ostringstream ss;

            ss << "GameStateFactory::Save(dir=" << DIR_PATH.string() << ", file_name=\"" << FILE_STR
               << "\", file_ext=\"" << EXT_STR << "\", ";

            if (GAMESTATE_PTR_OPT)
            {
                ss << "game version";
            }
            else if (CHARACTER_PTR_OPT)
            {
                ss << "character version, character={" << CHARACTER_PTR_OPT->Obj().ToString()
                   << "}";
            }
            else
            {
                ss << "ERROR=\"neither a game or character was given to save\"";
            }
            ss << ")";
            return ss.str();
        } };

        M_ASSERT_OR_LOGANDTHROW_SS(
            (GAMESTATE_PTR_OPT || CHARACTER_PTR_OPT),
            makeFunctionDescStr() << " neither a game or character was given to save.");

        if (DoesDirectoryExist(DIR_PATH) == false)
        {
            boost::system::error_code errorCode;

            M_ASSERT_OR_LOGANDTHROW_SS(
                (bfs::create_directory(DIR_PATH, errorCode)),
                makeFunctionDescStr()
                    << " was unable to create that directory.  errorCode=" << errorCode);
        }

        // find the next available filename to create and save to
        bfs::path fileToSavePath;
        for (std::size_t i(1);; ++i) // start at one so we can detect rollover
        {
            std::ostringstream filenameSS;
            filenameSS << FILE_STR << i << EXT_STR;

            fileToSavePath = DIR_PATH / bfs::path(filenameSS.str());

            if (bfs::exists(fileToSavePath) == false)
            {
                break;
            }
            else if (0 == i)
            {
                std::ostringstream errorSS;
                errorSS << makeFunctionDescStr() << " failed to find an available filename after "
                        << std::numeric_limits<std::size_t>::max()
                        << " tries.  Infinite loop detected.  Something is very wrong...";

                throw std::runtime_error(errorSS.str());
            }
        }

        // clean up the path in case we need to log it
        fileToSavePath = bfs::system_complete(fileToSavePath).normalize();

        try
        {
            std::ofstream outputFileStream(fileToSavePath.string());
            boost::archive::text_oarchive outputTextArchive(outputFileStream);

            if (GAMESTATE_PTR_OPT)
            {
                GAMESTATE_PTR_OPT->Obj().DateTimeOfLastSaveSet(
                    sfml_util::DateTime::CurrentDateTime());

                GAMESTATE_PTR_OPT->Obj().BeforeSerialize();
                outputTextArchive << *GAMESTATE_PTR_OPT.value();
                GAMESTATE_PTR_OPT->Obj().AfterSerialize();
            }
            else
            {
                CHARACTER_PTR_OPT->Obj().BeforeSerialize();
                outputTextArchive << CHARACTER_PTR_OPT->Obj();
                CHARACTER_PTR_OPT->Obj().AfterSerialize();
            }
        }
        catch (const std::exception & E)
        {
            std::ostringstream ss;

            ss << makeFunctionDescStr() << " while trying to serialize file \""
               << fileToSavePath.string() << "\", threw exception \"" << E.what()
               << "\".  Save failed.";

            throw std::runtime_error(ss.str());
        }
    }

    const boost::filesystem::path GameStateFactory::CreateDirectoryPathFromCurrent(
        const std::string & SUBDIRECTORY_NAME) const
    {
        namespace bfs = boost::filesystem;

        return bfs::system_complete(bfs::current_path() / bfs::path(SUBDIRECTORY_NAME)).normalize();
    }

    bool GameStateFactory::DoesDirectoryExist(const boost::filesystem::path & DIR_PATH) const
    {
        namespace bfs = boost::filesystem;

        return (bfs::exists(DIR_PATH) && bfs::is_directory(DIR_PATH));
    }

    const std::vector<boost::filesystem::path>
        GameStateFactory::CreateVectorOfMatchingFilePathsInDir(
            const boost::filesystem::path & DIR_PATH,
            const std::string & FILENAME_PREFIX,
            const std::string & FILENAME_EXTENSION) const
    {
        namespace bfs = boost::filesystem;

        std::vector<bfs::path> pathsToMatchingFiles;

        for (auto const & FILE_PATH : DIR_PATH)
        {
            if (bfs::is_regular_file(FILE_PATH))
            {
                namespace ba = boost::algorithm;

                if (FILENAME_PREFIX.empty()
                    || ba::starts_with(FILE_PATH.leaf().string(), FILENAME_PREFIX))
                {
                    if (FILENAME_EXTENSION.empty()
                        || ba::ends_with(FILE_PATH.leaf().string(), FILENAME_EXTENSION))
                    {
                        pathsToMatchingFiles.emplace_back(
                            (DIR_PATH / FILE_PATH.leaf()).normalize());
                    }
                }
            }
        }

        return pathsToMatchingFiles;
    }

} // namespace game
} // namespace heroespath
