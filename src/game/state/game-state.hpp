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
#ifndef GAME_STATE_GAMESTATE_INCLUDED
#define GAME_STATE_GAMESTATE_INCLUDED
//
// game-state.hpp
//  A class that represents a game in play.
//
#include "misc/boost-serialize-includes.hpp"
#include "sfml-util/date-time.hpp"

#include <memory>
#include <vector>
#include <set>


namespace game
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
        GameState(const GameState &) =delete;

        //prevent copy assignment
        GameState & operator=(const GameState &) =delete;

    public:
        explicit GameState(const player::PartySPtr_t &      PARTY_SPTR       = player::PartySPtr_t(),
                           const WorldStateSPtr_t &         WORLD_STATE_SPTR = WorldStateSPtr_t(),
                           const location::LocationSPtr_t & LOCATION_SPTR    = location::LocationSPtr_t());

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
#endif //GAME_STATE_GAMESTATE_INCLUDED
