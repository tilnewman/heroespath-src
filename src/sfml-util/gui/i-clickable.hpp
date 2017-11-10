///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software.  If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef HEROESPATH_SFMLUTIL_GUI_ICLICKABLE_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_GUI_ICLICKABLE_HPP_INCLUDED
//
// i-clickable.hpp
//  Interface for all click-able gui elements
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/mouse-state-enum.hpp"

#include <string>


namespace heroespath
{
namespace sfml_util
{
namespace gui
{

    //Actions required of a gui element that is clickable.
    struct IClickable
    {
        IClickable() {}
        virtual ~IClickable() {}

        virtual MouseState::Enum GetMouseState() const = 0;
        virtual void SetMouseState(const MouseState::Enum) = 0;

        //returns true only if the entityMouseState_ has changed
        //calls OnClick() only if the MOUSE_POS_V was within the entity's region
        virtual bool MouseUp(const sf::Vector2f & MOUSE_POS_V) = 0;

        //returns true only if the entityMouseState_ changed due to the
        //MOUSE_POS_V being within the entity's region
        virtual bool MouseDown(const sf::Vector2f & MOUSE_POS_V) = 0;

        virtual bool IsHeldDown() const = 0;

        //called from within the draw loop if RequiresMouseUpdates()
        //returns true only when entityMouseState_ changed.
        virtual bool UpdateMousePos(const sf::Vector2f & MOUSE_POS_V) = 0;

        //called from within the game loop if RequiresMouseWheelUpdates()
        virtual bool UpdateMouseWheel(const sf::Vector2f & MOUSE_POS_V, const float WHEEL_MOTION) = 0;

        virtual bool HasFocus() const = 0;

        //returns true if the focus state changed
        virtual bool SetHasFocus(const bool) = 0;

        virtual bool WillAcceptFocus() const = 0;
        virtual void SetWillAcceptFocus(const bool) = 0;
        virtual void FakeColorSetAsIfFocusIs(const bool) = 0;

        virtual const std::string GetMouseHoverText() = 0;
        virtual void SetMouseHoverText(const std::string &) = 0;

    protected:
        //called only if MouseUp() occurs while still withing the entity's region
        virtual void OnClick(const sf::Vector2f &) = 0;
        virtual void OnDoubleClick(const sf::Vector2f &) = 0;
    };

}
}
}

#endif //HEROESPATH_SFMLUTIL_GUI_ICLICKABLE_HPP_INCLUDED
