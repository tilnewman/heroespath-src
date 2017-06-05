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
#ifndef SFMLUTIL_GUIENTITY_INCLUDED
#define SFMLUTIL_GUIENTITY_INCLUDED
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
        //prevent copy construction
        GuiEntity(const GuiEntity &) =delete;

        //prevent copy assignment
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
        virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const = 0;

        //Required by IClickable
        inline virtual MouseState::Enum GetMouseState() const                   { return entityMouseState_; }
        inline virtual void SetMouseState(const MouseState::Enum E)             { entityMouseState_ = E; }

        virtual bool MouseUp(const sf::Vector2f & MOUSE_POS_V);
        virtual bool MouseDown(const sf::Vector2f & MOUSE_POS_V);

        inline virtual bool IsHeldDown() const                                  { return (MouseState::Down == GetMouseState()); }

        //returns true only if the MouseState changed
        virtual bool UpdateMousePos(const sf::Vector2f & MOUSE_POS_V);

        //disabled until bug in wheel movement direction can be resolved zTn 2016-12-11
        inline virtual bool UpdateMouseWheel(const sf::Vector2f &, const float) { return false; }

        inline virtual bool HasFocus() const                                    { return entityHasFocus_; }
        virtual bool SetHasFocus(const bool);
        inline virtual bool WillAcceptFocus() const                             { return entityWillFocus_; }
        inline virtual void SetWillAcceptFocus(const bool WILL_ACCEPT_FOCUS)    { entityWillFocus_ = WILL_ACCEPT_FOCUS; }
        inline virtual const std::string GetMouseHoverText(const sf::Vector2f &){ return entityMouseHoverText_; }
        inline virtual void SetMouseHoverText(const std::string & S)            { entityMouseHoverText_ = S; }

    protected:
        virtual void OnClick(const sf::Vector2f &) = 0;
        inline virtual void OnDoubleClick(const sf::Vector2f &) {}
        inline virtual void OnColorChange() {}
        void ChangeColors();

        //Required by IGuiEntity
    public:
        inline virtual bool GetEntityWillDraw() const                           { return entityWillDraw_; }
        inline virtual void SetEntityWillDraw(const bool WILL_DRAW)             { entityWillDraw_ = WILL_DRAW; }

        inline virtual const sf::Vector2f GetEntityPos() const                  { return sf::Vector2f(entityRegion_.left, entityRegion_.top); }
        //
        inline virtual void SetEntityPos(const sf::Vector2f & V)                { SetEntityPos(V.x, V.y); }
        virtual void SetEntityPos(const float POS_LEFT, const float POS_TOP);
        virtual void MoveEntityPos(const float HORIZ, const float VERT);

        void MoveEntityOffScreen();
        void MoveEntityBackFromOffScreen();

        inline virtual bool IsInEntityRegion(const sf::Vector2f & V) const      { return entityRegion_.contains(V); }

        virtual void SetEntityColors(const ColorSet & COLOR_SET);
        virtual void SetEntityColorFg(const sf::Color & FG_COLOR);
        virtual void SetEntityColorFgBoth(const sf::Color & FG_COLOR);
        inline virtual const ColorSet GetEntityColors() const                   { return entityColorSet_; }
        inline virtual const sf::Color GetEntityColorForeground() const         { return entityFgColor_; }
        inline virtual const sf::Color GetEntityColorBackground() const         { return entityBgColor_; }
        virtual void FakeColorSetAsIfFocusIs(const bool);

        inline virtual const std::string GetEntityName() const                  { return entityName_; }

        inline virtual const sf::FloatRect GetEntityRegion() const              { return entityRegion_; }
        inline virtual void  SetEntityRegion(const sf::FloatRect & REGION)      { entityRegion_ = REGION; }

        inline virtual bool UpdateTime(const float)                             { return false; }

        inline virtual bool KeyPress(const sf::Event::KeyEvent &)               { return false; }
        inline virtual bool KeyRelease(const sf::Event::KeyEvent &)             { return false; }

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

#endif //SFMLUTIL_GUIENTITY_INCLUDED
