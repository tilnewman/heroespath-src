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
//  Code regarding the details about drawn text.
//
#include "misc/boost-optional-that-throws.hpp"
#include "sfml-util/font-manager.hpp"
#include "sfml-util/justified-enum.hpp"
#include "sfml-util/sfml-util-color.hpp"

#include <SFML/Graphics/BlendMode.hpp>
#include <SFML/Graphics/Text.hpp>

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        // Responsible for wrapping all information required to draw text except for position.
        //
        // Note: A default constructed TextInfo is invalid and cannot be used, or exceptions will be
        // thrown.  See IsValid().
        class TextInfo
        {
        public:
            explicit TextInfo(
                const std::string & TEXT = "",
                const FontPtrOpt_t FONT_PTR_OPT = boost::none,
                const unsigned int CHAR_SIZE = FontManager::Instance()->Size_Normal(),
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

            // this looks weird but there are many cases where one TextInfo is made very similar to
            // another
            TextInfo(
                const TextInfo & TEXT_INFO_TO_COPY,
                const std::string & TEXT,
                const sf::Color & COLOR = sfml_util::Colors::None,
                const unsigned CHAR_SIZE = 0);

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
            void Apply(sf::Text &) const;

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
