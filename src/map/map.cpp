// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// map.cpp
//
#include "map.hpp"

#include "avatar/avatar-enum.hpp"
#include "avatar/lpc-view.hpp"
#include "creature/player-party.hpp"
#include "game/game-state.hpp"
#include "game/game.hpp"
#include "game/world.hpp"
#include "gui/sound-manager.hpp"
#include "interact/interaction-factory.hpp"
#include "map/layout.hpp"
#include "map/map-anim.hpp"
#include "map/map-display.hpp"
#include "misc/config-file.hpp"
#include "misc/random.hpp"
#include "misc/vector-map.hpp"
#include "sfutil/display.hpp"
#include "sfutil/overlap.hpp"
#include "sfutil/size-and-scale.hpp"

#include <exception>
#include <sstream>
#include <utility>

namespace heroespath
{
namespace map
{

    // These are different so that players can always move faster than, and catch, NPCs.
    const float Map::PLAYER_MOVE_DISTANCE_ { 3.5f };
    const float Map::NONPLAYER_MOVE_DISTANCE_ { 3.0f };

    Map::Map(const sf::FloatRect & REGION, interact::InteractionManager & interactionManager)
        : WALK_SFX_VOLUME_RATIO_(misc::ConfigFile::Instance()->ValueOrDefault<float>(
            "heroespath-sound-map-walk-sfx-volume-ratio"))
        , mapDisplayUPtr_(std::make_unique<map::MapDisplay>(*this, REGION))
        , interactionManager_(interactionManager)
        , collisionVec_()
        , quadTree_()
        , collisionGrid_()
        , collisionTimeTrials_("Collision Detection", TimeRes::Nano, true, 50, 0.1)
        , collisionNaiveIndex_(collisionTimeTrials_.AddCollecter("Naive"))
        , collisionQuadIndex_(collisionTimeTrials_.AddCollecter("Quad"))
        , collisionGridIndex_(collisionTimeTrials_.AddCollecter("Grid"))
        , collisionNpcIndex_(collisionTimeTrials_.AddCollecter("Npc"))
        , transitionVec_()
        , level_(Level::Count)
        , player_(game::Game::Instance()->State().Party().Avatar())
        , nonPlayers_()
        , walkRectVecMap_()
        , walkSfxLayers_()
        , walkSfx_(gui::sound_effect::Count)
        , walkSfxIsWalking_(false)
        , sfxTimer_(misc::ConfigFile::Instance()->ValueOrDefault<float>(
              "heroespath-sound-map-sfx-time-between-updates"))
    {}

    Map::~Map() { StopWalkSfxIfValid(); }

    void Map::TransitionLevel(const Transition & TRANSITION)
    {
        PlayTransitionSfx(TRANSITION.SfxType(), true);
        Load(TRANSITION.WhichLevel(), level_);
        PlayTransitionSfx(TRANSITION.SfxType(), false);
    }

    void Map::Load(
        const Level::Enum LEVEL_TO_LOAD, const Level::Enum LEVEL_FROM, const bool IS_TEST_LOAD)
    {
        StopWalkSfxIfValid();

        transitionVec_.clear();

        Layout & layout { mapDisplayUPtr_->GetLayoutRef() };
        layout.Reset();

        MapAnimVec_t animInfoVec;

        ParsePacket packet(
            Level::Path(LEVEL_TO_LOAD),
            layout,
            collisionVec_,
            transitionVec_,
            walkRectVecMap_,
            animInfoVec,
            walkSfxLayers_);

        Parser mapParser;
        mapParser.Parse(packet);

        level_ = LEVEL_TO_LOAD;

        // the order of these three is critical
        game::Game::Instance()->State().GetWorld().HandleLevelUnload(LEVEL_FROM);
        game::Game::Instance()->State().GetWorld().HandleLevelLoad(LEVEL_TO_LOAD);
        ResetNonPlayers();

        if (IS_TEST_LOAD == false)
        {
            mapDisplayUPtr_->Load(
                FindPlayerStartPos(transitionVec_, LEVEL_TO_LOAD, LEVEL_FROM), animInfoVec);
        }

        player_.SetCenteredMapPos(mapDisplayUPtr_->PlayerPosMap());

        quadTree_.Setup(
            sf::FloatRect(sf::Vector2f(), mapDisplayUPtr_->MapSizeInMapCoordinatesf()),
            collisionVec_);

        collisionGrid_.Setup(mapDisplayUPtr_->MapSizeInMapCoordinatesf(), collisionVec_);

        collisionTimeTrials_.EndAllContests();
    }

