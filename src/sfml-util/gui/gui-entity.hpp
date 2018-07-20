// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_GUIENTITY_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_GUIENTITY_HPP_INCLUDED
//
// gui-entity.hpp
//
#include "sfml-util/gui/color-set.hpp"
#include "sfml-util/gui/i-clickable.hpp"
#include "sfml-util/gui/i-gui-entity.hpp"
#include "sfml-util/mouse-state-enum.hpp"
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

        // Encapsulates the basics of everything needed by a clickable gui element.
        class GuiEntity : public IGuiEntity
        {
        public:
            GuiEntity(const GuiEntity &) = delete;
            GuiEntity(GuiEntity &&) = delete;
            GuiEntity & operator=(const GuiEntity &) = delete;
            GuiEntity & operator=(GuiEntity &&) = delete;

        public:
            GuiEntity(
                const std::string & NAME,
                const sf::FloatRect & SCREEN_REGION,
                const ColorSet & COLORS = ColorSet());

            GuiEntity(
                const std::string & NAME,
                const sf::Vector2f & SCREEN_POS_V,
                const ColorSet & COLORS = ColorSet());

            GuiEntity(
                const std::string & NAME,
                const float POS_LEFT,
                const float POS_TOP,
                const ColorSet & COLORS = ColorSet());

            virtual ~GuiEntity();

            // The only member required by sf::Drawable.
            void draw(sf::RenderTarget & target, sf::RenderStates states) const override = 0;

            // Required by IClickable
            MouseState::Enum GetMouseState() const override { return entityMouseState_; }

            void SetMouseState(const MouseState::Enum E) override { entityMouseState_ = E; }

            bool MouseUp(const sf::Vector2f & MOUSE_POS_V) override;
            bool MouseDown(const sf::Vector2f & MOUSE_POS_V) override;

            bool IsHeldDown() const override { return (MouseState::Down == GetMouseState()); }

            // returns true only if the MouseState changed
            bool UpdateMousePos(const sf::Vector2f & MOUSE_POS_V) override;

            // disabled until bug in wheel movement direction can be resolved zTn 2016-12-11
            bool UpdateMouseWheel(const sf::Vector2f &, const float) override { return false; }

            bool HasFocus() const override { return entityHasFocus_; }
            bool SetHasFocus(const bool) override;
            bool WillAcceptFocus() const override { return entityWillFocus_; }
            void SetWillAcceptFocus(const bool WILL) override { entityWillFocus_ = WILL; }
            const std::string GetMouseHoverText() override { return entityMouseHoverText_; }

            void SetMouseHoverText(const std::string & S) override { entityMouseHoverText_ = S; }

        protected:
            void OnClick(const sf::Vector2f &) override = 0;
            void OnDoubleClick(const sf::Vector2f &) override {}
            void OnColorChange() override {}
            void ChangeColors();

            // Required by IGuiEntity
        public:
            bool GetEntityWillDraw() const override { return entityWillDraw_; }
            void SetEntityWillDraw(const bool WILL_DRAW) override { entityWillDraw_ = WILL_DRAW; }

            const sf::Vector2f GetEntityPos() const override
            {
                return sf::Vector2f(entityRegion_.left, entityRegion_.top);
            }

            void SetEntityPos(const sf::Vector2f & V) override { SetEntityPos(V.x, V.y); }
            void SetEntityPos(const float POS_LEFT, const float POS_TOP) override;

            void MoveEntityPos(const sf::Vector2f & V) override { MoveEntityPos(V.x, V.y); }
            void MoveEntityPos(const float HORIZ, const float VERT) override;

            void MoveEntityOffScreen();
            void MoveEntityBackFromOffScreen();

            void SetEntityColors(const ColorSet & COLOR_SET) override;
            void SetEntityColorFg(const sf::Color & FG_COLOR) override;
            void SetEntityColorFgBoth(const sf::Color & FG_COLOR) override;
            const ColorSet GetEntityColors() const override { return entityColorSet_; }
            const sf::Color GetEntityColorForeground() const override { return entityFgColor_; }
            const sf::Color GetEntityColorBackground() const override { return entityBgColor_; }
            void FakeColorSetAsIfFocusIs(const bool) override;

            const std::string GetEntityName() const override { return entityName_; }

            const sf::FloatRect GetEntityRegion() const override { return entityRegion_; }

            void SetEntityRegion(const sf::FloatRect & REGION) override { entityRegion_ = REGION; }

            bool UpdateTime(const float) override { return false; }

            bool KeyPress(const sf::Event::KeyEvent &) override { return false; }
            bool KeyRelease(const sf::Event::KeyEvent &) override { return false; }

            friend bool operator<(const GuiEntity & L, const GuiEntity & R);
            friend bool operator==(const GuiEntity & L, const GuiEntity & R);

        protected:
            static const sf::Int32 DOUBLE_CLICK_TIME_MS_;
            //
            std::string entityName_;
            sf::FloatRect entityRegion_;
            MouseState::Enum entityMouseState_;
            ColorSet entityColorSet_;
            sf::Color entityFgColor_;
            sf::Color entityBgColor_;
            bool entityHasFocus_;
            bool entityWillFocus_;
            bool entityWillDraw_;
            std::string entityMouseHoverText_;
            std::unique_ptr<sf::Clock> entityClockUPtr_;
            sf::Vector2f entityPrevPos_;
        };

        bool operator<(const GuiEntity & L, const GuiEntity & R);

        bool operator==(const GuiEntity & L, const GuiEntity & R);

        inline bool operator!=(const GuiEntity & L, const GuiEntity & R) { return !(L == R); }
    } // namespace gui
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_GUIENTITY_HPP_INCLUDED
