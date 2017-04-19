#ifndef APPBASE_HEROESPATH_GAME_INCLUDED
#define APPBASE_HEROESPATH_GAME_INCLUDED
//
// game.hpp
//  A class that provides access to all game information.
//
#include <memory>


namespace heroespath
{

    //forward declaration
    namespace state
    {
        class GameState;
        using GameStateSPtr_t = std::shared_ptr<GameState>;
    }


    //types required by the singleton implementation
    class Game;
    using GameSPtr_t = std::shared_ptr<Game>;


    //provides access to all game information
    class Game
    {
        //prevent copy construction
        Game(const Game &);

        //prevent copy assignment
        Game & operator=(const Game &);

        //prevent non-singleton construction
        Game();

    public:
        virtual ~Game();
        static GameSPtr_t Instance();

        inline state::GameStateSPtr_t State() const             { return stateSPtr_; }
        inline void StateSet(const state::GameStateSPtr_t & GS) { stateSPtr_ = GS; }

    private:
        static GameSPtr_t instanceSPtr_;
        //
        state::GameStateSPtr_t stateSPtr_;
    };

}
#endif //APPBASE_HEROESPATH_STATE_GAMESTATE_INCLUDED
