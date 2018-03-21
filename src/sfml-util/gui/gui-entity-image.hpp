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
#ifndef HEROESPATH_SFMLUTIL_GUIIMAGE_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_GUIIMAGE_HPP_INCLUDED
//
// gui-entity-image.hpp
//
#include "sfml-util/gui/gui-entity.hpp"
#include "sfml-util/sfml-graphics.hpp"

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        // Encapsulates a GuiEntity that manages three images based on entityMouseState_.
        class GuiImage : public GuiEntity
        {
        public:
            GuiImage(
                const std::string & NAME,
                const sf::FloatRect & SCREEN_REGION,
                const sf::Sprite & SPRITE_UP = sf::Sprite(),
                const sf::Sprite & SPRITE_DOWN = sf::Sprite(),
                const sf::Sprite & SPRITE_OVER = sf::Sprite());

            GuiImage(
                const std::string & NAME,
                const float POS_LEFT,
                const float POS_TOP,
                const sf::Sprite & SPRITE_UP = sf::Sprite(),
                const sf::Sprite & SPRITE_DOWN = sf::Sprite(),
                const sf::Sprite & SPRITE_OVER = sf::Sprite());

            virtual ~GuiImage();

            virtual void Setup(const float POS_LEFT, const float POS_TOP);

            // The only member required by sf::Drawable.
            void draw(sf::RenderTarget & target, sf::RenderStates states) const override;

            virtual void SpriteColorsSet(const sf::Color & NEW_COLOR);
            virtual void SpriteColorsReset();

            inline sf::Sprite & GetUpSprite() { return upSprite_; }
            inline sf::Sprite & GetDownSprite() { return downSprite_; }
            inline sf::Sprite & GetOverSprite() { return overSprite_; }

            void SetEntityPos(const float POS_LEFT, const float POS_TOP) override;
            void MoveEntityPos(const float HORIZ, const float VERT) override;

        protected:
            inline void OnClick(const sf::Vector2f &) override {}

            sf::Sprite upSprite_;
            sf::Sprite downSprite_;
            sf::Sprite overSprite_;
        };

        using GuiImageUPtr_t = std::unique_ptr<GuiImage>;
    } // namespace gui
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_GUIIMAGE_HPP_INCLUDED
