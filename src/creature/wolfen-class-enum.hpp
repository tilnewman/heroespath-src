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
#ifndef HEROESPATH_CREATURE_WOLFENCLASSENUM_HPP_INCLUDED
#define HEROESPATH_CREATURE_WOLFENCLASSENUM_HPP_INCLUDED
//
// wolfen-class-enum.hpp
//  An enumeration of all Wolfen classes as determined by Rank.
//
#include "misc/types.hpp"

#include <string>

namespace heroespath
{
namespace creature
{

    struct wolfen_class
    {
        enum Enum
        {
            Pup = 0,
            Juvenile,
            Adult,
            Noble,
            Highborn,
            Elder,
            Count
        };

        static const std::string Name(const wolfen_class::Enum);
        static const std::string Desc(const wolfen_class::Enum);
        inline static const std::string ToString(const wolfen_class::Enum E) { return Name(E); }
        static wolfen_class::Enum ClassFromRank(const Rank_t &);
    };
}
}

#endif // HEROESPATH_CREATURE_WOLFENCLASSENUM_HPP_INCLUDED
