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
#include "sfutil/primitives.hpp"

namespace heroespath
{
namespace gui
{

    const float TextEntryBox::INNER_PAD_(2.0f);
    const float TextEntryBox::CURSOR_WIDTH_(0.0f);
    const float TextEntryBox::CURSOR_BLINK_DELAY_SEC_(0.5);

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
        cursorColor_ = CURSOR_COLOR;
        SetEntityRegion(REGION);

        innerRegion_ = REGION;
        innerRegion_.left += INNER_PAD_;
        innerRegion_.top += INNER_PAD_;
        innerRegion_.width -= (INNER_PAD_ * 2.0f);
        innerRegion_.height -= (INNER_PAD_ * 2.0f);

        auto cursorRegion { innerRegion_ };
        cursorRegion.width = CURSOR_WIDTH_;
        cursorRect_.Rect(cursorRegion);

        textInfo_ = TEXT_INFO;

        boxEntity_.Setup(boxEntity_.GetEntityRegion(), BOX_INFO);
    }

    void TextEntryBox::UpdateText()
    {
        if (textInfo_.text.empty() == false)
        {
            sf::FloatRect r(innerRegion_);
            r.width = 0.0f;

            textRegionUPtr_ = std::make_unique<TextRegion>(
                std::string(GetEntityName()).append("'s"),
                textInfo_,
                r,
                TextRegion::DEFAULT_NO_RESIZE_,
                BoxEntityInfo());

            cursorRect_.SetPos(
                innerRegion_.left + textRegionUPtr_->GetEntityRegion().width + 3.0f,
                cursorRect_.Rect().top);
        }
        else
        {
            textRegionUPtr_.reset();
        }

        if (callbackHandlerPtrOpt_)
        {
            callbackHandlerPtrOpt_.value()->HandleCallback(this);
        }
    }

    void TextEntryBox::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        target.draw(boxEntity_, states);

        if (textRegionUPtr_)
        {
            textRegionUPtr_->draw(target, states);
        }

