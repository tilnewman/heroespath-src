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
#include "sfutil/keyboard.hpp"
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

        const auto CHAR { sfutil::sfKeyToNameValidCharacterNameCharOrZero(
            KEY_EVENT.code, KEY_EVENT.shift) };

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

} // namespace gui
} // namespace heroespath
