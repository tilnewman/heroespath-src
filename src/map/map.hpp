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
#ifndef HEROESPATH_MAP_MAP_HPP_INCLUDED
#define HEROESPATH_MAP_MAP_HPP_INCLUDED
//
// map.hpp
//
#include "sfml-util/collision-quad-tree.hpp"
#include "sfml-util/direction-enum.hpp"

#include "map/level-enum.hpp"
#include "map/transition.hpp"

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Drawable.hpp>

#include <memory>


namespace heroespath
{
namespace map
{

    class MapDisplay;
    using MapDisplayUPtr_t = std::unique_ptr<MapDisplay>;


    //Responsible for all state and operation of a 2D map of the game world.
    class Map : public sf::Drawable
    {
        Map(const Map &) = delete;
        Map & operator=(const Map &) = delete;

    public:
        Map(const sf::Vector2f & WIN_POS_V, const sf::Vector2f & WIN_SIZE_V);
        
        void Load(const Level::Enum LEVEL_TO_LOAD, const Level::Enum LEVEL_FROM);

        bool Move(const sfml_util::Direction::Enum, const float ADJUSTMENT);

        virtual void draw(sf::RenderTarget &, sf::RenderStates) const override;

        inline Level::Enum Level() const { return level_; }

    private:
        bool DoesAdjPlayerPosCollide(
            const sfml_util::Direction::Enum DIR,
            const float ADJ) const;

        const std::string ComposeMapFilePath(const Level::Enum) const;

        const sf::Vector2f FindStartPos(
            const TransitionVec_t &,
            const Level::Enum LEVEL_TO_LOAD,
            const Level::Enum LEVEL_FROM);

        bool ChangeLevelOnExit(
            const sfml_util::Direction::Enum DIRECTION,
            const float ADJUSTMENT);

        const sf::Vector2f CalcAdjPlayerPos(
            const sfml_util::Direction::Enum DIRECTION,
            const float ADJUSTMENT) const;

    private:
        MapDisplayUPtr_t mapDisplayUPtr_;
        sfml_util::QuadTree collisionQTree_;
        TransitionVec_t transitionVec_;
        Level::Enum level_;
    };

    using MapUPtr_t = std::unique_ptr<Map>;

}
}

#endif //HEROESPATH_MAP_MAP_HPP_INCLUDED