// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// sound-effects-set.cpp
//
#include "sound-effects-set.hpp"

#include "sfml-util/sound-manager.hpp"

#include "misc/random.hpp"
#include "misc/vectors.hpp"

namespace heroespath
{
namespace sfml_util
{

    SfxSet::SfxSet(const SfxEnumVec_t & ENUM_VEC)
        : sfxEnums_(ENUM_VEC)
    {
        // no error checking of empty vector here because that is the default case
    }

    SfxSet::SfxSet(const sound_effect::Enum ENUM)
        : sfxEnums_(1, ENUM)
    {
        M_ASSERT_OR_LOGANDTHROW_SS(
            ((ENUM != sound_effect::None) && (ENUM != sound_effect::Count)),
            "SfxSet::Constructor(enum=" << ENUM << ") enum was invalid.");
    }

    SfxSet::SfxSet(const sound_effect::Enum FIRST_ENUM, const sound_effect::Enum LAST_ENUM)
        : sfxEnums_()
    {
        M_ASSERT_OR_LOGANDTHROW_SS(
            ((FIRST_ENUM != sound_effect::None) && (FIRST_ENUM != sound_effect::Count)),
            "SfxSet::Constructor(first=" << FIRST_ENUM << ", last=" << LAST_ENUM
                                         << ") first was invalid.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            ((LAST_ENUM != sound_effect::None) && (LAST_ENUM != sound_effect::Count)),
            "SfxSet::Constructor(first=" << sound_effect::ToString(FIRST_ENUM)
                                         << ", last=" << LAST_ENUM << ") last was invalid.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (FIRST_ENUM != LAST_ENUM),
            "SfxSet::Constructor(first=" << sound_effect::ToString(FIRST_ENUM)
                                         << ", last=" << sound_effect::ToString(LAST_ENUM)
                                         << ") first and last were the same.");

        auto const FIRST{ static_cast<int>(FIRST_ENUM) };
        auto const LAST{ static_cast<int>(LAST_ENUM) };
        sfxEnums_.reserve(static_cast<std::size_t>((LAST - FIRST) + 1));

        for (int i(FIRST); i <= LAST; ++i)
        {
            sfxEnums_.emplace_back(static_cast<sound_effect::Enum>(i));
        }

        M_ASSERT_OR_LOGANDTHROW_SS(
            (sfxEnums_.empty() == false),
            "SfxSet::Constructor(first=" << sound_effect::ToString(FIRST_ENUM)
                                         << ", last=" << sound_effect::ToString(LAST_ENUM)
                                         << ") resulted in an empty sfxEnums_ vector.");
    }

    void SfxSet::Play(const sound_effect::Enum E) const
    {
        if (sfxEnums_.empty())
        {
            return;
        }

        if (E == sound_effect::Random)
        {
            PlayRandom();
        }
        else
        {
            const std::size_t NUM_SOUNDS(sfxEnums_.size());
            for (std::size_t i(0); i < NUM_SOUNDS; ++i)
            {
                if (E == sfxEnums_[i])
                {
                    PlayAt(i);
                    return;
                }
            }

            std::ostringstream ssErr;
            ssErr << "sfml_util::SfxSet::Play(" << sound_effect::ToString(E)
                  << ") did not find that sound effect amoung the static sounds.";
            throw std::range_error(ssErr.str());
        }
    }

    void SfxSet::PlayAt(const std::size_t INDEX) const
    {
        M_ASSERT_OR_LOGANDTHROW_SS(
            (INDEX < sfxEnums_.size()),
            "sfml_util::SfxSet::PlayAt(" << INDEX << ") was given an index out of range.");

        SoundManager::Instance()->SoundEffectPlay(sfxEnums_[INDEX]);
    }

    void SfxSet::PlayRandom() const
    {
        if (sfxEnums_.empty())
        {
            return;
        }
        else if (sfxEnums_.size() == 1)
        {
            PlayAt(0);
        }
        else
        {
            PlayAt(misc::random::SizeT(sfxEnums_.size() - 1));
        }
    }

    sound_effect::Enum SfxSet::SelectRandom() const
    {
        if (sfxEnums_.empty())
        {
            return sound_effect::None;
        }
        else
        {
            return misc::Vector::SelectRandom(sfxEnums_);
        }
    }
} // namespace sfml_util
} // namespace heroespath
