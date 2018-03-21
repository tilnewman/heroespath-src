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
#ifndef HEROESPATH_SFMLUTIL_FADE_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_FADE_HPP_INCLUDED
//
// fade.hpp
//  Helper classes and functions for fading the screen in and out.
//
#include "sfml-util/sfml-graphics.hpp"

#include <memory>
#include <vector>

namespace heroespath
{
namespace sfml_util
{

    // A class allowing simple fading in and out of a color to any part of the screen.
    class Fade : public sf::Drawable
    {
    public:
        Fade(const Fade &) = delete;
        Fade(Fade &&) = delete;
        Fade & operator=(const Fade &) = delete;
        Fade & operator=(Fade &&) = delete;

    public:
        Fade(); // must call Setup() before any other functinos if using this constructor

        Fade(
            const float REGION_LEFT,
            const float REGION_TOP,
            const float REGION_WIDTH,
            const float REGION_HEIGHT);

        explicit Fade(const sf::FloatRect & SCREEN_REGION);

        virtual ~Fade();

        void
            FadeTo(const sf::Color & NEW_COLOR = sf::Color::Black, const float SPEED_MULT = 200.0f);

        void Reset(const sf::Color & NEW_COLOR);

        const sf::Color GetCurrentColor() const;
        const sf::Color GetTargetColor() const;

        // returns true when the fade in or out is complete
        bool Update(const float SECONDS);

        virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const;

        void UpdateRegion(const float WIDTH, const float HEIGHT);

    private:
        void Setup(const sf::FloatRect & SCREEN_REGION);

        void Setup(
            const float REGION_LEFT,
            const float REGION_TOP,
            const float REGION_WIDTH,
            const float REGION_HEIGHT);

        float targetColorRed_;
        float targetColorGreen_;
        float targetColorBlue_;
        float targetColorAlpha_;
        float currentColorRed_;
        float currentColorGreen_;
        float currentColorBlue_;
        float currentColorAlpha_;
        float speedMultiplier_;
        sf::VertexArray quad_;
        bool isRedIncr_;
        bool isBlueIncr_;
        bool isGreenIncr_;
        bool isAlphaIncr_;
    };

    using FadeSPtr_t = std::shared_ptr<Fade>;
    using FadeSVec_t = std::vector<FadeSPtr_t>;
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_FADE_HPP_INCLUDED
