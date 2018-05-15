// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_MAP_MAPANIM_HPP_INCLUDED
#define HEROESPATH_MAP_MAPANIM_HPP_INCLUDED
//
// map-anim.hpp
//
#include "sfml-util/animation-enum.hpp"
#include "sfml-util/music-enum.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <string>
#include <vector>

namespace heroespath
{
namespace map
{

    // Responsible for wrapping all the data about an animation in the map.
    struct MapAnim
    {
        explicit MapAnim(
            const sfml_util::Animations::Enum ENUM = sfml_util::Animations::Count,
            const sf::FloatRect & RECT = sf::FloatRect(),
            const sfml_util::MusicEnumVec_t & MUSIC_VEC = sfml_util::MusicEnumVec_t())
            : which_anim(ENUM)
            , rect(RECT)
            , music_vec(MUSIC_VEC)
        {}

        sfml_util::Animations::Enum which_anim;
        sf::FloatRect rect;
        sfml_util::MusicEnumVec_t music_vec;
    };

    using MapAnimVec_t = std::vector<MapAnim>;
} // namespace map
} // namespace heroespath

#endif // HEROESPATH_MAP_LEVELENUM_HPP_INCLUDED
