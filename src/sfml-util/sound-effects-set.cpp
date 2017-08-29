// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
//
// sound-effects-set.cpp
//
#include "sound-effects-set.hpp"

#include "sfml-util/sound-manager.hpp"

#include "misc/random.hpp"


namespace sfml_util
{

    SfxSet::SfxSet(const SfxEnumVec_t & ENUM_VEC)
    :
        sfxEnums_(ENUM_VEC)
    {
        //no error checking of empty vector here because that is the default case
    }


    SfxSet::SfxSet(const sound_effect::Enum ENUM)
    :
        sfxEnums_(1, ENUM)
    {
        M_ASSERT_OR_LOGANDTHROW_SS(
            ((ENUM != sound_effect::None) && (ENUM != sound_effect::Count)),
            "SfxSet::Constructor(enum=" << ENUM << ") enum was invalid.");
    }


    SfxSet::SfxSet(const sound_effect::Enum FIRST_ENUM,
                     const sound_effect::Enum LAST_ENUM)
    :
        sfxEnums_()
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
        auto const LAST { static_cast<int>(LAST_ENUM) };
        sfxEnums_.reserve(static_cast<std::size_t>((LAST - FIRST) + 1));

        for (int i(FIRST); i <= LAST; ++i)
        {
            sfxEnums_.push_back(static_cast<sound_effect::Enum>(i));
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
        M_ASSERT_OR_LOGANDTHROW_SS((INDEX < sfxEnums_.size()),
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
            PlayAt(static_cast<std::size_t>(
                misc::random::Int(static_cast<int>(sfxEnums_.size()) - 1)));
        }
    }

}
