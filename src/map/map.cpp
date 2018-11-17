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
        , mapDisplayUPtr_(std::make_unique<map::MapDisplay>(REGION))
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
        , AVATAR_TO_AVATAR_COLLISION_DETECTION_RECT_HEIGHT_(
              player_.CurrentSprite().getGlobalBounds().height * 0.333f)
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

        // the order of everything below is critical so be careful making changes

        game::Game::Instance()->State().GetWorld().HandleLevelUnload(LEVEL_FROM);
        game::Game::Instance()->State().GetWorld().HandleLevelLoad(LEVEL_TO_LOAD);

        const auto PLAYER_START_POS_V { FindPlayerStartPos(
            transitionVec_, LEVEL_TO_LOAD, LEVEL_FROM) };

        if (IS_TEST_LOAD == false)
        {
            mapDisplayUPtr_->Load(PLAYER_START_POS_V, animInfoVec);
        }

        player_.MapPos(mapDisplayUPtr_->PlayerPosMap());

        ResetNonPlayers();

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

        // calculate the new player position and map rect
        const auto MOVE_V { MoveVector(DIRECTION, PLAYER_MOVE_DISTANCE_) };
        const auto MOVED_PLAYER_POS_V { (player_.MapPos() + MOVE_V) };

        const auto MOVED_PLAYER_RECT { sfutil::MoveCopy(
            player_.CurrentSprite().getGlobalBounds(), MOVE_V) };

        if (DoesRectCollideWithMap(AdjustRectForCollisionDetectionWithMap(MOVED_PLAYER_RECT)))
        {
            return false;
        }

        if (UpdatePlayerToNonPlayerCollisionStatus(
                AdjustRectForCollisionDetectionWithNonPlayers(MOVED_PLAYER_RECT)))
        {
            return false;
        }

        // check if the player just walked into an exit transition
        const auto COLLIDING_EXIT_TRANSITION_ITER { std::find_if(
            std::begin(transitionVec_), std::end(transitionVec_), [&](const auto & TRANSITION) {
                return (
                    (TRANSITION.IsEntry() == false)
                    && (TRANSITION.Rect().contains(MOVED_PLAYER_POS_V)));
            }) };

        if (COLLIDING_EXIT_TRANSITION_ITER != std::end(transitionVec_))
        {
            HandleExitTransition(*COLLIDING_EXIT_TRANSITION_ITER);
            return false;
        }

        // at this point failure to move means the player hit the edges of the map or screen
        if (mapDisplayUPtr_->Move(DIRECTION, PLAYER_MOVE_DISTANCE_) == false)
        {
            return false;
        }

        if (interactionManager_.Current())
        {
            interactionManager_.RemoveCurrent();
        }

        player_.MapPos(mapDisplayUPtr_->PlayerPosMap());
        return true;
    }

    void Map::MoveNonPlayers()
    {
        const auto PLAYER_RECT_FOR_COLL_DET_WITH_NPCS {
            AdjustRectForCollisionDetectionWithNonPlayers(player_.CurrentSprite().getGlobalBounds())
        };

        for (auto & npcPtrModelPair : nonPlayersPtrModelMap_)
        {
            auto & avatar { npcPtrModelPair.second };

            if (avatar.IsWalking() == false)
            {
                continue;
            }

            const auto MOVE_V { MoveVector(avatar.Direction(), NONPLAYER_MOVE_DISTANCE_) };

            const auto MOVED_RECT_FOR_COLL_DET_WITH_OTHER_NPC { sfutil::MoveCopy(
                AdjustRectForCollisionDetectionWithNonPlayers(
                    avatar.CurrentSprite().getGlobalBounds()),
                MOVE_V) };

            if (MOVED_RECT_FOR_COLL_DET_WITH_OTHER_NPC.intersects(
                    PLAYER_RECT_FOR_COLL_DET_WITH_NPCS))
            {
                avatar.StopWalking();
                avatar.SetIsNextToPlayer(true, player_.MapPos(), false);
                continue;
            }

            if (DoesAvatarAtRectCollideWithAnyNonPlayers(
                    avatar.Id(), MOVED_RECT_FOR_COLL_DET_WITH_OTHER_NPC))
            {
                avatar.StopWalking();
                continue;
            }

            avatar.Move(MOVE_V);
        }
    }

    void Map::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        target.draw(*mapDisplayUPtr_, states);
    }

    void Map::Update(const float TIME_ELAPSED)
    {
        player_.Update(TIME_ELAPSED);

        for (auto & npcPtrModelPair : nonPlayersPtrModelMap_)
        {
            npcPtrModelPair.second.Update(TIME_ELAPSED);
        }

        SetupAvatarSpritesForDrawing();

        mapDisplayUPtr_->Update(TIME_ELAPSED);

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

    bool Map::UpdatePlayerToNonPlayerCollisionStatus(const sf::FloatRect & MOVED_PLAYER_RECT)
    {
        bool hasAlreadyCollided { false };
        for (auto & npcPtrAvatarPair : nonPlayersPtrModelMap_)
        {
            auto & otherAvatar { npcPtrAvatarPair.second };

            if (hasAlreadyCollided)
            {
                otherAvatar.SetIsNextToPlayer(false, player_.MapPos(), false);
            }
            else
            {
                const auto OTHER_AVATAR_RECT { AdjustRectForCollisionDetectionWithNonPlayers(
                    otherAvatar.CurrentSprite().getGlobalBounds()) };

                if (MOVED_PLAYER_RECT.intersects(OTHER_AVATAR_RECT))
                {
                    hasAlreadyCollided = true;
                    player_.MovingIntoSet(npcPtrAvatarPair.first);
                    otherAvatar.SetIsNextToPlayer(true, player_.MapPos(), true);
                }
            }
        }

        if (!hasAlreadyCollided)
        {
            player_.MovingIntoReset();
        }

        return hasAlreadyCollided;
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
        if (TRANS_TYPE == gui::sound_effect::MapTransition::Count)
        {
            return;
        }

        const auto DOOR_ACTION { (
            (IS_DOOR_OPENING) ? gui::sound_effect::DoorAction::Open
                              : gui::sound_effect::DoorAction::Close) };

        const auto TRANS_SFX { gui::sound_effect::RandomMapTransitionSfx(TRANS_TYPE, DOOR_ACTION) };

        if ((TRANS_SFX == gui::sound_effect::Stairs) && IS_DOOR_OPENING)
        {
            return;
        }

        if (TRANS_SFX == gui::sound_effect::Count)
        {
            return;
        }

        if (IS_DOOR_OPENING)
        {
            gui::SoundManager::Instance()->SoundEffectPlay(TRANS_SFX);
        }
        else
        {
            gui::SoundManager::Instance()->SoundEffectPlay(TRANS_SFX, 1.0f);
        }
    }

    void Map::UpdateWalkSfx()
    {
        const auto NEW_IS_WALKING { (player_.WhichPose() == avatar::Pose::Walking) };
        const auto NEW_WALK_SFX { walkSfxLayers_.FindSfx(player_.MapPos()) };

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
        const auto NPC_WALK_BOUNDS_SET_INDEX { NPC_PTR->WalkBoundsSetIndex() };
        const auto NPC_AVATAR_IMAGE_ENUM { NPC_PTR->AvatarImage() };

        std::vector<sf::FloatRect> walkRects;
        const auto WAS_FOUND { walkRectVecMap_.Find(NPC_WALK_BOUNDS_SET_INDEX, walkRects) };

        if (WAS_FOUND == false)
        {
            std::ostringstream ss;
            ss << " That walkBoundsSetIndex was not found in the map file. (npc="
               << NPC_PTR->ToString() << ")" << M_HP_VAR_STR(NPC_WALK_BOUNDS_SET_INDEX)
               << "  Valid set indexes are: [";

            for (const auto & WALK_BOUNDS_PAIR : walkRectVecMap_)
            {
                ss << WALK_BOUNDS_PAIR.first << ", ";
            }

            ss << "].  So this npc will not be placed in the map.";

            M_HP_LOG_ERR(ss.str());
            return;
        }

        if (walkRects.empty())
        {
            M_HP_LOG_ERR(
                "That walkBoundsSetIndex, while valid, was "
                "not associated with any actual walk bounds.  "
                "The vector was empty.  So this npc will not be placed in the map."
                << NPC_PTR->ToString() << ")" << M_HP_VAR_STR(NPC_WALK_BOUNDS_SET_INDEX));

            return;
        }

        const auto [WAS_VALID_LOCATION_FOUND, WALK_BOUNDS_RECT_INDEX, MAP_POS_V]
            = PickRandomLocationToPlaceAvatar(walkRects);

        if (WAS_VALID_LOCATION_FOUND)
        {
            nonPlayersPtrModelMap_.Append(
                NPC_PTR,
                avatar::Model(NPC_AVATAR_IMAGE_ENUM, walkRects, WALK_BOUNDS_RECT_INDEX, MAP_POS_V));
        }
        else
        {
            M_HP_LOG_ERR(
                "After thousands of attempts, no free space (not already taken up by another NPC) "
                "could be found to place this NPC, so it will not be placed in the map."
                << NPC_PTR->ToString() << ")" << M_HP_VAR_STR(NPC_WALK_BOUNDS_SET_INDEX));
        }
    }

    const sf::Sprite Map::GetNpcDefaultPoseSprite(const game::NpcPtr_t NPC_PTR) const
    {
        const auto FOUND_ITER { nonPlayersPtrModelMap_.Find(NPC_PTR) };

        if (FOUND_ITER == std::end(nonPlayersPtrModelMap_))
        {
            return sf::Sprite();
        }
        else
        {
            return FOUND_ITER->second.DefaultPoseSprite();
        }
    }

    const std::tuple<bool, std::size_t, sf::Vector2f>
        Map::PickRandomLocationToPlaceAvatar(const FloatRectVec_t & WALK_RECTS) const
    {
        struct WalkRegion
        {
            sf::FloatRect rect;
            std::size_t index;
        };

        std::vector<WalkRegion> walkRegions;

        for (std::size_t index(0); index < WALK_RECTS.size(); ++index)
        {
            WalkRegion walkRegion;
            walkRegion.index = index;
            walkRegion.rect = WALK_RECTS.at(index);
            walkRegions.emplace_back(walkRegion);
        }

        // shuffle so that each NPC likely starts in a random walk rect
        misc::Vector::ShuffleVec(walkRegions);

        const auto PLAYER_RECT { AdjustRectForCollisionDetectionWithNonPlayers(
            player_.CurrentSprite().getGlobalBounds()) };

        // We have not yet made this NPC's avatar::Model yet so we don't technically know the size
        // of it's sprite, and yes, some avatar sprites are scaled differently than others.  But we
        // need to check for collisions with other NPCs, so we need that size.  Instead we will use
        // the fact that the player's sprite size is as big or bigger than all others, and so we can
        // use it as an overestimation.
        const auto AVATAR_IMAGE_SIZE_V { sfutil::Size(PLAYER_RECT) };

        for (const auto & WALK_REGION : walkRegions)
        {
            const auto ATTEMPTS_PER_WALK_RECT { 1000 };

            for (int attemptCounter(0); attemptCounter < ATTEMPTS_PER_WALK_RECT; ++attemptCounter)
            {
                const sf::Vector2f RAW_PROPOSED_NPC_POS_V(
                    misc::random::Float(WALK_REGION.rect.left, sfutil::Right(WALK_REGION.rect)),
                    misc::random::Float(WALK_REGION.rect.top, sfutil::Bottom(WALK_REGION.rect)));

                const sf::FloatRect RAW_PROPOSED_NPC_RECT(
                    (RAW_PROPOSED_NPC_POS_V - (AVATAR_IMAGE_SIZE_V * 0.5f)), AVATAR_IMAGE_SIZE_V);

                const auto FINAL_PROPOSED_NPC_RECT { AdjustRectForCollisionDetectionWithNonPlayers(
                    RAW_PROPOSED_NPC_RECT) };

                if (FINAL_PROPOSED_NPC_RECT.intersects(PLAYER_RECT))
                {
                    continue;
                }

                if (DoesAvatarAtRectCollideWithAnyNonPlayers(
                        avatar::Model::ID_INVALID_, FINAL_PROPOSED_NPC_RECT))
                {
                    continue;
                }

                return std::make_tuple(true, WALK_REGION.index, RAW_PROPOSED_NPC_POS_V);
            }
        }

        return std::make_tuple(false, 0, sf::Vector2f());
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
        // TEMP TODO REMOVE AFTER TESTING run all three collision detection algorithms so that
        // each can be timed

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

    const sf::FloatRect
        Map::AdjustRectForCollisionDetectionWithMap(const sf::FloatRect & RECT) const
    {
        return sfutil::ScaleAndReCenterCopy(RECT, sf::Vector2f(0.5f, 0.7f));
    }

    const sf::FloatRect
        Map::AdjustRectForCollisionDetectionWithNonPlayers(const sf::FloatRect & RECT_ORIG) const
    {
        sf::FloatRect rectNew { AdjustRectForCollisionDetectionWithMap(RECT_ORIG) };

        if (rectNew.height > AVATAR_TO_AVATAR_COLLISION_DETECTION_RECT_HEIGHT_)
        {
            rectNew.top
                = sfutil::Bottom(rectNew) - AVATAR_TO_AVATAR_COLLISION_DETECTION_RECT_HEIGHT_;

            rectNew.height = AVATAR_TO_AVATAR_COLLISION_DETECTION_RECT_HEIGHT_;
        }

        return rectNew;
    }

    void Map::SetupAvatarSpritesForDrawing()
    {
        auto & cachedAvatarSprites { mapDisplayUPtr_->AvatarSprites() };

        if (cachedAvatarSprites.size()
            != (nonPlayersPtrModelMap_.Size() + 1)) // plus one for the player
        {
            cachedAvatarSprites.clear();

            for (const auto & NPCPTR_AVATAR_PAIR : nonPlayersPtrModelMap_)
            {
                cachedAvatarSprites.emplace_back(NPCPTR_AVATAR_PAIR.second.CurrentSprite());
            }

            cachedAvatarSprites.emplace_back(player_.CurrentSprite());
            return;
        }

        const auto CURRENT_NONPLAYER_SPRITES_COUNT { nonPlayersPtrModelMap_.Size() };
        const auto CACHED_SPRITES_COUNT { cachedAvatarSprites.size() };

        if ((CURRENT_NONPLAYER_SPRITES_COUNT == 0) || (CACHED_SPRITES_COUNT == 0)
            || (CACHED_SPRITES_COUNT != (CURRENT_NONPLAYER_SPRITES_COUNT + 1)))
        {
            return;
        }

        auto updateSprite = [&](sf::Sprite & cachedSprite, const sf::Sprite & CURRENT_SPRITE) {
            cachedSprite.setPosition(CURRENT_SPRITE.getPosition());
            cachedSprite.setTextureRect(CURRENT_SPRITE.getTextureRect());
        };

        std::size_t index { 0 };
        for (; index < CURRENT_NONPLAYER_SPRITES_COUNT; ++index)
        {
            updateSprite(
                cachedAvatarSprites.at(index), nonPlayersPtrModelMap_.At(index).CurrentSprite());
        }

        updateSprite(cachedAvatarSprites.at(index), player_.CurrentSprite());
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

    bool Map::DoesAvatarAtRectCollideWithAnyNonPlayers(
        const std::size_t AVATAR_ID, const sf::FloatRect & RECT_ADJ_FOR_COLL_DET_WITH_NPCS) const
    {
        return std::any_of(
            std::begin(nonPlayersPtrModelMap_),
            std::end(nonPlayersPtrModelMap_),
            [&](const auto & NPCPTR_AVATAR_PAIR) {
                const auto & OTHER_AVATAR { NPCPTR_AVATAR_PAIR.second };

                if (AVATAR_ID == OTHER_AVATAR.Id())
                {
                    return false;
                }

                const auto OTHER_AVATAR_RECT_FOR_COLL_DET_WITH_OTHER_NPC {
                    AdjustRectForCollisionDetectionWithNonPlayers(
                        OTHER_AVATAR.CurrentSprite().getGlobalBounds())
                };

                return RECT_ADJ_FOR_COLL_DET_WITH_NPCS.intersects(
                    OTHER_AVATAR_RECT_FOR_COLL_DET_WITH_OTHER_NPC);
            });
    }

} // namespace map
} // namespace heroespath
