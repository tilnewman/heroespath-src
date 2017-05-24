//
// game.cpp
//
#include "game.hpp"
#include "game/state/game-state.hpp"

#include <memory>


namespace game
{

    std::unique_ptr<Game> Game::instanceUPtr_{ nullptr };


    Game::Game()
    :
        stateSPtr_()
    {}


    Game * const Game::Instance()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            instanceUPtr_.reset( new Game );
        }

        return instanceUPtr_.get();
    }


    void Game::InstanceRelease()
    {
        instanceUPtr_.reset();
    }

}
