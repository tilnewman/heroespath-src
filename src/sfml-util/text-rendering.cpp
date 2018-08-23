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

#include "sfml-util/font-manager.hpp"
#include "sfml-util/gui/sliderbar.hpp"
#include "sfml-util/sfml-util-position.hpp"
#include "sfml-util/sfml-util-size-and-scale.hpp"

#include <cctype> //for std::isdigit()

namespace heroespath
{
namespace sfml_util
{

    const RenderedText TextRenderer::Render(
        const gui::TextInfo & TEXT_INFO,
        const sf::Vector2f & POS_V_ORIG,
        const float WIDTH_LIMIT,
        const FontPtrOpt_t & NUMBERS_FONT_PTR_OPT)
    {
        RenderedText renderedText;

        M_ASSERT_OR_LOGANDTHROW_SS(
            (!!TEXT_INFO.font_ptr_opt),
            "TextRender::Render(\"" << TEXT_INFO.text << "\") was given a TEXT_INFO with a null"
                                    << " font pointer.");

        const FontPtr_t NUMBERS_FONT_PTR { (
            (NUMBERS_FONT_PTR_OPT)
                ? NUMBERS_FONT_PTR_OPT.value()
                : sfml_util::FontManager::Instance()->GetFont(sfml_util::GuiFont::Number)) };

        const auto BLANK_LINE_HEIGHT = [&]() {
            sf::Text sfTextForBlankLine("X", *TEXT_INFO.font_ptr_opt.value(), TEXT_INFO.char_size);
            return sfTextForBlankLine.getGlobalBounds().height;
        }();

        std::size_t textPosIndex(0);
        sf::Vector2f posV { POS_V_ORIG };

        const std::size_t TEXT_LENGTH(TEXT_INFO.text.size());
        while (textPosIndex < TEXT_LENGTH)
        {
            SfTextVec_t sfTextVecForLine { RenderLine(
                TEXT_INFO.text,
                TEXT_INFO.char_size,
                TEXT_INFO.font_ptr_opt.value(),
                NUMBERS_FONT_PTR,
                posV,
                WIDTH_LIMIT,
                textPosIndex) };

            if (sfTextVecForLine.empty())
            {
                // drop the position down by one blank line
                posV.y += BLANK_LINE_HEIGHT;
            }
            else
            {
                // for some reason sfml does not actually draw the sf::Text where you tell it...
                // this vertical shift corrects that...
                auto lineRegion { MinimallyEnclosing(sfTextVecForLine) };
                const auto VERTICAL_SHIFT { posV.y - lineRegion.top };

                for (auto & sfText : sfTextVecForLine)
                {
                    sfText.move(0.0f, VERTICAL_SHIFT);
                }

                lineRegion.top += VERTICAL_SHIFT;

                renderedText.text_vecs.emplace_back(sfTextVecForLine);
                renderedText.regions.emplace_back(lineRegion);
                posV.y = Bottom(lineRegion) + 1.0f;
            }
        }

        renderedText.region = sfml_util::MinimallyEnclosing(renderedText.regions);

        // apply justification
        if ((TEXT_INFO.justified == Justified::Center) || (TEXT_INFO.justified == Justified::Right))
        {
            const auto LINE_VEC_COUNT { renderedText.text_vecs.size() };
            for (std::size_t i(0); i < LINE_VEC_COUNT; ++i)
            {
                auto & lineVec { renderedText.text_vecs[i] };
                auto & lineVecRegion { renderedText.regions[i] };
                const auto LINE_LENGTH { lineVecRegion.width };

                float horizOffset { 0.0f };
                if (TEXT_INFO.justified == Justified::Center)
                {
                    if (WIDTH_LIMIT > 0.0f)
                    {
                        horizOffset = ((WIDTH_LIMIT * 0.5f) - (LINE_LENGTH * 0.5f));
                    }
                    else
                    {
                        horizOffset = ((renderedText.region.width * 0.5f) - (LINE_LENGTH * 0.5f));
                    }
                }
                else if (TEXT_INFO.justified == Justified::Right)
                {
                    if (WIDTH_LIMIT > 0.0f)
                    {
                        horizOffset = (WIDTH_LIMIT - LINE_LENGTH);
                    }
                    else
                    {
                        horizOffset = (renderedText.region.width - LINE_LENGTH);
                    }
                }

                if (horizOffset > 0.0f)
                {
                    for (auto & sfText : lineVec)
                    {
                        sfText.move(horizOffset, 0.0f);
                        lineVecRegion.left += horizOffset;
                    }
                }
            }

            renderedText.region = sfml_util::MinimallyEnclosing(renderedText.regions);
        }

        return renderedText;
    }

