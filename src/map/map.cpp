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
#include "npc/i-view.hpp"
#include "npc/view-factory.hpp"
#include "sfml-util/sfml-util.hpp"
#include "misc/vector-map.hpp"


namespace heroespath
{
namespace map
{

    const float Map::PLAYER_MOVE_SPEED_{ 3.5f };
    const float Map::NONPLAYER_MOVE_SPEED_{ 3.0f };


    Map::Map(const sf::Vector2f & WIN_POS_V, const sf::Vector2f & WIN_SIZE_V)
    :
        mapDisplayUPtr_(std::make_unique<map::MapDisplay>( * this, WIN_POS_V, WIN_SIZE_V) ),
        collisionVec_(),
        transitionVec_(),
        level_(Level::Count),
        player_(npc::ViewFactory::MakeLPCView("media-images-npc-lpc-puck")),
        nonPlayers_()
    {}


    void Map::Load(const Level::Enum LEVEL_TO_LOAD, const Level::Enum LEVEL_FROM)
    {
        transitionVec_.clear();

        Layout & layout{ mapDisplayUPtr_->GetLayoutRef() };
        layout.Reset();

        WalkRectMap_t walkRectMap;

        Parser mapParser;
        mapParser.Parse(
            ComposeMapFilePath(LEVEL_TO_LOAD),
            layout,
            collisionVec_,
            transitionVec_,
            walkRectMap);

        mapDisplayUPtr_->Load( FindStartPos(transitionVec_, LEVEL_TO_LOAD, LEVEL_FROM) );
        level_ = LEVEL_TO_LOAD;

        nonPlayers_.push_back(
            npc::Model(npc::ViewFactory::MakeLPCView("media-images-npc-lpc-beardy"),
                walkRectMap[0]) );
    }


    bool Map::MovePlayer(const sfml_util::Direction::Enum DIRECTION)
    {
        if (DoesAdjPlayerPosCollide(DIRECTION, PLAYER_MOVE_SPEED_))
        {
            return false;
        }
        else
        {
            if (ChangeLevelOnExit(DIRECTION, PLAYER_MOVE_SPEED_))
            {
                return true;
            }
            else
            {
                return mapDisplayUPtr_->Move(DIRECTION, PLAYER_MOVE_SPEED_);
            }
        }
    }


    void Map::MoveNonPlayers()
    {
        for (auto & nonPlayer : nonPlayers_)
        {
            nonPlayer.Move(NONPLAYER_MOVE_SPEED_);
        }
    }


    void Map::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        target.draw( * mapDisplayUPtr_, states);
    }


    void Map::Update(const float TIME_ELAPSED)
    {
        player_.Update(TIME_ELAPSED);

        for (auto & npc : nonPlayers_)
        {
            npc.Update(TIME_ELAPSED);
        }
    }


    void Map::SetPlayerWalkAnim(const sfml_util::Direction::Enum DIRECTION, const bool WILL_START)
    {
        player_.SetWalkAnim(DIRECTION, WILL_START);
    }


    bool Map::DoesAdjPlayerPosCollide(
        const sfml_util::Direction::Enum DIR,
        const float ADJ) const
    {
        auto const PLAYER_POS_V{ CalcAdjPlayerPos(DIR, ADJ) };

        auto const ADJ_V{ [&]()
        {
            auto v(player_.GetView().SpriteSize());
            v.x *= 0.3f;
            v.y *= 0.5f;
            return v;
        }() };

        const sf::FloatRect PLAYER_POS_RECT(
            PLAYER_POS_V.x - ADJ_V.x,
            PLAYER_POS_V.y - ADJ_V.y,
            ADJ_V.x * 2.0f,
            ADJ_V.y * 2.0f);
        
        for (auto const & COLLISION_RECT : collisionVec_)
        {
            if (sfml_util::DoRectsOverlap(COLLISION_RECT, PLAYER_POS_RECT))
            {
                return true;
            }
        }

        return false;
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


    bool Map::ChangeLevelOnExit(
        const sfml_util::Direction::Enum DIRECTION,
        const float ADJUSTMENT)
    {
        auto const ADJ_PLAYER_POS_V{ CalcAdjPlayerPos(DIRECTION, ADJUSTMENT) };

        for (auto const & TRANSITION : transitionVec_)
        {
            if ((TRANSITION.IsEntry() == false) &&
                (TRANSITION.Rect().contains(ADJ_PLAYER_POS_V)))
            {
                Load(TRANSITION.Level(), level_);
                return true;
            }
        }

        return false;
    }


    const sf::Vector2f Map::CalcAdjPlayerPos(
        const sfml_util::Direction::Enum DIRECTION,
        const float ADJUSTMENT) const
    {
        auto posToTest{ mapDisplayUPtr_->PlayerPosMap() };

        switch (DIRECTION)
        {
            case sfml_util::Direction::Left:  { posToTest.x -= ADJUSTMENT; break; }
            case sfml_util::Direction::Right: { posToTest.x += ADJUSTMENT; break; }
            case sfml_util::Direction::Up:    { posToTest.y -= ADJUSTMENT; break; }
            case sfml_util::Direction::Down:  { posToTest.y += ADJUSTMENT; break; }
            case sfml_util::Direction::Count:
            default:                          { break; }
        }

        return posToTest;
    }

}
}
