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
#include "creature/player-party-factory.hpp"
#include "creature/player-party.hpp"
#include "game/game-state.hpp"
#include "game/world-factory.hpp"
#include "game/world.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/boost-string-includes.hpp"
#include "misc/filesystem.hpp"
#include "misc/log-macros.hpp"

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

        return misc::NotNull<GameStateFactory *>(instanceUPtr_.get());
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
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (instanceUPtr_), "GameStateFactory::Release() found instanceUPtr that was null.");

        instanceUPtr_.reset();
    }

    GameStateUPtr_t GameStateFactory::MakeForNewGame(creature::PlayerPartyUPtr_t PARTY_UPTR) const
    {
        auto gameStateUPtr { std::make_unique<GameState>(
            std::move(PARTY_UPTR), WorldFactory::MakeForNewGame()) };

        gameStateUPtr->SetupAsNewGame();

        return gameStateUPtr;
    }

    GameStateUPtr_t GameStateFactory::MakeForNewGameForTesting() const
    {
        creature::PlayerPartyFactory partyFactory;
        return MakeForNewGame(partyFactory.MakeFakeForTesting());
    }

    const GameStatePVec_t GameStateFactory::LoadAllGames() const
    {
        GameStatePVec_t gameStatePVec;

        for (const auto & FILE_PATH_STR : misc::filesystem::FindFiles(
                 false,
                 misc::filesystem::AppendPathsToCurrentThenClean(SAVED_HEROESPATH_DIR_NAME_),
                 SAVED_HEROESPATH_FILE_NAME_,
                 SAVED_HEROESPATH_FILE_EXT_))
        {
            auto gameStatePtr { new GameState() };

            try
            {
                std::ifstream inputFileStream(FILE_PATH_STR);
                boost::archive::text_iarchive inputTextArchive(inputFileStream);
                inputTextArchive >> *gameStatePtr;
            }
            catch (const std::exception & EXCEPTION)
            {
                M_HP_LOG_ERR(
                    "GameStateFactory::LoadAllGames() while trying to "
                    << "de-serialize saved game file \"" << FILE_PATH_STR
                    << "\", threw exception \"" << EXCEPTION.what()
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
        creature::CreaturePVec_t characterPVec;

        creature::CreatureFactory creatureFactory;

        for (const auto & FILE_PATH_STR : misc::filesystem::FindFiles(
                 false,
                 misc::filesystem::AppendPathsToCurrentThenClean(UNPLAYED_CHAR_DIR_NAME_),
                 "",
                 UNPLAYED_CHAR_FILE_EXT_))
        {
            const auto CREATURE_PTR { creatureFactory.MakeDefaultForDeserialization() };

            try
            {
                std::ifstream inputFileStream(FILE_PATH_STR);
                boost::archive::text_iarchive inputTextArchive(inputFileStream);
                inputTextArchive >> *CREATURE_PTR;
            }
            catch (const std::exception & EXCEPTION)
            {
                M_HP_LOG_ERR(
                    "GameStateFactory::LoadAllUnplayedCharacters(), while trying to "
                    << "de-serialize/load saved unplayed character file \"" << FILE_PATH_STR
                    << "\" threw exception \"" << EXCEPTION.what()
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
        for (const auto & FILE_PATH_STR : misc::filesystem::FindFiles(
                 false,
                 misc::filesystem::AppendPathsToCurrentThenClean(UNPLAYED_CHAR_DIR_NAME_),
                 "",
                 UNPLAYED_CHAR_FILE_EXT_))
        {
            // intentionally not using the CreatureFactory here, to avoid interactions with
            // CreatureWarehouse that are not needed
            creature::Creature loadedCharacter;

            try
            {
                std::ifstream inputFileStream(FILE_PATH_STR);
                boost::archive::text_iarchive inputTextArchive(inputFileStream);
                inputTextArchive >> loadedCharacter;
            }
            catch (const std::exception & EX)
            {
                M_HP_LOG_ERR(
                    "GameStateFactory::DeleteUnplayedCharacterFile(character={"
                    << CHAR_TO_DELETE_PTR->ToString() << "}) threw exception \"" << EX.what()
                    << "\" when trying to deserialize file \"" << FILE_PATH_STR
                    << "\".  This file is somehow invalid and cannot be loaded.  The partially "
                    << "deserialized character={" << loadedCharacter.ToString() << "}");

                continue;
            }

            loadedCharacter.AfterDeserialize();

            if (loadedCharacter == (*CHAR_TO_DELETE_PTR))
            {
                misc::filesystem::Delete(FILE_PATH_STR);
                break;
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
        const auto DIR_PATH_STR { misc::filesystem::AppendPathsToCurrentThenClean(DIR_STR) };

        auto makeFunctionDescStr { [&]() {
            std::ostringstream ss;

            ss << "GameStateFactory::Save(dir=" << DIR_PATH_STR << ", file_name=\"" << FILE_STR
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

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (GAMESTATE_PTR_OPT || CHARACTER_PTR_OPT),
            makeFunctionDescStr() << " neither a game or character was given to save.");

        if (misc::filesystem::ExistsAndIsDirectory(DIR_PATH_STR) == false)
        {
            misc::filesystem::CreateDirectory(DIR_PATH_STR);
        }

        // find the next available filename to create and save to
        const auto NEXT_AVAILABLE_FILE_PATH_STR {
            misc::filesystem::FindFirstAvailableNumberedFilenamePath(
                DIR_PATH_STR, FILE_STR, EXT_STR)
        };

        try
        {
            std::ofstream outputFileStream(NEXT_AVAILABLE_FILE_PATH_STR);
            boost::archive::text_oarchive outputTextArchive(outputFileStream);

            if (GAMESTATE_PTR_OPT)
            {
                GAMESTATE_PTR_OPT.value()->DateTimeOfLastSaveSet(gui::DateTime::CurrentDateTime());

                GAMESTATE_PTR_OPT.value()->BeforeSerialize();
                outputTextArchive << *GAMESTATE_PTR_OPT.value();
                GAMESTATE_PTR_OPT.value()->AfterSerialize();
            }
            else
            {
                CHARACTER_PTR_OPT.value()->BeforeSerialize();
                outputTextArchive << *CHARACTER_PTR_OPT.value();
                CHARACTER_PTR_OPT.value()->AfterSerialize();
            }
        }
        catch (const std::exception & EXCEPTION)
        {
            std::ostringstream ss;

            ss << makeFunctionDescStr() << " while trying to serialize file \""
               << NEXT_AVAILABLE_FILE_PATH_STR << "\", threw exception \"" << EXCEPTION.what()
               << "\".  Save failed.";

            throw std::runtime_error(ss.str());
        }
    }

} // namespace game
} // namespace heroespath
