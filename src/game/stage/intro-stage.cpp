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
// intro-stage.cpp
//
#include "intro-stage.hpp"

#include "sfml-util/sfml-util.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/sound-manager.hpp"
#include "sfml-util/music-operator.hpp"

#include "game/game-data-file.hpp"

#include "misc/real.hpp"


namespace game
{
namespace stage
{

    IntroStage::IntroStage()
    :
        Stage(
            "Intro",
            0.0f,
            0.0f,
            sfml_util::Display::Instance()->GetWinWidth(),
            sfml_util::Display::Instance()->GetWinHeight()),
        titleTexture_(),
        titleSprite_ (),
        initialDrawHoldCounter_(0)
    {}


    IntroStage::~IntroStage()
    {
        //If the theme music volume was changed just because this was the
        //Intro Stage, then set it back again once leaving the Intro Stage.
        sfml_util::SoundManager::Instance()->MusicVolumeFadeToCurrent(sfml_util::music::Theme);

        ClearAllEntities();
    }


    void IntroStage::Setup()
    {
        sfml_util::LoadTexture(titleTexture_,
            GameDataFile::Instance()->GetMediaPath("media-images-title-intro"));

        titleSprite_.setTexture(titleTexture_);

        auto const SCALE{ sfml_util::MapByRes(0.4f, 2.5f) };
        titleSprite_.setScale(SCALE, SCALE);

        PositionTitleImage();
    }


    void IntroStage::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        //This counter eliminates the flicker caused by the Loop's Fade in feature
        //that does not clear the screen for the first few draw loops.
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
        auto const NEW_SCALE{
            titleSprite_.getScale().x * (1.0f + (ELAPSED_TIME_SECONDS * 0.028f)) };

        titleSprite_.setScale(NEW_SCALE, NEW_SCALE);
        PositionTitleImage();
    }


    void IntroStage::PositionTitleImage()
    {
        auto const TITLE_POS_LEFT{
            (StageRegionWidth()  * 0.5f) - (titleSprite_.getGlobalBounds().width * 0.5f) };

        auto const TITLE_POS_TOP{
            (StageRegionHeight() * 0.5f) - (titleSprite_.getGlobalBounds().height * 0.5f) };

        titleSprite_.setPosition(TITLE_POS_LEFT, TITLE_POS_TOP);
    }

}
}
