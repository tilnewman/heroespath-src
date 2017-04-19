//
// gui-entity.cpp
//
#include "gui-entity.hpp"

#include "sfml-util/sfml-util.hpp"
#include "sfml-util/display.hpp"


namespace sfml_util
{
namespace gui
{

    const sf::Int32 GuiEntity::DOUBLE_CLICK_TIME_MS_(300);


    GuiEntity::GuiEntity(const std::string &   NAME,
                         const sf::FloatRect & SCREEN_REGION,
                         const ColorSet &      COLORS)
    :
        entityName_          (std::string(NAME).append("_GuiEntity")),
        entityRegion_        (SCREEN_REGION),
        entityMouseState_    (MouseState::Up),
        entityColorSet_      (COLORS),
        entityFgColor_       (COLORS.foreground_without_focus),
        entityBgColor_       (COLORS.background_without_focus),
        entityHasFocus_      (false),
        entityWillFocus_     (true),
        entityWillDraw_      (true),
        entityMouseHoverText_(""),
        entityClockSPtr_     (),
        entityPrevPos_       (0.0f, 0.0f)
    {}


    GuiEntity::GuiEntity(const std::string & NAME,
                         const float         POS_LEFT,
                         const float         POS_TOP,
                         const ColorSet &    COLORS)
    :
        entityName_          (std::string(NAME).append("_GuiEntity")),
        entityRegion_        (sf::FloatRect(POS_LEFT, POS_TOP, 0.0f, 0.0f)),
        entityMouseState_    (MouseState::Up),
        entityColorSet_      (COLORS),
        entityFgColor_       (COLORS.foreground_without_focus),
        entityBgColor_       (COLORS.background_without_focus),
        entityHasFocus_      (false),
        entityWillFocus_     (true),
        entityWillDraw_      (true),
        entityMouseHoverText_(""),
        entityClockSPtr_     (),
        entityPrevPos_       (0.0f, 0.0f)
    {}


    GuiEntity::~GuiEntity()
    {}


    bool GuiEntity::MouseUp(const sf::Vector2f & MOUSE_POS_V)
    {
        if (MouseState::Down == entityMouseState_)
        {
            if (IsInEntityRegion(MOUSE_POS_V))
            {
                entityMouseState_ = MouseState::Over;

                bool didDoubleClickAlready(false);

                if (entityClockSPtr_.get() == nullptr)
                    entityClockSPtr_.reset( new sf::Clock );
                else
                {
                    if (entityClockSPtr_->getElapsedTime().asMilliseconds() < DOUBLE_CLICK_TIME_MS_)
                    {
                        OnDoubleClick(MOUSE_POS_V);
                        didDoubleClickAlready = true;
                    }

                    entityClockSPtr_->restart();
                }

                if (false == didDoubleClickAlready)
                    OnClick(MOUSE_POS_V);
            }
            else
            {
                entityMouseState_ = MouseState::Up;
                entityClockSPtr_.reset();
            }

            return true;
        }

        return false;
    }


    bool GuiEntity::MouseDown(const sf::Vector2f & MOUSE_POS_V)
    {
        if (IsInEntityRegion(MOUSE_POS_V))
        {
            entityMouseState_ = MouseState::Down;
            return true;
        }
        else
            return false;
    }


    bool GuiEntity::UpdateMousePos(const sf::Vector2f & MOUSE_POS_V)
    {
        if (IsInEntityRegion(MOUSE_POS_V))
        {
            if (MouseState::Up == entityMouseState_)
            {
                entityMouseState_ = MouseState::Over;
                return true;
            }
        }
        else
        {
            if (MouseState::Over == entityMouseState_)
            {
                entityMouseState_ = MouseState::Up;
                return true;
            }
        }

        return false;
    }


    bool GuiEntity::SetHasFocus(const bool HAS_FOCUS)
    {
        if (false == entityWillFocus_)
            return false;

        const bool DID_FOCUS_CHANGE(entityHasFocus_ != HAS_FOCUS);
        entityHasFocus_ = HAS_FOCUS;

        if (DID_FOCUS_CHANGE)
            ChangeColors();

        return DID_FOCUS_CHANGE;
    }


    void GuiEntity::SetEntityPos(const float POS_LEFT, const float POS_TOP)
    {
        entityRegion_.left = POS_LEFT;
        entityRegion_.top = POS_TOP;
    }


    void GuiEntity::MoveEntityPos(const float HORIZ, const float VERT)
    {
        entityRegion_.left += HORIZ;
        entityRegion_.top  += VERT;
    }


    void GuiEntity::MoveEntityOffScreen()
    {
        entityPrevPos_ = GetEntityPos();
        SetEntityPos(sfml_util::Display::Instance()->GetWinWidth() + 1.0f, sfml_util::Display::Instance()->GetWinHeight() + 1.0f);
    }


