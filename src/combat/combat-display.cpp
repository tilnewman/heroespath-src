// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// combat-display.cpp
//
#include "combat-display.hpp"

#include "combat/combat-anim.hpp"
#include "combat/combat-node.hpp"
#include "combat/encounter.hpp"
#include "combat/summary-view.hpp"
#include "creature/algorithms.hpp"
#include "creature/creature.hpp"
#include "creature/player-party.hpp"
#include "game/game-state.hpp"
#include "game/game.hpp"
#include "misc/config-file.hpp"
#include "misc/log-macros.hpp"
#include "misc/random.hpp"
#include "misc/real.hpp"
#include "sfml-util/box-entity.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/font-manager.hpp"
#include "sfml-util/image-util.hpp"
#include "sfml-util/text-info.hpp"
#include "sfml-util/text-region.hpp"
#include "sfutil/display.hpp"
#include "sfutil/size-and-scale.hpp"

#include <algorithm>
#include <numeric>
#include <sstream>

namespace heroespath
{
namespace combat
{

    NodePosTracker::NodePosTracker()
        : posHorizOrig(0.0f)
        , posVertOrig(0.0f)
        , horizDiff(0.0f)
        , vertDiff(0.0f)
    {}

    NodePosTracker::NodePosTracker(
        const CombatNodePtr_t COMBAT_NODE_PTR,
        const float TARGET_POS_LEFT,
        const float TARGET_POS_TOP)
        : posHorizOrig(COMBAT_NODE_PTR->GetEntityPos().x)
        , posVertOrig(COMBAT_NODE_PTR->GetEntityPos().y)
        , horizDiff(TARGET_POS_LEFT - COMBAT_NODE_PTR->GetEntityPos().x)
        , vertDiff(TARGET_POS_TOP - COMBAT_NODE_PTR->GetEntityPos().y)
    {}

    const float CombatDisplay::POSITIONING_CELL_SIZE_RATIO_MIN_HORIZ_(0.4f);
    const float CombatDisplay::POSITIONING_CELL_SIZE_RATIO_MAX_HORIZ_(4.0f);
    const float CombatDisplay::POSITIONING_CELL_SIZE_RATIO_MIN_VERT_(0.4f);
    const float CombatDisplay::POSITIONING_CELL_SIZE_RATIO_MAX_VERT_(2.0f);
    const int CombatDisplay::BLOCKING_POS_MARGIN_(3);

    // any number over one thousand should work here
    const int CombatDisplay::BLOCKING_POS_INVALID_(1001);

    const float CombatDisplay::CREATURE_MOVE_SLIDER_SPEED_(4.0f);
    const float CombatDisplay::BATTLEFIELD_DRAG_SPEED_(3.0f);
    const std::size_t CombatDisplay::SHOULDER_TO_SHOULDER_MAX_(10);

    CombatDisplay::CombatDisplay(
        const CombatAnimationPtr_t COMBAT_ANIM_PTR, const sf::FloatRect & REGION)
        : Stage("CombatDisplay", REGION, {}, false)
        , POSITIONING_MARGIN_HORIZ_(sfutil::MapByRes(50.0f, 300.0f))
        , POSITIONING_MARGIN_VERT_(sfutil::MapByRes(50.0f, 300.0f))
        , POSITIONING_BETWEEN_SPACER_HORIZ_(sfutil::MapByRes(5.0f, 200.0f))
        , POSITIONING_BETWEEN_SPACER_VERT_(sfutil::MapByRes(25.0f, 200.0f))
        , CELL_HEIGHT_(sfutil::MapByRes(
              sfml_util::StandardImageDimmension() * POSITIONING_CELL_SIZE_RATIO_MIN_VERT_,
              sfml_util::StandardImageDimmension() * POSITIONING_CELL_SIZE_RATIO_MAX_VERT_))
        , NAME_CHAR_SIZE_ORIG_(sfml_util::FontManager::Instance()->Size_CombatCreatureLabels())
        , SCREEN_WIDTH_(sfml_util::Display::Instance()->GetWinWidth())
        , SCREEN_HEIGHT_(sfml_util::Display::Instance()->GetWinHeight())
        , nameCharSizeCurr_(NAME_CHAR_SIZE_ORIG_)
        , battlefieldRect_()
        , boxUPtr_()
        , bgCachedTexture_(
              "media-images-backgrounds-tile-darkpaper",
              sfml_util::ImageOpt::Default | sfml_util::ImageOpt::Repeated)
        , offScreenTexture_()
        , offScreenSprite_()
        , offScreenPosX_(0.0f)
        , offScreenPosY_(0.0f)
        , combatTree_()
        , prevScrollPosVert_(0.0f)
        , prevScrollPosHoriz_(0.0f)
        , summaryViewUPtr_(std::make_unique<SummaryView>())
        , isSummaryViewAllowed_(false)
        , isScrollAllowed_(false)
        , battlefieldWidth_(0.0f)
        , battlefieldHeight_(0.0f)
        , blockingPosMin_(BLOCKING_POS_INVALID_)
        , blockingPosMax_(BLOCKING_POS_INVALID_)
        , zoomLevel_(1.0f)
        , isMouseHeldDownInBF_(false)
        , prevMousePos_(0.0f, 0.0f)
        , isPlayerTurn_(false)
        , isStatusMessageAnim_(false)
        , isSummaryViewInProgress_(false)
        , combatAnimationPtr_(COMBAT_ANIM_PTR)
        , isCombatOver_(false)
        , nodePosTrackerMap_()
        , centeringToPosV_(-1.0f, -1.0f) // any negative values will work here
    {}

    CombatDisplay::~CombatDisplay() { sfml_util::Stage::ClearAllEntities(); }

    void CombatDisplay::Setup()
    {
        // create CombatNodes and add them into the combateTree_
        for (const auto & NEXT_CHARACTER_PTR : game::Game::Instance()->State().Party().Characters())
        {
            EntityAdd(combatTree_.AddVertex(NEXT_CHARACTER_PTR).Ptr());
        }
        InitialPlayerPartyCombatTreeSetup();
        //
        for (const auto & NEXT_CREATURE_PTR : creature::Algorithms::NonPlayers())
        {
            EntityAdd(combatTree_.AddVertex(NEXT_CREATURE_PTR).Ptr());
        }
        InitialNonPlayerPartyCombatTreeSetup();

        // establish primary drawing area as battlefieldRect_
        // StageRegionSet() must have already been called
        battlefieldRect_ = StageRegion();

        PositionCombatTreeCells(false);

        // battlefield bounding box
        sfml_util::BoxEntityInfo boxInfo;
        boxInfo.SetupBorder(true);

        boxInfo.focus_colors = sfml_util::FocusColors(sf::Color::White, sf::Color::Transparent);

        boxUPtr_
            = std::make_unique<sfml_util::BoxEntity>("CombatDisplay's'", battlefieldRect_, boxInfo);

        // setup offscreen texture
        const auto OFFSCREEN_SIZE_V_U { sf::Vector2u(sfutil::Size(boxUPtr_->InnerRegion())) };
        offScreenTexture_.create(OFFSCREEN_SIZE_V_U.x, OFFSCREEN_SIZE_V_U.y);
        offScreenTexture_.clear(sf::Color::Transparent);
        offScreenTexture_.setRepeated(true);

        // make temp use of offScreenSprite_ to tile the background onto offScreenTexture_
        offScreenSprite_.setPosition(0.0f, 0.0f);
        offScreenSprite_.setTexture(bgCachedTexture_.Get());

        offScreenSprite_.setTextureRect(
            sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(offScreenTexture_.getSize())));

        offScreenTexture_.draw(offScreenSprite_);
        offScreenTexture_.display();

        // setup offscreen drawing sprite
        offScreenSprite_.setTexture(offScreenTexture_.getTexture());
        offScreenSprite_.setTextureRect(sf::IntRect(battlefieldRect_));
        offScreenSprite_.setPosition(battlefieldRect_.left, battlefieldRect_.top);
    }