        if (willDrawCursor_ && HasFocus())
        {
            target.draw(cursorRect_, states);
        }
    }

    void TextEntryBox::SetText(const std::string & NEW_TEXT)
    {
        textInfo_.text = NEW_TEXT;
        UpdateText();

        if (!textRegionUPtr_)
        {
            cursorRect_.SetPos(innerRegion_.left, cursorRect_.Rect().top);
        }
        else
        {
            cursorRect_.SetPos(
                innerRegion_.left + textRegionUPtr_->GetEntityRegion().width,
                cursorRect_.Rect().top);
        }
    }

    const std::string TextEntryBox::GetText() const
    {
        if (!textRegionUPtr_)
        {
            return "";
        }
        else
        {
            return textRegionUPtr_->GetText();
        }
    }

    bool TextEntryBox::KeyPress(const sf::Event::KeyEvent & KE)
    {
        if ((KE.code != sf::Keyboard::LShift) && (KE.code != sf::Keyboard::RShift))
        {
            SoundManager::Instance()->PlaySfx_Keypress();
        }

        return true;
    }

    bool TextEntryBox::KeyRelease(const sf::Event::KeyEvent & KE)
    {
        char charToAppend(0);
        switch (KE.code)
        {
            case sf::Keyboard::A:
            {
                if (KE.shift)
                    charToAppend = 'A';
                else
                    charToAppend = 'a';
                break;
            }
            case sf::Keyboard::B:
            {
                if (KE.shift)
                    charToAppend = 'B';
                else
                    charToAppend = 'b';
                break;
            }
            case sf::Keyboard::C:
            {
                if (KE.shift)
                    charToAppend = 'C';
                else
                    charToAppend = 'c';
                break;
            }
            case sf::Keyboard::D:
            {
                if (KE.shift)
                    charToAppend = 'D';
                else
                    charToAppend = 'd';
                break;
            }
            case sf::Keyboard::E:
            {
                if (KE.shift)
                    charToAppend = 'E';
                else
                    charToAppend = 'e';
                break;
            }
            case sf::Keyboard::F:
            {
                if (KE.shift)
                    charToAppend = 'F';
                else
                    charToAppend = 'f';
                break;
            }
            case sf::Keyboard::G:
            {
                if (KE.shift)
                    charToAppend = 'G';
                else
                    charToAppend = 'g';
                break;
            }
            case sf::Keyboard::H:
            {
                if (KE.shift)
                    charToAppend = 'H';
                else
                    charToAppend = 'h';
                break;
            }
            case sf::Keyboard::I:
            {
                if (KE.shift)
                    charToAppend = 'I';
                else
                    charToAppend = 'i';
                break;
            }
            case sf::Keyboard::J:
            {
                if (KE.shift)
                    charToAppend = 'J';
                else
                    charToAppend = 'j';
                break;
            }
            case sf::Keyboard::K:
            {
                if (KE.shift)
                    charToAppend = 'K';
                else
                    charToAppend = 'k';
                break;
            }
            case sf::Keyboard::L:
            {
                if (KE.shift)
                    charToAppend = 'L';
                else
                    charToAppend = 'l';
                break;
            }
            case sf::Keyboard::M:
            {
                if (KE.shift)
                    charToAppend = 'M';
                else
                    charToAppend = 'm';
                break;
            }
            case sf::Keyboard::N:
            {
                if (KE.shift)
                    charToAppend = 'N';
                else
                    charToAppend = 'n';
                break;
            }
            case sf::Keyboard::O:
            {
                if (KE.shift)
                    charToAppend = 'O';
                else
                    charToAppend = 'o';
                break;
            }
            case sf::Keyboard::P:
            {
                if (KE.shift)
                    charToAppend = 'P';
                else
                    charToAppend = 'p';
                break;
            }
            case sf::Keyboard::Q:
            {
                if (KE.shift)
                    charToAppend = 'Q';
                else
                    charToAppend = 'q';
                break;
            }
            case sf::Keyboard::R:
            {
                if (KE.shift)
                    charToAppend = 'R';
                else
                    charToAppend = 'r';
                break;
            }
            case sf::Keyboard::S:
            {
                if (KE.shift)
                    charToAppend = 'S';
                else
                    charToAppend = 's';
                break;
            }
            case sf::Keyboard::T:
            {
                if (KE.shift)
                    charToAppend = 'T';
                else
                    charToAppend = 't';
                break;
            }
            case sf::Keyboard::U:
            {
                if (KE.shift)
                    charToAppend = 'U';
                else
                    charToAppend = 'u';
                break;
            }
            case sf::Keyboard::V:
            {
                if (KE.shift)
                    charToAppend = 'V';
                else
                    charToAppend = 'v';
                break;
            }
            case sf::Keyboard::W:
            {
                if (KE.shift)
                    charToAppend = 'W';
                else
                    charToAppend = 'w';
                break;
            }
            case sf::Keyboard::X:
            {
                if (KE.shift)
                    charToAppend = 'X';
                else
                    charToAppend = 'x';
                break;
            }
            case sf::Keyboard::Y:
            {
                if (KE.shift)
                    charToAppend = 'Y';
                else
                    charToAppend = 'y';
                break;
            }
            case sf::Keyboard::Z:
            {
                if (KE.shift)
                    charToAppend = 'Z';
                else
                    charToAppend = 'z';
                break;
            }
            case sf::Keyboard::Num0:
            {
                charToAppend = '0';
                break;
            }
            case sf::Keyboard::Num1:
            {
                charToAppend = '1';
                break;
            }
            case sf::Keyboard::Num2:
            {
                charToAppend = '2';
                break;
            }
            case sf::Keyboard::Num3:
            {
                charToAppend = '3';
                break;
            }
            case sf::Keyboard::Num4:
            {
                charToAppend = '4';
                break;
            }
            case sf::Keyboard::Num5:
            {
                charToAppend = '5';
                break;
            }
            case sf::Keyboard::Num6:
            {
                charToAppend = '6';
                break;
            }
            case sf::Keyboard::Num7:
            {
                charToAppend = '7';
                break;
            }
            case sf::Keyboard::Num8:
            {
                charToAppend = '8';
                break;
            }
            case sf::Keyboard::Num9:
            {
                charToAppend = '9';
                break;
            }
            case sf::Keyboard::Dash:
            {
                if (KE.shift)
                    charToAppend = '_';
                else
                    charToAppend = '-';
                break;
            }
            case sf::Keyboard::Period:
            {
                if (KE.shift)
                    charToAppend = '>';
                else
                    charToAppend = '.';
                break;
            }
            case sf::Keyboard::Right:
            case sf::Keyboard::Space:
            {
                charToAppend = ' ';
                break;
            }
            case sf::Keyboard::Left:
            case sf::Keyboard::BackSpace:
            {
                if (textInfo_.text.empty() == false)
                    textInfo_.text.pop_back();

                UpdateText();

                break;
            }
            case sf::Keyboard::Multiply:
            {
                charToAppend = '*';
                break;
            }
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
            {
                break;
            }
            case sf::Keyboard::Comma:
            {
                if (KE.shift)
                    charToAppend = '<';
                else
                    charToAppend = ',';
                break;
            }
            case sf::Keyboard::Quote:
            {
                if (KE.shift)
                    charToAppend = '\"';
                else
                    charToAppend = '\'';
                break;
            }
            case sf::Keyboard::Slash:
            {
                break;
            }
            case sf::Keyboard::SemiColon:
            {
                charToAppend = ':';
                break;
            }
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
                charToAppend = 0;
                break;
            }
        }

        bool isWithinBounds(true);
        if (textRegionUPtr_)
        {
            if (false == textRegionUPtr_->GetText().empty())
            {
                if ((textRegionUPtr_->GetEntityRegion().width > (innerRegion_.width - 30.0f)))
                {
                    isWithinBounds = false;
                }
            }
        }

        if ((charToAppend != 0) && isWithinBounds)
        {
            textInfo_.text = textInfo_.text.append(1, charToAppend);
            UpdateText();
            return true;
        }
        else
        {
            return false;
        }
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
        UpdateText();
    }

} // namespace gui
} // namespace heroespath
