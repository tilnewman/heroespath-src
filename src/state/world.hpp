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
#ifndef HEROESPATH_STATE_WORLD_HPP_INCLUDED
#define HEROESPATH_STATE_WORLD_HPP_INCLUDED
//
// world.hpp
//  A class that represents the entire state of the game world.
//
#include "misc/boost-serialize-includes.hpp"
#include "state/maps.hpp"

#include <memory>

namespace heroespath
{
namespace state
{

    // Encapsulates all states that describe the game world.
    class World
    {
        World(const World &) = delete;
        World & operator=(const World &) = delete;

    public:
        World();

        inline Maps & GetMaps() { return maps_; }
        inline std::size_t EncounterCount() const { return encounterCount_; }
        inline void EncounterCountInc() { ++encounterCount_; }

    private:
        // TODO quests
        // TODO events
        Maps maps_;
        std::size_t encounterCount_;

    private:
        friend class boost::serialization::access;
        template <typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & encounterCount_;
        }
    };

    using WorldUPtr_t = std::unique_ptr<World>;
}
}

#endif // HEROESPATH_STATE_WORLDcd_HPP_INCLUDED
