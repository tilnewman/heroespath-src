// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
//
// music-set.cpp
//
#include "music-set.hpp"

#include "misc/random.hpp"
#include "misc/vectors.hpp"
#include "misc/assertlogandthrow.hpp"


namespace heroespath
{
namespace sfml_util
{

    MusicSet::MusicSet(const music::Enum WHICH_MUSIC_ENUM,
                       const bool        WILL_LOOP,
                       const float       FADE_MULT,
                       const float       VOLUME)
    :
        whichVec_          (1, WHICH_MUSIC_ENUM),
        currentlyPlaying_  (WHICH_MUSIC_ENUM),
        previouslyPlaying_ (WHICH_MUSIC_ENUM),
        willRandomize_     (false),
        fadeInMult_        (FADE_MULT),
        volume_            (VOLUME),
        willLoop_          (WILL_LOOP)
    {}


    MusicSet::MusicSet(const MusicEnumVec_t & MUSIC_ENUM_VEC,
                       const bool             WILL_RANDOMIZE,
                       const bool             WILL_START_AT_RANDOM,
                       const float            FADE_MULT,
                       const float            VOLUME,
                       const bool             WILL_LOOP)
    :
        whichVec_          (MUSIC_ENUM_VEC),
        currentlyPlaying_  (music::None),
        previouslyPlaying_ (music::None),
        willRandomize_     (WILL_RANDOMIZE),
        fadeInMult_        (FADE_MULT),
        volume_            (VOLUME),
        willLoop_          (WILL_LOOP)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((whichVec_.empty() == false),
            "sfml_util::MusicSet::Constructor(vector version) was given an empty vector.");

        auto const ORIG_RANDOM_SETTING{ willRandomize_ };
        willRandomize_ = WILL_START_AT_RANDOM;

        auto const ORIG_LOOP_SETTING{ willLoop_ };
        willLoop_ = true;

        currentlyPlaying_ = PickNextSong();

        willRandomize_ = ORIG_RANDOM_SETTING;
        willLoop_ = ORIG_LOOP_SETTING;
    }


    bool MusicSet::IsValid() const
    {
        return ((whichVec_.empty() == false) &&
                (Contains(music::None) == false) &&
                (Contains(music::All) == false) &&
                (Contains(music::Count) == false));
    }


    music::Enum MusicSet::Advance()
    {
        auto const PREV_PLAYING{ currentlyPlaying_ };
        currentlyPlaying_ = PickNextSong();
        previouslyPlaying_ = PREV_PLAYING;
        return currentlyPlaying_;
    }


    music::Enum MusicSet::PickNextSong()
    {
        if (whichVec_.size() == 1)
        {
            if (willLoop_)
            {
                return whichVec_[0];
            }
            else
            {
                return music::None;
            }
        }

        if (willRandomize_)
        {
            MusicEnumVec_t possibleSongs{ whichVec_ };

            whichVec_.erase(std::remove(whichVec_.begin(),
                                        whichVec_.end(),
                                        currentlyPlaying_), whichVec_.end());

            return heroespath::misc::Vector::SelectRandom(possibleSongs);
        }
        else
        {
            auto currentlyPlayingIter{ std::find(whichVec_.begin(),
                                                 whichVec_.end(),
                                                 currentlyPlaying_) };

            if (++currentlyPlayingIter == whichVec_.end())
            {
                if (willLoop_)
                {
                    return * whichVec_.begin();
                }
                else
                {
                    return music::None;
                }
            }
            else
            {
                return * currentlyPlayingIter;
            }
        }
    }


    bool operator==(const MusicSet & L, const MusicSet & R)
    {
        //The whichVec_ member is ordered, so it can be compared in the usual way.

        return (std::tie(L.whichVec_,
                         L.currentlyPlaying_,
                         L.previouslyPlaying_,
                         L.willRandomize_,
                         L.fadeInMult_,
                         L.volume_)
            ==
            std::tie(R.whichVec_,
                     R.currentlyPlaying_,
                     R.previouslyPlaying_,
                     R.willRandomize_,
                     R.fadeInMult_,
                     R.volume_));
    }

}
}
