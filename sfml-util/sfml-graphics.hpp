#ifndef SFMLUTIL_SFMLGRAPHICS_HPP_INCLUDED
#define SFMLUTIL_SFMLGRAPHICS_HPP_INCLUDED
//
// sfml-graphics.hpp
//

//prevent warnings that can be ignored in SFML
#include "utilz/platform.hpp"
#ifdef PPLATFFORMDDETECT__APPPPLE_OOSS
#pragma GCC diagnostic ignored "-Wundef"
#pragma GCC diagnostic ignored "-Wfloat-equal"
#endif

#include <SFML/Graphics.hpp>

#ifdef PPLATFFORMDDETECT__APPPPLE_OOSS
#pragma GCC diagnostic warning "-Wfloat-equal"
#pragma GCC diagnostic warning "-Wundef"
#endif

#include <memory>
#include <vector>


namespace sfml_util
{

    using ImageSPtr_t = std::shared_ptr<sf::Image>;
    using ImageSVec_t = std::vector<ImageSPtr_t>;
    //
    using TextureSPtr_t = std::shared_ptr<sf::Texture>;
    using TextureSVec_t = std::vector<TextureSPtr_t>;
    //
    using SpriteSPtr_t = std::shared_ptr<sf::Sprite>;
    using SpriteSVec_t = std::vector<SpriteSPtr_t>;
    //
    using RendTextSPtr_t = std::shared_ptr<sf::RenderTexture>;
    using RendTextSVec_t = std::vector<RendTextSPtr_t>;
    //
    using FontSPtr_t = std::shared_ptr<sf::Font>;
    using FontSVec_t = std::vector<FontSPtr_t>;
    //
    using FloatRectVec_t = std::vector<sf::FloatRect>;
    //
    using WinSPtr_t = std::shared_ptr<sf::RenderWindow>;

}

#endif //SFMLUTIL_SFMLGRAPHICS_HPP_INCLUDED
