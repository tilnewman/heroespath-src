//
// game.cpp
//
#include "game.hpp"
#include "heroespath/state/game-state.hpp"


namespace heroespath
{

    GameSPtr_t Game::instanceSPtr_;


    Game::Game()
    :
        stateSPtr_()
    {}


    Game::~Game()
    {}


    GameSPtr_t Game::Instance()
    {
        if (instanceSPtr_.get() == nullptr)
            instanceSPtr_.reset( new Game );

        return instanceSPtr_;
    }

}
