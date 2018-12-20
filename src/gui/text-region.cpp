// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// text-region.cpp
//
#include "text-region.hpp"

#include "gui/box-entity.hpp"
#include "gui/text-renderer.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/log-macros.hpp"
#include "sfutil/position.hpp"
#include "sfutil/primitives.hpp"
#include "sfutil/size-and-scale.hpp"
#include "stage/i-stage.hpp"

#include <SFML/Graphics/RenderTexture.hpp>

#include <algorithm>

namespace heroespath
{
namespace gui
{

    TextRegion::TextRegion(const std::string & NAME)
        : Entity(std::string(NAME).append("_TextRegion(name_only_constructor)"), 0.0f, 0.0f)
        , boxEntityUPtr_()
        , sliderBarUPtr_()
        , stagePtrOpt_()
        , renderTextureUPtr_()
        , sprite_()
        , willDraw_(false)
        , regionOrig_()
        , textInfoOrig_()
    {}

    TextRegion::TextRegion(
        const std::string & NAME,
        const TextInfo & TEXT_INFO,
        const sf::FloatRect & REGION,
        const BoxEntityInfo & BOX_INFO,
        const stage::IStagePtrOpt_t & ISTAGE_PTR_OPT)
        : Entity(std::string(NAME).append("_TextRegion"), REGION)
        , boxEntityUPtr_()
        , sliderBarUPtr_()
        , stagePtrOpt_()
        , renderTextureUPtr_()
        , sprite_()
        , willDraw_(false)
        , regionOrig_()
        , textInfoOrig_()
    {
        Setup(TEXT_INFO, REGION, BOX_INFO, ISTAGE_PTR_OPT);
    }

    TextRegion::~TextRegion()
    {
        if (stagePtrOpt_)
        {
            if (sliderBarUPtr_)
            {
                stagePtrOpt_.value()->EntityRemove(sliderBarUPtr_);
            }

            if (boxEntityUPtr_)
            {
                stagePtrOpt_.value()->EntityRemove(boxEntityUPtr_);
            }
        }
    }

    const std::string TextRegion::HandleCallback(
        const SliderBar::Callback_t::Packet_t & PACKET, const std::string &)
    {
        if (!sliderBarUPtr_)
        {
            return "";
        }

        const auto TEXT_HEIGHT_ACTUAL { static_cast<float>(renderTextureUPtr_->getSize().y) };

        if (TEXT_HEIGHT_ACTUAL < GetEntityRegion().height)
        {
            return "";
        }

        const auto VERT_RANGE { (TEXT_HEIGHT_ACTUAL - GetEntityRegion().height) };

        const auto VISIBLE_TEXT_POS_TOP { (PACKET.PositionRatio() * VERT_RANGE) };

        const sf::IntRect VISIBLE_TEXTURE_RECT(
            0,
            static_cast<int>(VISIBLE_TEXT_POS_TOP),
            static_cast<int>(renderTextureUPtr_->getSize().x),
            static_cast<int>(GetEntityRegion().height));

        sprite_.setTextureRect(VISIBLE_TEXTURE_RECT);

        return "";
    }

    void TextRegion::Setup(
        const TextInfo & TEXT_INFO,
        const sf::FloatRect & REGION,
        const BoxEntityInfo & BOX_INFO,
        const stage::IStagePtrOpt_t & ISTAGE_PTR_OPT)
    {
        // set member states
        willDraw_ = false;
        stagePtrOpt_ = ISTAGE_PTR_OPT;

        // remember original values in case we need them for SetText()
        regionOrig_ = REGION;
        textInfoOrig_ = TEXT_INFO;

        // prevent color changes when clicking on text or on the scrollbar
        SetWillAcceptFocus(false);

        SetupRender(TEXT_INFO, REGION);
        SetupSliderbar(REGION);
        SetupEntityRegion(REGION);
        SetupBox(BOX_INFO, REGION);
        // EstablishWhichLinesToDraw(0.0f);
    }

    void TextRegion::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        if (false == willDraw_)
        {
            return;
        }

        // Don't draw the Box or SliderBar here.  They are being drawn by the stage.

        target.draw(sprite_, states);

        // target.draw(sfutil::MakeRectangleHollow(entityRegion_, sf::Color::Red),
        // states);
        //
        // target.draw(
        //    sfutil::MakeRectangleHollow(sprite_.getGlobalBounds(), sf::Color::Yellow),
        //    states);
    }

    void TextRegion::SetEntityPos(const float POS_LEFT, const float POS_TOP)
    {
        MoveEntityPos(POS_LEFT - entityRegion_.left, POS_TOP - entityRegion_.top);
    }

    void TextRegion::MoveEntityPos(const float HORIZ, const float VERT)
    {
        Entity::MoveEntityPos(HORIZ, VERT);

        sprite_.move(HORIZ, VERT);

        if (boxEntityUPtr_)
        {
            boxEntityUPtr_->MoveEntityPos(HORIZ, VERT);
        }

        if (sliderBarUPtr_)
        {
            sliderBarUPtr_->MoveEntityPos(HORIZ, VERT);
        }
    }

