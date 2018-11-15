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
        , transitionVec_()
        , level_(Level::Count)
        , player_(game::Game::Instance()->State().Party().Avatar())
        , nonPlayersPtrModelMap_()
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

        player_.CenteredMapPos(mapDisplayUPtr_->PlayerPosMap());

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

        if (DoesPlayerCollideAfterMove(DIRECTION, PLAYER_MOVE_DISTANCE_))
        {
            return false;
        }

        if (DetectCollisionWithExitTransitionAndHandle(DIRECTION, PLAYER_MOVE_DISTANCE_))
        {
            return true;
        }

        if (interactionManager_.Current())
        {
            interactionManager_.RemoveCurrent();
        }

        // at this point failure to move means the player hit the edges of the map
        const auto DID_ACTUALLY_MOVE { mapDisplayUPtr_->Move(DIRECTION, PLAYER_MOVE_DISTANCE_) };
        player_.CenteredMapPos(mapDisplayUPtr_->PlayerPosMap());
        return DID_ACTUALLY_MOVE;
    }

    void Map::MoveNonPlayers()
    {
        const auto PLAYER_RECT_FOR_COLL_DET { AvatarRectForCollisionDetection(player_) };

        for (auto & npcPtrModelPair : nonPlayersPtrModelMap_)
        {
            MoveNonPlayer(
                npcPtrModelPair.second, NONPLAYER_MOVE_DISTANCE_, PLAYER_RECT_FOR_COLL_DET);
        }
    }

    void Map::MoveNonPlayer(
        avatar::Model & npcAvatar,
        const float MOVE_AMOUNT,
        const sf::FloatRect & PLAYER_RECT_FOR_COLL_DET)
    {
        if (npcAvatar.IsWalking() == false)
        {
            return;
        }

        const auto DIRECTION { npcAvatar.GetView().Direction() };

        const auto MOVED_NPC_REC_FOR_COLL_DET { AvatarRectForCollisionDetectionAfterMove(
            npcAvatar, DIRECTION, MOVE_AMOUNT) };

        // const auto NPC_RECT_FOR_PLAYER_COLLISIONS { [&]() {
        //    auto rect { NPC_RECT_ADJ };
        //    rect.left -= PLAYER_ADJ_POS_V.x * 0.25f;
        //    rect.width *= 1.75f;
        //    rect.height *= 1.75f;
        //    return rect;
        //}() };

        if (MOVED_NPC_REC_FOR_COLL_DET.intersects(PLAYER_RECT_FOR_COLL_DET))
        {
            npcAvatar.StopWalking();
            npcAvatar.SetIsNextToPlayer(true, player_.CenteredMapPos(), false);
            return;
        }

        const auto DID_COLLIDE_WTIH_OTHER_NPC { DoesAvatarAtRectCollideWithNPCs(
            npcAvatar, MOVED_NPC_REC_FOR_COLL_DET) };

        if (DID_COLLIDE_WTIH_OTHER_NPC)
        {
            npcAvatar.StopWalking();
        }
        else
        {
            npcAvatar.Move(MoveVector(DIRECTION, MOVE_AMOUNT));
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

        for (auto & npcPtrModelPair : nonPlayersPtrModelMap_)
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

    bool Map::DoesPlayerCollideAfterMove(
        const gui::Direction::Enum DIRECTION, const float MOVE_AMOUNT)
    {
        const auto MOVED_PLAYER_RECT_FOR_COLL_DET { AvatarRectForCollisionDetectionAfterMove(
            player_, DIRECTION, MOVE_AMOUNT) };

        if (DoesRectCollideWithMap(MOVED_PLAYER_RECT_FOR_COLL_DET))
        {
            return true;
        }

        const auto COLLIDING_ITER { std::find_if(
            std::begin(nonPlayersPtrModelMap_),
            std::end(nonPlayersPtrModelMap_),
            [&](const auto & NPC_PTR_MODEL_PAIR) {
                const auto NPC_RECT_FOR_COLL_DET { AvatarRectForCollisionDetection(
                    NPC_PTR_MODEL_PAIR.second) };

                return MOVED_PLAYER_RECT_FOR_COLL_DET.intersects(NPC_RECT_FOR_COLL_DET);
            }) };

        if (COLLIDING_ITER == std::end(nonPlayersPtrModelMap_))
        {
            COLLIDING_ITER->second.SetIsNextToPlayer(false, player_.CenteredMapPos(), false);
            player_.MovingIntoReset();
            return false;
        }
        else
        {

            player_.MovingIntoSet(COLLIDING_ITER->first);
            COLLIDING_ITER->second.SetIsNextToPlayer(true, player_.CenteredMapPos(), true);
            return true;
        }
    }

    const sf::Vector2f Map::FindPlayerStartPos(
        const TransitionVec_t & TRANS_VEC,
        const Level::Enum LEVEL_TO_LOAD,
        const Level::Enum LEVEL_FROM)
    {
        for (const auto & TRANSITION : TRANS_VEC)
        {
            if (TRANSITION.IsEntry() && (TRANSITION.WhichLevel() == LEVEL_FROM))
            {
                return sfutil::CenterOf(TRANSITION.Rect());
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

        const auto FOUND_ITER { std::find_if(
            std::begin(TRANS_VEC), std::end(TRANS_VEC), [&](const auto & TRANSITION) {
                return TRANSITION.IsEntry();
            }) };

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (FOUND_ITER != std::end(TRANS_VEC)),
            errorSS.str()
                << "  In fact no entry transitions were found at all.  This is definitely a "
                   "problem with the map file.  Use the Tiled app to add all the entry "
                   "transitions required and use the live/instrumented test mode to make sure "
                   "they are all there and correct.  Since there is no known valid point in "
                   "the map to place the player, crashing.");

        M_HP_LOG_ERR(
            errorSS.str() << "  Placing the player into the first entry transition point found, "
                             "which is for map "
                          << map::Level::ToString(FOUND_ITER->WhichLevel())
                          << ", which is better than crashing I guess.");

        return sfutil::CenterOf(FOUND_ITER->Rect());
    }

    bool Map::DetectCollisionWithExitTransitionAndHandle(
        const gui::Direction::Enum DIRECTION, const float ADJUSTMENT)
    {
        const auto PLAYER_MOVED_CENTERED_MAP_POS_V { player_.CenteredMapPos()
                                                     + MoveVector(DIRECTION, ADJUSTMENT) };

        for (const auto & TRANSITION : transitionVec_)
        {
            if ((TRANSITION.IsEntry() == false)
                && (TRANSITION.Rect().contains(PLAYER_MOVED_CENTERED_MAP_POS_V)))
            {
                HandleExitTransition(TRANSITION);
                return true;
            }
        }

        return false;
    }

    void Map::HandleExitTransition(const Transition & TRANSITION)
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

            if ((TRANS_SFX == gui::sound_effect::Stairs) && IS_DOOR_OPENING)
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
        const auto NEW_IS_WALKING { (player_.GetView().WhichPose() == avatar::Pose::Walking) };
        const auto NEW_WALK_SFX { walkSfxLayers_.FindSfx(player_.CenteredMapPos()) };

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
        nonPlayersPtrModelMap_.Clear();

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
            nonPlayersPtrModelMap_.Append(
                NPC_PTR, avatar::Model(AVATAR_IMAGE_ENUM, walkRects, walkRectsIndex, startingPosV));
        }
    }

    const sf::Sprite Map::GetNpcDefaultPoseSprite(const game::NpcPtr_t NPC_PTR) const
    {
        for (const auto & NPC_PTR_MODEL_PAIR : nonPlayersPtrModelMap_)
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
        const auto DISTANCE_HORIZ { std::abs(AVATAR.CenteredMapPos().x - POS_V.x) };
        const auto DISTANCE_VERT { std::abs(AVATAR.CenteredMapPos().y - POS_V.y) };
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

        for (const auto & NPC_PTR_MODEL_PAIR : nonPlayersPtrModelMap_)
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

    bool Map::DoesRectCollideWithMap(const sf::FloatRect & RECT) const
    {
        // TEMP TODO REMOVE AFTER TESTING run all three collision detection algorithms so that each
        // can be timed

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

        bool didCollideWithMap { false };

        {
            misc::ScopedTimeTrial scopedTimer(collisionTimeTrials_, collisionQuadIndex_);
            DoesRectCollideWithMap_UsingAlgorithm_Quad(RECT);
        }

        {
            misc::ScopedTimeTrial scopedTimerJane(collisionTimeTrials_, collisionNaiveIndex_);
            DoesRectCollideWithMap_UsingAlgorithm_Naive(RECT);
        }

        {
            misc::ScopedTimeTrial scopedTimerRobot(collisionTimeTrials_, collisionGridIndex_);
            didCollideWithMap = DoesRectCollideWithMap_UsingAlgorithm_Grid(RECT);
        }

        if (collisionTimeTrials_.ContestCount() == 6)
        {
            collisionTimeTrials_.EndAllContests();
        }

        return didCollideWithMap;
    }

    bool Map::DoesRectCollideWithMap_UsingAlgorithm_Naive(const sf::FloatRect & RECT) const
    {
        for (const auto & COLLISION_RECT : collisionVec_)
        {
            if (COLLISION_RECT.intersects(RECT))
            {
                return true;
            }
        }

        return false;
    }

    bool Map::DoesRectCollideWithMap_UsingAlgorithm_Quad(const sf::FloatRect & RECT) const
    {
        return collisionGrid_.DoesRectCollide(RECT);
    }

    bool Map::DoesRectCollideWithMap_UsingAlgorithm_Grid(const sf::FloatRect & RECT) const
    {
        return quadTree_.DoesRectCollide(RECT);
    }

    bool Map::DoesAvatarAtRectCollideWithNPCs(
        const avatar::Model & AVATAR, const sf::FloatRect & AVATAR_RECT_FOR_COLL_DET) const
    {
        for (const auto & NPC_PTR_MODEL_PAIR : nonPlayersPtrModelMap_)
        {
            if (&AVATAR == &NPC_PTR_MODEL_PAIR.second)
            {
                continue;
            }

            const auto NPC_RECT_FOR_COLL_DET { AvatarRectForCollisionDetection(
                NPC_PTR_MODEL_PAIR.second) };

            if (AVATAR_RECT_FOR_COLL_DET.intersects(NPC_RECT_FOR_COLL_DET))
            {
                return true;
            }
        }

        return false;
    }

    const sf::Vector2f
        Map::MoveVector(const gui::Direction::Enum DIRECTION, const float MOVE_AMOUNT) const
    {
        sf::Vector2f moveV(0.0f, 0.0f);

        if ((DIRECTION == gui::Direction::Left) || (DIRECTION == gui::Direction::Right))
        {
            moveV.x = MOVE_AMOUNT;
        }
        else
        {
            moveV.y = MOVE_AMOUNT;
        }

        if ((DIRECTION == gui::Direction::Left) || (DIRECTION == gui::Direction::Up))
        {
            moveV *= -1.0f;
        }

        return moveV;
    }

    const sf::FloatRect Map::AvatarCenteredMapRect(const avatar::Model & AVATAR) const
    {
        const sf::Vector2f AVATAR_SIZE_V { AVATAR.GetView().MapSize() };
        return sf::FloatRect((AVATAR.CenteredMapPos() - (AVATAR_SIZE_V * 0.5f)), AVATAR_SIZE_V);
    }

    const sf::FloatRect
        Map::ResizeAvatarRectForCollisionDetection(const sf::FloatRect & RECT_ORIG) const
    {
        return sfutil::ScaleAndReCenterCopy(RECT_ORIG, sf::Vector2f(0.5f, 0.7f));
    }

    const sf::FloatRect Map::AvatarRectForCollisionDetection(const avatar::Model & AVATAR) const
    {
        return ResizeAvatarRectForCollisionDetection(AvatarCenteredMapRect(AVATAR));
    }

    const sf::FloatRect Map::AvatarRectForCollisionDetectionAfterMove(
        const avatar::Model & AVATAR,
        const gui::Direction::Enum DIRECTION,
        const float MOVE_AMOUNT) const
    {
        return ResizeAvatarRectForCollisionDetection(
            sfutil::MoveCopy(AvatarCenteredMapRect(AVATAR), MoveVector(DIRECTION, MOVE_AMOUNT)));
    }

} // namespace map
} // namespace heroespath
