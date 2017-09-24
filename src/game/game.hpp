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
#ifndef GAME_GAME_INCLUDED
#define GAME_GAME_INCLUDED
//
// game.hpp
//  A class that provides access to all game information.
//
#include <memory>


namespace game
{

    //forward declaration
    namespace state
    {
        class GameState;
        using GameStatePtr_t = GameState *;
        using GameStateUPtr_t = std::unique_ptr<GameState>;
    }


    //Singleton class that provides access to all game information
    class Game
    {
        Game(const Game &) =delete;
        Game & operator=(const Game &) =delete;

        //singleton construction only
        Game();

    public:
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

}
#endif //GAME_STATE_GAMESTATE_INCLUDED
