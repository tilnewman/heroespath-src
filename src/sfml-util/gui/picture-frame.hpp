// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_GUI_PICTUREFRAME_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_GUI_PICTUREFRAME_HPP_INCLUDED
//
// picture-frame.hpp
//
#include "sfml-util/sfml-graphics.hpp"

#include <memory>
#include <vector>

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        // Responsible for drawing a pciture frame border in a given region.
        class PictureFrame : public sf::Drawable

        {
        public:
            PictureFrame(const PictureFrame &) = delete;
            PictureFrame(PictureFrame &&) = delete;
            PictureFrame & operator=(const PictureFrame &) = delete;
            PictureFrame & operator=(PictureFrame &&) = delete;

        public:
            explicit PictureFrame(
                const sf::FloatRect & RECT = sf::FloatRect(0.0f, 0.0f, 0.0f, 0.0f),
                const sf::Color & COLOR = sf::Color::White);

            const sf::FloatRect OuterRect() const { return outerRect_; }
            const sf::FloatRect InnerRect() const { return innerRect_; }

            void draw(sf::RenderTarget &, sf::RenderStates) const override;

            // returns the innerRect_
            const sf::FloatRect Setup(const sf::FloatRect & RECT, const sf::Color & COLOR);

        private:
            sf::Color color_;
            sf::FloatRect outerRect_;
            sf::FloatRect innerRect_;
            sf::Texture texture_;
            sf::Sprite topLeftSprite_;
            sf::Sprite topRightSprite_;
            sf::Sprite botLeftSprite_;
            sf::Sprite botRightSprite_;
            std::vector<sf::Sprite> sideSprites_;
        };

        using PictureFrameUPtr_t = std::unique_ptr<PictureFrame>;
    } // namespace gui
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_GUI_PICTUREFRAME_HPP_INCLUDED
