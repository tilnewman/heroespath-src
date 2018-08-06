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
#include "misc/filesystem-helpers.hpp"

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
            M_HP_LOG_ERR(
                "Subsystem Instance() called but instanceUPtr_ was null: GameStateFactory");
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
        namespace fs = misc::filesystem;

        GameStatePVec_t gameStatePVec;

        for (auto const & FILE_PATH : fs::FindFilesInDirectory(
                 fs::MakeSubDirectoryPathFromCurrent(SAVED_HEROESPATH_DIR_NAME_),
                 SAVED_HEROESPATH_FILE_NAME_,
                 SAVED_HEROESPATH_FILE_EXT_))
        {
            auto gameStatePtr { new GameState() };

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
        namespace fs = misc::filesystem;

        creature::CreaturePVec_t characterPVec;

        creature::CreatureFactory creatureFactory;

        for (auto const & FILE_PATH : fs::FindFilesInDirectory(
                 fs::MakeSubDirectoryPathFromCurrent(UNPLAYED_CHAR_DIR_NAME_),
                 "",
                 UNPLAYED_CHAR_FILE_EXT_))
        {
            auto const CREATURE_PTR { creatureFactory.MakeDefaultForDeserialization() };

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
        namespace fs = misc::filesystem;

        for (auto const & FILE_PATH : fs::FindFilesInDirectory(
                 fs::MakeSubDirectoryPathFromCurrent(UNPLAYED_CHAR_DIR_NAME_),
                 "",
                 UNPLAYED_CHAR_FILE_EXT_))
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
    }

    void GameStateFactory::Save(
        const GameStatePtrOpt_t & GAMESTATE_PTR_OPT,
        const creature::CreaturePtrOpt_t & CHARACTER_PTR_OPT,
        const std::string & DIR_STR,
        const std::string & FILE_STR,
        const std::string & EXT_STR) const
    {
        namespace fs = misc::filesystem;

        auto const DIR_PATH { fs::MakeSubDirectoryPathFromCurrent(DIR_STR) };

        auto makeFunctionDescStr { [&]() {
            std::ostringstream ss;

            ss << "GameStateFactory::Save(dir=" << DIR_PATH.string() << ", file_name=\"" << FILE_STR
               << "\", file_ext=\"" << EXT_STR << "\", ";

            if (GAMESTATE_PTR_OPT)
            {
                ss << "game version";
            }
            else if (CHARACTER_PTR_OPT)
            {
                ss << "character version, character={" << CHARACTER_PTR_OPT.value()->ToString()
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

        if (fs::DoesDirectoryExist(DIR_PATH) == false)
        {
            boost::system::error_code errorCode;

            M_ASSERT_OR_LOGANDTHROW_SS(
                (boost::filesystem::create_directory(DIR_PATH, errorCode)),
                makeFunctionDescStr()
                    << " was unable to create that directory.  errorCode=" << errorCode);
        }

        // find the next available filename to create and save to
        auto const NEXT_AVAILABLE_FILE_PATH { fs::FindFirstAvailableNumberedFilename(
            DIR_PATH, FILE_STR, EXT_STR) };

        try
        {
            std::ofstream outputFileStream(NEXT_AVAILABLE_FILE_PATH.string());
            boost::archive::text_oarchive outputTextArchive(outputFileStream);

            if (GAMESTATE_PTR_OPT)
            {
                GAMESTATE_PTR_OPT.value()->DateTimeOfLastSaveSet(
                    sfml_util::DateTime::CurrentDateTime());

                GAMESTATE_PTR_OPT.value()->BeforeSerialize();
                outputTextArchive << *GAMESTATE_PTR_OPT.value();
                GAMESTATE_PTR_OPT.value()->AfterSerialize();
            }
            else
            {
                CHARACTER_PTR_OPT.value()->BeforeSerialize();
                outputTextArchive << CHARACTER_PTR_OPT.value().Obj();
                CHARACTER_PTR_OPT.value()->AfterSerialize();
            }
        }
        catch (const std::exception & E)
        {
            std::ostringstream ss;

            ss << makeFunctionDescStr() << " while trying to serialize file \""
               << NEXT_AVAILABLE_FILE_PATH.string() << "\", threw exception \"" << E.what()
               << "\".  Save failed.";

            throw std::runtime_error(ss.str());
        }
    }

} // namespace game
} // namespace heroespath
