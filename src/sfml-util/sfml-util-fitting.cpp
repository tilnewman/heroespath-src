// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// sfml-util-fitting.cpp
//
#include "sfml-util-fitting.hpp"

#include "sfml-util/sfml-util-center-to.hpp"

#include <SFML/Graphics/Sprite.hpp>

namespace heroespath
{
namespace sfml_util
{

    void Fit(sf::Sprite & s, const sf::Vector2f & LIMIT)
    {
        const float NEW_SCALE { ScaleThatFits(Size(s), FitCopy(Size(s), LIMIT)) };
        s.setScale(s.getScale().x * NEW_SCALE, s.getScale().y * NEW_SCALE);
    }

    void Fit(sf::Sprite & s, const float WIDTH_LIMIT, const float HEIGHT_LIMIT)
    {
        Fit(s, sf::Vector2f(WIDTH_LIMIT, HEIGHT_LIMIT));
    }

    void FitAndReCenter(sf::Sprite & s, const sf::Vector2f & LIMIT)
    {
        auto const ORIG_CENTER { CenterOf(s) };
        Fit(s, LIMIT);
        s.setPosition(ORIG_CENTER - (Size(s, 0.5f)) + s.getOrigin());
    }

    void FitAndReCenter(sf::Sprite & s, const float WIDTH_LIMIT, const float HEIGHT_LIMIT)
    {
        FitAndReCenter(s, sf::Vector2f(WIDTH_LIMIT, HEIGHT_LIMIT));
    }

    void FitAndReCenter(sf::Sprite & s, const sf::FloatRect & LIMIT)
    {
        FitAndReCenter(s, Size(LIMIT));
    }

    void FitAndReCenter(sf::Sprite & s, const sf::Sprite & LIMIT)
    {
        FitAndReCenter(s, LIMIT.getGlobalBounds());
    }

    void FitAndCenterTo(sf::Sprite & s, const sf::FloatRect & LIMIT)
    {
        Fit(s, Size(LIMIT));
        s.setPosition((CenterOf(LIMIT) - (Size(s) * 0.5f)) + s.getOrigin());
    }

    void FitAndCenterTo(sf::Sprite & s, const sf::Sprite & LIMIT)
    {
        FitAndCenterTo(s, LIMIT.getGlobalBounds());
    }

} // namespace sfml_util
} // namespace heroespath
