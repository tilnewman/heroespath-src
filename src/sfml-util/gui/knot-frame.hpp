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
#ifndef HEROESPATH_SFMLUTIL_GUI_KNOTFRAME_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_GUI_KNOTFRAME_HPP_INCLUDED
//
// knot-frame.hpp
//
#include "sfml-util/sfml-graphics.hpp"

#include <memory>

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        // Responsible for drawing a celtic knot border around a given region.
        // OuterSize() is the width of the frame border, while InnerSize() is the rest of
        // the image width.
        class KnotFrame : public sf::Drawable
        {
            KnotFrame(const KnotFrame &) = delete;
            KnotFrame(const KnotFrame &&) = delete;
            KnotFrame & operator=(const KnotFrame &) = delete;
            KnotFrame & operator=(const KnotFrame &&) = delete;

        public:
            // If using this constructor, calling Setup(...) is required before drawing.
            KnotFrame();

            // The INNER_REGION is the region to be framed, not the outer limit of the frame.
            KnotFrame(
                const sf::FloatRect & INNER_REGION, const float SIZE, const sf::Color & COLOR);

            void Setup(
                const sf::FloatRect & INNER_REGION, const float SIZE, const sf::Color & COLOR);

            inline const sf::FloatRect RegionInner() const { return regionInner_; }
            inline void RegionInner(const sf::FloatRect & R) { Setup(R, size_, color_); }

            inline const sf::FloatRect RegionOuter() const { return regionOuter_; }
            inline const sf::FloatRect Region() const { return RegionOuter(); }

            inline const sf::Color Color() const { return color_; }
            inline void Color(const sf::Color & C) { Setup(regionInner_, size_, C); }

            inline float Size() const { return size_; }
            inline void Size(const float S) { Setup(regionInner_, S, color_); }

            inline float InnerSize() const
            {
                return topLeftSprite_.getGlobalBounds().width * INNER_WIDTH_RATIO_;
            }

            inline float OuterSize() const
            {
                return topLeftSprite_.getGlobalBounds().width * OUTER_WIDTH_RATIO_;
            }

            inline float FrameSize() const
            {
                return topLeftSprite_.getGlobalBounds().width * FRAME_WIDTH_RATIO_;
            }

            virtual void draw(sf::RenderTarget &, sf::RenderStates) const override;

        private:
            static const float OUTER_WIDTH_RATIO_;
            static const float INNER_WIDTH_RATIO_;
            static const float FRAME_WIDTH_RATIO_;
            sf::FloatRect regionInner_;
            sf::FloatRect regionOuter_;
            float size_;
            sf::Color color_;
            std::vector<sf::Vertex> quadVerts_;
            sf::Texture texture_;
            sf::Sprite topLeftSprite_;
            sf::Sprite topRightSprite_;
            sf::Sprite botLeftSprite_;
            sf::Sprite botRightSprite_;
        };

        using KnotFrameUPtr_t = std::unique_ptr<KnotFrame>;
    } // namespace gui
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_GUI_KNOTFRAME_HPP_INCLUDED
