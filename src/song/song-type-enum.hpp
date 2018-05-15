// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SONG_SONGTYPEENUM_HPP_INCLUDED
#define HEROESPATH_SONG_SONGTYPEENUM_HPP_INCLUDED
//
// song-type-enum.hpp
//
#include <string>

namespace heroespath
{
namespace song
{

    struct SongType
    {
        enum Enum
        {
            Guitar = 0,
            Drum,
            Count
        };

        static const std::string ToString(const Enum);
    };
} // namespace song
} // namespace heroespath

#endif // HEROESPATH_SONG_SONGTYPEENUM_HPP_INCLUDED
