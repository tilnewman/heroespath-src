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
#ifndef HEROESPATH_SFMLUTIL_SOUNDEFFECTSSET_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_SOUNDEFFECTSSET_HPP_INCLUDED
//
// sound-effects-set.hpp
//  Simple wrapper of sound effects
//
#include "sfml-util/sound-effects-enum.hpp"

#include <memory>
#include <vector>

namespace heroespath
{
namespace sfml_util
{

    // Wraps a set of sound effects for easy playing of a random selection
    class SfxSet
    {
    public:
        explicit SfxSet(const SfxEnumVec_t & ENUM_VEC = SfxEnumVec_t());

        explicit SfxSet(const sound_effect::Enum);

        SfxSet(const sound_effect::Enum FIRST_ENUM, const sound_effect::Enum LAST_ENUM);

        // throws runtime_error if given is not in the set
        void Play(const sound_effect::Enum) const;

        // throws range_error if out of bounds
        void PlayAt(const std::size_t) const;

        void PlayRandom() const;

        inline std::size_t Size() const { return sfxEnums_.size(); }

        inline bool IsValid() const { return !sfxEnums_.empty(); }

        sound_effect::Enum SelectRandom() const;

    private:
        SfxEnumVec_t sfxEnums_;
    };

    using SfxSetVec_t = std::vector<SfxSet>;
}
}

#endif // HEROESPATH_SFMLUTIL_SOUNDEFFECTSSET_HPP_INCLUDED
