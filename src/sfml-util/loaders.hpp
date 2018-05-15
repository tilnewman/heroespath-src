// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_LOADERS_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_LOADERS_HPP_INCLUDED
//
// loaders.hpp
//  Functions that help load assests and media.
//
#include "sfml-util/sfml-audio.hpp"
#include "sfml-util/sfml-graphics.hpp"

#include <string>
#include <vector>

namespace heroespath
{
namespace sfml_util
{

    void LoadTexture(
        sf::Texture & texture, const std::string & PATH_STR, const bool WILL_SMOOTH = true);

    std::size_t LoadAllTexturesInDir(
        std::vector<sf::Texture> & textureVec,
        const std::string & DIR_STR,
        const bool WILL_SMOOTH = true);

    void LoadFont(sf::Font & font, const std::string & PATH_STR);

    MusicUPtr_t LoadMusic(const std::string & PATH_STR);
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_LOADERS_HPP_INCLUDED
