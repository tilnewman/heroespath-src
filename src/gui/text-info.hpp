// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_TEXTINFO_HPP_INCLUDED
#define HEROESPATH_GUI_TEXTINFO_HPP_INCLUDED
//
// text-info.hpp
//
#include "gui/font-enum.hpp"
#include "gui/justified-enum.hpp"
#include "misc/boost-optional-that-throws.hpp"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>

#include <ostream>
#include <string>
#include <vector>

namespace heroespath
{
namespace gui
{

    class Text;

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
            const sf::Uint32 STYLE = 0); // zero is sf::Text::Style::Regular

        // creates a modified copy of another TextInfo, params with default values ignored
        TextInfo(
            const TextInfo & TEXT_INFO_TO_COPY,
            const std::string & TEXT,
            const sf::Color & COLOR = sf::Color::Transparent,
            const unsigned SIZE = 0,
            const GuiFont::Enum LETTERS_FONT = GuiFont::Count,
            const GuiFont::Enum NUMBERS_FONT = GuiFont::Count);

        TextInfo(const TextInfo &) = default;
        TextInfo(TextInfo &&) = default;
        TextInfo & operator=(const TextInfo &) = default;
        TextInfo & operator=(TextInfo &&) = default;

        // returns true if size and fonts are valid and if text is not empty
        bool IsValid() const;

        // returns true if IsValid() and color is not transparent
        bool WillDraw() const
        {
            return (IsValid() && (color.a > 0) && (size > 0) && !text.empty());
        }

        const std::string ToString(
            const bool WILL_PREFIX = true,
            const Wrap WILL_WRAP = Wrap::Yes,
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

inline std::ostream & operator<<(std::ostream & os, const gui::TextInfo & TI)
{
    os << TI.ToString();
    return os;
}

} // namespace heroespath

#endif // HEROESPATH_GUI_TEXTINFO_HPP_INCLUDED
