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

        inline bool IsValid() const { return (op.IsValid() && set.IsValid()); }

        MusicSet set;
        MusicOperator op;
    };

    bool operator==(const SongSet & L, const SongSet & R);

    inline bool operator!=(const SongSet & L, const SongSet & R) { return !(L == R); }

    using SongSetVec_t = std::vector<SongSet>;
}
}

#endif // HEROESPATH_SFMLUTIL_SONGS_HPP_INCLUDED
