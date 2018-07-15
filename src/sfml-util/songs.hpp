// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_SONGS_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_SONGS_HPP_INCLUDED
//
// songs.hpp
//
#include "sfml-util/music-operator.hpp"
#include "sfml-util/music-set.hpp"

#include <memory>
#include <vector>

namespace heroespath
{
namespace sfml_util
{

    // Responsible for wrapping a set of songs as an enum vector and a single sf::Music instance.
    struct SongSet
    {
        explicit SongSet(const MusicSet & SET = MusicSet(), MusicOperator OP = MusicOperator());

        friend bool operator==(const SongSet & L, const SongSet & R);

        bool IsValid() const { return (op.IsValid() && set.IsValid()); }

        MusicSet set;
        MusicOperator op;
    };

    bool operator==(const SongSet & L, const SongSet & R);

    inline bool operator!=(const SongSet & L, const SongSet & R) { return !(L == R); }

    using SongSetVec_t = std::vector<SongSet>;

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_SONGS_HPP_INCLUDED
