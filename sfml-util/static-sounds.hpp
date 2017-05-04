#ifndef SFMLUTIL_STATICSOUNDS_INCLUDED
#define SFMLUTIL_STATICSOUNDS_INCLUDED
//
// static-sounds.hpp
//  Simple wrapper of sound effects
//
#include "sfml-util/sfml-audio.hpp"
#include "sfml-util/sound-effects-enum.hpp"

#include <memory>


namespace sfml_util
{

    //forward declarations
    class SoundManager;
    using SoundManagerPtr_t = SoundManager *;


    //wraps a set of sound effects and keeps them in memory and ready to play
    class StaticSounds
    {
        //prevent copy assignment
        StaticSounds & operator=(const StaticSounds &) =delete;

        //prevent copy construction
        StaticSounds(const StaticSounds &) =delete;

    public:
        //If default (INVALID_VOLUME_) is used, then the volume is set to the master sound effects volume in SoundManager.
        StaticSounds(const SoundEffectEnumVec_t & ENUM_VEC,
                     const float                  VOLUME = INVALID_VOLUME_,
                     const bool                   WILL_LOAD_ALL = false);

        StaticSounds(const sound_effect::Enum FIRST_SOUND_EFFECT_ENUM,
                     const sound_effect::Enum LAST_SOUND_EFFECT_ENUM,
                     const float              VOLUME                 = INVALID_VOLUME_,
                     const bool               WILL_LOAD_ALL          = false);

        virtual ~StaticSounds();

        void Play(const sound_effect::Enum);
        void PlayAt(const std::size_t);
        void PlayRandom();

        inline std::size_t Size() const                                 { return sptrVec_.size(); }
        inline bool Contains(const sound_effect::Enum E) const          { return (std::find(enumVec_.begin(), enumVec_.end(), E) != enumVec_.end()); }
        inline sound_effect::Enum GetEnumAt(const std::size_t I) const  { return enumVec_.at(I); }

        inline float Volume() const                                     { return volume_; }
        void Volume(const float NEW_VOLUME);
        inline void VolumeSetToMaster()                                 { Volume(INVALID_VOLUME_); }

        void LoadAt(const std::size_t);
        void Load(const sound_effect::Enum);
        void LoadAll();

        void ReleaseAt(const std::size_t);
        void Release(const sound_effect::Enum);
        void ReleaseAll();

    public:
        static const float INVALID_VOLUME_;

    private:
        const SoundManagerPtr_t SOUND_MANAGER_CPTR_;
        SoundSVec_t sptrVec_;
        SoundEffectEnumVec_t enumVec_;
        float volume_;
    };

    using StaticSoundsSPtr_t = std::shared_ptr<StaticSounds>;

}
#endif //SFMLUTIL_STATICSOUNDS_INCLUDED
