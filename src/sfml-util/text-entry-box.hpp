// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_TEXTENTRYBOX_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_TEXTENTRYBOX_HPP_INCLUDED
//
// text-entry-box.hpp
//
#include "misc/not-null.hpp"
#include "sfml-util/box-entity-info.hpp"
#include "sfml-util/box-entity.hpp"
#include "sfml-util/callback.hpp"
#include "sfml-util/colored-rect.hpp"
#include "sfml-util/entity.hpp"
#include "sfml-util/text-info.hpp"

#include <memory>
#include <string>

namespace heroespath
{
namespace sfml_util
{

    class TextRegion;
    using TextRegionUPtr_t = std::unique_ptr<TextRegion>;

    // Encapsulates a single line of user type-able text with a blinking cursor
    class TextEntryBox : public Entity
    {
    public:
        using Callback_t = Callback<TextEntryBox>;

        TextEntryBox(const TextEntryBox &) = delete;
        TextEntryBox(TextEntryBox &&) = delete;
        TextEntryBox & operator=(const TextEntryBox &) = delete;
        TextEntryBox & operator=(TextEntryBox &&) = delete;

        TextEntryBox(
            const Callback_t::IHandlerPtrOpt_t & CALLBACK_HANDLER_PTR_OPT,
            const std::string & NAME,
            const sf::FloatRect & REGION,
            const TextInfo & TEXT_INFO,
            const sf::Color & CURSOR_COLOR = sf::Color::White,
            const BoxEntityInfo & BOX_INFO = BoxEntityInfo());

        virtual ~TextEntryBox();

        void Setup(
            const sf::FloatRect & REGION,
            const TextInfo & TEXT_INFO,
            const sf::Color & CURSOR_COLOR = sf::Color::White,
            const BoxEntityInfo & BOX_INFO = BoxEntityInfo());

        void draw(sf::RenderTarget & target, sf::RenderStates states) const override;

        void SetText(const std::string & NEW_TEXT);
        const std::string GetText() const;

        bool KeyPress(const sf::Event::KeyEvent &) override;
        bool KeyRelease(const sf::Event::KeyEvent &) override;

        bool UpdateTime(const float) override;

        void SetTextColor(const sf::Color & TEXT_COLOR);

    protected:
        void SetEntityRegion(const sf::FloatRect & R) override { Entity::SetEntityRegion(R); }

    private:
        void UpdateText();
        void OnColorChange() override;

        static const float INNER_PAD_;
        static const float CURSOR_WIDTH_;
        static const float CURSOR_BLINK_DELAY_SEC_;
        //
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

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_TEXTENTRYBOX_HPP_INCLUDED
