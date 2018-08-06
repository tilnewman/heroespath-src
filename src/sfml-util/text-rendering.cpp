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

        std::size_t textPosIndex(0);
        sf::Vector2f posV { POS_V_ORIG };

        const std::size_t TEXT_LENGTH(TEXT_INFO.text.size());
        while (textPosIndex < TEXT_LENGTH)
        {
            const SfTextVec_t SF_TEXT_VEC_LINE { RenderLine(
                TEXT_INFO.text,
                TEXT_INFO.char_size,
                NUMBERS_FONT_PTR_OPT.value(),
                NUMBERS_FONT_PTR,
                posV,
                WIDTH_LIMIT,
                textPosIndex) };

            if (SF_TEXT_VEC_LINE.empty())
            {
                // something went wrong, bail
                break;
            }

            renderedText.text_vecs.emplace_back(SF_TEXT_VEC_LINE);
            renderedText.SyncRegion();
            posV.y = Bottom(renderedText.region) + 1.0f;
        }

        // apply justification
        if ((TEXT_INFO.justified == Justified::Center) || (TEXT_INFO.justified == Justified::Right))
        {
            for (auto & sfTextVecLine : renderedText.text_vecs)
            {
                const auto LINE_LENGTH { MininallyEnclosing(sfTextVecLine).width };

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
                    for (auto & sfText : sfTextVecLine)
                    {
                        sfText.move(horizOffset, 0.0f);
                    }
                }
            }

            renderedText.SyncRegion();
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

        SfTextVec_t sfTextVecLineComplete;
        bool willPrefixSpaceToNextWord { false };

        auto isHorizPosWithinLimit = [&](const float POS_LEFT) {
            return (!(WIDTH_LIMIT > 0.0f) || (POS_LEFT < (POS_V_ORIG.x + WIDTH_LIMIT)));
        };

        sf::Vector2f posV { POS_V_ORIG };

        while (textPosIndex < TEXT_LENGTH)
        {
            const auto TEXT_POS_BEFORE_RENDERING_WORD_INDEX { textPosIndex };

            posV.x = POS_V_ORIG.x + MininallyEnclosing(sfTextVecLineComplete).width + 1.0f;

            char wordTerminatingChar { 0 };
            SfTextVec_t sfTextVecWord { RenderWord(
                TEXT,
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
                if (('\n' == wordTerminatingChar) && (sfTextVecLineComplete.empty()))
                {
                    // found only spaces and a newline, so add an empty vec to mark the empty line
                    sfTextVecLineComplete.emplace_back(
                        sf::Text(" ", *LETTERS_FONT_PTR, CHARACTER_SIZE));

                    return sfTextVecLineComplete;
                }
                else
                {
                    // something went wrong, end the line and bail
                    break;
                }
            }

            const auto SF_TEXT_VEC_WORD_BOUNDS_ORIG { MininallyEnclosing(sfTextVecWord) };

            if ((SF_TEXT_VEC_WORD_BOUNDS_ORIG.width > 0.0f) == false)
            {
                // something went wrong, end the line and bail
                break;
            }

            if (isHorizPosWithinLimit(Right(SF_TEXT_VEC_WORD_BOUNDS_ORIG)))
            {
                std::copy(
                    std::begin(sfTextVecWord),
                    std::end(sfTextVecWord),
                    std::back_inserter(sfTextVecLineComplete));

                if ('\n' == wordTerminatingChar)
                {
                    break;
                }

                willPrefixSpaceToNextWord = (' ' == wordTerminatingChar);
            }
            else
            {
                if (sfTextVecLineComplete.empty())
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
                        std::begin(sfTextVecWord),
                        newEndIter,
                        std::back_inserter(sfTextVecLineComplete));
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

        return sfTextVecLineComplete;
    }

    const SfTextVec_t TextRenderer::RenderWord(
        const std::string & TEXT,
        const unsigned CHARACTER_SIZE,
        const FontPtr_t & LETTERS_FONT_PTR,
        const FontPtr_t & NUMBERS_FONT_PTR,
        const sf::Vector2f & POS_V,
        std::size_t & textPosIndex,
        const bool WILL_PREFIX_SPACE,
        char & terminatingChar)
    {
        SfTextVec_t sfTextVecLine;

        const std::size_t TEXT_LENGTH { TEXT.size() };

        if (textPosIndex >= TEXT_LENGTH)
        {
            terminatingChar = 0;
            return sfTextVecLine;
        }

        // skip leading spaces
        while (TEXT[textPosIndex++] == ' ')
        {
            if (textPosIndex >= TEXT_LENGTH)
            {
                terminatingChar = 0;
                return sfTextVecLine;
            }
        }

        // return empty vec with terminatingChar='\n' if newline was the first non-space found
        if ('\n' == TEXT[textPosIndex])
        {
            terminatingChar = '\n';
            return sfTextVecLine;
        }

        auto willCharUseLetterFont = [&](const char CHAR) {
            return (std::isdigit(static_cast<unsigned char>(CHAR)) == false);
        };

        auto makeNewSfTextToTheRightOf = [&](const char CHAR, const sf::Text & LEFT_TEXT) {
            sf::Text newSfText;

            if (willCharUseLetterFont(CHAR))
            {
                newSfText = sf::Text(std::string(1, CHAR), *LETTERS_FONT_PTR, CHARACTER_SIZE);
            }
            else
            {
                newSfText = sf::Text(std::string(1, CHAR), *NUMBERS_FONT_PTR, CHARACTER_SIZE);
            }

            const auto LEFT_TEXT_BOUNDS { LEFT_TEXT.getGlobalBounds() };
            newSfText.setPosition(Right(LEFT_TEXT_BOUNDS) + 1.0f, LEFT_TEXT_BOUNDS.top);

            return newSfText;
        };

        auto appendCharOrAppendTextAndStartNew
            = [&](bool & isCurrTextLetters, sf::Text & sfText, const char CHAR) {
                  if (willCharUseLetterFont(CHAR) == isCurrTextLetters)
                  {
                      sfText.setString(sfText.getString() + CHAR);
                  }
                  else
                  {
                      isCurrTextLetters = !isCurrTextLetters;
                      sfTextVecLine.emplace_back(sfText);
                      sfText = makeNewSfTextToTheRightOf(CHAR, sfText);
                  }
              };

        sf::Text sfText;
        sfText.setPosition(POS_V);

        if (WILL_PREFIX_SPACE)
        {
            bool tempToIgnore { true };
            appendCharOrAppendTextAndStartNew(tempToIgnore, sfText, ' ');
        }

        char currChar(TEXT[textPosIndex]);

        bool isCurrCharLetter { willCharUseLetterFont(currChar) };

        appendCharOrAppendTextAndStartNew(isCurrCharLetter, sfText, currChar);

        while (++textPosIndex < TEXT_LENGTH)
        {
            currChar = TEXT[textPosIndex];
            terminatingChar = currChar;

            if ((' ' == currChar) || ('\n' == currChar))
            {
                break;
            }

            appendCharOrAppendTextAndStartNew(isCurrCharLetter, sfText, currChar);
        }

        if (sfText.getString().isEmpty() == false)
        {
            sfTextVecLine.emplace_back(sfText);
        }

        return sfTextVecLine;
    }

} // namespace sfml_util
} // namespace heroespath
