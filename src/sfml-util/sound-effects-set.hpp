// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
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

        std::size_t Size() const { return sfxEnums_.size(); }

        bool IsValid() const { return !sfxEnums_.empty(); }

        sound_effect::Enum SelectRandom() const;

    private:
        SfxEnumVec_t sfxEnums_;
    };

    using SfxSetVec_t = std::vector<SfxSet>;

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_SOUNDEFFECTSSET_HPP_INCLUDED
