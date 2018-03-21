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
#ifndef HEROESPATH_SFMLUTIL_GUI_TEXTENTRYBOX_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_GUI_TEXTENTRYBOX_HPP_INCLUDED
//
// text-entry-box.hpp
//  Code for a box that users can type a single line of text into, with a blinking cursor, etc.
//
#include "sfml-util/gui/box-info.hpp"
#include "sfml-util/gui/box.hpp"
#include "sfml-util/gui/gui-entity.hpp"
#include "sfml-util/gui/text-info.hpp"
#include "sfml-util/i-callback-handler.hpp"
#include "sfml-util/sfml-graphics.hpp"

#include <memory>
#include <string>

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        class TextRegion;
        using TextRegionUPtr_t = std::unique_ptr<TextRegion>;

        class TextEntryBox;
        namespace callback
        {
            using TextEntryBoxCallbackPackage_t = sfml_util::callback::PtrWrapper<TextEntryBox>;
            using ITextEntryBoxCallbackHandler_t
                = sfml_util::callback::ICallbackHandler<TextEntryBoxCallbackPackage_t, bool>;
        } // namespace callback

        // Encapsulates a single line of user type-able text with a blinking cursor
        class TextEntryBox : public GuiEntity
        {
        public:
            TextEntryBox(const TextEntryBox &) = delete;
            TextEntryBox(TextEntryBox &&) = delete;
            TextEntryBox & operator=(const TextEntryBox &) = delete;
            TextEntryBox & operator=(TextEntryBox &&) = delete;

        public:
            // if using this constructor, Setup() must be called before any other function
            explicit TextEntryBox(const std::string & NAME);

            TextEntryBox(
                const std::string & NAME,
                const sf::FloatRect & REGION,
                const TextInfo & TEXT_INFO,
                const sf::Color & CURSOR_COLOR = sf::Color::White,
                const box::Info & BOX_INFO = box::Info(),
                callback::ITextEntryBoxCallbackHandler_t * const CHANGE_HANDLER_PTR = nullptr);

            virtual ~TextEntryBox();

            void Setup(
                const sf::FloatRect & REGION,
                const TextInfo & TEXT_INFO,
                const box::Info & BOX_INFO = box::Info());

            virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const;

            virtual void SetText(const std::string & NEW_TEXT);
            virtual const std::string GetText() const;

            virtual bool KeyPress(const sf::Event::KeyEvent &);
            virtual bool KeyRelease(const sf::Event::KeyEvent &);

            virtual bool UpdateTime(const float);

            virtual void SetTextColor(const sf::Color & TEXT_COLOR);

        protected:
            void UpdateText();
            virtual void OnClick(const sf::Vector2f &) {}
            virtual void OnColorChange();

        protected:
            static const float INNER_PAD_;
            static const float CURSOR_WIDTH_;
            static const float CURSOR_BLINK_DELAY_SEC_;
            //
            box::Box box_;
            TextInfo textInfo_;
            sf::FloatRect cursorRect_;
            sf::Color cursorColor_;
            sf::FloatRect innerRegion_;
            TextRegionUPtr_t textRegionUPtr_;
            bool willDrawCursor_;
            float cursorBlinkTimer_;
            callback::ITextEntryBoxCallbackHandler_t * callbackHandlerPtr_;
        };

        using TextEntryBoxUPtr_t = std::unique_ptr<TextEntryBox>;
    } // namespace gui
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_GUI_TEXTENTRYBOX_HPP_INCLUDED
