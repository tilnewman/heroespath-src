//
// intro-stage.cpp
//
#include "intro-stage.hpp"

#include "sfml-util/sfml-util.hpp"
#include "utilz/real.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/sound-manager.hpp"
#include "sfml-util/music-operator.hpp"

#include "game/game-data-file.hpp"


namespace game
{
namespace stage
{

    IntroStage::IntroStage()
    :
        Stage        ("Intro", 0.0f, 0.0f, sfml_util::Display::Instance()->GetWinWidth(), sfml_util::Display::Instance()->GetWinHeight()),
        titleTexture_(),
        titleSprite_ (),
        initialDrawHoldCounter_(0)
    {}


    IntroStage::~IntroStage()
    {
        //If the theme music volume was changed just because this was the
        //Intro Stage, then set it back again once leaving the Intro Stage.
        sfml_util::MusicOperatorSPtr_t musicOperatorSPtr(sfml_util::SoundManager::Instance()->MusicOperator(sfml_util::music::Theme));
        if (musicOperatorSPtr.get() != nullptr)
        {
            const float CURRENT_VOLUME(musicOperatorSPtr->Volume());
            const float INTENDED_VOLUME(sfml_util::SoundManager::Instance()->MusicVolume());
            if (utilz::IsRealClose(CURRENT_VOLUME, INTENDED_VOLUME) == false)
            {
                if (utilz::IsRealClose(INTENDED_VOLUME, 0.0f))
                    musicOperatorSPtr->VolumeFadeOut();
                else
                    musicOperatorSPtr->VolumeFadeTo(INTENDED_VOLUME, ((CURRENT_VOLUME < INTENDED_VOLUME) ? sfml_util::MusicOperator::FADE_MULT_DEFAULT_IN_ : sfml_util::MusicOperator::FADE_MULT_DEFAULT_OUT_));
            }
        }
    }


    void IntroStage::Setup()
    {
        sfml_util::LoadImageOrTexture<sf::Texture>(titleTexture_, GameDataFile::Instance()->GetMediaPath("media-images-title-paper"));
        titleTexture_.setSmooth(true);

        titleSprite_.setTexture(titleTexture_);
        const float SCALE(sfml_util::MapByRes(0.75f, 3.0f));
        titleSprite_.setScale(SCALE, SCALE);
        titleSprite_.setPosition((sfml_util::Display::Instance()->GetWinWidth()  * 0.5f) - (titleSprite_.getGlobalBounds().width * 0.5f),
                                 (sfml_util::Display::Instance()->GetWinHeight() * 0.5f) - (titleSprite_.getGlobalBounds().height * 0.5f));
    }


    void IntroStage::Draw(sf::RenderTarget & target, sf::RenderStates states)
    {
        //This counter eliminates the flicker caused by the Loop's Fade in feature
        //that does not clear the screen for the first few draw loops.
        if (initialDrawHoldCounter_ < 5)
            ++initialDrawHoldCounter_;
        else
            target.draw(titleSprite_, states);
    }


    void IntroStage::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        const float NEW_SCALE(titleSprite_.getScale().x * (1.0f + (ELAPSED_TIME_SECONDS * 0.028f)));
        titleSprite_.setScale(NEW_SCALE, NEW_SCALE);

        titleSprite_.setPosition((sfml_util::Display::Instance()->GetWinWidth()  * 0.5f) - (titleSprite_.getGlobalBounds().width * 0.5f),
                                 (sfml_util::Display::Instance()->GetWinHeight() * 0.5f) - (titleSprite_.getGlobalBounds().height * 0.5f));
    }

}
}
