// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
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

            sf::Sprite & GetUpSprite() { return upSprite_; }
            sf::Sprite & GetDownSprite() { return downSprite_; }
            sf::Sprite & GetOverSprite() { return overSprite_; }

            void SetEntityPos(const float POS_LEFT, const float POS_TOP) override;
            void MoveEntityPos(const float HORIZ, const float VERT) override;

        protected:
            void OnClick(const sf::Vector2f &) override {}

            sf::Sprite upSprite_;
            sf::Sprite downSprite_;
            sf::Sprite overSprite_;
        };

        using GuiImageUPtr_t = std::unique_ptr<GuiImage>;
    } // namespace gui
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_GUIIMAGE_HPP_INCLUDED
