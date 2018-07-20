// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_GUI_IGUIENTITY_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_GUI_IGUIENTITY_HPP_INCLUDED
//
// i-gui-entity.hpp
//  Common interface for all gui elements
//
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"
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
            virtual void SetEntityPos(const sf::Vector2f &) = 0;
            virtual void SetEntityPos(const float LEFT, const float TOP) = 0;
            virtual void MoveEntityPos(const sf::Vector2f &) = 0;
            virtual void MoveEntityPos(const float HORIZ, const float VERT) = 0;

            virtual const std::string GetEntityName() const = 0;

            virtual const sf::FloatRect GetEntityRegion() const = 0;
            virtual void SetEntityRegion(const sf::FloatRect & R) = 0;

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
            void draw(sf::RenderTarget & target, sf::RenderStates states) const override = 0;

            // IClickable
            MouseState::Enum GetMouseState() const override = 0;
            void SetMouseState(const MouseState::Enum) override = 0;
            bool MouseUp(const sf::Vector2f & MOUSE_POS_V) override = 0;
            bool MouseDown(const sf::Vector2f & MOUSE_POS_V) override = 0;
            bool IsHeldDown() const override = 0;
            bool UpdateMousePos(const sf::Vector2f & MOUSE_POS_V) override = 0;
            bool UpdateMouseWheel(
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

        using IGuiEntityPtr_t = misc::NotNull<IGuiEntity *>;
        using IGuiEntityPVec_t = std::vector<IGuiEntityPtr_t>;
        using IGuiEntityPtrOpt_t = boost::optional<IGuiEntityPtr_t>;

    } // namespace gui
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_GUI_IGUIENTITY_HPP_INCLUDED
