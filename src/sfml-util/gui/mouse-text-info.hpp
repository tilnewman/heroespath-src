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
#include "sfml-util/mouse-state-enum.hpp"
#include "sfml-util/sfml-util-vector-rect.hpp"

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
            explicit MouseTextInfo(const FontPtrOpt_t NUMBERS_FONT_PTR_OPT = boost::none);

            explicit MouseTextInfo(
                const TextInfo & TEXT_INFO, const FontPtrOpt_t NUMBERS_FONT_PTR_OPT = boost::none);

            MouseTextInfo(
                const TextInfo & UP,
                const TextInfo & DOWN,
                const TextInfo & OVER,
                const TextInfo & DISABLED,
                const FontPtrOpt_t NUMBERS_FONT_PTR_OPT = boost::none);

            MouseTextInfo(
                const TextInfo & TEXT_INFO_UP,
                const sf::Color & COLOR_DOWN,
                const sf::Color & COLOR_OVER,
                const sf::Color & COLOR_DISABLED,
                const FontPtrOpt_t NUMBERS_FONT_PTR_OPT = boost::none);

            MouseTextInfo(
                const std::string & TEXT,
                const FontPtr_t FONT_PTR,
                const unsigned int SIZE,
                const sf::Color & COLOR_UP = sf::Color(220, 220, 220),
                const sf::Color & COLOR_DOWN = sf::Color::White,
                const sf::Color & COLOR_OVER = sf::Color::White,
                const sf::Color & COLOR_DISABLED = sf::Color::White,
                const FontPtrOpt_t NUMBERS_FONT_PTR_OPT = boost::none);

            MouseTextInfo(const MouseTextInfo &) = default;
            MouseTextInfo(MouseTextInfo &&) = default;
            MouseTextInfo & operator=(const MouseTextInfo &) = default;
            MouseTextInfo & operator=(MouseTextInfo &&) = default;

            // MouseTextInfo generating helper functions
            static const MouseTextInfo Make_PopupButtonSet(
                const std::string & NAME,
                const popup::PopupButtonColor::Enum,
                const FontPtrOpt_t NUMBERS_FONT_PTR_OPT = boost::none);

            static const MouseTextInfo Make_InteractionButtonSet(
                const std::string & NAME, const FontPtrOpt_t NUMBERS_FONT_PTR_OPT = boost::none);

            TextInfo & FromMouseState(const MouseState::Enum MOUSE_STATE)
            {
                return const_cast<TextInfo &>(
                    static_cast<const MouseTextInfo &>(*this).FromMouseState(MOUSE_STATE));
            }

            // returns 'up' if MOUSE_STATE is invalid
            const TextInfo & FromMouseState(const MouseState::Enum MOUSE_STATE) const
            {
                if (MOUSE_STATE == MouseState::Down)
                {
                    return down;
                }
                else if (MOUSE_STATE == MouseState::Over)
                {
                    return over;
                }
                else
                {
                    return up;
                }
            }

            const std::string ToString(
                const bool WILL_PREFIX = true,
                const misc::Wrap WILL_WRAP = misc::Wrap::Yes,
                const std::string & SEPARATOR = "_") const;

            TextInfo up;
            TextInfo down;
            TextInfo over;
            TextInfo disabled;
            FontPtr_t numbers_font_ptr;

        private:
            FontPtr_t OptionalNumbersFontOrDefault(const FontPtrOpt_t &) const;
        };

        using MouseTextInfoVec_t = std::vector<MouseTextInfo>;

        bool operator<(const MouseTextInfo & L, const MouseTextInfo & R);

        bool operator==(const MouseTextInfo & L, const MouseTextInfo & R);

        inline bool operator!=(const MouseTextInfo & L, const MouseTextInfo & R)
        {
            return !(L == R);
        }

    } // namespace gui
} // namespace sfml_util

inline std::ostream & operator<<(std::ostream & os, const sfml_util::gui::MouseTextInfo & MTI)
{
    os << MTI.ToString();
    return os;
}

} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_GUI_MOUSETEXTINFO_HPP_INCLUDED
