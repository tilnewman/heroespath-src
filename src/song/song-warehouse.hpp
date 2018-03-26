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
#ifndef HEROESPATH_SONG_SONGWAREHOUSE_HPP_INCLUDED
#define HEROESPATH_SONG_SONGWAREHOUSE_HPP_INCLUDED
//
// song-warehouse.hpp
//
#include "misc/not-null.hpp"
#include "song/song-enum.hpp"

#include <memory>
#include <vector>

namespace heroespath
{
namespace song
{

    // forward declarations
    class Song;
    using SongPtr_t = misc::NotNull<Song *>;
    using SongUPtr_t = std::unique_ptr<Song>;
    using SongUVec_t = std::vector<SongUPtr_t>;

    // Responsible for pre-game-start creation and testing of all spells, and
    // then providing access to them.
    struct Warehouse
    {
        static void Fill();
        static void Empty();
        static bool Test();
        static const SongPtr_t Get(const Songs::Enum);

    private:
        static SongUVec_t songsUVec_;
    };

} // namespace song
} // namespace heroespath

#endif // HEROESPATH_SONG_SONGWAREHOUSE_HPP_INCLUDED
