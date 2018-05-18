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
#include "game/game-data-file.hpp"
#include "game/game.hpp"
#include "interact/interaction-factory.hpp"
#include "map/layout.hpp"
#include "map/map-anim.hpp"
#include "map/map-display.hpp"
#include "misc/random.hpp"
#include "misc/vector-map.hpp"
#include "sfml-util/sfml-util.hpp"
#include "sfml-util/sound-manager.hpp"
#include "state/game-state.hpp"
#include "state/world.hpp"

#include <exception>
#include <sstream>

namespace heroespath
{
namespace map
{

    // These are different so that players can always move faster than, and catch, NPCs.
    const float Map::PLAYER_MOVE_DISTANCE_{ 3.5f };
    const float Map::NONPLAYER_MOVE_DISTANCE_{ 3.0f };

    Map::Map(const sf::FloatRect & REGION, interact::InteractionManager & interactionManager)
        : WALK_SFX_VOLUME_RATIO_(game::GameDataFile::Instance()->GetCopyFloat(
              "heroespath-sound-map-walk-sfx-volume-ratio"))
        , mapDisplayUPtr_(std::make_unique<map::MapDisplay>(*this, REGION))
        , interactionManager_(interactionManager)
        , collisionVec_()
        , transitionVec_()
        , level_(Level::Count)
        , player_(avatar::Model(
              std::make_unique<avatar::LPCView>(game::Game::Instance()->State().Party().Avatar())))
        , nonPlayers_()
        , walkRectVecMap_()
        , walkSfxLayers_()
        , walkMusicWhich_(sfml_util::music::Count)
        , walkMusicIsWalking_(false)
        , sfxTimer_(game::GameDataFile::Instance()->GetCopyFloat(
              "heroespath-sound-map-sfx-time-between-updates"))
        , interactionRect_()
    {}

    Map::~Map() = default;

    void Map::TransitionLevel(const Transition & TRANSITION)
    {
        PlayTransitionSfx(TRANSITION.TransType(), true);
        Load(TRANSITION.WhichLevel(), level_);
        PlayTransitionSfx(TRANSITION.TransType(), false);
    }

