//
// sound-effects-set.cpp
//
#include "sound-effects-set.hpp"

#include "sfml-util/sound-manager.hpp"

#include "utilz/random.hpp"


namespace sfml_util
{

    SoundEffectsSet::SoundEffectsSet(const SoundEffectEnumVec_t & ENUM_VEC)
    :
        enumVec_(ENUM_VEC)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((enumVec_.empty() == false), "sfml_util::SoundEffectsSet::Constructor was given an empty ENUM_VEC.");
    }


    SoundEffectsSet::SoundEffectsSet(const sound_effect::Enum FIRST_SOUND_EFFECT_ENUM,
                                     const sound_effect::Enum LAST_SOUND_EFFECT_ENUM)
    :
        enumVec_()
    {
        auto const FIRST{ static_cast<int>(FIRST_SOUND_EFFECT_ENUM) };
        auto const LAST { static_cast<int>(LAST_SOUND_EFFECT_ENUM) };
        enumVec_.reserve(static_cast<std::size_t>((LAST - FIRST) + 1));

        for (int i(FIRST); i <= LAST; ++i)
            enumVec_.push_back( static_cast<sound_effect::Enum>(i) );

        M_ASSERT_OR_LOGANDTHROW_SS((enumVec_.empty() == false), "sfml_util::SoundEffectsSet::Constructor(enum, enum) was given an equal enums.");
    }


    void SoundEffectsSet::Play(const sound_effect::Enum E)
    {
        if (E == sound_effect::Random)
        {
            PlayRandom();
        }
        else
        {
            const std::size_t NUM_SOUNDS(enumVec_.size());
            for (std::size_t i(0); i < NUM_SOUNDS; ++i)
            {
                if (E == enumVec_[i])
                {
                    PlayAt(i);
                    return;
                }
            }

            std::ostringstream ssErr;
            ssErr << "sfml_util::SoundEffectsSet::Play(" << sound_effect::ToString(E) << ") did not find that sound effect amoung the static sounds.";
            throw std::range_error(ssErr.str());
        }
    }


    void SoundEffectsSet::PlayAt(const std::size_t INDEX)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((INDEX < enumVec_.size()), "sfml_util::SoundEffectsSet::PlayAt(" << INDEX << ") was given an index out of range.");
        SoundManager::Instance()->SoundEffectPlay(enumVec_[INDEX]);
    }


    void SoundEffectsSet::PlayRandom()
    {
        if (enumVec_.size() == 1)
        {
            PlayAt(0);
        }
        else
        {
            PlayAt(static_cast<std::size_t>(utilz::random::Int(static_cast<int>(enumVec_.size()) - 1)));
        }
    }

}
