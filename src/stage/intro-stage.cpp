// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// intro-stage.cpp
//
#include "intro-stage.hpp"

#include "gui/music-operator.hpp"
#include "gui/sound-manager.hpp"
#include "misc/config-file.hpp"
#include "misc/real.hpp"
#include "sfutil/center.hpp"
#include "sfutil/display.hpp"
#include "sfutil/fitting.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

namespace heroespath
{
namespace stage
{

    IntroStage::IntroStage()
        : StageBase("Intro", {}, true)
        , backgroundMusic_(gui::music::Theme)
        , musicVolumeOrig_(gui::SoundManager::Instance()->MusicVolume())
        , timeFromStartToMusicSec_(1.5f)
        , timeFromMusicToAnimSec_(2.0f)
        , timeFromAnimToExitSec_(4.0f)
        , imageScaleSpeed_(0.028f)
        , titleCachedTexture_("media-images-title-intro")
        , titleSprite_(titleCachedTexture_.Get())
        , elapsedSec_(0.0f)
        , hasMusicStarted_(false)
        , hasAnimStarted_(false)
        , hasExited_(false)
        , wasMusicVolumeChanged_(false)
    {}

    IntroStage::~IntroStage()
    {
        if (wasMusicVolumeChanged_)
        {
            gui::SoundManager::Instance()->MusicVolume(
                { backgroundMusic_ }, musicVolumeOrig_, gui::MusicOperator::FADE_MULT_DEFAULT_IN_);
        }

        StageBase::ClearAllEntities();
    }

    void IntroStage::Setup()
    {
        const sf::Vector2f TITLE_IMAGE_INITIAL_SIZE_CONSTRAINTS_V(
            sfutil::ScreenRatioToPixelsHoriz(0.33f), 0.0f);

        sfutil::Fit(titleSprite_, TITLE_IMAGE_INITIAL_SIZE_CONSTRAINTS_V);
        sfutil::Center(titleSprite_);
    }

    void IntroStage::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        if (hasAnimStarted_)
        {
            target.draw(titleSprite_, states);
        }
    }

    void IntroStage::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        elapsedSec_ += ELAPSED_TIME_SECONDS;

        if (!hasMusicStarted_ && (elapsedSec_ > timeFromStartToMusicSec_))
        {
            hasMusicStarted_ = true;

            const auto VOLUME_MIN { 15.0f };

            auto volume { musicVolumeOrig_ };
            if (volume < VOLUME_MIN)
            {
                volume = VOLUME_MIN;
                wasMusicVolumeChanged_ = true;
            }

            gui::SoundManager::Instance()->MusicStart(
                backgroundMusic_, gui::MusicOperator::FADE_MULT_DEFAULT_IN_, volume);
        }

        if (!hasAnimStarted_
            && (elapsedSec_ > (timeFromStartToMusicSec_ + timeFromMusicToAnimSec_)))
        {
            hasAnimStarted_ = true;
        }

        if (hasAnimStarted_)
        {
            const auto NEW_SCALE { titleSprite_.getScale().x
                                   * (1.0f + (ELAPSED_TIME_SECONDS * imageScaleSpeed_)) };

            titleSprite_.setScale(NEW_SCALE, NEW_SCALE);
            sfutil::Center(titleSprite_);
        }

        if (!hasExited_
            && (elapsedSec_
                > (timeFromStartToMusicSec_ + timeFromMusicToAnimSec_ + timeFromAnimToExitSec_)))
        {
            hasExited_ = true;
            TransitionTo(stage::Stage::Menu);
        }
    }

    bool IntroStage::KeyPress(const sf::Event::KeyEvent &)
    {
        TransitionTo(stage::Stage::Menu);
        return false;
    }

    void IntroStage::UpdateMouseDown(const sf::Vector2f &) { TransitionTo(stage::Stage::Menu); }

} // namespace stage
} // namespace heroespath
