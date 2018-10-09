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

#include "misc/config-file.hpp"
#include "misc/real.hpp"
#include "sfml-util/music-operator.hpp"
#include "sfml-util/sound-manager.hpp"
#include "sfutil/center.hpp"
#include "sfutil/display.hpp"
#include "sfutil/fitting.hpp"

namespace heroespath
{
namespace stage
{

    IntroStage::IntroStage()
        : Stage("Intro", {}, true)
        , titleCachedTexture_("media-images-title-intro")
        , titleSprite_(titleCachedTexture_.Get())
        , initialDrawHoldCounter_(0)
    {}

    IntroStage::~IntroStage()
    {
        // If the theme music volume was changed just because this was the
        // Intro Stage, then set it back again once leaving the Intro Stage.
        sfml_util::SoundManager::Instance()->MusicVolumeFadeToCurrent(sfml_util::music::Theme);

        Stage::ClearAllEntities();
    }

    void IntroStage::Setup()
    {
        const sf::Vector2f TITLE_IMAGE_INITIAL_SIZE_CONSTRAINTS_V(
            sfutil::ScreenRatioToPixelsHoriz(0.33f), 0.0f);

        sfutil::Fit(titleSprite_, TITLE_IMAGE_INITIAL_SIZE_CONSTRAINTS_V);
        sfutil::Center(titleSprite_);
    }

    void IntroStage::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        // This counter eliminates the flicker caused by the Loop's Fade in feature
        // that does not clear the screen for the first few draw loops.
        if (initialDrawHoldCounter_ < 5)
        {
            ++initialDrawHoldCounter_;
        }
        else
        {
            Stage::Draw(target, STATES);
            target.draw(titleSprite_, STATES);
        }
    }

    void IntroStage::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        const auto NEW_SCALE { titleSprite_.getScale().x
                               * (1.0f + (ELAPSED_TIME_SECONDS * 0.028f)) };

        titleSprite_.setScale(NEW_SCALE, NEW_SCALE);
        sfutil::Center(titleSprite_);
    }

} // namespace stage
} // namespace heroespath
