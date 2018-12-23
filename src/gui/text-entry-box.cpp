// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// text-entry-box.cpp
//
#include "text-entry-box.hpp"

#include "gui/sound-manager.hpp"
#include "gui/text-region.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/log-macros.hpp"
#include "sfutil/display.hpp"
#include "sfutil/primitives.hpp"

namespace heroespath
{
namespace gui
{

    const float TextEntryBox::INNER_PAD_ { 3.0f };
    const float TextEntryBox::CURSOR_WIDTH_ { 0.0f };
    const float TextEntryBox::CURSOR_BLINK_DELAY_SEC_ { 0.5 };

    TextEntryBox::TextEntryBox(
        const Callback_t::IHandlerPtrOpt_t & CALLBACK_HANDLER_PTR_OPT,
        const std::string & NAME,
        const sf::FloatRect & REGION,
        const TextInfo & TEXT_INFO,
        const sf::Color & CURSOR_COLOR,
        const BoxEntityInfo & BOX_INFO)
        : Entity(std::string(NAME).append("_TextEntryBox"), REGION)
        , boxEntity_(NAME + "_TextEntryBox's", REGION, BOX_INFO)
        , textInfo_(TEXT_INFO)
        , cursorRect_()
        , cursorColor_(CURSOR_COLOR)
        , innerRegion_()
        , textRegionUPtr_()
        , willDrawCursor_(false)
        , cursorBlinkTimer_(0.0f)
        , callbackHandlerPtrOpt_(CALLBACK_HANDLER_PTR_OPT)
    {
        Setup(REGION, TEXT_INFO, CURSOR_COLOR, BOX_INFO);
    }

    TextEntryBox::~TextEntryBox() = default;

    void TextEntryBox::Setup(
        const sf::FloatRect & REGION,
        const TextInfo & TEXT_INFO,
        const sf::Color & CURSOR_COLOR,
        const BoxEntityInfo & BOX_INFO)
    {
        SetEntityRegion(REGION);

        boxEntity_.Setup(REGION, BOX_INFO);

        innerRegion_ = REGION;

        if (BOX_INFO != BoxEntityInfo())
        {
            innerRegion_ = boxEntity_.InnerRegion();
            SetEntityRegion(boxEntity_.OuterRegion());
        }

        innerRegion_.left += INNER_PAD_;
        innerRegion_.top += INNER_PAD_;
        innerRegion_.width -= (INNER_PAD_ * 2.0f);
        innerRegion_.height -= (INNER_PAD_ * 2.0f);

        cursorColor_ = CURSOR_COLOR;

        auto cursorRegion { innerRegion_ };
        cursorRegion.width = CURSOR_WIDTH_;
        cursorRect_.Rect(cursorRegion);

        textInfo_ = TEXT_INFO;

        sf::FloatRect initialTextRegion(innerRegion_);
        initialTextRegion.width = 0.0f;

        textRegionUPtr_ = std::make_unique<TextRegion>(
            std::string(GetEntityName()).append("'s"), textInfo_, initialTextRegion);
    }

    void TextEntryBox::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        target.draw(boxEntity_, states);

        textRegionUPtr_->draw(target, states);

        if (willDrawCursor_ && HasFocus())
        {
            target.draw(cursorRect_, states);
        }
    }

    void TextEntryBox::SetText(const std::string & TEXT)
    {
        textInfo_.text = TEXT;
        textRegionUPtr_->SetText(TEXT);

        const auto CURSOR_POS_LEFT { innerRegion_.left
                                     + (textRegionUPtr_->ActualTextRegion().width + 3.0f) };

        cursorRect_.SetPos(CURSOR_POS_LEFT, cursorRect_.Rect().top);

        if (callbackHandlerPtrOpt_)
        {
            Callback_t::HandleAndLog(*callbackHandlerPtrOpt_.value(), *this, "");
        }
    }

    const std::string TextEntryBox::GetText() const { return textRegionUPtr_->GetText(); }

    bool TextEntryBox::KeyPress(const sf::Event::KeyEvent & KE)
    {
        if ((KE.code != sf::Keyboard::LShift) && (KE.code != sf::Keyboard::RShift))
        {
            SoundManager::Instance()->PlaySfx_Keypress();
        }

        return true;
    }

