// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// music-operator.cpp
//
#include "music-operator.hpp"

#include "sfml-util/sfml-audio.hpp"
#include "sfml-util/sound-manager.hpp"

#include "log/log-macros.hpp"

#include "misc/real.hpp"

#include <exception>
#include <sstream>
#include <tuple>

namespace heroespath
{
namespace sfml_util
{

    const std::string music_update_status::ToString(const music_update_status::Enum E)
    {
        switch (E)
        {
            case Stopped:
            {
                return "Stopped";
            }
            case Playing:
            {
                return "Playing";
            }
            case FadingOut:
            {
                return "FadingOut";
            }
            case FadedOut:
            {
                return "FadedOut";
            }
            case FadedOutKill:
            {
                return "FadedOutKill";
            }
            case FadingIn:
            {
                return "FadingIn";
            }
            case FadedIn:
            {
                return "FadedIn";
            }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "sfml_util::music_update_status::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }

    const float MusicOperator::VOLUME_USE_GLOBAL_(-1.0f); // any negative value will work here
    const float MusicOperator::FADE_MULT_IMMEDIATE_(-1.0f); //""
    const float MusicOperator::FADE_MULT_DEFAULT_IN_(50.0f);
    const float MusicOperator::FADE_MULT_DEFAULT_OUT_(30.0f);

    MusicOperator::MusicOperator(
        const MusicInfo & MUSIC_INFO,
        MusicUPtr_t MUSIC_UPTR,
        const float FADE_IN_MULT,
        const float TARGET_VOLUME)
        : info_(MUSIC_INFO)
        , targetVolume_(TARGET_VOLUME)
        , fadeInMult_(FADE_IN_MULT)
        , fadeOutMult_(0.0f)
        , killAfterFadeOut_(false)
        , musicUPtr_(std::move(MUSIC_UPTR))
    {
        if (IsValid())
        {
            info_.Duration(sfml_util::Time(musicUPtr_->getDuration()));
            musicUPtr_->setLoop(MUSIC_INFO.IsLooped());
        }
    }

    MusicOperator::MusicOperator(MusicOperator && MO)
        : info_(MO.info_)
        , targetVolume_(MO.targetVolume_)
        , fadeInMult_(MO.fadeInMult_)
        , fadeOutMult_(MO.fadeOutMult_)
        , killAfterFadeOut_(MO.killAfterFadeOut_)
        , musicUPtr_(std::move(MO.musicUPtr_))
    {}

    MusicOperator & MusicOperator::operator=(MusicOperator && MO)
    {
        if (&MO != this)
        {
            info_ = MO.info_;
            targetVolume_ = MO.targetVolume_;
            fadeInMult_ = MO.fadeInMult_;
            fadeOutMult_ = MO.fadeOutMult_;
            killAfterFadeOut_ = MO.killAfterFadeOut_;
            musicUPtr_ = std::move(MO.musicUPtr_);
        }

        return *this;
    }

    bool MusicOperator::IsValid() const
    {
        return (
            (info_.Which() != music::Count) && (info_.Which() != music::All)
            && (info_.Which() != music::None) && (musicUPtr_));
    }

    MusicOperator::~MusicOperator()
    {
        if (musicUPtr_)
        {
            musicUPtr_->stop();
        }
    }

    void MusicOperator::VolumeFadeTo(const float TARGET_VOL, const float FADE_MULT)
    {
        if (IsValid())
        {
            targetVolume_ = TARGET_VOL;
            const float VOLUME_CURR(musicUPtr_->getVolume());

            if (VOLUME_CURR < targetVolume_)
            {
                fadeOutMult_ = 0.0f;
                fadeInMult_ = FADE_MULT;
            }
            else
            {
                fadeOutMult_ = FADE_MULT;
                fadeInMult_ = 0.0f;
                killAfterFadeOut_ = false;
            }
        }
    }

    void MusicOperator::VolumeFadeToGlobal(const float FADE_MULT)
    {
        VolumeFadeTo(sfml_util::SoundManager::Instance()->MusicVolume(), FADE_MULT);
    }

    void MusicOperator::VolumeFadeOut(const float FADE_MULT, const bool WILL_KILL_AFTER)
    {
        targetVolume_ = 0.0f;
        fadeInMult_ = 0.0f;
        fadeOutMult_ = FADE_MULT;
        killAfterFadeOut_ = WILL_KILL_AFTER;
    }

    music_update_status::Enum MusicOperator::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        if (IsValid() == false)
        {
            return music_update_status::Stopped;
        }

        if (fadeInMult_ < 0.0f)
        {
            Volume(targetVolume_);
            fadeInMult_ = 0.0f;
            return music_update_status::FadedIn;
        }
        else if (fadeInMult_ > 0.0f)
        {
            const float NEW_VOL(musicUPtr_->getVolume() + (ELAPSED_TIME_SECONDS * fadeInMult_));
            Volume(NEW_VOL);
            if (NEW_VOL > targetVolume_)
            {
                fadeInMult_ = 0.0f;
                return music_update_status::FadedIn;
            }
            else
            {
                return music_update_status::FadingIn;
            }
        }
        else if (fadeOutMult_ < 0.0f)
        {
            Volume(targetVolume_);
            fadeOutMult_ = 0.0f;
            return music_update_status::FadedOut;
        }
        else if (fadeOutMult_ > 0.0f)
        {
            const float NEW_VOL(musicUPtr_->getVolume() - (ELAPSED_TIME_SECONDS * fadeOutMult_));
            Volume(NEW_VOL);
            if (NEW_VOL < targetVolume_)
            {
                fadeOutMult_ = 0.0f;

                auto const DID_FADE_TO_ZERO{ (NEW_VOL < 0.0f) || misc::IsRealZero(targetVolume_) };

                if (killAfterFadeOut_ && DID_FADE_TO_ZERO)
                {
                    return music_update_status::FadedOutKill;
                }
                else
                {
                    return music_update_status::FadedOut;
                }
            }
            else
            {
                return music_update_status::FadingOut;
            }
        }

        if (musicUPtr_->getStatus() == sf::Music::Playing)
        {
            return music_update_status::Playing;
        }
        else
        {
            return music_update_status::Stopped;
        }
    }

    bool operator==(const MusicOperator & L, const MusicOperator & R)
    {
        return (
            std::tie(
                L.info_,
                L.targetVolume_,
                L.fadeInMult_,
                L.fadeOutMult_,
                L.killAfterFadeOut_,
                L.musicUPtr_)
            == std::tie(
                   R.info_,
                   R.targetVolume_,
                   R.fadeInMult_,
                   R.fadeOutMult_,
                   R.killAfterFadeOut_,
                   R.musicUPtr_));
    }
} // namespace sfml_util
} // namespace heroespath
