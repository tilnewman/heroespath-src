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
#ifndef HEROESPATH_STATE_GAMESTATE_HPP_INCLUDED
#define HEROESPATH_STATE_GAMESTATE_HPP_INCLUDED
//
// game-state.hpp
//  A class that represents a game in play.
//
#include "misc/boost-serialize-includes.hpp"
#include "sfml-util/date-time.hpp"

#include <memory>
#include <vector>
#include <set>


namespace heroespath
{
namespace player
{
    class Party;
    using PartyPtr_t  = Party *;
    using PartyUPtr_t = std::unique_ptr<Party>;
}
namespace state
{

    //forward declarations
    class World;
    using WorldPtr_t  = World *;
    using WorldUPtr_t = std::unique_ptr<World>;


    //Encapsulates everything about a saved game.
    class GameState
    {
        GameState(const GameState &) =delete;
        GameState & operator=(const GameState &) =delete;

    public:
        explicit GameState(
            const player::PartyPtr_t PARTY_PTR = nullptr,
            const WorldPtr_t WORLD_PTR = nullptr);

        virtual ~GameState();

        World & World();

        player::Party & Party();

        inline bool IsNewGame() const
        {
            return isGameNew_;
        }

        inline void IsNewGameSet(const bool B)
        {
            isGameNew_ = B;
        }

        inline const sfml_util::DateTime DateTimeStarted() const
        {
            return dateTimeStarted_;
        }

        inline void DateTimeStartedSet(const sfml_util::DateTime & DT)
        {
            dateTimeStarted_ = DT;
        }

        inline const sfml_util::DateTime DateTimeOfLastSave() const
        {
            return dateTimeLastSave_;
        }

        inline void DateTimeOfLastSaveSet(const sfml_util::DateTime & DT)
        {
            dateTimeLastSave_ = DT;
        }

        friend bool operator<(const GameState & L, const GameState & R);
        friend bool operator==(const GameState & L, const GameState & R);

    private:
        player::PartyUPtr_t      partyUPtr_;
        WorldUPtr_t              worldUPtr_;
        bool                     isGameNew_;
        sfml_util::DateTime      dateTimeStarted_;
        sfml_util::DateTime      dateTimeLastSave_;

    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & partyUPtr_;
            ar & worldUPtr_;
            ar & isGameNew_;
            ar & dateTimeStarted_;
            ar & dateTimeLastSave_;
        }
    };


    using GameStatePtr_t  = GameState *;
    using GameStatePSet_t = std::set<GameStatePtr_t>;


    bool operator<(const GameState & L, const GameState & R);

    bool operator==(const GameState & L, const GameState & R);

    inline bool operator!=(const GameState & L, const GameState & R)
    {
        return ! (L == R);
    }

}
}

#endif //HEROESPATH_STATE_GAMESTATE_HPP_INCLUDED
