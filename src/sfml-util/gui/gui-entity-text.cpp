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
// gui-entity-text.cpp
//
#include "gui-entity-text.hpp"

#include "sfml-util/text-rendering.hpp"
#include "sfml-util/sound-manager.hpp"
#include "sfml-util/gui/mouse-text-info.hpp"

#include "misc/assertlogandthrow.hpp"


namespace sfml_util
{
namespace gui
{

    GuiText::GuiText(const std::string & NAME,
                     const float         TEXT_WIDTH_LIMIT,
                     const FontPtr_t     NUMBERS_FONT_PTR)
    :
        GuiEntity        (std::string(NAME).append("_GuiText"), 0.0f, 0.0f),
        text_            (""),
        upTextInfo_      (),
        downTextInfo_    (),
        overTextInfo_    (),
        numberFontPtr_   (NUMBERS_FONT_PTR),
        textureSPtr_     (),
        sprite_          (),
        textWidthLimit_  (TEXT_WIDTH_LIMIT),
        renderedTextSPtr_()
    {}


    GuiText::GuiText(const std::string &    NAME,
                     const sf::FloatRect &  REGION,
                     const MouseTextInfo &  MOUSE_TEXT_INFO,
                     const float            TEXT_WIDTH_LIMIT,
                     const FontPtr_t        NUMBERS_FONT_PTR)
    :
        GuiEntity        (std::string(NAME).append("_GuiText"), REGION),
        text_            (""),
        upTextInfo_      (MOUSE_TEXT_INFO.up),
        downTextInfo_    (MOUSE_TEXT_INFO.down),
        overTextInfo_    (MOUSE_TEXT_INFO.over),
        numberFontPtr_   (NUMBERS_FONT_PTR),
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
              NUMBERS_FONT_PTR);
    }


    GuiText::GuiText(const std::string &   NAME,
                     const float           POS_LEFT,
                     const float           POS_TOP,
                     const MouseTextInfo & MOUSE_TEXT_INFO,
                     const float           TEXT_WIDTH_LIMIT,
                     const FontPtr_t       NUMBERS_FONT_PTR)
    :
        GuiEntity        (std::string(NAME).append("_GuiText"), POS_LEFT, POS_TOP),
        text_            (""),
        upTextInfo_      (MOUSE_TEXT_INFO.up),
        downTextInfo_    (MOUSE_TEXT_INFO.down),
        overTextInfo_    (MOUSE_TEXT_INFO.over),
        numberFontPtr_   (NUMBERS_FONT_PTR),
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
              NUMBERS_FONT_PTR);
    }


    GuiText::~GuiText()
    {}


    void GuiText::Setup(const std::string &   TEXT,
                        const float           POS_LEFT,
                        const float           POS_TOP,
                        const MouseTextInfo & MOUSE_TEXT_INFO,
                        const float           TEXT_WIDTH_LIMIT,
                        const FontPtr_t       NUMBERS_FONT_PTR)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((false == TEXT.empty()), entityName_ << " GuiText::Setup() was given a TEXT string that was empty.");
        M_ASSERT_OR_LOGANDTHROW_SS((MOUSE_TEXT_INFO.up.fontPtr != nullptr), entityName_ << " GuiText::Setup(\"" << TEXT << "\") was given an upTextInfo with a null font pointer.");

        upTextInfo_ = MOUSE_TEXT_INFO.up;
        downTextInfo_ = MOUSE_TEXT_INFO.down;
        overTextInfo_ = MOUSE_TEXT_INFO.over;

        text_ = TEXT;
        textWidthLimit_ = TEXT_WIDTH_LIMIT;

        renderedTextSPtr_ = std::make_shared<text_render::RenderedText>();

        TextInfo textInfoChar(MOUSE_TEXT_INFO.up);
        if ((MouseState::Down == entityMouseState_) &&
            (nullptr != MOUSE_TEXT_INFO.down.fontPtr))
        {
            textInfoChar = MOUSE_TEXT_INFO.down;
        }
        else
        {
            if ((MouseState::Over == entityMouseState_) &&
                (nullptr != MOUSE_TEXT_INFO.over.fontPtr))
            {
                textInfoChar = MOUSE_TEXT_INFO.over;
            }
        }

        TextInfo textInfoNum(textInfoChar);

        if (nullptr == NUMBERS_FONT_PTR)
        {
            if (nullptr == numberFontPtr_)
            {
                textInfoNum.fontPtr = FontManager::Instance()->Font_NumbersDefault1();
            }
            else
            {
                textInfoNum.fontPtr = numberFontPtr_;
            }
        }
        else
        {
            textInfoNum.fontPtr = NUMBERS_FONT_PTR;
            numberFontPtr_ = NUMBERS_FONT_PTR;
        }

        text_render::Render( * renderedTextSPtr_,
                            textInfoChar,
                            textInfoNum,
                            text_,
                            sf::FloatRect(0, 0, textWidthLimit_, 0));

        textureSPtr_ = text_render::RenderAndDraw( * renderedTextSPtr_ );
        
        sprite_.setTexture(textureSPtr_->getTexture());
        
        sprite_.setTextureRect( sf::IntRect(0,
                                            0,
                                            static_cast<int>(renderedTextSPtr_->longest_line),
                                            static_cast<int>(renderedTextSPtr_->total_height)));

        sprite_.setPosition(POS_LEFT, POS_TOP);

        SetEntityRegion(sprite_.getGlobalBounds());
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
              numberFontPtr_);
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
                SoundManager::Instance()->SoundEffectsSet_TickOn().PlayRandom();
            else
                SoundManager::Instance()->SoundEffectsSet_TickOff().PlayRandom();

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
