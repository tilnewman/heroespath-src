// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// four-state-button.cpp
//
#include "four-state-button.hpp"

#include "misc/assertlogandthrow.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/gui/text-info.hpp"
#include "sfml-util/gui/text-region.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/sfml-util.hpp"
#include "sfml-util/sound-manager.hpp"

#include <exception>
#include <sstream>

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        FourStateButton::FourStateButton(const std::string & NAME)
            : GuiEntity(std::string(NAME).append("_FourStateButton"), 0.0f, 0.0f)
            , isDisabled_(false)
            , textureUp_()
            , textureDown_()
            , textureOver_()
            , textureDisabled_()
            , hasUp_(false)
            , hasDown_(false)
            , hasOver_(false)
            , hasDisabled_(false)
            , buttonSprite_()
            , textRegionCurrPtrOpt_(boost::none)
            , textRegionUpUPtr_()
            , textRegionDownUPtr_()
            , textRegionOverUPtr_()
            , textRegionDisabledUPtr_()
            , boxUPtr_()
            , scale_(1.0f)
            , color_(sf::Color::White)
            , callbackHandlerPtrOpt_(boost::none)
        {}

        FourStateButton::FourStateButton(
            const std::string & NAME,
            const float POS_LEFT,
            const float POS_TOP,
            const std::string & IMAGE_PATH_UP,
            const std::string & IMAGE_PATH_DOWN,
            const std::string & IMAGE_PATH_OVER,
            const std::string & IMAGE_PATH_DISABLED,
            const MouseTextInfo & MOUSE_TEXT_INFO,
            const TextInfo & TEXT_INFO_DISABLED,
            const bool WILL_BOX,
            const float SCALE,
            const bool IS_DISABLED,
            const sf::Color & SPRITE_COLOR)
            : GuiEntity(std::string(NAME).append("_FourStateButton"), POS_LEFT, POS_TOP)
            , isDisabled_(IS_DISABLED)
            , textureUp_()
            , textureDown_()
            , textureOver_()
            , textureDisabled_()
            , hasUp_(false)
            , hasDown_(false)
            , hasOver_(false)
            , hasDisabled_(false)
            , buttonSprite_()
            , textRegionCurrPtrOpt_(boost::none)
            , textRegionUpUPtr_()
            , textRegionDownUPtr_()
            , textRegionOverUPtr_()
            , textRegionDisabledUPtr_()
            , boxUPtr_()
            , scale_(SCALE)
            , color_(SPRITE_COLOR)
            , callbackHandlerPtrOpt_(boost::none)
        {
            FourStateButton::Setup(
                POS_LEFT,
                POS_TOP,
                IMAGE_PATH_UP,
                IMAGE_PATH_DOWN,
                IMAGE_PATH_OVER,
                IMAGE_PATH_DISABLED,
                MOUSE_TEXT_INFO,
                TEXT_INFO_DISABLED,
                WILL_BOX,
                SCALE,
                IS_DISABLED,
                SPRITE_COLOR);
        }

        FourStateButton::FourStateButton(
            const std::string & NAME,
            const float POS_LEFT,
            const float POS_TOP,
            const sf::Texture & IMAGE_UP,
            const bool HAS_IMAGE_UP,
            const sf::Texture & IMAGE_DOWN,
            const bool HAS_IMAGE_DOWN,
            const sf::Texture & IMAGE_OVER,
            const bool HAS_IMAGE_OVER,
            const sf::Texture & IMAGE_DISABLED,
            const bool HAS_IMAGE_DISABLED,
            const MouseTextInfo & MOUSE_TEXT_INFO,
            const TextInfo & TEXT_INFO_DISABLED,
            const bool WILL_BOX,
            const float SCALE,
            const bool IS_DISABLED,
            const sf::Color & SPRITE_COLOR)
            : GuiEntity(std::string(NAME).append("_FourStateButton"), POS_LEFT, POS_TOP)
            , isDisabled_(IS_DISABLED)
            , textureUp_()
            , textureDown_()
            , textureOver_()
            , textureDisabled_()
            , hasUp_(false)
            , hasDown_(false)
            , hasOver_(false)
            , hasDisabled_(false)
            , buttonSprite_()
            , textRegionCurrPtrOpt_(boost::none)
            , textRegionUpUPtr_()
            , textRegionDownUPtr_()
            , textRegionOverUPtr_()
            , textRegionDisabledUPtr_()
            , boxUPtr_()
            , scale_(SCALE)
            , color_(SPRITE_COLOR)
            , callbackHandlerPtrOpt_(boost::none)
        {
            FourStateButton::Setup(
                POS_LEFT,
                POS_TOP,
                IMAGE_UP,
                HAS_IMAGE_UP,
                IMAGE_DOWN,
                HAS_IMAGE_DOWN,
                IMAGE_OVER,
                HAS_IMAGE_OVER,
                IMAGE_DISABLED,
                HAS_IMAGE_DISABLED,
                MOUSE_TEXT_INFO,
                TEXT_INFO_DISABLED,
                WILL_BOX,
                SCALE,
                IS_DISABLED,
                SPRITE_COLOR);
        }

        FourStateButton::~FourStateButton() = default;

        void FourStateButton::Setup(
            const float POS_LEFT,
            const float POS_TOP,
            const std::string & IMAGE_PATH_UP,
            const std::string & IMAGE_PATH_DOWN,
            const std::string & IMAGE_PATH_OVER,
            const std::string & IMAGE_PATH_DISABLED,
            const MouseTextInfo & MOUSE_TEXT_INFO,
            const TextInfo & TEXT_INFO_DISABLED,
            const bool WILL_BOX,
            const float SCALE,
            const bool IS_DISABLED,
            const sf::Color & SPRITE_COLOR)
        {
            sf::Texture up;
            bool hasUp{ false };
            sf::Texture down;
            auto hasDown{ false };
            sf::Texture over;
            auto hasOver{ false };
            sf::Texture disabled;
            auto hasDisabled{ false };

            // up image is required
            if (IMAGE_PATH_UP.empty() == false)
            {
                sfml_util::Loaders::Texture(up, IMAGE_PATH_UP);
                hasUp = true;
            }
            // down, over, disabled images are not required
            if (IMAGE_PATH_DOWN.empty() == false)
            {
                sfml_util::Loaders::Texture(down, IMAGE_PATH_DOWN);
                hasDown = true;
            }

            if (IMAGE_PATH_OVER.empty() == false)
            {
                sfml_util::Loaders::Texture(over, IMAGE_PATH_OVER);
                hasOver = true;
            }

            if (IMAGE_PATH_DISABLED.empty() == false)
            {
                sfml_util::Loaders::Texture(disabled, IMAGE_PATH_DISABLED);
                hasDisabled = true;
            }

            Setup(
                POS_LEFT,
                POS_TOP,
                up,
                hasUp,
                down,
                hasDown,
                over,
                hasOver,
                disabled,
                hasDisabled,
                MOUSE_TEXT_INFO,
                TEXT_INFO_DISABLED,
                WILL_BOX,
                SCALE,
                IS_DISABLED,
                SPRITE_COLOR);
        }

        void FourStateButton::Setup(
            const float POS_LEFT,
            const float POS_TOP,
            const sf::Texture & IMAGE_UP,
            const bool HAS_IMAGE_UP,
            const sf::Texture & IMAGE_DOWN,
            const bool HAS_IMAGE_DOWN,
            const sf::Texture & IMAGE_OVER,
            const bool HAS_IMAGE_OVER,
            const sf::Texture & IMAGE_DISABLED,
            const bool HAS_IMAGE_DISABLED,
            const MouseTextInfo & MOUSE_TEXT_INFO,
            const TextInfo & TEXT_INFO_DISABLED,
            const bool WILL_BOX,
            const float SCALE,
            const bool IS_DISABLED,
            const sf::Color & SPRITE_COLOR)
        {
            // validate TextInfo objects if text is given
            if (MOUSE_TEXT_INFO.up.text.empty() == false)
            {
                M_ASSERT_OR_LOGANDTHROW_SS(
                    (!!MOUSE_TEXT_INFO.up.fontPtrOpt),
                    "FourStateButton::Setup(\"" << MOUSE_TEXT_INFO.up.text
                                                << "\") (UP) was given a null font pointer.");
            }

            if (MOUSE_TEXT_INFO.down.text.empty() == false)
            {
                M_ASSERT_OR_LOGANDTHROW_SS(
                    (!!MOUSE_TEXT_INFO.down.fontPtrOpt),
                    "FourStateButton::Setup(\"" << MOUSE_TEXT_INFO.down.text
                                                << "\") (DOWN) was given a null font pointer.");
            }

            if (MOUSE_TEXT_INFO.over.text.empty() == false)
            {
                M_ASSERT_OR_LOGANDTHROW_SS(
                    (!!MOUSE_TEXT_INFO.over.fontPtrOpt),
                    "FourStateButton::Setup(\"" << MOUSE_TEXT_INFO.over.text
                                                << "\") (OVER) was given a null font pointer.");
            }

            if (TEXT_INFO_DISABLED.text.empty() == false)
            {
                M_ASSERT_OR_LOGANDTHROW_SS(
                    (!!TEXT_INFO_DISABLED.fontPtrOpt),
                    "FourStateButton::Setup(\"" << TEXT_INFO_DISABLED.text
                                                << "\") (DISABLED) was given a null font pointer.");
            }

            color_ = SPRITE_COLOR;
            scale_ = SCALE;
            isDisabled_ = IS_DISABLED;
            textureUp_ = IMAGE_UP;
            textureDown_ = IMAGE_DOWN;
            textureOver_ = IMAGE_OVER;
            textureDisabled_ = IMAGE_DISABLED;

            hasUp_ = HAS_IMAGE_UP;
            hasDown_ = HAS_IMAGE_DOWN;
            hasOver_ = HAS_IMAGE_OVER;
            hasDisabled_ = HAS_IMAGE_DISABLED;

            sf::FloatRect tempRect(POS_LEFT, POS_TOP, 0.0f, 0.0f);

            if (MOUSE_TEXT_INFO.up.fontPtrOpt && (MOUSE_TEXT_INFO.up.text.empty() == false))
            {
                textRegionUpUPtr_ = std::make_unique<TextRegion>(
                    GetEntityName() + "Up", MOUSE_TEXT_INFO.up, tempRect);
            }

            if (textRegionUpUPtr_)
            {
                SetEntityRegion(sf::FloatRect(
                    POS_LEFT,
                    POS_TOP,
                    textRegionUpUPtr_->GetEntityRegion().width,
                    textRegionUpUPtr_->GetEntityRegion().height));
            }
            else
            {
                if (HAS_IMAGE_UP)
                {
                    SetEntityRegion(sf::FloatRect(
                        POS_LEFT,
                        POS_TOP,
                        static_cast<float>(textureUp_.getSize().x),
                        static_cast<float>(textureUp_.getSize().y)));
                }
                else
                {
                    std::ostringstream ss;
                    ss << "sfml_util::gui::FourStateButton::Setup()  Both the given 'up' "
                       << "text and the 'up' texture were invalid.  One must be valid to "
                       << "create a FourStateButton.";

                    throw std::runtime_error(ss.str());
                }
            }

            if (MOUSE_TEXT_INFO.down.fontPtrOpt && (MOUSE_TEXT_INFO.down.text.empty() == false))
            {
                textRegionDownUPtr_ = std::make_unique<TextRegion>(
                    GetEntityName() + "Down", MOUSE_TEXT_INFO.down, GetEntityRegion());
            }

            if (MOUSE_TEXT_INFO.over.fontPtrOpt && (MOUSE_TEXT_INFO.over.text.empty() == false))
            {
                textRegionOverUPtr_ = std::make_unique<TextRegion>(
                    GetEntityName() + "Over", MOUSE_TEXT_INFO.over, GetEntityRegion());
            }

            if (TEXT_INFO_DISABLED.fontPtrOpt && (TEXT_INFO_DISABLED.text.empty() == false))
            {
                textRegionDisabledUPtr_ = std::make_unique<TextRegion>(
                    GetEntityName() + "Diabled", TEXT_INFO_DISABLED, GetEntityRegion());
            }

            if (WILL_BOX)
            {
                boxUPtr_ = std::make_unique<box::Box>(
                    "FourStateButton's",
                    box::Info(
                        true,
                        GetEntityRegion(),
                        gui::ColorSet(sf::Color::White),
                        gui::BackgroundInfo()));
            }

            SetMouseState(MouseState::Up);
            Reset();
        }

        void FourStateButton::draw(sf::RenderTarget & target, sf::RenderStates states) const
        {
            if (hasUp_)
            {
                target.draw(buttonSprite_, states);
            }

            if (textRegionCurrPtrOpt_)
            {
                textRegionCurrPtrOpt_->Obj().draw(target, states);
            }

            if (boxUPtr_)
            {
                target.draw(*boxUPtr_, states);
            }
        }

        bool FourStateButton::MouseUp(const sf::Vector2f & MOUSE_POS_V)
        {
            if (isDisabled_)
            {
                return false;
            }

            const bool DID_STATE_CHANGE(GuiEntity::MouseUp(MOUSE_POS_V));

            if (DID_STATE_CHANGE)
            {
                SoundManager::Instance()->PlaySfx_MouseClick();
                Reset();
            }

            return DID_STATE_CHANGE;
        }

        bool FourStateButton::MouseDown(const sf::Vector2f & MOUSE_POS_V)
        {
            if (isDisabled_)
            {
                return false;
            }

            const bool DID_STATE_CHANGE(GuiEntity::MouseDown(MOUSE_POS_V));

            if (DID_STATE_CHANGE)
            {
                SoundManager::Instance()->PlaySfx_MouseClick();
                Reset();
            }

            return DID_STATE_CHANGE;
        }

        bool FourStateButton::UpdateMousePos(const sf::Vector2f & MOUSE_POS_V)
        {
            if (false == isDisabled_)
            {
                if (GuiEntity::UpdateMousePos(MOUSE_POS_V))
                {
                    OnMousePosStateChange();
                    return true;
                }
            }

            return false;
        }

        void FourStateButton::SetEntityPos(const float POS_LEFT, const float POS_TOP)
        {
            GuiEntity::SetEntityPos(POS_LEFT, POS_TOP);
            Reset();
        }

        void FourStateButton::MoveEntityPos(const float HORIZ, const float VERT)
        {
            GuiEntity::MoveEntityPos(HORIZ, VERT);
            Reset();
        }

        void FourStateButton::SetIsDisabled(const bool B)
        {
            isDisabled_ = B;
            Reset();
        }

        void FourStateButton::SetMouseState(const MouseState::Enum E)
        {
            GuiEntity::SetMouseState(E);
            Reset();
        }

        void FourStateButton::SetScaleToRes()
        {
            if (hasUp_)
            {
                Scale(sfml_util::MapByRes(0.65f, 2.25f));
            }
        }

        void FourStateButton::SetVertPositionToBottomOfScreenByRes(const float POS_LEFT)
        {
            if (hasUp_)
            {
                auto const POS_TOP{ (sfml_util::Display::Instance()->GetWinHeight()
                                     - buttonSprite_.getGlobalBounds().height)
                                    - sfml_util::MapByRes(30.0f, 150.0f) };

                buttonSprite_.setPosition(POS_LEFT, POS_TOP);
                SetEntityPos(POS_LEFT, POS_TOP);
            }
        }

        void FourStateButton::Scale(const float NEW_SCALE)
        {
            if (hasUp_)
            {
                scale_ = NEW_SCALE;
                buttonSprite_.setScale(NEW_SCALE, NEW_SCALE);
                sf::FloatRect region(GetEntityRegion());
                region.width = buttonSprite_.getGlobalBounds().width;
                region.height = buttonSprite_.getGlobalBounds().height;
                SetEntityRegion(region);
            }
        }

        void FourStateButton::SetText(
            const std::string & TEXT_UP,
            const std::string & TEXT_DOWN,
            const std::string & TEXT_OVER,
            const std::string & TEXT_DISABLED)
        {
            textRegionUpUPtr_->SetText(TEXT_UP);
            textRegionDownUPtr_->SetText(((TEXT_DOWN.empty()) ? TEXT_UP : TEXT_DOWN));
            textRegionOverUPtr_->SetText(((TEXT_OVER.empty()) ? TEXT_UP : TEXT_OVER));
            textRegionDisabledUPtr_->SetText(((TEXT_DISABLED.empty()) ? TEXT_UP : TEXT_DISABLED));
        }

        void FourStateButton::Reset()
        {
            if (boxUPtr_)
            {
                boxUPtr_ = std::make_unique<box::Box>(
                    "FourStateButton's",
                    gui::box::Info(true, GetEntityRegion(), gui::ColorSet(sf::Color::White)));
            }

            if (isDisabled_)
            {
                if (textRegionDisabledUPtr_)
                {
                    textRegionCurrPtrOpt_ = textRegionDisabledUPtr_.get();
                }

                if (hasDisabled_)
                {
                    buttonSprite_.setTexture(textureDisabled_, true);
                }
            }
            else if (MouseState::Up == entityMouseState_)
            {
                if (hasUp_)
                {
                    buttonSprite_.setTexture(textureUp_, true);
                }

                if (textRegionUpUPtr_)
                {
                    textRegionCurrPtrOpt_ = textRegionUpUPtr_.get();
                }
            }
            else
            {
                if ((MouseState::Down == entityMouseState_) && hasDown_)
                {
                    buttonSprite_.setTexture(textureDown_, true);
                }

                if ((MouseState::Over == entityMouseState_) && hasOver_)
                {
                    buttonSprite_.setTexture(textureOver_, true);
                }

                if ((MouseState::Down == entityMouseState_) && (textRegionDownUPtr_))
                {
                    textRegionCurrPtrOpt_ = textRegionDownUPtr_.get();
                }

                if ((MouseState::Over == entityMouseState_) && (textRegionOverUPtr_))
                {
                    textRegionCurrPtrOpt_ = textRegionOverUPtr_.get();
                }
            }

            buttonSprite_.setPosition(GetEntityPos());
            buttonSprite_.setColor(color_);

            Scale(scale_);

            // center the text within the button image
            if (textRegionCurrPtrOpt_)
            {
                textRegionCurrPtrOpt_->Obj().SetEntityPos(
                    GetEntityRegion().left, GetEntityRegion().top);
            }
        }

        void FourStateButton::OnClick(const sf::Vector2f &)
        {
            if ((false == isDisabled_) && callbackHandlerPtrOpt_)
            {
                callbackHandlerPtrOpt_->Obj().HandleCallback(this);
            }
        }

        void FourStateButton::OnMousePosStateChange()
        {
            PlayMouseStateSfx();
            Reset();
        }

        void FourStateButton::PlayMouseStateSfx()
        {
            if (GetMouseState() == MouseState::Over)
            {
                SoundManager::Instance()->PlaySfx_TickOn();
            }
            else
            {
                SoundManager::Instance()->PlaySfx_TickOff();
            }
        }

    } // namespace gui
} // namespace sfml_util
} // namespace heroespath