    bool Map::MovePlayer(const gui::Direction::Enum DIRECTION)
    {
        if (interactionManager_.IsLocked())
        {
            return false;
        }

        if (DoesAdjPlayerPosCollide(DIRECTION, PLAYER_MOVE_DISTANCE_))
        {
            return false;
        }
        else
        {
            const auto TRANSITION_OPT { CheckIfEnteringTransition(
                DIRECTION, PLAYER_MOVE_DISTANCE_) };

            if (TRANSITION_OPT)
            {
                HandleEnteringTransition(TRANSITION_OPT.value());
                return true;
            }
            else
            {
                if (interactionManager_.Current())
                {
                    interactionManager_.RemoveCurrent();
                }

                const auto DID_ACTUALLY_MOVE { mapDisplayUPtr_->Move(
                    DIRECTION, PLAYER_MOVE_DISTANCE_) };

                player_.SetCenteredMapPos(mapDisplayUPtr_->PlayerPosMap());
                return DID_ACTUALLY_MOVE;
            }
        }
    }

    void Map::MoveNonPlayers()
    {
        const auto PLAYER_ADJ_POS_V { [&]() {
            return sfutil::Size(player_.GetView().SpriteRef(), 0.5f);
        }() };

        const auto PLAYER_RECT_FOR_NPC_COLLISIONS { [&]() {
            const auto PLAYER_POS_V { mapDisplayUPtr_->PlayerPosMap() };

            return sf::FloatRect(
                PLAYER_POS_V.x + (PLAYER_ADJ_POS_V.x * 1.2f),
                PLAYER_POS_V.y + (PLAYER_ADJ_POS_V.x * 0.8f),
                PLAYER_ADJ_POS_V.x * 0.05f,
                PLAYER_ADJ_POS_V.y * 1.45f);
        }() };

        for (auto & npcPtrModelPair : nonPlayers_)
        {
            if (npcPtrModelPair.second.IsWalking() == false)
            {
                continue;
            }

            // check if NPC move will collide with the player
            const auto & NPC_VIEW { npcPtrModelPair.second.GetView() };

            const auto NPC_RECT_ADJ { [&]() {
                auto rect { NPC_VIEW.SpriteRef().getGlobalBounds() };

                switch (NPC_VIEW.Direction())
                {
                    case gui::Direction::Left:
                    {
                        rect.left -= NONPLAYER_MOVE_DISTANCE_;
                        break;
                    }
                    case gui::Direction::Right:
                    {
                        rect.left += NONPLAYER_MOVE_DISTANCE_;
                        break;
                    }
                    case gui::Direction::Up:
                    {
                        rect.top -= NONPLAYER_MOVE_DISTANCE_;
                        break;
                    }
                    case gui::Direction::Down:
                    case gui::Direction::Count:
                    default:
                    {
                        rect.top += NONPLAYER_MOVE_DISTANCE_;
                        break;
                    }
                }

                return rect;
            }() };

            const auto NPC_RECT_FOR_PLAYER_COLLISIONS { [&]() {
                auto rect { NPC_RECT_ADJ };
                rect.left -= PLAYER_ADJ_POS_V.x * 0.25f;
                rect.width *= 1.75f;
                rect.height *= 1.75f;
                return rect;
            }() };

            if (NPC_RECT_FOR_PLAYER_COLLISIONS.intersects(PLAYER_RECT_FOR_NPC_COLLISIONS))
            {
                npcPtrModelPair.second.StopWalking();
                npcPtrModelPair.second.SetIsNextToPlayer(true, player_.GetCenteredMapPos(), false);
                return;
            }

            // check if NPC move will collide with other NPCs
            const auto NPC_POS_V { NPC_VIEW.SpriteRef().getPosition() };
            auto didNPCsCollide { false };
            for (const auto & SUB_NPC_PTR_MODEL_PAIR : nonPlayers_)
            {
                if (npcPtrModelPair.first == SUB_NPC_PTR_MODEL_PAIR.first)
                {
                    continue;
                }

                if (NPC_RECT_ADJ.intersects(
                        SUB_NPC_PTR_MODEL_PAIR.second.GetView().SpriteRef().getGlobalBounds()))
                {
                    const auto SUB_POS_V {
                        SUB_NPC_PTR_MODEL_PAIR.second.GetView().SpriteRef().getPosition()
                    };

                    const auto IS_IN_THE_WAY { [&]() {
                        switch (NPC_VIEW.Direction())
                        {
                            case gui::Direction::Left:
                            {
                                return (SUB_POS_V.x < NPC_POS_V.x);
                            }
                            case gui::Direction::Right:
                            {
                                return (SUB_POS_V.x > NPC_POS_V.x);
                            }
                            case gui::Direction::Up:
                            {
                                return (SUB_POS_V.y < NPC_POS_V.y);
                            }
                            case gui::Direction::Down:
                            case gui::Direction::Count:
                            default:
                            {
                                return (SUB_POS_V.y > NPC_POS_V.y);
                            }
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
                npcPtrModelPair.second.StopWalking();
            }
            else
            {
                npcPtrModelPair.second.MoveIfWalking(NONPLAYER_MOVE_DISTANCE_);
            }
        }
    }

    void Map::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        target.draw(*mapDisplayUPtr_, states);
    }

    void Map::Update(const float TIME_ELAPSED)
    {
        mapDisplayUPtr_->Update(TIME_ELAPSED);

        player_.Update(TIME_ELAPSED);

        auto movingIntoNpcPtrOpt { player_.MovingIntoUpdate(TIME_ELAPSED) };
        if (movingIntoNpcPtrOpt)
        {
            if (!interactionManager_.Current())
            {
                movingIntoNpcPtrOpt.value()->RemakeConversationIfRandom();
                if (movingIntoNpcPtrOpt.value()->ConversationPoint().IsValid())
                {
                    player_.StopWalking();

                    interactionManager_.SetNext(interact::InteractionFactory::MakeConversation(
                        movingIntoNpcPtrOpt.value(),
                        GetNpcDefaultPoseSprite(movingIntoNpcPtrOpt.value())));
                }
            }
        }

        for (auto & npcPtrModelPair : nonPlayers_)
        {
            npcPtrModelPair.second.Update(TIME_ELAPSED);
        }

        if (sfxTimer_.Update(TIME_ELAPSED))
        {
            mapDisplayUPtr_->UpdateAnimMusicVolume();
            UpdateWalkSfx();
        }
    }

    void Map::SetPlayerWalkAnim(const gui::Direction::Enum DIRECTION, const bool WILL_START)
    {
        player_.SetWalkAnim(DIRECTION, WILL_START);
        player_.MovingIntoReset();
    }

    void Map::EntryAndExitLevels(
        std::vector<Level::Enum> & entryLevels, std::vector<Level::Enum> & exitLevels)
    {
        for (const auto & TRANSITION : transitionVec_)
        {
            if (TRANSITION.IsEntry())
            {
                entryLevels.emplace_back(TRANSITION.WhichLevel());
            }
            else
            {
                exitLevels.emplace_back(TRANSITION.WhichLevel());
            }
        }
    }

    bool Map::DoesAdjPlayerPosCollide(const gui::Direction::Enum DIR, const float ADJ)
    {
        const auto PLAYER_POS_V { CalcAdjPlayerPos(DIR, ADJ) };

        const auto ADJ_FOR_COLLISIONS_V { [&]() {
            return sfutil::Size(player_.GetView().SpriteRef(), sf::Vector2f(0.3f, 0.5f));
        }() };

        const sf::FloatRect PLAYER_RECT_FOR_MAP_COLLISIONS(
            PLAYER_POS_V.x - ADJ_FOR_COLLISIONS_V.x,
            PLAYER_POS_V.y - (ADJ_FOR_COLLISIONS_V.y * 0.6f),
            ADJ_FOR_COLLISIONS_V.x * 2.0f,
            ADJ_FOR_COLLISIONS_V.y * 1.4f);

        bool didCollideWithMap { false };
        {
            misc::ScopedTimeTrial scopedTimerJane(collisionTimeTrials_, collisionQuadIndex_);
            DoesMapCoordinateRectCollideWithMapUsingQuadAlgorithm(PLAYER_RECT_FOR_MAP_COLLISIONS);
        }

        {
            misc::ScopedTimeTrial scopedTimerRobot(collisionTimeTrials_, collisionGridIndex_);
            DoesMapCoordinateRectCollideWithMapUsingGridAlgorithm(PLAYER_RECT_FOR_MAP_COLLISIONS);
        }

        {
            misc::ScopedTimeTrial scopedTimer(collisionTimeTrials_, collisionNaiveIndex_);

            didCollideWithMap = DoesMapCoordinateRectCollideWithMapUsingNaiveAlgorithm(
                PLAYER_RECT_FOR_MAP_COLLISIONS);
        }

        //// determine if the player's new position collides with a map object
        //// use naive algorithm if the collision rect count is low enough
        // if (collisionVec_.size() < 40)
        //{
        //    if (DoesMapCoordinateRectCollideWithMapUsingNaiveAlgorithm(
        //            PLAYER_RECT_FOR_MAP_COLLISIONS))
        //    {
        //        return true;
        //    }
        //}
        // else
        //{
        //    if (DoesMapCoordinateRectCollideWithMapUsingGridAlgorithm(
        //            PLAYER_RECT_FOR_MAP_COLLISIONS))
        //    {
        //        return true;
        //    }
        //}

        const auto ADJ_FOR_NPC_COLLISIONS_V { [&]() {
            return sfutil::Size(player_.GetView().SpriteRef(), 0.5f);
        }() };

        const sf::FloatRect PLAYER_RECT_FOR_NPC_COLLISIONS(
            PLAYER_POS_V.x + (ADJ_FOR_NPC_COLLISIONS_V.x * 1.0f),
            PLAYER_POS_V.y + (ADJ_FOR_NPC_COLLISIONS_V.x * 0.5f),
            ADJ_FOR_NPC_COLLISIONS_V.x * 0.25f,
            ADJ_FOR_NPC_COLLISIONS_V.y * 1.25f);

        bool didCollideWithNpc { false };

        {
            misc::ScopedTimeTrial scopedTimerRobot(collisionTimeTrials_, collisionNpcIndex_);

            for (auto & npcPtrModelPair : nonPlayers_)
            {
                const auto DID_COLLIDE_WITH_NPC { sfutil::intersects(
                    npcPtrModelPair.second.GetView().SpriteRef(), PLAYER_RECT_FOR_NPC_COLLISIONS) };

                if (DID_COLLIDE_WITH_NPC)
                {
                    player_.MovingIntoSet(npcPtrModelPair.first);

                    npcPtrModelPair.second.SetIsNextToPlayer(
                        true, player_.GetCenteredMapPos(), true);

                    didCollideWithNpc = true;
                    break;
                }
                else
                {
                    npcPtrModelPair.second.SetIsNextToPlayer(
                        false, player_.GetCenteredMapPos(), false);
                }
            }
        }

        if (collisionTimeTrials_.ContestCount() == 4)
        {
            M_HP_LOG_WRN("forcing end to collision timing trials");
            collisionTimeTrials_.EndAllContests();
        }

        if (didCollideWithMap)
        {
            return true;
        }

        if (didCollideWithNpc)
        {
            return true;
        }

        player_.MovingIntoReset();
        return false;
    }

    const sf::Vector2f Map::FindPlayerStartPos(
        const TransitionVec_t & TRANS_VEC,
        const Level::Enum LEVEL_TO_LOAD,
        const Level::Enum LEVEL_FROM)
    {
        TransitionOpt_t firstEntryTransitionFound(boost::none);

        for (const auto & TRANSITION : TRANS_VEC)
        {
            if (TRANSITION.IsEntry())
            {
                if (TRANSITION.WhichLevel() == LEVEL_FROM)
                {
                    return sfutil::CenterOf(TRANSITION.Rect());
                }

                if (!firstEntryTransitionFound)
                {
                    firstEntryTransitionFound = TRANSITION;
                }
            }
        }

        std::ostringstream errorSS;

        errorSS << "Tried to find where to place the player in the new map but failed to find the "
                   "matching entry transition among "
                << TRANS_VEC.size()
                << " possibilities. Transitioning from map=" << Level::ToString(LEVEL_FROM)
                << " to " << Level::ToString(LEVEL_TO_LOAD)
                << ".  You probably need to break out the Tiled app "
                   "and add or fix some transitions...";

        sf::Vector2f startPosV;
        if (!firstEntryTransitionFound)
        {
            startPosV = { 100.0f, 100.0f };

            errorSS << "  In fact no entry transitions were found at all.  This is definitely a "
                       "problem with the map file.  Use the Tiled app to add all the entry "
                       "transitions required.  Placing the player at a default location of "
                    << startPosV << ".  Good luck with that.";
        }
        else
        {
            startPosV = sfutil::CenterOf(firstEntryTransitionFound->Rect());
            errorSS << "  Placing the player into the first entry transition point found at "
                    << startPosV << " for map "
                    << map::Level::ToString(firstEntryTransitionFound->WhichLevel())
                    << ", which is better than crashing I guess.";
        }

        M_HP_LOG_ERR(errorSS.str());
        return startPosV;
    }

    const TransitionOpt_t Map::CheckIfEnteringTransition(
        const gui::Direction::Enum DIRECTION, const float ADJUSTMENT) const
    {
        const auto ADJ_PLAYER_POS_V { CalcAdjPlayerPos(DIRECTION, ADJUSTMENT) };

        for (const auto & TRANSITION : transitionVec_)
        {
            if ((TRANSITION.IsEntry() == false) && (TRANSITION.Rect().contains(ADJ_PLAYER_POS_V)))
            {
                return TRANSITION;
            }
        }

        return boost::none;
    }

    void Map::HandleEnteringTransition(const Transition & TRANSITION)
    {
        player_.StopWalking();

        const auto IS_LOCKED_DOOR {
            game::Game::Instance()->State().GetWorld().GetMaps().Current().IsDoorLocked(
                TRANSITION.WhichLevel())
        };

        if (IS_LOCKED_DOOR)
        {
            // prevent making duplicate locked door interactions
            const auto CURRENT_INTERACTION_PTR_OPT { interactionManager_.Current() };
            if (CURRENT_INTERACTION_PTR_OPT)
            {
                if (CURRENT_INTERACTION_PTR_OPT.value()->Type() == interact::Interact::Lock)
                {
                    return;
                }
            }

            interactionManager_.SetNext(interact::InteractionFactory::MakeLockedDoor(TRANSITION));
        }
        else
        {
            TransitionLevel(TRANSITION);
        }
    }

    const sf::Vector2f
        Map::CalcAdjPlayerPos(const gui::Direction::Enum DIRECTION, const float ADJUSTMENT) const
    {
        auto posToTest { mapDisplayUPtr_->PlayerPosMap() };

        switch (DIRECTION)
        {
            case gui::Direction::Left:
            {
                posToTest.x -= ADJUSTMENT;
                break;
            }
            case gui::Direction::Right:
            {
                posToTest.x += ADJUSTMENT;
                break;
            }
            case gui::Direction::Up:
            {
                posToTest.y -= ADJUSTMENT;
                break;
            }
            case gui::Direction::Down:
            {
                posToTest.y += ADJUSTMENT;
                break;
            }
            case gui::Direction::Count:
            default:
            {
                break;
            }
        }

        return posToTest;
    }

    void Map::PlayTransitionSfx(
        const gui::sound_effect::MapTransition TRANS_TYPE, const bool IS_DOOR_OPENING) const
    {
        if (TRANS_TYPE != gui::sound_effect::MapTransition::Count)
        {
            const auto DOOR_ACTION { (
                (IS_DOOR_OPENING) ? gui::sound_effect::DoorAction::Open
                                  : gui::sound_effect::DoorAction::Close) };

            const auto TRANS_SFX { gui::sound_effect::RandomMapTransitionSfx(
                TRANS_TYPE, DOOR_ACTION) };

            if ((TRANS_SFX == gui::sound_effect::Stairs) && (IS_DOOR_OPENING))
            {
                return;
            }

            if (TRANS_SFX != gui::sound_effect::Count)
            {
                if (IS_DOOR_OPENING)
                {
                    gui::SoundManager::Instance()->SoundEffectPlay(TRANS_SFX);
                }
                else
                {
                    gui::SoundManager::Instance()->SoundEffectPlay(TRANS_SFX, 1.0f);
                }
            }
        }
    }

    void Map::UpdateWalkSfx()
    {
        const auto NEW_IS_WALKING { (Player().GetView().WhichPose() == avatar::Pose::Walking) };
        const auto NEW_WALK_SFX { walkSfxLayers_.FindSfx(mapDisplayUPtr_->PlayerPosMap()) };

        if (NEW_IS_WALKING && ((NEW_WALK_SFX != walkSfx_) || (NEW_IS_WALKING != walkSfxIsWalking_)))
        {
            StopWalkSfxIfValid();

            if (NEW_WALK_SFX != gui::sound_effect::Count)
            {
                gui::SoundManager::Instance()->SoundEffectPlay(
                    NEW_WALK_SFX, true, WALK_SFX_VOLUME_RATIO_);
            }
        }
        else if ((NEW_IS_WALKING == false) && (gui::sound_effect::Count != walkSfx_))
        {
            gui::SoundManager::Instance()->SoundEffectStop(walkSfx_);
        }

        walkSfx_ = NEW_WALK_SFX;
        walkSfxIsWalking_ = NEW_IS_WALKING;
    }

    void Map::ResetNonPlayers()
    {
        nonPlayers_.Clear();

        for (const auto & NPC_PTR :
             game::Game::Instance()->State().GetWorld().GetMaps().Current().SpecificNPCs())
        {
            AddNonPlayerAvatar(NPC_PTR);
        }

        for (const auto & NPC_PTR :
             game::Game::Instance()->State().GetWorld().GetMaps().Current().RandomNPCs())
        {
            AddNonPlayerAvatar(NPC_PTR);
        }
    }

    void Map::AddNonPlayerAvatar(const game::NpcPtr_t NPC_PTR)
    {
        const auto WALK_BOUNDS_INDEX { NPC_PTR->WalkBoundsIndex() };
        const auto AVATAR_IMAGE_ENUM { NPC_PTR->AvatarImage() };

        std::vector<sf::FloatRect> walkRects;
        const auto WAS_FOUND { walkRectVecMap_.Find(WALK_BOUNDS_INDEX, walkRects) };

        if (WAS_FOUND == false)
        {
            std::ostringstream ss;
            ss << "map::Map::AddNonPlayerAvatar(avatar="
               << avatar::Avatar::ToString(AVATAR_IMAGE_ENUM)
               << ", walkBoundsIndex=" << WALK_BOUNDS_INDEX
               << ") but that walkBoundsIndex was not found in all the walk bounds loaded from the "
                  "map file.  Valid indexes are: [";

            for (const auto & WALK_BOUNDS_PAIR : walkRectVecMap_)
            {
                ss << WALK_BOUNDS_PAIR.first << ", ";
            }

            ss << "]";

            throw std::runtime_error(ss.str());
        }

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (walkRects.empty() == false),
            "map::Map::AddNonPlayerAvatar(avatar="
                << avatar::Avatar::ToString(AVATAR_IMAGE_ENUM)
                << ", walkBoundsIndex=" << WALK_BOUNDS_INDEX
                << ") but that walkBoundsIndex, while valid, was "
                   "not associated with any actual walk bounds.  "
                   "The vector was empty.");

        sf::Vector2f startingPosV(-1.0f, -1.0f);
        std::size_t walkRectsIndex { walkRects.size() };
        FindLocationToPlaceAvatar(walkRects, walkRectsIndex, startingPosV);
        if ((walkRectsIndex >= walkRects.size()) || (startingPosV.x < 0.0f)
            || (startingPosV.y < 0.0f))
        {
            M_HP_LOG_ERR(
                "map::Map::FindLocationToPlaceAvatar() failed to find an open spot to put an "
                "NPC, so it will not be added to the map at all.");
        }
        else
        {
            nonPlayers_.Append(
                NPC_PTR, avatar::Model(AVATAR_IMAGE_ENUM, walkRects, walkRectsIndex, startingPosV));
        }
    }

    const sf::Sprite Map::GetNpcDefaultPoseSprite(const game::NpcPtr_t NPC_PTR) const
    {
        for (const auto & NPC_PTR_MODEL_PAIR : nonPlayers_)
        {
            if (NPC_PTR_MODEL_PAIR.first == NPC_PTR)
            {
                return NPC_PTR_MODEL_PAIR.second.GetView().DefaultPoseSprite();
            }
        }

        return sf::Sprite();
    }

    bool Map::IsPosTooCloseToAvatar(
        const sf::Vector2f & POS_V,
        const avatar::Model & AVATAR,
        const float DISTANCE_TOO_CLOSE) const
    {
        const auto DISTANCE_HORIZ { std::abs(AVATAR.GetCenteredMapPos().x - POS_V.x) };
        const auto DISTANCE_VERT { std::abs(AVATAR.GetCenteredMapPos().y - POS_V.y) };
        return ((DISTANCE_HORIZ < DISTANCE_TOO_CLOSE) && (DISTANCE_VERT < DISTANCE_TOO_CLOSE));
    }

    bool Map::IsPosTooCloseToAnyAvatars(const sf::Vector2f & POS_V) const
    {
        const auto PLAYER_GLOBAL_BOUNDS_RECT { player_.GetView().SpriteRef().getGlobalBounds() };

        const auto DISTANCE_TOO_CLOSE {
            std::max(PLAYER_GLOBAL_BOUNDS_RECT.width, PLAYER_GLOBAL_BOUNDS_RECT.height) * 1.25f
        };

        if (IsPosTooCloseToAvatar(POS_V, player_, DISTANCE_TOO_CLOSE))
        {
            return true;
        }

        for (const auto & NPC_PTR_MODEL_PAIR : nonPlayers_)
        {
            if (IsPosTooCloseToAvatar(POS_V, NPC_PTR_MODEL_PAIR.second, DISTANCE_TOO_CLOSE))
            {
                return true;
            }
        }

        return false;
    }

    void Map::FindLocationToPlaceAvatar(
        const FloatRectVec_t & WALK_RECTS,
        std::size_t & walkRectsIndex,
        sf::Vector2f & startingPosV)
    {
        const auto ATTEMPT_COUNT_MAX { 10000 };

        auto attemptCounter { 0 };

        do
        {
            walkRectsIndex = misc::random::SizeT(WALK_RECTS.size() - 1);
            const auto RECT { WALK_RECTS.at(walkRectsIndex) };
            const auto X { misc::random::Float(RECT.left, RECT.left + RECT.width) };
            const auto Y { misc::random::Float(RECT.top, RECT.top + RECT.height) };
            startingPosV = sf::Vector2f(X, Y);

            if (IsPosTooCloseToAnyAvatars(startingPosV) == false)
            {
                return;
            }

        } while (++attemptCounter < ATTEMPT_COUNT_MAX);

        // no position found so set everything to invalid
        walkRectsIndex = WALK_RECTS.size();
        startingPosV = sf::Vector2f(-1.0f, -1.0f);
    }

    void Map::StopWalkSfxIfValid()
    {
        if (gui::sound_effect::Count != walkSfx_)
        {
            gui::SoundManager::Instance()->SoundEffectStop(walkSfx_);
        }
    }

    bool Map::DoesMapCoordinateRectCollideWithMapUsingNaiveAlgorithm(const sf::FloatRect & MAP_RECT)
    {
        for (const auto & COLLISION_RECT : collisionVec_)
        {
            if (COLLISION_RECT.intersects(MAP_RECT))
            {
                return true;
            }
        }

        return false;
    }

    bool Map::DoesMapCoordinateRectCollideWithMapUsingGridAlgorithm(const sf::FloatRect & MAP_RECT)
    {
        return collisionGrid_.DoesRectCollide(MAP_RECT);
    }

    bool Map::DoesMapCoordinateRectCollideWithMapUsingQuadAlgorithm(const sf::FloatRect & MAP_RECT)
    {
        return quadTree_.DoesRectCollide(MAP_RECT);
    }

} // namespace map
} // namespace heroespath
