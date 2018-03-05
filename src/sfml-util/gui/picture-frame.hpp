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
            PictureFrame(const PictureFrame &) = delete;
            PictureFrame & operator=(const PictureFrame &) = delete;

        public:
            PictureFrame(
                const sf::FloatRect & RECT = sf::FloatRect(0.0f, 0.0f, 0.0f, 0.0f),
                const sf::Color & COLOR = sf::Color::White);

            inline const sf::FloatRect OuterRect() const { return outerRect_; }
            inline const sf::FloatRect InnerRect() const { return innerRect_; }

            virtual void draw(sf::RenderTarget &, sf::RenderStates) const override;

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
