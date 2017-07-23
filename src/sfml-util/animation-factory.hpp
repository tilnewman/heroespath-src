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
#ifndef SFMLUTIL_ANIMATIONFACTORY_HPP_INCLUDE
#define SFMLUTIL_ANIMATIONFACTORY_HPP_INCLUDE
//
// animation-factory.hpp
//
#include "sfml-util/animation-base.hpp"
#include "sfml-util/animation-enum.hpp"


namespace sfml_util
{

    struct AnimationFactory
    {
        static AnimationUPtr_t Make(
            const Animations::Enum ENUM,
            const float            SCALE,
            const float            TIME_PER_FRAME_SEC,
            const sf::Color &      COLOR      = sf::Color::White,
            const sf::BlendMode &  BLEND_MODE = sf::BlendAdd);

        static AnimationUPtr_t Make(
            const Animations::Enum ENUM,
            const float            SCALE,
            const float            TIME_PER_FRAME_SEC,
            const sf::Color &      COLOR_FROM,
            const sf::Color &      COLOR_TO,
            const sf::BlendMode &  BLEND_MODE = sf::BlendAdd);

        static AnimationUPtr_t Make(
            const Animations::Enum ENUM,
            const sf::FloatRect &  REGION,
            const float            TIME_PER_FRAME_SEC,
            const sf::Color &      COLOR              = sf::Color::White,
            const sf::BlendMode &  BLEND_MODE         = sf::BlendAdd);

        static AnimationUPtr_t Make(
            const Animations::Enum ENUM,
            const sf::FloatRect &  REGION,
            const float            TIME_PER_FRAME_SEC,
            const sf::Color &      COLOR_FROM,
            const sf::Color &      COLOR_TO,
            const sf::BlendMode &  BLEND_MODE = sf::BlendAdd);
    };

}

#endif //SFMLUTIL_ANIMATIONFACTORY_HPP_INCLUDE
