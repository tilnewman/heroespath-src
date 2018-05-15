// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// text-render.cpp
//
#include "text-rendering.hpp"

#include "log/log-macros.hpp"
#include "misc/assertlogandthrow.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/font-manager.hpp"
#include "sfml-util/gui/sliderbar.hpp"
#include "sfml-util/sfml-util.hpp"

#include <cctype> //for isDigit()
#include <numeric>

namespace heroespath
{
namespace sfml_util
{
    namespace text_render
    {

        const float Constants::SLIDERBAR_HORIZ_OFFSET_{ 18.0f };

        const gui::SliderBarPtrOpt_t RenderToArea(
            const std::string & NAME,
            const gui::TextInfo & TEXT_INFO,
            RenderedText & renderedText,
            const sf::FloatRect & REGION,
            const unsigned int SMALLER_FONT_SIZE,
            const Margins & MARGINS,
            const bool WILL_ALLOW_SCROLLBAR)
        {
            gui::SliderBarPtrOpt_t sliderbarPtrOpt{ boost::none };

            // render the prompt text for the popup
            text_render::Render(renderedText, TEXT_INFO, REGION.width, MARGINS);

            // calculate the text's vertical position, which is vertically centered,
            // then verify still within bounds
            float vertPosToUse(
                (REGION.top + (REGION.height * 0.5f)) - (renderedText.total_height * 0.5f));

            if (vertPosToUse < REGION.top)
            {
                vertPosToUse = REGION.top;
            }

            // check if the vertical size is too large
            if (((vertPosToUse + renderedText.total_height) > (REGION.top + REGION.height))
                && (REGION.height > 0.0f))
            {
                // re-render with smaller font if original size did not fit vertically
                gui::TextInfo smallerTextInfo(TEXT_INFO);
                if ((0 != SMALLER_FONT_SIZE) && (TEXT_INFO.charSize > SMALLER_FONT_SIZE))
                {
                    smallerTextInfo.charSize = SMALLER_FONT_SIZE;

                    renderedText.Reset();
                    text_render::Render(renderedText, smallerTextInfo, REGION.width, MARGINS);

                    // re-calculate the vertical position after re-sizing the font
                    vertPosToUse
                        = ((REGION.top + (REGION.height * 0.5f))
                           - (renderedText.total_height * 0.5f));

                    if (vertPosToUse < REGION.top)
                    {
                        vertPosToUse = REGION.top;
                    }
                }

                if (((vertPosToUse + renderedText.total_height) > (REGION.top + REGION.height))
                    && (REGION.height > 0.0f) && WILL_ALLOW_SCROLLBAR)
                {
                    // if the text still won't fit vertically, add the sliderbar, re-calculate,
                    // and then configure the sliderbar.
                    sliderbarPtrOpt = new gui::SliderBar(
                        std::string(NAME).append("Vertical"),
                        (REGION.left + REGION.width) - Constants::SLIDERBAR_HORIZ_OFFSET_,
                        REGION.top + 5.0f,
                        REGION.height - 10.0f,
                        gui::SliderStyle(Orientation::Vert, Brightness::Bright, true, true));

                    // re-render the text with the reduced horiz space to make room for the
                    // sliderbar
                    auto const NEW_WIDTH{ ((REGION.width - MARGINS.left) - MARGINS.right)
                                          - (Constants::SLIDERBAR_HORIZ_OFFSET_ * 2.0f) };

                    renderedText.Reset();
                    text_render::Render(renderedText, smallerTextInfo, NEW_WIDTH, MARGINS);
                }
            }

            return sliderbarPtrOpt;
        }

        const gui::SliderBarPtrOpt_t ApplyToArea(
            const std::string & NAME,
            const RenderedText & RENDERED_TEXT,
            const sf::FloatRect & REGION)
        {
            gui::SliderBarPtrOpt_t sliderbarPtrOpt{ boost::none };

            // calculate the text's vertical position, which is vertically centered,
            // then verify still within bounds
            float vertPosToUse(
                (REGION.top + (REGION.height * 0.5f)) - (RENDERED_TEXT.total_height * 0.5f));

            if (vertPosToUse < REGION.top)
            {
                vertPosToUse = REGION.top;
            }

            // check if the vertical size is too large
            if (((vertPosToUse + RENDERED_TEXT.total_height) > (REGION.top + REGION.height))
                && (REGION.height > 0.0f))
            {
                // if the text won't fit vertically, add the sliderbar
                sliderbarPtrOpt = new gui::SliderBar(
                    std::string(NAME).append("Vertical"),
                    (REGION.left + REGION.width) - Constants::SLIDERBAR_HORIZ_OFFSET_,
                    REGION.top + 5.0f,
                    REGION.height - 10.0f,
                    gui::SliderStyle(Orientation::Vert, Brightness::Bright, true, true));
            }

            return sliderbarPtrOpt;
        }

