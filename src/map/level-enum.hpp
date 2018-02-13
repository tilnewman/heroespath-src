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
#ifndef HEROESPATH_MAP_LEVELENUM_HPP_INCLUDED
#define HEROESPATH_MAP_LEVELENUM_HPP_INCLUDED
//
// level-enum.hpp
//
#include <string>


namespace heroespath
{
namespace map
{

    struct Level
    {
        enum Enum
        {
            Thornberry = 0,
            Thornberry_GuardPostWest,
            Thornberry_GuardPostEast,
            ThornberryMeadows,
            ThornberryHighlands,
            Mudgate,
            Bridgeway,
            Count
        };

        static const std::string FILENAME_EXTENSION;
        static const std::string ToString(const Level::Enum);
        static Level::Enum FromString(const std::string &);
        static const std::string ToFilename(const Level::Enum);
    };

}
}

#endif //HEROESPATH_MAP_LEVELENUM_HPP_INCLUDED