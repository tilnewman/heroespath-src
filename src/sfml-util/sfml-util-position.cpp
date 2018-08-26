// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// sfml-util-position.cpp
//
#include "sfml-util-position.hpp"

#include "sfml-util/sfml-util-size-and-scale.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

namespace heroespath
{
namespace sfml_util
{

    const sf::Vector2f Position(const sf::Sprite & S) { return S.getPosition(); }
    const sf::Vector2f Position(const sf::Text & T) { return T.getPosition(); }

    float Left(const sf::Sprite & S) { return Left(S.getGlobalBounds()); }
    float Left(const sf::Text & T) { return Left(T.getGlobalBounds()); }

    float Right(const sf::Sprite & S) { return Right(S.getGlobalBounds()); }
    float Right(const sf::Text & T) { return Right(T.getGlobalBounds()); }

    float Top(const sf::Sprite & S) { return Top(S.getGlobalBounds()); }
    float Top(const sf::Text & T) { return Top(T.getGlobalBounds()); }

    float Bottom(const sf::Sprite & S) { return Bottom(S.getGlobalBounds()); }
    float Bottom(const sf::Text & T) { return Bottom(T.getGlobalBounds()); }

    const sf::Vector2f SetTextPosition(sf::Text & sfText, const sf::Vector2f & NEW_POS_V)
    {
        const auto CORRECTION_V { ScaleCopy(Position(sfText.getLocalBounds()), sfText.getScale()) };
        const auto CORRECTED_POS_V { NEW_POS_V - CORRECTION_V };
        sfText.setPosition(CORRECTED_POS_V);
        return CORRECTED_POS_V;
    }

} // namespace sfml_util
} // namespace heroespath
