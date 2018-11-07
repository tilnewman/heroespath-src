// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_ENTITY_HPP_INCLUDED
#define HEROESPATH_GUI_ENTITY_HPP_INCLUDED
//
// entity.hpp
//
#include "gui/color-set.hpp"
#include "gui/i-entity.hpp"
#include "gui/mouse-state-enum.hpp"
#include "sfutil/vector-and-rect.hpp"

#include <SFML/System/Clock.hpp>

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace gui
{

    // Responsible for the common state and operations of a "Gui Entity", which is anything
    // drawn on screen that the player might interact with.  Examples include: images that
    // change with mouse-clicks or keypresses, buttons, borders, radio-buttons, check-boxes,
    // something with a mouse-over popup, and more.  All Entity's track their position and size
    // on screen, track and respond to the mouse being over them or not,and have a name so that
    // player gui interactions can be tracked in the log.  Stage's keep track of all Entity's
    // and pass mouse and keyboard interactions to whichever is currently in focus.
    class Entity : public IEntity
    {
    public:
        Entity(const Entity &) = delete;
        Entity(Entity &&) = delete;
        Entity & operator=(const Entity &) = delete;
        Entity & operator=(Entity &&) = delete;

        Entity(
            const std::string & NAME,
            const sf::FloatRect & SCREEN_REGION,
            const FocusColors & COLORS = FocusColors());

        Entity(
            const std::string & NAME,
            const sf::Vector2f & SCREEN_POS_V,
            const FocusColors & COLORS = FocusColors());

        Entity(
            const std::string & NAME,
            const float POS_LEFT,
            const float POS_TOP,
            const FocusColors & COLORS = FocusColors());

        virtual ~Entity();

        void draw(sf::RenderTarget &, sf::RenderStates) const override = 0;

        MouseState::Enum GetMouseState() const override { return entityMouseState_; }

        bool SetMouseState(const MouseState::Enum ENUM) override;

        bool MouseUp(const sf::Vector2f & MOUSE_POS_V) override;
        bool MouseDown(const sf::Vector2f & MOUSE_POS_V) override;

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
        void OnClick(const sf::Vector2f &) override {}
        void OnDoubleClick(const sf::Vector2f &) override {}
        void OnColorChange() override {}
        void ChangeColors();

        // IEntity
    public:
        const sf::Vector2f GetEntityPos() const override
        {
            return sf::Vector2f(entityRegion_.left, entityRegion_.top);
        }

        // only calls SetEntityPos(float, float), don't override
        void SetEntityPos(const sf::Vector2f & V) override { SetEntityPos(V.x, V.y); }

        // sets entityRegion_.left and entityRegion_.top, override this one
        void SetEntityPos(const float POS_LEFT, const float POS_TOP) override;

        // only calls MoveEntityPos(float, float), don't override
        void MoveEntityPos(const sf::Vector2f & V) override { MoveEntityPos(V.x, V.y); }

        // entityRegion_.left/top += HORIZ/VERT, override this one
        void MoveEntityPos(const float HORIZ, const float VERT) override;

        void MoveEntityOffScreen();
        void MoveEntityBackFromOffScreen();

        void SetEntityColors(const FocusColors & COLOR_SET) override;
        void SetEntityColorFg(const sf::Color & FG_COLOR) override;
        void SetEntityColorFgBoth(const sf::Color & FG_COLOR) override;
        const FocusColors GetEntityColors() const override { return entityFocusColors_; }

        const sf::Color GetEntityColorForeground() const override { return entityFgColor_; }

        const sf::Color GetEntityColorBackground() const override { return entityBgColor_; }

        void FakeFocusColorsAsIfFocusIs(const bool) override;

        const std::string GetEntityName() const override { return entityName_; }

        const sf::FloatRect GetEntityRegion() const override { return entityRegion_; }

        void SetEntityRegion(const sf::FloatRect & REGION) override { entityRegion_ = REGION; }

        bool UpdateTime(const float) override { return false; }

        bool KeyPress(const sf::Event::KeyEvent &) override { return false; }
        bool KeyRelease(const sf::Event::KeyEvent &) override { return false; }

        friend bool operator<(const Entity & L, const Entity & R);
        friend bool operator==(const Entity & L, const Entity & R);

    protected:
        static const sf::Int32 DOUBLE_CLICK_TIME_MS_;
        //
        std::string entityName_;
        sf::FloatRect entityRegion_;
        MouseState::Enum entityMouseState_;
        FocusColors entityFocusColors_;
        sf::Color entityFgColor_;
        sf::Color entityBgColor_;
        bool entityHasFocus_;
        bool entityWillFocus_;
        std::string entityMouseHoverText_;
        sf::Clock entityClock_;
        sf::Vector2f entityPrevPos_;
        bool hasClockStarted_;
    };

    bool operator<(const Entity & L, const Entity & R);

    bool operator==(const Entity & L, const Entity & R);

    inline bool operator!=(const Entity & L, const Entity & R) { return !(L == R); }

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_ENTITY_HPP_INCLUDED
