// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_TEXTENTRYBOX_HPP_INCLUDED
#define HEROESPATH_GUI_TEXTENTRYBOX_HPP_INCLUDED
//
// text-entry-box.hpp
//
#include "gui/box-entity-info.hpp"
#include "gui/box-entity.hpp"
#include "gui/colored-rect.hpp"
#include "gui/entity.hpp"
#include "gui/text-info.hpp"
#include "misc/callback.hpp"
#include "misc/not-null.hpp"

#include <memory>
#include <string>

namespace heroespath
{
namespace gui
{

    class TextRegion;
    using TextRegionUPtr_t = std::unique_ptr<TextRegion>;

    // Encapsulates a single line of user type-able text with a blinking cursor
    class TextEntryBox : public Entity
    {
    public:
        using Callback_t = misc::Callback<TextEntryBox>;

        TextEntryBox(
            const Callback_t::IHandlerPtrOpt_t & CALLBACK_HANDLER_PTR_OPT,
            const std::string & NAME,
            const sf::FloatRect & REGION,
            const TextInfo & TEXT_INFO,
            const sf::Color & CURSOR_COLOR = sf::Color::White,
            const BoxEntityInfo & BOX_INFO = BoxEntityInfo());

        virtual ~TextEntryBox();

        TextEntryBox(const TextEntryBox &) = delete;
        TextEntryBox(TextEntryBox &&) = delete;
        TextEntryBox & operator=(const TextEntryBox &) = delete;
        TextEntryBox & operator=(TextEntryBox &&) = delete;

        void Setup(
            const sf::FloatRect & REGION,
            const TextInfo & TEXT_INFO,
            const sf::Color & CURSOR_COLOR = sf::Color::White,
            const BoxEntityInfo & BOX_INFO = BoxEntityInfo());

        void draw(sf::RenderTarget & target, sf::RenderStates states) const override;

        void SetText(const std::string & TEXT);
        const std::string GetText() const;

        bool KeyPress(const sf::Event::KeyEvent &) override;
        bool KeyRelease(const sf::Event::KeyEvent &) override;

        bool UpdateTime(const float) override;

        void SetTextColor(const sf::Color & TEXT_COLOR);

        void SetEntityPos(const float POS_LEFT, const float POS_TOP) override;
        void MoveEntityPos(const float HORIZ, const float VERT) override;

    protected:
        void SetEntityRegion(const sf::FloatRect & R) override { Entity::SetEntityRegion(R); }

    private:
        void OnColorChange() override;
        char KeyToCharacter(const sf::Keyboard::Key KEY_CODE, const bool IS_SHIFT_DOWN) const;

        static const float INNER_PAD_;
        static const float CURSOR_WIDTH_;
        static const float CURSOR_BLINK_DELAY_SEC_;

        BoxEntity boxEntity_;
        TextInfo textInfo_;
        ColoredRect cursorRect_;
        sf::Color cursorColor_;
        sf::FloatRect innerRegion_;
        TextRegionUPtr_t textRegionUPtr_;
        bool willDrawCursor_;
        float cursorBlinkTimer_;
        Callback_t::IHandlerPtrOpt_t callbackHandlerPtrOpt_;
    };

    using TextEntryBoxUPtr_t = std::unique_ptr<TextEntryBox>;

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_TEXTENTRYBOX_HPP_INCLUDED
