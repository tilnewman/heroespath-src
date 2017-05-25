// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software.  If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
//
// game-state.cpp
//
#include "game-state.hpp"

#include "game/player/party.hpp"
#include "game/state/world-state.hpp"
#include "game/location/location.hpp"

#include <tuple>


namespace game
{
namespace state
{

    GameState::GameState(const player::PartySPtr_t &      PARTY_SPTR,
                         const WorldStateSPtr_t &         WORLD_STATE_SPTR,
                         const location::LocationSPtr_t & LOCATION_SPTR)
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
