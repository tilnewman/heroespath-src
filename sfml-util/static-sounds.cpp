//
// static-sounds.cpp
//
#include "static-sounds.hpp"

#include "utilz/random.hpp"
#include "sfml-util/sound-manager.hpp"


namespace sfml_util
{

    const float StaticSounds::INVALID_VOLUME_(-1.0f);//any value below zero will work here


    StaticSounds::StaticSounds(const SoundEffectEnumVec_t & ENUM_VEC,
                               const float                  VOLUME,
                               const bool                   WILL_LOAD_ALL)
    :
        SOUND_MANAGER_CPTR_(SoundManager::Instance().get()),
        soundSVec_         (),
        enumVec_           (ENUM_VEC),
        volume_            (0.0f)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((enumVec_.empty() == false), "sfml_util::StaticSounds::Constructor was given an empty ENUM_VEC.");

        soundSVec_.resize(enumVec_.size());

        if (WILL_LOAD_ALL)
            LoadAll();

        Volume(VOLUME);
    }


    StaticSounds::StaticSounds(const sound_effect::Enum FIRST_SOUND_EFFECT_ENUM,
                               const sound_effect::Enum LAST_SOUND_EFFECT_ENUM,
                               const float              VOLUME,
                               const bool               WILL_LOAD_ALL)
    :
        SOUND_MANAGER_CPTR_(SoundManager::Instance().get()),
        soundSVec_           (),
        enumVec_           (),
        volume_            (0.0f)
    {
        auto const FIRST{ static_cast<int>(FIRST_SOUND_EFFECT_ENUM) };
        auto const LAST { static_cast<int>(LAST_SOUND_EFFECT_ENUM) };
        enumVec_.reserve((LAST - FIRST) + 1);

        for (int i(FIRST); i <= LAST; ++i)
            enumVec_.push_back( static_cast<sound_effect::Enum>(i) );

        soundSVec_.resize(enumVec_.size());

        if (WILL_LOAD_ALL)
            LoadAll();

        Volume(VOLUME);
    }


    StaticSounds::~StaticSounds()
    {
        ReleaseAll();
    }


    void StaticSounds::Play(const sound_effect::Enum E)
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
            ssErr << "sfml_util::StaticSounds::Play(" << sound_effect::ToString(E) << ") did not find that sound effect amoung the static sounds.";
            throw std::range_error(ssErr.str());
        }
    }


    void StaticSounds::PlayAt(const std::size_t INDEX)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((INDEX < soundSVec_.size()), "sfml_util::StaticSounds::PlayAt(" << INDEX << ") was given an index out of range.");

        if (soundSVec_[INDEX].get() == nullptr)
            LoadAt(INDEX);

        soundSVec_[INDEX]->play();
    }


    void StaticSounds::PlayRandom()
    {
        if (soundSVec_.size() == 1)
        {
            PlayAt(0);
        }
        else
        {
            PlayAt(utilz::random::Int(soundSVec_.size() - 1));
        }
    }


    void StaticSounds::Volume(const float NEW_VOLUME)
    {
        if (NEW_VOLUME < 0.0f)
        {
            volume_ = SOUND_MANAGER_CPTR_->SoundEffectVolume();
        }
        else
        {
            if (NEW_VOLUME > 100.0f)
            {
                volume_ = 100.0f;
            }
            else
            {
                volume_ = NEW_VOLUME;
            }
        }

        for (auto & sptr : soundSVec_)
            if (sptr.get() != nullptr)
                sptr->setVolume(volume_);
    }


    void StaticSounds::LoadAt(const std::size_t INDEX)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((INDEX < soundSVec_.size()), "sfml_util::StaticSounds::LoadAt(" << INDEX << ") was given an index out of range.");

        if (soundSVec_[INDEX].get() == nullptr)
        {
            soundSVec_[INDEX] = SOUND_MANAGER_CPTR_->SoundEffectAcquire(enumVec_[INDEX]);
            soundSVec_[INDEX]->setVolume(volume_);
        }
    }


    void StaticSounds::Load(const sound_effect::Enum E)
    {
        const std::size_t NUM_SOUNDS(enumVec_.size());
        for (std::size_t i(0); i < NUM_SOUNDS; ++i)
            if (E == enumVec_[i])
                LoadAt(i);
    }


    void StaticSounds::LoadAll()
    {
        const std::size_t NUM_SOUNDS(soundSVec_.size());
        for (std::size_t i(0); i < NUM_SOUNDS; ++i)
            LoadAt(i);
    }


    void StaticSounds::ReleaseAt(const std::size_t INDEX)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((INDEX < soundSVec_.size()), "sfml_util::StaticSounds::ReleaseAt(" << INDEX << ") was given an index out of range.");

        if (soundSVec_[INDEX].get() != nullptr)
        {
            soundSVec_[INDEX].reset();
            SOUND_MANAGER_CPTR_->SoundEffectRelease(enumVec_[INDEX]);
        }
    }


    void StaticSounds::Release(const sound_effect::Enum E)
    {
        const std::size_t NUM_SOUNDS(enumVec_.size());
        for (std::size_t i(0); i < NUM_SOUNDS; ++i)
            if (E == enumVec_[i])
                ReleaseAt(i);
    }


    void StaticSounds::ReleaseAll()
    {
        const std::size_t NUM_SOUNDS(soundSVec_.size());
        for (std::size_t i(0); i < NUM_SOUNDS; ++i)
            ReleaseAt(i);
    }

}
