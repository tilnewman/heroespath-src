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
#include "sfutil/common.hpp"
#include "sfutil/primitives.hpp"
#include "sfutil/scale.hpp"
#include "stage/i-stage.hpp"

#include <SFML/Graphics/RenderTexture.hpp>

#include <algorithm>

namespace heroespath
{
namespace gui
{

    // this constructor's initializer list does not intialize members, see Setup()
    TextRegion::TextRegion(
        const std::string & NAME,
        const TextInfo & TEXT_INFO,
        const sf::FloatRect & REGION,
        const BoxEntityInfo & BOX_INFO,
        const stage::IStagePtrOpt_t & ISTAGE_PTR_OPT)
        : Entity(
            NAME + "_TextRegion(\"" + misc::MakeLoggableString(TEXT_INFO.text, 10, false) + "\")",
            REGION)
        , textInfo_()
        , boxEntityUPtr_()
        , sliderBarUPtr_()
        , stagePtrOpt_()
        , renderTextureUPtr_()
        , sprite_()
        , willDraw_(false)
        , origSize_()
    {
        // text regions cannot do anything with focus, they can only take focus away from the
        // BoxEntity's underneath them that can and should be the ones changing with focus, so
        // prevent all TextRegions from taking focus here
        SetWillAcceptFocus(false);

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

        const auto TEXT_HEIGHT_ACTUAL { ActualTextRegion().height };

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
        const TextInfo & TEXT_INFO_PARAM,
        const sf::FloatRect & REGION_PARAM,
        const BoxEntityInfo & BOX_INFO_PARAM,
        const stage::IStagePtrOpt_t & ISTAGE_PTR_OPT)
    {
        if (TEXT_INFO_PARAM.text == " ")
        {
            M_HP_LOG_ERR(
                "TEXT_INFO.text=\" \", which should not be happening anymore."
                + M_HP_VAR_STR(GetEntityName()) + M_HP_VAR_STR(TEXT_INFO_PARAM.ToString()));
        }

        const auto REGION { (
            (REGION_PARAM == sf::FloatRect())
                ? sf::FloatRect(sfutil::Position(GetEntityRegion()), origSize_)
                : REGION_PARAM) };

        origSize_ = sfutil::Size(REGION);

        const auto BOX_INFO { (
            (BOX_INFO_PARAM == BoxEntityInfo() && boxEntityUPtr_) ? boxEntityUPtr_->Info()
                                                                  : BOX_INFO_PARAM) };

        if (ISTAGE_PTR_OPT)
        {
            stagePtrOpt_ = ISTAGE_PTR_OPT;
        }

        if (TEXT_INFO_PARAM != TextInfo())
        {
            textInfo_ = TEXT_INFO_PARAM;
        }

        willDraw_ = false;

        SetupRender(textInfo_, REGION);
        SetupSliderbar(REGION);
        SetupEntityRegion(REGION);
        SetupBox(BOX_INFO, REGION);
    }

    void TextRegion::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        if (false == willDraw_)
        {
            return;
        }

        // Don't draw the Box or SliderBar here.  They are being drawn by the stage.

        target.draw(sprite_, states);
    }

    void TextRegion::SetEntityPos(const float POS_LEFT, const float POS_TOP)
    {
        MoveEntityPos((POS_LEFT - entityRegion_.left), (POS_TOP - entityRegion_.top));
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
        if (NEW_TEXT == " ")
        {
            M_HP_LOG_ERR(
                "NEW_TEXT=\" \", which should not be happening anymore."
                + M_HP_VAR_STR(GetEntityName()) + M_HP_VAR_STR(textInfo_.ToString()));
        }

        gui::BoxEntityInfo boxInfo;
        if (boxEntityUPtr_)
        {
            boxInfo = boxEntityUPtr_->Info();
        }

        textInfo_.text = NEW_TEXT;

        Setup(
            textInfo_,
            sf::FloatRect(sfutil::Position(GetEntityRegion()), origSize_),
            boxInfo,
            stagePtrOpt_);
    }

    const sf::FloatRect TextRegion::ActualTextRegion() const { return sprite_.getGlobalBounds(); }

    void TextRegion::SetupRender(const TextInfo & TEXT_INFO, const sf::FloatRect & REGION)
    {
        if (TEXT_INFO.WillDraw() == false)
        {
            willDraw_ = false;
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

        if (!HAS_BOTH_HORIZ_AND_VERT_LIMIT || !willDraw_)
        {
            return;
        }

        const auto DOES_HEIGHT_EXCEED_LIMIT { (
            static_cast<float>(renderTextureUPtr_->getSize().y) > REGION.height) };

        if (!DOES_HEIGHT_EXCEED_LIMIT || !stagePtrOpt_)
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

    void TextRegion::SetupEntityRegion(const sf::FloatRect & REGION_ORIG)
    {
        const auto ACTUAL_REGION { ActualTextRegion() };

        // set the entity region to the TEXT_REGION except for width and height, if either of those
        // is <= 0 then use the actual size of the rendered text
        auto newRegion { REGION_ORIG };

        newRegion.width
            = ((misc::IsRealZeroOrLess(REGION_ORIG.width)) ? ACTUAL_REGION.width
                                                           : REGION_ORIG.width);

        newRegion.height
            = ((misc::IsRealZeroOrLess(REGION_ORIG.height)) ? ACTUAL_REGION.height
                                                            : REGION_ORIG.height);

        Entity::SetEntityRegion(newRegion);
    }

    void TextRegion::SetupBox(const BoxEntityInfo & BOX_INFO, const sf::FloatRect & REGION)
    {
        if (stagePtrOpt_ && boxEntityUPtr_)
        {
            stagePtrOpt_.value()->EntityRemove(boxEntityUPtr_);
            boxEntityUPtr_.reset();
        }

        if (!stagePtrOpt_ || (BOX_INFO == BoxEntityInfo()))
        {
            return;
        }

        boxEntityUPtr_ = std::make_unique<BoxEntity>(GetEntityName() + "'s_", REGION, BOX_INFO);
        stagePtrOpt_.value()->EntityAdd(boxEntityUPtr_);
    }

    void TextRegion::OnColorChange()
    {
        sprite_.setColor(entityFgColor_);
        textInfo_.color = entityFgColor_;

        if (boxEntityUPtr_)
        {
            boxEntityUPtr_->SetEntityColors(entityFocusColors_);
        }
    }

} // namespace gui
} // namespace heroespath
