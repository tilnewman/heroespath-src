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

#include "misc/config-file.hpp"
#include "misc/filesystem.hpp"
#include "misc/log-macros.hpp"

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
            {
                return "(Count)";
            }
            default:
            {
                M_HP_LOG_ERR(
                    "enum_value=" << static_cast<EnumUnderlying_t>(LEVEL_TYPE)
                                  << " is invalid. (count=" << static_cast<EnumUnderlying_t>(Count)
                                  << ")");

                return "";
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
            {
                return "(Count)";
            }
            default:
            {
                M_HP_LOG_ERR(
                    "enum_value=" << static_cast<EnumUnderlying_t>(LEVEL) << " is invalid. (count="
                                  << static_cast<EnumUnderlying_t>(Count) << ")");

                return "";
            }
        }
    }

    const std::string Level::Path(const Level::Enum LEVEL)
    {
        static std::string dirPath;

        if (dirPath.empty())
        {
            dirPath = misc::ConfigFile::Instance()->GetMediaPath("media-maps-dir");
        }

        return misc::filesystem::CombinePaths(dirPath, ToString(LEVEL) + FILENAME_EXTENSION);
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
                M_HP_LOG_ERR(
                    "enum_value=" << static_cast<EnumUnderlying_t>(LEVEL) << " is invalid. (count="
                                  << static_cast<EnumUnderlying_t>(Count) << ")");

                return LevelType::Count;
            }
        }
    }

} // namespace map
} // namespace heroespath
