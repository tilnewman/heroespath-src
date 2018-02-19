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
#ifndef HEROESPATH_CHARANIM_ANIMENUM_HPP_INCLUDED
#define HEROESPATH_CHARANIM_ANIMENUM_HPP_INCLUDED
//
// anim-enum.hpp
//
#include <string>
#include <vector>


namespace heroespath
{
namespace char_anim
{

    struct Anim
    {
        enum NameEnum
        {
            Metal,
            Puck,
            Sara,
            Rags,
            Robes,
            Whitebeard
        };

        enum Enum
        {
            Player_First = 0,
            Metal_Female_Dark = Player_First,
            Metal_Female_Light,
            Metal_Male_Dark,
            Metal_Male_Light,
            Puck_Female_Dark,
            Puck_Female_Light,
            Puck_Male_Dark,
            Puck_Male_Light,
            Sara_BlueWhite,
            Sara_Cyan,
            Sara_Green,
            Sara_White,
            Player_Last = Sara_White,
            NonPlayer_First,
            Rags_Dark = NonPlayer_First,
            Rags_Tan1,
            Rags_Tan2,
            Robes_Light_Blonde,
            Robes_Light_Crow,
            Robes_Tan1_Blonde,
            Robes_Tan1_Crow,
            Robes_Tan1_CrowLong,
            Robes_Tan1_Red,
            Robes_Tan2_BrunLong,
            Robes_Tan2_Crow,
            Robes_Whitebeard,
            NonPlayer_Last,
            Count = NonPlayer_Last
        };

        static const std::string ToString(const NameEnum);
        static bool IsPlayer(const NameEnum);
        static const std::vector<Enum> Animations(const NameEnum);

        static const std::string ToString(const Enum);
        static NameEnum Name(const Enum);
        static bool IsPlayer(const Enum);
        static const std::string ImagePath(const Enum);
    };

}
}

#endif //HEROESPATH_CHARANIM_ANIMENUM_HPP_INCLUDED
