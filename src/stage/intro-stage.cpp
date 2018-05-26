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

#include "sfml-util/display.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/music-operator.hpp"
#include "sfml-util/sfml-util.hpp"
#include "sfml-util/sound-manager.hpp"

#include "game/game-data-file.hpp"

#include "misc/real.hpp"

namespace heroespath
{
namespace stage
{

    IntroStage::IntroStage()
        : Stage(
              "Intro",
              0.0f,
              0.0f,
              sfml_util::Display::Instance()->GetWinWidth(),
              sfml_util::Display::Instance()->GetWinHeight(),
              {},
              true)
        , titleTexture_()
        , titleSprite_()
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
        sfml_util::Loaders::Texture(
            titleTexture_,
            game::GameDataFile::Instance()->GetMediaPath("media-images-title-intro"));

        titleSprite_.setTexture(titleTexture_);

        auto const SCALE{ sfml_util::MapByRes(0.4f, 2.5f) };
        titleSprite_.setScale(SCALE, SCALE);

        PositionTitleImage();
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
        auto const NEW_SCALE{ titleSprite_.getScale().x
                              * (1.0f + (ELAPSED_TIME_SECONDS * 0.028f)) };

        titleSprite_.setScale(NEW_SCALE, NEW_SCALE);
        PositionTitleImage();
    }

    void IntroStage::PositionTitleImage()
    {
        auto const TITLE_POS_LEFT{ (StageRegionWidth() * 0.5f)
                                   - (titleSprite_.getGlobalBounds().width * 0.5f) };

        auto const TITLE_POS_TOP{ (StageRegionHeight() * 0.5f)
                                  - (titleSprite_.getGlobalBounds().height * 0.5f) };

        titleSprite_.setPosition(TITLE_POS_LEFT, TITLE_POS_TOP);
    }
} // namespace stage
} // namespace heroespath
