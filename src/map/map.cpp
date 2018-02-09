//This is an open source non-commercial project. Dear PVS-Studio, please check it.
//PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
///////////////////////////////////////////////////////////////////////////////
//
//Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
//Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
//This software is provided 'as-is', without any express or implied warranty.
//In no event will the authors be held liable for any damages arising from
//the use of this software.
//
//Permission is granted to anyone to use this software for any purpose,
//including commercial applications, and to alter it and redistribute it
//freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software.  If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
//
// map.cpp
//
#include "map.hpp"

#include "map/map-display.hpp"
#include "map/parser.hpp"
#include "map/layout.hpp"
#include "game/game-data-file.hpp"


namespace heroespath
{
namespace map
{

    Map::Map(const sf::Vector2f & WIN_POS_V, const sf::Vector2f & WIN_SIZE_V)
    :
        mapDisplayUPtr_( std::make_unique<map::MapDisplay>(WIN_POS_V, WIN_SIZE_V) ),
        collisionQTree_(),
        transitionVec_(),
        level_(Level::Count)
    {}


    void Map::Load(const Level::Enum LEVEL_TO_LOAD, const Level::Enum LEVEL_FROM)
    {
        transitionVec_.clear();

        Layout & layout{ mapDisplayUPtr_->GetLayoutRef() };
        layout.Reset();

        Parser mapParser;
        mapParser.Parse(
            ComposeMapFilePath(LEVEL_TO_LOAD),
            layout,
            collisionQTree_,
            transitionVec_);

        mapDisplayUPtr_->Load( FindStartPos(transitionVec_, LEVEL_TO_LOAD, LEVEL_FROM) );
        level_ = LEVEL_TO_LOAD;
    }


    bool Map::MoveUp(const float ADJUSTMENT)
    {
        if (DoesAdjPlayerPosCollide(sfml_util::Direction::Up, ADJUSTMENT))
        {
            return false;
        }
        else
        {
            return mapDisplayUPtr_->MoveUp(ADJUSTMENT);
        }
    }


    bool Map::MoveDown(const float ADJUSTMENT)
    {
        if (DoesAdjPlayerPosCollide(sfml_util::Direction::Down, ADJUSTMENT))
        {
            return false;
        }
        else
        {
            return mapDisplayUPtr_->MoveDown(ADJUSTMENT);
        }
    }


    bool Map::MoveLeft(const float ADJUSTMENT)
    {
        if (DoesAdjPlayerPosCollide(sfml_util::Direction::Left, ADJUSTMENT))
        {
            return false;
        }
        else
        {
            return mapDisplayUPtr_->MoveLeft(ADJUSTMENT);
        }
    }


    bool Map::MoveRight(const float ADJUSTMENT)
    {
        if (DoesAdjPlayerPosCollide(sfml_util::Direction::Right, ADJUSTMENT))
        {
            return false;
        }
        else
        {
            return mapDisplayUPtr_->MoveRight(ADJUSTMENT);
        }
    }


    void Map::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        target.draw( * mapDisplayUPtr_, states);
    }


    bool Map::DoesAdjPlayerPosCollide(
        const sfml_util::Direction::Enum DIR,
        const float ADJ) const
    {
        auto posToTest{ mapDisplayUPtr_->PlayerPosMap() };

        switch (DIR)
        {
            case sfml_util::Direction::Left:  { posToTest.x -= ADJ; break; }
            case sfml_util::Direction::Right: { posToTest.x += ADJ; break; }
            case sfml_util::Direction::Up:    { posToTest.y -= ADJ; break; }
            case sfml_util::Direction::Down:  { posToTest.y += ADJ; break; }
            case sfml_util::Direction::Count:
            default:                          { break; }
        }

        return collisionQTree_.IsPointWithinCollisionRect(posToTest);
    }


    const std::string Map::ComposeMapFilePath(const Level::Enum E) const
    {
        return game::GameDataFile::Instance()->GetMediaPath("media-maps-dir") +
            Level::ToFilename(E);
    }


    const sf::Vector2f Map::FindStartPos(
        const TransitionVec_t & TRANS_VEC,
        const Level::Enum LEVEL_TO_LOAD,
        const Level::Enum LEVEL_FROM)
    {
        sf::Vector2f startPos(-1.0f, - 1.0f);

        for (auto const & TRANSITION : TRANS_VEC)
        {
            if (TRANSITION.IsEntry() && (TRANSITION.Level() == LEVEL_FROM))
            {
                startPos = TRANSITION.Center();
                break;
            }
        }

        M_ASSERT_OR_LOGANDTHROW_SS(((startPos.x > 0.0f) && (startPos.y > 0.0f)),
            "map::Map::Load(to_load=" << Level::ToString(LEVEL_TO_LOAD)
            << ", from=" << Level::ToString(LEVEL_FROM)
            << ") unable to find an entry transition.");

        return startPos;
    }

}
}
