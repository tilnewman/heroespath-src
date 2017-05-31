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

#include "game/log-macros.hpp"
#include "game/state/game-state.hpp"

#include "misc/assertlogandthrow.hpp"

#include <memory>
#include <sstream>
#include <exception>


namespace game
{

    std::unique_ptr<Game> Game::instanceUPtr_{ nullptr };


    Game::Game()
    :
        stateUPtr_()
    {
        M_HP_LOG_DBG("Singleton Construction: Game");
    }


    Game::~Game()
    {
        M_HP_LOG_DBG("Singleton Destruction: Game");
    }


    Game * Game::Instance()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            M_HP_LOG_WRN("Singleton Instance() before Acquire(): Game");
            Acquire();
        }

        return instanceUPtr_.get();
    }


    void Game::Acquire()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            instanceUPtr_.reset(new Game);
        }
        else
        {
            M_HP_LOG_WRN("Singleton Acquire() after Construction: Game");
        }
    }


    void Game::Release()
    {
        M_ASSERT_OR_LOGANDTHROW_SS((instanceUPtr_.get() != nullptr),
            "game::Game::Release() found instanceUPtr that was null.");
        instanceUPtr_.reset();
    }


    state::GameState & Game::State() const
    {
        if (stateUPtr_.get() == nullptr)
        {
            std::ostringstream ss;
            ss << "game::Game::State() was called when there was no state.";
            throw std::runtime_error(ss.str());
        }
        else
        {
            return * stateUPtr_.get();
        }
    }


    void Game::StateStore(const state::GameStatePtr_t STATE_PTR)
    {
        if (stateUPtr_.get() != nullptr)
        {
            M_HP_LOG_WRN("game::Game::StateSet() is going to free an old game state"
                << " and replace it with a new one.");
        }

        stateUPtr_.reset(STATE_PTR);
    }

}
