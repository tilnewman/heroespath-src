//
// game.cpp
//
#include "game.hpp"
#include "game/state/game-state.hpp"

#include "utilz/assertlogandthrow.hpp"

#include <memory>


namespace game
{

    std::unique_ptr<Game> Game::instanceUPtr_{ nullptr };


    Game::Game()
    :
        stateSPtr_()
    {}


    Game * Game::Instance()
    {
        if (instanceUPtr_.get() == nullptr)
        {
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
    }


    void Game::Release()
    {
        M_ASSERT_OR_LOGANDTHROW_SS((instanceUPtr_.get() != nullptr), "game::Game::Release() called while instanceUPtr_ was null.");
        instanceUPtr_.reset();
    }

}
