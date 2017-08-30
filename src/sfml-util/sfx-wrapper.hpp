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
#ifndef SFMLUTIL_SFXWRAPPER_HPP_INCLUDED
#define SFMLUTIL_SFXWRAPPER_HPP_INCLUDED
//
// sfx-wrapper.hpp
//
#include "sfml-util/sfml-audio.hpp"
#include "sfml-util/sound-effects-enum.hpp"

#include <tuple>
#include <vector>
#include <memory>


namespace sfml_util
{

    //Responsible for storing all sfml objects relating to a single sound effect,
    //and presenting an interface for controlling it.
    class SfxWrapper
    {
        SfxWrapper(const SfxWrapper &) =delete;
        SfxWrapper & operator=(const SfxWrapper &) =delete;

    public:
        SfxWrapper(
            const sound_effect::Enum ENUM        = sound_effect::Count,
            SoundUPtr_t              SOUND_UPTR  = SoundUPtr_t(),
            SoundBufferUPtr_t        BUFFER_UPTR = SoundBufferUPtr_t());

        ~SfxWrapper();

        SfxWrapper(SfxWrapper &&);

        SfxWrapper & operator=(SfxWrapper &&);

        bool IsValid() const;

        void Reset();

        inline sound_effect::Enum Which() const
        {
            return which_;
        }

        inline void Play()
        {
            if (IsValid())
            {
                soundUPtr_->play();
            }
        }

        inline void Stop()
        {
            if (IsValid())
            {
                soundUPtr_->stop();
            }
        }
        
        inline float Volume() const
        {
            return ((IsValid()) ? soundUPtr_->getVolume() : 0.0f);
        }

        inline void Volume(const float NEW_VOL)
        {
            if (IsValid())
            {
                soundUPtr_->setVolume(NEW_VOL);
            }
        }

        inline sf::SoundSource::Status Status() const
        {
            return ((IsValid()) ? soundUPtr_->getStatus() : sf::SoundSource::Stopped);
        }

        friend bool operator==(const SfxWrapper & L, const SfxWrapper & R);
        friend bool operator<(const SfxWrapper & L, const SfxWrapper & R);

    private:
        sound_effect::Enum which_;
        SoundUPtr_t soundUPtr_;
        SoundBufferUPtr_t bufferUPtr_;
    };


    inline bool operator==(const SfxWrapper & L, const SfxWrapper & R)
    {
        return std::tie(L.which_, L.soundUPtr_, L.bufferUPtr_) ==
               std::tie(R.which_, R.soundUPtr_, R.bufferUPtr_);
    }


    inline bool operator!=(const SfxWrapper & L, const SfxWrapper & R)
    {
        return ! (L == R);
    }


    inline bool operator<(const SfxWrapper & L, const SfxWrapper & R)
    {
        return std::tie(L.which_, L.soundUPtr_, L.bufferUPtr_) <
               std::tie(R.which_, R.soundUPtr_, R.bufferUPtr_);
    }


    using SfxWrapperVec_t = std::vector<SfxWrapper>;

}

#endif //SFMLUTIL_SFXWRAPPER_HPP_INCLUDED