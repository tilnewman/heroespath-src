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

#include "game/game-state-factory.hpp"
#include "game/loop-manager.hpp"
#include "misc/real.hpp"
#include "popup/popup-manager.hpp"
#include "sfml-util/animation-factory.hpp"
#include "sfml-util/gui/gui-images.hpp"
#include "sfml-util/ouroboros.hpp"
#include "sfml-util/sfml-util-display.hpp"
#include "sfml-util/sfml-util-fitting.hpp"

namespace heroespath
{
namespace stage
{

    InnStage::InnStage()
        : Stage(
              "Inn",
              { sfml_util::GuiFont::Default,
                sfml_util::GuiFont::System,
                sfml_util::GuiFont::SystemCondensed,
                sfml_util::GuiFont::Number,
                sfml_util::GuiFont::Handwriting },
              true)
        , stageTitle_()
        , backgroundBox_(
              "InnStage'sBackground",
              StageRegion(),
              sfml_util::gui::BoxEntityInfo(sfml_util::CachedTexture(
                  "media-images-backgrounds-tile-darkknot",
                  sfml_util::ImageOpt::Default | sfml_util::ImageOpt::Repeated)))
        , candleCachedTexture_("media-images-candle")
        , candleSprite_(candleCachedTexture_.Get())
        , candleAnimUPtr_()
        , ouroborosUPtr_()
        , bottomSymbol_()
    {}

    InnStage::~InnStage() { Stage::ClearAllEntities(); }

    void InnStage::Setup()
    {
        // ouroboros
        ouroborosUPtr_ = std::make_unique<sfml_util::Ouroboros>("InnStage's");
        EntityAdd(ouroborosUPtr_.get());

        // candle image
        const sf::Vector2f CANDLE_POS_V(
            StageRegionWidth() - sfml_util::ScreenRatioToPixelsHoriz(0.16f),
            StageRegionHeight() - sfml_util::ScreenRatioToPixelsVert(0.222f));

        const sf::Vector2f CANDLE_SIZE_CONSTRAINTS_V(
            sfml_util::ScreenRatioToPixelsHoriz(0.065f), 0.0f);

        const sf::FloatRect CANDLE_REGION_CONSTRAINTS(CANDLE_POS_V, CANDLE_SIZE_CONSTRAINTS_V);

        sfml_util::FitAndCenterTo(candleSprite_, CANDLE_REGION_CONSTRAINTS);

        // candle anim
        candleAnimUPtr_
            = sfml_util::AnimationFactory::Make(sfml_util::Animations::CandleFlame, 1.0f, 0.05f);

        candleAnimUPtr_->SetEntityPos(CANDLE_POS_V - sf::Vector2f(25.0f, 90.0f));

        EntityAdd(candleAnimUPtr_.get());
    }

    void InnStage::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        target.draw(backgroundBox_, STATES);
        target.draw(bottomSymbol_, STATES);
        Stage::Draw(target, STATES);
    }

} // namespace stage
} // namespace heroespath
