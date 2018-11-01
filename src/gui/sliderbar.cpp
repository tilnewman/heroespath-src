// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// sliderbar.cpp
//
#include "sliderbar.hpp"

#include "gui/gui-images.hpp"
#include "gui/sound-manager.hpp"
#include "misc/assertlogandthrow.hpp"
#include "sfutil/position.hpp"
#include "sfutil/size-and-scale.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

namespace heroespath
{
namespace gui
{

    const float SliderBar::POS_OFFSET_HORIZ_(18.0f);
    const float SliderBar::POS_OFFSET_VERT_(5.0f);
    const float SliderBar::MOVE_AMOUNT_RATIO_(0.1f);

    SliderBar::SliderBar(
        const std::string & NAME,
        const float POS_LEFT,
        const float POS_TOP,
        const float LENGTH,
        const SliderStyle & STYLE,
        const Callback_t::IHandlerPtrOpt_t & CALLBACK_HANDLER_PTR_OPT,
        const float INITIAL_POS_RATIO,
        const bool WILL_INVERT_POSITION)
        : Entity(std::string(NAME).append("_SliderBar"), POS_LEFT, POS_TOP)
        , currentPosRatio_(INITIAL_POS_RATIO)
        , length_(LENGTH)
        , style_(STYLE)
        , guiElementsCachedTexture_(GuiImages::PathKey())
        , botOrLeftSprite_(
              guiElementsCachedTexture_.Get(), TextureCoordsBaseOnStyle_BottomOrLeft(STYLE))
        , topOrRightSprite_(
              guiElementsCachedTexture_.Get(), TextureCoordsBaseOnStyle_TopOrRight(STYLE))
        , barSprite_(guiElementsCachedTexture_.Get(), TextureCoordsBasedOnStyle_Bar(STYLE))
        , padSprite_(guiElementsCachedTexture_.Get(), TextureCoordsBasedOnStyle_Pad(STYLE))
        , callbackHandlerPtrOpt_(CALLBACK_HANDLER_PTR_OPT)
        , botOrLeftIsMouseDown_(false)
        , topOrRightIsMouseDown_(false)
        , barIsMouseDown_(false)
        , padIsMouseDown_(false)
        , willInvertPosition_(WILL_INVERT_POSITION)
    {
        if (style_.orientation == Orientation::Horiz)
        {
            botOrLeftSprite_.setPosition(POS_LEFT, POS_TOP);

            topOrRightSprite_.setPosition(
                POS_LEFT + (length_ - topOrRightSprite_.getLocalBounds().width), POS_TOP);

            barSprite_.setPosition(
                POS_LEFT + (botOrLeftSprite_.getLocalBounds().width * 0.5f),
                POS_TOP + (botOrLeftSprite_.getLocalBounds().height * 0.5f)
                    - (barSprite_.getGlobalBounds().height * 0.5f));

            barSprite_.setScale(
                (length_ - botOrLeftSprite_.getLocalBounds().width)
                    / barSprite_.getLocalBounds().width,
                1.0f);
        }
        else
        {
            // bar sprites are only horizontal so rotate if orientation is vertical
            barSprite_.rotate(270.0f);

            botOrLeftSprite_.setPosition(
                POS_LEFT, POS_TOP + (length_ - botOrLeftSprite_.getLocalBounds().height));

            topOrRightSprite_.setPosition(POS_LEFT, POS_TOP);

            barSprite_.setPosition(
                (POS_LEFT + (topOrRightSprite_.getGlobalBounds().width * 0.5f))
                    - (barSprite_.getGlobalBounds().width * 0.5f),
                (POS_TOP + (topOrRightSprite_.getGlobalBounds().height * 0.5f)) + length_);

            barSprite_.setScale(
                (length_ - topOrRightSprite_.getLocalBounds().height)
                    / barSprite_.getLocalBounds().width,
                1.0f);
        }

        SetEntityRegion(sfutil::MinimallyEnclosing(
            { botOrLeftSprite_, topOrRightSprite_, barSprite_, padSprite_ }));

        UpdatePadPosition();
    }

