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
#ifndef HEROESPATH_LOCATION_ILOCATION_HPP_INCLUDED
#define HEROESPATH_LOCATION_ILOCATION_HPP_INCLUDED
//
// i-location.hpp
//  An interface for all Location objects
//
#include "sfml-util/sfml-graphics.hpp"
#include "location/map-enum.hpp"
#include <string>


namespace heroespath
{
namespace location
{

    //an interface for all Location objects
    struct ILocation
    {
        virtual ~ILocation() {}

        virtual const std::string Name() const = 0;

        virtual map::Enum Map() const = 0;
        virtual void MapSet(const map::Enum) = 0;

        virtual const std::string MapFilename() const = 0;

        virtual const sf::Vector2f MapPosOffset() const = 0;
        virtual void MapPosOffset(const float OFFSET_X, const float OFFSET_Y) = 0;
    };

    using ILocationSPtr_t = std::shared_ptr<ILocation>;

}
}

#endif //HEROESPATH_LOCATION_ILOCATION_HPP_INCLUDED
