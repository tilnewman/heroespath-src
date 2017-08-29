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
    public:
        explicit MusicOperator(const MusicInfo &   MUSIC_INFO    = MusicInfo(),
                               const MusicSPtr_t & MUSIC_SPTR    = MusicSPtr_t(),
                               const float         FADE_IN_MULT  = FADE_MULT_IMMEDIATE_,
                               const float         TARGET_VOLUME = VOLUME_USE_GLOBAL_);

        virtual ~MusicOperator();

        inline const MusicInfo Info() const         { return info_; }

        inline void Play()                          { musicSPtr_->play(); }
        inline void Pause()                         { musicSPtr_->pause(); }
        inline void Stop()                          { musicSPtr_->stop(); }
        inline sf::Music::Status Status() const     { return musicSPtr_->getStatus(); }

        inline bool IsLooped() const                { return musicSPtr_->getLoop(); }
        inline void IsLooped(const bool B)          { musicSPtr_->setLoop(B); }

        inline bool IsFadingIn() const              { return (fadeInMult_ > 0.0f); }
        inline bool IsFadingOut() const             { return (fadeOutMult_ > 0.0f); }

        inline float FadingInMult() const           { return fadeInMult_; }
        inline void FadingInMult(const float F)     { fadeInMult_ = F; }

        inline float FadingOutMult() const          { return fadeOutMult_; }
        inline void FadingOutMult(const float F)    { fadeOutMult_ = F; }

        inline bool WillKillAfterFadeOut() const        { return killAfterFadeOut_; }
        inline void WillKillAfterFadeOut(const bool B)  { killAfterFadeOut_ = B; }

        inline float Volume() const                 { return musicSPtr_->getVolume(); }
        inline void Volume(const float F)           { musicSPtr_->setVolume(F); }
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
        MusicInfo info_;
        float     targetVolume_;
        float     fadeInMult_;
        float     fadeOutMult_;
        bool      killAfterFadeOut_;
        sfml_util::MusicSPtr_t musicSPtr_;
    };


    using MusicOperatorSPtr_t = std::shared_ptr<MusicOperator>;


    bool operator==(const MusicOperator & L, const MusicOperator & R);


    inline bool operator!=(const MusicOperator & L, const MusicOperator & R)
    {
        return ! (L == R);
    }

}
#endif //SFMLUTIL_MUSIC_OPERATOR_INCLUDED
