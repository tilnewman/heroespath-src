// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_GUI_MOUSETEXTINFO_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_GUI_MOUSETEXTINFO_HPP_INCLUDED
//
// mouse-text-info.hpp
//  A handy wrapper struct that holds three colors relating to mouse position.
//
#include "popup/popup-enums.hpp"
#include "sfml-util/gui/text-info.hpp"
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

        class MouseTextInfo
        {
        public:
            MouseTextInfo();

            explicit MouseTextInfo(const TextInfo & TEXT_INFO);

            MouseTextInfo(const TextInfo & UP, const TextInfo & DOWN, const TextInfo & OVER);

            MouseTextInfo(
                const TextInfo & TEXT_INFO_UP,
                const sf::Color & COLOR_DOWN,
                const sf::Color & COLOR_OVER);

            MouseTextInfo(
                const std::string & TEXT,
                const FontPtr_t FONT_PTR,
                const unsigned int SIZE,
                const sf::Color & COLOR_UP = sf::Color(220, 220, 220),
                const sf::Color & COLOR_DOWN = sf::Color::White,
                const sf::Color & COLOR_OVER = sf::Color::White);

            // MouseTextInfo generating helper functions
            static const MouseTextInfo
                Make_PopupButtonSet(const std::string & NAME, const popup::PopupButtonColor::Enum);

            static const MouseTextInfo Make_InteractionButtonSet(const std::string & NAME);

            TextInfo up;
            TextInfo down;
            TextInfo over;
        };

        using MouseTextInfoUPtr_t = std::unique_ptr<MouseTextInfo>;
        using MouseTextInfoVec_t = std::vector<MouseTextInfo>;

        bool operator<(const MouseTextInfo & L, const MouseTextInfo & R);

        bool operator==(const MouseTextInfo & L, const MouseTextInfo & R);

        inline bool operator!=(const MouseTextInfo & L, const MouseTextInfo & R)
        {
            return !(L == R);
        }

    } // namespace gui
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_GUI_MOUSETEXTINFO_HPP_INCLUDED
