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
#ifndef GAME_CREATURE_SEXENUM_HPP_INCLUDED
#define GAME_CREATURE_SEXENUM_HPP_INCLUDED
//
// sex-enum.hpp
//  An enumeration of all sexes.
//
#include <string>


namespace game
{
namespace creature
{

    struct sex
    {
        enum Enum
        {
            Unknown = 0, //some creatures will be too bizarre to be either male or female
            Male,
            Female,
            Count
        };

        static const std::string ToString(const sex::Enum E);

        static bool IsValid(const sex::Enum E);

        static const std::string HeSheIt(const sex::Enum E, const bool WILL_CAPITALIZE);

        static const std::string HisHerIts(const sex::Enum E,
                                           const bool      WILL_CAPITALIZE,
                                           const bool      WILL_POSSESSIVE_HER);

        static const std::string HimHerIt(const sex::Enum E, const bool WILL_CAPITALIZE);
    };

}
}
#endif //GAME_CREATURE_SEXENUM_HPP_INCLUDED
