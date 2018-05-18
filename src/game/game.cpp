// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// game.cpp
//
#include "game.hpp"

#include "creature/player-party.hpp"
#include "game/game-state.hpp"
#include "game/world-factory.hpp"
#include "log/log-macros.hpp"
#include "misc/assertlogandthrow.hpp"

#include <exception>
#include <memory>
#include <sstream>

namespace heroespath
{
namespace game
{

    std::unique_ptr<Game> Game::instanceUPtr_;

    Game::Game()
        : stateUPtr_()
    {
        M_HP_LOG_DBG("Subsystem Construction: Game");
    }

    Game::~Game() { M_HP_LOG_DBG("Subsystem Destruction: Game"); }

    misc::NotNull<Game *> Game::Instance()
    {
        if (!instanceUPtr_)
        {
            M_HP_LOG_ERR("Subsystem Instance() before Acquire(): Game");
            Acquire();
        }

        return instanceUPtr_.get();
    }

    void Game::Acquire()
    {
        if (!instanceUPtr_)
        {
            instanceUPtr_ = std::make_unique<Game>();
        }
        else
        {
            M_HP_LOG_ERR("Subsystem Acquire() after Construction: Game");
        }
    }

    void Game::Release()
    {
        M_ASSERT_OR_LOGANDTHROW_SS(
            (instanceUPtr_), "Game::Release() found instanceUPtr that was null.");

        instanceUPtr_.reset();
    }

    GameState & Game::State() const
    {
        M_ASSERT_OR_LOGANDTHROW_SS(
            (instanceUPtr_), "Game::State() was called when there was no state.");

        return *stateUPtr_;
    }

    const GameStatePtr_t Game::MakeNewGame(creature::PlayerPartyUPtr_t PARTY_UPTR)
    {
        stateUPtr_
            = std::make_unique<GameState>(std::move(PARTY_UPTR), WorldFactory::MakeForNewGame());

        stateUPtr_->IsNewGameSet(true);
        stateUPtr_->DateTimeStartedSet(sfml_util::DateTime::CurrentDateTime());

        return stateUPtr_.get();
    }

} // namespace game
} // namespace heroespath
