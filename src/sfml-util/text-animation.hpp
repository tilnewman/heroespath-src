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
#ifndef HEROESPATH_SFMLUTIL_TEXTANIMATION_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_TEXTANIMATION_HPP_INCLUDED
//
// text-animation.hpp
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/sliders.hpp"

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {
        class TextRegion;
        using TextRegionUPtr_t = std::unique_ptr<TextRegion>;
    } // namespace gui

    namespace animation
    {

        // Responsible for drawing emitted sparks
        class TextAnimation : public sf::Drawable
        {
            TextAnimation(const TextAnimation &);
            TextAnimation & operator=(const TextAnimation &);

        public:
            explicit TextAnimation(
                const std::string & TEXT,
                const sf::FloatRect & REGION,
                const float SPEED,
                const unsigned int START_FONT_SIZE,
                const unsigned int END_FONT_SIZE,
                const sf::Color & START_COLOR,
                const sf::Color & END_COLOR);

            inline bool IsFinished() const { return isFinished_; }
            void Update(const float ELAPSED_TIME_SEC);
            void draw(sf::RenderTarget & target, sf::RenderStates states) const;

        private:
            const std::string TEXT_;
            const unsigned int START_FONT_SIZE_;
            const unsigned int END_FONT_SIZE_;
            const sf::Color START_COLOR_;
            const sf::Color END_COLOR_;
            sf::Vector2f startPosV_;
            sf::Vector2f endPosV_;
            bool isFinished_;
            gui::TextRegionUPtr_t textRegionUPtr_;
            sliders::ZeroSliderOnce<float> slider_;
        };

        using TextAnimationUPtr_t = std::unique_ptr<TextAnimation>;
        using TextAnimationUVec_t = std::vector<TextAnimationUPtr_t>;
    } // namespace animation
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_TEXTANIMATION_HPP_INCLUDED
