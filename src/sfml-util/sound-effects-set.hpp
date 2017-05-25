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
