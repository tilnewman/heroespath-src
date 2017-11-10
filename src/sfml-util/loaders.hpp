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
#ifndef HEROESPATH_SFMLUTIL_LOADERS_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_LOADERS_HPP_INCLUDED
//
// loaders.hpp
//  Functions that help load assests and media.
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/sfml-audio.hpp"

#include <string>
#include <vector>


namespace heroespath
{
namespace sfml_util
{

    void LoadTexture(sf::Texture &       texture,
                     const std::string & PATH_STR,
                     const bool          WILL_SMOOTH = true);


    std::size_t LoadAllTexturesInDir(
        std::vector<sf::Texture> & textureVec,
        const std::string &        DIR_STR,
        const bool                 WILL_SMOOTH = true);


    void LoadFont(sf::Font & font, const std::string & PATH_STR);


    MusicUPtr_t LoadMusic(const std::string & PATH_STR);

}
}

#endif //HEROESPATH_SFMLUTIL_LOADERS_HPP_INCLUDED