        void RenderAndDraw(RenderedText & renderedText, sf::RenderTexture & renderTexture)
        {
            renderTexture.create(
                static_cast<unsigned>(renderedText.longest_line),
                static_cast<unsigned>(renderedText.total_height));

            renderTexture.clear(sf::Color::Transparent);

            sf::RenderStates states;
            text_render::Draw(renderedText, renderTexture, states);
            renderTexture.display();
        }

        void Render(
            RenderedText & renderText,
            const gui::TextInfo & TEXT_INFO,
            const float WIDTH_LIMIT,
            const Margins & MARGINS)
        {
            sf::FloatRect r;
            r.left = 0.0f;
            r.top = 0.0f;
            r.width = WIDTH_LIMIT;
            r.height = 0.0f;

            return Render(renderText, TEXT_INFO, r, MARGINS);
        }

        void Render(
            RenderedText & renderText,
            const gui::TextInfo & TEXT_INFO,
            const float POS_LEFT,
            const float POS_TOP,
            const Margins & MARGINS)
        {
            sf::FloatRect r;
            r.left = POS_LEFT;
            r.top = POS_TOP;
            r.width = 0.0f;
            r.height = 0.0f;

            return Render(renderText, TEXT_INFO, r, MARGINS);
        }

        void Render(
            RenderedText & renderText,
            const gui::TextInfo & TEXT_INFO,
            const sf::FloatRect & REGION,
            const Margins & MARGINS)
        {
            gui::TextInfo numbersTextInfo(TEXT_INFO);
            numbersTextInfo.fontPtrOpt = sfml_util::FontManager::Instance()->Font_NumbersDefault1();
            return Render(renderText, TEXT_INFO, numbersTextInfo, TEXT_INFO.text, REGION, MARGINS);
        }

        void Render(
            RenderedText & renderText,
            const gui::TextInfo & TEXT_INFO_CHAR,
            const gui::TextInfo & TEXT_INFO_NUM,
            const std::string & TEXT,
            const sf::FloatRect & REGION,
            const Margins & MARGINS)
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (false == TEXT.empty()), "TextRener::Render() was given an empty string.");

            M_ASSERT_OR_LOGANDTHROW_SS(
                (!!TEXT_INFO_CHAR.fontPtrOpt),
                "TextRender::Render(\"" << TEXT << "\") was given a TEXT_INFO_CHAR with a null"
                                        << " font pointer.");

            M_ASSERT_OR_LOGANDTHROW_SS(
                (!!TEXT_INFO_NUM.fontPtrOpt),
                "TextRender::Render(\"" << TEXT << "\") was given a TEXT_INFO_NUM with a null"
                                        << " font pointer.");

            std::size_t strIndex(0);
            float heightTracker(0.0f);
            sf::Vector2f textPos(0.0f, 0.0f);

            // one vector of char TextSnippets per line
            std::vector<TextSnippetVec_t> textSnippetVecVec;

            float longestLine(0.0f);

            // keep track of individual line lengths for justification below
            std::vector<float> lineLengthVec;

            const std::size_t STR_LEN(TEXT.size());
            while (strIndex < STR_LEN)
            {
                TextSnippetVec_t nextTextSnippetVec(RenderLine(
                    TextInfoSet(TEXT, TEXT_INFO_NUM, TEXT_INFO_CHAR),
                    textPos,
                    REGION.width - MARGINS.right,
                    heightTracker,
                    strIndex));

                textSnippetVecVec.emplace_back(nextTextSnippetVec);

                lineLengthVec.emplace_back(textPos.x);

                if (longestLine < textPos.x)
                {
                    longestLine = textPos.x;
                }

                textPos.x = 0;
                textPos.y += heightTracker + 1;
                heightTracker = 0.0f;
            }