    void TextRegion::SetText(const std::string & NEW_TEXT)
    {
        textInfoOrig_.text = NEW_TEXT;

        gui::BoxEntityInfo boxInfo;
        if (boxEntityUPtr_)
        {
            boxInfo = boxEntityUPtr_->Info();
        }

        Setup(textInfoOrig_, regionOrig_, boxInfo, stagePtrOpt_);
    }

    void TextRegion::ShrinkEntityRegionToFitActualTextRegion()
    {
        Entity::SetEntityRegion(sprite_.getGlobalBounds());
    }

    const sf::FloatRect TextRegion::ActualTextRegion() const { return sprite_.getGlobalBounds(); }

    void TextRegion::SetupRender(const TextInfo & TEXT_INFO, const sf::FloatRect & REGION)
    {
        if (TEXT_INFO.IsValid() == false)
        {
            return;
        }

        willDraw_ = TextRenderer::ToSprite(TEXT_INFO, REGION, renderTextureUPtr_, sprite_);

        if (!renderTextureUPtr_)
        {
            willDraw_ = false;
        }
    }

    void TextRegion::SetupSliderbar(const sf::FloatRect & REGION)
    {
        // remove any existing sliderbar first
        if (stagePtrOpt_ && sliderBarUPtr_)
        {
            stagePtrOpt_.value()->EntityRemove(sliderBarUPtr_);
            sliderBarUPtr_.reset();
        }

        // only create a sliderbar if there is both a vert and horiz limit
        const auto HAS_BOTH_HORIZ_AND_VERT_LIMIT { (
            (REGION.width > 0.0f) && (REGION.height > 0.0f)) };

        if (HAS_BOTH_HORIZ_AND_VERT_LIMIT == false)
        {
            return;
        }

        if (!renderTextureUPtr_)
        {
            return;
        }

        const auto DOES_HEIGHT_EXCEED_LIMIT { (
            static_cast<float>(renderTextureUPtr_->getSize().y) > REGION.height) };

        if (DOES_HEIGHT_EXCEED_LIMIT == false)
        {
            return;
        }

        if (!stagePtrOpt_)
        {
            return;
        }

        const auto VISIBLE_TO_TOTAL_HEIGHT_RATIO { (
            REGION.height / static_cast<float>(renderTextureUPtr_->getSize().y)) };

        sliderBarUPtr_ = std::make_unique<SliderBar>(
            GetEntityName() + "'s_",
            sfutil::Right(REGION) - SliderBar::POS_OFFSET_HORIZ_,
            REGION.top + SliderBar::POS_OFFSET_VERT_,
            REGION.height - (SliderBar::POS_OFFSET_VERT_ * 2.0f),
            SliderStyle(Orientation::Vert, Brightness::Bright, true, true),
            SliderBar::Callback_t::IHandlerPtrOpt_t(this),
            0.0f,
            false,
            VISIBLE_TO_TOTAL_HEIGHT_RATIO);

        sliderBarUPtr_->PositionRatio(0.0f);
        stagePtrOpt_.value()->EntityAdd(sliderBarUPtr_, true);
    }

    void TextRegion::SetupEntityRegion(const sf::FloatRect & REGION_PARAM)
    {
        // set the entity region to the TEXT_REGION except for width and height, if either of those
        // is <= 0 then use the actual size of the rendered text
        const sf::FloatRect NEW_REGION(
            REGION_PARAM.left,
            REGION_PARAM.top,
            ((misc::IsRealZeroOrLess(REGION_PARAM.width)) ? sfutil::Width(sprite_)
                                                          : REGION_PARAM.width),
            ((misc::IsRealZeroOrLess(REGION_PARAM.height)) ? sfutil::Height(sprite_)
                                                           : REGION_PARAM.height));

        Entity::SetEntityRegion(NEW_REGION);
    }

    void TextRegion::SetupBox(const BoxEntityInfo & BOX_INFO, const sf::FloatRect & REGION)
    {
        // remove existing first
        if (stagePtrOpt_ && boxEntityUPtr_)
        {
            stagePtrOpt_.value()->EntityRemove(boxEntityUPtr_);
            boxEntityUPtr_.reset();
        }

        if (!stagePtrOpt_)
        {
            return;
        }

        if (BOX_INFO == BoxEntityInfo())
        {
            return;
        }

        boxEntityUPtr_ = std::make_unique<BoxEntity>("TextRegion's", REGION, BOX_INFO);
        stagePtrOpt_.value()->EntityAdd(boxEntityUPtr_);
    }

    void TextRegion::OnColorChange()
    {
        sprite_.setColor(entityFgColor_);

        if (boxEntityUPtr_)
        {
            boxEntityUPtr_->SetEntityColors(entityFocusColors_);
        }
    }

} // namespace gui
} // namespace heroespath
