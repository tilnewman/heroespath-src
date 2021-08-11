// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// entity.cpp
//
#include "entity.hpp"

#include "gui/display.hpp"

namespace heroespath
{
namespace gui
{

    const sf::Int32 Entity::DOUBLE_CLICK_TIME_MS_(300);

    Entity::Entity(
        const std::string & NAME, const sf::FloatRect & SCREEN_REGION, const FocusColors & COLORS)
        : entityName_(std::string(NAME).append("_Entity"))
        , entityRegion_(SCREEN_REGION)
        , entityMouseState_(MouseState::Up)
        , entityFocusColors_(COLORS)
        , entityFgColor_(COLORS.foreground_without)
        , entityBgColor_(COLORS.background_without)
        , entityHasFocus_(false)
        , entityWillFocus_(true)
        , entityMouseHoverText_("")
        , entityClock_()
        , entityPrevPos_(0.0f, 0.0f)
        , hasClockStarted_(false)
    {}

    Entity::Entity(const std::string & NAME, const sf::Vector2f & POS_V, const FocusColors & COLORS)
        : entityName_(std::string(NAME).append("_Entity"))
        , entityRegion_(sf::FloatRect(POS_V, sf::Vector2f(0.0f, 0.0f)))
        , entityMouseState_(MouseState::Up)
        , entityFocusColors_(COLORS)
        , entityFgColor_(COLORS.foreground_without)
        , entityBgColor_(COLORS.background_without)
        , entityHasFocus_(false)
        , entityWillFocus_(true)
        , entityMouseHoverText_("")
        , entityClock_()
        , entityPrevPos_(0.0f, 0.0f)
        , hasClockStarted_(false)
    {}

    Entity::Entity(
        const std::string & NAME,
        const float POS_LEFT,
        const float POS_TOP,
        const FocusColors & COLORS)
        : entityName_(std::string(NAME).append("_Entity"))
        , entityRegion_(sf::FloatRect(POS_LEFT, POS_TOP, 0.0f, 0.0f))
        , entityMouseState_(MouseState::Up)
        , entityFocusColors_(COLORS)
        , entityFgColor_(COLORS.foreground_without)
        , entityBgColor_(COLORS.background_without)
        , entityHasFocus_(false)
        , entityWillFocus_(true)
        , entityMouseHoverText_("")
        , entityClock_()
        , entityPrevPos_(0.0f, 0.0f)
        , hasClockStarted_(false)
    {}

    Entity::~Entity() = default;

    bool Entity::SetMouseState(const MouseState::Enum NEW_MOUSE_STATE)
    {
        if (NEW_MOUSE_STATE != entityMouseState_)
        {
            entityMouseState_ = NEW_MOUSE_STATE;
            return true;
        }

        return false;
    }

    bool Entity::MouseUp(const sf::Vector2f & MOUSE_POS_V)
    {
        if (MouseState::Down == entityMouseState_)
        {
            if (entityRegion_.contains(MOUSE_POS_V))
            {
                SetMouseState(MouseState::Over);

                bool didDoubleClickAlready(false);

                if (hasClockStarted_)
                {
                    hasClockStarted_ = false;

                    if (entityClock_.getElapsedTime().asMilliseconds() < DOUBLE_CLICK_TIME_MS_)
                    {
                        OnDoubleClick(MOUSE_POS_V);
                        didDoubleClickAlready = true;
                    }
                }
                else
                {
                    hasClockStarted_ = true;
                    entityClock_.restart();
                }

                if (false == didDoubleClickAlready)
                {
                    OnClick(MOUSE_POS_V);
                }
            }
            else
            {
                SetMouseState(MouseState::Up);
                hasClockStarted_ = false;
            }

            return true;
        }

        return false;
    }

    bool Entity::MouseDown(const sf::Vector2f & MOUSE_POS_V)
    {
        if (entityRegion_.contains(MOUSE_POS_V) && (MouseState::Disabled != entityMouseState_))
        {
            if (entityMouseState_ != MouseState::Down)
            {
                SetMouseState(MouseState::Down);
            }
            return true;
        }
        else
        {
            return false;
        }
    }

    bool Entity::UpdateMousePos(const sf::Vector2f & MOUSE_POS_V)
    {
        if (MouseState::Disabled != entityMouseState_)
        {
            if (entityRegion_.contains(MOUSE_POS_V))
            {
                if (MouseState::Up == entityMouseState_)
                {
                    SetMouseState(MouseState::Over);
                    return true;
                }
            }
            else
            {
                if (MouseState::Over == entityMouseState_)
                {
                    SetMouseState(MouseState::Up);
                    return true;
                }
            }
        }

        return false;
    }

