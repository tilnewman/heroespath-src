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
#ifndef SFMLUTIL_GUIENTITY_HPP_INCLUDED
#define SFMLUTIL_GUIENTITY_HPP_INCLUDED
//
// gui-entity.hpp
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/sfml-system.hpp"
#include "sfml-util/mouse-state-enum.hpp"
#include "sfml-util/gui/i-clickable.hpp"
#include "sfml-util/gui/i-gui-entity.hpp"
#include "sfml-util/gui/color-set.hpp"

#include <memory>
#include <string>
#include <vector>


namespace sfml_util
{
namespace gui
{

    //Encapsulates the basics of everything needed by a clickable gui element.
    class GuiEntity : public IGuiEntity
    {
        GuiEntity(const GuiEntity &) =delete;
        GuiEntity & operator=(const GuiEntity &) =delete;

    public:
        GuiEntity(  const std::string &   NAME,
                    const sf::FloatRect & SCREEN_REGION,
                    const ColorSet &      COLORS = ColorSet());

        GuiEntity(  const std::string &  NAME,
                    const sf::Vector2f & SCREEN_POS_V,
                    const ColorSet &     COLORS = ColorSet());

        GuiEntity(  const std::string & NAME,
                    const float         POS_LEFT,
                    const float         POS_TOP,
                    const ColorSet &    COLORS = ColorSet());

        virtual ~GuiEntity();

        //The only member required by sf::Drawable.
        void draw(sf::RenderTarget & target, sf::RenderStates states) const override = 0;

        //Required by IClickable
        inline MouseState::Enum GetMouseState() const override          { return entityMouseState_; }
        inline void SetMouseState(const MouseState::Enum E) override    { entityMouseState_ = E; }

        bool MouseUp(const sf::Vector2f & MOUSE_POS_V) override;
        bool MouseDown(const sf::Vector2f & MOUSE_POS_V) override;

        inline bool IsHeldDown() const override { return (MouseState::Down == GetMouseState()); }

        //returns true only if the MouseState changed
        bool UpdateMousePos(const sf::Vector2f & MOUSE_POS_V) override;

        //disabled until bug in wheel movement direction can be resolved zTn 2016-12-11
        inline bool UpdateMouseWheel(const sf::Vector2f &, const float) override { return false; }

        inline bool HasFocus() const override { return entityHasFocus_; }
        bool SetHasFocus(const bool) override;
        inline bool WillAcceptFocus() const override { return entityWillFocus_; }
        inline void SetWillAcceptFocus(const bool WILL) override { entityWillFocus_ = WILL; }
        inline const std::string GetMouseHoverText() override { return entityMouseHoverText_; }

        inline void SetMouseHoverText(const std::string & S) override{ entityMouseHoverText_ = S; }

    protected:
        void OnClick(const sf::Vector2f &) override = 0;
        inline void OnDoubleClick(const sf::Vector2f &) override {}
        inline void OnColorChange() override {}
        void ChangeColors();

        //Required by IGuiEntity
    public:
        inline bool GetEntityWillDraw() const override { return entityWillDraw_; }
        inline void SetEntityWillDraw(const bool WILL_DRAW) override { entityWillDraw_ = WILL_DRAW; }

        inline const sf::Vector2f GetEntityPos() const override
        {
            return sf::Vector2f(entityRegion_.left, entityRegion_.top);
        }
        
        inline void SetEntityPos(const sf::Vector2f & V) override { SetEntityPos(V.x, V.y); }
        void SetEntityPos(const float POS_LEFT, const float POS_TOP) override;
        void MoveEntityPos(const float HORIZ, const float VERT) override;

        void MoveEntityOffScreen();
        void MoveEntityBackFromOffScreen();

        inline bool IsInEntityRegion(const sf::Vector2f & V) const override
        {
            return entityRegion_.contains(V);
        }

        void SetEntityColors(const ColorSet & COLOR_SET) override;
        void SetEntityColorFg(const sf::Color & FG_COLOR) override;
        void SetEntityColorFgBoth(const sf::Color & FG_COLOR) override;
        inline const ColorSet GetEntityColors() const override { return entityColorSet_; }
        inline const sf::Color GetEntityColorForeground() const override { return entityFgColor_; }
        inline const sf::Color GetEntityColorBackground() const override { return entityBgColor_; }
        void FakeColorSetAsIfFocusIs(const bool) override;

        inline const std::string GetEntityName() const override { return entityName_; }

        inline const sf::FloatRect GetEntityRegion() const override { return entityRegion_; }

        inline void  SetEntityRegion(const sf::FloatRect & REGION) override
        {
            entityRegion_ = REGION;
        }

        inline bool UpdateTime(const float) override { return false; }

        inline bool KeyPress(const sf::Event::KeyEvent &) override { return false; }
        inline bool KeyRelease(const sf::Event::KeyEvent &) override { return false; }

        friend bool operator<(const GuiEntity & L, const GuiEntity & R);
        friend bool operator==(const GuiEntity & L, const GuiEntity & R);

    protected:
        static const sf::Int32 DOUBLE_CLICK_TIME_MS_;
        //
        std::string      entityName_;
        sf::FloatRect    entityRegion_;
        MouseState::Enum entityMouseState_;
        ColorSet         entityColorSet_;
        sf::Color        entityFgColor_;
        sf::Color        entityBgColor_;
        bool             entityHasFocus_;
        bool             entityWillFocus_;
        bool             entityWillDraw_;
        std::string      entityMouseHoverText_;
        std::unique_ptr<sf::Clock> entityClockUPtr_;
        sf::Vector2f     entityPrevPos_;
    };


    bool operator<(const GuiEntity & L, const GuiEntity & R);

    bool operator==(const GuiEntity & L, const GuiEntity & R);

    inline bool operator!=(const GuiEntity & L, const GuiEntity & R)
    {
        return ! (L == R);
    }

}
}

#endif //SFMLUTIL_GUIENTITY_HPP_INCLUDED
