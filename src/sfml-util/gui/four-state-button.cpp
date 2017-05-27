// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software.  If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
//
// four-state-button.cpp
//
#include "four-state-button.hpp"

#include "sfml-util/sfml-util.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/sound-manager.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/gui/text-region.hpp"
#include "sfml-util/gui/text-info.hpp"

#include "misc/assertlogandthrow.hpp"

#include <exception>
#include <sstream>


namespace sfml_util
{
namespace gui
{

    FourStateButton::FourStateButton(const std::string & NAME)
    :
        GuiEntity              (std::string(NAME).append("_FourStateButton"), 0.0f, 0.0f),
        isDisabled_            (false),
        textureUp_             (),
        textureDown_           (),
        textureOver_           (),
        textureDisabled_       (),
        hasUp_                 (false),
        hasDown_               (false),
        hasOver_               (false),
        hasDisabled_           (false),
        buttonSprite_          (),
        textRegionCurrSPtr_    (),
        textRegionUpSPtr_      (),
        textRegionDownSPtr_    (),
        textRegionOverSPtr_    (),
        textRegionDisabledSPtr_(),
        boxSPtr_               (),
        scale_                 (1.0f),
        callbackHandlerPtr_    (nullptr)
    {}


    FourStateButton::FourStateButton(const std::string &   NAME,
                                     const float           POS_LEFT,
                                     const float           POS_TOP,
                                     const std::string &   IMAGE_PATH_UP,
                                     const std::string &   IMAGE_PATH_DOWN,
                                     const std::string &   IMAGE_PATH_OVER,
                                     const std::string &   IMAGE_PATH_DISABLED,
                                     const MouseTextInfo & MOUSE_TEXT_INFO,
                                     const TextInfo &      TEXT_INFO_DISABLED,
                                     const bool            WILL_BOX,
                                     const float           SCALE,
                                     const bool            IS_DISABLED)
    :
        GuiEntity              (std::string(NAME).append("_FourStateButton"), POS_LEFT, POS_TOP),
        isDisabled_            (IS_DISABLED),
        textureUp_             (),
        textureDown_           (),
        textureOver_           (),
        textureDisabled_       (),
        hasUp_                 (false),
        hasDown_               (false),
        hasOver_               (false),
        hasDisabled_           (false),
        buttonSprite_          (),
        textRegionCurrSPtr_    (),
        textRegionUpSPtr_      (),
        textRegionDownSPtr_    (),
        textRegionOverSPtr_    (),
        textRegionDisabledSPtr_(),
        boxSPtr_               (),
        scale_                 (SCALE),
        callbackHandlerPtr_    (nullptr)
    {
        Setup(POS_LEFT,
              POS_TOP,
              IMAGE_PATH_UP,
              IMAGE_PATH_DOWN,
              IMAGE_PATH_OVER,
              IMAGE_PATH_DISABLED,
              MOUSE_TEXT_INFO,
              TEXT_INFO_DISABLED,
              WILL_BOX,
              SCALE,
              IS_DISABLED);
    }


    FourStateButton::FourStateButton(const std::string &   NAME,
                                     const float           POS_LEFT,
                                     const float           POS_TOP,
                                     const sf::Texture &   IMAGE_UP,
                                     const bool            HAS_IMAGE_UP,
                                     const sf::Texture &   IMAGE_DOWN,
                                     const bool            HAS_IMAGE_DOWN,
                                     const sf::Texture &   IMAGE_OVER,
                                     const bool            HAS_IMAGE_OVER,
                                     const sf::Texture &   IMAGE_DISABLED,
                                     const bool            HAS_IMAGE_DISABLED,
                                     const MouseTextInfo & MOUSE_TEXT_INFO,
                                     const TextInfo &      TEXT_INFO_DISABLED,
                                     const bool            WILL_BOX,
                                     const float           SCALE,
                                     const bool            IS_DISABLED)
    :
        GuiEntity              (std::string(NAME).append("_FourStateButton"), POS_LEFT, POS_TOP),
        isDisabled_            (IS_DISABLED),
        textureUp_             (),
        textureDown_           (),
        textureOver_           (),
        textureDisabled_       (),
        hasUp_                 (false),
        hasDown_               (false),
        hasOver_               (false),
        hasDisabled_           (false),
        buttonSprite_          (),
        textRegionCurrSPtr_    (),
        textRegionUpSPtr_      (),
        textRegionDownSPtr_    (),
        textRegionOverSPtr_    (),
        textRegionDisabledSPtr_(),
        boxSPtr_               (),
        scale_                 (SCALE),
        callbackHandlerPtr_    (nullptr)
    {
        Setup(POS_LEFT,
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
              IS_DISABLED);
    }


