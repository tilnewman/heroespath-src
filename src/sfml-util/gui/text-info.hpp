// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_GUI_TEXTINFO_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_GUI_TEXTINFO_HPP_INCLUDED
//
// text-info.hpp
//
#include "misc/boost-optional-that-throws.hpp"
#include "sfml-util/font-enum.hpp"
#include "sfml-util/justified-enum.hpp"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>

#include <ostream>
#include <string>
#include <vector>

namespace heroespath
{
namespace sfml_util
{

    class Text;

    namespace gui
    {

        // Responsible for wrapping all information required to draw text except for position and
        // blend mode. The default constructor creates an invalid TextInfo.  Rendering an invalid
        // TextInfo will render nothing and cause a warning to be logged.
        struct TextInfo
        {
            TextInfo();

            TextInfo(
                const std::string & TEXT,
                const GuiFont::Enum LETTERS_FONT,
                const unsigned int SIZE,
                const sf::Color & COLOR = sf::Color::White,
                const Justified::Enum JUSTIFIED = Justified::Left,
                const sf::Uint32 STYLE = 0); // 0=sf::Text::Style::Regular

            // creates a modified copy of another TextInfo, params with default values ignored
            TextInfo(
                const TextInfo & TEXT_INFO_TO_COPY,
                const std::string & TEXT,
                const sf::Color & COLOR = sf::Color::Transparent,
                const unsigned SIZE = 0,
                const GuiFont::Enum LETTERS_FONT = GuiFont::Count,
                const GuiFont::Enum NUMBERS_FONT = GuiFont::Count);

            // creates a TextInfo that is a copy of another with different fonts, params with
            // default values ignored
            // TextInfo(
            //    const TextInfo & TEXT_INFO_TO_COPY,
            //    const GuiFont::Enum NEW_LETTERS_FONT,
            //    const GuiFont::Enum NEW_NUMBERS_FONT = GuiFont::Count);

            TextInfo(const TextInfo &) = default;
            TextInfo(TextInfo &&) = default;
            TextInfo & operator=(const TextInfo &) = default;
            TextInfo & operator=(TextInfo &&) = default;

            // returns true if size and fonts are valid and if text is not empty
            bool IsValid() const
            {
                return (
                    (text.empty() == false) && GuiFont::IsValid(font_letters)
                    && GuiFont::IsValid(font_numbers) && (size > 0));
            }

            // returns true if IsValid() and color is not transparent
            bool WillDraw() const { return (IsValid() && (color.a > 0)); }

            const std::string ToString(
                const bool WILL_PREFIX = true,
                const misc::Wrap WILL_WRAP = misc::Wrap::Yes,
                const std::string & SEPARATOR = "/") const;

            std::string text;
            GuiFont::Enum font_letters;
            GuiFont::Enum font_numbers;
            unsigned int size;
            sf::Color color;
            sf::Uint32 style;
            Justified::Enum justified;
        };

        using TextInfoOpt_t = boost::optional<TextInfo>;
        using TextInfoVec_t = std::vector<TextInfo>;

        // comparison ordered to group similar: font/justification/style/size/color/text
        bool operator<(const TextInfo & L, const TextInfo & R);

        // comparison ordered for speed
        bool operator==(const TextInfo & L, const TextInfo & R);

        // comparison ordered for speed
        inline bool operator!=(const TextInfo & L, const TextInfo & R) { return !(L == R); }

    } // namespace gui
} // namespace sfml_util

inline std::ostream & operator<<(std::ostream & os, const sfml_util::gui::TextInfo & TI)
{
    os << TI.ToString();
    return os;
}

} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_GUI_TEXTINFO_HPP_INCLUDED
