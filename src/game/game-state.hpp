// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GAME_GAME_STATE_HPP_INCLUDED
#define HEROESPATH_GAME_GAME_STATE_HPP_INCLUDED
//
// game-state.hpp
//  A class that represents a game in play.
//
#include "creature/player-party.hpp"
#include "game/world.hpp"
#include "gui/date-time.hpp"
#include "misc/boost-optional-that-throws.hpp"
#include "misc/boost-serialize-includes.hpp"
#include "misc/not-null.hpp"

#include <memory>
#include <vector>

namespace heroespath
{
namespace game
{

    // Encapsulates everything about a playing game.
    class GameState
    {
    public:
        GameState(const GameState &) = delete;
        GameState(GameState &&) = delete;
        GameState & operator=(const GameState &) = delete;
        GameState & operator=(GameState &&) = delete;

        explicit GameState(
            creature::PlayerPartyUPtr_t PARTY_UPTR = creature::PlayerPartyUPtr_t(),
            WorldUPtr_t WORLD_UPTR = WorldUPtr_t());

        virtual ~GameState();

        World & GetWorld();

        creature::PlayerParty & Party();

        bool IsNewGame() const { return isGameNew_; }

        void SetupAsNewGame();

        const gui::DateTime DateTimeStarted() const { return dateTimeStarted_; }

        void DateTimeStartedSet(const gui::DateTime & DT) { dateTimeStarted_ = DT; }

        const gui::DateTime DateTimeOfLastSave() const { return dateTimeLastSave_; }

        void DateTimeOfLastSaveSet(const gui::DateTime & DT) { dateTimeLastSave_ = DT; }

        void BeforeSerialize();
        void AfterSerialize();
        void AfterDeserialize();

        friend bool operator<(const GameState & L, const GameState & R);
        friend bool operator==(const GameState & L, const GameState & R);

    private:
        creature::PlayerPartyUPtr_t partyUPtr_;
        WorldUPtr_t worldUPtr_;
        bool isGameNew_;
        gui::DateTime dateTimeStarted_;
        gui::DateTime dateTimeLastSave_;

    private:
        friend class boost::serialization::access;
        template <typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & partyUPtr_;
            ar & worldUPtr_;
            ar & isGameNew_;
            ar & dateTimeStarted_;
            ar & dateTimeLastSave_;
        }
    };

    using GameStatePtr_t = misc::NotNull<GameState *>;
    using GameStatePVec_t = std::vector<GameStatePtr_t>;
    using GameStateUPtr_t = std::unique_ptr<GameState>;

    bool operator<(const GameState & L, const GameState & R);

    bool operator==(const GameState & L, const GameState & R);

    inline bool operator!=(const GameState & L, const GameState & R) { return !(L == R); }

} // namespace game
} // namespace heroespath

#endif // HEROESPATH_GAME_GAME_STATE_HPP_INCLUDED
