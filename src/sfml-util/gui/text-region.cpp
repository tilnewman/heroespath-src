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
#include "sfml-util/gui/box-info.hpp"
#include "sfml-util/gui/box.hpp"
#include "sfml-util/i-stage.hpp"
#include "sfml-util/sfml-util.hpp"

#include <algorithm>

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        const unsigned int TextRegion::DEFAULT_NO_RESIZE_{ 0 };

        TextRegion::TextRegion(const std::string & NAME)
            : GuiEntity(std::string(NAME).append("_TextRegion"), 0.0f, 0.0f)
            , boxInfo_()
            , boxUPtr_()
            , sliderBarUPtr_()
            , stagePtrOpt_(boost::none)
            , text_("")
            , renderedText_()
            , startLine_(0)
            , regionOrig_()
            , textInfoOrig_()
            , smallFontSizeOrig_()
            , marginsOrig_()
            , allowScrollbarOrig_()
            , textSnipsToDrawVec_()
        {}

        TextRegion::TextRegion(
            const std::string & NAME,
            const TextInfo & TEXT_INFO,
            const sf::FloatRect & REGION,
            const unsigned int SMALLER_FONT_SIZE,
            const box::Info & BOX_INFO,
            const Margins & MARGINS)
            : GuiEntity(std::string(NAME).append("_TextRegion"), REGION)
            , boxInfo_()
            , boxUPtr_()
            , sliderBarUPtr_()
            , stagePtrOpt_(boost::none)
            , text_("")
            , renderedText_()
            , startLine_(0)
            , regionOrig_()
            , textInfoOrig_()
            , smallFontSizeOrig_()
            , marginsOrig_()
            , allowScrollbarOrig_()
            , textSnipsToDrawVec_()
        {
            Setup(TEXT_INFO, REGION, SMALLER_FONT_SIZE, BOX_INFO, MARGINS);
        }

        TextRegion::TextRegion(
            const std::string & NAME,
            const TextInfo & TEXT_INFO,
            const sf::FloatRect & REGION,
            const IStagePtr_t ISTAGE_PTR,
            const unsigned int SMALLER_FONT_SIZE,
            const box::Info & BOX_INFO,
            const Margins & MARGINS)
            : GuiEntity(std::string(NAME).append("_TextRegion"), REGION)
            , boxInfo_()
            , boxUPtr_()
            , sliderBarUPtr_()
            , stagePtrOpt_(ISTAGE_PTR)
            , text_("")
            , renderedText_()
            , startLine_(0)
            , regionOrig_()
            , textInfoOrig_()
            , smallFontSizeOrig_()
            , marginsOrig_()
            , allowScrollbarOrig_()
            , textSnipsToDrawVec_()
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

                if (boxUPtr_)
                {
                    stagePtrOpt_.value()->EntityRemove(boxUPtr_.get());
                }
            }
        }

        void TextRegion::Setup(
            const TextInfo & TEXT_INFO,
            const sf::FloatRect & REGION,
            const unsigned int SMALLER_FONT_SIZE,
            const box::Info & BOX_INFO,
            const Margins & MARGINS)
        {
            Setup(TEXT_INFO, REGION, stagePtrOpt_, SMALLER_FONT_SIZE, BOX_INFO, MARGINS, false);
        }

        void TextRegion::Setup(
            const TextInfo & TEXT_INFO,
            const sf::FloatRect & REGION,
            const IStagePtrOpt_t ISTAGE_PTR_OPT,
            const unsigned int SMALLER_FONT_SIZE,
            const box::Info & BOX_INFO,
            const Margins & MARGINS,
            const bool WILL_ALLOW_SCROLLBAR)
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (false == TEXT_INFO.text.empty()),
                GetEntityName() << " TextRegion::Setup() was given a TEXT_INFO.up.text string"
                                << " that was empty.");

            M_ASSERT_OR_LOGANDTHROW_SS(
                (!!TEXT_INFO.fontPtrOpt),
                GetEntityName() << " TextRegion::Setup(\"...\") was given an upTextInfo with a"
                                << " null font pointer.");

            regionOrig_ = REGION;
            textInfoOrig_ = TEXT_INFO;
            smallFontSizeOrig_ = SMALLER_FONT_SIZE;
            marginsOrig_ = MARGINS;
            allowScrollbarOrig_ = WILL_ALLOW_SCROLLBAR;

            // prevent color changes when clicking on text or on the scrollbar
            SetWillAcceptFocus(false);

            text_ = TEXT_INFO.text;
            stagePtrOpt_ = ISTAGE_PTR_OPT;

            HandleSliderBar(text_render::RenderToArea(
                GetEntityName(),
                TEXT_INFO,
                renderedText_,
                REGION,
                SMALLER_FONT_SIZE,
                MARGINS,
                WILL_ALLOW_SCROLLBAR));

            // position the text for on-screen coordinates
            auto const NUM_LINES{ renderedText_.vec_vec.size() };
            for (std::size_t l(0); l < NUM_LINES; ++l)
            {
                auto const NUM_SNIPPETS{ renderedText_.vec_vec[l].size() };
                for (std::size_t s(0); s < NUM_SNIPPETS; ++s)
                {
                    renderedText_.vec_vec[l][s].sf_text.move(REGION.left, REGION.top);
                }
            }

            // establish the region
            auto heightToUse{ renderedText_.total_height };
            if (REGION.height > 1.0f)
            {
                heightToUse = REGION.height;
            }

            const sf::FloatRect OUTLINE_RECT(
                REGION.left,
                REGION.top,
                std::max(REGION.width, renderedText_.longest_line),
                heightToUse);

            SetEntityRegion(OUTLINE_RECT);
            HandleBox(BOX_INFO);
            EstablishWhichLinesToDraw(0.0f);
            ResetDrawCache();
        }

        void TextRegion::HandleSliderBar(const sfml_util::gui::SliderBarPtrOpt_t SLIDERBAR_PTR_OPT)
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
                    sliderBarUPtr_->SetOnChangeHandler(this);
                    sliderBarUPtr_->SetCurrentValue(0.0f);
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

        void TextRegion::HandleBox(const box::Info & BOX_INFO)
        {
            // deal with the Box that may need to be created or destroyed
            if (stagePtrOpt_ && (boxInfo_ != BOX_INFO) && (renderedText_.vec_vec.empty() == false)
                && (renderedText_.vec_vec[0].empty() == false))
            {
                boxInfo_ = BOX_INFO;

                sf::FloatRect newRegion(entityRegion_);

                auto const FIRST_LINE_HEIGHT{ static_cast<float>(
                    renderedText_.vec_vec[0][0].sf_text.getCharacterSize()) };

                newRegion.top = entityRegion_.top - FIRST_LINE_HEIGHT;
                newRegion.height = entityRegion_.height + FIRST_LINE_HEIGHT;

                if (sliderBarUPtr_)
                {
                    sliderBarUPtr_->MoveEntityPos(25.0f, 0.0f);
                }

                boxInfo_.SetBoxAndBackgroundRegion(newRegion);

                if (boxUPtr_)
                {
                    stagePtrOpt_.value()->EntityRemove(boxUPtr_.get());
                }

                boxUPtr_ = std::make_unique<box::Box>("TextRegion's", boxInfo_);

                // Q: Why add the box to Stage's entitySSet?  Why not keep it as a class member
                //   and draw it ourselves?  ztn 2016-10-8
                // A: Because it is easier to let the Stage and its entityPSet_ handle all the
                //   mouse events that keep the focus and other features working.
                // Q: So what to do when the stage draws the box AFTER and on top of the text,
                //   such as the box's gradient covering the text?
                // A: Have whatever stage owns this TextRegion draw this TextRegion last.
                stagePtrOpt_.value()->EntityAdd(boxUPtr_.get());
            }
        }

        void TextRegion::draw(sf::RenderTarget & target, sf::RenderStates states) const
        {
            // Note:  Don't draw the Box or SliderBar here.  They are being drawn by the stage.

            if (false == entityWillDraw_)
            {
                return;
            }

            for (auto const & TEXT_SNIP_NUMS : textSnipsToDrawVec_)
            {
                if (TEXT_SNIP_NUMS.line_num < renderedText_.vec_vec.size())
                {
                    auto const & SEG_VEC{ renderedText_.vec_vec[TEXT_SNIP_NUMS.line_num] };

                    if (TEXT_SNIP_NUMS.seg_num < SEG_VEC.size())
                    {
                        target.draw(SEG_VEC[TEXT_SNIP_NUMS.seg_num].sf_text, states);
                    }
                }
            }
        }

        void TextRegion::SetEntityPos(const float POS_LEFT, const float POS_TOP)
        {
            MoveEntityPos(POS_LEFT - entityRegion_.left, POS_TOP - entityRegion_.top);
        }

        void TextRegion::MoveEntityPos(const float HORIZ, const float VERT)
        {
            GuiEntity::MoveEntityPos(HORIZ, VERT);

            auto const NUM_LINES{ renderedText_.vec_vec.size() };
            for (std::size_t l(0); l < NUM_LINES; ++l)
            {
                auto const NUM_SNIPPETS{ renderedText_.vec_vec[l].size() };
                for (std::size_t s(0); s < NUM_SNIPPETS; ++s)
                {
                    renderedText_.vec_vec[l][s].sf_text.move(HORIZ, VERT);
                }
            }

            if (boxUPtr_)
            {
                boxUPtr_->MoveEntityPos(HORIZ, VERT);
            }

            if (sliderBarUPtr_)
            {
                sliderBarUPtr_->MoveEntityPos(HORIZ, VERT);
            }

            ResetDrawCache();
        }

        bool TextRegion::HandleCallback(const callback::SliderBarCallbackPackage_t & PACKAGE)
        {
            if (sliderBarUPtr_)
            {
                EstablishWhichLinesToDraw(PACKAGE.PTR_->GetCurrentValue());
                ResetDrawCache();
                return true;
            }
            else
            {
                return false;
            }
        }

        void TextRegion::Append(const TextRegion & TEXT_REGION)
        {
            // add the new lines of text
            auto const NUM_LINES{ TEXT_REGION.renderedText_.vec_vec.size() };
            for (std::size_t l(0); l < NUM_LINES; ++l)
            {
                // adjust vertical positions
                sfml_util::text_render::TextSnippetVec_t nextSnippetVec(
                    TEXT_REGION.renderedText_.vec_vec[l]);

                auto const NUM_SNIPPETS{ nextSnippetVec.size() };
                for (std::size_t s(0); s < NUM_SNIPPETS; ++s)
                {
                    nextSnippetVec[s].sf_text.move(0.0f, renderedText_.total_height);
                }

                renderedText_.vec_vec.emplace_back(nextSnippetVec);
            }

            // adjust total height
            renderedText_.total_height += TEXT_REGION.renderedText_.total_height;

            // adjust longest line if needed
            if (renderedText_.longest_line < TEXT_REGION.renderedText_.longest_line)
            {
                renderedText_.longest_line = TEXT_REGION.renderedText_.longest_line;
            }

            HandleSliderBar(
                text_render::ApplyToArea(GetEntityName(), renderedText_, GetEntityRegion()));

            // establish the new region
            const sf::FloatRect OUTLINE_RECT(
                entityRegion_.left,
                entityRegion_.top,
                renderedText_.longest_line,
                entityRegion_.height);

            SetEntityRegion(OUTLINE_RECT);

            // keep the text_ member accurate
            text_ += TEXT_REGION.text_;

            ResetDrawCache();
        }

        void TextRegion::OnColorChange()
        {
            auto const NUM_LINES{ renderedText_.vec_vec.size() };
            for (std::size_t l(0); l < NUM_LINES; ++l)
            {
                auto const NUM_SNIPPETS{ renderedText_.vec_vec[l].size() };
                for (std::size_t s(0); s < NUM_SNIPPETS; ++s)
                {
                    sfml_util::SetTextColor(renderedText_.vec_vec[l][s].sf_text, entityFgColor_);
                }
            }

            if (boxUPtr_)
            {
                boxUPtr_->SetEntityColors(entityColorSet_);
            }
        }

        void TextRegion::SetText(const std::string & NEW_TEXT)
        {
            textInfoOrig_.text = NEW_TEXT;

            Setup(
                textInfoOrig_,
                regionOrig_,
                stagePtrOpt_,
                smallFontSizeOrig_,
                boxInfo_,
                marginsOrig_,
                allowScrollbarOrig_);
        }

        void TextRegion::ResetDrawCache()
        {
            textSnipsToDrawVec_.clear();

            // this number found by experiment to be a good upper bound
            textSnipsToDrawVec_.reserve(100);

            // don't draw farther down than the region extends, keep track with posY
            auto posY{ 0.0f };
            auto const NUM_LINES{ renderedText_.vec_vec.size() };
            for (std::size_t l(startLine_); l < NUM_LINES; ++l)
            {
                auto const & SNIP_VEC{ renderedText_.vec_vec[l] };

                if (SNIP_VEC.empty())
                {
                    continue;
                }

                // remove extra space created by newline characters
                sf::Text t(SNIP_VEC[0].sf_text);

                t.setString(
                    boost::algorithm::replace_all_copy(std::string(t.getString()), "\n", ""));

                // there should be no empty lines, instead choose a value that fits
                if (std::string(t.getString()).empty())
                {
                    t.setString("\n");

                    // this magic number found by experiment
                    posY += t.getGlobalBounds().height * 0.8f;
                }
                else
                {
                    posY += t.getGlobalBounds().height;
                }

                auto const SCROLL_PAD{ (!sliderBarUPtr_)
                                           ? 0.0f
                                           : static_cast<float>(t.getCharacterSize()) };

                if (posY > (entityRegion_.height - SCROLL_PAD))
                {
                    break;
                }

                auto const NUM_SNIPPETS{ SNIP_VEC.size() };
                for (std::size_t s(0); s < NUM_SNIPPETS; ++s)
                {
                    textSnipsToDrawVec_.emplace_back(TextSnipNum(l, s));
                }
            }
        }

        void TextRegion::EstablishWhichLinesToDraw(const float SCROLL_RATIO)
        {
            EstablishWhichLinesToDraw(SCROLL_RATIO, entityRegion_.height);
        }

        void TextRegion::EstablishWhichLinesToDraw(
            const float SCROLL_RATIO, const float REGION_HEIGHT)
        {
            if ((renderedText_.vec_vec.empty()) || (renderedText_.vec_vec[0].empty()))
            {
                return;
            }

            // establish how far down the user has scrolled
            auto const SCROLL_POS{ SCROLL_RATIO
                                   * (renderedText_.total_height - (REGION_HEIGHT * 0.8f)) };

            auto const FIRST_LINE_HEIGHT{ static_cast<float>(
                renderedText_.vec_vec[0][0].sf_text.getCharacterSize()) };

            // establish which line to start drawing
            auto startPosY{ SCROLL_POS / FIRST_LINE_HEIGHT };
            if (startPosY < 0.0f)
            {
                startPosY = 0.0f;
            }

            startLine_ = static_cast<std::size_t>(startPosY);

            // move the text to compensate
            auto const NUM_LINES{ renderedText_.vec_vec.size() };
            for (std::size_t l(0); l < NUM_LINES; ++l)
            {
                auto const NUM_SNIPPETS{ renderedText_.vec_vec[l].size() };
                for (std::size_t s(0); s < NUM_SNIPPETS; ++s)
                {
                    renderedText_.vec_vec[l][s].sf_text.setPosition(
                        renderedText_.vec_vec[l][s].sf_text.getPosition().x,
                        (entityRegion_.top + (static_cast<float>(l) * FIRST_LINE_HEIGHT))
                            - SCROLL_POS);
                }
            }
        }

    } // namespace gui
} // namespace sfml_util
} // namespace heroespath
