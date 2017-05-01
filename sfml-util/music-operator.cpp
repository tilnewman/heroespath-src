//
// music-operator.cpp
//
#include "music-operator.hpp"

#include "utilz/real.hpp"
#include "sfml-util/sfml-audio.hpp"
#include "sfml-util/sound-manager.hpp"

#include "heroespath/log-macros.hpp"

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
            musicSPtr_->stop();
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
                return music_update_status::FadingIn;
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
                if (killAfterFadeOut_ && ((NEW_VOL < 0.0f) || sfml_util::IsRealClose(targetVolume_, 0.0f)))
                    return music_update_status::FadedOutKill;
                else
                    return music_update_status::FadedOut;
            }
            else
                return music_update_status::FadingOut;
        }

        if (musicSPtr_->getStatus() == sf::Music::Playing)
            return music_update_status::Playing;
        else
            return music_update_status::Stopped;
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
