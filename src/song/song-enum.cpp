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

#include "game/game-data-file.hpp"
#include "misc/boost-string-includes.hpp"

#include <exception>
#include <sstream>

namespace heroespath
{
namespace song
{

    const std::string Songs::ToString(const Enum E)
    {
        switch (E)
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
            default:
            {
                ThrowInvalidValueForFunction(E, "ToString");
            }
        }
    }

    const std::string Songs::Name(const Enum E)
    {
        switch (E)
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
            default:
            {
                ThrowInvalidValueForFunction(E, "Name");
            }
        }
    }

    const std::string Songs::ShortDesc(const Songs::Enum E)
    {
        std::ostringstream keySS;
        keySS << "heroespath-song-" << ToString(E) << "-short-desc";
        return game::GameDataFile::Instance()->GetCopyStr(keySS.str());
    }

    const std::string Songs::ExtraDesc(const Songs::Enum E)
    {
        std::ostringstream keySS;
        keySS << "heroespath-song-" << ToString(E) << "-extra-desc";
        return game::GameDataFile::Instance()->GetCopyStr(keySS.str());
    }

    const std::string Songs::ImageFilename(const Songs::Enum E)
    {
        return boost::algorithm::to_lower_copy(ToString(E) + ".png");
    }

} // namespace song
} // namespace heroespath
