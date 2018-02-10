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
// level-enum.cpp
//
#include "level-enum.hpp"

#include <exception>
#include <sstream>

#include <boost/algorithm/string.hpp>


namespace heroespath
{
namespace map
{

    const std::string Level::FILENAME_EXTENSION{ ".tmx" };


    const std::string Level::ToString(const Level::Enum E)
    {
        switch (E)
        {
            case Thornberry:                { return "Thornberry"; }
            case Thornberry_GuardPostWest:  { return "Thornberry_GuardPostWest"; }
            case Thornberry_GuardPostEast:  { return "Thornberry_GuardPostEast"; }
            case ThornberryMeadows:         { return "ThornberryMeadows"; }
            case ThornberryHighlands:       { return "ThornberryHighlands"; }
            case Mudgate:                   { return "Mudgate"; }
            case Bridgeway:                 { return "Bridgeway"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "map::Level::Enum::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    const std::string Level::ToFilename(const Level::Enum E)
    {
        return ToString(E) + FILENAME_EXTENSION;
    }


    Level::Enum Level::FromString(const std::string & NAME)
    {
        namespace ba = boost::algorithm;

        for (int i(0); i < Level::Count; ++i)
        {
            auto const ENUM{ static_cast<Level::Enum>(i) };

            if (ba::to_lower_copy(ToString(ENUM)) == ba::to_lower_copy(NAME))
            {
                return ENUM;
            }
        }

        std::ostringstream ss;
        ss << "map::Level::Enum::FromString(\"" << NAME << "\")_InvalidValueError.";
        throw std::runtime_error(ss.str());
    }

}
}