    void SliderBar::PositionRatio(const float NEW_VAL)
    {
        currentPosRatio_ = NEW_VAL;
        UpdatePadPosition();
    }

    float SliderBar::CalcPositionRatioChangeOnMouseUp(const sf::Vector2f & MOUSE_POS_V) const
    {
        if (willInvertPosition_)
        {
            return -1.0f * CalcPositionRatioChangeOnMouseUp_Inner(MOUSE_POS_V);
        }
        else
        {
            return CalcPositionRatioChangeOnMouseUp_Inner(MOUSE_POS_V);
        }
    }

    float SliderBar::CalcPositionRatioFromScreenPos(const sf::Vector2f & SCREEN_POS_V) const
    {
        if (willInvertPosition_)
        {
            return 1.0f - CalcPositionRatioFromScreenPos_Inner(SCREEN_POS_V);
        }
        else
        {
            return CalcPositionRatioFromScreenPos_Inner(SCREEN_POS_V);
        }
    }

    void SliderBar::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        target.draw(barSprite_, states);
        target.draw(padSprite_, states);
        target.draw(botOrLeftSprite_, states);
        target.draw(topOrRightSprite_, states);
    }

    bool SliderBar::UpdateMouseWheel(const sf::Vector2f & MOUSE_POS_V, const float WHEEL_MOTION)
    {
        if (entityRegion_.contains(MOUSE_POS_V))
        {
            if (WHEEL_MOTION < 0.0f)
            {
                currentPosRatio_ -= 0.02f;
            }
            else
            {
                currentPosRatio_ += 0.02f;
            }

            UpdatePadPosition();
            return true;
        }
        else
        {
            return false;
        }
    }

    bool SliderBar::MouseDown(const sf::Vector2f & MOUSE_POS_V)
    {
        ResetMouseDownStatus();

        // important to check the pad and end-points before the bar
        if (padSprite_.getGlobalBounds().contains(MOUSE_POS_V))
        {
            padIsMouseDown_ = true;
        }
        else if (botOrLeftSprite_.getGlobalBounds().contains(MOUSE_POS_V))
        {
            botOrLeftIsMouseDown_ = true;
        }
        else if (topOrRightSprite_.getGlobalBounds().contains(MOUSE_POS_V))
        {
            topOrRightIsMouseDown_ = true;
        }
        else if (barSprite_.getGlobalBounds().contains(MOUSE_POS_V))
        {
            barIsMouseDown_ = true;
        }

        if (botOrLeftIsMouseDown_ || topOrRightIsMouseDown_ || barIsMouseDown_ || padIsMouseDown_)
        {
            SetMouseState(MouseState::Down);
            return true;
        }
        else
        {
            SetMouseState(MouseState::Up);
            return false;
        }
    }

    bool SliderBar::MouseUp(const sf::Vector2f & MOUSE_POS_V)
    {
        const auto POS_CHANGE_RATIO { CalcPositionRatioChangeOnMouseUp(MOUSE_POS_V) };

        ResetMouseDownStatus();

        if ((POS_CHANGE_RATIO < 0.0f) || (POS_CHANGE_RATIO > 0.0f))
        {
            UpdatePadPosition();
            return true;
        }
        else
        {
            return false;
        }
    }

    bool SliderBar::UpdateMousePos(const sf::Vector2f & MOUSE_POS_V)
    {
        if (padIsMouseDown_)
        {
            currentPosRatio_ = CalcPositionRatioFromScreenPos(MOUSE_POS_V);
            UpdatePadPosition();
            return true;
        }
        else
        {
            return false;
        }
    }

    void SliderBar::SetEntityPos(const float POS_LEFT, const float POS_TOP)
    {
        const auto DIFF_V { sf::Vector2f(POS_LEFT, POS_TOP) - GetEntityPos() };
        MoveEntityPos(DIFF_V.x, DIFF_V.y);
    }

