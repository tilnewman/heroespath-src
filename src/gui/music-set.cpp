// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// music-set.cpp
//
#include "music-set.hpp"

#include "misc/assertlogandthrow.hpp"
#include "misc/random.hpp"
#include "misc/vectors.hpp"

namespace heroespath
{
namespace gui
{

    MusicSet::MusicSet(
        const music::Enum WHICH_MUSIC_ENUM,
        const bool WILL_LOOP,
        const float FADE_MULT,
        const float VOLUME)
        : whichVec_(1, WHICH_MUSIC_ENUM)
        , currentlyPlaying_(WHICH_MUSIC_ENUM)
        , previouslyPlaying_(WHICH_MUSIC_ENUM)
        , willRandomize_(false)
        , fadeInMult_(FADE_MULT)
        , volume_(VOLUME)
        , willLoop_(WILL_LOOP)
    {}

    MusicSet::MusicSet(
        const MusicEnumVec_t & MUSIC_ENUM_VEC,
        const bool WILL_RANDOMIZE,
        const bool WILL_START_AT_RANDOM,
        const float FADE_MULT,
        const float VOLUME,
        const bool WILL_LOOP)
        : whichVec_(MUSIC_ENUM_VEC)
        , currentlyPlaying_(music::None)
        , previouslyPlaying_(music::None)
        , willRandomize_(WILL_RANDOMIZE)
        , fadeInMult_(FADE_MULT)
        , volume_(VOLUME)
        , willLoop_(WILL_LOOP)
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (whichVec_.empty() == false),
            "gui::MusicSet::Constructor(vector version) was given an empty vector.");

        const auto ORIG_RANDOM_SETTING { willRandomize_ };
        willRandomize_ = WILL_START_AT_RANDOM;

        const auto ORIG_LOOP_SETTING { willLoop_ };
        willLoop_ = true;

        currentlyPlaying_ = PickNextSong();

        willRandomize_ = ORIG_RANDOM_SETTING;
        willLoop_ = ORIG_LOOP_SETTING;
    }

    bool MusicSet::IsValid() const { return (!whichVec_.empty() && !ContainsInvalid()); }

    music::Enum MusicSet::Advance()
    {
        const auto PREV_PLAYING { currentlyPlaying_ };
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
            MusicEnumVec_t possibleSongs { whichVec_ };

            whichVec_.erase(
                std::remove(whichVec_.begin(), whichVec_.end(), currentlyPlaying_),
                whichVec_.end());

            return misc::Vector::SelectRandom(possibleSongs);
        }
        else
        {
            auto currentlyPlayingIter { std::find(
                whichVec_.begin(), whichVec_.end(), currentlyPlaying_) };

            if (++currentlyPlayingIter == whichVec_.end())
            {
                if (willLoop_)
                {
                    return *whichVec_.begin();
                }
                else
                {
                    return music::None;
                }
            }
            else
            {
                return *currentlyPlayingIter;
            }
        }
    }

    bool operator==(const MusicSet & L, const MusicSet & R)
    {
        // The whichVec_ member is ordered, so it can be compared in the usual way.

        return (
            std::tie(
                L.whichVec_,
                L.currentlyPlaying_,
                L.previouslyPlaying_,
                L.willRandomize_,
                L.fadeInMult_,
                L.volume_)
            == std::tie(
                R.whichVec_,
                R.currentlyPlaying_,
                R.previouslyPlaying_,
                R.willRandomize_,
                R.fadeInMult_,
                R.volume_));
    }
} // namespace gui
} // namespace heroespath
