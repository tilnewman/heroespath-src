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
#ifndef GAME_STATE_WORLDSTATE_INCLUDED
#define GAME_STATE_WORLDSTATE_INCLUDED
//
// world-state.hpp
//  A class that represents the entire state of the game world.
//
#include "misc/boost-serialize-includes.hpp"

#include <memory>


namespace game
{
namespace state
{

    //Encapsulates everything about the game world.
    class WorldState
    {
        //prevent copy construction
        WorldState(const WorldState &) =delete;

        //prevent copy assignment
        WorldState & operator=(const WorldState &) =delete;

    public:
        WorldState();
        virtual ~WorldState();

        inline std::size_t EncounterCount() const   { return encounterCount_; }
        inline void EncoundterCountInc()            { ++encounterCount_; }

    private:
        //TODO quests
        //TODO events
        //TODO map states
        std::size_t encounterCount_;

    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & encounterCount_;
        }
    };

    using WorldStatePtr_t  = WorldState *;
    using WorldStateUPtr_t = std::unique_ptr<WorldState>;

}
}
#endif //GAME_STATE_WORLDSTATE_INCLUDED
