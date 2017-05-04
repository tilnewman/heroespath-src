//
// game.cpp
//
#include "game.hpp"
#include "game/state/game-state.hpp"


namespace game
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
