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
// game.cpp
//
#include "game.hpp"

#include "log/log-macros.hpp"
#include "misc/assertlogandthrow.hpp"
#include "player/party.hpp"
#include "state/game-state.hpp"
#include "state/world-factory.hpp"

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

    state::GameState & Game::State() const
    {
        M_ASSERT_OR_LOGANDTHROW_SS(
            (instanceUPtr_), "Game::State() was called when there was no state.");

        return *stateUPtr_;
    }

    const state::GameStatePtr_t Game::MakeNewGame(player::PartyUPtr_t PARTY_UPTR)
    {
        stateUPtr_ = std::make_unique<state::GameState>(
            std::move(PARTY_UPTR), state::WorldFactory::MakeForNewGame());

        stateUPtr_->IsNewGameSet(true);
        stateUPtr_->DateTimeStartedSet(sfml_util::DateTime::CurrentDateTime());

        return stateUPtr_.get();
    }

} // namespace game
} // namespace heroespath
