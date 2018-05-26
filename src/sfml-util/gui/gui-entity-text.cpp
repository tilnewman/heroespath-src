// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// gui-entity-text.cpp
//
#include "gui-entity-text.hpp"

#include "misc/assertlogandthrow.hpp"
#include "sfml-util/gui/mouse-text-info.hpp"
#include "sfml-util/sound-manager.hpp"
#include "sfml-util/text-rendering.hpp"

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        GuiText::GuiText(
            const std::string & NAME,
            const float TEXT_WIDTH_LIMIT,
            const FontPtrOpt_t NUMBERS_FONT_PTR_OPT)
            : GuiEntity(std::string(NAME).append("_GuiText"), 0.0f, 0.0f)
            , text_("")
            , upTextInfo_()
            , downTextInfo_()
            , overTextInfo_()
            , numberFontPtrOpt_(NUMBERS_FONT_PTR_OPT)
            , offscreenTexture_()
            , sprite_()
            , textWidthLimit_(TEXT_WIDTH_LIMIT)
            , renderedText_()
        {}

        GuiText::GuiText(
            const std::string & NAME,
            const sf::FloatRect & REGION,
            const MouseTextInfo & MOUSE_TEXT_INFO,
            const float TEXT_WIDTH_LIMIT,
            const FontPtrOpt_t NUMBERS_FONT_PTR_OPT)
            : GuiEntity(std::string(NAME).append("_GuiText"), REGION)
            , text_("")
            , upTextInfo_(MOUSE_TEXT_INFO.up)
            , downTextInfo_(MOUSE_TEXT_INFO.down)
            , overTextInfo_(MOUSE_TEXT_INFO.over)
            , numberFontPtrOpt_(NUMBERS_FONT_PTR_OPT)
            , offscreenTexture_()
            , sprite_()
            , textWidthLimit_(TEXT_WIDTH_LIMIT)
            , renderedText_()
        {
            Setup(
                MOUSE_TEXT_INFO.up.text,
                REGION.left,
                REGION.top,
                MOUSE_TEXT_INFO,
                TEXT_WIDTH_LIMIT,
                NUMBERS_FONT_PTR_OPT);
        }

        GuiText::GuiText(
            const std::string & NAME,
            const float POS_LEFT,
            const float POS_TOP,
            const MouseTextInfo & MOUSE_TEXT_INFO,
            const float TEXT_WIDTH_LIMIT,
            const FontPtrOpt_t NUMBERS_FONT_PTR_OPT)
            : GuiEntity(std::string(NAME).append("_GuiText"), POS_LEFT, POS_TOP)
            , text_("")
            , upTextInfo_(MOUSE_TEXT_INFO.up)
            , downTextInfo_(MOUSE_TEXT_INFO.down)
            , overTextInfo_(MOUSE_TEXT_INFO.over)
            , numberFontPtrOpt_(NUMBERS_FONT_PTR_OPT)
            , offscreenTexture_()
            , sprite_()
            , textWidthLimit_(TEXT_WIDTH_LIMIT)
            , renderedText_()
        {
            Setup(
                MOUSE_TEXT_INFO.up.text,
                POS_LEFT,
                POS_TOP,
                MOUSE_TEXT_INFO,
                TEXT_WIDTH_LIMIT,
                NUMBERS_FONT_PTR_OPT);
        }

        GuiText::~GuiText() = default;

        void GuiText::Setup(
            const std::string & TEXT,
            const float POS_LEFT,
            const float POS_TOP,
            const MouseTextInfo & MOUSE_TEXT_INFO,
            const float TEXT_WIDTH_LIMIT,
            const FontPtrOpt_t NUMBERS_FONT_PTR_OPT)
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (false == TEXT.empty()),
                entityName_ << " GuiText::Setup() was given a TEXT string that was empty.");

            M_ASSERT_OR_LOGANDTHROW_SS(
                (!!MOUSE_TEXT_INFO.up.fontPtrOpt),
                entityName_ << " GuiText::Setup(\"" << TEXT
                            << "\") was given an upTextInfo with a null font pointer.");

            upTextInfo_ = MOUSE_TEXT_INFO.up;
            downTextInfo_ = MOUSE_TEXT_INFO.down;
            overTextInfo_ = MOUSE_TEXT_INFO.over;

            text_ = TEXT;
            textWidthLimit_ = TEXT_WIDTH_LIMIT;

            renderedText_.Reset();

            TextInfo textInfoChar(MOUSE_TEXT_INFO.up);
            if ((MouseState::Down == entityMouseState_) && MOUSE_TEXT_INFO.down.fontPtrOpt)
            {
                textInfoChar = MOUSE_TEXT_INFO.down;
            }
            else
            {
                if ((MouseState::Over == entityMouseState_) && MOUSE_TEXT_INFO.over.fontPtrOpt)
                {
                    textInfoChar = MOUSE_TEXT_INFO.over;
                }
            }

            TextInfo textInfoNum(textInfoChar);

            if (NUMBERS_FONT_PTR_OPT)
            {
                textInfoNum.fontPtrOpt = NUMBERS_FONT_PTR_OPT;
                numberFontPtrOpt_ = NUMBERS_FONT_PTR_OPT;
            }
            else
            {
                if (numberFontPtrOpt_)
                {
                    textInfoNum.fontPtrOpt = numberFontPtrOpt_;
                }
                else
                {
                    textInfoNum.fontPtrOpt
                        = sfml_util::FontManager::Instance()->GetFont(sfml_util::Font::Number);
                }
            }

            text_render::Render(
                renderedText_,
                textInfoChar,
                textInfoNum,
                text_,
                sf::FloatRect(0, 0, textWidthLimit_, 0));

            text_render::RenderAndDraw(renderedText_, offscreenTexture_);

            sprite_.setTexture(offscreenTexture_.getTexture());

            sprite_.setTextureRect(sf::IntRect(
                0,
                0,
                static_cast<int>(renderedText_.longest_line),
                static_cast<int>(renderedText_.total_height)));

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
            const bool WILL_RESET{ E != GetMouseState() };

            GuiEntity::SetMouseState(E);

            if (WILL_RESET)
            {
                ResetText();
            }
        }

        void GuiText::ResetText()
        {
            Setup(
                text_,
                GetEntityPos().x,
                GetEntityPos().y,
                MouseTextInfo(upTextInfo_, downTextInfo_, overTextInfo_),
                textWidthLimit_,
                numberFontPtrOpt_);
        }

        bool GuiText::MouseUp(const sf::Vector2f & MOUSE_POS_V)
        {
            const bool DID_MOUSE_STATE_CHANGE{ GuiEntity::MouseUp(MOUSE_POS_V) };

            if (DID_MOUSE_STATE_CHANGE)
            {
                ResetText();
            }

            return DID_MOUSE_STATE_CHANGE;
        }

        bool GuiText::MouseDown(const sf::Vector2f & MOUSE_POS_V)
        {
            const bool DID_MOUSE_STATE_CHANGE{ GuiEntity::MouseDown(MOUSE_POS_V) };

            if (DID_MOUSE_STATE_CHANGE)
            {
                ResetText();
            }

            return DID_MOUSE_STATE_CHANGE;
        }

        bool GuiText::UpdateMousePos(const sf::Vector2f & MOUSE_POS_V)
        {
            const bool DID_MOUSE_STATE_CHANGE{ GuiEntity::UpdateMousePos(MOUSE_POS_V) };

            if (DID_MOUSE_STATE_CHANGE)
            {
                if (GetMouseState() == MouseState::Over)
                {
                    SoundManager::Instance()->PlaySfx_TickOn();
                }
                else
                {
                    SoundManager::Instance()->PlaySfx_TickOff();
                }

                ResetText();
            }

            return DID_MOUSE_STATE_CHANGE;
        }

        void GuiText::draw(sf::RenderTarget & target, sf::RenderStates states) const
        {
            target.draw(sprite_, states);
        }

        void GuiText::SetNewColors(
            const sf::Color & UP_COLOR, const sf::Color & DOWN_COLOR, const sf::Color & OVER_COLOR)
        {
            upTextInfo_.color = UP_COLOR;
            downTextInfo_.color = DOWN_COLOR;
            overTextInfo_.color = OVER_COLOR;
        }

        void GuiText::SpriteColorSet(const sf::Color & NEW_COLOR) { sprite_.setColor(NEW_COLOR); }

        void GuiText::SpriteColorReset() { sprite_.setColor(sf::Color::White); }

    } // namespace gui
} // namespace sfml_util
} // namespace heroespath
