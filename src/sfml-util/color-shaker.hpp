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
#ifndef HEROESPATH_SFMLUTIL_COLORSHAKER_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_COLORSHAKER_HPP_INCLUDED
//
// color-shaker.cpp
//
#include "sfml-util/sliders.hpp"

#include <SFML/Graphics/Color.hpp>

namespace heroespath
{
namespace sfml_util
{

    // Responsible for alternating between two colors at the given speed.
    class ColorShaker
    {
    public:
        explicit ColorShaker(
            const sf::Color & COLOR_ORIG,
            const sf::Color & COLOR_ALT = sf::Color::White,
            const float SPEED = 50.0f,
            const bool WILL_START = true);

        const sf::Color Update(const float ELAPSED_TIME_SECONDS);

        void Reset();

        inline void Start() { isShaking_ = true; }
        inline void Stop() { isShaking_ = false; }
        inline float Speed() const { return static_cast<float>(slider_.Speed()); }
        inline bool IsShaking() const { return isShaking_; }
        inline const sf::Color Current() const { return colorCurr_; }

    private:
        sliders::Slider<float> slider_;
        sf::Color colorOrig_;
        sf::Color colorAlt_;
        sf::Color colorCurr_;
        bool isShaking_;
        float redDiff_;
        float greenDiff_;
        float blueDiff_;
        float alphaDiff_;
    };
}
}

#endif // HEROESPATH_SFMLUTIL_COLORSHAKER_HPP_INCLUDED
