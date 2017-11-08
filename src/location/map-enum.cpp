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
// map-enum.cpp
//
#include "map-enum.hpp"
#include <exception>
#include <sstream>


namespace heroespath
{
namespace location
{

    const std::string map::MAP_FILENAME_EXT(".tmx");


    const std::string map::ToString(const map::Enum E)
    {
        switch (E)
        {
            case Thornberry:        { return "Thornberry"; }
            case ThornberryForest:  { return "ThronberryForest"; }
            case Mudgate:           { return "Mudgate"; }
            case Bridgeway:         { return "Bridgeway"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "location::map::Enum::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    const std::string map::MapFilename(const map::Enum E)
    {
        std::ostringstream ss;
        ss << ToString(E) << MAP_FILENAME_EXT;
        return ss.str();
    }

}
}
