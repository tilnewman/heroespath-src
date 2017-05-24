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
        using GameStateSPtr_t = std::shared_ptr<GameState>;
    }


    //Singleton class that provides access to all game information
    class Game
    {
        //prevent copy construction
        Game(const Game &) =delete;

        //prevent copy assignment
        Game & operator=(const Game &) =delete;

        //prevent non-singleton construction
        Game();

    public:
        static Game * Instance();
        static void Acquire();
        static void Release();

        inline state::GameStateSPtr_t State() const             { return stateSPtr_; }
        inline void StateSet(const state::GameStateSPtr_t & GS) { stateSPtr_ = GS; }

    private:
        static std::unique_ptr<Game> instanceUPtr_;
        state::GameStateSPtr_t stateSPtr_;
    };

}
#endif //GAME_STATE_GAMESTATE_INCLUDED
