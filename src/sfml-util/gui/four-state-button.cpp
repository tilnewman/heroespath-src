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
            , upTextureOpt_(boost::none)
            , downTextureOpt_(boost::none)
            , overTextureOpt_(boost::none)
            , disabledTextureOpt_(boost::none)
            , sprite_()
            , currTextRegionPtrOpt_(boost::none)
            , upTextRegionUPtr_()
            , downTextRegionUPtr_()
            , overTextRegionUPtr_()
            , disabledTextRegionUPtr_()
            , boxUPtr_()
            , callbackHandlerPtrOpt_(boost::none)
        {}

        FourStateButton::FourStateButton(
            const std::string & NAME,
            const sf::Vector2f & POS_V,
            const ButtonStateImageKeys & STATE_IMAGE_PATH_KEYS,
            const MouseTextInfo & STATE_TEXTINFO,
            const TextInfo & DISABLED_TEXTINFO,
            const bool WILL_BOX,
            const bool IS_DISABLED,
            const sf::Color & IMAGE_COLOR_OVERLAY,
            const float FINAL_SCALE)
            : GuiEntity(std::string(NAME).append("_FourStateButton"), POS_V)
            , isDisabled_(IS_DISABLED)
            , upTextureOpt_(boost::none)
            , downTextureOpt_(boost::none)
            , overTextureOpt_(boost::none)
            , disabledTextureOpt_(boost::none)
            , sprite_()
            , currTextRegionPtrOpt_(boost::none)
            , upTextRegionUPtr_()
            , downTextRegionUPtr_()
            , overTextRegionUPtr_()
            , disabledTextRegionUPtr_()
            , boxUPtr_()
            , callbackHandlerPtrOpt_(boost::none)
        {
            FourStateButton::Setup(
                POS_V,
                STATE_IMAGE_PATH_KEYS,
                STATE_TEXTINFO,
                DISABLED_TEXTINFO,
                WILL_BOX,
                IS_DISABLED,
                IMAGE_COLOR_OVERLAY,
                FINAL_SCALE);
        }

        FourStateButton::~FourStateButton() = default;

        void FourStateButton::Setup(
            const sf::Vector2f & POS_V,
            const ButtonStateImageKeys & STATE_IMAGE_PATH_KEYS,
            const MouseTextInfo & STATE_TEXTINFO,
            const TextInfo & DISABLED_TEXTINFO,
            const bool WILL_BOX,
            const bool IS_DISABLED,
            const sf::Color & IMAGE_COLOR_OVERLAY,
            const float FINAL_SCALE)
        {
            // load textures
            if (STATE_IMAGE_PATH_KEYS.up.empty() == false)
            {
                upTextureOpt_ = sfml_util::CachedTexture(STATE_IMAGE_PATH_KEYS.up);
            }

            if (STATE_IMAGE_PATH_KEYS.down.empty() == false)
            {
                downTextureOpt_ = sfml_util::CachedTexture(STATE_IMAGE_PATH_KEYS.down);
            }

            if (STATE_IMAGE_PATH_KEYS.over.empty() == false)
            {
                overTextureOpt_ = sfml_util::CachedTexture(STATE_IMAGE_PATH_KEYS.over);
            }

            if (STATE_IMAGE_PATH_KEYS.disabled.empty() == false)
            {
                disabledTextureOpt_ = sfml_util::CachedTexture(STATE_IMAGE_PATH_KEYS.disabled);
            }

            // validate TextInfo objects if text is given
            if (STATE_TEXTINFO.up.text.empty() == false)
            {
                M_ASSERT_OR_LOGANDTHROW_SS(
                    (STATE_TEXTINFO.up.fontPtrOpt),
                    "FourStateButton::Setup(\"" << STATE_TEXTINFO.up.text
                                                << "\") UP had an image but no font.");
            }

            if (STATE_TEXTINFO.down.text.empty() == false)
            {
                M_ASSERT_OR_LOGANDTHROW_SS(
                    (STATE_TEXTINFO.down.fontPtrOpt),
                    "FourStateButton::Setup(\"" << STATE_TEXTINFO.down.text
                                                << "\") DOWN had an image but no font.");
            }

            if (STATE_TEXTINFO.over.text.empty() == false)
            {
                M_ASSERT_OR_LOGANDTHROW_SS(
                    (STATE_TEXTINFO.over.fontPtrOpt),
                    "FourStateButton::Setup(\"" << STATE_TEXTINFO.over.text
                                                << "\") OVER had an image but no font.");
            }

            if (DISABLED_TEXTINFO.text.empty() == false)
            {
                M_ASSERT_OR_LOGANDTHROW_SS(
                    (DISABLED_TEXTINFO.fontPtrOpt),
                    "FourStateButton::Setup(\"" << DISABLED_TEXTINFO.text
                                                << "\") DISABLED had an image but no font.");
            }

            sprite_.setColor(IMAGE_COLOR_OVERLAY);
            isDisabled_ = IS_DISABLED;

            sf::FloatRect textRegionRect(POS_V, sf::Vector2f(0.0f, 0.0f));

            if (STATE_TEXTINFO.up.fontPtrOpt && (STATE_TEXTINFO.up.text.empty() == false))
            {
                upTextRegionUPtr_ = std::make_unique<TextRegion>(
                    GetEntityName() + "Up", STATE_TEXTINFO.up, textRegionRect);
            }

            if (upTextRegionUPtr_)
            {
                SetEntityRegion(sf::FloatRect(
                    POS_V,
                    sf::Vector2f(
                        upTextRegionUPtr_->GetEntityRegion().width,
                        upTextRegionUPtr_->GetEntityRegion().height)));
            }
            else if (upTextureOpt_)
            {
                SetEntityRegion(sf::FloatRect(POS_V, sf::Vector2f(upTextureOpt_->Get().getSize())));
            }
            else
            {
                std::ostringstream ss;
                ss << "sfml_util::gui::FourStateButton::Setup()  Both the given 'up' "
                   << "text and the 'up' texture were invalid.  One must be valid to "
                   << "create a FourStateButton.";

                throw std::runtime_error(ss.str());
            }

            if (STATE_TEXTINFO.down.fontPtrOpt && (STATE_TEXTINFO.down.text.empty() == false))
            {
                downTextRegionUPtr_ = std::make_unique<TextRegion>(
                    GetEntityName() + "Down", STATE_TEXTINFO.down, GetEntityRegion());
            }

            if (STATE_TEXTINFO.over.fontPtrOpt && (STATE_TEXTINFO.over.text.empty() == false))
            {
                overTextRegionUPtr_ = std::make_unique<TextRegion>(
                    GetEntityName() + "Over", STATE_TEXTINFO.over, GetEntityRegion());
            }

            if (DISABLED_TEXTINFO.fontPtrOpt && (DISABLED_TEXTINFO.text.empty() == false))
            {
                disabledTextRegionUPtr_ = std::make_unique<TextRegion>(
                    GetEntityName() + "Diabled", DISABLED_TEXTINFO, GetEntityRegion());
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
            Scale(FINAL_SCALE);
        }

        void FourStateButton::draw(sf::RenderTarget & target, sf::RenderStates states) const
        {
            if (upTextureOpt_)
            {
                target.draw(sprite_, states);
            }

            if (currTextRegionPtrOpt_)
            {
                currTextRegionPtrOpt_.value()->draw(target, states);
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

            auto const DID_STATE_CHANGE { GuiEntity::MouseUp(MOUSE_POS_V) };

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

            auto const DID_STATE_CHANGE { GuiEntity::MouseDown(MOUSE_POS_V) };

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
            if (upTextureOpt_)
            {
                Scale(sfml_util::MapByRes(0.65f, 2.25f));
            }
        }

        void FourStateButton::SetVertPositionToBottomOfScreenByRes(const float POS_LEFT)
        {
            if (upTextureOpt_)
            {
                auto const POS_TOP { (sfml_util::Display::Instance()->GetWinHeight()
                                      - sprite_.getGlobalBounds().height)
                                     - sfml_util::ScreenRatioToPixelsVert(0.0333f) };

                sprite_.setPosition(POS_LEFT, POS_TOP);
                SetEntityPos(POS_LEFT, POS_TOP);
            }
        }

        void FourStateButton::Scale(const float NEW_SCALE)
        {
            if (upTextureOpt_)
            {
                sprite_.setScale(NEW_SCALE, NEW_SCALE);
                const sf::FloatRect NEW_REGION { Position(GetEntityRegion()), Size(sprite_) };
                SetEntityRegion(NEW_REGION);
            }
        }

        void FourStateButton::SetText(
            const std::string & TEXT_UP,
            const std::string & TEXT_DOWN,
            const std::string & TEXT_OVER,
            const std::string & TEXT_DISABLED)
        {
            if (upTextRegionUPtr_)
            {
                upTextRegionUPtr_->SetText(TEXT_UP);
            }

            if (downTextRegionUPtr_)
            {
                downTextRegionUPtr_->SetText(((TEXT_DOWN.empty()) ? TEXT_UP : TEXT_DOWN));
            }

            if (overTextRegionUPtr_)
            {
                overTextRegionUPtr_->SetText(((TEXT_OVER.empty()) ? TEXT_UP : TEXT_OVER));
            }

            if (disabledTextRegionUPtr_)
            {
                disabledTextRegionUPtr_->SetText(
                    ((TEXT_DISABLED.empty()) ? TEXT_UP : TEXT_DISABLED));
            }
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
                if (disabledTextRegionUPtr_)
                {
                    currTextRegionPtrOpt_ = disabledTextRegionUPtr_.get();
                }

                if (disabledTextureOpt_)
                {
                    sprite_.setTexture(disabledTextureOpt_->Get(), true);
                }
            }
            else if (MouseState::Up == entityMouseState_)
            {
                if (upTextureOpt_)
                {
                    sprite_.setTexture(upTextureOpt_->Get(), true);
                }

                if (upTextRegionUPtr_)
                {
                    currTextRegionPtrOpt_ = upTextRegionUPtr_.get();
                }
            }
            else
            {
                if ((MouseState::Down == entityMouseState_) && downTextureOpt_)
                {
                    sprite_.setTexture(downTextureOpt_->Get(), true);
                }

                if ((MouseState::Over == entityMouseState_) && overTextureOpt_)
                {
                    sprite_.setTexture(overTextureOpt_->Get(), true);
                }

                if ((MouseState::Down == entityMouseState_) && (downTextRegionUPtr_))
                {
                    currTextRegionPtrOpt_ = downTextRegionUPtr_.get();
                }

                if ((MouseState::Over == entityMouseState_) && (overTextRegionUPtr_))
                {
                    currTextRegionPtrOpt_ = overTextRegionUPtr_.get();
                }
            }

            sprite_.setPosition(GetEntityPos());

            Scale(sprite_.getScale().x);

            // center the text within the button image
            if (currTextRegionPtrOpt_)
            {
                currTextRegionPtrOpt_.value()->SetEntityPos(
                    GetEntityRegion().left, GetEntityRegion().top);
            }
        }

        void FourStateButton::OnClick(const sf::Vector2f &)
        {
            if ((false == isDisabled_) && callbackHandlerPtrOpt_)
            {
                callbackHandlerPtrOpt_.value()->HandleCallback(this);
            }
        }

        void FourStateButton::OnMousePosStateChange()
        {
            if (!callbackHandlerPtrOpt_
                || callbackHandlerPtrOpt_.value()->WillAllowMousePosStateChange())
            {
                PlayMouseStateSfx();
                Reset();
            }
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
