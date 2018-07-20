// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_SFMLGRAPHICS_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_SFMLGRAPHICS_HPP_INCLUDED
//
// sfml-graphics.hpp
//
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"

// prevent warnings that can be ignored in SFML
#include "misc/platform.hpp"
#ifdef HEROESPATH_PLATFORM_DETECTED_IS_APPLE
#pragma GCC diagnostic ignored "-Wundef"
#pragma GCC diagnostic ignored "-Wfloat-equal"
#endif

#include <SFML/Graphics.hpp> // this includes SFML/System.hpp

#ifdef HEROESPATH_PLATFORM_DETECTED_IS_APPLE
#pragma GCC diagnostic warning "-Wfloat-equal"
#pragma GCC diagnostic warning "-Wundef"
#endif

#include <list>
#include <memory>
#include <vector>

namespace heroespath
{
namespace sfml_util
{
    using Vector2fVec_t = std::vector<sf::Vector2f>;
    using Vector2iVec_t = std::vector<sf::Vector2i>;
    using Vector2uVec_t = std::vector<sf::Vector2u>;

    using FloatRectVec_t = std::vector<sf::FloatRect>;

    using Vector2fOpt_t = boost::optional<sf::Vector2f>;
    using Vector2iOpt_t = boost::optional<sf::Vector2i>;
    using Vector2uOpt_t = boost::optional<sf::Vector2u>;

    using FloatRectOpt_t = boost::optional<sf::FloatRect>;

    using TextureVec_t = std::vector<sf::Texture>;
    using TextureUPtr_t = std::unique_ptr<sf::Texture>;
    using TextureUVec_t = std::vector<TextureUPtr_t>;

    using SpriteVec_t = std::vector<sf::Sprite>;

    using FontPtr_t = misc::NotNull<sf::Font *>;
    using FontPtrOpt_t = boost::optional<FontPtr_t>;
    using FontUPtr_t = std::unique_ptr<sf::Font>;
    using FontUVec_t = std::vector<FontUPtr_t>;

    using WinUPtr_t = std::unique_ptr<sf::RenderWindow>;

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_SFMLGRAPHICS_HPP_INCLUDED
