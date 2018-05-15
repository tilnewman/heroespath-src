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
#include "sfml-util/font-manager.hpp"
#include "sfml-util/justified-enum.hpp"
#include "sfml-util/sfml-graphics.hpp"

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        // Encapsulates all required information about drawing text except for position.
        // Note:  Using all default values is not generally safe, and may cause some
        //       functions to throw exceptions.  To be safe, always provide a non-empty
        //       TEXT parameter, and a non-null FONT pointer, that will always work with
        //       all functions.  You can use the IsValid() function to check these conditions.
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

            // returns true if there is non-empty text and a non-null font pointer
            bool IsValid() const { return ((false == text.empty()) && fontPtrOpt); }

            std::string text;
            FontPtrOpt_t fontPtrOpt;
            unsigned int charSize;
            sf::Color color;
            sf::BlendMode blendMode;
            sf::Uint32 style;
            Justified::Enum justified;
            bool isOutlineOnly;
            float outlineThickness;
        };

        bool operator<(const TextInfo & L, const TextInfo & R);

        bool operator==(const TextInfo & L, const TextInfo & R);

        inline bool operator!=(const TextInfo & L, const TextInfo & R) { return !(L == R); }

        using TextInfoUPtr_t = std::unique_ptr<TextInfo>;
        using TextInfoVec_t = std::vector<TextInfo>;

        // Helper function used to setup an sf::Text object
        void SetupText(sf::Text & text, const TextInfo & TEXT_INFO);

    } // namespace gui
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_GUI_TEXTINFO_HPP_INCLUDED