    bool TextEntryBox::KeyRelease(const sf::Event::KeyEvent & KEY_EVENT)
    {
        std::string text { GetText() };

        if ((KEY_EVENT.code == sf::Keyboard::Left) || (KEY_EVENT.code == sf::Keyboard::BackSpace))
        {
            if (text.empty() == false)
            {
                text.pop_back();
                SetText(text);
            }

            return true;
        }

        const auto CHAR { KeyToCharacter(KEY_EVENT.code, KEY_EVENT.shift) };

        if (CHAR == 0)
        {
            return false;
        }

        text += CHAR;
        SetText(text);

        const auto IS_TOO_LONG { (textRegionUPtr_->ActualTextRegion().width > innerRegion_.width) };

        if (IS_TOO_LONG)
        {
            text.pop_back();
            SetText(text);
            return false;
        }

        return true;
    }

    bool TextEntryBox::UpdateTime(const float ELAPSED_TIME_SEC)
    {
        cursorBlinkTimer_ += ELAPSED_TIME_SEC;

        if (cursorBlinkTimer_ > CURSOR_BLINK_DELAY_SEC_)
        {
            cursorBlinkTimer_ = 0.0f;
            willDrawCursor_ = !willDrawCursor_;
            return true;
        }
        else
        {
            return false;
        }
    }

    void TextEntryBox::OnColorChange() { boxEntity_.SetEntityColors(entityFocusColors_); }

    void TextEntryBox::SetTextColor(const sf::Color & TEXT_COLOR)
    {
        cursorColor_ = TEXT_COLOR;
        textInfo_.color = TEXT_COLOR;
        textRegionUPtr_->SetEntityColorFg(TEXT_COLOR);
    }

    void TextEntryBox::SetEntityPos(const float POS_LEFT, const float POS_TOP)
    {
        const auto DIFF_V { sf::Vector2f(POS_LEFT, POS_TOP) - GetEntityPos() };
        MoveEntityPos(DIFF_V.x, DIFF_V.y);
    }

    void TextEntryBox::MoveEntityPos(const float HORIZ, const float VERT)
    {
        Entity::MoveEntityPos(HORIZ, VERT);

        boxEntity_.MoveEntityPos(HORIZ, VERT);
        cursorRect_.MovePos(HORIZ, VERT);
        sfutil::Move(innerRegion_, sf::Vector2f(HORIZ, VERT));
        textRegionUPtr_->MoveEntityPos(HORIZ, VERT);
    }

