//
// gui-entity-image-text.cpp
//
#include "two-state-entity.hpp"

#include "sfml-util/gui/text-info.hpp"
#include "sfml-util/gui/mouse-text-info.hpp"

#include "heroespath/assertlogandthrow.hpp"


namespace sfml_util
{
namespace gui
{

    const float TwoStateEntity::HORIZ_PAD_(5.0f);
    const float TwoStateEntity::UPPER_PAD_(5.0f);


    TwoStateEntity::TwoStateEntity( const std::string &   NAME,
                                    const float           POS_LEFT,
                                    const float           POS_TOP,
                                    const sf::Sprite &    SPRITE_UP,
                                    const sf::Sprite &    SPRITE_DOWN,
                                    const MouseTextInfo & MOUSE_TEXT_INFO)
    :
        GuiEntity       (std::string(NAME).append("_TwoStateEntity"), POS_LEFT, POS_TOP),
        guiText_        (std::string(NAME).append("'s_GuiText"), POS_LEFT, POS_TOP, MOUSE_TEXT_INFO),
        guiImage_       (std::string(NAME).append("'s_GuiImage"), POS_LEFT, POS_TOP, SPRITE_UP, SPRITE_DOWN),
        wasDownInRegion_(false)
    {}


    TwoStateEntity::TwoStateEntity( const std::string &   NAME,
                                    const float           POS_LEFT,
                                    const float           POS_TOP,
                                    const MouseTextInfo & MOUSE_TEXT_INFO)
    :
        GuiEntity       (std::string(NAME).append("_TwoStateEntity"), POS_LEFT, POS_TOP),
        guiText_        (std::string(NAME).append("'s_GuiText"), POS_LEFT, POS_TOP, MOUSE_TEXT_INFO),
        guiImage_       (std::string(NAME).append("'s_GuiImage"), POS_LEFT, POS_TOP, sf::Sprite()),
        wasDownInRegion_(false)
    {}


    TwoStateEntity::TwoStateEntity( const std::string &   NAME,
                                    const float           POS_LEFT,
                                    const float           POS_TOP,
                                    const TextInfo &      TEXT_INFO_UP )
    :
        GuiEntity       (std::string(NAME).append("_TwoStateEntity"), POS_LEFT, POS_TOP),
        guiText_        (std::string(NAME).append("'s_GuiText"), POS_LEFT, POS_TOP, MouseTextInfo(TEXT_INFO_UP)),
        guiImage_       (std::string(NAME).append("'s_GuiImage"), POS_LEFT, POS_TOP, sf::Sprite()),
        wasDownInRegion_(false)
    {}


    TwoStateEntity::~TwoStateEntity()
    {}


    void TwoStateEntity::PostSpriteSetup(const float POS_LEFT, const float POS_TOP)
    {
        //up and down state sprites are required, but the over state sprite is not used.
        M_ASSERT_OR_LOGANDTHROW_SS((guiImage_.GetUpSprite().getTexture() != nullptr), entityName_ << "'s TwoStateEntity::PostSpriteSetup() was given a guiImage_.GetUpSprite() with a nullptr texture.");
        M_ASSERT_OR_LOGANDTHROW_SS((guiImage_.GetDownSprite().getTexture() != nullptr), entityName_ << "'s TwoStateEntity::PostSpriteSetup() was given a guiImage_.GetDownSprite() with a nullptr texture.");

        const float UP_SPRITE_POS_Y((POS_TOP + (guiText_.GetTextHeight() * 0.5f)) - (guiImage_.GetUpSprite().getLocalBounds().height * 0.5f));
        guiImage_.GetUpSprite().setPosition(POS_LEFT, UP_SPRITE_POS_Y);

        const float DOWN_SPRITE_POS_Y((POS_TOP + (guiText_.GetTextHeight() * 0.5f)) - (guiImage_.GetDownSprite().getLocalBounds().height * 0.5f));
        guiImage_.GetDownSprite().setPosition(POS_LEFT, DOWN_SPRITE_POS_Y);

        guiText_.SetEntityPos(POS_LEFT + guiImage_.GetUpSprite().getLocalBounds().width + HORIZ_PAD_, POS_TOP - UPPER_PAD_);

        SetEntityRegion( sf::FloatRect(POS_LEFT, POS_TOP, guiImage_.GetUpSprite().getLocalBounds().width + guiText_.GetTextWidth() + HORIZ_PAD_, guiText_.GetTextHeight()) );
    }


