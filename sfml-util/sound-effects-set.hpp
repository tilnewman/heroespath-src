#ifndef SFMLUTIL_SOUNDEFFECTSSET_INCLUDED
#define SFMLUTIL_SOUNDEFFECTSSET_INCLUDED
//
// sound-effects-set.hpp
//  Simple wrapper of sound effects
//
#include "sfml-util/sound-effects-enum.hpp"

#include <memory>
#include <vector>


namespace sfml_util
{

    //Wraps a set of sound effects for easy playing of a random selection
    class SoundEffectsSet
    {
    public:
        explicit SoundEffectsSet(const SoundEffectEnumVec_t & ENUM_VEC);

        SoundEffectsSet(const sound_effect::Enum FIRST_SOUND_EFFECT_ENUM,
                        const sound_effect::Enum LAST_SOUND_EFFECT_ENUM);

        //throws runtime_error if given is not in the set
        void Play(const sound_effect::Enum);

        //throws range_error if out of bounds
        void PlayAt(const std::size_t);

        void PlayRandom();

        inline std::size_t Size() const
        {
            return enumVec_.size();
        }

    private:
        SoundEffectEnumVec_t enumVec_;
    };

    using SoundEffectsSetPtr_t  = SoundEffectsSet *;
    using SoundEffectsSetSPtr_t = std::shared_ptr<SoundEffectsSet>;
    using SoundEffectsSetSVec_t = std::vector<SoundEffectsSetSPtr_t>;

}
#endif //SFMLUTIL_SOUNDEFFECTSSET_INCLUDED
