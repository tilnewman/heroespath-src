//
// gui-entity-text.cpp
//
#include "gui-entity-text.hpp"

#include "sfml-util/text-rendering.hpp"
#include "sfml-util/sound-manager.hpp"
#include "sfml-util/gui/mouse-text-info.hpp"

#include "utilz/assertlogandthrow.hpp"


namespace sfml_util
{
namespace gui
{

    GuiText::GuiText(const std::string & NAME,
                     const float         TEXT_WIDTH_LIMIT,
                     const FontSPtr_t &  NUMBERS_FONTSPTR)
    :
        GuiEntity        (std::string(NAME).append("_GuiText"), 0.0f, 0.0f),
        text_            (""),
        upTextInfo_      (),
        downTextInfo_    (),
        overTextInfo_    (),
        numberFontSPtr_  (NUMBERS_FONTSPTR),
        textureSPtr_     (),
        sprite_          (),
        textWidthLimit_  (TEXT_WIDTH_LIMIT),
        renderedTextSPtr_()
    {}


    GuiText::GuiText(const std::string &    NAME,
                     const sf::FloatRect &  REGION,
                     const MouseTextInfo &  MOUSE_TEXT_INFO,
                     const float            TEXT_WIDTH_LIMIT,
                     const FontSPtr_t &     NUMBERS_FONTSPTR)
    :
        GuiEntity        (std::string(NAME).append("_GuiText"), REGION),
        text_            (""),
        upTextInfo_      (MOUSE_TEXT_INFO.up),
        downTextInfo_    (MOUSE_TEXT_INFO.down),
        overTextInfo_    (MOUSE_TEXT_INFO.over),
        numberFontSPtr_  (NUMBERS_FONTSPTR),
        textureSPtr_     (),
        sprite_          (),
        textWidthLimit_  (TEXT_WIDTH_LIMIT),
        renderedTextSPtr_()
    {
        Setup(MOUSE_TEXT_INFO.up.text,
              REGION.left,
              REGION.top,
              MOUSE_TEXT_INFO,
              TEXT_WIDTH_LIMIT,
              NUMBERS_FONTSPTR);
    }


    GuiText::GuiText(const std::string &   NAME,
                     const float           POS_LEFT,
                     const float           POS_TOP,
                     const MouseTextInfo & MOUSE_TEXT_INFO,
                     const float           TEXT_WIDTH_LIMIT,
                     const FontSPtr_t &    NUMBERS_FONTSPTR)
    :
        GuiEntity        (std::string(NAME).append("_GuiText"), POS_LEFT, POS_TOP),
        text_            (""),
        upTextInfo_      (MOUSE_TEXT_INFO.up),
        downTextInfo_    (MOUSE_TEXT_INFO.down),
        overTextInfo_    (MOUSE_TEXT_INFO.over),
        numberFontSPtr_  (NUMBERS_FONTSPTR),
        textureSPtr_     (),
        sprite_          (),
        textWidthLimit_  (TEXT_WIDTH_LIMIT),
        renderedTextSPtr_()
    {
        Setup(MOUSE_TEXT_INFO.up.text,
              POS_LEFT,
              POS_TOP,
              MOUSE_TEXT_INFO,
              TEXT_WIDTH_LIMIT,
              NUMBERS_FONTSPTR);
    }


    GuiText::~GuiText()
    {}


    void GuiText::Setup(const std::string &   TEXT,
                        const float           POS_LEFT,
                        const float           POS_TOP,
                        const MouseTextInfo & MOUSE_TEXT_INFO,
                        const float           TEXT_WIDTH_LIMIT,
                        const FontSPtr_t &    NUMBERS_FONTSPTR)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((false == TEXT.empty()), entityName_ << " GuiText::Setup() was given a TEXT string that was empty.");
        M_ASSERT_OR_LOGANDTHROW_SS((MOUSE_TEXT_INFO.up.fontSPtr.get() != nullptr), entityName_ << " GuiText::Setup(\"" << TEXT << "\") was given an upTextInfo with a null font pointer.");

        upTextInfo_ = MOUSE_TEXT_INFO.up;
        downTextInfo_ = MOUSE_TEXT_INFO.down;
        overTextInfo_ = MOUSE_TEXT_INFO.over;

        text_ = TEXT;
        textWidthLimit_ = TEXT_WIDTH_LIMIT;

        renderedTextSPtr_.reset(new text_render::RenderedText);

