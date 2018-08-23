// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_TEXTRENDERING_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_TEXTRENDERING_HPP_INCLUDED
//
// text-rendering.hpp
//
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"
#include "sfml-util/gui/text-info.hpp"
#include "sfml-util/margins.hpp"
#include "sfml-util/rendered-text.hpp"

#include <string>
#include <vector>

namespace sf
{
class Font;
}

namespace heroespath
{

using FontPtr_t = misc::NotNull<sf::Font *>;
using FontPtrOpt_t = boost::optional<FontPtr_t>;

namespace sfml_util
{

    namespace gui
    {
        class SliderBar;
        using SliderBarPtr_t = misc::NotNull<SliderBar *>;
        using SliderBarPtrOpt_t = boost::optional<SliderBarPtr_t>;
    } // namespace gui

    // Responsible for wrapping the text rendering functions.
    struct TextRenderer
    {
        // use to render text to the given POS_V, this does NOT draw to screen but instead returns a
        // wrapped collection of sf::Text objects called RenderedText, if WIDTH_LIMIT <= 0 then it
        // is ignored and there is no horizontal limit, if no numbers font is provided then the
        // default will be used
        static const RenderedText Render(
            const gui::TextInfo & TEXT_INFO,
            const sf::Vector2f & POS_V,
            const float WIDTH_LIMIT,
            const FontPtrOpt_t & NUMBERS_FONT_PTR_OPT = boost::none);

    private:
        // REGION.height is ignored, if the rendered text is taller than REGION.height then the
        // caller will managed that situation with a sliderbar, if REGION.width <= 0 then it is
        // ignored (there is no length limit)
        static const SfTextVec_t RenderLine(
            const std::string & TEXT,
            const unsigned CHARACTER_SIZE,
            const FontPtr_t & LETTERS_FONT_PTR,
            const FontPtr_t & NUMBERS_FONT_PTR,
            const sf::Vector2f & POS_V,
            const float WIDTH_LIMIT,
            std::size_t & textPosIndex);

        // returns an empty vec with terminatingChar=0 if textPosIndex >= TEXT.size(), skips leading
        // spaces before rendering then continues until reaching a space or newline, returns an
        // empty vec with terminatingChar='\n' if a newline was the first non-space character found,
        // that final terminating character is NOT rendered so that the caller can decide if that
        // char needs to be fit on the current line or not
        static const SfTextVec_t RenderWords(
            const std::string & TEXT,
            const float WIDTH_LIMIT,
            const unsigned CHARACTER_SIZE,
            const FontPtr_t & LETTERS_FONT_PTR,
            const FontPtr_t & NUMBERS_FONT_PTR,
            const sf::Vector2f & POS_V,
            std::size_t & textPosIndex,
            const bool WILL_PREFIX_SPACE,
            char & terminatingChar);
    };

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_TEXTRENDERING_HPP_INCLUDED