    void Map::Load(
        const Level::Enum LEVEL_TO_LOAD, const Level::Enum LEVEL_FROM, const bool IS_TEST_LOAD)
    {
        transitionVec_.clear();

        Layout & layout{ mapDisplayUPtr_->GetLayoutRef() };
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
                FindStartPos(transitionVec_, LEVEL_TO_LOAD, LEVEL_FROM), animInfoVec);
        }
    }

    bool Map::MovePlayer(const sfml_util::Direction::Enum DIRECTION)
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
            Transition transition;
            if (CheckIfEnteringTransition(DIRECTION, PLAYER_MOVE_DISTANCE_, transition))
            {
                HandleEnteringTransition(transition);
                return true;
            }
            else
            {
                if (sf::FloatRect() != interactionRect_)
                {
                    interactionRect_ = sf::FloatRect();
                    interactionManager_.RemoveCurrent();
                }

                return mapDisplayUPtr_->Move(DIRECTION, PLAYER_MOVE_DISTANCE_);
            }
        }
    }

    void Map::MoveNonPlayers()
    {
        for (std::size_t npcIndex(0); npcIndex < nonPlayers_.size(); ++npcIndex)
        {
            // check if NPC move will collide with the player
            auto & npc{ nonPlayers_[npcIndex] };

            auto const ADJ_POS_V{ [&]() {
                auto v{ player_.GetView().SpriteSize() };
                v.x *= 0.5f;
                v.y *= 0.5f;
                return v;
            }() };

            auto const PLAYER_RECT_FOR_NPC_COLLISIONS{ [&]() {
                auto const PLAYER_POS_V{ mapDisplayUPtr_->PlayerPosMap() };

                return sf::FloatRect(
                    PLAYER_POS_V.x + (ADJ_POS_V.x * 1.2f),
                    PLAYER_POS_V.y + (ADJ_POS_V.x * 0.8f),
                    ADJ_POS_V.x * 0.05f,
                    ADJ_POS_V.y * 1.45f);
            }() };

            auto const NPC_RECT_ADJ{ [&]() {
                auto rect{ npc.GetView().SpriteRef().getGlobalBounds() };

                switch (npc.GetView().Direction())
                {
                    case sfml_util::Direction::Left:
                    {
                        rect.left -= NONPLAYER_MOVE_DISTANCE_;
                        break;
                    }
                    case sfml_util::Direction::Right:
                    {
                        rect.left += NONPLAYER_MOVE_DISTANCE_;
                        break;
                    }
                    case sfml_util::Direction::Up:
                    {
                        rect.top -= NONPLAYER_MOVE_DISTANCE_;
                        break;
                    }
                    case sfml_util::Direction::Down:
                    case sfml_util::Direction::Count:
                    default:
                    {
                        rect.top += NONPLAYER_MOVE_DISTANCE_;
                        break;
                    }
                }

                return rect;
            }() };

            auto const NPC_RECT_FOR_PLAYER_COLLISIONS{ [&]() {
                auto rect{ NPC_RECT_ADJ };
                rect.left -= ADJ_POS_V.x * 0.25f;
                rect.width *= 1.75f;
                rect.height *= 1.75f;
                return rect;
            }() };

            if (sfml_util::DoRectsOverlap(
                    NPC_RECT_FOR_PLAYER_COLLISIONS, PLAYER_RECT_FOR_NPC_COLLISIONS))
            {
                npc.StopWalking();
                return;
            }

            // check if NPC move will collide with other NPCs
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
                        NPC_RECT_ADJ, subNPC.GetView().SpriteRef().getGlobalBounds()))
                {
                    auto const SUB_POS_V{ subNPC.GetView().SpriteRef().getPosition() };

                    auto const IS_IN_THE_WAY{ [&]() {
                        switch (npc.GetView().Direction())
                        {
                            case sfml_util::Direction::Left:
                            {
                                return (SUB_POS_V.x < NPC_POS_V.x);
                            }
                            case sfml_util::Direction::Right:
                            {
                                return (SUB_POS_V.x > NPC_POS_V.x);
                            }
                            case sfml_util::Direction::Up:
                            {
                                return (SUB_POS_V.y < NPC_POS_V.y);
                            }
                            case sfml_util::Direction::Down:
                            case sfml_util::Direction::Count:
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
        target.draw(*mapDisplayUPtr_, states);
    }

    void Map::Update(const float TIME_ELAPSED)
    {
        mapDisplayUPtr_->Update(TIME_ELAPSED);

        player_.Update(TIME_ELAPSED);

        for (auto & npc : nonPlayers_)
        {
            npc.Update(TIME_ELAPSED);
        }

        if (sfxTimer_.Update(TIME_ELAPSED))
        {
            mapDisplayUPtr_->UpdateAnimMusicVolume();
            UpdateWalkMusic();
        }
    }

    void Map::SetPlayerWalkAnim(const sfml_util::Direction::Enum DIRECTION, const bool WILL_START)
    {
        player_.SetWalkAnim(DIRECTION, WILL_START);
        player_.MovingIntoReset();
    }

    void Map::EntryAndExitLevels(
        std::vector<Level::Enum> & entryLevels, std::vector<Level::Enum> & exitLevels)
    {
        for (auto const & TRANSITION : transitionVec_)
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

    bool Map::DoesAdjPlayerPosCollide(const sfml_util::Direction::Enum DIR, const float ADJ)
    {
        auto const PLAYER_POS_V{ CalcAdjPlayerPos(DIR, ADJ) };

        auto const ADJ_FOR_COLLISIONS_V{ [&]() {
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

        auto const ADJ_FOR_NPC_COLLISIONS_V{ [&]() {
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
        sf::Vector2f startPos(-1.0f, -1.0f);

        for (auto const & TRANSITION : TRANS_VEC)
        {
            if (TRANSITION.IsEntry() && (TRANSITION.WhichLevel() == LEVEL_FROM))
            {
                startPos = TRANSITION.Center();
                break;
            }
        }

        M_ASSERT_OR_LOGANDTHROW_SS(
            ((startPos.x > 0.0f) && (startPos.y > 0.0f)),
            "map::Map::FindStartPos(level_to_load="
                << Level::ToString(LEVEL_TO_LOAD) << ", level_from=" << Level::ToString(LEVEL_FROM)
                << ") unable to find an entry transition.");

        return startPos;
    }

    bool Map::CheckIfEnteringTransition(
        const sfml_util::Direction::Enum DIRECTION,
        const float ADJUSTMENT,
        Transition & transition) const
    {
        auto const ADJ_PLAYER_POS_V{ CalcAdjPlayerPos(DIRECTION, ADJUSTMENT) };

        for (auto const & TRANSITION : transitionVec_)
        {
            if ((TRANSITION.IsEntry() == false) && (TRANSITION.Rect().contains(ADJ_PLAYER_POS_V)))
            {
                transition = TRANSITION;
                return true;
            }
        }

        return false;
    }

    void Map::HandleEnteringTransition(const Transition & TRANSITION)
    {
        player_.StopWalking();

        auto const IS_LOCKED{
            game::Game::Instance()->State().GetWorld().GetMaps().Current().IsDoorLocked(
                TRANSITION.WhichLevel())
        };

        if (IS_LOCKED)
        {
            if (TRANSITION.Rect() != interactionRect_)
            {
                interactionManager_.SetNext(
                    interact::InteractionFactory::MakeLockedDoor(TRANSITION));

                interactionRect_ = TRANSITION.Rect();
            }
        }
        else
        {
            TransitionLevel(TRANSITION);
        }
    }

    const sf::Vector2f Map::CalcAdjPlayerPos(
        const sfml_util::Direction::Enum DIRECTION, const float ADJUSTMENT) const
    {
        auto posToTest{ mapDisplayUPtr_->PlayerPosMap() };

        switch (DIRECTION)
        {
            case sfml_util::Direction::Left:
            {
                posToTest.x -= ADJUSTMENT;
                break;
            }
            case sfml_util::Direction::Right:
            {
                posToTest.x += ADJUSTMENT;
                break;
            }
            case sfml_util::Direction::Up:
            {
                posToTest.y -= ADJUSTMENT;
                break;
            }
            case sfml_util::Direction::Down:
            {
                posToTest.y += ADJUSTMENT;
                break;
            }
            case sfml_util::Direction::Count:
            default:
            {
                break;
            }
        }

        return posToTest;
    }

    void Map::PlayTransitionSfx(
        const sfml_util::sound_effect::MapTransition TRANS_TYPE, const bool IS_DOOR_OPENING) const
    {
        if (TRANS_TYPE != sfml_util::sound_effect::MapTransition::Count)
        {
            auto const DOOR_ACTION{ (
                (IS_DOOR_OPENING) ? sfml_util::sound_effect::DoorAction::Open
                                  : sfml_util::sound_effect::DoorAction::Close) };

            auto const TRANS_SFX{ sfml_util::sound_effect::RandomMapTransitionSfx(
                TRANS_TYPE, DOOR_ACTION) };

            if ((TRANS_SFX == sfml_util::sound_effect::Stairs) && (IS_DOOR_OPENING))
            {
                return;
            }

            if (TRANS_SFX != sfml_util::sound_effect::Count)
            {
                if (IS_DOOR_OPENING)
                {
                    sfml_util::SoundManager::Instance()->SoundEffectPlay(TRANS_SFX);
                }
                else
                {
                    sfml_util::SoundManager::Instance()->SoundEffectPlay(TRANS_SFX, 1.0f);
                }
            }
        }
    }

    void Map::UpdateWalkMusic()
    {
        auto const NEW_IS_WALKING{ (Player().GetView().WhichPose() == avatar::Pose::Walking) };
        auto const NEW_WALK_MUSIC{ walkSfxLayers_.FindSfx(mapDisplayUPtr_->PlayerPosMap()) };

        if (NEW_IS_WALKING
            && ((NEW_WALK_MUSIC != walkMusicWhich_) || (NEW_IS_WALKING != walkMusicIsWalking_)))
        {
            if (sfml_util::music::Count != walkMusicWhich_)
            {
                sfml_util::SoundManager::Instance()->MusicStop(
                    walkMusicWhich_, sfml_util::MusicOperator::FADE_MULT_IMMEDIATE_);
            }

            if (NEW_WALK_MUSIC != sfml_util::music::Count)
            {
                auto const VOLUME{ sfml_util::SoundManager::Instance()->SoundEffectVolume()
                                   * WALK_SFX_VOLUME_RATIO_ };

                sfml_util::SoundManager::Instance()->MusicStart(
                    NEW_WALK_MUSIC, sfml_util::MusicOperator::FADE_MULT_IMMEDIATE_, VOLUME);
            }
        }
        else if ((NEW_IS_WALKING == false) && (sfml_util::music::Count != walkMusicWhich_))
        {
            sfml_util::SoundManager::Instance()->MusicStop(
                walkMusicWhich_, sfml_util::MusicOperator::FADE_MULT_IMMEDIATE_);
        }

        walkMusicWhich_ = NEW_WALK_MUSIC;
        walkMusicIsWalking_ = NEW_IS_WALKING;
    }

    void Map::ResetNonPlayers()
    {
        nonPlayers_.clear();

        for (auto const & NPC_PTR :
             game::Game::Instance()->State().GetWorld().GetMaps().Current().SpecificNPCs())
        {
            AddNonPlayerAvatar(NPC_PTR);
        }

        for (auto const & NPC_PTR :
             game::Game::Instance()->State().GetWorld().GetMaps().Current().RandomNPCs())
        {
            AddNonPlayerAvatar(NPC_PTR);
        }
    }

    void Map::AddNonPlayerAvatar(const state::NpcPtr_t NPC_PTR)
    {
        auto const WALK_BOUNDS_INDEX{ NPC_PTR->WalkBoundsIndex() };
        auto const AVATAR_IMAGE_ENUM{ NPC_PTR->AvatarImage() };

        std::vector<sf::FloatRect> walkRects;
        auto const WAS_FOUND{ walkRectVecMap_.Find(WALK_BOUNDS_INDEX, walkRects) };

        if (WAS_FOUND == false)
        {
            std::ostringstream ss;
            ss << "map::Map::AddNonPlayerAvatar(avatar="
               << avatar::Avatar::ToString(AVATAR_IMAGE_ENUM)
               << ", walkBoundsIndex=" << WALK_BOUNDS_INDEX
               << ") but that walkBoundsIndex was not found in all the walk bounds loaded from the "
                  "map file.  Valid indexes are: [";

            for (auto const & WALK_BOUNDS_PAIR : walkRectVecMap_)
            {
                ss << WALK_BOUNDS_PAIR.first << ", ";
            }

            ss << "]";

            throw std::runtime_error(ss.str());
        }

        M_ASSERT_OR_LOGANDTHROW_SS(
            (walkRects.empty() == false),
            "map::Map::AddNonPlayerAvatar(avatar="
                << avatar::Avatar::ToString(AVATAR_IMAGE_ENUM)
                << ", walkBoundsIndex=" << WALK_BOUNDS_INDEX
                << ") but that walkBoundsIndex, while valid, was "
                   "not associated with any actual walk bounds.  "
                   "The vector was empty.");

        nonPlayers_.emplace_back(
            avatar::Model(std::make_unique<avatar::LPCView>(AVATAR_IMAGE_ENUM), walkRects));
    }

} // namespace map
} // namespace heroespath
