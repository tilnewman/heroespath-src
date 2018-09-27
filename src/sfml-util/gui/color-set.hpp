// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_GUI_COLORSET_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_GUI_COLORSET_HPP_INCLUDED
//
// color-set.hpp
//
#include <SFML/Graphics/Color.hpp>

#include <string>

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        // Encapsulates the colors required to draw a Entity that can
        // have focus or not have focus.  Using the default constructor
        // will use sf::Color::White when HasFocus() and a slightly darker
        // white color when ! HasFocus().
        struct FocusColors
        {
            static const sf::Uint8 DEFAULT_OFFSET_VALUE_;
            static const sf::Color DEFAULT_OFFSET_COLOR_;

            // use this constructor for default white/offwhite colors
            FocusColors();

            // use this constructor to specify all custom colors
            FocusColors(
                const sf::Color & FG_WITH_FOCUS,
                const sf::Color & BG_WITH_FOCUS,
                const sf::Color & FG_WITHOUT_FOCUS,
                const sf::Color & BG_WITHOUT_FOCUS);

            // use this constructor when the bg is white and colors are the same with and without
            // focus
            explicit FocusColors(const sf::Color & FG);

            // use this constructor when the without colors are automatically darker than
            // those with
            FocusColors(const sf::Color & FG_WITH_FOCUS, const sf::Color & BG_WITH_FOCUS);

            void ForceWithoutFocusToMatchWithFocus();

            const std::string ToString(const bool WILL_WRAP = false) const;

            sf::Color foreground_with;
            sf::Color background_with;
            sf::Color foreground_without;
            sf::Color background_without;
        };

        bool operator<(const FocusColors & L, const FocusColors & R);

        bool operator==(const FocusColors & L, const FocusColors & R);

        inline bool operator!=(const FocusColors & L, const FocusColors & R) { return !(L == R); }

    } // namespace gui
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_GUI_COLORSET_HPP_INCLUDED
