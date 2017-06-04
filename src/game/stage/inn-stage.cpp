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
// inn-stage.cpp
//
#include "inn-stage.hpp"

#include "sfml-util/sfml-util.hpp"
#include "misc/real.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/tile.hpp"
#include "sfml-util/sliders.hpp"
#include "sfml-util/animation.hpp"
#include "sfml-util/gui/gui-elements.hpp"
#include "sfml-util/gui/popup-manager.hpp"

#include "game/ouroboros.hpp"
#include "game/game-data-file.hpp"
#include "game/state/game-state-factory.hpp"
#include "game/loop-manager.hpp"


namespace game
{
namespace stage
{

    InnStage::InnStage()
    :
        Stage              ("Inn"),
        SCREEN_WIDTH_      (sfml_util::Display::Instance()->GetWinWidth()),
        SCREEN_HEIGHT_     (sfml_util::Display::Instance()->GetWinHeight()),
        titleSymbolTexture_(),
        titleSymbolSprite_ (),
        backgroundImage_   ("media-images-backgrounds-tile-darkknot"),
        candleTexture_     (),
        candleSprite_      (),
        candleAnimSPtr_    (),
        ouroborosUPtr_     (),
        bottomSymbol_      ()
    {}


    InnStage::~InnStage()
    {
        ClearAllEntities();
    }


    void InnStage::Setup()
    {
        //title symbol
        sfml_util::LoadImageOrTexture<sf::Texture>(titleSymbolTexture_, GameDataFile::Instance()->GetMediaPath("media-images-gui-accents-symbol2"));
        titleSymbolSprite_.setTexture(titleSymbolTexture_);
        titleSymbolSprite_.setScale(0.60f, 0.60f);
        titleSymbolSprite_.setPosition((SCREEN_WIDTH_ * 0.5f) - (titleSymbolSprite_.getGlobalBounds().width * 0.5f), 10.0f);

        //ouroboros
        ouroborosUPtr_ = std::make_unique<Ouroboros>("InnStage's");
        EntityAdd(ouroborosUPtr_.get());

        //candle
        sfml_util::LoadImageOrTexture<sf::Texture>(candleTexture_, GameDataFile::Instance()->GetMediaPath("media-images-candle"));
        candleSprite_.setTexture(candleTexture_);
        candleSprite_.setPosition(SCREEN_WIDTH_ - 200.0f, SCREEN_HEIGHT_ - 200.0f);

        //candle anim
        candleAnimSPtr_ = std::make_shared<sfml_util::SingleTextureAnimation>(
            "Candle",
            GameDataFile::Instance()->GetMediaPath("media-anim-images-candleflame"),
            SCREEN_WIDTH_ - 225.0f,
            SCREEN_HEIGHT_ - 290.0f,
            128,
            128,
            0.05f,
            64,
            sf::BlendAdd);

        EntityAdd(candleAnimSPtr_.get());
    }


    void InnStage::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        target.draw(backgroundImage_, STATES);
        bottomSymbol_.Draw(target, STATES);
        Stage::Draw(target, STATES);
    }

}
}
