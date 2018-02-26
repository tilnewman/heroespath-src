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
#include "map/layout.hpp"
#include "map/walk-sfx.hpp"
#include "map/map-anim.hpp"
#include "game/game-data-file.hpp"
#include "game/game.hpp"
#include "state/game-state.hpp"
#include "player/party.hpp"
#include "avatar/lpc-view.hpp"
#include "avatar/avatar-enum.hpp"
#include "sfml-util/sfml-util.hpp"
#include "sfml-util/sound-manager.hpp"
#include "misc/vector-map.hpp"
#include "misc/random.hpp"


namespace heroespath
{
namespace map
{

    //These are different so that players can always move faster than, and catch, NPCs.
    const float Map::PLAYER_MOVE_DISTANCE_{ 3.5f };
    const float Map::NONPLAYER_MOVE_DISTANCE_{ 3.0f };


    Map::Map(const sf::FloatRect & REGION)
    :
        mapDisplayUPtr_(std::make_unique<map::MapDisplay>( * this, REGION) ),
        collisionVec_(),
        transitionVec_(),
        level_(Level::Count),
        player_(std::make_unique<avatar::LPCView>(game::Game::Instance()->State().Party().Avatar())),
        nonPlayers_(),
        walkRectVecMap_()
    {}


    Map::~Map()
    {}


    void Map::Load(
        const Level::Enum LEVEL_TO_LOAD,
        const Level::Enum LEVEL_FROM,
        const bool IS_TEST_LOAD)
    {
        transitionVec_.clear();

        Layout & layout{ mapDisplayUPtr_->GetLayoutRef() };
        layout.Reset();

        MapAnimVec_t animInfoVec;

        WalkSfxRegionLayers walkSfxLayers;

        ParsePacket packet(
            Level::Path(LEVEL_TO_LOAD),
            layout,
            collisionVec_,
            transitionVec_,
            walkRectVecMap_,
            animInfoVec,
            walkSfxLayers);

        Parser mapParser;
        mapParser.Parse(packet);

        level_ = LEVEL_TO_LOAD;

        if (IS_TEST_LOAD == false)
        {
            mapDisplayUPtr_->Load(
                FindStartPos(transitionVec_, LEVEL_TO_LOAD, LEVEL_FROM),
                animInfoVec,
                walkSfxLayers);
        }
    }


    bool Map::MovePlayer(const sfml_util::Direction::Enum DIRECTION)
    {
        if (DoesAdjPlayerPosCollide(DIRECTION, PLAYER_MOVE_DISTANCE_))
        {
            return false;
        }
        else
        {
            if (ChangeLevelOnExit(DIRECTION, PLAYER_MOVE_DISTANCE_))
            {
                return true;
            }
            else
            {
                return mapDisplayUPtr_->Move(DIRECTION, PLAYER_MOVE_DISTANCE_);
            }
        }
    }