            // adjust positions
            const std::size_t NUM_LINES(textSnippetVecVec.size());
            for (std::size_t l(0); l < NUM_LINES; ++l)
            {
                // calculate justification offset
                float offset(0.0f);
                const float CURR_LINE_LEN(lineLengthVec[l]);
                if (TEXT_INFO_CHAR.justified == Justified::Center)
                {
                    if (misc::IsRealClose(0.0f, REGION.width))
                    {
                        offset = ((longestLine * 0.5f) - (CURR_LINE_LEN * 0.5f));
                    }
                    else
                    {
                        offset = ((REGION.width * 0.5f) - (CURR_LINE_LEN * 0.5f));
                    }
                }
                else
                {
                    if (TEXT_INFO_CHAR.justified == Justified::Right)
                    {
                        if (misc::IsRealClose(0.0f, REGION.width))
                        {
                            offset = (longestLine - CURR_LINE_LEN);
                        }
                        else
                        {
                            offset = (REGION.width - CURR_LINE_LEN);
                        }
                    }
                }

                const std::size_t NUM_CHARS(textSnippetVecVec[l].size());
                for (std::size_t c(0); c < NUM_CHARS; ++c)
                {
                    auto & sfText{ textSnippetVecVec[l][c].sf_text };

                    // adjust positions to fall within in the REGION given
                    sfText.setPosition(
                        sfText.getPosition().x + REGION.left,
                        sfText.getPosition().y + REGION.top + MARGINS.top);

                    // adjust position for the justification given
                    sfText.setPosition(sfText.getPosition().x + offset, sfText.getPosition().y);

                    // adjust for left margin
                    sfText.setPosition(
                        sfText.getPosition().x + MARGINS.left, sfText.getPosition().y);

                    const float RIGHT_EDGE(sfText.getPosition().x + sfText.getLocalBounds().width);

                    if (longestLine < RIGHT_EDGE)
                    {
                        longestLine = RIGHT_EDGE;
                    }
                }
            }

            renderText.longest_line = longestLine;

            // zTn Um...where did this 8.0f come from?  2016/10/26
            renderText.total_height = textPos.y + 8.0f + MARGINS.bottom + MARGINS.top;

            renderText.vec_vec = std::move(textSnippetVecVec);
        }

        TextSnippetVec_t RenderLine(
            const TextInfoSet & TEXT_INFO_SET,
            sf::Vector2f & textPos,
            const float WIDTH_LIMIT_ORIG,
            float & heightTracker,
            std::size_t & strIndex)
        {
            TextSnippetVec_t textSnippetVec;

            // if not specified use the screen width for maximum line length
            auto const INVALID_WIDTH{ Display::Instance()->GetWinWidth() };

            auto const WIDTH_LIMIT_TO_USE{ (
                (misc::IsRealClose(0.0f, WIDTH_LIMIT_ORIG)) ? INVALID_WIDTH : WIDTH_LIMIT_ORIG) };

            auto const STR_LEN{ TEXT_INFO_SET.text.size() };

            while ((strIndex < STR_LEN) && (textPos.x <= WIDTH_LIMIT_TO_USE))
            {
                char termChar(0);
                auto const ORIG_STR_INDEX{ strIndex };
                auto const ORIG_POS_X{ textPos.x };

                TextSnippetVec_t nextTextSnippetVec(
                    RenderWord(TEXT_INFO_SET, textPos, heightTracker, strIndex, termChar));

                if (textPos.x <= WIDTH_LIMIT_TO_USE)
                {
                    // group snippets together if their fonts match
                    GroupTextSnippets(textSnippetVec, nextTextSnippetVec);

                    if ('\n' == termChar)
                    {
                        break;
                    }
                }
                else
                {
                    // if a single word won't fit on a single line,
                    // clip characters from the end until it will fit
                    if (textSnippetVec.empty())
                    {
                        textPos.x = WIDTH_LIMIT_TO_USE;

                        for (std::size_t i(0); i < nextTextSnippetVec.size(); ++i)
                        {
                            nextTextSnippetVec.pop_back();

                            auto const LINE_LENGTH{ std::accumulate(
                                std::begin(nextTextSnippetVec),
                                std::end(nextTextSnippetVec),
                                0.0f,
                                [](auto const SUBTOTAL, auto const & TEXT_SNIPPET) {
                                    return SUBTOTAL + TEXT_SNIPPET.sf_text.getLocalBounds().width;
                                }) };

                            if (LINE_LENGTH < WIDTH_LIMIT_TO_USE)
                            {
                                break;
                            }
                        }

                        if (false == nextTextSnippetVec.empty())
                        {
                            nextTextSnippetVec.pop_back();
                        }

                        for (auto const & NEXT_TEXT_SNIPPET : nextTextSnippetVec)
                        {
                            textSnippetVec.emplace_back(NEXT_TEXT_SNIPPET);
                        }

                        break;
                    }
                    else
                    {
                        strIndex = ORIG_STR_INDEX;
                        textPos.x = ORIG_POS_X;
                        break;
                    }
                }
            }

            // adjust for...okay, I don't know why this adjustment is needed.  zTn 2016-11-20
            auto const NUM_SNIPPETS{ textSnippetVec.size() };
            if (NUM_SNIPPETS > 1)
            {
                auto const ADJ{ textSnippetVec[1].sf_text.getPosition().x
                                - textSnippetVec[0].sf_text.getLocalBounds().width };

                for (std::size_t i(1); i < NUM_SNIPPETS; ++i)
                {
                    auto & sfText{ textSnippetVec[i].sf_text };
                    sfText.setPosition(sfText.getPosition().x - ADJ, sfText.getPosition().y);
                }
            }

            return textSnippetVec;
        }

