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
#ifndef HEROESPATH_SFMLUTIL_GUI_IGUIENTITY_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_GUI_IGUIENTITY_HPP_INCLUDED
//
// i-gui-entity.hpp
//  Common interface for all gui elements
//
#include "sfml-util/gui/color-set.hpp"
#include "sfml-util/gui/i-clickable.hpp"
#include "sfml-util/mouse-state-enum.hpp"
#include "sfml-util/sfml-graphics.hpp"

#include <list>
#include <set>
#include <string>
#include <vector>

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        // Interface for GuiEntities
        class IGuiEntity
            : public sf::Drawable
            , public IClickable
        {
        public:
            IGuiEntity() = default;
            virtual ~IGuiEntity() = default;

            virtual bool GetEntityWillDraw() const = 0;
            virtual void SetEntityWillDraw(const bool WILL_DRAW) = 0;

            virtual const sf::Vector2f GetEntityPos() const = 0;
            virtual void SetEntityPos(const sf::Vector2f & V) = 0;
            virtual void SetEntityPos(const float LEFT, const float TOP) = 0;
            virtual void MoveEntityPos(const float HORIZ, const float VERT) = 0;

            virtual const std::string GetEntityName() const = 0;

            virtual const sf::FloatRect GetEntityRegion() const = 0;
            virtual void SetEntityRegion(const sf::FloatRect & R) = 0;

            virtual bool IsInEntityRegion(const sf::Vector2f & V) const = 0;

            virtual void SetEntityColors(const ColorSet & COLOR_SET) = 0;
            virtual void SetEntityColorFg(const sf::Color & FG_COLOR) = 0;
            virtual void SetEntityColorFgBoth(const sf::Color & FG_COLOR) = 0;
            virtual const ColorSet GetEntityColors() const = 0;
            virtual const sf::Color GetEntityColorForeground() const = 0;
            virtual const sf::Color GetEntityColorBackground() const = 0;

            virtual bool KeyPress(const sf::Event::KeyEvent &) = 0;
            virtual bool KeyRelease(const sf::Event::KeyEvent &) = 0;

            virtual bool UpdateTime(const float ELAPSED_TIME_SEC) = 0;

        protected:
            virtual void OnColorChange() = 0;

            // The only member required by sf::Drawable.
        public:
            virtual void
                draw(sf::RenderTarget & target, sf::RenderStates states) const override = 0;

            // IClickable
            MouseState::Enum GetMouseState() const override = 0;
            void SetMouseState(const MouseState::Enum) override = 0;
            bool MouseUp(const sf::Vector2f & MOUSE_POS_V) override = 0;
            bool MouseDown(const sf::Vector2f & MOUSE_POS_V) override = 0;
            bool IsHeldDown() const override = 0;
            bool UpdateMousePos(const sf::Vector2f & MOUSE_POS_V) override = 0;
            virtual bool UpdateMouseWheel(
                const sf::Vector2f & MOUSE_POS_V, const float WHEEL_MOTION) override = 0;
            bool HasFocus() const override = 0;
            bool SetHasFocus(const bool) override = 0;
            bool WillAcceptFocus() const override = 0;
            void SetWillAcceptFocus(const bool) override = 0;
            void FakeColorSetAsIfFocusIs(const bool) override = 0;
            const std::string GetMouseHoverText() override = 0;
            void SetMouseHoverText(const std::string &) override = 0;

        protected:
            void OnClick(const sf::Vector2f &) override = 0;
            void OnDoubleClick(const sf::Vector2f &) override = 0;
        };

        using IGuiEntityPtr_t = IGuiEntity *;
        using IGuiEntityPVec_t = std::vector<IGuiEntityPtr_t>;
    } // namespace gui
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_GUI_IGUIENTITY_HPP_INCLUDED
