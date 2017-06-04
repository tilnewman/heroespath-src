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
#ifndef SFMLUTIL_MUSICSET_INCLUDED
#define SFMLUTIL_MUSICSET_INCLUDED
//
// music-set.hpp
//  A class that defines a set of music that is played on after the other.
//
#include "sfml-util/music-enum.hpp"
#include "sfml-util/music-operator.hpp"

#include <memory>
#include <vector>
#include <tuple>


namespace sfml_util
{

    //Everything needed to describe a list of songs, and in which order to play them.
    //If there is more than one song, then a song will never repeat.
    //After construction, the currently playing member is already set to what should play first.
    class MusicSet
    {
    public:
        explicit MusicSet(const music::Enum WHICH_SINGLE_MUSIC,
                          const bool        WILL_LOOP,
                          const float       FADE_MULT          = MusicOperator::FADE_MULT_DEFAULT_IN_,
                          const float       VOLUME             = MusicOperator::VOLUME_USE_GLOBAL_);

        explicit MusicSet(const MusicEnumVec_t & MUSIC_SET_VEC,
                          const bool             WILL_RANDOMIZE       = true,
                          const bool             WILL_START_AT_RANDOM = true,
                          const float            FADE_MULT            = MusicOperator::FADE_MULT_DEFAULT_IN_,
                          const float            VOLUME               = MusicOperator::VOLUME_USE_GLOBAL_,
                          const bool             WILL_LOOP            = true);

        inline std::size_t Count() const                  { return whichVec_.size(); }
        inline bool IsSingleSong() const             { return (whichVec_.size() == 1); }

        inline bool WillLoop() const                 { return willLoop_; }
        inline void WillLoop(const bool B)           { willLoop_ = B; }

        inline music::Enum CurrentlyPlaying() const  { return currentlyPlaying_; }
        inline music::Enum PreviouslyPlaying() const { return previouslyPlaying_; }

        inline bool IsRandom() const                 { return willRandomize_; }
        inline void IsRandom(const bool B)           { willRandomize_ = B; }

        inline float FadeInMult() const              { return fadeInMult_; }
        inline float Volume() const                  { return volume_; }

        //returns the next song to play  (result of NextToPlay())
        music::Enum Advance();

        //if there is more than one song then songs never repeat
        music::Enum PickNextSong();

        bool Contains(const music::Enum);

        friend bool operator==(const MusicSet & L, const MusicSet & R);

    private:
        MusicEnumVec_t whichVec_;
        music::Enum    currentlyPlaying_;
        music::Enum    previouslyPlaying_;
        bool           willRandomize_;
        float          fadeInMult_;
        float          volume_;
        bool           willLoop_;
    };

    using MusicSetSPtr_t = std::shared_ptr<MusicSet>;
    using MusicSetSVec_t = std::vector<MusicSetSPtr_t>;


    bool operator==(const MusicSet & L, const MusicSet & R);

    inline bool operator!=(const MusicSet & L, const MusicSet & R)
    {
        return ! (L == R);
    }

}

#endif //SFMLUTIL_MUSICSET_INCLUDED