    void SliderBar::MoveEntityPos(const float HORIZ, const float VERT)
    {
        entityRegion_.left += HORIZ;
        entityRegion_.top += VERT;

        botOrLeftSprite_.move(HORIZ, VERT);
        topOrRightSprite_.move(HORIZ, VERT);
        barSprite_.move(HORIZ, VERT);
        padSprite_.move(HORIZ, VERT);
    }

    void SliderBar::OnChange(const float)
    {
        if (callbackHandlerPtrOpt_)
        {
            callbackHandlerPtrOpt_.value()->HandleCallback(this);
        }
    }

    const sf::IntRect
        SliderBar::TextureCoordsBaseOnStyle_BottomOrLeft(const SliderStyle STYLE) const
    {
        if (style_.orientation == Orientation::Horiz)
        {
            return TextureCoordsBaseOnStyle_BottomOrLeft_Horizontal(STYLE);
        }
        else
        {
            return TextureCoordsBaseOnStyle_BottomOrLeft_Vertical(STYLE);
        }
    }

    const sf::IntRect SliderBar::TextureCoordsBaseOnStyle_TopOrRight(const SliderStyle STYLE) const
    {
        if (style_.orientation == Orientation::Horiz)
        {
            return TextureCoordsBaseOnStyle_TopOrRight_Horizontal(STYLE);
        }
        else
        {
            return TextureCoordsBaseOnStyle_TopOrRight_Vertical(STYLE);
        }
    }

    const sf::IntRect
        SliderBar::TextureCoordsBaseOnStyle_BottomOrLeft_Horizontal(const SliderStyle STYLE) const
    {
        switch (STYLE.brightness)
        {
            case Brightness::Medium:
            {
                if (STYLE.will_label_arrows)
                {
                    return GuiImages::ArrowMinusLeftMedI();
                }
                else
                {
                    return GuiImages::ArrowLeftMedI();
                }
            }
            case Brightness::Dark:
            {
                if (STYLE.will_label_arrows)
                {
                    return GuiImages::ArrowMinusLeftDarkI();
                }
                else
                {
                    return GuiImages::ArrowLeftDarkI();
                }
            }

            case Brightness::Bright:
            case Brightness::Count:
            default:
            {
                if (STYLE.will_label_arrows)
                {
                    return GuiImages::ArrowMinusLeftBrightI();
                }
                else
                {
                    return GuiImages::ArrowLeftBrightI();
                }
            }
        }
    }

    const sf::IntRect
        SliderBar::TextureCoordsBaseOnStyle_BottomOrLeft_Vertical(const SliderStyle STYLE) const
    {
        switch (STYLE.brightness)
        {
            case Brightness::Medium:
            {
                if (STYLE.will_label_arrows)
                {
                    return GuiImages::ArrowMinusDownMedI();
                }
                else
                {
                    return GuiImages::ArrowDownMedI();
                }
            }
            case Brightness::Dark:
            {
                if (STYLE.will_label_arrows)
                {
                    return GuiImages::ArrowMinusDownDarkI();
                }
                else
                {
                    return GuiImages::ArrowDownDarkI();
                }
            }

            case Brightness::Bright:
            case Brightness::Count:
            default:
            {
                if (STYLE.will_label_arrows)
                {
                    return GuiImages::ArrowMinusDownBrightI();
                }
                else
                {
                    return GuiImages::ArrowDownBrightI();
                }
            }
        }
    }

