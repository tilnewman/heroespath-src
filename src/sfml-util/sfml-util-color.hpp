// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_SFMLUTIL_COLOR_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_SFMLUTIL_COLOR_HPP_INCLUDED
//
// sfml-util-color.hpp
//
#include "misc/boost-optional-that-throws.hpp"
#include "misc/to-string-prefix-enum.hpp"
#include "sfml-util/gui/color-set.hpp"

#include <SFML/Graphics/Color.hpp>

#include <string>

namespace sf
{

class Text;

bool operator<(const sf::Color & L, const sf::Color & R);
bool operator>(const sf::Color & L, const sf::Color & R);
bool operator<=(const sf::Color & L, const sf::Color & R);
bool operator>=(const sf::Color & L, const sf::Color & R);

// operator== already provided by sfml

std::ostream & operator<<(std::ostream & os, const sf::Color & C);

} // namespace sf

namespace heroespath
{

using ColorOpt_t = boost::optional<sf::Color>;

namespace sfml_util
{

    namespace defaults
    {
        const sf::Color COLOR {};

        const sf::Color GrayLight { 200, 200, 200 };
        const sf::Color GrayLighter { 232, 232, 232 };
        const sf::Color GrayDark { 100, 100, 100 };
        const sf::Color GrayDarker { 64, 64, 64 };
        const sf::Color Orange { 255, 223, 181 };
        const sf::Color Light { 220, 220, 220 };
        const sf::Color GoldLight { 255, 248, 220 };
        const sf::Color FocusIn { 220, 220, 220 };
        const sf::Color FocusOut { 180, 180, 180 };

        // same as sf::Color::Transparent
        const sf::Color None { 0, 0, 0, 0 };

        const sfml_util::gui::FocusColors GuiFocusColors { FocusIn, FocusIn, FocusOut, FocusOut };

    } // namespace defaults

    const sf::Color MakeHighlight(const sf::Uint8 COLOR_VALUE);

    const std::string ToString(
        const sf::Color & C,
        const misc::ToStringPrefix::Enum OPTIONS = misc::ToStringPrefix::Default);

    // return a color between FROM and TO at the given RATIO
    const sf::Color Transition(const sf::Color & FROM, const sf::Color & TO, const float RATIO);

    bool IsEqualWithoutAlpha(const sf::Color & L, const sf::Color & R);
    bool IsLessWithoutAlpha(const sf::Color & L, const sf::Color & R);

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_SFMLUTIL_COLOR_HPP_INCLUDED
