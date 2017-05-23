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

#include "utilz/assertlogandthrow.hpp"

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
        textureUpSPtr_         (),
        textureDownSPtr_       (),
        textureOverSPtr_       (),
        textureDisabledSPtr_   (),
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
        textureUpSPtr_         (),
        textureDownSPtr_       (),
        textureOverSPtr_       (),
        textureDisabledSPtr_   (),
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
                                     const TextureSPtr_t & IMAGE_UP,
                                     const TextureSPtr_t & IMAGE_DOWN,
                                     const TextureSPtr_t & IMAGE_OVER,
                                     const TextureSPtr_t & IMAGE_DISABLED,
                                     const MouseTextInfo & MOUSE_TEXT_INFO,
                                     const TextInfo &      TEXT_INFO_DISABLED,
                                     const bool            WILL_BOX,
                                     const float           SCALE,
                                     const bool            IS_DISABLED)
    :
        GuiEntity              (std::string(NAME).append("_FourStateButton"), POS_LEFT, POS_TOP),
        isDisabled_            (IS_DISABLED),
        textureUpSPtr_         (),
        textureDownSPtr_       (),
        textureOverSPtr_       (),
        textureDisabledSPtr_   (),
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
              IMAGE_DOWN,
              IMAGE_OVER,
              IMAGE_DISABLED,
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
        TextureSPtr_t up;
        TextureSPtr_t down;
        TextureSPtr_t over;
        TextureSPtr_t disabled;

        //up image is required
        if (IMAGE_PATH_UP.empty() == false)
            sfml_util::LoadImageOrTextureSPtr<TextureSPtr_t>(up, IMAGE_PATH_UP);

        //down, over, disabled images are not required
        if (IMAGE_PATH_DOWN.empty() == false)
            sfml_util::LoadImageOrTextureSPtr<TextureSPtr_t>(down, IMAGE_PATH_DOWN);

        if (IMAGE_PATH_OVER.empty() == false)
            sfml_util::LoadImageOrTextureSPtr<TextureSPtr_t>(over, IMAGE_PATH_OVER);

        if (IMAGE_PATH_DISABLED.empty() == false)
            sfml_util::LoadImageOrTextureSPtr<TextureSPtr_t>(disabled, IMAGE_PATH_DISABLED);

        Setup(POS_LEFT,
              POS_TOP,
              up,
              down,
              over,
              disabled,
              MOUSE_TEXT_INFO,
              TEXT_INFO_DISABLED,
              WILL_BOX,
              SCALE,
              IS_DISABLED);
    }


    void FourStateButton::Setup(const float           POS_LEFT,
                                const float           POS_TOP,
                                const TextureSPtr_t & IMAGE_UP,
                                const TextureSPtr_t & IMAGE_DOWN,
                                const TextureSPtr_t & IMAGE_OVER,
                                const TextureSPtr_t & IMAGE_DISABLED,
                                const MouseTextInfo & MOUSE_TEXT_INFO,
                                const TextInfo &      TEXT_INFO_DISABLED,
                                const bool            WILL_BOX,
                                const float           SCALE,
                                const bool            IS_DISABLED)
    {
        //validate TextInfo objects if text is given
        if (MOUSE_TEXT_INFO.up.text.empty() == false)
            M_ASSERT_OR_LOGANDTHROW_SS((MOUSE_TEXT_INFO.up.fontSPtr.get() != nullptr), "FourStateButton::Setup(\"" << MOUSE_TEXT_INFO.up.text << "\") (UP) was given a null font pointer.");

        if (MOUSE_TEXT_INFO.down.text.empty() == false)
            M_ASSERT_OR_LOGANDTHROW_SS((MOUSE_TEXT_INFO.down.fontSPtr.get() != nullptr), "FourStateButton::Setup(\"" << MOUSE_TEXT_INFO.down.text << "\") (DOWN) was given a null font pointer.");

        if (MOUSE_TEXT_INFO.over.text.empty() == false)
            M_ASSERT_OR_LOGANDTHROW_SS((MOUSE_TEXT_INFO.over.fontSPtr.get() != nullptr), "FourStateButton::Setup(\"" << MOUSE_TEXT_INFO.over.text << "\") (OVER) was given a null font pointer.");

        if (TEXT_INFO_DISABLED.text.empty() == false)
            M_ASSERT_OR_LOGANDTHROW_SS((TEXT_INFO_DISABLED.fontSPtr.get() != nullptr), "FourStateButton::Setup(\"" << TEXT_INFO_DISABLED.text << "\") (DISABLED) was given a null font pointer.");

        scale_           = SCALE;
        isDisabled_      = IS_DISABLED;
        textureUpSPtr_   = IMAGE_UP;
        textureDownSPtr_ = IMAGE_DOWN;
        textureOverSPtr_ = IMAGE_OVER;
        textureDisabledSPtr_ = IMAGE_DISABLED;

        sf::FloatRect tempRect(POS_LEFT, POS_TOP, 0.0f, 0.0f);
        if ((MOUSE_TEXT_INFO.up.fontSPtr.get() != nullptr) && (MOUSE_TEXT_INFO.up.text.empty() == false))
            textRegionUpSPtr_.reset( new TextRegion(GetEntityName() + "Up", MOUSE_TEXT_INFO.up, tempRect) );

        if (textRegionUpSPtr_.get() != nullptr)
        {
            SetEntityRegion( sf::FloatRect(POS_LEFT, POS_TOP, textRegionUpSPtr_->GetEntityRegion().width, textRegionUpSPtr_->GetEntityRegion().height) );
        }
        else
        {
            if (textureUpSPtr_.get() != nullptr)
            {
                SetEntityRegion(sf::FloatRect(POS_LEFT, POS_TOP, static_cast<float>(textureUpSPtr_->getSize().x), static_cast<float>(textureUpSPtr_->getSize().y)));
            }
            else
            {
                std::ostringstream ss;
                ss << "sfml_util::gui::FourStateButton::Setup()  Both the given 'up' text and the 'up' texture were invalid.  One must be valid to create a FourStateButton.";
                throw std::runtime_error(ss.str());
            }
        }

        if ((MOUSE_TEXT_INFO.down.fontSPtr.get() != nullptr) && (MOUSE_TEXT_INFO.down.text.empty() == false))
            textRegionDownSPtr_.reset( new TextRegion(GetEntityName() + "Down", MOUSE_TEXT_INFO.down, GetEntityRegion()) );

        if ((MOUSE_TEXT_INFO.over.fontSPtr.get() != nullptr) && (MOUSE_TEXT_INFO.over.text.empty() == false))
            textRegionOverSPtr_.reset( new TextRegion(GetEntityName() + "Over", MOUSE_TEXT_INFO.over, GetEntityRegion()) );

        if ((TEXT_INFO_DISABLED.fontSPtr.get() != nullptr) && (TEXT_INFO_DISABLED.text.empty() == false))
            textRegionDisabledSPtr_.reset( new TextRegion(GetEntityName() + "Diabled", TEXT_INFO_DISABLED, GetEntityRegion()) );

        if (WILL_BOX)
            boxSPtr_.reset(new box::Box("FourStateButton's", box::Info(true, GetEntityRegion(), gui::ColorSet(sf::Color::White), gui::BackgroundInfo())));

        SetMouseState(MouseState::Up);
        Reset();
    }


    void FourStateButton::SetScaleToRes()
    {
        if (textureUpSPtr_.get() != nullptr)
            SetScale(sfml_util::MapByRes(0.65f, 2.25f));
    }


    void FourStateButton::SetVertPositionToBottomOfScreenByRes(const float POS_LEFT)
    {
        if (textureUpSPtr_.get() != nullptr)
        {
            const float POS_TOP((sfml_util::Display::Instance()->GetWinHeight() - buttonSprite_.getGlobalBounds().height) - sfml_util::MapByRes(42.0f, 170.0f));
            buttonSprite_.setPosition(POS_LEFT, POS_TOP);
            SetEntityPos(POS_LEFT, POS_TOP);
        }
    }


    void FourStateButton::Reset()
    {
        if (boxSPtr_.get() != nullptr)
            boxSPtr_.reset( new box::Box("FourStateButton's", gui::box::Info(true, GetEntityRegion(), gui::ColorSet(sf::Color::White))));

        if (isDisabled_)
        {
            if (textRegionDisabledSPtr_.get() != nullptr)
                textRegionCurrSPtr_ = textRegionDisabledSPtr_;

            if (textureDisabledSPtr_.get() != nullptr)
                buttonSprite_.setTexture( * textureDisabledSPtr_, true);
        }
        else if (MouseState::Up == entityMouseState_)
        {
            if (textureUpSPtr_.get() != nullptr)
                buttonSprite_.setTexture( * textureUpSPtr_, true );

            if (textRegionUpSPtr_.get() != nullptr)
                textRegionCurrSPtr_ = textRegionUpSPtr_;
        }
        else
        {
            if ((MouseState::Down == entityMouseState_) && (textureDownSPtr_.get() != nullptr))
                buttonSprite_.setTexture( * textureDownSPtr_, true );

            if ((MouseState::Over == entityMouseState_) && (textureOverSPtr_.get() != nullptr))
                buttonSprite_.setTexture( * textureOverSPtr_, true);

            if ((MouseState::Down == entityMouseState_) && (textRegionDownSPtr_.get() != nullptr))
                textRegionCurrSPtr_ = textRegionDownSPtr_;

            if ((MouseState::Over == entityMouseState_) && (textRegionOverSPtr_.get() != nullptr))
                textRegionCurrSPtr_ = textRegionOverSPtr_;
        }

        buttonSprite_.setPosition( GetEntityPos() );

        if (textureUpSPtr_.get() != nullptr)
            textureUpSPtr_->setSmooth(true);

        if (textureDownSPtr_.get() != nullptr)
            textureDownSPtr_->setSmooth(true);

        if (textureOverSPtr_.get() != nullptr)
            textureOverSPtr_->setSmooth(true);

        if (textureDisabledSPtr_.get() != nullptr)
            textureDisabledSPtr_->setSmooth(true);

        SetScale(scale_);

        //center the text within the button image
        if (textRegionCurrSPtr_.get() != nullptr)
            textRegionCurrSPtr_->SetEntityPos(GetEntityRegion().left, GetEntityRegion().top);
    }


    void FourStateButton::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        if (textureUpSPtr_.get() != nullptr)
            target.draw(buttonSprite_, states);

        if (textRegionCurrSPtr_.get() != nullptr)
            textRegionCurrSPtr_->draw(target, states);

        if (boxSPtr_.get() != nullptr)
            target.draw( * boxSPtr_, states);
    }


    bool FourStateButton::MouseUp(const sf::Vector2f & MOUSE_POS_V)
    {
        if (isDisabled_)
            return false;

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
            return false;

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
            return false;

        const bool DID_STATE_CHANGE(GuiEntity::UpdateMousePos(MOUSE_POS_V));

        if (DID_STATE_CHANGE)
        {
            if (GetMouseState() == MouseState::Over)
                SoundManager::Instance()->SoundEffectsSet_TickOn()->PlayRandom();
            else
                SoundManager::Instance()->SoundEffectsSet_TickOff()->PlayRandom();

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
            callbackHandlerPtr_->HandleCallback(this);
    }


    void FourStateButton::SetMouseState(const MouseState::Enum E)
    {
        GuiEntity::SetMouseState(E);
        Reset();
    }


    void FourStateButton::SetScale(const float NEW_SCALE)
    {
        if (textureUpSPtr_.get() != nullptr)
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
