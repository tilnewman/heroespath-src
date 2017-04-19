#ifndef APPBASE_HEROESPATH_STATE_GAMESTATE_INCLUDED
#define APPBASE_HEROESPATH_STATE_GAMESTATE_INCLUDED
//
// game-state.hpp
//  A class that represents a game in play.
//
#include "sfml-util/boost-serialize-includes.hpp"
#include "sfml-util/date-time.hpp"

#include <memory>
#include <vector>
#include <set>


namespace heroespath
{
    namespace player
    {
        class Party;
        using PartySPtr_t = std::shared_ptr<Party>;
    }

    namespace location
    {
        class Location;
        using LocationSPtr_t = std::shared_ptr<Location>;
    }

namespace state
{

    //forward declarations
    class WorldState;
    using WorldStateSPtr_t = std::shared_ptr<WorldState>;


    //Encapsulates everything about a saved game.
    class GameState
    {
        //prevent copy construction
        GameState(const GameState &);

        //prevent copy assignment
        GameState & operator=(const GameState &);

    public:
        GameState(const player::PartySPtr_t &    PARTY_SPTR       = player::PartySPtr_t(),
                  const WorldStateSPtr_t &       WORLD_STATE_SPTR = WorldStateSPtr_t(),
                  const location::LocationSPtr_t LOCATION_SPTR    = location::LocationSPtr_t());

        virtual ~GameState();

        inline WorldStateSPtr_t World()         { return worldStateSPtr_; }

        inline player::PartySPtr_t Party()      { return partySPtr_; }

        inline bool IsNewGame() const           { return isGameNew_; }
        inline void IsNewGameSet(const bool B)  { isGameNew_ = B; }

        inline const sfml_util::DateTime DateTimeStarted() const            { return dateTimeStarted_; }
        inline void DateTimeStartedSet(const sfml_util::DateTime & DT)      { dateTimeStarted_ = DT; }

        inline const sfml_util::DateTime DateTimeOfLastSave() const         { return dateTimeLastSave_; }
        inline void DateTimeOfLastSaveSet(const sfml_util::DateTime & DT)   { dateTimeLastSave_ = DT; }

        inline const location::LocationSPtr_t Location() const              { return locationSPtr_; }
        inline void LocationSet(const location::LocationSPtr_t & L_SPTR)    { locationSPtr_ = L_SPTR; }

        friend bool operator<(const GameState & L, const GameState & R);
        friend bool operator==(const GameState & L, const GameState & R);

    private:
        player::PartySPtr_t      partySPtr_;
        WorldStateSPtr_t         worldStateSPtr_;
        bool                     isGameNew_;
        sfml_util::DateTime      dateTimeStarted_;
        sfml_util::DateTime      dateTimeLastSave_;
        location::LocationSPtr_t locationSPtr_;

    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & partySPtr_;
            ar & worldStateSPtr_;
            ar & isGameNew_;
            ar & dateTimeStarted_;
            ar & dateTimeLastSave_;
            ar & locationSPtr_;
        }
    };


    using GameStateSPtr_t = std::shared_ptr<GameState>;
    using GameStateSVec_t = std::vector<GameStateSPtr_t>;
    using GameStateSSet_t = std::set<GameStateSPtr_t>;


    bool operator<(const GameState & L, const GameState & R);

    bool operator==(const GameState & L, const GameState & R);

    inline bool operator!=(const GameState & L, const GameState & R)
    {
        return ! (L == R);
    }

}
}
#endif //APPBASE_HEROESPATH_STATE_GAMESTATE_INCLUDED