    FourStateButton::~FourStateButton()
    {}


    void FourStateButton::Setup(const float           POS_LEFT,
                                const float           POS_TOP,
                                const std::string &   IMAGE_PATH_UP,
                                const std::string &   IMAGE_PATH_DOWN,
                                const std::string &   IMAGE_PATH_OVER,
                                const std::string &   IMAGE_PATH_DISABLED,
                                const MouseTextInfo & MOUSE_TEXT_INFO,
                                const TextInfo &      TEXT_INFO_DISABLED,
                                const bool            WILL_BOX,
                                const float           SCALE,
                                const bool            IS_DISABLED)
    {
        sf::Texture up;
        bool hasUp{ false };
        sf::Texture down;
        auto hasDown{ false };
        sf::Texture over;
        auto hasOver{ false };
        sf::Texture disabled;
        auto hasDisabled{ false };

        //up image is required
        if (IMAGE_PATH_UP.empty() == false)
        {
            sfml_util::LoadImageOrTexture(up, IMAGE_PATH_UP);
            hasUp = true;
        }
        //down, over, disabled images are not required
        if (IMAGE_PATH_DOWN.empty() == false)
        {
            sfml_util::LoadImageOrTexture(down, IMAGE_PATH_DOWN);
            hasDown = true;
        }

        if (IMAGE_PATH_OVER.empty() == false)
        {
            sfml_util::LoadImageOrTexture(over, IMAGE_PATH_OVER);
            hasOver = true;
        }

        if (IMAGE_PATH_DISABLED.empty() == false)
        {
            sfml_util::LoadImageOrTexture(disabled, IMAGE_PATH_DISABLED);
            hasDisabled = true;
        }

        Setup(POS_LEFT,
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
              IS_DISABLED);
    }


