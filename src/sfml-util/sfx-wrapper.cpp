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
// sfx-wrapper.cpp
//
#include "sfx-wrapper.hpp"

#include "misc/assertlogandthrow.hpp"


namespace heroespath
{
namespace sfml_util
{

    SfxWrapper::SfxWrapper(
        const sound_effect::Enum ENUM,
        SoundUPtr_t              soundUPtr,
        SoundBufferUPtr_t        BUFFER_UPTR)
    :
        which_      (ENUM),
        soundUPtr_  (std::move(soundUPtr)),
        bufferUPtr_ (std::move(BUFFER_UPTR))
    {
        if (IsValid())
        {
            soundUPtr_->setBuffer( * bufferUPtr_);
        }
    }


    SfxWrapper::~SfxWrapper()
    {
        if (soundUPtr_.get() != nullptr)
        {
            soundUPtr_->stop();
        }
    }


    SfxWrapper::SfxWrapper(SfxWrapper && SFXW)
    :
        which_      (SFXW.which_),
        soundUPtr_  (std::move(SFXW.soundUPtr_)),
        bufferUPtr_ (std::move(SFXW.bufferUPtr_))
    {}


    SfxWrapper & SfxWrapper::operator=(SfxWrapper && SFXW)
    {
        if (&SFXW != this)
        {
            which_ = SFXW.which_;
            soundUPtr_ = std::move(SFXW.soundUPtr_);
            bufferUPtr_ = std::move(SFXW.bufferUPtr_);
        }

        return * this;
    }


    bool SfxWrapper::IsValid() const
    {
        return ((which_ != sound_effect::Count) &&
                (which_ != sound_effect::None) &&
                (soundUPtr_.get() != nullptr) &&
                (bufferUPtr_.get() != nullptr));
    }


    void SfxWrapper::Reset()
    {
        if (soundUPtr_.get() != nullptr)
        {
            soundUPtr_->stop();
        }

        which_ = sound_effect::Count;
        soundUPtr_.reset();
        bufferUPtr_.reset();
    }

}
}
