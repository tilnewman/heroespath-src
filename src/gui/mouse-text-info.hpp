// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_MOUSETEXTINFO_HPP_INCLUDED
#define HEROESPATH_GUI_MOUSETEXTINFO_HPP_INCLUDED
//
// mouse-text-info.hpp
//  A handy wrapper struct that holds three colors relating to mouse position.
//
#include "gui/font-enum.hpp"
#include "gui/mouse-state-enum.hpp"
#include "gui/text-info.hpp"
#include "popup/popup-enums.hpp"
#include "sfutil/vector-and-rect.hpp"

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace gui
{

    class MouseTextInfo
    {
    public:
        MouseTextInfo();

        explicit MouseTextInfo(const TextInfo & TEXT_INFO);

        MouseTextInfo(
            const TextInfo & UP,
            const TextInfo & DOWN,
            const TextInfo & OVER,
            const TextInfo & DISABLED);

        MouseTextInfo(
            const TextInfo & TEXT_INFO_UP,
            const sf::Color & COLOR_DOWN,
            const sf::Color & COLOR_OVER,
            const sf::Color & COLOR_DISABLED);

        MouseTextInfo(
            const std::string & TEXT,
            const GuiFont::Enum LETTER_FONT,
            const unsigned int SIZE,
            const sf::Color & COLOR_UP = sf::Color(220, 220, 220),
            const sf::Color & COLOR_DOWN = sf::Color::White,
            const sf::Color & COLOR_OVER = sf::Color::White,
            const sf::Color & COLOR_DISABLED = sf::Color::White);

        MouseTextInfo(const MouseTextInfo &) = default;
        MouseTextInfo(MouseTextInfo &&) = default;
        MouseTextInfo & operator=(const MouseTextInfo &) = default;
        MouseTextInfo & operator=(MouseTextInfo &&) = default;

        // MouseTextInfo generating helper functions
        static const MouseTextInfo
            Make_PopupButtonSet(const std::string & NAME, const popup::PopupButtonColor::Enum);

        static const MouseTextInfo Make_InteractionButtonSet(const std::string & NAME);

        TextInfo & FromMouseState(const MouseState::Enum MOUSE_STATE)
        {
            return const_cast<TextInfo &>(
                static_cast<const MouseTextInfo &>(*this).FromMouseState(MOUSE_STATE));
        }

        // returns 'up' if MOUSE_STATE is invalid
        const TextInfo & FromMouseState(const MouseState::Enum MOUSE_STATE) const;

        const std::string ToString(
            const bool WILL_PREFIX = true,
            const Wrap WILL_WRAP = Wrap::Yes,
            const std::string & SEPARATOR = "_") const;

        TextInfo up;
        TextInfo down;
        TextInfo over;
        TextInfo disabled;
    };

    using MouseTextInfoVec_t = std::vector<MouseTextInfo>;

    bool operator<(const MouseTextInfo & L, const MouseTextInfo & R);

    bool operator==(const MouseTextInfo & L, const MouseTextInfo & R);

    inline bool operator!=(const MouseTextInfo & L, const MouseTextInfo & R) { return !(L == R); }

} // namespace gui

inline std::ostream & operator<<(std::ostream & os, const gui::MouseTextInfo & MTI)
{
    os << MTI.ToString();
    return os;
}

} // namespace heroespath

#endif // HEROESPATH_GUI_MOUSETEXTINFO_HPP_INCLUDED