    const SfTextVec_t TextRenderer::RenderLine(
        const std::string & TEXT,
        const unsigned CHARACTER_SIZE,
        const FontPtr_t & LETTERS_FONT_PTR,
        const FontPtr_t & NUMBERS_FONT_PTR,
        const sf::Vector2f & POS_V_ORIG,
        const float WIDTH_LIMIT,
        std::size_t & textPosIndex)
    {
        auto const TEXT_LENGTH { TEXT.size() };

        SfTextVec_t sfTextVecLine;
        bool willPrefixSpaceToNextWord { false };

        auto isHorizPosWithinLimit = [&](const float POS_LEFT) {
            return (!(WIDTH_LIMIT > 0.0f) || (POS_LEFT < (POS_V_ORIG.x + WIDTH_LIMIT)));
        };

        sf::Vector2f posV { POS_V_ORIG };

        while (textPosIndex < TEXT_LENGTH)
        {
            const auto TEXT_POS_BEFORE_RENDERING_WORD_INDEX { textPosIndex };
            const auto CURRENT_WIDTH { MinimallyEnclosing(sfTextVecLine).width };

            posV.x = POS_V_ORIG.x + CURRENT_WIDTH;

            char wordTerminatingChar { 0 };
            SfTextVec_t sfTextVecWord { RenderWords(
                TEXT,
                WIDTH_LIMIT - CURRENT_WIDTH,
                CHARACTER_SIZE,
                LETTERS_FONT_PTR,
                NUMBERS_FONT_PTR,
                posV,
                textPosIndex,
                willPrefixSpaceToNextWord,
                wordTerminatingChar) };

            willPrefixSpaceToNextWord = false;

            if (sfTextVecWord.empty())
            {
                // found only spaces and newlines, or something went wrong, end the line and bail
                break;
            }

            const auto SF_TEXT_VEC_WORD_BOUNDS_ORIG { MinimallyEnclosing(sfTextVecWord) };

            if (IsSizeZeroOrLessEither(SF_TEXT_VEC_WORD_BOUNDS_ORIG))
            {
                // something went wrong, end the line and bail
                break;
            }

            if (isHorizPosWithinLimit(Right(SF_TEXT_VEC_WORD_BOUNDS_ORIG)))
            {
                std::copy(
                    std::begin(sfTextVecWord),
                    std::end(sfTextVecWord),
                    std::back_inserter(sfTextVecLine));

                if ('\n' == wordTerminatingChar)
                {
                    break;
                }

                willPrefixSpaceToNextWord = (' ' == wordTerminatingChar);
            }
            else
            {
                if (sfTextVecLine.empty())
                {
                    // at this point we know that nothing as been rendered to the line so far,
                    // and that the first word is too long to fit horizontally, so truncate the
                    // word,  whatever did not fit on this line will NOT be added to the next
                    // line
                    SfTextVec_t::iterator newEndIter { std::begin(sfTextVecWord) };
                    while ((newEndIter != std::end(sfTextVecWord))
                           && isHorizPosWithinLimit(Right(newEndIter->getGlobalBounds())))
                    {
                        ++newEndIter;
                    }

                    std::copy(
                        std::begin(sfTextVecWord), newEndIter, std::back_inserter(sfTextVecLine));
                }
                else
                {
                    // at this point we know there are already words on the current line and the
                    // current word didn't fit, so backup textPosIndex to the start
                    // of the word that didn't fit so it will be the start of the next line
                    textPosIndex = TEXT_POS_BEFORE_RENDERING_WORD_INDEX;
                }

                // the line is full, end the line
                break;
            }
        }

        return sfTextVecLine;
    }

