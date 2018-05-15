// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_MUSIC_OPERATOR_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_MUSIC_OPERATOR_HPP_INCLUDED
//
// music-operator.hpp
//  The only place that sf::Music objects are stored and controlled.
//
#include "sfml-util/music-info.hpp"
#include "sfml-util/sfml-audio.hpp"

#include <memory>
#include <string>

namespace sf
{
class Music;
}

namespace heroespath
{
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

    // Responsible for storing sf::Muisc objects, information about them,
    // and for presenting an interface to controll them.  The primary feature
    // added by MusicOperator is fading, which is easy to control with the
    // interface provided.  Set the FadeMult param to FADE_MULT_IMMEDIATE_
    // of any negative float if you want the fade to be instant.
    class MusicOperator
    {
    public:
        MusicOperator(const MusicOperator &) = delete;
        MusicOperator & operator=(const MusicOperator &) = delete;

    public:
        explicit MusicOperator(
            const MusicInfo & MUSIC_INFO = MusicInfo(),
            MusicUPtr_t musicUPtr = MusicUPtr_t(),
            const float FADE_IN_MULT = FADE_MULT_IMMEDIATE_,
            const float TARGET_VOLUME = VOLUME_USE_GLOBAL_);

        virtual ~MusicOperator();

        MusicOperator(MusicOperator &&);

        MusicOperator & operator=(MusicOperator &&);

        bool IsValid() const;

        const MusicInfo Info() const { return info_; }

        void Play()
        {
            if (IsValid())
                musicUPtr_->play();
        }
        void Pause()
        {
            if (IsValid())
                musicUPtr_->pause();
        }
        void Stop()
        {
            if (IsValid())
                musicUPtr_->stop();
        }

        sf::Music::Status Status() const
        {
            return ((IsValid()) ? musicUPtr_->getStatus() : sf::Music::Status::Stopped);
        }

        bool IsLooped() const { return ((IsValid()) ? musicUPtr_->getLoop() : false); }

        void IsLooped(const bool B)
        {
            if (IsValid())
                musicUPtr_->setLoop(B);
        }

        bool IsFadingIn() const { return (fadeInMult_ > 0.0f); }
        bool IsFadingOut() const { return (fadeOutMult_ > 0.0f); }

        float FadingInMult() const { return fadeInMult_; }
        void FadingInMult(const float F) { fadeInMult_ = F; }

        float FadingOutMult() const { return fadeOutMult_; }
        void FadingOutMult(const float F) { fadeOutMult_ = F; }

        bool WillKillAfterFadeOut() const { return killAfterFadeOut_; }
        void WillKillAfterFadeOut(const bool B) { killAfterFadeOut_ = B; }

        float Volume() const { return ((IsValid()) ? musicUPtr_->getVolume() : 0.0f); }

        void Volume(const float F)
        {
            if (IsValid())
                musicUPtr_->setVolume((F < 0.0f) ? 0.0f : ((F > 100.0f) ? 100.0f : F));
        }

        float VolumeTarget() const { return targetVolume_; }
        void VolumeTarget(const float F) { targetVolume_ = F; }

        void VolumeFadeTo(const float TARGET_VOL, const float FADE_MULT = FADE_MULT_IMMEDIATE_);
        void VolumeFadeToGlobal(const float FADE_MULT = FADE_MULT_IMMEDIATE_);
        void VolumeFadeOut(
            const float FADE_MULT = FADE_MULT_DEFAULT_OUT_, const bool WILL_KILL_AFTER = true);

        music_update_status::Enum UpdateTime(const float ELAPSED_TIME_SECONDS);

        friend bool operator==(const MusicOperator & L, const MusicOperator & R);

    public:
        static const float VOLUME_USE_GLOBAL_;
        static const float FADE_MULT_IMMEDIATE_;
        static const float FADE_MULT_DEFAULT_IN_;
        static const float FADE_MULT_DEFAULT_OUT_;

    private:
        MusicInfo info_;
        float targetVolume_;
        float fadeInMult_;
        float fadeOutMult_;
        bool killAfterFadeOut_;
        MusicUPtr_t musicUPtr_;
    };

    bool operator==(const MusicOperator & L, const MusicOperator & R);

    inline bool operator!=(const MusicOperator & L, const MusicOperator & R) { return !(L == R); }
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_MUSIC_OPERATOR_HPP_INCLUDED
