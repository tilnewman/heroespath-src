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

#include "log/log-macros.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/boost-string-includes.hpp"
#include "sfml-util/gui/box-entity-info.hpp"
#include "sfml-util/gui/box-entity.hpp"
#include "sfml-util/i-stage.hpp"
#include "sfml-util/sfml-util-position.hpp"
#include "sfml-util/sfml-util-size-and-scale.hpp"

#include <algorithm>

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        const unsigned int TextRegion::DEFAULT_NO_RESIZE_ { 0 };

        TextRegion::TextRegion(const std::string & NAME)
            : Entity(std::string(NAME).append("_TextRegion"), 0.0f, 0.0f)
            , boxEntityUPtr_()
            , sliderBarUPtr_()
            , stagePtrOpt_()
            , renderedText_()
            , startLine_(0)
            , stopLine_(0)
            , regionOrig_()
            , textInfoOrig_()
            , smallFontSizeOrig_()
            , marginsOrig_()
            , allowScrollbarOrig_()
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
            , renderedText_()
            , startLine_(0)
            , stopLine_(0)
            , regionOrig_()
            , textInfoOrig_()
            , smallFontSizeOrig_()
            , marginsOrig_()
            , allowScrollbarOrig_()
        {
            Setup(TEXT_INFO, REGION, SMALLER_FONT_SIZE, BOX_INFO, MARGINS);
        }

        TextRegion::TextRegion(
            const std::string & NAME,
            const TextInfo & TEXT_INFO,
            const sf::FloatRect & REGION,
            const IStagePtr_t ISTAGE_PTR,
            const unsigned int SMALLER_FONT_SIZE,
            const BoxEntityInfo & BOX_INFO,
            const Margins & MARGINS)
            : Entity(std::string(NAME).append("_TextRegion"), REGION)
            , boxEntityUPtr_()
            , sliderBarUPtr_()
            , stagePtrOpt_(ISTAGE_PTR)
            , renderedText_()
            , startLine_(0)
            , stopLine_(0)
            , regionOrig_()
            , textInfoOrig_()
            , smallFontSizeOrig_()
            , marginsOrig_()
            , allowScrollbarOrig_()
        {
            Setup(TEXT_INFO, REGION, stagePtrOpt_, SMALLER_FONT_SIZE, BOX_INFO, MARGINS, true);
        }

        TextRegion::~TextRegion()
        {
            if (stagePtrOpt_)
            {
                if (sliderBarUPtr_)
                {
                    stagePtrOpt_.value()->EntityRemove(sliderBarUPtr_.get());
                }

                stagePtrOpt_.value()->EntityRemove(boxEntityUPtr_.get());
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
            const IStagePtrOpt_t & ISTAGE_PTR_OPT,
            const unsigned int SMALLER_FONT_SIZE,
            const BoxEntityInfo & BOX_INFO,
            const Margins & MARGINS,
            const bool WILL_ALLOW_SCROLLBAR)
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (false == TEXT_INFO.text.empty()),
                GetEntityName() << " TextRegion::Setup() was given a TEXT_INFO.up.text string"
                                << " that was empty.");

            M_ASSERT_OR_LOGANDTHROW_SS(
                (!!TEXT_INFO.font_ptr_opt),
                GetEntityName() << " TextRegion::Setup(\"...\") was given an upTextInfo with a"
                                << " null font pointer.");

            // set member states
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

            const auto TEXT_REGION { MARGINS.ApplyShrink(REGION_ORIG) };
            HandleSliderBar(RenderText(TEXT_INFO, TEXT_REGION, MARGINS, SMALLER_FONT_SIZE));

            SetupEntityRegion(TEXT_REGION);
            SetupBox(BOX_INFO);
            EstablishWhichLinesToDraw(0.0f);
        }

        void TextRegion::HandleSliderBar(const SliderBarPtrOpt_t SLIDERBAR_PTR_OPT)
        {
            if (stagePtrOpt_)
            {
                if (sliderBarUPtr_)
                {
                    stagePtrOpt_.value()->EntityRemove(sliderBarUPtr_.get());
                    sliderBarUPtr_.reset();
                }

                if (SLIDERBAR_PTR_OPT)
                {
                    sliderBarUPtr_.reset(SLIDERBAR_PTR_OPT->Ptr());

                    sliderBarUPtr_->SetCallbackHandler(
                        SliderBar::Callback_t::IHandlerPtrOpt_t(this));

                    sliderBarUPtr_->PositionRatio(0.0f);
                    stagePtrOpt_.value()->EntityAdd(sliderBarUPtr_.get());
                }
            }
            else
            {
                if (SLIDERBAR_PTR_OPT)
                {
                    delete SLIDERBAR_PTR_OPT->Ptr();
                }
            }
        }

        void TextRegion::draw(sf::RenderTarget & target, sf::RenderStates states) const
        {
            // Note:  Don't draw the Box or SliderBar here.  They are being drawn by the stage.

            if ((false == entityWillDraw_) || renderedText_.text_vecs.empty())
            {
                return;
            }

            for (std::size_t i(startLine_); i <= stopLine_; ++i)
            {
                const auto & SF_TEXT_VEC_LINE { renderedText_.text_vecs[i] };

                for (const auto & SF_TEXT : SF_TEXT_VEC_LINE)
                {
                    target.draw(SF_TEXT, states);
                }
            }
        }

        void TextRegion::SetEntityPos(const float POS_LEFT, const float POS_TOP)
        {
            MoveEntityPos(POS_LEFT - entityRegion_.left, POS_TOP - entityRegion_.top);
        }

        void TextRegion::MoveEntityPos(const float HORIZ, const float VERT)
        {
            Entity::MoveEntityPos(HORIZ, VERT);

            renderedText_.MovePos(HORIZ, VERT);

            boxEntityUPtr_->MoveEntityPos(HORIZ, VERT);

            if (sliderBarUPtr_)
            {
                sliderBarUPtr_->MoveEntityPos(HORIZ, VERT);
            }
        }

        bool TextRegion::HandleCallback(const SliderBar::Callback_t::PacketPtr_t & PACKET_PTR)
        {
            if (sliderBarUPtr_)
            {
                EstablishWhichLinesToDraw(PACKET_PTR->PositionRatio());
                return true;
            }
            else
            {
                return false;
            }
        }

        void TextRegion::Append(const TextRegion & TEXT_REGION)
        {
            renderedText_.AppendLines(TEXT_REGION.renderedText_);

            if (allowScrollbarOrig_ && (GetEntityRegion().height > 0.0f)
                && (renderedText_.region.height > GetEntityRegion().height))
            {
                HandleSliderBar(MakeSliderBar(GetEntityRegion()));
            }

            SetupEntityRegion(GetEntityRegion());
            textInfoOrig_.text += "\n" + TEXT_REGION.GetText();
            EstablishWhichLinesToDraw(((sliderBarUPtr_) ? sliderBarUPtr_->PositionRatio() : 0.0f));
        }

        void TextRegion::OnColorChange()
        {
            renderedText_.SetColor(entityFgColor_);
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
            return new SliderBar(
                GetEntityName() + "'s_",
                Right(REGION) - SliderBar::POS_OFFSET_HORIZ_,
                REGION.top + SliderBar::POS_OFFSET_VERT_,
                REGION.height - (SliderBar::POS_OFFSET_VERT_ * 2.0f),
                SliderStyle(Orientation::Vert, Brightness::Bright, true, true));
        }

        void TextRegion::SetupEntityRegion(const sf::FloatRect & REGION_ORIG)
        {
            // set the entity region to the TEXT_REGION except for height, if TEXT_REGION.height <=
            // 0 then use the actual height of the rendered text
            const sf::FloatRect ENTITY_REGION(
                REGION_ORIG.left,
                REGION_ORIG.top,
                REGION_ORIG.width,
                ((REGION_ORIG.height > 1.0f) ? REGION_ORIG.height : renderedText_.region.height));

            SetEntityRegion(ENTITY_REGION);
        }

        void TextRegion::SetupBox(const BoxEntityInfo & BOX_INFO)
        {
            if (boxEntityUPtr_ && stagePtrOpt_)
            {
                stagePtrOpt_.value()->EntityRemove(boxEntityUPtr_.get());
            }

            boxEntityUPtr_ = std::make_unique<BoxEntity>("TextRegion's", regionOrig_, BOX_INFO);

            if (stagePtrOpt_)
            {
                stagePtrOpt_.value()->EntityAdd(boxEntityUPtr_.get(), true);
            }
        }

        const SliderBarPtrOpt_t TextRegion::RenderText(
            const TextInfo & TEXT_INFO,
            const sf::FloatRect & REGION,
            const Margins & MARGINS,
            const unsigned int SMALLER_FONT_SIZE)
        {
            SliderBarPtrOpt_t sliderBarPtrOpt;

            renderedText_ = TextRenderer::Render(TEXT_INFO, Position(REGION), REGION.width);

            // note:  only shrink or making a sliderbar if the there is a vert AND horiz limit
            auto isRenderedTextTallerThanLimit = [&]() {
                return (
                    (REGION.width > 0.0f) && (REGION.height > 0.0f)
                    && (renderedText_.region.height > REGION.height));
            };

            if (isRenderedTextTallerThanLimit())
            {
                TextInfo newTextInfo { TEXT_INFO };

                if ((SMALLER_FONT_SIZE > 0) && (TEXT_INFO.char_size > SMALLER_FONT_SIZE))
                {
                    newTextInfo
                        = TextInfo(TEXT_INFO, TEXT_INFO.text, TEXT_INFO.color, SMALLER_FONT_SIZE);

                    renderedText_
                        = TextRenderer::Render(newTextInfo, Position(REGION), REGION.width);
                }

                if (isRenderedTextTallerThanLimit())
                {
                    sliderBarPtrOpt = MakeSliderBar(REGION);

                    const float WIDTH_LIMIT_SHRUNK { (REGION.width - SliderBar::POS_OFFSET_HORIZ_)
                                                     - MARGINS.right };

                    renderedText_
                        = TextRenderer::Render(newTextInfo, Position(REGION), WIDTH_LIMIT_SHRUNK);
                }
            }

            return sliderBarPtrOpt;
        }

        void TextRegion::EstablishWhichLinesToDraw(const float SCROLL_RATIO)
        {
            startLine_ = 0;
            stopLine_ = 0;

            if (renderedText_.text_vecs.empty())
            {
                return;
            }

            // um...where did this magic number come from?
            auto const SCROLL_POS {
                SCROLL_RATIO * (renderedText_.region.height - (GetEntityRegion().height * 0.8f))
            };

            // find the line to start drawing
            float startLineOrigPosTop { 0.0f };
            while (startLine_ < renderedText_.text_vecs.size())
            {
                const auto & SF_TEXT_VEC_LINE { renderedText_.text_vecs[startLine_] };
                if (SF_TEXT_VEC_LINE.empty() == false)
                {
                    startLineOrigPosTop = SF_TEXT_VEC_LINE[0].getGlobalBounds().top;
                    if ((startLineOrigPosTop - renderedText_.region.top) > SCROLL_POS)
                    {
                        break;
                    }
                }

                ++startLine_;
            }

            if (startLine_ > 0)
            {
                --startLine_;
            }

            // find the line to stop drawing
            stopLine_ = startLine_;

            while (stopLine_ < renderedText_.text_vecs.size())
            {
                const auto & SF_TEXT_VEC_LINE { renderedText_.text_vecs[stopLine_] };
                if (SF_TEXT_VEC_LINE.empty() == false)
                {
                    const auto CURRENT_LINE_BOUNDS { MininallyEnclosing(SF_TEXT_VEC_LINE) };

                    if (Bottom(CURRENT_LINE_BOUNDS) > Bottom(GetEntityRegion()))
                    {
                        break;
                    }
                }

                ++stopLine_;
            }

            if (stopLine_ > startLine_)
            {
                --stopLine_;
            }

            // move all the text up to compensate for where the siderbar is
            const auto AMOUNT_TO_MOVE_TEXT_UP { startLineOrigPosTop - renderedText_.region.top };
            renderedText_.MovePos(0.0f, -1.0f * AMOUNT_TO_MOVE_TEXT_UP);
        }

    } // namespace gui
} // namespace sfml_util
} // namespace heroespath
