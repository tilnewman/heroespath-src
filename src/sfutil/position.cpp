// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// position.cpp
//
#include "position.hpp"

#include "gui/text.hpp"
#include "sfutil/size-and-scale.hpp"

#include <SFML/Graphics/Sprite.hpp>

namespace heroespath
{
namespace sfutil
{

    const sf::Vector2f Position(const sf::Sprite & S) { return S.getPosition(); }
    const sf::Vector2f Position(const gui::Text & T) { return T.getPosition(); }

    float Left(const sf::Sprite & S) { return Left(S.getGlobalBounds()); }
    float Left(const gui::Text & T) { return Left(T.getGlobalBounds()); }

    float Right(const sf::Sprite & S) { return Right(S.getGlobalBounds()); }
    float Right(const gui::Text & T) { return Right(T.getGlobalBounds()); }

    float Top(const sf::Sprite & S) { return Top(S.getGlobalBounds()); }
    float Top(const gui::Text & T) { return Top(T.getGlobalBounds()); }

    float Bottom(const sf::Sprite & S) { return Bottom(S.getGlobalBounds()); }
    float Bottom(const gui::Text & T) { return Bottom(T.getGlobalBounds()); }

} // namespace sfutil
} // namespace heroespath