        TextInfo textInfoChar(MOUSE_TEXT_INFO.up);
        if ((MouseState::Down == entityMouseState_) && (nullptr != MOUSE_TEXT_INFO.down.fontSPtr.get()))
            textInfoChar = MOUSE_TEXT_INFO.down;
        else
        {
            if ((MouseState::Over == entityMouseState_) && (nullptr != MOUSE_TEXT_INFO.over.fontSPtr.get()))
                textInfoChar = MOUSE_TEXT_INFO.over;
        }

        TextInfo textInfoNum(textInfoChar);

        if (nullptr == NUMBERS_FONTSPTR.get())
        {
            if (nullptr == numberFontSPtr_.get())
                textInfoNum.fontSPtr = FontManager::Instance()->Font_NumbersDefault1();
            else
                textInfoNum.fontSPtr = numberFontSPtr_;
        }
        else
        {
            textInfoNum.fontSPtr = NUMBERS_FONTSPTR;
            numberFontSPtr_ = NUMBERS_FONTSPTR;
        }

        text_render::Render( * renderedTextSPtr_, textInfoChar, textInfoNum, text_, sf::FloatRect(0, 0, textWidthLimit_, 0));

        textureSPtr_ = text_render::RenderAndDraw( * renderedTextSPtr_ );
        sprite_.setTexture(textureSPtr_->getTexture());
        sprite_.setTextureRect(sf::IntRect(0, 0, static_cast<int>(renderedTextSPtr_->longest_line), static_cast<int>(renderedTextSPtr_->total_height)));
        sprite_.setPosition(POS_LEFT, POS_TOP);

        SetEntityRegion(sf::FloatRect(sprite_.getGlobalBounds().left,
                                      sprite_.getGlobalBounds().top,
                                      sprite_.getGlobalBounds().width,
                                      sprite_.getGlobalBounds().height));
    }


    void GuiText::SetEntityPos(const float POS_LEFT, const float POS_TOP)
    {
        GuiEntity::SetEntityPos(POS_LEFT, POS_TOP);
        sprite_.setPosition(POS_LEFT, POS_TOP);
    }


    void GuiText::MoveEntityPos(const float HORIZ, const float VERT)
    {
        GuiEntity::MoveEntityPos(HORIZ, VERT);
        sprite_.move(HORIZ, VERT);
    }


    void GuiText::SetMouseState(const MouseState::Enum E)
    {
        const bool WILL_RESET(E != GetMouseState());

        GuiEntity::SetMouseState(E);

        if (WILL_RESET)
            ResetText();
    }


    void GuiText::ResetText()
    {
        Setup(text_,
              GetEntityPos().x,
              GetEntityPos().y,
              MouseTextInfo(upTextInfo_, downTextInfo_, overTextInfo_),
              textWidthLimit_,
              numberFontSPtr_);
    }


    bool GuiText::MouseUp(const sf::Vector2f & MOUSE_POS_V)
    {
        const bool DID_MOUSE_STATE_CHANGE(GuiEntity::MouseUp(MOUSE_POS_V));
        if (DID_MOUSE_STATE_CHANGE)
            ResetText();

        return DID_MOUSE_STATE_CHANGE;
    }


    bool GuiText::MouseDown(const sf::Vector2f & MOUSE_POS_V)
    {
        const bool DID_MOUSE_STATE_CHANGE( GuiEntity::MouseDown(MOUSE_POS_V) );

        if (DID_MOUSE_STATE_CHANGE)
            ResetText();

        return DID_MOUSE_STATE_CHANGE;
    }


    bool GuiText::UpdateMousePos(const sf::Vector2f & MOUSE_POS_V)
    {
        const bool DID_MOUSE_STATE_CHANGE( GuiEntity::UpdateMousePos(MOUSE_POS_V) );

        if (DID_MOUSE_STATE_CHANGE)
        {
            if (GetMouseState() == MouseState::Over)
                SoundManager::Instance()->SoundEffectsSet_TickOn()->PlayRandom();
            else
                SoundManager::Instance()->SoundEffectsSet_TickOff()->PlayRandom();

            ResetText();
        }

        return DID_MOUSE_STATE_CHANGE;
    }


    void GuiText::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        target.draw(sprite_, states);
    }


    void GuiText::SetNewColors(const sf::Color & UP_COLOR,
                               const sf::Color & DOWN_COLOR,
                               const sf::Color & OVER_COLOR)
    {
        upTextInfo_.color   = UP_COLOR;
        downTextInfo_.color = DOWN_COLOR;
        overTextInfo_.color = OVER_COLOR;
    }


    void GuiText::SpriteColorSet(const sf::Color & NEW_COLOR)
    {
        sprite_.setColor(NEW_COLOR);
    }


    void GuiText::SpriteColorReset()
    {
        sprite_.setColor(sf::Color::White);
    }

}
}
