// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// level-enum.cpp
//
#include "level-enum.hpp"

#include "game/game-data-file.hpp"
#include "misc/filesystem.hpp"

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

    const std::string Level::FILENAME_EXTENSION { ".tmx" };

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
        return misc::filesystem::CombinePathsThenClean(
            game::GameDataFile::Instance()->GetMediaPath("media-maps-dir"),
            ToString(LEVEL) + FILENAME_EXTENSION);
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
