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

#include "player/party.hpp"
#include "state/world.hpp"

#include <tuple>
#include <sstream>
#include <exception>


namespace heroespath
{
namespace state
{

    GameState::GameState(
        player::PartyUPtr_t PARTY_UPTR,
        WorldUPtr_t WORLD_UPTR)
    :
        partyUPtr_       (std::move(PARTY_UPTR)),
        worldUPtr_       (std::move(WORLD_UPTR)),
        isGameNew_       (false),
        dateTimeStarted_ (),
        dateTimeLastSave_()
    {}


    GameState::~GameState()
    {}


    World & GameState::World()
    {
        if (worldUPtr_.get() == nullptr)
        {
            std::ostringstream ss;
            ss << "state::GameState::World() called when the world ptr was null.";
            throw std::runtime_error(ss.str());
        }
        else
        {
            return * worldUPtr_;
        }
    }


    player::Party & GameState::Party()
    {
        if (partyUPtr_.get() == nullptr)
        {
            std::ostringstream ss;
            ss << "state::GameState::Party() called when the party ptr was null.";
            throw std::runtime_error(ss.str());
        }
        else
        {
            return * partyUPtr_;
        }
    }


    bool operator<(const GameState & L, const GameState & R)
    {
        return std::tie(L.partyUPtr_,
                        L.worldUPtr_,
                        L.isGameNew_,
                        L.dateTimeStarted_,
                        L.dateTimeLastSave_)
               <
               std::tie(R.partyUPtr_,
                        R.worldUPtr_,
                        R.isGameNew_,
                        R.dateTimeStarted_,
                        R.dateTimeLastSave_);
    }


    bool operator==(const GameState & L, const GameState & R)
    {
        return std::tie(L.partyUPtr_,
                        L.worldUPtr_,
                        L.isGameNew_,
                        L.dateTimeStarted_,
                        L.dateTimeLastSave_)
               ==
               std::tie(R.partyUPtr_,
                        R.worldUPtr_,
                        R.isGameNew_,
                        R.dateTimeStarted_,
                        R.dateTimeLastSave_);
    }

}
}