    void FourStateButton::Setup(const float           POS_LEFT,
                                const float           POS_TOP,
                                const sf::Texture &   IMAGE_UP,
                                const bool            HAS_IMAGE_UP,
                                const sf::Texture &   IMAGE_DOWN,
                                const bool            HAS_IMAGE_DOWN,
                                const sf::Texture &   IMAGE_OVER,
                                const bool            HAS_IMAGE_OVER,
                                const sf::Texture &   IMAGE_DISABLED,
                                const bool            HAS_IMAGE_DISABLED,
                                const MouseTextInfo & MOUSE_TEXT_INFO,
                                const TextInfo &      TEXT_INFO_DISABLED,
                                const bool            WILL_BOX,
                                const float           SCALE,
                                const bool            IS_DISABLED)
    {
        //validate TextInfo objects if text is given
        if (MOUSE_TEXT_INFO.up.text.empty() == false)
        {
            M_ASSERT_OR_LOGANDTHROW_SS((MOUSE_TEXT_INFO.up.fontPtr != nullptr), "FourStateButton::Setup(\"" << MOUSE_TEXT_INFO.up.text << "\") (UP) was given a null font pointer.");
        }

        if (MOUSE_TEXT_INFO.down.text.empty() == false)
        {
            M_ASSERT_OR_LOGANDTHROW_SS((MOUSE_TEXT_INFO.down.fontPtr != nullptr), "FourStateButton::Setup(\"" << MOUSE_TEXT_INFO.down.text << "\") (DOWN) was given a null font pointer.");
        }

        if (MOUSE_TEXT_INFO.over.text.empty() == false)
        {
            M_ASSERT_OR_LOGANDTHROW_SS((MOUSE_TEXT_INFO.over.fontPtr != nullptr), "FourStateButton::Setup(\"" << MOUSE_TEXT_INFO.over.text << "\") (OVER) was given a null font pointer.");
        }

        if (TEXT_INFO_DISABLED.text.empty() == false)
        {
            M_ASSERT_OR_LOGANDTHROW_SS((TEXT_INFO_DISABLED.fontPtr != nullptr), "FourStateButton::Setup(\"" << TEXT_INFO_DISABLED.text << "\") (DISABLED) was given a null font pointer.");
        }

        scale_           = SCALE;
        isDisabled_      = IS_DISABLED;
        textureUp_       = IMAGE_UP;
        textureDown_     = IMAGE_DOWN;
        textureOver_     = IMAGE_OVER;
        textureDisabled_ = IMAGE_DISABLED;

        hasUp_ = HAS_IMAGE_UP;
        hasDown_ = HAS_IMAGE_DOWN;
        hasOver_ = HAS_IMAGE_OVER;
        hasDisabled_ = HAS_IMAGE_DISABLED;

        sf::FloatRect tempRect(POS_LEFT, POS_TOP, 0.0f, 0.0f);
        if ((MOUSE_TEXT_INFO.up.fontPtr != nullptr) && (MOUSE_TEXT_INFO.up.text.empty() == false))
        {
            textRegionUpSPtr_.reset(new TextRegion(GetEntityName() + "Up", MOUSE_TEXT_INFO.up, tempRect));
        }

        if (textRegionUpSPtr_.get() != nullptr)
        {
            SetEntityRegion( sf::FloatRect(POS_LEFT, POS_TOP, textRegionUpSPtr_->GetEntityRegion().width, textRegionUpSPtr_->GetEntityRegion().height) );
        }
        else
        {
            if (HAS_IMAGE_UP)
            {
                SetEntityRegion(sf::FloatRect(POS_LEFT, POS_TOP, static_cast<float>(textureUp_.getSize().x), static_cast<float>(textureUp_.getSize().y)));
            }
            else
            {
                std::ostringstream ss;
                ss << "sfml_util::gui::FourStateButton::Setup()  Both the given 'up' text and the 'up' texture were invalid.  One must be valid to create a FourStateButton.";
                throw std::runtime_error(ss.str());
            }
        }

        if ((MOUSE_TEXT_INFO.down.fontPtr != nullptr) && (MOUSE_TEXT_INFO.down.text.empty() == false))
        {
            textRegionDownSPtr_.reset(new TextRegion(GetEntityName() + "Down", MOUSE_TEXT_INFO.down, GetEntityRegion()));
        }

        if ((MOUSE_TEXT_INFO.over.fontPtr != nullptr) && (MOUSE_TEXT_INFO.over.text.empty() == false))
        {
            textRegionOverSPtr_.reset(new TextRegion(GetEntityName() + "Over", MOUSE_TEXT_INFO.over, GetEntityRegion()));
        }

        if ((TEXT_INFO_DISABLED.fontPtr != nullptr) && (TEXT_INFO_DISABLED.text.empty() == false))
        {
            textRegionDisabledSPtr_.reset(new TextRegion(GetEntityName() + "Diabled", TEXT_INFO_DISABLED, GetEntityRegion()));
        }

        if (WILL_BOX)
        {
            boxSPtr_.reset(new box::Box("FourStateButton's", box::Info(true, GetEntityRegion(), gui::ColorSet(sf::Color::White), gui::BackgroundInfo())));
        }

        SetMouseState(MouseState::Up);
        Reset();
    }


    void FourStateButton::SetScaleToRes()
    {
        if (hasUp_)
        {
            SetScale(sfml_util::MapByRes(0.65f, 2.25f));
        }
    }


