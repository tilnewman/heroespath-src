// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_I_ENTITY_HPP_INCLUDED
#define HEROESPATH_GUI_I_ENTITY_HPP_INCLUDED
//
// i-entity.hpp
//
#include "gui/color-set.hpp"
#include "gui/i-clickable.hpp"
#include "gui/mouse-state-enum.hpp"
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>

#include <string>
#include <vector>

namespace heroespath
{
namespace gui
{

    // Responsible for defining a common interface for all gui Entities.
    class IEntity
        : public sf::Drawable
        , public IClickable
    {
    public:
        IEntity() = default;
        virtual ~IEntity() = default;

        virtual const sf::Vector2f GetEntityPos() const = 0;
        virtual void SetEntityPos(const sf::Vector2f &) = 0;
        virtual void SetEntityPos(const float LEFT, const float TOP) = 0;
        virtual void MoveEntityPos(const sf::Vector2f &) = 0;
        virtual void MoveEntityPos(const float HORIZ, const float VERT) = 0;

        virtual const std::string GetEntityName() const = 0;

        virtual const sf::FloatRect GetEntityRegion() const = 0;
        virtual void SetEntityRegion(const sf::FloatRect & R) = 0;

        virtual bool Contains(const sf::Vector2f &) const = 0;
        virtual bool Contains(const float LEFT, const float TOP) const = 0;
        virtual bool Contains(const sf::FloatRect &) const = 0;
        virtual bool Intersects(const sf::FloatRect &) const = 0;

        virtual void SetEntityColors(const FocusColors & COLOR_SET) = 0;
        virtual void SetEntityColorFg(const sf::Color & FG_COLOR) = 0;
        virtual void SetEntityColorFgBoth(const sf::Color & FG_COLOR) = 0;
        virtual const FocusColors GetEntityColors() const = 0;
        virtual const sf::Color GetEntityColorForeground() const = 0;
        virtual const sf::Color GetEntityColorBackground() const = 0;

        virtual bool KeyPress(const sf::Event::KeyEvent &) = 0;
        virtual bool KeyRelease(const sf::Event::KeyEvent &) = 0;

        virtual bool UpdateTime(const float ELAPSED_TIME_SEC) = 0;

    protected:
        virtual void OnColorChange() = 0;

    public:
        void draw(sf::RenderTarget & target, sf::RenderStates states) const override = 0;

        // IClickable
        MouseState::Enum GetMouseState() const override = 0;
        bool SetMouseState(const MouseState::Enum) override = 0;
        bool MouseUp(const sf::Vector2f & MOUSE_POS_V) override = 0;
        bool MouseDown(const sf::Vector2f & MOUSE_POS_V) override = 0;
        bool UpdateMousePos(const sf::Vector2f & MOUSE_POS_V) override = 0;

        bool UpdateMouseWheel(
            const sf::Vector2f & MOUSE_POS_V, const float WHEEL_MOTION) override = 0;

        bool HasFocus() const override = 0;
        bool SetHasFocus(const bool) override = 0;
        bool WillAcceptFocus() const override = 0;
        void SetWillAcceptFocus(const bool) override = 0;
        void FakeFocusColorsAsIfFocusIs(const bool) override = 0;
        const std::string GetMouseHoverText() override = 0;
        void SetMouseHoverText(const std::string &) override = 0;

    protected:
        void OnClick(const sf::Vector2f &) override = 0;
        void OnDoubleClick(const sf::Vector2f &) override = 0;
    };

    using IEntityPtr_t = misc::NotNull<IEntity *>;
    using IEntityPtrOpt_t = boost::optional<IEntityPtr_t>;
    using IEntityPVec_t = std::vector<IEntityPtr_t>;
    using IEntityUPtr_t = std::unique_ptr<IEntity>;

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_I_ENTITY_HPP_INCLUDED
