// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GAME_HEROESPATH_HPP_INCLUDED
#define HEROESPATH_GAME_HEROESPATH_HPP_INCLUDED
//
// game.hpp
//  A class that provides access to all game information.
//
#include "misc/not-null.hpp"

#include <memory>

namespace heroespath
{
namespace player
{
    class Party;
    using PartyUPtr_t = std::unique_ptr<Party>;
} // namespace player
namespace state
{
    class GameState;
    using GameStatePtr_t = misc::NotNull<GameState *>;
    using GameStateUPtr_t = std::unique_ptr<GameState>;
} // namespace state

namespace game
{

    // Subsystem class that provides access to all game information
    class Game
    {
    public:
        Game(const Game &) = delete;
        Game(Game &&) = delete;
        Game & operator=(const Game &) = delete;
        Game & operator=(Game &&) = delete;

    public:
        Game();
        ~Game();

        static misc::NotNull<Game *> Instance();
        static void Acquire();
        static void Release();

        state::GameState & State() const;
        const state::GameStatePtr_t MakeNewGame(player::PartyUPtr_t PARTY_UPTR);

    private:
        static std::unique_ptr<Game> instanceUPtr_;
        state::GameStateUPtr_t stateUPtr_;
    };

} // namespace game
} // namespace heroespath

#endif // HEROESPATH_GAME_HEROESPATH_HPP_INCLUDED
