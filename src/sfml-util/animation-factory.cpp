// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
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

} // namespace sfml_util
} // namespace heroespath