    void FourStateButton::SetVertPositionToBottomOfScreenByRes(const float POS_LEFT)
    {
        if (hasUp_)
        {
            const float POS_TOP((sfml_util::Display::Instance()->GetWinHeight() - buttonSprite_.getGlobalBounds().height) - sfml_util::MapByRes(42.0f, 170.0f));
            buttonSprite_.setPosition(POS_LEFT, POS_TOP);
            SetEntityPos(POS_LEFT, POS_TOP);
        }
    }


    void FourStateButton::Reset()
    {
        if (boxSPtr_.get() != nullptr)
        {
            boxSPtr_.reset(new box::Box("FourStateButton's", gui::box::Info(true, GetEntityRegion(), gui::ColorSet(sf::Color::White))));
        }

        if (isDisabled_)
        {
            if (textRegionDisabledSPtr_.get() != nullptr)
            {
                textRegionCurrSPtr_ = textRegionDisabledSPtr_;
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

            if (textRegionUpSPtr_.get() != nullptr)
            {
                textRegionCurrSPtr_ = textRegionUpSPtr_;
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

            if ((MouseState::Down == entityMouseState_) &&
                (textRegionDownSPtr_.get() != nullptr))
            {
                textRegionCurrSPtr_ = textRegionDownSPtr_;
            }

            if ((MouseState::Over == entityMouseState_) &&
                (textRegionOverSPtr_.get() != nullptr))
            {
                textRegionCurrSPtr_ = textRegionOverSPtr_;
            }
        }

        buttonSprite_.setPosition( GetEntityPos() );

        if (hasUp_)
        {
            textureUp_.setSmooth(true);
        }

        if (hasDown_)
        {
            textureDown_.setSmooth(true);
        }

        if (hasOver_)
        {
            textureOver_.setSmooth(true);
        }

        if (hasDisabled_)
        {
            textureDisabled_.setSmooth(true);
        }

        SetScale(scale_);

        //center the text within the button image
        if (textRegionCurrSPtr_.get() != nullptr)
        {
            textRegionCurrSPtr_->SetEntityPos(GetEntityRegion().left, GetEntityRegion().top);
        }
    }


    void FourStateButton::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        if (hasUp_)
        {
            target.draw(buttonSprite_, states);
        }

        if (textRegionCurrSPtr_.get() != nullptr)
        {
            textRegionCurrSPtr_->draw(target, states);
        }

        if (boxSPtr_.get() != nullptr)
        {
            target.draw(*boxSPtr_, states);
        }
    }


    bool FourStateButton::MouseUp(const sf::Vector2f & MOUSE_POS_V)
    {
        if (isDisabled_)
        {
            return false;
        }

        const bool DID_STATE_CHANGE( GuiEntity::MouseUp(MOUSE_POS_V) );

        if (DID_STATE_CHANGE)
        {
            SoundManager::Instance()->SoundEffectsSet_Switch()->PlayRandom();
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
            SoundManager::Instance()->SoundEffectsSet_Switch()->PlayRandom();
            Reset();
        }

        return DID_STATE_CHANGE;
    }


    bool FourStateButton::UpdateMousePos(const sf::Vector2f & MOUSE_POS_V)
    {
        if (isDisabled_)
        {
            return false;
        }

        const bool DID_STATE_CHANGE(GuiEntity::UpdateMousePos(MOUSE_POS_V));

        if (DID_STATE_CHANGE)
        {
            if (GetMouseState() == MouseState::Over)
            {
                SoundManager::Instance()->SoundEffectsSet_TickOn()->PlayRandom();
            }
            else
            {
                SoundManager::Instance()->SoundEffectsSet_TickOff()->PlayRandom();
            }

            Reset();
        }

        return DID_STATE_CHANGE;
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


    void FourStateButton::OnClick(const sf::Vector2f &)
    {
        if ((false == isDisabled_) && (callbackHandlerPtr_ != nullptr))
        {
            callbackHandlerPtr_->HandleCallback(this);
        }
    }


    void FourStateButton::SetMouseState(const MouseState::Enum E)
    {
        GuiEntity::SetMouseState(E);
        Reset();
    }


    void FourStateButton::SetScale(const float NEW_SCALE)
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

}
}
