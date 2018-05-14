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

#include "game/game-data-file.hpp"

namespace heroespath
{
namespace map
{

    const std::string LevelType::ToString(const LevelType::Enum LEVEL_TYPE)
    {
        switch (LEVEL_TYPE)
        {
            case Town:
            {
                return "Town";
            }
            case Hideaway:
            {
                return "Hideaway";
            }
            case Inside:
            {
                return "Inside";
            }
            case Realm:
            {
                return "Realm";
            }
            case Count:
            default:
            {
                ThrowInvalidValueForFunction(LEVEL_TYPE, "ToString");
            }
        }
    }

    const std::string Level::FILENAME_EXTENSION{ ".tmx" };

    const std::string Level::ToString(const Level::Enum LEVEL)
    {
        switch (LEVEL)
        {
            case Thornberry:
            {
                return "Thornberry";
            }
            case Thornberry_GuardPostWest:
            {
                return "Thornberry_GuardPostWest";
            }
            case Thornberry_GuardPostEast:
            {
                return "Thornberry_GuardPostEast";
            }
            case ThornberryMeadows:
            {
                return "ThornberryMeadows";
            }
            case ThornberryHighlands:
            {
                return "ThornberryHighlands";
            }
            case Count:
            default:
            {
                ThrowInvalidValueForFunction(LEVEL, "ToString");
            }
        }
    }

    const std::string Level::Path(const Level::Enum LEVEL)
    {
        return game::GameDataFile::Instance()->GetMediaPath("media-maps-dir") + ToString(LEVEL)
            + FILENAME_EXTENSION;
    }

    LevelType::Enum Level::Type(const Level::Enum LEVEL)
    {
        switch (LEVEL)
        {
            case Thornberry:
            {
                return LevelType::Town;
            }
            case Thornberry_GuardPostWest:
            {
                return LevelType::Inside;
            }
            case Thornberry_GuardPostEast:
            {
                return LevelType::Inside;
            }
            case ThornberryMeadows:
            {
                return LevelType::Realm;
            }
            case ThornberryHighlands:
            {
                return LevelType::Realm;
            }
            case Count:
            default:
            {
                ThrowInvalidValueForFunction(LEVEL, "Type");
            }
        }
    }

} // namespace map
} // namespace heroespath
