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
#ifndef HEROESPATH_SFMLUTIL_GUITEXT_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_GUITEXT_HPP_INCLUDED
//
// gui-entity-text.hpp
//  Code for handling a piece of text that can be in one of three states.
//
#include "sfml-util/gui/gui-entity.hpp"
#include "sfml-util/gui/mouse-text-info.hpp"
#include "sfml-util/gui/text-info.hpp"
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/text-rendering.hpp"

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        // Base class for gui text that can be in any of MouseState::Enum states.
        class GuiText : public GuiEntity
        {
        public:
            GuiText(const GuiText &) = delete;
            GuiText(const GuiText &&) = delete;
            GuiText & operator=(const GuiText &) = delete;
            GuiText & operator=(const GuiText &&) = delete;

        public:
            // if this constructor is used, then Setup() must be called before any other functions
            explicit GuiText(
                const std::string & NAME,
                const float TEXT_WIDTH_LIMIT = 0.0f,
                const FontPtr_t NUMBERS_FONT_PTR = nullptr);

            GuiText(
                const std::string & NAME,
                const sf::FloatRect & REGION,
                const MouseTextInfo & MOUSE_TEXT_INFO,
                const float TEXT_WIDTH_LIMIT = 0.0f,
                const FontPtr_t NUMBERS_FONT_PTR = nullptr);

            GuiText(
                const std::string & NAME,
                const float POS_LEFT,
                const float POS_TOP,
                const MouseTextInfo & MOUSE_TEXT_INFO,
                const float TEXT_WIDTH_LIMIT = 0.0f,
                const FontPtr_t NUMBERS_FONT_PTR = nullptr);

            virtual ~GuiText();

            virtual void SetMouseState(const MouseState::Enum E);

            virtual bool MouseUp(const sf::Vector2f & MOUSE_POS_V);
            virtual bool MouseDown(const sf::Vector2f & MOUSE_POS_V);

            virtual bool UpdateMousePos(const sf::Vector2f & MOUSE_POS_V);

            virtual const std::string GetText() const { return text_; }

            virtual float GetTextWidth() const { return sprite_.getLocalBounds().width; }
            virtual float GetTextHeight() const { return sprite_.getLocalBounds().height; }

            virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const;

            virtual void SetNewColors(
                const sf::Color & UP_COLOR,
                const sf::Color & DOWN_COLOR,
                const sf::Color & OVER_COLOR);

            virtual void SpriteColorSet(const sf::Color & NEW_COLOR);
            virtual void SpriteColorReset();

            void Setup(
                const std::string & TEXT,
                const float POS_LEFT,
                const float POS_TOP,
                const MouseTextInfo & MOUSE_TEXT_INFO,
                const float TEXT_WIDTH_LIMIT = 0.0f,
                const FontPtr_t NUMBERS_FONT_PTR = nullptr);

            virtual void SetEntityPos(const float POS_LEFT, const float POS_TOP);
            virtual void MoveEntityPos(const float HORIZ, const float VERT);

        protected:
            virtual void ResetText();
            virtual void OnClick(const sf::Vector2f &) {}

        protected:
            std::string text_;
            TextInfo upTextInfo_;
            TextInfo downTextInfo_;
            TextInfo overTextInfo_;
            FontPtr_t numberFontPtr_;
            sf::RenderTexture offscreenTexture_;
            sf::Sprite sprite_;
            float textWidthLimit_;
            text_render::RenderedText renderedText_;
        };

        using GuiTextSPtr_t = std::shared_ptr<GuiText>;
        using GuiTextSVec_t = std::vector<GuiTextSPtr_t>;
    } // namespace gui
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_GUITEXT_HPP_INCLUDED