    const SfTextVec_t TextRenderer::RenderWords(
        const std::string & TEXT,
        const float WIDTH_LIMIT,
        const unsigned CHARACTER_SIZE,
        const FontPtr_t & LETTERS_FONT_PTR,
        const FontPtr_t & NUMBERS_FONT_PTR,
        const sf::Vector2f & POS_V,
        std::size_t & textPosIndex,
        const bool WILL_PREFIX_SPACE,
        char & terminatingChar)
    {
        SfTextVec_t sfTextVecWord;

        const std::size_t TEXT_LENGTH { TEXT.size() };

        if (textPosIndex >= TEXT_LENGTH)
        {
            terminatingChar = 0;
            return sfTextVecWord;
        }

        // skip leading spaces
        while (TEXT[textPosIndex] == ' ')
        {
            if (++textPosIndex >= TEXT_LENGTH)
            {
                terminatingChar = 0;
                return sfTextVecWord;
            }
        }

        // return empty vec with terminatingChar='\n' if newline was the first non-space found
        if ('\n' == TEXT[textPosIndex])
        {
            ++textPosIndex;
            terminatingChar = '\n';
            return sfTextVecWord;
        }

        // this number found by experiment to be a good estimate for the game
        sfTextVecWord.reserve(20);

        auto willCharUseLetterFont = [&](const char CHAR) {
            return (std::isdigit(static_cast<unsigned char>(CHAR)) == false);
        };

        auto makeNewSfTextToTheRightOf = [&](const char CHAR, const sf::Text & LEFT_TEXT) {
            sf::Text newSfText(
                std::string(1, CHAR),
                ((willCharUseLetterFont(CHAR)) ? *LETTERS_FONT_PTR : *NUMBERS_FONT_PTR),
                CHARACTER_SIZE);

            newSfText.setPosition(Right(LEFT_TEXT.getGlobalBounds()) + 1.0f, POS_V.y);
            return newSfText;
        };

        auto appendCharOrAppendTextAndStartNew
            = [&](bool & isCurrTextLetters, sf::Text & sfText, const char CHAR) {
                  if ((CHAR == ' ') || (willCharUseLetterFont(CHAR) == isCurrTextLetters))
                  {
                      sfText.setString(sfText.getString() + CHAR);
                  }
                  else
                  {
                      isCurrTextLetters = !isCurrTextLetters;
                      sfTextVecWord.emplace_back(sfText);
                      sfText = makeNewSfTextToTheRightOf(CHAR, sfText);
                  }
              };

        char currChar(TEXT[textPosIndex]);

        bool isCurrCharLetter { willCharUseLetterFont(currChar) };

        sf::Text sfText(
            "", ((isCurrCharLetter) ? *LETTERS_FONT_PTR : *NUMBERS_FONT_PTR), CHARACTER_SIZE);

        sfText.setPosition(POS_V);

        if (WILL_PREFIX_SPACE)
        {
            bool tempToIgnore { true };
            appendCharOrAppendTextAndStartNew(tempToIgnore, sfText, ' ');
        }

        appendCharOrAppendTextAndStartNew(isCurrCharLetter, sfText, currChar);

        std::size_t textPosLastSpace { 0 };
        SfTextVec_t sfTextVecWordLastSpace;
        sf::Text sfTextLastSpace;

        while (++textPosIndex < TEXT_LENGTH)
        {
            currChar = TEXT[textPosIndex];
            terminatingChar = currChar;

            if ((' ' == currChar) && (WIDTH_LIMIT > 0.0f))
            {
                if ((MinimallyEnclosing(sfTextVecWord).width + sfText.getGlobalBounds().width)
                    < WIDTH_LIMIT)
                {
                    textPosLastSpace = textPosIndex;
                    sfTextVecWordLastSpace = sfTextVecWord;
                    sfTextLastSpace = sfText;
                }
                else
                {
                    if (textPosLastSpace != 0)
                    {
                        textPosIndex = textPosLastSpace;
                        sfTextVecWord = sfTextVecWordLastSpace;
                        sfText = sfTextLastSpace;
                    }

                    terminatingChar = ' ';
                    break;
                }
            }
            else if ('\n' == currChar)
            {
                ++textPosIndex;
                break;
            }

            appendCharOrAppendTextAndStartNew(isCurrCharLetter, sfText, currChar);
        }

        if (sfText.getString().isEmpty() == false)
        {
            sfTextVecWord.emplace_back(sfText);
        }

        return sfTextVecWord;
    }

} // namespace sfml_util
} // namespace heroespath