    char TextEntryBox::KeyToCharacter(
        const sf::Keyboard::Key KEY_CODE, const bool IS_SHIFT_DOWN) const
    {
        char character = [KEY_CODE]() {
            switch (KEY_CODE)
            {
                case sf::Keyboard::A:
                {
                    return 'a';
                }
                case sf::Keyboard::B:
                {
                    return 'b';
                }
                case sf::Keyboard::C:
                {
                    return 'c';
                }
                case sf::Keyboard::D:
                {
                    return 'd';
                }
                case sf::Keyboard::E:
                {
                    return 'e';
                }
                case sf::Keyboard::F:
                {
                    return 'f';
                }
                case sf::Keyboard::G:
                {
                    return 'g';
                }
                case sf::Keyboard::H:
                {
                    return 'h';
                }
                case sf::Keyboard::I:
                {
                    return 'i';
                }
                case sf::Keyboard::J:
                {
                    return 'j';
                }
                case sf::Keyboard::K:
                {
                    return 'k';
                }
                case sf::Keyboard::L:
                {
                    return 'l';
                }
                case sf::Keyboard::M:
                {
                    return 'm';
                }
                case sf::Keyboard::N:
                {
                    return 'n';
                }
                case sf::Keyboard::O:
                {
                    return 'o';
                }
                case sf::Keyboard::P:
                {
                    return 'p';
                }
                case sf::Keyboard::Q:
                {
                    return 'q';
                }
                case sf::Keyboard::R:
                {
                    return 'r';
                }
                case sf::Keyboard::S:
                {
                    return 's';
                }
                case sf::Keyboard::T:
                {
                    return 't';
                }
                case sf::Keyboard::U:
                {
                    return 'u';
                }
                case sf::Keyboard::V:
                {
                    return 'v';
                }
                case sf::Keyboard::W:
                {
                    return 'w';
                }
                case sf::Keyboard::X:
                {
                    return 'x';
                }
                case sf::Keyboard::Y:
                {
                    return 'y';
                }
                case sf::Keyboard::Z:
                {
                    return 'z';
                }
                case sf::Keyboard::Num0:
                {
                    return '0';
                }
                case sf::Keyboard::Num1:
                {
                    return '1';
                }
                case sf::Keyboard::Num2:
                {
                    return '2';
                }
                case sf::Keyboard::Num3:
                {
                    return '3';
                }
                case sf::Keyboard::Num4:
                {
                    return '4';
                }
                case sf::Keyboard::Num5:
                {
                    return '5';
                }
                case sf::Keyboard::Num6:
                {
                    return '6';
                }
                case sf::Keyboard::Num7:
                {
                    return '7';
                }
                case sf::Keyboard::Num8:
                {
                    return '8';
                }
                case sf::Keyboard::Num9:
                {
                    return '9';
                }
                case sf::Keyboard::Dash:
                {
                    return '-';
                }
                case sf::Keyboard::Period:
                {
                    return '.';
                }
                case sf::Keyboard::Right:
                case sf::Keyboard::Space:
                {
                    return ' ';
                }
                case sf::Keyboard::Multiply:
                {
                    return '*';
                }
                case sf::Keyboard::Comma:
                {
                    return ',';
                }
                case sf::Keyboard::Quote:
                {
                    return '\'';
                }
                case sf::Keyboard::SemiColon:
                {
                    return ':';
                }
                case sf::Keyboard::Slash:
                case sf::Keyboard::Left:
                case sf::Keyboard::BackSpace:
                case sf::Keyboard::Escape:
                case sf::Keyboard::Unknown:
                case sf::Keyboard::LControl:
                case sf::Keyboard::LShift:
                case sf::Keyboard::LSystem:
                case sf::Keyboard::LAlt:
                case sf::Keyboard::RControl:
                case sf::Keyboard::RShift:
                case sf::Keyboard::RSystem:
                case sf::Keyboard::RAlt:
                case sf::Keyboard::LBracket:
                case sf::Keyboard::RBracket:
                case sf::Keyboard::Menu:
                case sf::Keyboard::BackSlash:
                case sf::Keyboard::Tilde:
                case sf::Keyboard::Equal:
                case sf::Keyboard::Return:
                case sf::Keyboard::Tab:
                case sf::Keyboard::PageUp:
                case sf::Keyboard::PageDown:
                case sf::Keyboard::End:
                case sf::Keyboard::Home:
                case sf::Keyboard::Insert:
                case sf::Keyboard::Delete:
                case sf::Keyboard::Add:
                case sf::Keyboard::Subtract:
                case sf::Keyboard::Divide:
                case sf::Keyboard::Up:
                case sf::Keyboard::Down:
                case sf::Keyboard::Numpad0:
                case sf::Keyboard::Numpad1:
                case sf::Keyboard::Numpad2:
                case sf::Keyboard::Numpad3:
                case sf::Keyboard::Numpad4:
                case sf::Keyboard::Numpad5:
                case sf::Keyboard::Numpad6:
                case sf::Keyboard::Numpad7:
                case sf::Keyboard::Numpad8:
                case sf::Keyboard::Numpad9:
                case sf::Keyboard::F1:
                case sf::Keyboard::F2:
                case sf::Keyboard::F3:
                case sf::Keyboard::F4:
                case sf::Keyboard::F5:
                case sf::Keyboard::F6:
                case sf::Keyboard::F7:
                case sf::Keyboard::F8:
                case sf::Keyboard::F9:
                case sf::Keyboard::F10:
                case sf::Keyboard::F11:
                case sf::Keyboard::F12:
                case sf::Keyboard::F13:
                case sf::Keyboard::F14:
                case sf::Keyboard::F15:
                case sf::Keyboard::Pause:
                case sf::Keyboard::KeyCount:
                default:
                {
                    return char(0);
                }
            }
        }();

        if (IS_SHIFT_DOWN)
        {
            misc::ToUpper(character);
        }

        return character;
    }

} // namespace gui
} // namespace heroespath