        TextSnippetVec_t RenderWord(
            const TextInfoSet & TEXT_INFO_SET,
            sf::Vector2f & textPos,
            float & heightTracker,
            std::size_t & strIndex,
            char & termChar)
        {
            char currChar(0);
            TextSnippetVec_t textSnippetVec;
            const std::size_t STR_LEN(TEXT_INFO_SET.text.size());

            for (; strIndex < STR_LEN; ++strIndex)
            {
                char prevChar = currChar;
                currChar = TEXT_INFO_SET.text[strIndex];

                TextSnippet nextTextSnippet;

                // replace font of numbers to a special font that makes the numbers look cool
                if (isdigit(static_cast<unsigned char>(currChar)))
                {
                    nextTextSnippet.Setup(
                        textPos.x, textPos.y, std::string(1, currChar), TEXT_INFO_SET.ti_num);
                }
                else
                {
                    nextTextSnippet.Setup(
                        textPos.x, textPos.y, std::string(1, currChar), TEXT_INFO_SET.ti_char);
                }

                textPos.x += nextTextSnippet.sf_text.getLocalBounds().width + 1;

                if (0 != prevChar)
                {
                    textPos.x += nextTextSnippet.sf_text.getFont()->getKerning(
                        static_cast<sf::Uint32>(prevChar),
                        static_cast<sf::Uint32>(currChar),
                        nextTextSnippet.sf_text.getCharacterSize());
                }

                float textHeight(nextTextSnippet.sf_text.getFont()->getLineSpacing(
                    nextTextSnippet.sf_text.getCharacterSize()));

                if (nextTextSnippet.sf_text.getCharacterSize() < 30)
                {
                    textHeight -= static_cast<float>(
                        (static_cast<int>(nextTextSnippet.sf_text.getCharacterSize()) / 5) * 2);
                }

                if (heightTracker < textHeight)
                {
                    heightTracker = textHeight;
                }

                textSnippetVec.emplace_back(nextTextSnippet);

                // break after spaces, dashes, and new-lines
                if ((' ' == currChar) || ('-' == currChar) || ('\n' == currChar))
                {
                    break;
                }
            }

            // keep track of the last character in case of newlines, or other special cases
            termChar = currChar;

            // move past the current character
            ++strIndex;

            // group snippets together if their fonts match
            TextSnippetVec_t finalVec;
            GroupTextSnippets(finalVec, textSnippetVec);

            return finalVec;
        }

        void GroupTextSnippets(TextSnippetVec_t & resultVec, const TextSnippetVec_t & inputVec)
        {
            const std::size_t NUM_SNIPPETS(inputVec.size());

            if (NUM_SNIPPETS > 0)
            {
                std::size_t i(0);

                if (resultVec.empty())
                    resultVec.emplace_back(inputVec[i++]);

                for (; i < NUM_SNIPPETS; ++i)
                {
                    if (resultVec[resultVec.size() - 1].sf_text.getFont()
                        == inputVec[i].sf_text.getFont())
                    {
                        resultVec[resultVec.size() - 1].text_info.text
                            += inputVec[i].text_info.text;

                        resultVec[resultVec.size() - 1].sf_text.setString(
                            resultVec[resultVec.size() - 1].text_info.text);
                    }
                    else
                    {
                        resultVec.emplace_back(inputVec[i]);
                    }
                }
            }
        }
    } // namespace text_render
} // namespace sfml_util
} // namespace heroespath