    void CombatDisplay::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        target.draw(offScreenSprite_, STATES);
        target.draw(*boxUPtr_, STATES);
        Stage::Draw(target, STATES);
        target.draw(*summaryViewUPtr_, STATES);
    }

    bool CombatDisplay::StartSummaryView(const sf::Vector2f & MOUSE_POS)
    {
        if (isSummaryViewAllowed_ && isPlayerTurn_ && (false == isMouseHeldDownInBF_)
            && (GetIsStatusMessageAnimating() == false))
        {
            const auto COMBAT_NODE_PTR_OPT { combatTree_.GetNodePtrOpt(MOUSE_POS.x, MOUSE_POS.y) };

            if (COMBAT_NODE_PTR_OPT)
            {
                const auto COMBAT_NODE_PTR { COMBAT_NODE_PTR_OPT.value() };

                if (COMBAT_NODE_PTR->WillDraw() == true)
                {
                    combatAnimationPtr_->ShakeAnimTemporaryStop(COMBAT_NODE_PTR->Creature());
                    summaryViewUPtr_->SetupAndStartTransition(COMBAT_NODE_PTR, battlefieldRect_);
                    SetIsSummaryViewInProgress(true);
                    return true;
                }
            }
        }

        return false;
    }

    float CombatDisplay::SetZoomLevel(const float ZOOM_LEVEL)
    {
        const auto ORIG_ZOOM_LEVEL { zoomLevel_ };
        zoomLevel_ = ZOOM_LEVEL;

        if (misc::IsRealClose(ZOOM_LEVEL, ORIG_ZOOM_LEVEL))
        {
            return zoomLevel_;
        }

        // prevent zoom level from going too low
        const auto MIN_ZOOM_LEVEL { 0.1f };
        if (zoomLevel_ < MIN_ZOOM_LEVEL)
        {
            zoomLevel_ = MIN_ZOOM_LEVEL;
        }

        nameCharSizeCurr_
            = static_cast<unsigned int>(static_cast<float>(NAME_CHAR_SIZE_ORIG_) * ZOOM_LEVEL);

        const auto BATTLEFIELD_CENTER_BEFORE_V { GetCenterOfAllNodes() };
        PositionCombatTreeCells(false);
        const auto BATTLEFIELD_CENTER_AFTER_V { GetCenterOfAllNodes() };

        const auto HORIZ_DIFF { BATTLEFIELD_CENTER_BEFORE_V.x - BATTLEFIELD_CENTER_AFTER_V.x };
        const auto VERT_DIFF { BATTLEFIELD_CENTER_BEFORE_V.y - BATTLEFIELD_CENTER_AFTER_V.y };

        // move nodes to keep the battlefield centered
        for (const auto & NEXT_COMBATNODE_PTR : combatTree_.GetCombatNodes())
        {
            NEXT_COMBATNODE_PTR->MoveEntityPos(HORIZ_DIFF, VERT_DIFF);
        }

        // if all creatures are already visible then prevent further zoom out
        if (battlefieldHeight_ < battlefieldRect_.height)
        {
            zoomLevel_ = ORIG_ZOOM_LEVEL;

            nameCharSizeCurr_ = static_cast<unsigned int>(
                static_cast<float>(NAME_CHAR_SIZE_ORIG_) * ORIG_ZOOM_LEVEL);

            PositionCombatTreeCells(false);
        }

        UpdateWhichNodesWillDraw();
        return zoomLevel_;
    }

    bool CombatDisplay::UpdateWhichNodesWillDraw()
    {
        bool isAnyNodeDrawn(false);

        for (const auto & COMBAT_NODE_PTR : combatTree_.GetCombatNodes())
        {
            const sf::FloatRect NEXT_VERT_RECT(COMBAT_NODE_PTR->GetEntityRegion());

            if ((battlefieldRect_.contains(NEXT_VERT_RECT.left, NEXT_VERT_RECT.top))
                && (battlefieldRect_.contains(
                       NEXT_VERT_RECT.left + NEXT_VERT_RECT.width, NEXT_VERT_RECT.top))
                && (battlefieldRect_.contains(
                       NEXT_VERT_RECT.left, NEXT_VERT_RECT.top + NEXT_VERT_RECT.height))
                && (battlefieldRect_.contains(
                       NEXT_VERT_RECT.left + NEXT_VERT_RECT.width,
                       NEXT_VERT_RECT.top + NEXT_VERT_RECT.height)))
            {
                isAnyNodeDrawn = true;
                COMBAT_NODE_PTR->WillDraw(true);
            }
            else
            {
                COMBAT_NODE_PTR->WillDraw(false);
            }
        }

        return isAnyNodeDrawn;
    }

    void CombatDisplay::UpdateMousePos(const sf::Vector2i & NEW_MOUSE_POS)
    {
        const sf::Vector2f NEW_MOUSE_POS_F { NEW_MOUSE_POS };

        Stage::UpdateMousePos(NEW_MOUSE_POS);

        if (isScrollAllowed_ && isMouseHeldDownInBF_)
        {
            MoveBattlefieldVert((prevMousePos_.y - NEW_MOUSE_POS_F.y) * BATTLEFIELD_DRAG_SPEED_);
            MoveBattlefieldHoriz((prevMousePos_.x - NEW_MOUSE_POS_F.x) * BATTLEFIELD_DRAG_SPEED_);
        }

        prevMousePos_ = NEW_MOUSE_POS_F;
    }

    void CombatDisplay::UpdateMouseDown(const sf::Vector2f & MOUSE_POS_V)
    {
        if ((GetIsSummaryViewInProgress() == false) && isScrollAllowed_)
        {
            Stage::UpdateMouseDown(MOUSE_POS_V);

            const auto COMBAT_NODE_CLICKED_ON_PTR_OPT { combatTree_.GetNodePtrOpt(
                MOUSE_POS_V.x, MOUSE_POS_V.y) };

            if (!COMBAT_NODE_CLICKED_ON_PTR_OPT)
            {
                if (battlefieldRect_.contains(MOUSE_POS_V))
                {
                    isMouseHeldDownInBF_ = true;
                }
            }
        }

        prevMousePos_ = MOUSE_POS_V;
    }

    const sfml_util::IEntityPtrOpt_t CombatDisplay::UpdateMouseUp(const sf::Vector2f & MOUSE_POS_V)
    {
        if (isScrollAllowed_)
        {
            isMouseHeldDownInBF_ = false;
            return Stage::UpdateMouseUp(MOUSE_POS_V);
        }
        else
        {
            return boost::none;
        }
    }

    const sf::Vector2f CombatDisplay::GetCenterOfAllNodes() const
    {
        auto posHorizMin { 0.0f };
        auto posHorizMax { 0.0f };
        auto posVertMin { 0.0f };
        auto posVertMax { 0.0f };
        auto hasSetAnyValuesYet { false };
        for (const auto & NEXT_COMBATNODE_PTR : combatTree_.GetCombatNodes())
        {
            const auto NEXT_REGION { NEXT_COMBATNODE_PTR->GetEntityRegion() };
            if (false == hasSetAnyValuesYet)
            {
                posHorizMin = NEXT_REGION.left;
                posHorizMax = NEXT_REGION.left + NEXT_REGION.width;
                posVertMin = NEXT_REGION.top;
                posVertMax = NEXT_REGION.top + NEXT_REGION.height;
                hasSetAnyValuesYet = true;
            }
            else
            {
                if (NEXT_REGION.left < posHorizMin)
                {
                    posHorizMin = NEXT_REGION.left;
                }

                if ((NEXT_REGION.left + NEXT_REGION.width) > posHorizMax)
                {
                    posHorizMax = NEXT_REGION.left + NEXT_REGION.width;
                }

                if (NEXT_REGION.top < posVertMin)
                {
                    posVertMin = NEXT_REGION.top;
                }

                if ((NEXT_REGION.top + NEXT_REGION.height) > posVertMax)
                {
                    posVertMax = NEXT_REGION.top + NEXT_REGION.height;
                }
            }
        }

        return sf::Vector2f(
            posHorizMin + ((posHorizMax - posHorizMin) * 0.5f),
            posVertMin + ((posVertMax - posVertMin) * 0.5f));
    }

    void CombatDisplay::MoveBattlefieldVert(const float AMOUNT, const bool WILL_MOVE_BACKGROUND)
    {
        // keep track of all original values in case undo is required
        const auto ORIG_OFFSCREEN_SPRITE_RECT { offScreenSprite_.getTextureRect() };

        if (WILL_MOVE_BACKGROUND)
        {
            offScreenPosY_ += AMOUNT;

            offScreenSprite_.setTextureRect(sf::IntRect(sf::FloatRect(
                offScreenPosX_,
                offScreenPosY_,
                offScreenSprite_.getLocalBounds().width,
                offScreenSprite_.getLocalBounds().height)));
        }

        // move the creature nodes
        for (const auto & COMBAT_NODE_PTR : combatTree_.GetCombatNodes())
        {
            COMBAT_NODE_PTR->MoveEntityPos(0.0f, AMOUNT * -1.0f);
        }

        if (UpdateWhichNodesWillDraw())
        {
            if (WILL_MOVE_BACKGROUND)
            {
                prevScrollPosVert_ = offScreenPosY_;
            }

            centeringToPosV_ = sf::Vector2f(centeringToPosV_.x, centeringToPosV_.y - AMOUNT);
        }
        else
        {
            // if no creature/combat nodes are drawn to the screen anymore, then move back
            if (WILL_MOVE_BACKGROUND)
            {
                offScreenPosY_ = prevScrollPosVert_;
            }

            offScreenSprite_.setTextureRect(ORIG_OFFSCREEN_SPRITE_RECT);

            for (const auto & COMBAT_NODE_PTR : combatTree_.GetCombatNodes())
            {
                COMBAT_NODE_PTR->MoveEntityPos(0.0f, AMOUNT);
            }

            UpdateWhichNodesWillDraw();
        }
    }

    void CombatDisplay::MoveBattlefieldHoriz(const float AMOUNT, const bool WILL_MOVE_BACKGROUND)
    {
        // keep track of all original values in case undo is required
        const auto ORIG_OFFSCREEN_SPRITE_RECT { offScreenSprite_.getTextureRect() };

        if (WILL_MOVE_BACKGROUND)
        {
            offScreenPosX_ += AMOUNT;

            offScreenSprite_.setTextureRect(sf::IntRect(sf::FloatRect(
                offScreenPosX_,
                offScreenPosY_,
                offScreenSprite_.getLocalBounds().width,
                offScreenSprite_.getLocalBounds().height)));
        }

        // move the creature nodes
        for (const auto & COMBAT_NODE_PTR : combatTree_.GetCombatNodes())
        {
            COMBAT_NODE_PTR->MoveEntityPos(AMOUNT * -1.0f, 0.0f);
        }

        if (UpdateWhichNodesWillDraw())
        {
            if (WILL_MOVE_BACKGROUND)
            {
                prevScrollPosHoriz_ = offScreenPosX_;
            }

            centeringToPosV_ = sf::Vector2f(centeringToPosV_.x - AMOUNT, centeringToPosV_.y);
        }
        else
        {
            // if no creature/combat nodes are drawn to the screen anymore, then move back
            if (WILL_MOVE_BACKGROUND)
            {
                offScreenPosX_ = prevScrollPosHoriz_;
            }

            offScreenSprite_.setTextureRect(ORIG_OFFSCREEN_SPRITE_RECT);

            for (const auto & COMBAT_NODE_PTR : combatTree_.GetCombatNodes())
            {
                COMBAT_NODE_PTR->MoveEntityPos(AMOUNT, 0.0f);
            }

            UpdateWhichNodesWillDraw();
        }
    }

    const creature::CreaturePVec_t CombatDisplay::FindClosestLivingByType(
        const creature::CreaturePtr_t CREATURE_PTR, const bool WILL_FIND_PLAYERS) const
    {
        return FindClosestLiving(
            CREATURE_PTR,
            creature::Algorithms::PlayersByType(
                ((WILL_FIND_PLAYERS) ? creature::Algorithms::TypeOpt::Player
                                     : creature::Algorithms::TypeOpt::NonPlayer),
                creature::Algorithms::Living));
    }

    const creature::CreaturePVec_t CombatDisplay::FindClosestLiving(
        const creature::CreaturePtr_t CREATURE_PTR,
        const creature::CreaturePVec_t & AMONG_PVEC) const
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (AMONG_PVEC.empty() == false),
            "combat::CombatDisplay::FindClosestLiving(creature={"
                << CREATURE_PTR->ToString() << "}, among_size=" << AMONG_PVEC.size()
                << ") was given an empty AMONG_PVEC.");

        if (AMONG_PVEC.size() == 1)
        {
            return AMONG_PVEC;
        }
        else
        {
            auto livingCreaturesPVec { creature::Algorithms::FindByAlive(AMONG_PVEC) };

            // sort by who is closest in terms of blocking position
            std::sort(
                livingCreaturesPVec.begin(),
                livingCreaturesPVec.end(),
                [this, CREATURE_PTR](const auto & A_PTR, const auto & B_PTR) {
                    return (combatTree_.GetBlockingDistanceBetween(A_PTR, CREATURE_PTR))
                        < std::abs(combatTree_.GetBlockingDistanceBetween(B_PTR, CREATURE_PTR));
                });

            const auto MIN_BLOCKING_DISTANCE { std::abs(combatTree_.GetBlockingDistanceBetween(
                *livingCreaturesPVec.begin(), CREATURE_PTR)) };

            creature::CreaturePVec_t closestCreaturesPVec;

            for (const auto & NEXT_CREATURE_PTR : livingCreaturesPVec)
            {
                if (std::abs(
                        combatTree_.GetBlockingDistanceBetween(NEXT_CREATURE_PTR, CREATURE_PTR))
                    == MIN_BLOCKING_DISTANCE)
                {
                    closestCreaturesPVec.emplace_back(NEXT_CREATURE_PTR);
                }
            }

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (closestCreaturesPVec.empty() == false),
                "combat::CombatDisplay::FindClosestLiving("
                    << CREATURE_PTR->Name() << ", among_size=" << AMONG_PVEC.size()
                    << ") reached an invalid state where there"
                    << "were no closest creatures when there should be at least one of the "
                    << AMONG_PVEC.size() << " total possible.");

            return closestCreaturesPVec;
        }
    }

    const creature::CreaturePVec_t CombatDisplay::FindCreaturesThatCanBeAttackedOfType(
        const creature::CreaturePtr_t CREATURE_PTR, const bool WILL_FIND_PLAYERS) const
    {
        creature::CreaturePVec_t creaturePVec;

        if (CREATURE_PTR->HasWeaponsHeld() == false)
        {
            return creaturePVec;
        }

        const auto IS_FLYING { Encounter::Instance()->GetTurnInfoCopy(CREATURE_PTR).GetIsFlying() };

        if (CREATURE_PTR->IsHoldingProjectileWeapon() || (IS_FLYING))
        {
            creature::Algorithms::PlayersByType(
                creaturePVec,
                ((WILL_FIND_PLAYERS) ? creature::Algorithms::TypeOpt::Player
                                     : creature::Algorithms::TypeOpt::NonPlayer),
                creature::Algorithms::Living);
        }
        else
        {
            const auto ALLAROUND_CREATURES_PVEC { FindCreaturesAllRoundOfType(
                CREATURE_PTR, WILL_FIND_PLAYERS, true) };

            const auto NOTFLYING_ALLAROUND_CREATURES_PVEC { creature::Algorithms::FindByIsFlying(
                ALLAROUND_CREATURES_PVEC, creature::Algorithms::CriteriaOpt::DoesNotMeet) };

            const auto REACHABLE_CREATURES_PVEC { (
                (IS_FLYING) ? ALLAROUND_CREATURES_PVEC : NOTFLYING_ALLAROUND_CREATURES_PVEC) };

            if (REACHABLE_CREATURES_PVEC.empty() == false)
            {
                std::copy(
                    REACHABLE_CREATURES_PVEC.begin(),
                    REACHABLE_CREATURES_PVEC.end(),
                    back_inserter(creaturePVec));
            }
        }

        return creaturePVec;
    }

    const creature::CreaturePVec_t CombatDisplay::FindCreaturesAllRoundOfType(
        const creature::CreaturePtr_t CREATURE_PTR,
        const bool WILL_FIND_PLAYERS,
        const bool LIVING_ONLY) const
    {
        const auto COMBAT_NODES_ALL_AROUND_PVEC { combatTree_.GetNodesAllAroundBlockingPos(
            combatTree_.GetNodePtr(CREATURE_PTR)->GetBlockingPos()) };

        creature::CreaturePVec_t creaturePVec;
        creaturePVec.reserve(COMBAT_NODES_ALL_AROUND_PVEC.size());

        for (const auto & NEXT_COMBATNODE_PTR : COMBAT_NODES_ALL_AROUND_PVEC)
        {
            if ((NEXT_COMBATNODE_PTR->Creature() != CREATURE_PTR)
                && (NEXT_COMBATNODE_PTR->Creature()->IsPlayerCharacter() == WILL_FIND_PLAYERS))
            {
                if ((LIVING_ONLY == false)
                    || (LIVING_ONLY && (NEXT_COMBATNODE_PTR->Creature()->IsDead() == false)))
                {
                    creaturePVec.emplace_back(NEXT_COMBATNODE_PTR->Creature());
                }
            }
        }

        return creaturePVec;
    }

    const creature::CreaturePVec_t CombatDisplay::FindCreaturesAtBlockingPosOfType(
        const int BLOCKING_POS, const bool WILL_FIND_PLAYERS) const
    {
        const auto COMBAT_NODES_AT_POS_PVEC { combatTree_.GetNodesAtBlockingPos(BLOCKING_POS) };

        creature::CreaturePVec_t creaturesPVec;
        creaturesPVec.reserve(COMBAT_NODES_AT_POS_PVEC.size());

        for (const auto & NEXT_COMBATNODE_PTR : COMBAT_NODES_AT_POS_PVEC)
        {
            if (NEXT_COMBATNODE_PTR->Creature()->IsPlayerCharacter() == WILL_FIND_PLAYERS)
            {
                creaturesPVec.emplace_back(NEXT_COMBATNODE_PTR->Creature());
            }
        }

        return creaturesPVec;
    }

    int CombatDisplay::FindBlockingPos(const creature::CreaturePtr_t CREATURE_PTR) const
    {
        return combatTree_.GetNodePtr(CREATURE_PTR)->GetBlockingPos();
    }

    const creature::CreaturePVec_t CombatDisplay::FindClosestAmongOfType(
        const creature::CreaturePtr_t CREATURE_OF_ORIGIN_PTR,
        const creature::CreaturePVec_t & CREATURES_TO_FIND_AMONG_PVEC,
        const bool WILL_FIND_PLAYERS) const
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (CREATURES_TO_FIND_AMONG_PVEC.empty() == false),
            "combat::FindClosestAmongOfType(creature_of_origin_name=\""
                << CREATURE_OF_ORIGIN_PTR->Name()
                << "\") was given a CREATURES_TO_FIND_AMONG_PVEC that was empty.");

        const auto BLOCKING_POS_ORIGIN { FindBlockingPos(CREATURE_OF_ORIGIN_PTR) };

        auto closestBlockingDistanceABS { combatTree_.GetBlockingDistanceMax() + 1 };
        for (const auto & NEXT_CREATURE_PTR : CREATURES_TO_FIND_AMONG_PVEC)
        {
            const auto NEXT_BLOCKING_DISTANCE_ABS { std::abs(
                FindBlockingPos(NEXT_CREATURE_PTR) - BLOCKING_POS_ORIGIN) };

            if ((NEXT_CREATURE_PTR->IsPlayerCharacter() == WILL_FIND_PLAYERS)
                && (NEXT_BLOCKING_DISTANCE_ABS < closestBlockingDistanceABS))
            {
                closestBlockingDistanceABS = NEXT_BLOCKING_DISTANCE_ABS;
            }
        }

        creature::CreaturePVec_t closestCreaturesPVec;

        for (const auto & NEXT_CREATURE_PTR : CREATURES_TO_FIND_AMONG_PVEC)
        {
            const auto NEXT_BLOCKING_DISTANCE_ABS { std::abs(
                FindBlockingPos(NEXT_CREATURE_PTR) - BLOCKING_POS_ORIGIN) };

            if ((NEXT_CREATURE_PTR->IsPlayerCharacter() == WILL_FIND_PLAYERS)
                && (NEXT_BLOCKING_DISTANCE_ABS == closestBlockingDistanceABS))
            {
                closestCreaturesPVec.emplace_back(NEXT_CREATURE_PTR);
            }
        }

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (closestCreaturesPVec.empty() == false),
            "combat::FindClosestAmongOfType(creature_of_origin_name=\""
                << CREATURE_OF_ORIGIN_PTR->Name()
                << "\", creatures_to_find_among_pvec_size=" << CREATURES_TO_FIND_AMONG_PVEC.size()
                << ", will_find_players=" << std::boolalpha << WILL_FIND_PLAYERS
                << ") was unable to find a closest creature among those given.");

        return closestCreaturesPVec;
    }

    bool CombatDisplay::IsCreatureAPossibleFightTarget(
        const creature::CreaturePtr_t CREATURE_FIGHTING_PTR,
        const creature::CreaturePtr_t CREATURE_TARGETED_PTR) const
    {
        for (const auto & NEXT_CREATURE_PTR :
             FindCreaturesThatCanBeAttackedOfType(CREATURE_FIGHTING_PTR, false))
        {
            if (NEXT_CREATURE_PTR == CREATURE_TARGETED_PTR)
            {
                return true;
            }
        }

        return false;
    }

    CombatNodePtr_t
        CombatDisplay::GetCombatNodeForCreature(const creature::CreaturePtr_t CREATURE_PTR) const
    {
        return combatTree_.GetNodePtr(CREATURE_PTR);
    }

    const CombatNodePVec_t CombatDisplay::GetCombatNodesForCreatures(
        const creature::CreaturePVec_t & CREATURES_PVEC) const
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (CREATURES_PVEC.empty() == false),
            "combat::GetCombatNodesForCreatures() was given a CREATURES_PVEC "
                << "that was empty.");

        CombatNodePVec_t creatureCombatNodesPVec;
        creatureCombatNodesPVec.reserve(CREATURES_PVEC.size());

        for (const auto & NEXT_CREATURE_PTR : CREATURES_PVEC)
        {
            creatureCombatNodesPVec.emplace_back(GetCombatNodeForCreature(NEXT_CREATURE_PTR));
        }

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (creatureCombatNodesPVec.size() == CREATURES_PVEC.size()),
            "combat::CombatDisplay::GetCombatNodesForCreatures(CREATURES_PVEC.size()="
                << CREATURES_PVEC.size()
                << ") was not able to find CombatNodePtr_ts for every CreaturePtr_t.");

        return creatureCombatNodesPVec;
    }

    const std::string CombatDisplay::CanAdvanceOrRetreat(
        const creature::CreaturePtr_t CREATURE_PTR, const bool TRYING_TO_ADVANCE) const
    {
        const int BLOCKING_POS(combatTree_.GetNodePtr(CREATURE_PTR)->GetBlockingPos());

        const bool ATTEMPTING_BLOCKING_POS_INCREMENT(
            CREATURE_PTR->IsPlayerCharacter() == TRYING_TO_ADVANCE);

        const std::string ADVANCE_OR_RETREAT_STR((TRYING_TO_ADVANCE) ? "advance" : "retreat");

        // check if attempting to move past battlefield boundaries/edges
        if (((ATTEMPTING_BLOCKING_POS_INCREMENT) && (BLOCKING_POS >= blockingPosMax_))
            || ((ATTEMPTING_BLOCKING_POS_INCREMENT == false) && (BLOCKING_POS <= blockingPosMin_)))
        {
            return "Cannot " + ADVANCE_OR_RETREAT_STR + " because already at limit.";
        }

        // flying creatures can always advance or retreat except if at the edge of the battlefield
        if (Encounter::Instance()->GetTurnInfoCopy(CREATURE_PTR).GetIsFlying())
        {
            return "";
        }

        const int BLOCKING_POS_NEW(
            (ATTEMPTING_BLOCKING_POS_INCREMENT) ? (BLOCKING_POS + 1) : (BLOCKING_POS - 1));

        const std::size_t OBSTACLE_CREATURE_COUNT_AT_NEW_POS {
            GetObstacleCreaturesAtBlockingPos(CREATURE_PTR, BLOCKING_POS_NEW).size()
        };

        // check if attempting to move into a shoulder-to-shoulder line
        // that already has too many creatures
        if (OBSTACLE_CREATURE_COUNT_AT_NEW_POS > SHOULDER_TO_SHOULDER_MAX_)
        {
            std::ostringstream ss;
            ss << "Cannot " << ADVANCE_OR_RETREAT_STR << "because there are too many ("
               << OBSTACLE_CREATURE_COUNT_AT_NEW_POS << ") opposing creatres in the way."
               << "  The limit is " << SHOULDER_TO_SHOULDER_MAX_ << ".";

            return ss.str();
        }

        // Check if attempting to move into a shoulder-to-shoulder line that
        // has an opposing creature blocking.
        for (const auto & NEXT_COMBATNODE_PTR : combatTree_.GetCombatNodes())
        {
            const auto NEXT_CREATURE_PTR { NEXT_COMBATNODE_PTR->Creature() };

            if ((NEXT_COMBATNODE_PTR->GetBlockingPos() == BLOCKING_POS_NEW)
                && (NEXT_CREATURE_PTR->IsPlayerCharacter() != CREATURE_PTR->IsPlayerCharacter())
                && (NEXT_CREATURE_PTR->HasConditionNotAThreatPerm(creature::UnconOpt::Include)
                    == false)
                && (Encounter::Instance()
                        ->GetTurnInfoCopy(NEXT_CREATURE_PTR)
                        .GetTurnActionInfo()
                        .Action()
                    == TurnAction::Block))
            {
                return "Cannot " + ADVANCE_OR_RETREAT_STR
                    + " because there is a creature blocking.";
            }
        }

        return "";
    }

    const creature::CreaturePtrOpt_t
        CombatDisplay::GetCreatureAtPosPtrOpt(const sf::Vector2f & POS_V)
    {
        const auto COMBAT_NODE_PTR_OPT { combatTree_.GetNodePtrOpt(POS_V.x, POS_V.y) };

        if (COMBAT_NODE_PTR_OPT)
        {
            return COMBAT_NODE_PTR_OPT.value()->Creature();
        }
        else
        {
            return boost::none;
        }
    }

    void CombatDisplay::MoveCreatureBlockingPosition(
        const creature::CreaturePtr_t CREATURE_PTR, const bool WILL_MOVE_FORWARD)
    {
        const auto CREATURE_NODE_ID { combatTree_.GetNodeId(CREATURE_PTR) };
        auto blockingPos { combatTree_.GetNodePtr(CREATURE_NODE_ID)->GetBlockingPos() };

        // Note: For player-characters (facing right), 'Forward' is moving to
        //      the right on the battlefield (increasing blocking pos), and
        //      'Backward' is moving to the left (decreasing blocking pos).
        //      Non-player-characters have the inverse concept of forward and
        //      backward.
        if (CREATURE_PTR->IsPlayerCharacter() == WILL_MOVE_FORWARD)
        {
            ++blockingPos;
        }
        else
        {
            --blockingPos;
        }

        combatTree_.ChangeBlockingPositionAndUpdateTree(CREATURE_NODE_ID, blockingPos);

        // re-position CombatNodes/Creatures on the battlefield in the slow animated way
        PositionCombatTreeCells(true);
    }

    void CombatDisplay::CancelSummaryViewAndStartTransitionBack()
    {
        summaryViewUPtr_->StartTransitionBack();
    }

    void CombatDisplay::HandleFlyingChange(
        const creature::CreaturePtr_t CREATURE_PTR, const bool IS_FLYING)
    {
        combatTree_.GetNodePtr(CREATURE_PTR)->IsFlying(IS_FLYING);
        Encounter::Instance()->SetIsFlying(CREATURE_PTR, IS_FLYING);
    }

    void CombatDisplay::HandleEndOfTurnTasks()
    {
        UpdateHealthTasks();
        combatAnimationPtr_->ShakeAnimStop(boost::none);
    }

    void CombatDisplay::UpdateHealthTasks()
    {
        for (const auto & COMBAT_NODE_PTR : combatTree_.GetCombatNodes())
        {
            COMBAT_NODE_PTR->HealthChangeTasks();
        }
    }

    bool CombatDisplay::IsCreatureVisible(const creature::CreaturePtr_t CREATURE_PTR) const
    {
        return combatTree_.GetNodePtr(CREATURE_PTR)->WillDraw();
    }

    bool CombatDisplay::AreAllCreaturesVisible(
        const creature::CreaturePVec_t & CREATURES_TO_CHECK_PVEC)
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (CREATURES_TO_CHECK_PVEC.empty() == false),
            "combat::CombatDisplay::AreAllCreaturesVisible() was given a "
                << "CREATURES_TO_CHECK_PVEC that was empty.");

        const auto COMBATNODES_PVEC { GetCombatNodesForCreatures(CREATURES_TO_CHECK_PVEC) };

        for (const auto & COMBAT_NODE_PTR : COMBATNODES_PVEC)
        {
            if (COMBAT_NODE_PTR->WillDraw() == false)
            {
                return false;
            }
        }

        return true;
    }

    bool CombatDisplay::IsZoomOutRequired(const creature::CreaturePVec_t & CREATURES_PVEC) const
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (CREATURES_PVEC.empty() == false),
            "combat::CombatDisplay::IsZoomOutRequired() was given a "
                << "CREATURES_PVEC that was empty.");

        auto horizPosDiffMax { 0.0f };
        auto vertPosDiffMax { 0.0f };

        const auto COMBAT_NODES_PVEC { GetCombatNodesForCreatures(CREATURES_PVEC) };
        for (const auto & OUTER_COMBAT_NODE_PTR : COMBAT_NODES_PVEC)
        {
            for (const auto & INNER_COMBAT_NODE_PTR : COMBAT_NODES_PVEC)
            {
                if (OUTER_COMBAT_NODE_PTR != INNER_COMBAT_NODE_PTR)
                {
                    auto HORIZ_DIFF { std::abs(
                        OUTER_COMBAT_NODE_PTR->GetEntityPos().x
                        - INNER_COMBAT_NODE_PTR->GetEntityPos().x) };

                    if (HORIZ_DIFF > horizPosDiffMax)
                    {
                        horizPosDiffMax = HORIZ_DIFF;
                    }

                    auto VERT_DIFF { std::abs(
                        OUTER_COMBAT_NODE_PTR->GetEntityPos().y
                        - INNER_COMBAT_NODE_PTR->GetEntityPos().y) };

                    if (VERT_DIFF > vertPosDiffMax)
                    {
                        vertPosDiffMax = VERT_DIFF;
                    }
                }
            }
        }

        const auto TOLERANCE { sfutil::MapByRes(300.0f, 600.0f) };

        return (
            (horizPosDiffMax > (sfml_util::Display::Instance()->GetWinWidth() - TOLERANCE))
            || (vertPosDiffMax > (sfml_util::Display::Instance()->GetWinHeight() - TOLERANCE)));
    }

    const sf::Vector2f CombatDisplay::FindCenterOfCreatures(
        const creature::CreaturePVec_t & CREATURES_TO_CENTER_ON_PVEC) const
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (CREATURES_TO_CENTER_ON_PVEC.empty() == false),
            "combat::CombatDisplay::FindCenterOfCreatures() was given a "
                << "CREATURES_TO_CENTER_ON_PVEC that was empty.");

        std::vector<float> horizPosVec;
        std::vector<float> vertPosVec;

        for (const auto & COMBAT_NODE_PTR : GetCombatNodesForCreatures(CREATURES_TO_CENTER_ON_PVEC))
        {
            horizPosVec.emplace_back(
                COMBAT_NODE_PTR->GetEntityPos().x
                + (COMBAT_NODE_PTR->GetEntityRegion().width * 0.5f));

            vertPosVec.emplace_back(
                COMBAT_NODE_PTR->GetEntityPos().y
                + (COMBAT_NODE_PTR->GetEntityRegion().height * 0.5f));
        }

        const auto HORIZ_AVG_POS { std::accumulate(horizPosVec.begin(), horizPosVec.end(), 0.0f)
                                   / static_cast<float>(horizPosVec.size()) };

        const auto VERT_AVG_POS { std::accumulate(vertPosVec.begin(), vertPosVec.end(), 0.0f)
                                  / static_cast<float>(vertPosVec.size()) };

        return sf::Vector2f(HORIZ_AVG_POS, VERT_AVG_POS);
    }

    void CombatDisplay::SetCreatureHighlight(
        const creature::CreaturePtr_t CREATURE_PTR, const bool WILL_HIGHLIGHT)
    {
        GetCombatNodeForCreature(CREATURE_PTR)->SetHighlight(WILL_HIGHLIGHT, false);
    }

    void CombatDisplay::InitialPlayerPartyCombatTreeSetup()
    {
        // apply the player's preferred blocking pattern if one is saved
        InitialCreaturePositionsSetup(true);

        /*
         * um, we should still be doing this...right?
         *
        //whatever blocking order pattern, leave the disabled characters farthest behind
        const int DISABLED_CREATURES_POSITION(position - 1);
        const auto CHAR_PVEC( game::Game::Instance()->State().Party().Characters() );
        for (const auto & NEXT_CHARACTER_PTR : CHAR_PVEC)
        {
            if (NEXT_CHARACTER_PTR->CanTakeAction() == false)
            {
                const ID_t NEXT_NODE_ID(combatTree_.GetNodeId(NEXT_CHARACTER_PTR));
                combatTree_.GetNodePtr(NEXT_NODE_ID)->SetBlockingPos(DISABLED_CREATURES_POSITION);
            }
        }

        combatTree_.ConnectAllAtPosition(DISABLED_CREATURES_POSITION,
            combat::EdgeType::ShoulderToShoulder);
        */
    }

    void CombatDisplay::InitialNonPlayerPartyCombatTreeSetup()
    {
        InitialCreaturePositionsSetup(false);
    }

    void CombatDisplay::SortCreatureListByDisplayedPosition(
        creature::CreaturePVec_t & creaturesPVec) const
    {
        std::sort(
            creaturesPVec.begin(),
            creaturesPVec.end(),
            [this](const auto & PTR_A, const auto & PTR_B) {
                const auto COMBAT_NODE_PTR_A { GetCombatNodeForCreature(PTR_A) };
                const auto COMBAT_NODE_PTR_B { GetCombatNodeForCreature(PTR_B) };

                return std::tie(
                           COMBAT_NODE_PTR_A->GetEntityPos().x, COMBAT_NODE_PTR_A->GetEntityPos().y)
                    < std::tie(
                           COMBAT_NODE_PTR_B->GetEntityPos().x,
                           COMBAT_NODE_PTR_B->GetEntityPos().y);
            });
    }

    void CombatDisplay::HandleCombatNodeElimination(
        const combat::CombatNodePVec_t & COMBATNODES_PVEC)
    {
        IDVec_t ids;
        ids.reserve(COMBATNODES_PVEC.size());

        for (const auto & NEXT_COMBATNODE_PTR : COMBATNODES_PVEC)
        {
            ids.emplace_back(combatTree_.GetNodeId(NEXT_COMBATNODE_PTR->Creature()));
        }

        RemoveCombatNodes(ids);
    }

    void CombatDisplay::HandleCombatNodeElimination(const creature::CreaturePtr_t CREATURE_PTR)
    {
        HandleCombatNodeElimination({ combatTree_.GetNodePtr(CREATURE_PTR) });
    }

    const creature::CreaturePVec_t CombatDisplay::GetObstacleCreaturesAtBlockingPos(
        const creature::CreaturePtr_t CREATURE_ATTEMPTING_PTR, const int BLOCKING_POS) const
    {
        const auto COMBAT_NODES_PVEC { combatTree_.GetCombatNodes() };

        creature::CreaturePVec_t creaturesPVec;
        creaturesPVec.reserve(COMBAT_NODES_PVEC.size());

        for (const auto & NEXT_COMBAT_NODE_PTR : COMBAT_NODES_PVEC)
        {
            const auto NEXT_CREATURE_PTR { NEXT_COMBAT_NODE_PTR->Creature() };

            if ((NEXT_COMBAT_NODE_PTR->GetBlockingPos() == BLOCKING_POS)
                && (NEXT_CREATURE_PTR->HasConditionNotAThreatPerm(creature::UnconOpt::Include)
                    == false)
                && (Encounter::Instance()->GetTurnInfoCopy(NEXT_CREATURE_PTR).GetIsFlying()
                    == false)
                && (NEXT_CREATURE_PTR->IsPlayerCharacter()
                    == CREATURE_ATTEMPTING_PTR->IsPlayerCharacter()))
            {
                creaturesPVec.emplace_back(NEXT_CREATURE_PTR);
            }
        }

        return creaturesPVec;
    }

    const creature::CreaturePVec_t CombatDisplay::GetCreaturesInRoaringDistance(
        const creature::CreaturePtr_t CREATURE_ROARING_PTR) const
    {
        const auto OPPONENT_CREATURES_PVEC { creature::Algorithms::PlayersByType(
            ((CREATURE_ROARING_PTR->IsPlayerCharacter()) ? creature::Algorithms::TypeOpt::NonPlayer
                                                         : creature::Algorithms::TypeOpt::Player),
            creature::Algorithms::Living) };

        creature::CreaturePVec_t creaturesPVec;
        creaturesPVec.reserve(OPPONENT_CREATURES_PVEC.size());

        for (const auto & NEXT_OPPONENT_CREATURE_PTR : OPPONENT_CREATURES_PVEC)
        {
            if (std::abs(combatTree_.GetBlockingDistanceBetween(
                    CREATURE_ROARING_PTR, NEXT_OPPONENT_CREATURE_PTR))
                <= 2)
            {
                creaturesPVec.emplace_back(NEXT_OPPONENT_CREATURE_PTR);
            }
        }

        return creaturesPVec;
    }

    const creature::CreaturePVec_t CombatDisplay::GetCreaturesFlying() const
    {
        creature::CreaturePVec_t creaturesFlyingPVec;

        const auto COMBAT_NODES_PVEC { combatTree_.GetCombatNodes() };
        creaturesFlyingPVec.reserve(COMBAT_NODES_PVEC.size());

        for (const auto & COMBAT_NODE_PTR : COMBAT_NODES_PVEC)
        {
            if (COMBAT_NODE_PTR->IsFlying())
            {
                creaturesFlyingPVec.emplace_back(COMBAT_NODE_PTR->Creature());
            }
        }

        return creaturesFlyingPVec;
    }

    const CreatureBlockingPosMap_t CombatDisplay::MakeCreatureBlockingMap() const
    {
        CreatureBlockingPosMap_t creatureBlockingMap;

        const auto COMBAT_NODES_PVEC { combatTree_.GetCombatNodes() };

        creatureBlockingMap.Reserve(COMBAT_NODES_PVEC.size());

        for (const auto & COMBAT_NODE_PTR : COMBAT_NODES_PVEC)
        {
            creatureBlockingMap[COMBAT_NODE_PTR->Creature()] = COMBAT_NODE_PTR->GetBlockingPos();
        }

        return creatureBlockingMap;
    }

    void CombatDisplay::SetBlockingPositions(const CreatureBlockingPosMap_t & BLOCKING_MAP)
    {
        for (const auto & CREATURE_PTR_BLOCKINGPOS_PAIR : BLOCKING_MAP)
        {
            for (const auto & COMBAT_NODE_PTR : combatTree_.GetCombatNodes())
            {
                if (CREATURE_PTR_BLOCKINGPOS_PAIR.first == COMBAT_NODE_PTR->Creature())
                {
                    COMBAT_NODE_PTR->SetBlockingPos(CREATURE_PTR_BLOCKINGPOS_PAIR.second);
                }
            }
        }

        combatTree_.ResetAllEdges();
        PositionCombatTreeCells(false);
        UpdateWhichNodesWillDraw();
    }

    const sf::Vector2f
        CombatDisplay::GetCombatNodeCenter(const creature::CreaturePtr_t CREATURE_PTR) const
    {
        for (const auto & COMBAT_NODE_PTR : combatTree_.GetCombatNodes())
        {
            if (COMBAT_NODE_PTR->Creature() == CREATURE_PTR)
            {
                const auto REGION { COMBAT_NODE_PTR->GetEntityRegion() };
                return sf::Vector2f(
                    REGION.left + REGION.width * 0.5f, REGION.top + REGION.height * 0.5f);
            }
        }

        return sf::Vector2f(0.0f, 0.0f);
    }

    void CombatDisplay::SetInitiallyFlyingCreaturesToFlying()
    {
        for (const auto & COMBAT_NODE_PTR : combatTree_.GetCombatNodes())
        {
            const auto CREATURE_PTR { COMBAT_NODE_PTR->Creature() };

            if ((creature::race::WillInitiallyFly(CREATURE_PTR->Race()))
                || (creature::role::WillInitiallyFly(CREATURE_PTR->Role())))
            {
                COMBAT_NODE_PTR->IsFlying(true);
            }
        }
    }

    void CombatDisplay::RepositionCombatNodesBasedOnSliderPosition(const float SLIDER_POS)
    {
        for (const auto & NEXT_NODEPOSTRACK_PAIR : nodePosTrackerMap_)
        {
            const float NEW_POS_HORIZ(
                NEXT_NODEPOSTRACK_PAIR.second.posHorizOrig
                + (NEXT_NODEPOSTRACK_PAIR.second.horizDiff * SLIDER_POS));

            const float NEW_POS_VERT(
                NEXT_NODEPOSTRACK_PAIR.second.posVertOrig
                + (NEXT_NODEPOSTRACK_PAIR.second.vertDiff * SLIDER_POS));

            NEXT_NODEPOSTRACK_PAIR.first->SetEntityPos(NEW_POS_HORIZ, NEW_POS_VERT);
        }
    }

    void CombatDisplay::PositionCombatTreeCells(const bool WILL_DELAY)
    {
        nodePosTrackerMap_.Clear();

        if (combatTree_.GetCombatNodes().empty())
        {
            return;
        }

        // find highest and lowest blocking position,
        // and the most creatures shoulder-to-shoulder,
        // and the longest displayed name
        int lowestBlockingPos(0);
        int highestBlockingPos(0);
        std::size_t shoulderToShoulderMax(0);
        float maxNameWidth(0.0f);
        for (const auto & COMBAT_NODE_PTR : combatTree_.GetCombatNodes())
        {
            if (COMBAT_NODE_PTR->GetBlockingPos() < lowestBlockingPos)
            {
                lowestBlockingPos = COMBAT_NODE_PTR->GetBlockingPos();
            }

            if (COMBAT_NODE_PTR->GetBlockingPos() > highestBlockingPos)
            {
                highestBlockingPos = COMBAT_NODE_PTR->GetBlockingPos();
            }

            const auto SHOULDER_TO_SHOULDER_COUNT { combatTree_.VertexCountByBlockingPos(
                COMBAT_NODE_PTR->GetBlockingPos()) };

            if (shoulderToShoulderMax < SHOULDER_TO_SHOULDER_COUNT)
            {
                shoulderToShoulderMax = SHOULDER_TO_SHOULDER_COUNT;
            }

            const auto NEXT_NAME_WIDTH { COMBAT_NODE_PTR->GetNameWidth() };

            if (maxNameWidth < NEXT_NAME_WIDTH)
            {
                maxNameWidth = NEXT_NAME_WIDTH;
            }
        }

        // If not already done, add a margin of extra blocking positions around the edges
        // so that creatures have room to retreat.
        // Store those as min/max blocking positions so creatures cannot move outside of them.
        if (BLOCKING_POS_INVALID_ == blockingPosMin_)
        {
            lowestBlockingPos -= BLOCKING_POS_MARGIN_;
            blockingPosMin_ = lowestBlockingPos;

            highestBlockingPos += BLOCKING_POS_MARGIN_;
            blockingPosMax_ = highestBlockingPos;

            shoulderToShoulderMax += static_cast<std::size_t>(2 * BLOCKING_POS_MARGIN_);
        }

        const float CELL_WIDTH_MIN(sfutil::MapByRes(
            sfml_util::StandardImageDimmension() * POSITIONING_CELL_SIZE_RATIO_MIN_VERT_,
            sfml_util::StandardImageDimmension() * POSITIONING_CELL_SIZE_RATIO_MAX_VERT_));

        const float CELL_WIDTH_ORIG(std::max(maxNameWidth, CELL_WIDTH_MIN));

        // adjust vars for zoomLevel_
        const float CELL_WIDTH_ZOOM_ADJ(CELL_WIDTH_ORIG); // already adjust for by nameCharSize_
        const float CELL_HEIGHT_ZOOM_ADJ(CELL_HEIGHT_ * zoomLevel_);

        const float POSITIONING_BETWEEN_SPACER_HORIZ_ZOOM_ADJ(
            POSITIONING_BETWEEN_SPACER_HORIZ_ * zoomLevel_);

        const float POSITIONING_BETWEEN_SPACER_VERT_ZOOM_ADJ(
            POSITIONING_BETWEEN_SPACER_VERT_ * zoomLevel_);

        const float POSITIONING_MARGIN_HORIZ_ZOOM_ADJ(POSITIONING_MARGIN_HORIZ_ * zoomLevel_);
        const float POSITIONING_MARGIN_VERT_ZOOM_ADJ(POSITIONING_MARGIN_VERT_ * zoomLevel_);

        const float CELL_HEIGHT_ZOOM_ADJ_WITH_SPACER(
            CELL_HEIGHT_ZOOM_ADJ + POSITIONING_BETWEEN_SPACER_VERT_ZOOM_ADJ);

        const float CELL_WIDTH_ZOOM_ADJ_WITH_SPACER(
            CELL_WIDTH_ZOOM_ADJ + POSITIONING_BETWEEN_SPACER_HORIZ_ZOOM_ADJ);

        battlefieldWidth_
            = std::abs(static_cast<float>(lowestBlockingPos) * CELL_WIDTH_ZOOM_ADJ_WITH_SPACER)
            + std::abs(static_cast<float>(highestBlockingPos) * CELL_WIDTH_ZOOM_ADJ_WITH_SPACER)
            + (POSITIONING_MARGIN_HORIZ_ZOOM_ADJ * 2.0f);

        battlefieldHeight_
            = (static_cast<float>(shoulderToShoulderMax) * CELL_HEIGHT_ZOOM_ADJ_WITH_SPACER)
            + (POSITIONING_MARGIN_VERT_ZOOM_ADJ * 2.0f);

        // set battlefield positions
        misc::VectorMap<int, std::size_t> shoulderToShoulderBlockingMap;
        for (const auto & COMBAT_NODE_PTR : combatTree_.GetCombatNodes())
        {
            const auto NEXT_BLOCKING_POS { COMBAT_NODE_PTR->GetBlockingPos() };

            // count the number of verts are shoulder-to-shoulder (vertical on screen) with this one
            const auto SHOULDER_TO_SHOULDER_COUNT { combatTree_.VertexCountByBlockingPos(
                NEXT_BLOCKING_POS) };

            // keep track of which shoulder-to-shoulder this vert/NEXT_BLOCKING_POS is
            const auto SHOULDER_TO_SHOULDER_POS {
                shoulderToShoulderBlockingMap[NEXT_BLOCKING_POS]++
            };

            const auto POS_LEFT { (battlefieldWidth_ * 0.5f)
                                  + (static_cast<float>(NEXT_BLOCKING_POS)
                                     * CELL_WIDTH_ZOOM_ADJ_WITH_SPACER) };

            const auto SHOULDER_TO_SHOULDER_TOTAL_HEIGHT_HALF {
                (static_cast<float>(SHOULDER_TO_SHOULDER_COUNT) * 0.5f)
                * CELL_HEIGHT_ZOOM_ADJ_WITH_SPACER
            };

            const auto SHOULDER_TO_SHOULDER_POS_HEIGHT {
                static_cast<float>(SHOULDER_TO_SHOULDER_POS) * CELL_HEIGHT_ZOOM_ADJ_WITH_SPACER
            };

            const auto POS_TOP { ((battlefieldHeight_ * 0.5f)
                                  - SHOULDER_TO_SHOULDER_TOTAL_HEIGHT_HALF)
                                 + SHOULDER_TO_SHOULDER_POS_HEIGHT };

            const sf::FloatRect RECT(POS_LEFT, POS_TOP, CELL_WIDTH_ZOOM_ADJ, CELL_HEIGHT_ZOOM_ADJ);

            if (WILL_DELAY)
            {
                nodePosTrackerMap_[COMBAT_NODE_PTR]
                    = NodePosTracker(COMBAT_NODE_PTR, POS_LEFT, POS_TOP);
            }
            else
            {
                COMBAT_NODE_PTR->SetRegion(RECT);
                COMBAT_NODE_PTR->SetCharacterSize(nameCharSizeCurr_);
            }
        }
    }

    void CombatDisplay::RemoveCombatNodes(const IDVec_t & COMBAT_TREE_IDS_TO_REMOVE_VEC)
    {
        for (const auto & ID : COMBAT_TREE_IDS_TO_REMOVE_VEC)
        {
            EntityRemove(combatTree_.GetNodePtr(ID).Ptr());
            combatTree_.RemoveVertex(ID, true);
        }

        // re-position CombatNodes/Creatures on the battlefield in the slow animated way
        PositionCombatTreeCells(true);
    }

    void CombatDisplay::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        if (isCombatOver_)
        {
            return;
        }

        Stage::UpdateTime(ELAPSED_TIME_SECONDS);

        summaryViewUPtr_->UpdateTime(ELAPSED_TIME_SECONDS);

        if (summaryViewUPtr_->MovingDir() == sfml_util::Moving::Toward)
        {
            CreatureToneDown(summaryViewUPtr_->GetTransitionStatus());
        }
        else if (summaryViewUPtr_->MovingDir() == sfml_util::Moving::Away)
        {
            CreatureToneDown(1.0f - summaryViewUPtr_->GetTransitionStatus());

            if (summaryViewUPtr_->GetTransitionStatus() > 0.98f)
            {
                SetIsSummaryViewInProgress(false);

                if (combatAnimationPtr_->IsThereAShakeAnimWasComabtNode())
                {
                    combatAnimationPtr_->ShakeAnimRestart();
                }
            }
        }
    }

    void CombatDisplay::CreatureToneDown(const float TONE_DOWN_VAL)
    {
        for (const auto & COMBAT_NODE_PTR : combatTree_.GetCombatNodes())
        {
            if (summaryViewUPtr_->CombatNodePtrOpt()
                && (summaryViewUPtr_->CombatNodePtrOpt().value() != COMBAT_NODE_PTR))
            {
                COMBAT_NODE_PTR->SetToneDown(TONE_DOWN_VAL);
            }
        }
    }

    void CombatDisplay::EndOfCombatCleanup()
    {
        isCombatOver_ = true;
        RemoveCombatNodes(combatTree_.GetAllIds());
    }

    void CombatDisplay::InitialCreaturePositionsSetup(const bool WILL_POSITION_PLAYERS)
    {
        // get a list of all combat nodes, either of all players or non-players
        CombatNodePVec_t cNodesOfPlayerTypePVec { combatTree_.GetCombatNodesOfPlayerType(
            WILL_POSITION_PLAYERS) };

        // assign combat_node's blocking positions
        const auto WILL_BLOCKING_POSITIONS_INCREASE { !WILL_POSITION_PLAYERS };
        auto blockingPos { 0 };

        for (int typeNumber(0); typeNumber < BlockingPosType::Count; ++typeNumber)
        {
            // move all combat_nodes with a role that matches the blocking type into a new vector
            const auto BLOCKING_POS_TYPE { static_cast<BlockingPosType::Enum>(typeNumber) };
            const auto ROLES_VEC { creature::role::RolesOfBlockingPosType(BLOCKING_POS_TYPE) };
            CombatNodePVec_t cNodesOfBlockingTypeRolePVec;

            MoveCombatNodesOfGivenRolesIntoTargetVec(
                ROLES_VEC, cNodesOfPlayerTypePVec, cNodesOfBlockingTypeRolePVec);

            if (cNodesOfBlockingTypeRolePVec.empty())
            {
                continue;
            }

            misc::Vector::ShuffleVec(cNodesOfBlockingTypeRolePVec);

            while (cNodesOfBlockingTypeRolePVec.empty() == false)
            {
                if (WILL_BLOCKING_POSITIONS_INCREASE)
                {
                    ++blockingPos;
                }
                else
                {
                    --blockingPos;
                }

                for (std::size_t shoulder(0); shoulder < SHOULDER_TO_SHOULDER_MAX_; ++shoulder)
                {
                    cNodesOfBlockingTypeRolePVec.at(0)->SetBlockingPos(blockingPos);
                    cNodesOfBlockingTypeRolePVec.erase(std::begin(cNodesOfBlockingTypeRolePVec));
                    if (cNodesOfBlockingTypeRolePVec.empty())
                    {
                        break;
                    }
                }

                combatTree_.ConnectAllAtPosition(blockingPos, combat::EdgeType::ShoulderToShoulder);
            }
        }
    }

    void CombatDisplay::MoveCombatNodesOfGivenRolesIntoTargetVec(
        const creature::RoleVec_t & ROLES_VEC,
        CombatNodePVec_t & sourceCNodePVec,
        CombatNodePVec_t & targetCNodePVec)
    {
        auto findMatchingRoleOfCombatNode = [&ROLES_VEC](const CombatNodePtr_t & CNODE_PTR) {
            const auto ROLE { CNODE_PTR->Creature()->Role() };

            const auto FOUND_ITER { std::find(std::begin(ROLES_VEC), std::end(ROLES_VEC), ROLE) };

            return (FOUND_ITER != std::end(ROLES_VEC));
        };

        std::copy_if(
            std::begin(sourceCNodePVec),
            std::end(sourceCNodePVec),
            std::back_inserter(targetCNodePVec),
            findMatchingRoleOfCombatNode);

        sourceCNodePVec.erase(
            std::remove_if(
                std::begin(sourceCNodePVec),
                std::end(sourceCNodePVec),
                findMatchingRoleOfCombatNode),
            std::end(sourceCNodePVec));
    }

} // namespace combat
} // namespace heroespath