    const sf::IntRect
        SliderBar::TextureCoordsBaseOnStyle_TopOrRight_Horizontal(const SliderStyle STYLE) const
    {
        switch (STYLE.brightness)
        {
            case Brightness::Medium:
            {
                if (STYLE.will_label_arrows)
                {
                    return GuiImages::ArrowPlusRightMedI();
                }
                else
                {
                    return GuiImages::ArrowRightMedI();
                }
            }
            case Brightness::Dark:
            {
                if (STYLE.will_label_arrows)
                {
                    return GuiImages::ArrowPlusRightDarkI();
                }
                else
                {
                    return GuiImages::ArrowRightDarkI();
                }
            }

            case Brightness::Bright:
            case Brightness::Count:
            default:
            {
                if (STYLE.will_label_arrows)
                {
                    return GuiImages::ArrowPlusRightBrightI();
                }
                else
                {
                    return GuiImages::ArrowRightBrightI();
                }
            }
        }
    }

    const sf::IntRect
        SliderBar::TextureCoordsBaseOnStyle_TopOrRight_Vertical(const SliderStyle STYLE) const
    {
        switch (STYLE.brightness)
        {
            case Brightness::Medium:
            {
                if (STYLE.will_label_arrows)
                {
                    return GuiImages::ArrowPlusUpMedI();
                }
                else
                {
                    return GuiImages::ArrowUpMedI();
                }
            }

            case Brightness::Dark:
            {
                if (STYLE.will_label_arrows)
                {
                    return GuiImages::ArrowPlusUpDarkI();
                }
                else
                {
                    return GuiImages::ArrowUpDarkI();
                }
            }

            case Brightness::Bright:
            case Brightness::Count:
            default:
            {
                if (STYLE.will_label_arrows)
                {
                    return GuiImages::ArrowPlusUpBrightI();
                }
                else
                {
                    return GuiImages::ArrowUpBrightI();
                }
            }
        }
    }

    const sf::IntRect SliderBar::TextureCoordsBasedOnStyle_Bar(const SliderStyle STYLE) const
    {
        if (STYLE.is_line_large)
        {
            return GuiImages::HorizontalLineLargeI();
        }
        else
        {
            return GuiImages::HorizontalLineSmallI();
        }
    }

    const sf::IntRect SliderBar::TextureCoordsBasedOnStyle_Pad(const SliderStyle STYLE) const
    {
        if (STYLE.orientation == Orientation::Horiz)
        {
            return GuiImages::PadHorizontalLargeI();
        }
        else
        {
            return GuiImages::PadVerticalLargeI();
        }
    }

    void SliderBar::UpdatePadPosition()
    {
        if (currentPosRatio_ > 1.0f)
        {
            currentPosRatio_ = 1.0f;
        }
        else if (currentPosRatio_ < 0.0f)
        {
            currentPosRatio_ = 0.0f;
        }

        padSprite_.setPosition(CalcPadPosition());
        OnChange(currentPosRatio_);
    }

    const sf::Vector2f SliderBar::CalcPadPosition() const
    {
        auto currentPosRatioToUse { currentPosRatio_ };

        if (willInvertPosition_)
        {
            currentPosRatioToUse = 1.0f - currentPosRatioToUse;
        }

        const sf::Vector2f POS_V(GetEntityPos());

        if (style_.orientation == Orientation::Horiz)
        {
            const auto SLIDER_RANGE { length_
                                      - (botOrLeftSprite_.getLocalBounds().width
                                         + topOrRightSprite_.getLocalBounds().width
                                         + padSprite_.getLocalBounds().width - 8.0f) };

            // magic number 3 moves passed the shadow
            const auto PAD_POS_LEFT { POS_V.x + botOrLeftSprite_.getLocalBounds().width
                                      + (SLIDER_RANGE * currentPosRatioToUse) - 3.0f };

            const auto PAD_POS_TOP { POS_V.y + (botOrLeftSprite_.getLocalBounds().height * 0.5f)
                                     - (padSprite_.getLocalBounds().height * 0.5f) };

            return sf::Vector2f(PAD_POS_LEFT, PAD_POS_TOP);
        }
        else
        {
            const auto SLIDER_RANGE { length_
                                      - (topOrRightSprite_.getLocalBounds().height
                                         + botOrLeftSprite_.getLocalBounds().height
                                         + padSprite_.getLocalBounds().height - 5.0f) };

            // magic number 1 moves passed the shadow
            const auto PAD_POS_TOP { POS_V.y + topOrRightSprite_.getLocalBounds().height
                                     + (SLIDER_RANGE * currentPosRatioToUse) - 1.0f };

            const auto PAD_POS_LEFT { POS_V.x + (topOrRightSprite_.getLocalBounds().width * 0.5f)
                                      - (padSprite_.getLocalBounds().width * 0.5f) };

            return sf::Vector2f(PAD_POS_LEFT, PAD_POS_TOP);
        }
    }

