// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_MUSICSET_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_MUSICSET_HPP_INCLUDED
//
// music-set.hpp
//  A class that keeps a set of songs and presents an interface to interact with them.
//
#include "sfml-util/music-enum.hpp"
#include "sfml-util/music-operator.hpp"

#include <algorithm>
#include <memory>
#include <tuple>
#include <vector>

namespace heroespath
{
namespace sfml_util
{

    // Everything needed to keep and interact with a list of songs, and the order they are played.
    // If there is more than one song, then a song will never be repeated back-to-back.
    // During construction, the currentlyPlaying_ member is set to what should play first.
    class MusicSet
    {
    public:
        explicit MusicSet(
            const music::Enum WHICH_MUSIC_ENUM = music::None,
            const bool WILL_LOOP = false,
            const float FADE_MULT = MusicOperator::FADE_MULT_DEFAULT_IN_,
            const float VOLUME = MusicOperator::VOLUME_USE_GLOBAL_);

        explicit MusicSet(
            const MusicEnumVec_t & MUSIC_ENUM_VEC,
            const bool WILL_RANDOMIZE = true,
            const bool WILL_START_AT_RANDOM = true,
            const float FADE_MULT = MusicOperator::FADE_MULT_DEFAULT_IN_,
            const float VOLUME = MusicOperator::VOLUME_USE_GLOBAL_,
            const bool WILL_LOOP = true);

        bool IsValid() const;

        std::size_t Count() const { return whichVec_.size(); }
        bool IsSingleSong() const { return (whichVec_.size() == 1); }

        bool WillLoop() const { return willLoop_; }
        void WillLoop(const bool B) { willLoop_ = B; }

        music::Enum CurrentlyPlaying() const { return currentlyPlaying_; }
        music::Enum PreviouslyPlaying() const { return previouslyPlaying_; }

        bool IsRandom() const { return willRandomize_; }
        void IsRandom(const bool B) { willRandomize_ = B; }

        float FadeInMult() const { return fadeInMult_; }
        float Volume() const { return volume_; }

        // Returns the next song that will play, put another way, the result of PickNextSong().
        music::Enum Advance();

        music::Enum PickNextSong();

        bool Contains(const music::Enum E) const
        {
            return std::find(whichVec_.begin(), whichVec_.end(), E) != whichVec_.end();
        }

        friend bool operator==(const MusicSet & L, const MusicSet & R);

    private:
        MusicEnumVec_t whichVec_;
        music::Enum currentlyPlaying_;
        music::Enum previouslyPlaying_;
        bool willRandomize_;
        float fadeInMult_;
        float volume_;
        bool willLoop_;
    };

    using MusicSetVec_t = std::vector<MusicSet>;

    bool operator==(const MusicSet & L, const MusicSet & R);

    inline bool operator!=(const MusicSet & L, const MusicSet & R) { return !(L == R); }
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_MUSICSET_HPP_INCLUDED
