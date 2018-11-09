// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_ANIMATIONFACTORY_HPP_INCLUDE
#define HEROESPATH_GUI_ANIMATIONFACTORY_HPP_INCLUDE
//
// animation-factory.hpp
//
#include "gui/animation-base.hpp"
#include "gui/animation-enum.hpp"

namespace heroespath
{
namespace gui
{

    struct AnimationFactory
    {
        AnimationFactory() = delete;

        static AnimationUPtr_t Make(
            const Animations::Enum ENUM,
            const float SCALE,
            const float TIME_PER_FRAME_SEC,
            const sf::Color & COLOR = sf::Color::White,
            const sf::BlendMode & BLEND_MODE = sf::BlendAdd);

        static AnimationUPtr_t Make(
            const Animations::Enum ENUM,
            const float SCALE,
            const float TIME_PER_FRAME_SEC,
            const sf::Color & COLOR_FROM,
            const sf::Color & COLOR_TO,
            const sf::BlendMode & BLEND_MODE = sf::BlendAdd);

        static AnimationUPtr_t Make(
            const Animations::Enum ENUM,
            const sf::FloatRect & REGION,
            const float TIME_PER_FRAME_SEC,
            const sf::Color & COLOR = sf::Color::White,
            const sf::BlendMode & BLEND_MODE = sf::BlendAdd);

        static AnimationUPtr_t Make(
            const Animations::Enum ENUM,
            const sf::FloatRect & REGION,
            const float TIME_PER_FRAME_SEC,
            const sf::Color & COLOR_FROM,
            const sf::Color & COLOR_TO,
            const sf::BlendMode & BLEND_MODE = sf::BlendAdd);
    };

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_ANIMATIONFACTORY_HPP_INCLUDE