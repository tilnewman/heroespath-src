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
#include "misc/real.hpp"
#include "popup/popup-manager.hpp"
#include "sfml-util/animation-factory.hpp"
#include "sfml-util/gui-images.hpp"
#include "sfml-util/ouroboros.hpp"
#include "sfutil/display.hpp"
#include "sfutil/fitting.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

namespace heroespath
{
namespace stage
{

    InnStage::InnStage()
        : StageBase(
            "Inn",
            { gui::GuiFont::Default,
              gui::GuiFont::System,
              gui::GuiFont::SystemCondensed,
              gui::GuiFont::Number,
              gui::GuiFont::Handwriting },
            true)
        , stageTitle_()
        , backgroundBox_(
              "InnStage'sBackground",
              StageRegion(),
              gui::BoxEntityInfo(gui::CachedTexture(
                  "media-images-backgrounds-tile-darkknot",
                  gui::ImageOpt::Default | gui::ImageOpt::Repeated)))
        , candleCachedTexture_("media-images-candle")
        , candleSprite_(candleCachedTexture_.Get())
        , candleAnimUPtr_()
        , ouroborosUPtr_()
        , bottomSymbol_()
    {}

    InnStage::~InnStage() { StageBase::ClearAllEntities(); }

    void InnStage::Setup()
    {
        // ouroboros
        ouroborosUPtr_ = std::make_unique<gui::Ouroboros>("InnStage's");
        EntityAdd(ouroborosUPtr_.get());

        // candle image
        const sf::Vector2f CANDLE_POS_V(
            StageRegion().width - sfutil::ScreenRatioToPixelsHoriz(0.16f),
            StageRegion().height - sfutil::ScreenRatioToPixelsVert(0.222f));

        const sf::Vector2f CANDLE_SIZE_CONSTRAINTS_V(
            sfutil::ScreenRatioToPixelsHoriz(0.065f), 0.0f);

        const sf::FloatRect CANDLE_REGION_CONSTRAINTS(CANDLE_POS_V, CANDLE_SIZE_CONSTRAINTS_V);

        sfutil::FitAndCenterTo(candleSprite_, CANDLE_REGION_CONSTRAINTS);

        // candle anim
        candleAnimUPtr_ = gui::AnimationFactory::Make(gui::Animations::CandleFlame, 1.0f, 0.05f);

        candleAnimUPtr_->SetEntityPos(CANDLE_POS_V - sf::Vector2f(25.0f, 90.0f));

        EntityAdd(candleAnimUPtr_.get());
    }

    void InnStage::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        target.draw(backgroundBox_, STATES);
        target.draw(bottomSymbol_, STATES);
        StageBase::Draw(target, STATES);
    }

} // namespace stage
} // namespace heroespath
