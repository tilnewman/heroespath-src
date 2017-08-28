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
#ifndef SFMLUTIL_SONGS_HPP_INCLUDED
#define SFMLUTIL_SONGS_HPP_INCLUDED
//
// songs.hpp
//
#include "sfml-util/music-set.hpp"

#include <memory>


namespace sfml_util
{

    class MusicOperator;
    using MusicOperatorSPtr_t = std::shared_ptr<MusicOperator>;


    //Responsible for wrapping a set of songs as an enum vector and a single sf::Music instance.
    struct Songs
    {
        explicit Songs(const MusicSet &            SET     = MusicSet(),
                       const MusicOperatorSPtr_t & OP_SPTR = MusicOperatorSPtr_t());

        friend bool operator==(const Songs & L, const Songs & R);

        MusicSet set;
        MusicOperatorSPtr_t op_sptr;
    };

    bool operator==(const Songs & L, const Songs & R);

    inline bool operator!=(const Songs & L, const Songs & R)
    {
        return ! (L == R);
    }

    using SongsSPtr_t = std::shared_ptr<Songs>;
    using SongsSVec_t = std::vector<SongsSPtr_t>;

}

#endif //SFMLUTIL_SONGS_HPP_INCLUDED
