// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_ICLICKABLE_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_ICLICKABLE_HPP_INCLUDED
//
// i-clickable.hpp
//
#include "sfml-util/mouse-state-enum.hpp"

#include <SFML/System/Vector2.hpp>

#include <string>

namespace heroespath
{
namespace sfml_util
{

    // Responsible for a common interface for all "clickable" Entities.
    struct IClickable
    {
        IClickable() = default;
        virtual ~IClickable() = default;

        virtual MouseState::Enum GetMouseState() const = 0;

        // returns true if the mouse state changed
        virtual bool SetMouseState(const MouseState::Enum) = 0;

        // returns true if the entityMouseState_ has changed, OnClick() or OnDoubleClick() might
        // have been called
        virtual bool MouseUp(const sf::Vector2f & MOUSE_POS_V) = 0;

        // returns true if the entityMouseState_ changed
        virtual bool MouseDown(const sf::Vector2f & MOUSE_POS_V) = 0;

        // returns true only when entityMouseState_ changed.
        virtual bool UpdateMousePos(const sf::Vector2f & MOUSE_POS_V) = 0;

        // called from within the game loop if RequiresMouseWheelUpdates()
        virtual bool UpdateMouseWheel(const sf::Vector2f & MOUSE_POS_V, const float WHEEL_MOTION)
            = 0;

        virtual bool HasFocus() const = 0;

        // returns true if the focus state changed
        virtual bool SetHasFocus(const bool) = 0;

        virtual bool WillAcceptFocus() const = 0;
        virtual void SetWillAcceptFocus(const bool) = 0;
        virtual void FakeFocusColorsAsIfFocusIs(const bool) = 0;

        virtual const std::string GetMouseHoverText() = 0;
        virtual void SetMouseHoverText(const std::string &) = 0;

    protected:
        // called only if MouseUp() occurs while still withing the entity's region
        virtual void OnClick(const sf::Vector2f &) = 0;
        virtual void OnDoubleClick(const sf::Vector2f &) = 0;
    };

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_ICLICKABLE_HPP_INCLUDED
