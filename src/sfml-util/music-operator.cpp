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
// music-operator.cpp
//
#include "music-operator.hpp"

#include "sfml-util/sfml-audio.hpp"
#include "sfml-util/sound-manager.hpp"

#include "game/log-macros.hpp"

#include "misc/real.hpp"

#include <exception>
#include <sstream>
#include <tuple>


namespace sfml_util
{

    const std::string music_update_status::ToString(const music_update_status::Enum E)
    {
        switch (E)
        {
            case Stopped:       { return "Stopped"; }
            case Playing:       { return "Playing"; }
            case FadingOut:     { return "FadingOut"; }
            case FadedOut:      { return "FadedOut"; }
            case FadedOutKill:  { return "FadedOutKill"; }
            case FadingIn:      { return "FadingIn"; }
            case FadedIn:       { return "FadedIn"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "sfml_util::music_update_status::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    const float MusicOperator::VOLUME_USE_GLOBAL_(-1.0f); //any negative value will work here
    const float MusicOperator::FADE_MULT_IMMEDIATE_(-1.0f);   //""
    const float MusicOperator::FADE_MULT_DEFAULT_IN_(50.0f);
    const float MusicOperator::FADE_MULT_DEFAULT_OUT_(30.0f);


    MusicOperator::MusicOperator(const MusicInfo &              MUSIC_INFO,
                                 const sfml_util::MusicSPtr_t & MUSIC_SPTR,
                                 const float                    FADE_IN_MULT,
                                 const float                    TARGET_VOLUME)
    :
        info_            (MUSIC_INFO),
        targetVolume_    (TARGET_VOLUME),
        fadeInMult_      (FADE_IN_MULT),
        fadeOutMult_     (0.0f),
        killAfterFadeOut_(false),
        musicSPtr_       (MUSIC_SPTR)
    {
        info_.Duration(sfml_util::Time(musicSPtr_->getDuration()));
        musicSPtr_->setLoop(MUSIC_INFO.IsLooped());
    }


    MusicOperator::~MusicOperator()
    {
        if (musicSPtr_.get() != nullptr)
        {
            musicSPtr_->stop();
        }
    }


    void MusicOperator::VolumeFadeTo(const float TARGET_VOL, const float FADE_MULT)
    {
        targetVolume_ = TARGET_VOL;
        const float VOLUME_CURR(musicSPtr_->getVolume());

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
        if (fadeInMult_ < 0.0f)
        {
            musicSPtr_->setVolume(targetVolume_);
            fadeInMult_ = 0.0f;
            return music_update_status::FadedIn;
        }
        else if (fadeInMult_ > 0.0f)
        {
            const float NEW_VOL(musicSPtr_->getVolume() + (ELAPSED_TIME_SECONDS * fadeInMult_));
            musicSPtr_->setVolume(NEW_VOL);
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
            musicSPtr_->setVolume(targetVolume_);
            fadeOutMult_ = 0.0f;
            return music_update_status::FadedOut;
        }
        else if (fadeOutMult_ > 0.0f)
        {
            const float NEW_VOL(musicSPtr_->getVolume() - (ELAPSED_TIME_SECONDS * fadeOutMult_));
            musicSPtr_->setVolume(NEW_VOL);
            if (NEW_VOL < targetVolume_)
            {
                fadeOutMult_ = 0.0f;

                //did we just fade out completely?
                if (killAfterFadeOut_ && ((NEW_VOL < 0.0f) || misc::IsRealZero(targetVolume_)))
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

        if (musicSPtr_->getStatus() == sf::Music::Playing)
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
        return (std::tie(L.info_,
                         L.targetVolume_,
                         L.fadeInMult_,
                         L.fadeOutMult_,
                         L.killAfterFadeOut_,
                         L.musicSPtr_)
            ==
            std::tie(R.info_,
                     R.targetVolume_,
                     R.fadeInMult_,
                     R.fadeOutMult_,
                     R.killAfterFadeOut_,
                     R.musicSPtr_));
    }

}
