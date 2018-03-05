// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
//
// animation-factory.cpp
//
#include "animation-factory.hpp"

#include "sfml-util/animation-multi.hpp"
#include "sfml-util/animation-single.hpp"

namespace heroespath
{
namespace sfml_util
{

    AnimationUPtr_t AnimationFactory::Make(
        const Animations::Enum ENUM,
        const float SCALE,
        const float TIME_PER_FRAME_SEC,
        const sf::Color & COLOR,
        const sf::BlendMode & BLEND_MODE)
    {
        return Make(ENUM, SCALE, TIME_PER_FRAME_SEC, COLOR, COLOR, BLEND_MODE);
    }

    AnimationUPtr_t AnimationFactory::Make(
        const Animations::Enum ENUM,
        const float SCALE,
        const float TIME_PER_FRAME_SEC,
        const sf::Color & COLOR_FROM,
        const sf::Color & COLOR_TO,
        const sf::BlendMode & BLEND_MODE)
    {
        // initial position is the origin, and the initial size is default
        sf::FloatRect r(0.0f, 0.0f, 0.0f, 0.0f);

        auto animUPtr{ Make(ENUM, r, TIME_PER_FRAME_SEC, COLOR_FROM, COLOR_TO, BLEND_MODE) };

        // correct the size (account for the given SCALE)
        r.width = animUPtr->OrigSize().x * SCALE;
        r.height = animUPtr->OrigSize().y * SCALE;
        animUPtr->SetEntityRegion(r);

        return animUPtr;
    }

    AnimationUPtr_t AnimationFactory::Make(
        const Animations::Enum ENUM,
        const sf::FloatRect & REGION,
        const float TIME_PER_FRAME_SEC,
        const sf::Color & COLOR,
        const sf::BlendMode & BLEND_MODE)
    {
        return Make(ENUM, REGION, TIME_PER_FRAME_SEC, COLOR, COLOR, BLEND_MODE);
    }

    AnimationUPtr_t AnimationFactory::Make(
        const Animations::Enum ENUM,
        const sf::FloatRect & REGION,
        const float TIME_PER_FRAME_SEC,
        const sf::Color & COLOR_FROM,
        const sf::Color & COLOR_TO,
        const sf::BlendMode & BLEND_MODE)
    {
        if (Animations::IsMultiTexture(ENUM))
        {
            return std::make_unique<AnimationMultiTexture>(
                ENUM, REGION, TIME_PER_FRAME_SEC, BLEND_MODE, COLOR_FROM, COLOR_TO);
        }
        else
        {
            return std::make_unique<AnimationSingleTexture>(
                ENUM, REGION, TIME_PER_FRAME_SEC, BLEND_MODE, COLOR_FROM, COLOR_TO);
        }
    }
}
}
