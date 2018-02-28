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
#ifndef HEROESPATH_STATE_LEVEL_HPP_INCLUDED
#define HEROESPATH_STATE_LEVEL_HPP_INCLUDED
//
// level.hpp
//
#include "map/level-enum.hpp"
#include "misc/boost-serialize-includes.hpp"
#include "misc/vector-map.hpp"

#include <memory>


namespace heroespath
{
namespace state
{

    //Responsible for storing all states that represent
    //the game world contained in a specific level.
    class Level
    {
        Level(const Level &) = delete;
        Level & operator=(const Level &) = delete;

    public:
        Level(const map::Level::Enum = map::Level::Enum::Count);

        inline map::Level::Enum WhichLevel() const { return level_; }

        bool IsDoorLocked(const map::Level::Enum) const;

    private:
        map::Level::Enum level_;
        misc::VectorMap<map::Level::Enum, bool> doorLockMap_;

    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & level_;
            ar & doorLockMap_;
        }
    };

}
}

#endif //HEROESPATH_STATE_LEVEL_HPP_INCLUDED