// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_LOADERS_HPP_INCLUDED
#define HEROESPATH_GUI_LOADERS_HPP_INCLUDED
//
// loaders.hpp
//
#include "sfml-util/sfml-audio.hpp"

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <string>
#include <vector>

namespace heroespath
{
namespace gui
{

    // Responsible for loading sfml media
    struct Loaders
    {
        Loaders() = delete;

        static void Texture(
            sf::Texture & texture, const std::string & PATH_STR, const bool WILL_SMOOTH = true);

        // Returns the number loaded into textureVec
        static std::size_t AllTexturesInDir(
            std::vector<sf::Texture> & textureVec,
            const std::string & DIR_PATH_STR,
            const bool WILL_SMOOTH = true);

        static void Font(sf::Font & font, const std::string & PATH_STR);

        static MusicUPtr_t Music(const std::string & PATH_STR);
    };

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_LOADERS_HPP_INCLUDED
