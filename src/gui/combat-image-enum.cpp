// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// combat-image-enum.hpp
//
#include "combat-image-enum.hpp"

#include "misc/config-file.hpp"
#include "misc/filesystem.hpp"
#include "misc/log-macros.hpp"

namespace heroespath
{
namespace gui
{

    const std::string CombatImageType::ToString(const Enum ENUM)
    {
        switch (ENUM)
        {
            case Wing:
            {
                return "Wing";
            }
            case Arrow1:
            {
                return "Arrow1";
            }
            case Arrow2:
            {
                return "Arrow2";
            }
            case Arrow3:
            {
                return "Arrow3";
            }
            case Bolt:
            {
                return "Bolt";
            }
            case Dart:
            {
                return "Dart";
            }
            case Stone1:
            {
                return "Stone1";
            }
            case Stone2:
            {
                return "Stone2";
            }
            case Stone3:
            {
                return "Stone3";
            }
            case Stone4:
            {
                return "Stone4";
            }
            case Count:
            {
                return "(Count)";
            }
            default:
            {
                M_HP_LOG_ERR(
                    "enum_value=" << static_cast<EnumUnderlying_t>(ENUM) << " is invalid. (count="
                                  << static_cast<EnumUnderlying_t>(Count) << ")");

                return "";
            }
        }
    }

    const std::string CombatImageType::ImageFilename(const Enum ENUM)
    {
        switch (ENUM)
        {
            case Wing:
            {
                return "wing.png";
            }
            case Arrow1:
            {
                return "arrow1.png";
            }
            case Arrow2:
            {
                return "arrow2.png";
            }
            case Arrow3:
            {
                return "arrow3.png";
            }
            case Bolt:
            {
                return "arrow4.png";
            }
            case Dart:
            {
                return "dart.png";
            }
            case Stone1:
            {
                return "stone1.png";
            }
            case Stone2:
            {
                return "stone2.png";
            }
            case Stone3:
            {
                return "stone3.png";
            }
            case Stone4:
            {
                return "stone4.png";
            }
            case Count:
            {
                return "(Count)";
            }
            default:
            {
                M_HP_LOG_ERR(
                    "enum_value=" << static_cast<EnumUnderlying_t>(ENUM) << " is invalid. (count="
                                  << static_cast<EnumUnderlying_t>(Count) << ")");

                return "";
            }
        }
    }

    const std::string CombatImageType::ImageDirectory()
    {
        return misc::ConfigFile::Instance()->GetMediaPath("media-images-combat-dir");
    }

    const std::string CombatImageType::ImagePath(const Enum ENUM)
    {
        return misc::filesystem::CombinePathsThenClean(ImageDirectory(), ImageFilename(ENUM));
    }

} // namespace gui
} // namespace heroespath