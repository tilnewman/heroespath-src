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
#ifndef HEROESPATH_GAME_HEROESPATH_HPP_INCLUDED
#define HEROESPATH_GAME_HEROESPATH_HPP_INCLUDED
//
// game.hpp
//  A class that provides access to all game information.
//
#include <memory>

namespace heroespath
{

namespace state
{
    class GameState;
    using GameStatePtr_t = GameState *;
    using GameStateUPtr_t = std::unique_ptr<GameState>;
} // namespace state

namespace game
{

    // Singleton class that provides access to all game information
    class Game
    {
        Game(const Game &) = delete;
        Game & operator=(const Game &) = delete;

    public:
        Game();
        ~Game();

        static Game * Instance();
        static void Acquire();
        static void Release();

        state::GameState & State() const;
        void StateStore(const state::GameStatePtr_t);

    private:
        static std::unique_ptr<Game> instanceUPtr_;
        state::GameStateUPtr_t stateUPtr_;
    };
} // namespace game
} // namespace heroespath

#endif // HEROESPATH_GAME_HEROESPATH_HPP_INCLUDED
