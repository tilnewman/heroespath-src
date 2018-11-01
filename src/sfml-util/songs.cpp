// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// songs.cpp
//
#include "songs.hpp"

#include "sfml-util/music-operator.hpp"
#include "sfml-util/music-set.hpp"

#include <tuple>

namespace heroespath
{
namespace gui
{

    SongSet::SongSet(const MusicSet & SET, MusicOperator OP)
        : set(SET)
        , op(std::move(OP))
    {}

    bool operator==(const SongSet & L, const SongSet & R)
    {
        return std::tie(L.set, L.op) == std::tie(R.set, R.op);
    }

} // namespace gui
} // namespace heroespath
