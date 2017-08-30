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
#ifndef SFMLUTIL_MUSIC_OPERATOR_INCLUDED
#define SFMLUTIL_MUSIC_OPERATOR_INCLUDED
//
// music-operator.hpp
//  The only place that sf::Music objects are stored and controlled.
//
#include "sfml-util/sfml-audio.hpp"
#include "sfml-util/music-info.hpp"

#include <memory>
#include <string>


namespace sf
{
    class Music;
}

namespace sfml_util
{

    struct music_update_status
    {
        enum Enum
        {
            Stopped = 0,
            Playing,
            FadingOut,
            FadedOut,
            FadedOutKill,
            FadingIn,
            FadedIn,
            Count
        };

        static const std::string ToString(const music_update_status::Enum);
    };


    //Responsible for storing sf::Muisc objects, information about them,
    //and for presenting an interface to controll them.  The primary feature
    //added by MusicOperator is fading, which is easy to control with the
    //interface provided.  Set the FadeMult param to FADE_MULT_IMMEDIATE_
    //of any negative float if you want the fade to be instant.
    class MusicOperator
    {
        MusicOperator(const MusicOperator &) =delete;
        MusicOperator & operator=(const MusicOperator &) =delete;

    public:
        explicit MusicOperator(const MusicInfo & MUSIC_INFO    = MusicInfo(),
                               MusicUPtr_t       musicUPtr     = MusicUPtr_t(),
                               const float       FADE_IN_MULT  = FADE_MULT_IMMEDIATE_,
                               const float       TARGET_VOLUME = VOLUME_USE_GLOBAL_);

        virtual ~MusicOperator();

        MusicOperator(MusicOperator &&);

        MusicOperator & operator=(MusicOperator &&);

        bool IsValid() const;

        inline const MusicInfo Info() const         { return info_; }

        inline void Play()                          { if (IsValid()) musicUPtr_->play(); }
        inline void Pause()                         { if (IsValid()) musicUPtr_->pause(); }
        inline void Stop()                          { if (IsValid()) musicUPtr_->stop(); }

        inline sf::Music::Status Status() const
        {
            return ((IsValid()) ? musicUPtr_->getStatus() : sf::Music::Status::Stopped);
        }

        inline bool IsLooped() const
        {
            return ((IsValid()) ? musicUPtr_->getLoop() : false);
        }

        inline void IsLooped(const bool B)          { if (IsValid()) musicUPtr_->setLoop(B); }

        inline bool IsFadingIn() const              { return (fadeInMult_ > 0.0f); }
        inline bool IsFadingOut() const             { return (fadeOutMult_ > 0.0f); }

        inline float FadingInMult() const           { return fadeInMult_; }
        inline void FadingInMult(const float F)     { fadeInMult_ = F; }

        inline float FadingOutMult() const          { return fadeOutMult_; }
        inline void FadingOutMult(const float F)    { fadeOutMult_ = F; }

        inline bool WillKillAfterFadeOut() const        { return killAfterFadeOut_; }
        inline void WillKillAfterFadeOut(const bool B)  { killAfterFadeOut_ = B; }

        inline float Volume() const
        {
            return ((IsValid()) ? musicUPtr_->getVolume() : 0.0f);
        }
        
        inline void Volume(const float F)
        {
            if (IsValid()) musicUPtr_->setVolume((F < 0.0f) ? 0.0f : ((F > 100.0f) ? 100.0f : F));
        }

        inline float VolumeTarget() const           { return targetVolume_; }
        inline void VolumeTarget(const float F)     { targetVolume_ = F; }

        void VolumeFadeTo(const float TARGET_VOL, const float FADE_MULT = FADE_MULT_IMMEDIATE_);
        void VolumeFadeToGlobal(const float FADE_MULT = FADE_MULT_IMMEDIATE_);
        void VolumeFadeOut(const float FADE_MULT = FADE_MULT_DEFAULT_OUT_, const bool WILL_KILL_AFTER = true);

        music_update_status::Enum UpdateTime(const float ELAPSED_TIME_SECONDS);
        
        friend bool operator==(const MusicOperator & L, const MusicOperator & R);
        
    public:
        static const float VOLUME_USE_GLOBAL_;
        static const float FADE_MULT_IMMEDIATE_;
        static const float FADE_MULT_DEFAULT_IN_;
        static const float FADE_MULT_DEFAULT_OUT_;

    private:
        MusicInfo   info_;
        float       targetVolume_;
        float       fadeInMult_;
        float       fadeOutMult_;
        bool        killAfterFadeOut_;
        MusicUPtr_t musicUPtr_;
    };


    using MusicOperatorSPtr_t = std::shared_ptr<MusicOperator>;


    bool operator==(const MusicOperator & L, const MusicOperator & R);


    inline bool operator!=(const MusicOperator & L, const MusicOperator & R)
    {
        return ! (L == R);
    }

}
#endif //SFMLUTIL_MUSIC_OPERATOR_INCLUDED
