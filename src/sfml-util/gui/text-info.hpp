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
#include "sfml-util/justified-enum.hpp"
#include "sfml-util/sfml-util-color.hpp"
#include "sfml-util/sfml-util-font.hpp"
#include "sfml-util/sfml-util-vector-rect.hpp"

#include <SFML/Graphics/Text.hpp>

#include <ostream>
#include <string>
#include <vector>

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        // Responsible for wrapping all information required to draw text except for position.
        // A default constructed TextInfo is invalid because the text will be an empty string and
        // because there will be no font.  If used, such a TextInfo will cause the text rendering
        // code to throw exceptions.
        class TextInfo
        {
        public:
            TextInfo();

            TextInfo(
                const std::string & TEXT,
                const FontPtrOpt_t & FONT_PTR_OPT,
                const unsigned int CHAR_SIZE,
                const sf::Color & COLOR = sf::Color::White,
                const sf::BlendMode & BLEND_MODE = sf::BlendAlpha,
                const sf::Uint32 STYLE = sf::Text::Style::Regular,
                const Justified::Enum JUSTIFIED = Justified::Left,
                const bool IS_OUTLINE_ONLY = false,
                const float OUTLINE_THICKNESS = 0.0f);

            TextInfo(
                const std::string & TEXT,
                const FontPtr_t FONT_PTR,
                const unsigned int CHAR_SIZE,
                const sf::Color & COLOR,
                const Justified::Enum JUSTIFIED);

            TextInfo(
                const TextInfo & TEXT_INFO_TO_COPY,
                const std::string & TEXT,
                const ColorOpt_t & COLOR_OPT = boost::none,
                const unsigned CHAR_SIZE = 0,
                const FontPtrOpt_t NUMBERS_FONT_PTR_OPT = boost::none);

            TextInfo(const TextInfo & TEXT_INFO_TO_COPY, const FontPtr_t NEW_FONT_PTR);

            TextInfo(const TextInfo &) = default;
            TextInfo(TextInfo &&) = default;
            TextInfo & operator=(const TextInfo &) = default;
            TextInfo & operator=(TextInfo &&) = default;

            // returns true if there is non-empty text and a non-null font pointer
            bool IsValid() const
            {
                return ((text.empty() == false) && font_ptr_opt && (char_size != 0));
            }

            // throws if IsValid()==false
            void Apply(sf::Text &, const FontPtrOpt_t & CUSTOM_FONT_PTR_OPT = boost::none) const;

            // throws if IsValid()==false
            const sf::Text Make() const;

            const std::string ToString(
                const bool WILL_PREFIX = true,
                const misc::Wrap WILL_WRAP = misc::Wrap::Yes,
                const std::string & SEPARATOR = "/") const;

            std::string text;
            FontPtrOpt_t font_ptr_opt;
            unsigned int char_size;
            sf::Color color;
            sf::BlendMode blend_mode;
            sf::Uint32 style;
            Justified::Enum justified;
            bool is_outline;
            float outline_thickness;
        };

        using TextInfoOpt_t = boost::optional<TextInfo>;
        using TextInfoVec_t = std::vector<TextInfo>;

        bool operator<(const TextInfo & L, const TextInfo & R);

        bool operator==(const TextInfo & L, const TextInfo & R);

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
