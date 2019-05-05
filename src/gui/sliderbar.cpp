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
#include "sfutil/common.hpp"
#include "sfutil/scale.hpp"

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
        const bool WILL_INVERT_POSITION,
        const float MIN_MOVE_RATIO)
        : Entity(std::string(NAME).append("_SliderBar"), POS_LEFT, POS_TOP)
        , length_(LENGTH)
        , style_(STYLE)
        , minMoveRatio_(MIN_MOVE_RATIO)
        , PAD_TEXTURE_END_LENGTH_(4)
        , currentPosRatio_(INITIAL_POS_RATIO)
        , guiElementsCachedTexture_(GuiImages::PathKey())
        , topOrLeftSprite_(
              guiElementsCachedTexture_.Get(), TextureCoordsBaseOnStyle_TopOrLeft(STYLE))
        , botOrRightSprite_(
              guiElementsCachedTexture_.Get(), TextureCoordsBaseOnStyle_BottomOrRight(STYLE))
        , barSprite_(guiElementsCachedTexture_.Get(), TextureCoordsBasedOnStyle_Bar(STYLE))
        , topOrLeftPadSprite_(guiElementsCachedTexture_.Get())
        , middlePadSprite_(guiElementsCachedTexture_.Get())
        , botOrRightPadSprite_(guiElementsCachedTexture_.Get())
        , callbackHandlerPtrOpt_(CALLBACK_HANDLER_PTR_OPT)
        , topOrLeftIsMouseDown_(false)
        , botOrRightIsMouseDown_(false)
        , barIsMouseDown_(false)
        , padIsMouseDown_(false)
        , willPlaySfx_(true)
        , willInvertPosition_(WILL_INVERT_POSITION)
        , padIsMouseDownPosV_()
    {
        SetupInitialSpritePositions(sf::Vector2f(POS_LEFT, POS_TOP));

        SetEntityRegion(sfutil::MinimallyEnclosing(
            topOrLeftSprite_,
            botOrRightSprite_,
            barSprite_,
            topOrLeftPadSprite_,
            middlePadSprite_,
            botOrRightPadSprite_));
    }

    void SliderBar::PositionRatio(const float NEW_VAL)
    {
        currentPosRatio_ = NEW_VAL;
        UpdatePadPosition();
    }

    void SliderBar::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        target.draw(barSprite_, states);

        target.draw(topOrLeftSprite_, states);
        target.draw(botOrRightSprite_, states);

        target.draw(topOrLeftPadSprite_, states);
        target.draw(middlePadSprite_, states);
        target.draw(botOrRightPadSprite_, states);
    }

    bool SliderBar::MouseDown(const sf::Vector2f & MOUSE_POS_V)
    {
        ResetMouseDownStatus();

        // check the pad and end-points before the bar because they are on top of the bar
        if (CalcPadSpriteRegion().contains(MOUSE_POS_V))
        {
            padIsMouseDown_ = true;
            padIsMouseDownPosV_ = MOUSE_POS_V;
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
            if (willPlaySfx_)
            {
                gui::SoundManager::Instance()->PlaySfx_TickOn();
            }

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
            if (style_.orientation == Orientation::Horiz)
            {
                const auto DRAG_DISTANCE { (MOUSE_POS_V.x - padIsMouseDownPosV_.x) };
                currentPosRatio_ += (DRAG_DISTANCE / length_);
            }
            else
            {
                const auto DRAG_DISTANCE { (MOUSE_POS_V.y - padIsMouseDownPosV_.y) };
                currentPosRatio_ += (DRAG_DISTANCE / length_);
            }

            UpdatePadPosition();
            padIsMouseDownPosV_ = MOUSE_POS_V;
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

        topOrLeftPadSprite_.move(HORIZ, VERT);
        middlePadSprite_.move(HORIZ, VERT);
        botOrRightPadSprite_.move(HORIZ, VERT);
    }

    void SliderBar::SetupInitialSpritePositions(const sf::Vector2f & POS_V)
    {
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

            // setup top/left pad
            {
                auto padSpriteTextureRect { TextureCoordsBasedOnStyle_Pad(style_) };
                padSpriteTextureRect.width = PAD_TEXTURE_END_LENGTH_;
                topOrLeftPadSprite_.setTextureRect(padSpriteTextureRect);
            }

            // setup bot/right pad
            {
                auto padSpriteTextureRect { TextureCoordsBasedOnStyle_Pad(style_) };

                padSpriteTextureRect.left
                    += ((padSpriteTextureRect.width - 1) - PAD_TEXTURE_END_LENGTH_);

                padSpriteTextureRect.width = PAD_TEXTURE_END_LENGTH_;

                botOrRightPadSprite_.setTextureRect(padSpriteTextureRect);
            }

            // setup middle pad
            {
                auto padSpriteTextureRect { TextureCoordsBasedOnStyle_Pad(style_) };

                padSpriteTextureRect.left += PAD_TEXTURE_END_LENGTH_;

                padSpriteTextureRect.width
                    = (padSpriteTextureRect.width - (PAD_TEXTURE_END_LENGTH_ * 2));

                middlePadSprite_.setTextureRect(padSpriteTextureRect);

                const auto PAD_LENGTH { CalcPadLength(
                    static_cast<float>(TextureCoordsBasedOnStyle_Pad(style_).width),
                    BAR_RANGE,
                    minMoveRatio_) };

                const auto MIDDLE_PAD_SCALE_HORIZ { (
                    (PAD_LENGTH - static_cast<float>(PAD_TEXTURE_END_LENGTH_ * 2))
                    / static_cast<float>(padSpriteTextureRect.width)) };

                middlePadSprite_.setScale(MIDDLE_PAD_SCALE_HORIZ, 1.0f);
            }

            SetPadPosition(sf::Vector2f(
                sfutil::Right(topOrLeftSprite_),
                (sfutil::CenterOfVert(topOrLeftSprite_)
                 - (sfutil::Height(middlePadSprite_) * 0.5f))));

            barSprite_.setPosition(
                sfutil::CenterOfHoriz(topOrLeftSprite_),
                (sfutil::CenterOfVert(topOrLeftSprite_) - (sfutil::Height(barSprite_) * 0.5f)));

            barSprite_.setScale((BAR_RANGE / sfutil::Width(barSprite_.getLocalBounds())), 1.0f);
        }
        else
        {
            topOrLeftSprite_.setPosition(POS_V);

            botOrRightSprite_.setPosition(
                POS_V.x, (POS_V.y + (length_ - sfutil::Height(botOrRightSprite_))));

            const auto BAR_RANGE { (
                sfutil::CenterOfVert(botOrRightSprite_) - sfutil::CenterOfVert(topOrLeftSprite_)) };

            // setup top/left pad
            {
                auto padSpriteTextureRect { TextureCoordsBasedOnStyle_Pad(style_) };
                padSpriteTextureRect.height = PAD_TEXTURE_END_LENGTH_;
                topOrLeftPadSprite_.setTextureRect(padSpriteTextureRect);
            }

            // setup bot/right pad
            {
                auto padSpriteTextureRect { TextureCoordsBasedOnStyle_Pad(style_) };

                padSpriteTextureRect.top
                    += ((padSpriteTextureRect.height - 1) - PAD_TEXTURE_END_LENGTH_);

                padSpriteTextureRect.height = PAD_TEXTURE_END_LENGTH_;

                botOrRightPadSprite_.setTextureRect(padSpriteTextureRect);
            }

            // setup middle pad
            {
                auto padSpriteTextureRect { TextureCoordsBasedOnStyle_Pad(style_) };

                padSpriteTextureRect.top += PAD_TEXTURE_END_LENGTH_;

                padSpriteTextureRect.height
                    = (padSpriteTextureRect.height - (PAD_TEXTURE_END_LENGTH_ * 2));

                middlePadSprite_.setTextureRect(padSpriteTextureRect);

                const auto PAD_LENGTH { CalcPadLength(
                    static_cast<float>(TextureCoordsBasedOnStyle_Pad(style_).height),
                    BAR_RANGE,
                    minMoveRatio_) };

                const auto MIDDLE_PAD_SCALE_VERT { (
                    (PAD_LENGTH - static_cast<float>(PAD_TEXTURE_END_LENGTH_ * 2))
                    / static_cast<float>(padSpriteTextureRect.height)) };

                middlePadSprite_.setScale(1.0f, MIDDLE_PAD_SCALE_VERT);
            }

            SetPadPosition(sf::Vector2f(
                (sfutil::CenterOfHoriz(topOrLeftSprite_)
                 - (sfutil::Width(middlePadSprite_) * 0.5f)),
                sfutil::Bottom(topOrLeftSprite_)));

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

    const sf::IntRect SliderBar::TextureCoordsBaseOnStyle_TopOrLeft(const SliderStyle & STYLE) const
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
        SliderBar::TextureCoordsBaseOnStyle_BottomOrRight(const SliderStyle & STYLE) const
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
        SliderBar::TextureCoordsBaseOnStyle_TopOrLeft_Horizontal(const SliderStyle & STYLE) const
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
        SliderBar::TextureCoordsBaseOnStyle_TopOrLeft_Vertical(const SliderStyle & STYLE) const
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

    const sf::IntRect SliderBar::TextureCoordsBaseOnStyle_BottomOrRight_Horizontal(
        const SliderStyle & STYLE) const
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
        SliderBar::TextureCoordsBaseOnStyle_BottomOrRight_Vertical(const SliderStyle & STYLE) const
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

    const sf::IntRect SliderBar::TextureCoordsBasedOnStyle_Bar(const SliderStyle & STYLE) const
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

    const sf::IntRect SliderBar::TextureCoordsBasedOnStyle_Pad(const SliderStyle & STYLE) const
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

        SetPadPosition(CalcPadPosition());
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

        const auto PAD_SPRITE_REGION { CalcPadSpriteRegion() };

        if (style_.orientation == Orientation::Horiz)
        {
            const auto LEFT { calcPos(
                (sfutil::Right(topOrLeftSprite_) - OVERLAP_OFFSET),
                (sfutil::Left(botOrRightSprite_) + OVERLAP_OFFSET),
                sfutil::Width(PAD_SPRITE_REGION),
                currentPosRatioToUse) };

            return sf::Vector2f(LEFT, sfutil::Top(PAD_SPRITE_REGION));
        }
        else
        {
            const auto TOP { calcPos(
                (sfutil::Bottom(topOrLeftSprite_) - OVERLAP_OFFSET),
                (sfutil::Top(botOrRightSprite_) + OVERLAP_OFFSET),
                sfutil::Height(PAD_SPRITE_REGION),
                currentPosRatioToUse) };

            return sf::Vector2f(sfutil::Left(PAD_SPRITE_REGION), TOP);
        }
    }

    void SliderBar::ResetMouseDownStatus()
    {
        topOrLeftIsMouseDown_ = false;
        botOrRightIsMouseDown_ = false;
        barIsMouseDown_ = false;
        padIsMouseDown_ = false;
        padIsMouseDownPosV_ = sf::Vector2f();
    }

    float SliderBar::CalcMoveAmountRatio() const
    {
        const auto MOVE_RATIO_DEFAULT = [&]() {
            const auto PAD_SPRITE_REGION { CalcPadSpriteRegion() };

            if (style_.orientation == Orientation::Horiz)
            {
                const auto MOVE_MAX_HORIZ { (
                    sfutil::Left(botOrRightSprite_) - sfutil::Right(topOrLeftSprite_)) };

                return (sfutil::Width(PAD_SPRITE_REGION) / MOVE_MAX_HORIZ);
            }
            else
            {
                const auto MOVE_MAX_VERT { (
                    sfutil::Top(botOrRightSprite_) - sfutil::Bottom(topOrLeftSprite_)) };

                return (sfutil::Height(PAD_SPRITE_REGION) / MOVE_MAX_VERT);
            }
        }();

        if (minMoveRatio_ > MOVE_RATIO_DEFAULT)
        {
            return minMoveRatio_;
        }
        else
        {
            return MOVE_RATIO_DEFAULT;
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
            const auto PAD_SPRITE_REGION { CalcPadSpriteRegion() };

            if (style_.orientation == Orientation::Horiz)
            {
                if (MOUSE_POS_V.x < sfutil::Left(PAD_SPRITE_REGION))
                {
                    return -MOVE_AMOUNT_RATIO;
                }
                else if (MOUSE_POS_V.x > sfutil::Right(PAD_SPRITE_REGION))
                {
                    return MOVE_AMOUNT_RATIO;
                }
            }
            else
            {
                if (MOUSE_POS_V.y < sfutil::Top(PAD_SPRITE_REGION))
                {
                    return -MOVE_AMOUNT_RATIO;
                }
                else if (MOUSE_POS_V.y > sfutil::Bottom(PAD_SPRITE_REGION))
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

    float SliderBar::CalcPadLength(
        const float SIZE_ORIG, const float RANGE, const float MIN_MOVE_RATIO) const
    {
        const auto MIN_MOVE_RANGE { (RANGE * MIN_MOVE_RATIO) };
        const auto TENTH_RANGE { (RANGE * 0.1f) };

        auto length { SIZE_ORIG };

        if (SIZE_ORIG > RANGE)
        {
            length = (RANGE * 0.5f);
        }
        else
        {
            if (SIZE_ORIG < TENTH_RANGE)
            {
                length = TENTH_RANGE;
            }

            if (length < MIN_MOVE_RANGE)
            {
                length = MIN_MOVE_RANGE;
            }
        }

        return length;
    }

    const sf::FloatRect SliderBar::CalcPadSpriteRegion() const
    {
        return sfutil::MinimallyEnclosing(
            topOrLeftPadSprite_, middlePadSprite_, botOrRightPadSprite_);
    }

    void SliderBar::SetPadPosition(const sf::Vector2f & POS_V)
    {
        topOrLeftPadSprite_.setPosition(POS_V);

        if (Orientation::Horiz == style_.orientation)
        {
            middlePadSprite_.setPosition(sfutil::Right(topOrLeftPadSprite_), POS_V.y);
            botOrRightPadSprite_.setPosition(sfutil::Right(middlePadSprite_), POS_V.y);
        }
        else
        {
            middlePadSprite_.setPosition(POS_V.x, sfutil::Bottom(topOrLeftPadSprite_));
            botOrRightPadSprite_.setPosition(POS_V.x, sfutil::Bottom(middlePadSprite_));
        }
    }

} // namespace gui
} // namespace heroespath
