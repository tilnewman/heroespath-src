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
        , topOrLeftSprite_(
              guiElementsCachedTexture_.Get(), TextureCoordsBaseOnStyle_TopOrLeft(STYLE))
        , botOrRightSprite_(
              guiElementsCachedTexture_.Get(), TextureCoordsBaseOnStyle_BottomOrRight(STYLE))
        , barSprite_(guiElementsCachedTexture_.Get(), TextureCoordsBasedOnStyle_Bar(STYLE))
        , padSprite_(guiElementsCachedTexture_.Get(), TextureCoordsBasedOnStyle_Pad(STYLE))
        , callbackHandlerPtrOpt_(CALLBACK_HANDLER_PTR_OPT)
        , topOrLeftIsMouseDown_(false)
        , botOrRightIsMouseDown_(false)
        , barIsMouseDown_(false)
        , padIsMouseDown_(false)
        , willInvertPosition_(WILL_INVERT_POSITION)
    {
        SetupInitialSpritePositions(sf::Vector2f(POS_LEFT, POS_TOP));

        SetEntityRegion(sfutil::MinimallyEnclosing(
            { topOrLeftSprite_, botOrRightSprite_, barSprite_, padSprite_ }));
    }

    void SliderBar::PositionRatio(const float NEW_VAL)
    {
        currentPosRatio_ = NEW_VAL;
        UpdatePadPosition();
    }

    void SliderBar::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        target.draw(barSprite_, states);
        target.draw(padSprite_, states);
        target.draw(topOrLeftSprite_, states);
        target.draw(botOrRightSprite_, states);
    }

    // bool SliderBar::UpdateMouseWheel(const sf::Vector2f & MOUSE_POS_V, const float WHEEL_MOTION)
    //{
    //    if (entityRegion_.contains(MOUSE_POS_V))
    //    {
    //        if (WHEEL_MOTION < 0.0f)
    //        {
    //            currentPosRatio_ -= 0.02f;
    //        }
    //        else
    //        {
    //            currentPosRatio_ += 0.02f;
    //        }
    //
    //        UpdatePadPosition();
    //        return true;
    //    }
    //    else
    //    {
    //        return false;
    //    }
    //}

    bool SliderBar::MouseDown(const sf::Vector2f & MOUSE_POS_V)
    {
        ResetMouseDownStatus();

        // check the pad and end-points before the bar because they are on top of the bar
        if (padSprite_.getGlobalBounds().contains(MOUSE_POS_V))
        {
            padIsMouseDown_ = true;
        }
        else if (topOrLeftSprite_.getGlobalBounds().contains(MOUSE_POS_V))
        {
            topOrLeftIsMouseDown_ = true;
        }
        else if (botOrRightSprite_.getGlobalBounds().contains(MOUSE_POS_V))
        {
            botOrRightIsMouseDown_ = true;
        }
        else if (barSprite_.getGlobalBounds().contains(MOUSE_POS_V))
        {
            barIsMouseDown_ = true;
        }

        if (topOrLeftIsMouseDown_ || botOrRightIsMouseDown_ || barIsMouseDown_ || padIsMouseDown_)
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
            currentPosRatio_ += POS_CHANGE_RATIO;
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
        Entity::MoveEntityPos(HORIZ, VERT);

        topOrLeftSprite_.move(HORIZ, VERT);
        botOrRightSprite_.move(HORIZ, VERT);
        barSprite_.move(HORIZ, VERT);
        padSprite_.move(HORIZ, VERT);
    }

    void SliderBar::SetupInitialSpritePositions(const sf::Vector2f POS_V)
    {
        auto calcPadScale = [](const float SIZE_ORIG, const float RANGE) {
            const auto TENTH_RANGE { (RANGE * 0.1f) };

            auto sizeFinal { SIZE_ORIG };

            if (SIZE_ORIG > RANGE)
            {
                sizeFinal = (RANGE * 0.5f);
            }
            else if (SIZE_ORIG < TENTH_RANGE)
            {
                sizeFinal = TENTH_RANGE;
            }

            return (sizeFinal / SIZE_ORIG);
        };

        // the end caps are not placed at either end of the length_ because that would grow the
        // length_, instead they are placed on-top-of or within the length_, which preserves the
        // total lenght_ on screen but reduces the range over which the pad can move, see
        // CalcPadPosition() for the code that assumes/handles this
        if (style_.orientation == Orientation::Horiz)
        {
            topOrLeftSprite_.setPosition(POS_V);

            botOrRightSprite_.setPosition(
                POS_V.x + (length_ - sfutil::Width(botOrRightSprite_)), POS_V.y);

            const auto BAR_RANGE {
                (sfutil::CenterOfHoriz(botOrRightSprite_) - sfutil::CenterOfHoriz(topOrLeftSprite_))
            };

            const auto PAD_RANGE { BAR_RANGE - sfutil::Width(padSprite_) };
            const auto PAD_SIZE_ORIG { sfutil::Width(padSprite_.getLocalBounds()) };

            padSprite_.setScale(calcPadScale(PAD_SIZE_ORIG, PAD_RANGE), 1.0f);

            padSprite_.setPosition(
                sfutil::Right(topOrLeftSprite_),
                (sfutil::CenterOfVert(topOrLeftSprite_) - (sfutil::Height(padSprite_) * 0.5f)));

            barSprite_.setPosition(
                sfutil::CenterOfHoriz(topOrLeftSprite_),
                (sfutil::CenterOfVert(topOrLeftSprite_) - (sfutil::Height(barSprite_) * 0.5f)));

            padSprite_.setPosition(
                sfutil::Right(topOrLeftSprite_),
                (sfutil::CenterOfVert(barSprite_) - (sfutil::Height(padSprite_) * 0.5f)));

            barSprite_.setScale((BAR_RANGE / sfutil::Width(barSprite_.getLocalBounds())), 1.0f);
        }
        else
        {
            topOrLeftSprite_.setPosition(POS_V);

            botOrRightSprite_.setPosition(
                POS_V.x, (POS_V.y + (length_ - sfutil::Height(botOrRightSprite_))));

            const auto BAR_RANGE { (
                sfutil::CenterOfVert(botOrRightSprite_) - sfutil::CenterOfVert(topOrLeftSprite_)) };

            const auto PAD_RANGE { BAR_RANGE - sfutil::Height(padSprite_) };
            const auto PAD_SIZE_ORIG { sfutil::Height(padSprite_.getLocalBounds()) };

            padSprite_.setScale(1.0f, calcPadScale(PAD_SIZE_ORIG, PAD_RANGE));

            padSprite_.setPosition(
                (sfutil::CenterOfHoriz(topOrLeftSprite_) - (sfutil::Width(padSprite_) * 0.5f)),
                sfutil::Bottom(topOrLeftSprite_));

            // this rotation puts the long shadow at the top which is not correct but because we are
            // covering the ends of the bar it doesn't matter
            barSprite_.setOrigin(sfutil::Size(barSprite_) * 0.5f);
            barSprite_.rotate(270.0f);
            barSprite_.setScale((BAR_RANGE / sfutil::Height(barSprite_)), 1.0f);

            barSprite_.setPosition(
                sfutil::CenterOfHoriz(topOrLeftSprite_),
                (sfutil::CenterOfVert(topOrLeftSprite_) + (BAR_RANGE * 0.5f)));
        }

        UpdatePadPosition();
    }

    float SliderBar::CalcPositionRatioChangeOnMouseUp(const sf::Vector2f & MOUSE_POS_V) const
    {
        const auto POS_RATIO_CHANGE { CalcPositionRatioChangeOnMouseUp_Impl(MOUSE_POS_V) };

        if (willInvertPosition_)
        {
            return (-1.0f * POS_RATIO_CHANGE);
        }
        else
        {
            return POS_RATIO_CHANGE;
        }
    }

    float SliderBar::CalcPositionRatioFromScreenPos(const sf::Vector2f & SCREEN_POS_V) const
    {
        const auto POS_RATIO { CalcPositionRatioFromScreenPos_Impl(SCREEN_POS_V) };

        if (willInvertPosition_)
        {
            return (1.0f - POS_RATIO);
        }
        else
        {
            return POS_RATIO;
        }
    }

    void SliderBar::OnChange(const float)
    {
        if (callbackHandlerPtrOpt_)
        {
            Callback_t::HandleAndLog(*callbackHandlerPtrOpt_.value(), *this, "");
        }
    }

    const sf::IntRect SliderBar::TextureCoordsBaseOnStyle_TopOrLeft(const SliderStyle STYLE) const
    {
        if (style_.orientation == Orientation::Horiz)
        {
            return TextureCoordsBaseOnStyle_TopOrLeft_Horizontal(STYLE);
        }
        else
        {
            return TextureCoordsBaseOnStyle_TopOrLeft_Vertical(STYLE);
        }
    }

    const sf::IntRect
        SliderBar::TextureCoordsBaseOnStyle_BottomOrRight(const SliderStyle STYLE) const
    {
        if (style_.orientation == Orientation::Horiz)
        {
            return TextureCoordsBaseOnStyle_BottomOrRight_Horizontal(STYLE);
        }
        else
        {
            return TextureCoordsBaseOnStyle_BottomOrRight_Vertical(STYLE);
        }
    }

    const sf::IntRect
        SliderBar::TextureCoordsBaseOnStyle_TopOrLeft_Horizontal(const SliderStyle STYLE) const
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
        SliderBar::TextureCoordsBaseOnStyle_TopOrLeft_Vertical(const SliderStyle STYLE) const
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

    const sf::IntRect
        SliderBar::TextureCoordsBaseOnStyle_BottomOrRight_Horizontal(const SliderStyle STYLE) const
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
        SliderBar::TextureCoordsBaseOnStyle_BottomOrRight_Vertical(const SliderStyle STYLE) const
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

    const sf::IntRect SliderBar::TextureCoordsBasedOnStyle_Bar(const SliderStyle STYLE) const
    {
        sf::IntRect textureRect;

        if (STYLE.is_line_large)
        {
            textureRect = GuiImages::HorizontalLineLargeI();
        }
        else
        {
            textureRect = GuiImages::HorizontalLineSmallI();
        }

        const auto END_CLIP_LENGTH { 20 };
        textureRect.left += END_CLIP_LENGTH;
        textureRect.width -= (END_CLIP_LENGTH * 2);

        return textureRect;
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
            currentPosRatioToUse = (1.0f - currentPosRatioToUse);
        }

        auto calcPos = [](const float MIN_POS,
                          const float MAX_POS,
                          const float PAD_SIZE,
                          const float POS_RATIO) {
            const auto RANGE { ((MAX_POS - MIN_POS) - PAD_SIZE) };

            return (MIN_POS + (RANGE * POS_RATIO));
        };

        // this offset moves the pad away from the center of the sliderbar and makes it overlap on
        // top of the end caps topOrLeftSprite_ and botOrRightSprite_, because those sprites have
        // semi-transparent shadows that prevent padSprite_ from actually touching the endcaps.  So
        // no matter how many times you click or how hard you drag the pad it never really touches
        // the endcaps, which looks wrong and drives users crazy.  So this makes the pad overlap the
        // endcaps a little, just enough to push past the shadow, and allows the pad to touch the
        // endcaps.
        const auto OVERLAP_OFFSET { 2.0f };

        if (style_.orientation == Orientation::Horiz)
        {
            const auto LEFT { calcPos(
                (sfutil::Right(topOrLeftSprite_) - OVERLAP_OFFSET),
                (sfutil::Left(botOrRightSprite_) + OVERLAP_OFFSET),
                sfutil::Width(padSprite_),
                currentPosRatioToUse) };

            return sf::Vector2f(LEFT, sfutil::Top(padSprite_));
        }
        else
        {
            const auto TOP { calcPos(
                (sfutil::Bottom(topOrLeftSprite_) - OVERLAP_OFFSET),
                (sfutil::Top(botOrRightSprite_) + OVERLAP_OFFSET),
                sfutil::Height(padSprite_),
                currentPosRatioToUse) };

            return sf::Vector2f(sfutil::Left(padSprite_), TOP);
        }
    }

    void SliderBar::ResetMouseDownStatus()
    {
        topOrLeftIsMouseDown_ = false;
        botOrRightIsMouseDown_ = false;
        barIsMouseDown_ = false;
        padIsMouseDown_ = false;
    }

    float SliderBar::CalcMoveAmountRatio() const
    {
        if (style_.orientation == Orientation::Horiz)
        {
            return (
                sfutil::Width(padSprite_)
                / (sfutil::Left(botOrRightSprite_) - sfutil::Right(topOrLeftSprite_)));
        }
        else
        {
            return (
                sfutil::Height(padSprite_)
                / (sfutil::Top(botOrRightSprite_) - sfutil::Bottom(topOrLeftSprite_)));
        }
    }

    float SliderBar::CalcPositionRatioChangeOnMouseUp_Impl(const sf::Vector2f & MOUSE_POS_V) const
    {
        const auto MOVE_AMOUNT_RATIO { CalcMoveAmountRatio() };

        if (topOrLeftIsMouseDown_)
        {
            return -MOVE_AMOUNT_RATIO;
        }
        else if (botOrRightIsMouseDown_)
        {
            return MOVE_AMOUNT_RATIO;
        }
        else if (barIsMouseDown_)
        {
            if (style_.orientation == Orientation::Horiz)
            {
                if (MOUSE_POS_V.x < sfutil::Left(padSprite_))
                {
                    return -MOVE_AMOUNT_RATIO;
                }
                else if (MOUSE_POS_V.x > sfutil::Right(padSprite_))
                {
                    return MOVE_AMOUNT_RATIO;
                }
            }
            else
            {
                if (MOUSE_POS_V.y < sfutil::Top(padSprite_))
                {
                    return -MOVE_AMOUNT_RATIO;
                }
                else if (MOUSE_POS_V.y > sfutil::Bottom(padSprite_))
                {
                    return MOVE_AMOUNT_RATIO;
                }
            }
        }

        return 0.0f;
    }

    float SliderBar::CalcPositionRatioFromScreenPos_Impl(const sf::Vector2f & SCREEN_POS_V) const
    {
        auto calcRatio = [](const float MIN_POS, const float MAX_POS, const float ACTUAL_POS) {
            if (ACTUAL_POS < MIN_POS)
            {
                return 0.0f;
            }
            else if (ACTUAL_POS > MAX_POS)
            {
                return 1.0f;
            }
            else
            {
                return ((ACTUAL_POS - MIN_POS) / (MAX_POS - MIN_POS));
            }
        };

        if (style_.orientation == Orientation::Horiz)
        {
            return calcRatio(
                sfutil::Right(topOrLeftSprite_), sfutil::Left(botOrRightSprite_), SCREEN_POS_V.x);
        }
        else
        {
            return calcRatio(
                sfutil::Bottom(topOrLeftSprite_), sfutil::Top(botOrRightSprite_), SCREEN_POS_V.y);
        }
    }

} // namespace gui
} // namespace heroespath
