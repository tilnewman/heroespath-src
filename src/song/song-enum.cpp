// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// song-enum.cpp
//
#include "song-enum.hpp"

#include "misc/config-file.hpp"
#include "misc/filesystem.hpp"
#include "misc/log-macros.hpp"
#include "misc/strings.hpp"

namespace heroespath
{
namespace song
{

    const std::string Songs::ToString(const Enum ENUM)
    {
        switch (ENUM)
        {
            case RallyDrum:
            {
                return "RallyDrum";
            }
            case SpiritResonance:
            {
                return "SpiritResonance";
            }
            case RousingRhythm:
            {
                return "RousingRhythm";
            }
            case TripBeat:
            {
                return "TripBeat";
            }
            case PanicStrings:
            {
                return "PanicStrings";
            }
            case Lullaby:
            {
                return "Lullaby";
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

    const std::string Songs::Name(const Enum ENUM)
    {
        switch (ENUM)
        {
            case RallyDrum:
            {
                return "Rally Drum";
            }
            case SpiritResonance:
            {
                return "Spirit Resonance";
            }
            case RousingRhythm:
            {
                return "Rousing Rhythm";
            }
            case TripBeat:
            {
                return "Trip Beat";
            }
            case PanicStrings:
            {
                return "Panic Strings";
            }
            case Lullaby:
            {
                return "Lullaby";
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

    const std::string Songs::ShortDesc(const Songs::Enum ENUM)
    {
        std::ostringstream keySS;
        keySS << "heroespath-song-" << ToString(ENUM) << "-short-desc";
        return misc::ConfigFile::Instance()->Value(keySS.str());
    }

    const std::string Songs::ExtraDesc(const Songs::Enum ENUM)
    {
        std::ostringstream keySS;
        keySS << "heroespath-song-" << ToString(ENUM) << "-extra-desc";
        return misc::ConfigFile::Instance()->Value(keySS.str());
    }

    const std::string Songs::ImageFilename(const Songs::Enum ENUM)
    {
        return misc::ToLowerCopy(ToString(ENUM) + ".png");
    }

    const std::string Songs::ImageDirectory()
    {
        return misc::ConfigFile::Instance()->GetMediaPath("media-images-songs-dir");
    }

    const std::string Songs::ImagePath(const Enum ENUM)
    {
        return misc::filesystem::CombinePathsThenClean(ImageDirectory(), ImageFilename(ENUM));
    }

} // namespace song
} // namespace heroespath