    void TwoStateEntity::SetIsInFirstState(const bool IS_IN_FIRST_STATE)
    {
        if (IS_IN_FIRST_STATE)
        {
            guiImage_.SetMouseState(MouseState::Up);
            guiText_.SetMouseState(MouseState::Up);
        }
        else
        {
            guiImage_.SetMouseState(MouseState::Down);
            guiText_.SetMouseState(MouseState::Down);
        }
    }


    bool TwoStateEntity::MouseUp(const sf::Vector2f & MOUSE_POS_V)
    {
        bool didStateChange(false);

        //Use MouseState::Down as the second state possible for a TwoStateEntity.
        //This behavior matches that of a checkbox, or radio-button, etc.
        if (GetEntityRegion().contains(MOUSE_POS_V) && wasDownInRegion_)
        {
            if (guiText_.GetMouseState() != MouseState::Down)
            {
                guiText_.SetMouseState(MouseState::Down);
                didStateChange = true;
            }

            if (guiImage_.GetMouseState() != MouseState::Down)
            {
                guiImage_.SetMouseState(MouseState::Down);
                didStateChange = true;
            }

            wasDownInRegion_ = false;
            return didStateChange;
        }

        wasDownInRegion_ = false;

        didStateChange = (guiText_.MouseUp(MOUSE_POS_V) || guiImage_.MouseUp(MOUSE_POS_V));

        //prevent the image from being in state MouseState::Over
        if (MouseState::Over == guiImage_.GetMouseState())
            guiImage_.SetMouseState(MouseState::Over);

        return false;
    }


    bool TwoStateEntity::MouseDown(const sf::Vector2f & MOUSE_POS_V)
    {
        //Prevent the down state from working normally,
        //and only allow it when un-clicking within the entity region.
        wasDownInRegion_ = GetEntityRegion().contains(MOUSE_POS_V);

        //prevent default MouseDown() behavior
        //guiText_.MouseDown(MOUSE_POS_V);
        //guiImage_.MouseDown(MOUSE_POS_V);

        return false;
    }


    bool TwoStateEntity::UpdateMousePos(const sf::Vector2f & MOUSE_POS_V)
    {
       const bool DID_STATE_CHANGE(guiText_.UpdateMousePos(MOUSE_POS_V) || guiImage_.UpdateMousePos(MOUSE_POS_V));

       //prevent the image from being in state MouseState::Over
       if (MouseState::Over == guiImage_.GetMouseState())
           guiImage_.SetMouseState(MouseState::Up);

       return DID_STATE_CHANGE;
    }


    void TwoStateEntity::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        guiImage_.draw(target, states);
        guiText_.draw(target, states);
    }


    void TwoStateEntity::SetMouseState(const MouseState::Enum E)
    {
        GuiEntity::SetMouseState(E);
        guiImage_.SetMouseState(E);
        guiText_.SetMouseState(E);
    }


    void TwoStateEntity::SetEntityPos(const float POS_LEFT, const float POS_TOP)
    {
        GuiEntity::SetEntityPos(POS_LEFT, POS_TOP);
        guiImage_.SetEntityPos(POS_LEFT, POS_TOP);
        guiText_.SetEntityPos(POS_LEFT, POS_TOP);
    }


    void TwoStateEntity::MoveEntityPos(const float HORIZ, const float VERT)
    {
        GuiEntity::MoveEntityPos(HORIZ, VERT);
        guiImage_.MoveEntityPos(HORIZ, VERT);
        guiText_.MoveEntityPos(HORIZ, VERT);
    }

}
}
