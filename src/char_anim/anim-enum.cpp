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
// anim-enum.cpp
//
#include "anim-enum.hpp"

#include "game/game-data-file.hpp"

#include <boost/algorithm/string.hpp>

#include <exception>
#include <sstream>


namespace heroespath
{
namespace char_anim
{
    
    const std::string Anim::ToString(const NameEnum E)
    {
        switch (E)
        {
            case Metal:         { return "Metal"; }
            case Puck:          { return "Puck"; }
            case Sara:          { return "Sara"; }
            case Rags:          { return "Rags"; }
            case Robes:         { return "Robes"; }
            case Whitebeard:    { return "Whitebeard"; }
            default:
            {
                std::ostringstream ss;
                ss << "char_anim::Anim::ToString(NameEnum=" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    bool Anim::IsPlayer(const NameEnum E)
    {
        return ((E == Metal) || (E == Puck) || (E == Sara));
    }


    const std::vector<Anim::Enum> Anim::Animations(const NameEnum E)
    {
        switch (E)
        {
            case Metal:
            {
                return {
                    Anim::Metal_Female_Dark,
                    Anim::Metal_Female_Light,
                    Anim::Metal_Male_Dark,
                    Anim::Metal_Male_Light };
            }
            case Puck:
            {
                return {
                    Anim::Puck_Female_Dark,
                    Anim::Puck_Female_Light,
                    Anim::Puck_Male_Dark,
                    Anim::Puck_Male_Light };
            }
            case Sara:
            {
                return {
                    Anim::Sara_BlueWhite,
                    Anim::Sara_Cyan,
                    Anim::Sara_Green,
                    Anim::Sara_White };
            }
            case Rags:
            {
                return {
                    Anim::Rags_Dark,
                    Anim::Rags_Tan1,
                    Anim::Rags_Tan2 };
            }
            case Robes:
            {
                return {
                    Anim::Robes_Light_Blonde,
                    Anim::Robes_Light_Crow,
                    Anim::Robes_Tan1_Blonde,
                    Anim::Robes_Tan1_Crow,
                    Anim::Robes_Tan1_CrowLong,
                    Anim::Robes_Tan1_Red,
                    Anim::Robes_Tan2_BrunLong,
                    Anim::Robes_Tan2_Crow };
            }
            case Whitebeard:
            {
                return { Anim::Robes_Whitebeard };
            }
            default:
            {
                std::ostringstream ss;
                ss << "char_anim::Anim::Animations(NameEnum=" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    const std::string Anim::ToString(const Anim::Enum E)
    {
        switch (E)
        {
            case Metal_Female_Dark:     { return "Metal-Female-Dark"; }
            case Metal_Female_Light:    { return "Metal-Female-Light"; }
            case Metal_Male_Dark:       { return "Metal-Male-Dark"; }
            case Metal_Male_Light:      { return "Metal-Male-Light"; }
            case Puck_Female_Dark:      { return "Puck-Female-Dark"; }
            case Puck_Female_Light:     { return "Puck-Female-Light"; }
            case Puck_Male_Dark:        { return "Puck-Male-Dark"; }
            case Puck_Male_Light:       { return "Puck-Male-Light"; }
            case Sara_BlueWhite:        { return "Sara-BlueWhite"; }
            case Sara_Cyan:             { return "Sara-Cyan"; }
            case Sara_Green:            { return "Sara-Green"; }
            case Sara_White:            { return "Sara-White"; }
            case Rags_Dark:             { return "Rags-Dark"; }
            case Rags_Tan1:             { return "Rags-Tan1"; }
            case Rags_Tan2:             { return "Rags-Tan2"; }
            case Robes_Light_Blonde:    { return "Robes-Light-Blonde"; }
            case Robes_Light_Crow:      { return "Robes-Light-Crow"; }
            case Robes_Tan1_Blonde:     { return "Robes-Tan1-Blonde"; }
            case Robes_Tan1_Crow:       { return "Robes-Tan1-Crow"; }
            case Robes_Tan1_CrowLong:   { return "Robes-Tan1-CrowLong"; }
            case Robes_Tan1_Red:        { return "Robes-Tan1-Red"; }
            case Robes_Tan2_BrunLong:   { return "Robes-Tan2-BrunLong"; }
            case Robes_Tan2_Crow:       { return "Robes-Tan2-Crow"; }
            case Robes_Whitebeard:      { return "Robes-Whitebeard"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "char_anim::Anim::ToString(Enum=" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    Anim::NameEnum Anim::Name(const Anim::Enum E)
    {
        switch (E)
        {
            case Metal_Female_Dark:
            case Metal_Female_Light:
            case Metal_Male_Dark:
            case Metal_Male_Light:      { return NameEnum::Metal; }
            case Puck_Female_Dark:
            case Puck_Female_Light:
            case Puck_Male_Dark:
            case Puck_Male_Light:       { return NameEnum::Puck; }
            case Sara_BlueWhite:
            case Sara_Cyan:
            case Sara_Green:
            case Sara_White:            { return NameEnum::Sara; }
            case Rags_Dark:
            case Rags_Tan1:
            case Rags_Tan2:             { return NameEnum::Rags; }
            case Robes_Light_Blonde:
            case Robes_Light_Crow:
            case Robes_Tan1_Blonde:
            case Robes_Tan1_Crow:
            case Robes_Tan1_CrowLong:
            case Robes_Tan1_Red:
            case Robes_Tan2_BrunLong:
            case Robes_Tan2_Crow:       { return NameEnum::Robes; }
            case Robes_Whitebeard:      { return NameEnum::Whitebeard; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "char_anim::Anim::Name(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    bool Anim::IsPlayer(const Anim::Enum E)
    {
        return ((E >= Anim::Player_First) && (E <= Anim::Player_Last));
    }


    const std::string Anim::ImagePath(const Anim::Enum E)
    {
        std::string keyStr{ ((Anim::IsPlayer(E)) ?
            game::GameDataFile::Instance()->GetMediaPath("media-images-character-player") :
            game::GameDataFile::Instance()->GetMediaPath("media-images-character-nonplayer")) };

        keyStr += boost::algorithm::to_lower_copy( Anim::ToString(E) );
        keyStr += ".png";

        return keyStr;
    }

}
}
