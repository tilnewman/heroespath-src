// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SONG_SONGENUM_HPP_INCLUDED
#define HEROESPATH_SONG_SONGENUM_HPP_INCLUDED
//
// song-enum.hpp
//
#include "misc/enum-common.hpp"

#include <string>
#include <vector>

namespace heroespath
{
namespace song
{

    // Responsible for identifying all songs that Bards can play in the game
    struct Songs : public EnumBaseCounting<>
    {
        enum Enum : EnumUnderlying_t
        {
            RallyDrum = 0,
            SpiritResonance,
            RousingRhythm,
            TripBeat,
            PanicStrings,
            Lullaby,
            Count
        };

        static constexpr std::string_view Name(const Enum ENUM) noexcept
        {
            switch (ENUM)
            {
                case RallyDrum: return "Rally Drum";
                case SpiritResonance: return "Spirit Resonance";
                case RousingRhythm: return "Rousing Rhythm";
                case TripBeat: return "Trip Beat";
                case PanicStrings: return "Panic Strings";
                case Lullaby: return "Lullaby";
                case Count: return "Count";
                default: return "song::Songs::Name(ENUM=out_of_range)";
            }
        }

        static const std::string ShortDesc(const Enum);
        static const std::string ExtraDesc(const Enum);
        static const std::string ImageFilename(const Enum);
        static const std::string ImageDirectory();
        static const std::string ImagePath(const Enum);
    };

    using SongEnumVec_t = std::vector<Songs::Enum>;

} // namespace song
} // namespace heroespath

#endif // HEROESPATH_SONG_SONGENUM_HPP_INCLUDED