    void Map::MoveNonPlayers()
    {
        for (std::size_t npcIndex(0); npcIndex < nonPlayers_.size(); ++npcIndex)
        {
            //check if NPC move will collide with the player
            auto & npc{ nonPlayers_[npcIndex] };

            auto const ADJ_POS_V{
                [&]()
                {
                    auto v{ player_.GetView().SpriteSize() };
                    v.x *= 0.5f;
                    v.y *= 0.5f;
                    return v;
                }() };

            auto const PLAYER_RECT_FOR_NPC_COLLISIONS{
                [&]()
                {
                    auto const PLAYER_POS_V{ mapDisplayUPtr_->PlayerPosMap() };

                    return sf::FloatRect(
                        PLAYER_POS_V.x + (ADJ_POS_V.x * 1.2f),
                        PLAYER_POS_V.y + (ADJ_POS_V.x * 0.8f),
                        ADJ_POS_V.x * 0.05f,
                        ADJ_POS_V.y * 1.45f);
                }() };

            auto const NPC_RECT_ADJ{
                [&]()
                {
                    auto rect{ npc.GetView().SpriteRef().getGlobalBounds() };

                    switch (npc.GetView().Direction())
                    {
                        case sfml_util::Direction::Left:  { rect.left -= NONPLAYER_MOVE_DISTANCE_; break; }
                        case sfml_util::Direction::Right: { rect.left += NONPLAYER_MOVE_DISTANCE_; break; }
                        case sfml_util::Direction::Up:    { rect.top -= NONPLAYER_MOVE_DISTANCE_; break; }
                        case sfml_util::Direction::Down:
                        case sfml_util::Direction::Count:
                        default:                          { rect.top += NONPLAYER_MOVE_DISTANCE_; break; }
                    }

                    return rect;
                }() };


            auto const NPC_RECT_FOR_PLAYER_COLLISIONS{
                [&]()
                {
                    auto rect{ NPC_RECT_ADJ };
                    rect.left -= ADJ_POS_V.x * 0.25f;
                    rect.width *= 1.75f;
                    rect.height *= 1.75f;
                    return rect;
                }() };

            if (sfml_util::DoRectsOverlap(
                NPC_RECT_FOR_PLAYER_COLLISIONS,
                PLAYER_RECT_FOR_NPC_COLLISIONS))
            {
                npc.StopWalking();
                return;
            }

            //check if NPC move will collide with other NPCs
            auto const NPC_POS_V{ npc.GetView().SpriteRef().getPosition() };
            auto didNPCsCollide{ false };
            for (std::size_t subNPCIndex(0); subNPCIndex < nonPlayers_.size(); ++subNPCIndex)
            {
                if (subNPCIndex == npcIndex)
                {
                    continue;
                }

                auto & subNPC{ nonPlayers_[subNPCIndex] };

                if (sfml_util::DoRectsOverlap(
                    NPC_RECT_ADJ,
                    subNPC.GetView().SpriteRef().getGlobalBounds()))
                {
                    auto const SUB_POS_V{ subNPC.GetView().SpriteRef().getPosition() };

                    auto const IS_IN_THE_WAY{
                        [&]()
                        {
                            switch (npc.GetView().Direction())
                            {
                                case sfml_util::Direction::Left: { return (SUB_POS_V.x < NPC_POS_V.x); }
                                case sfml_util::Direction::Right:{ return (SUB_POS_V.x > NPC_POS_V.x); }
                                case sfml_util::Direction::Up:   { return (SUB_POS_V.y < NPC_POS_V.y); }
                                case sfml_util::Direction::Down:
                                case sfml_util::Direction::Count:
                                default:                         { return (SUB_POS_V.y > NPC_POS_V.y); }
                            }
                        }() };

                    if (IS_IN_THE_WAY)
                    {
                        didNPCsCollide = true;
                        break;
                    }
                }
            }

            if (didNPCsCollide)
            {
                npc.StopWalking();
            }
            else
            {
                npc.Move(NONPLAYER_MOVE_DISTANCE_);
            }
        }
    }


    void Map::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        target.draw( * mapDisplayUPtr_, states);
    }


    void Map::Update(const float TIME_ELAPSED)
    {
        mapDisplayUPtr_->Update(TIME_ELAPSED);

        player_.Update(TIME_ELAPSED);

        for (auto & npc : nonPlayers_)
        {
            npc.Update(TIME_ELAPSED);
        }
    }


    void Map::UpdateInteractions(const float TIME_ELAPSED)
    {
        //TODO
        player_.MovingIntoUpdate(TIME_ELAPSED);
    }


    void Map::SetPlayerWalkAnim(const sfml_util::Direction::Enum DIRECTION, const bool WILL_START)
    {
        player_.SetWalkAnim(DIRECTION, WILL_START);
        player_.MovingIntoReset();
    }


