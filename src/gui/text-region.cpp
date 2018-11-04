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
#include "misc/boost-string-includes.hpp"
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

    const unsigned int TextRegion::DEFAULT_NO_RESIZE_ { 0 };

    TextRegion::TextRegion(const std::string & NAME)
        : Entity(std::string(NAME).append("_TextRegion"), 0.0f, 0.0f)
        , boxEntityUPtr_()
        , sliderBarUPtr_()
        , stagePtrOpt_()
        , startLine_(0)
        , stopLine_(0)
        , regionOrig_()
        , textInfoOrig_()
        , smallFontSizeOrig_()
        , marginsOrig_()
        , allowScrollbarOrig_()
        , renderTextureUPtr_()
        , sprite_()
        , willDraw_(false)
    {}

    TextRegion::TextRegion(
        const std::string & NAME,
        const TextInfo & TEXT_INFO,
        const sf::FloatRect & REGION,
        const unsigned int SMALLER_FONT_SIZE,
        const BoxEntityInfo & BOX_INFO,
        const Margins & MARGINS)
        : Entity(std::string(NAME).append("_TextRegion"), REGION)
        , boxEntityUPtr_()
        , sliderBarUPtr_()
        , stagePtrOpt_()
        , startLine_(0)
        , stopLine_(0)
        , regionOrig_()
        , textInfoOrig_()
        , smallFontSizeOrig_()
        , marginsOrig_()
        , allowScrollbarOrig_()
        , renderTextureUPtr_()
        , sprite_()
        , willDraw_(false)
    {
        Setup(TEXT_INFO, REGION, SMALLER_FONT_SIZE, BOX_INFO, MARGINS);
    }

    TextRegion::TextRegion(
        const std::string & NAME,
        const TextInfo & TEXT_INFO,
        const sf::FloatRect & REGION,
        const stage::IStagePtr_t ISTAGE_PTR,
        const unsigned int SMALLER_FONT_SIZE,
        const BoxEntityInfo & BOX_INFO,
        const Margins & MARGINS)
        : Entity(std::string(NAME).append("_TextRegion"), REGION)
        , boxEntityUPtr_()
        , sliderBarUPtr_()
        , stagePtrOpt_(ISTAGE_PTR)
        , startLine_(0)
        , stopLine_(0)
        , regionOrig_()
        , textInfoOrig_()
        , smallFontSizeOrig_()
        , marginsOrig_()
        , allowScrollbarOrig_()
        , renderTextureUPtr_()
        , sprite_()
        , willDraw_(false)
    {
        Setup(TEXT_INFO, REGION, stagePtrOpt_, SMALLER_FONT_SIZE, BOX_INFO, MARGINS, true);
    }

    TextRegion::~TextRegion()
    {
        if (stagePtrOpt_)
        {
            if (sliderBarUPtr_)
            {
                stagePtrOpt_.value()->EntityRemove(sliderBarUPtr_);
            }

            stagePtrOpt_.value()->EntityRemove(boxEntityUPtr_);
        }
    }

    void TextRegion::Setup(
        const TextInfo & TEXT_INFO,
        const sf::FloatRect & REGION,
        const unsigned int SMALLER_FONT_SIZE,
        const BoxEntityInfo & BOX_INFO,
        const Margins & MARGINS)
    {
        Setup(TEXT_INFO, REGION, stagePtrOpt_, SMALLER_FONT_SIZE, BOX_INFO, MARGINS, false);
    }

    void TextRegion::Setup(
        const TextInfo & TEXT_INFO,
        const sf::FloatRect & REGION_ORIG,
        const stage::IStagePtrOpt_t & ISTAGE_PTR_OPT,
        const unsigned int SMALLER_FONT_SIZE,
        const BoxEntityInfo & BOX_INFO,
        const Margins & MARGINS,
        const bool WILL_ALLOW_SCROLLBAR)
    {
        // set member states
        willDraw_ = false;
        stagePtrOpt_ = ISTAGE_PTR_OPT;

        // remember original values in case we need them
        {
            regionOrig_ = REGION_ORIG;
            textInfoOrig_ = TEXT_INFO;
            smallFontSizeOrig_ = SMALLER_FONT_SIZE;
            marginsOrig_ = MARGINS;
            allowScrollbarOrig_ = WILL_ALLOW_SCROLLBAR;
        }

        // prevent color changes when clicking on text or on the scrollbar
        SetWillAcceptFocus(false);

        const auto REGION_ACTUAL { MARGINS.ApplyShrinkCopy(REGION_ORIG) };
        HandleSliderBar(RenderText(TEXT_INFO, REGION_ACTUAL, MARGINS, SMALLER_FONT_SIZE));
        SetupEntityRegion(REGION_ACTUAL);
        SetupBox(BOX_INFO);
        // EstablishWhichLinesToDraw(0.0f);
    }

    void TextRegion::HandleSliderBar(const SliderBarPtrOpt_t SLIDERBAR_PTR_OPT)
    {
        if (stagePtrOpt_)
        {
            if (sliderBarUPtr_)
            {
                stagePtrOpt_.value()->EntityRemove(sliderBarUPtr_);
                sliderBarUPtr_.reset();
            }

            if (SLIDERBAR_PTR_OPT)
            {
                sliderBarUPtr_.reset(SLIDERBAR_PTR_OPT.value());

                sliderBarUPtr_->SetCallbackHandler(SliderBar::Callback_t::IHandlerPtrOpt_t(this));

                sliderBarUPtr_->PositionRatio(0.0f);
                stagePtrOpt_.value()->EntityAdd(sliderBarUPtr_);
            }
        }
        else
        {
            if (SLIDERBAR_PTR_OPT)
            {
                delete SLIDERBAR_PTR_OPT.value();
            }
        }
    }

    void TextRegion::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        // Don't draw the Box or SliderBar here.  They are being drawn by the stage.

        if (willDraw_)
        {
            target.draw(sprite_, states);

            // target.draw(sfutil::MakeRectangleHollow(entityRegion_, sf::Color::Red),
            // states);
            //
            // target.draw(
            //    sfutil::MakeRectangleHollow(sprite_.getGlobalBounds(), sf::Color::Yellow),
            //    states);
        }
    }

    void TextRegion::SetEntityPos(const float POS_LEFT, const float POS_TOP)
    {
        MoveEntityPos(POS_LEFT - entityRegion_.left, POS_TOP - entityRegion_.top);
    }

    void TextRegion::MoveEntityPos(const float HORIZ, const float VERT)
    {
        Entity::MoveEntityPos(HORIZ, VERT);
        boxEntityUPtr_->MoveEntityPos(HORIZ, VERT);
        sprite_.move(HORIZ, VERT);

        if (sliderBarUPtr_)
        {
            sliderBarUPtr_->MoveEntityPos(HORIZ, VERT);
        }
    }

    const std::string TextRegion::HandleCallback(
        const SliderBar::Callback_t::Packet_t & PACKET, const std::string &)
    {
        if (sliderBarUPtr_)
        {
            EstablishWhichLinesToDraw(PACKET.PositionRatio());
        }

        return "";
    }

    void TextRegion::ShrinkEntityRegionToFitText() { SetEntityRegion(sprite_.getGlobalBounds()); }

    void TextRegion::Append(const TextRegion &)
    {
        /*renderedText_.AppendLines(TEXT_REGION.renderedText_);

        if (allowScrollbarOrig_ && !sfUtil::IsSizeZeroOrLessEither(GetEntityRegion())
            && (renderedText_.region.height > GetEntityRegion().height))
        {
            HandleSliderBar(MakeSliderBar(GetEntityRegion()));
        }

        SetupEntityRegion(GetEntityRegion());
        textInfoOrig_.text += "\n" + TEXT_REGION.GetText();
        EstablishWhichLinesToDraw(((sliderBarUPtr_) ? sliderBarUPtr_->PositionRatio() :
        0.0f));*/
    }

    void TextRegion::OnColorChange()
    {
        sprite_.setColor(entityFgColor_);
        boxEntityUPtr_->SetEntityColors(entityFocusColors_);
    }

    void TextRegion::SetText(const std::string & NEW_TEXT)
    {
        textInfoOrig_.text = NEW_TEXT;

        Setup(
            textInfoOrig_,
            regionOrig_,
            stagePtrOpt_,
            smallFontSizeOrig_,
            boxEntityUPtr_->Info(),
            marginsOrig_,
            allowScrollbarOrig_);
    }

    const SliderBarPtr_t TextRegion::MakeSliderBar(const sf::FloatRect & REGION) const
    {
        return SliderBarPtr_t(new SliderBar(
            GetEntityName() + "'s_",
            sfutil::Right(REGION) - SliderBar::POS_OFFSET_HORIZ_,
            REGION.top + SliderBar::POS_OFFSET_VERT_,
            REGION.height - (SliderBar::POS_OFFSET_VERT_ * 2.0f),
            SliderStyle(Orientation::Vert, Brightness::Bright, true, true)));
    }

    void TextRegion::SetupEntityRegion(const sf::FloatRect & REGION_PARAM)
    {
        // set the entity region to the TEXT_REGION except for width and height, if either is
        // <= 0 then use the actual size of the rendered text
        const sf::FloatRect ENTITY_REGION(
            REGION_PARAM.left,
            REGION_PARAM.top,
            ((misc::IsRealZeroOrLess(REGION_PARAM.width)) ? sfutil::Width(sprite_)
                                                          : REGION_PARAM.width),
            ((misc::IsRealZeroOrLess(REGION_PARAM.height)) ? sfutil::Height(sprite_)
                                                           : REGION_PARAM.height));

        SetEntityRegion(ENTITY_REGION);
    }

    void TextRegion::SetupBox(const BoxEntityInfo & BOX_INFO)
    {
        if (boxEntityUPtr_ && stagePtrOpt_)
        {
            stagePtrOpt_.value()->EntityRemove(boxEntityUPtr_);
        }

        boxEntityUPtr_ = std::make_unique<BoxEntity>("TextRegion's", regionOrig_, BOX_INFO);

        if (stagePtrOpt_)
        {
            stagePtrOpt_.value()->EntityAdd(boxEntityUPtr_, true);
        }
    }

    const SliderBarPtrOpt_t TextRegion::RenderText(
        const TextInfo & TEXT_INFO,
        const sf::FloatRect & REGION,
        const Margins &,
        const unsigned int)
    {
        if (TEXT_INFO.IsValid())
        {
            willDraw_ = TextRenderer::ToSprite(TEXT_INFO, REGION, renderTextureUPtr_, sprite_);
        }

        return boost::none;

        // SliderBarPtrOpt_t sliderBarPtrOpt;
        //
        //// note:  only shrink or making a sliderbar if the there is a vert AND horiz limit
        // auto isRenderedTextTallerThanLimit = [&]() {
        //    return (
        //        (REGION.width > 0.0f) && (REGION.height > 0.0f)
        //        && (Height(sprite_) > REGION.height));
        //};
        //
        // if (isRenderedTextTallerThanLimit())
        //{
        //    TextInfo newTextInfo { TEXT_INFO };
        //
        //    if ((SMALLER_FONT_SIZE > 0) && (TEXT_INFO.size > SMALLER_FONT_SIZE))
        //    {
        //        newTextInfo
        //            = TextInfo(TEXT_INFO, TEXT_INFO.text, TEXT_INFO.color, SMALLER_FONT_SIZE);
        //
        //        TextRendering::Render(newTextInfo, REGION.width, renderTexture_);
        //        sprite_.setTexture(renderTexture_.getTexture(), true);
        //    }
        //
        //    if (isRenderedTextTallerThanLimit())
        //    {
        //        sliderBarPtrOpt = MakeSliderBar(REGION);
        //
        //        const float WIDTH_LIMIT_SHRUNK { (REGION.width - SliderBar::POS_OFFSET_HORIZ_)
        //                                         - MARGINS.right };
        //
        //        TextRendering::Render(newTextInfo, WIDTH_LIMIT_SHRUNK, renderTexture_);
        //        sprite_.setTexture(renderTexture_.getTexture(), true);
        //    }
        //}
        //
        // return sliderBarPtrOpt;
    }

    void TextRegion::EstablishWhichLinesToDraw(const float)
    {
        /*startLine_ = stopLine_ = 0;

        if (renderedText_.regions.empty() || sfutil::IsSizeZeroOrLessEither(GetEntityRegion()))
        {
            return;
        }

        const auto START_LINE_ORIG { startLine_ };
        const auto HEIGHT_LIMIT { GetEntityRegion().height };

        if (HEIGHT_LIMIT < renderedText_.region.height)
        {
            // find the lowest start line that can be drawn
            const std::size_t LOWEST_START_LINE = [&]() {
                float heightSumFromBottom { 0.0f };
                std::size_t lowestStartLine { renderedText_.regions.size() };
                for (; lowestStartLine--;)
                {
                    heightSumFromBottom += renderedText_.regions[lowestStartLine].height;

                    if (heightSumFromBottom > HEIGHT_LIMIT)
                    {
                        return lowestStartLine + 1;
                    }
                }

                return lowestStartLine;
            }();

            if (LOWEST_START_LINE >= renderedText_.regions.size())
            {
                startLine_ = stopLine_ = renderedText_.regions.size() - 1;
            }
            else
            {
                startLine_ = static_cast<std::size_t>(
                    static_cast<float>(LOWEST_START_LINE) * SCROLL_RATIO);

                float heightSumFromTop { 0.0f };
                for (stopLine_ = LOWEST_START_LINE; stopLine_ < renderedText_.regions.size();
                     ++stopLine_)
                {
                    heightSumFromTop += renderedText_.regions[stopLine_].height;

                    if (heightSumFromTop > HEIGHT_LIMIT)
                    {
                        break;
                    }
                }
            }
        }
        else
        {
            startLine_ = 0;
            stopLine_ = renderedText_.regions.size() - 1;
        }

        renderedText_.MovePos(
            0.0f,
            renderedText_.regions[START_LINE_ORIG].top -
        renderedText_.regions[startLine_].top);*/
    }

} // namespace gui
} // namespace heroespath
