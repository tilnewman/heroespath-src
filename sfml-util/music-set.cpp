//
// music-set.cpp
//
#include "music-set.hpp"
#include "sfml-util/random.hpp"


namespace sfml_util
{

    MusicSet::MusicSet(const music::Enum WHICH_SINGLE_MUSIC,
                       const bool        WILL_LOOP,
                       const float       FADE_MULT,
                       const float       VOLUME)
    :
        whichVec_          (1, WHICH_SINGLE_MUSIC),
        currentlyPlaying_  (WHICH_SINGLE_MUSIC),
        previouslyPlaying_ (WHICH_SINGLE_MUSIC),
        willRandomize_     (false),
        fadeInMult_        (FADE_MULT),
        volume_            (VOLUME),
        willLoop_          (WILL_LOOP)
    {}


    MusicSet::MusicSet(const MusicEnumVec_t & MUSIC_SET_VEC,
                       const bool             WILL_RANDOMIZE,
                       const bool             WILL_START_AT_RANDOM,
                       const float            FADE_MULT,
                       const float            VOLUME,
                       const bool             WILL_LOOP)
    :
        whichVec_          (MUSIC_SET_VEC),
        currentlyPlaying_  (music::None),
        previouslyPlaying_ (music::None),
        willRandomize_     (WILL_RANDOMIZE),
        fadeInMult_        (FADE_MULT),
        volume_            (VOLUME),
        willLoop_          (WILL_LOOP)
    {
        const bool ORIG_RANDOM_SETTING(willRandomize_);
        willRandomize_ = WILL_START_AT_RANDOM;
        const bool ORIG_LOOP_SETTING(willLoop_);
        willLoop_ = true;

        currentlyPlaying_ = PickNextSong();

        willRandomize_ = ORIG_RANDOM_SETTING;
        willLoop_ = ORIG_LOOP_SETTING;
    }


    music::Enum MusicSet::Advance()
    {
        const music::Enum ORIG_CURRENT(currentlyPlaying_);
        currentlyPlaying_ = PickNextSong();
        previouslyPlaying_ = ORIG_CURRENT;
        return currentlyPlaying_;
    }


    music::Enum MusicSet::PickNextSong()
    {
        if (false == willLoop_)
            return music::None;

        if (whichVec_.size() == 1)
        {
            return whichVec_[0];
        }
        else
        {
            if (willRandomize_)
            {
                //populate a vector of songs that does not include the currently (or just finished) playing
                MusicEnumVec_t possibleVec;
                for (auto const & NEXT_MUSIC_ENUM : whichVec_)
                    if (NEXT_MUSIC_ENUM != currentlyPlaying_)
                        possibleVec.push_back(NEXT_MUSIC_ENUM);

                //pick next song at random
                const std::size_t NUM_POSSIBLE_SONGS(possibleVec.size());
                if (NUM_POSSIBLE_SONGS == 1)
                    return possibleVec[0];
                else
                    return possibleVec[sfml_util::rand::Int(0, NUM_POSSIBLE_SONGS - 1)];
            }
            else
            {
                //pick the next song in order given at time of construction
                const std::size_t NUM_SONGS(whichVec_.size());
                for (std::size_t i(0); i < NUM_SONGS; ++i)
                {
                    if (whichVec_[i] == currentlyPlaying_)
                    {
                        if ((NUM_SONGS - 1) == i)
                            return whichVec_[0];
                        else
                            return whichVec_[i + 1];
                    }
                }

                //should never get here...
                return whichVec_[0];
            }
        }
    }


    bool MusicSet::Contains(const music::Enum ENUM_TO_FIND)
    {
        for (auto const & NEXT_MUSIC_ENUM : whichVec_)
            if (NEXT_MUSIC_ENUM == ENUM_TO_FIND)
                return true;

        return false;
    }

}
