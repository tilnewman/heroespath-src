// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// inn-stage.cpp
//
#include "inn-stage.hpp"

#include "sfml-util/animation-factory.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/gui/gui-elements.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/ouroboros.hpp"
#include "sfml-util/sfml-util.hpp"
#include "sfml-util/sliders.hpp"
#include "sfml-util/tile.hpp"

#include "misc/real.hpp"

#include "popup/popup-manager.hpp"

#include "game/game-data-file.hpp"
#include "game/loop-manager.hpp"

#include "game/game-state-factory.hpp"

namespace heroespath
{
namespace stage
{

    InnStage::InnStage()
        : Stage("Inn")
        , SCREEN_WIDTH_(sfml_util::Display::Instance()->GetWinWidth())
        , SCREEN_HEIGHT_(sfml_util::Display::Instance()->GetWinHeight())
        , titleSymbolTexture_()
        , titleSymbolSprite_()
        , backgroundImage_("media-images-backgrounds-tile-darkknot")
        , candleTexture_()
        , candleSprite_()
        , candleAnimUPtr_()
        , ouroborosUPtr_()
        , bottomSymbol_()
    {}

    InnStage::~InnStage() { Stage::ClearAllEntities(); }

    void InnStage::Setup()
    {
        // title symbol
        sfml_util::Loaders::Texture(
            titleSymbolTexture_,
            game::GameDataFile::Instance()->GetMediaPath("media-images-gui-accents-symbol2"));

        titleSymbolSprite_.setTexture(titleSymbolTexture_);
        titleSymbolSprite_.setScale(0.60f, 0.60f);

        titleSymbolSprite_.setPosition(
            (SCREEN_WIDTH_ * 0.5f) - (titleSymbolSprite_.getGlobalBounds().width * 0.5f), 10.0f);

        // ouroboros
        ouroborosUPtr_ = std::make_unique<sfml_util::Ouroboros>("InnStage's");
        EntityAdd(ouroborosUPtr_.get());

        // candle
        sfml_util::Loaders::Texture(
            candleTexture_, game::GameDataFile::Instance()->GetMediaPath("media-images-candle"));

        candleSprite_.setTexture(candleTexture_);
        candleSprite_.setPosition(SCREEN_WIDTH_ - 200.0f, SCREEN_HEIGHT_ - 200.0f);

        // candle anim
        candleAnimUPtr_
            = sfml_util::AnimationFactory::Make(sfml_util::Animations::CandleFlame, 1.0f, 0.05f);

        candleAnimUPtr_->SetEntityPos(SCREEN_WIDTH_ - 225.0f, SCREEN_HEIGHT_ - 290.0f);

        EntityAdd(candleAnimUPtr_.get());
    }

    void InnStage::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        target.draw(backgroundImage_, STATES);
        target.draw(bottomSymbol_, STATES);
        Stage::Draw(target, STATES);
    }
} // namespace stage
} // namespace heroespath