    void SliderBar::ResetMouseDownStatus()
    {
        botOrLeftIsMouseDown_ = false;
        topOrRightIsMouseDown_ = false;
        barIsMouseDown_ = false;
        padIsMouseDown_ = false;
    }

    float SliderBar::CalcPositionRatioChangeOnMouseUp_Inner(const sf::Vector2f & MOUSE_POS_V) const
    {
        if (botOrLeftIsMouseDown_)
        {
            if (style_.orientation == Orientation::Horiz)
            {
                return -MOVE_AMOUNT_RATIO_;
            }
            else
            {
                return MOVE_AMOUNT_RATIO_;
            }
        }
        else if (topOrRightIsMouseDown_)
        {
            if (style_.orientation == Orientation::Horiz)
            {
                return MOVE_AMOUNT_RATIO_;
            }
            else
            {
                return -MOVE_AMOUNT_RATIO_;
            }
        }
        else if (barIsMouseDown_)
        {
            if (style_.orientation == Orientation::Horiz)
            {
                if (MOUSE_POS_V.x < padSprite_.getGlobalBounds().left)
                {
                    return -MOVE_AMOUNT_RATIO_;
                }
                else if (MOUSE_POS_V.x > sfutil::Right(padSprite_))
                {
                    return MOVE_AMOUNT_RATIO_;
                }
            }
            else
            {
                if (MOUSE_POS_V.y < padSprite_.getGlobalBounds().top)
                {
                    return -MOVE_AMOUNT_RATIO_;
                }
                else if (MOUSE_POS_V.y > sfutil::Bottom(padSprite_))
                {
                    return MOVE_AMOUNT_RATIO_;
                }
            }
        }

        return 0.0f;
    }

    float SliderBar::CalcPositionRatioFromScreenPos_Inner(const sf::Vector2f & SCREEN_POS_V) const
    {
        const auto POS_V(GetEntityPos());

        // change from screen to slider coords
        if (style_.orientation == Orientation::Horiz)
        {
            const auto MIN_SCREENX { POS_V.x + botOrLeftSprite_.getLocalBounds().width };

            const auto MAX_SCREENX { (
                POS_V.x + (length_ - topOrRightSprite_.getLocalBounds().width)
                - barSprite_.getLocalBounds().height) };

            if (SCREEN_POS_V.x < MIN_SCREENX)
            {
                return 0.0f;
            }
            else
            {
                if (SCREEN_POS_V.x > MAX_SCREENX)
                {
                    return 1.0f;
                }
                else
                {
                    return (SCREEN_POS_V.x - MIN_SCREENX) / (MAX_SCREENX - MIN_SCREENX);
                }
            }
        }
        else
        {
            const auto MIN_SCREENY { POS_V.y + topOrRightSprite_.getLocalBounds().height };

            const auto MAX_SCREENY { (
                POS_V.y + (length_ - botOrLeftSprite_.getLocalBounds().height)
                - barSprite_.getLocalBounds().height) };

            if (SCREEN_POS_V.y < MIN_SCREENY)
            {
                return 0.0f;
            }
            else
            {
                if (SCREEN_POS_V.y > MAX_SCREENY)
                {
                    return 1.0f;
                }
                else
                {
                    return (SCREEN_POS_V.y - MIN_SCREENY) / (MAX_SCREENY - MIN_SCREENY);
                }
            }
        }
    }

} // namespace gui
} // namespace heroespath