    void Map::EntryAndExitLevels(
        std::vector<Level::Enum> & entryLevels,
        std::vector<Level::Enum> & exitLevels)
    {
        for (auto const & TRANSITION : transitionVec_)
        {
            if (TRANSITION.IsEntry())
            {
                entryLevels.push_back(TRANSITION.Level());
            }
            else
            {
                exitLevels.push_back(TRANSITION.Level());
            }
        }
    }


    bool Map::DoesAdjPlayerPosCollide(
        const sfml_util::Direction::Enum DIR,
        const float ADJ)
    {
        auto const PLAYER_POS_V{ CalcAdjPlayerPos(DIR, ADJ) };

        auto const ADJ_FOR_COLLISIONS_V{
            [&]()
            {
                auto v{ player_.GetView().SpriteSize() };
                v.x *= 0.3f;
                v.y *= 0.5f;
                return v;
            }() };

        const sf::FloatRect PLAYER_RECT_FOR_MAP_COLLISIONS(
            PLAYER_POS_V.x - ADJ_FOR_COLLISIONS_V.x,
            PLAYER_POS_V.y - (ADJ_FOR_COLLISIONS_V.y * 0.6f),
            ADJ_FOR_COLLISIONS_V.x * 2.0f,
            ADJ_FOR_COLLISIONS_V.y * 1.4f);

        for (auto const & COLLISION_RECT : collisionVec_)
        {
            if (sfml_util::DoRectsOverlap(COLLISION_RECT, PLAYER_RECT_FOR_MAP_COLLISIONS))
            {
                return true;
            }
        }

        auto const ADJ_FOR_NPC_COLLISIONS_V{
            [&]()
            {
                auto v{ player_.GetView().SpriteSize() };
                v.x *= 0.5f;
                v.y *= 0.5f;
                return v;
            }() };

        const sf::FloatRect PLAYER_RECT_FOR_NPC_COLLISIONS(
            PLAYER_POS_V.x + (ADJ_FOR_NPC_COLLISIONS_V.x * 1.0f),
            PLAYER_POS_V.y + (ADJ_FOR_NPC_COLLISIONS_V.x * 0.5f),
            ADJ_FOR_NPC_COLLISIONS_V.x * 0.25f,
            ADJ_FOR_NPC_COLLISIONS_V.y * 1.25f);

        for (std::size_t i(0); i < nonPlayers_.size(); ++i)
        {
            auto const & NPC{ nonPlayers_[i] };

            const sf::FloatRect NPC_RECT{ NPC.GetView().SpriteRef().getGlobalBounds() };

            if (sfml_util::DoRectsOverlap(NPC_RECT, PLAYER_RECT_FOR_NPC_COLLISIONS))
            {
                player_.MovingIntoSet(i);
                return true;
            }
        }

        player_.MovingIntoReset();
        return false;
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
                ChangeLevel(TRANSITION);

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


    void Map::ChangeLevel(const Transition TRANSITION)
    {
        PlayDoorSfx(TRANSITION.DoorType(), true);
        Load(TRANSITION.Level(), level_);
        PlayDoorSfx(TRANSITION.DoorType(), false);
    }


    void Map::PlayDoorSfx(
        const sfml_util::sound_effect::DoorType DOOR_TYPE,
        const bool IS_OPEN_SFX) const
    {
        if (DOOR_TYPE != sfml_util::sound_effect::DoorType::Count)
        {
            auto const DOOR_ACTION{ ((IS_OPEN_SFX) ?
                sfml_util::sound_effect::DoorAction::Open :
                sfml_util::sound_effect::DoorAction::Close) };

            auto const DOOR_SFX{
                sfml_util::sound_effect::RandomDoorSfx(DOOR_TYPE, DOOR_ACTION) };

            if (DOOR_SFX != sfml_util::sound_effect::Count)
            {
                if (IS_OPEN_SFX)
                {
                    sfml_util::SoundManager::Instance()->SoundEffectPlayNow(DOOR_SFX);
                }
                else
                {
                    sfml_util::SoundManager::Instance()->SoundEffectPlay(DOOR_SFX, 1.0f);
                }
            }
        }
    }

}
}
