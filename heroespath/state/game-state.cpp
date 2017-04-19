//
// game-state.cpp
//
#include "game-state.hpp"

#include "heroespath/player/party.hpp"
#include "heroespath/state/world-state.hpp"
#include "heroespath/location/location.hpp"

#include <tuple>


namespace heroespath
{
namespace state
{

    GameState::GameState(const player::PartySPtr_t &    PARTY_SPTR,
                         const WorldStateSPtr_t &       WORLD_STATE_SPTR,
                         const location::LocationSPtr_t LOCATION_SPTR)
    :
        partySPtr_       (PARTY_SPTR),
        worldStateSPtr_  (WORLD_STATE_SPTR),
        isGameNew_       (false),
        dateTimeStarted_ (),
        dateTimeLastSave_(),
        locationSPtr_    (LOCATION_SPTR)
    {}


    GameState::~GameState()
    {}


    bool operator<(const GameState & L, const GameState & R)
    {
        return std::tie(L.partySPtr_,
                        L.worldStateSPtr_,
                        L.isGameNew_,
                        L.dateTimeStarted_,
                        L.dateTimeLastSave_,
                        L.locationSPtr_)
               <
               std::tie(R.partySPtr_,
                        R.worldStateSPtr_,
                        R.isGameNew_,
                        R.dateTimeStarted_,
                        R.dateTimeLastSave_,
                        R.locationSPtr_);
    }


    bool operator==(const GameState & L, const GameState & R)
    {
        return std::tie(L.partySPtr_,
                        L.worldStateSPtr_,
                        L.isGameNew_,
                        L.dateTimeStarted_,
                        L.dateTimeLastSave_,
                        L.locationSPtr_)
               ==
               std::tie(R.partySPtr_,
                        R.worldStateSPtr_,
                        R.isGameNew_,
                        R.dateTimeStarted_,
                        R.dateTimeLastSave_,
                        R.locationSPtr_);
    }

}
}