    bool Entity::SetHasFocus(const bool HAS_FOCUS)
    {
        if ((false == entityWillFocus_) || (MouseState::Disabled == entityMouseState_))
        {
            return false;
        }

        const bool DID_FOCUS_CHANGE(entityHasFocus_ != HAS_FOCUS);
        entityHasFocus_ = HAS_FOCUS;

        if (DID_FOCUS_CHANGE)
        {
            ChangeColors();
        }

        return DID_FOCUS_CHANGE;
    }

    void Entity::SetEntityPos(const float POS_LEFT, const float POS_TOP)
    {
        entityRegion_.left = POS_LEFT;
        entityRegion_.top = POS_TOP;
    }

    void Entity::MoveEntityPos(const float HORIZ, const float VERT)
    {
        entityRegion_.left += HORIZ;
        entityRegion_.top += VERT;
    }

    void Entity::MoveEntityOffScreen()
    {
        entityPrevPos_ = GetEntityPos();

        SetEntityPos(
            gui::Display::Instance()->GetWinWidth() + 1.0f,
            gui::Display::Instance()->GetWinHeight() + 1.0f);
    }

    void Entity::MoveEntityBackFromOffScreen()
    {
        entityRegion_.left = entityPrevPos_.x;
        entityRegion_.top = entityPrevPos_.y;
    }

    void Entity::SetEntityColors(const FocusColors & COLOR_SET)
    {
        entityFocusColors_ = COLOR_SET;
        ChangeColors();
    }

    void Entity::SetEntityColorFg(const sf::Color & FG_COLOR)
    {
        entityFocusColors_.foreground_with = FG_COLOR;
        ChangeColors();
    }

    void Entity::SetEntityColorFgBoth(const sf::Color & FG_COLOR)
    {
        entityFocusColors_.foreground_with = FG_COLOR;
        entityFocusColors_.foreground_without = FG_COLOR;
        ChangeColors();
    }

    void Entity::ChangeColors()
    {
        if (entityHasFocus_)
        {
            entityFgColor_ = entityFocusColors_.foreground_with;
            entityBgColor_ = entityFocusColors_.background_with;
        }
        else
        {
            entityFgColor_ = entityFocusColors_.foreground_without;
            entityBgColor_ = entityFocusColors_.background_without;
        }

        OnColorChange();
    }

    void Entity::FakeFocusColorsAsIfFocusIs(const bool FAKED_FOCUS)
    {
        const bool WILL_CHANGE(entityHasFocus_ != FAKED_FOCUS);

        if (WILL_CHANGE)
        {
            entityHasFocus_ = FAKED_FOCUS;
        }

        ChangeColors();

        if (WILL_CHANGE)
        {
            entityHasFocus_ = !FAKED_FOCUS;
        }
    }

    bool operator<(const Entity & L, const Entity & R)
    {
        // intentionally ignore the clock and it's bool flag
        return std::tie(
                   L.entityName_,
                   L.entityMouseState_,
                   L.entityHasFocus_,
                   L.entityWillFocus_,
                   L.entityMouseHoverText_)
            < std::tie(
                   R.entityName_,
                   R.entityMouseState_,
                   R.entityHasFocus_,
                   R.entityWillFocus_,
                   R.entityMouseHoverText_);
    }

    bool operator==(const Entity & L, const Entity & R)
    {
        // intentionally ignore the clock and it's bool flag
        return std::tie(
                   L.entityName_,
                   L.entityRegion_,
                   L.entityMouseState_,
                   L.entityFocusColors_,
                   L.entityFgColor_,
                   L.entityBgColor_,
                   L.entityHasFocus_,
                   L.entityWillFocus_,
                   L.entityMouseHoverText_,
                   L.entityPrevPos_)
            == std::tie(
                   R.entityName_,
                   R.entityRegion_,
                   R.entityMouseState_,
                   R.entityFocusColors_,
                   R.entityFgColor_,
                   R.entityBgColor_,
                   R.entityHasFocus_,
                   R.entityWillFocus_,
                   R.entityMouseHoverText_,
                   R.entityPrevPos_);
    }

} // namespace gui
} // namespace heroespath