    void GuiEntity::MoveEntityBackFromOffScreen()
    {
        entityRegion_.left = entityPrevPos_.x;
        entityRegion_.top = entityPrevPos_.y;
    }


    void GuiEntity::SetEntityColors(const ColorSet & COLOR_SET)
    {
        entityColorSet_ = COLOR_SET;
        ChangeColors();
    }


    void GuiEntity::SetEntityColorFg(const sf::Color & FG_COLOR)
    {
        entityColorSet_.foreground_with_focus = FG_COLOR;
        ChangeColors();
    }


    void GuiEntity::SetEntityColorFgBoth(const sf::Color & FG_COLOR)
    {
        entityColorSet_.foreground_with_focus = FG_COLOR;
        entityColorSet_.foreground_without_focus = FG_COLOR;
        ChangeColors();
    }


    void GuiEntity::ChangeColors()
    {
        if (entityHasFocus_)
        {
            entityFgColor_ = entityColorSet_.foreground_with_focus;
            entityBgColor_ = entityColorSet_.background_with_focus;
        }
        else
        {
            entityFgColor_ = entityColorSet_.foreground_without_focus;
            entityBgColor_ = entityColorSet_.background_without_focus;
        }

        OnColorChange();
    }


    void GuiEntity::FakeColorSetAsIfFocusIs(const bool FAKED_FOCUS)
    {
        const bool WILL_CHANGE(entityHasFocus_ != FAKED_FOCUS);

        if (WILL_CHANGE)
            entityHasFocus_ = FAKED_FOCUS;

        ChangeColors();

        if (WILL_CHANGE)
            entityHasFocus_ = ! FAKED_FOCUS;
    }


    bool operator<(const GuiEntity & L, const GuiEntity & R)
    {
        if (L.entityRegion_ != R.entityRegion_)
        {
            if (sfml_util::IsRealClose(L.entityRegion_.left, R.entityRegion_.left) == false)
                return (L.entityRegion_.left < R.entityRegion_.left);

            if (sfml_util::IsRealClose(L.entityRegion_.top, R.entityRegion_.top) == false)
                return (L.entityRegion_.top< R.entityRegion_.top);

            if (sfml_util::IsRealClose(L.entityRegion_.width, R.entityRegion_.width) == false)
                return (L.entityRegion_.width< R.entityRegion_.width);

            if (sfml_util::IsRealClose(L.entityRegion_.height, R.entityRegion_.height) == false)
                return (L.entityRegion_.height < R.entityRegion_.height);
        }

        if (L.entityPrevPos_ != R.entityPrevPos_)
        {
            if (sfml_util::IsRealClose(L.entityPrevPos_.x, R.entityPrevPos_.x) == false)
                return L.entityPrevPos_.x < R.entityPrevPos_.x;

            if (sfml_util::IsRealClose(L.entityPrevPos_.y, R.entityPrevPos_.y) == false)
                return L.entityPrevPos_.y < R.entityPrevPos_.y;
        }

        if (L.entityFgColor_ != R.entityFgColor_)
            return sfml_util::ColorLess(L.entityFgColor_, R.entityFgColor_);

        if (L.entityBgColor_ != R.entityBgColor_)
            return sfml_util::ColorLess(L.entityBgColor_, R.entityBgColor_);

        return  std::tie(L.entityName_,
                         L.entityMouseState_,
                         L.entityColorSet_,
                         L.entityHasFocus_,
                         L.entityWillFocus_,
                         L.entityWillDraw_,
                         L.entityMouseHoverText_,
                         L.entityClockSPtr_)
                <
                std::tie(R.entityName_,
                         R.entityMouseState_,
                         R.entityColorSet_,
                         R.entityHasFocus_,
                         R.entityWillFocus_,
                         R.entityWillDraw_,
                         R.entityMouseHoverText_,
                         R.entityClockSPtr_);
    }


    bool operator==(const GuiEntity & L, const GuiEntity & R)
    {
        return  std::tie(L.entityName_,
                         L.entityRegion_,
                         L.entityMouseState_,
                         L.entityColorSet_,
                         L.entityFgColor_,
                         L.entityBgColor_,
                         L.entityHasFocus_,
                         L.entityWillFocus_,
                         L.entityWillDraw_,
                         L.entityMouseHoverText_,
                         L.entityClockSPtr_,
                         L.entityPrevPos_)
                ==
                std::tie(R.entityName_,
                         R.entityRegion_,
                         R.entityMouseState_,
                         R.entityColorSet_,
                         R.entityFgColor_,
                         R.entityBgColor_,
                         R.entityHasFocus_,
                         R.entityWillFocus_,
                         R.entityWillDraw_,
                         R.entityMouseHoverText_,
                         R.entityClockSPtr_,
                         R.entityPrevPos_);
    }

}
}
