// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
//
// combat-display.cpp
//
#include "combat-display.hpp"

#include "sfml-util/sfml-util.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/tile.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/gui/box.hpp"
#include "sfml-util/gui/text-info.hpp"
#include "sfml-util/gui/creature-image-manager.hpp"
#include "sfml-util/gui/text-region.hpp"

#include "game/game.hpp"
#include "game/log-macros.hpp"
#include "game/game-data-file.hpp"
#include "game/player/party.hpp"
#include "game/player/character.hpp"
#include "game/non-player/party.hpp"
#include "game/non-player/character.hpp"
#include "game/combat/encounter.hpp"
#include "game/combat/combat-node.hpp"
#include "game/combat/combat-anim.hpp"
#include "game/creature/algorithms.hpp"
#include "game/creature/name-info.hpp"
#include "game/state/game-state.hpp"
#include "game/combat/summary-view.hpp"

#include "misc/real.hpp"

#include <sstream>
#include <numeric>
#include <string>
#include <algorithm>


namespace game
{
namespace combat
{

    NodePosTracker::NodePosTracker()
    :
        posHorizOrig(0.0f),
        posVertOrig (0.0f),
        horizDiff   (0.0f),
        vertDiff    (0.0f)
    {}


    NodePosTracker::NodePosTracker(const CombatNodePtr_t COMBAT_NODE_PTR,
                                   const float           TARGET_POS_LEFT,
                                   const float           TARGET_POS_TOP)
    :
        posHorizOrig  (COMBAT_NODE_PTR->GetEntityPos().x),
        posVertOrig   (COMBAT_NODE_PTR->GetEntityPos().y),
        horizDiff     (TARGET_POS_LEFT - COMBAT_NODE_PTR->GetEntityPos().x),
        vertDiff      (TARGET_POS_TOP - COMBAT_NODE_PTR->GetEntityPos().y)
    {}


    const float       CombatDisplay::BATTLEFIELD_MARGIN_                    (0.0f);
    BlockingMap_t     CombatDisplay::blockingMap_                           ;
    const float       CombatDisplay::POSITIONING_CELL_SIZE_RATIO_MIN_HORIZ_ (0.4f);
    const float       CombatDisplay::POSITIONING_CELL_SIZE_RATIO_MAX_HORIZ_ (4.0f);
    const float       CombatDisplay::POSITIONING_CELL_SIZE_RATIO_MIN_VERT_  (0.4f);
    const float       CombatDisplay::POSITIONING_CELL_SIZE_RATIO_MAX_VERT_  (2.0f);
    const int         CombatDisplay::BLOCKING_POS_MARGIN_                   (3);

    //any number over one thousand should work here
    const int         CombatDisplay::BLOCKING_POS_INVALID_                  (1001);

    const float       CombatDisplay::CREATURE_MOVE_SLIDER_SPEED_            (4.0f);
    const float       CombatDisplay::BATTLEFIELD_DRAG_SPEED_                (3.0f);
    const std::size_t CombatDisplay::SHOULDER_TO_SHOULDER_MAX_              (10);
    const std::size_t CombatDisplay::SHOULDER_TO_SHOULDER_OPPOSITE_TYPE_MAX_(2);


    CombatDisplay::CombatDisplay(const sf::FloatRect & REGION)
    :
        Stage("CombatDisplay", REGION),
        POSITIONING_MARGIN_HORIZ_        (sfml_util::MapByRes(50.0f, 300.0f)),
        POSITIONING_MARGIN_VERT_         (sfml_util::MapByRes(50.0f, 300.0f)),
        POSITIONING_BETWEEN_SPACER_HORIZ_(sfml_util::MapByRes(5.0f,  200.0f)),
        POSITIONING_BETWEEN_SPACER_VERT_ (sfml_util::MapByRes(25.0f, 200.0f)),

        CELL_HEIGHT_                     (sfml_util::MapByRes(
            sfml_util::gui::CreatureImageManager::DimmensionMax() *
                POSITIONING_CELL_SIZE_RATIO_MIN_VERT_,
            sfml_util::gui::CreatureImageManager::DimmensionMax() *
                POSITIONING_CELL_SIZE_RATIO_MAX_VERT_)),

        NAME_CHAR_SIZE_ORIG_             (sfml_util::FontManager::Instance()->Size_Smallish()),
        SCREEN_WIDTH_                    (sfml_util::Display::Instance()->GetWinWidth()),
        SCREEN_HEIGHT_                   (sfml_util::Display::Instance()->GetWinHeight()),
        nameCharSizeCurr_          (NAME_CHAR_SIZE_ORIG_),
        battlefieldRect_           (),
        boxUPtr_                   (),
        bgTexture_                 (),
        offScreenTexture_          (),
        offScreenSprite_           (),
        offScreenPosX_             (0.0f),
        offScreenPosY_             (0.0f),
        combatTree_                (),
        prevScrollPosVert_         (0.0f),
        prevScrollPosHoriz_        (0.0f),
        summaryViewUPtr_           (std::make_unique<SummaryView>()),
        isSummaryViewAllowed_      (false),
        isScrollAllowed_           (false),
        battlefieldWidth_          (0.0f),
        battlefieldHeight_         (0.0f),
        blockingPosMin_            (BLOCKING_POS_INVALID_),
        blockingPosMax_            (BLOCKING_POS_INVALID_),
        zoomLevel_                 (1.0f),
        isMouseHeldDownInBF_       (false),
        prevMousePos_              (0.0f, 0.0f),
        isPlayerTurn_              (false),
        isStatusMessageAnim_       (false),
        isSummaryViewInProgress_   (false),
        combatNodeToGuiEntityMap_  (),
        combatAnimationPtr_        (nullptr),
        isCombatOver_              (false),
        nodePosTrackerMap_         (),
        centeringToPosV_           (-1.0f, -1.0f) //any negative values will work here
    {}


    CombatDisplay::~CombatDisplay()
    {
        ClearAllEntities();
    }


    void CombatDisplay::Setup()
    {
        //create CombatNodes and add them into the combateTree_
        auto const PLAYER_CHAR_PVEC( game::Game::Instance()->State().Party().Characters() );

        for (auto const NEXT_CHARACTER_PTR : PLAYER_CHAR_PVEC)
        {
            const combat::CombatNodeSPtr_t COMBAT_NODE_SPTR(
                std::make_shared<combat::CombatNode>(
                    NEXT_CHARACTER_PTR,
                    creature::NameInfo::Instance()->DefaultFont(),
                    nameCharSizeCurr_) );

            EntityAdd(COMBAT_NODE_SPTR.get());
            combatNodeToGuiEntityMap_[COMBAT_NODE_SPTR] = COMBAT_NODE_SPTR.get();
            combatTree_.AddVertex(COMBAT_NODE_SPTR);
        }
        InitialPlayerPartyCombatTreeSetup();
        //
        auto const NONPLAYER_CHAR_PVEC( Encounter::Instance()->NonPlayerParty().Characters() );
        for (auto const NEXT_CHARACTER_PTR : NONPLAYER_CHAR_PVEC)
        {
            const combat::CombatNodeSPtr_t COMBAT_NODE_SPTR(
                std::make_shared<combat::CombatNode>(
                    NEXT_CHARACTER_PTR,
                    creature::NameInfo::Instance()->DefaultFont(),
                    nameCharSizeCurr_) );

            EntityAdd(COMBAT_NODE_SPTR.get());
            combatNodeToGuiEntityMap_[COMBAT_NODE_SPTR] = COMBAT_NODE_SPTR.get();
            combatTree_.AddVertex(COMBAT_NODE_SPTR);
        }
        InitialNonPlayerPartyCombatTreeSetup();

        //establish primary drawing area as battlefieldRect_
        //StageRegionSet() must have already been called
        battlefieldRect_ = sf::FloatRect(StageRegionLeft(),
                                         StageRegionTop(),
                                         StageRegionWidth() - BATTLEFIELD_MARGIN_,
                                         StageRegionHeight() - BATTLEFIELD_MARGIN_);

        PositionCombatTreeCells(false);

        //battlefield bounding box
        const sfml_util::gui::box::Info BOX_INFO(
            true,
            battlefieldRect_,
            sfml_util::gui::ColorSet(sf::Color::White, sf::Color::Transparent));

        boxUPtr_ = std::make_unique<sfml_util::gui::box::Box>("CombatDisplay's'", BOX_INFO);

        //load background texture
        sfml_util::LoadTexture(bgTexture_,
            GameDataFile::Instance()->GetMediaPath("media-images-backgrounds-tile-darkpaper"));

        bgTexture_.setRepeated(true);
        offScreenSprite_.setTexture(bgTexture_);

        //setup offscreen texture
        auto const BG_TEXTUER_SIZE{ bgTexture_.getSize() };

        auto const OFFSCREEN_TEXTURE_SIZE_X(((sfml_util::Display::Instance()->GetWinWidthu()  /
            BG_TEXTUER_SIZE.x) + 1) * BG_TEXTUER_SIZE.x);
        
        auto const OFFSCREEN_TEXTURE_SIZE_Y(((sfml_util::Display::Instance()->GetWinHeightu() /
            BG_TEXTUER_SIZE.y) + 1) * BG_TEXTUER_SIZE.y);
        
        offScreenTexture_.create(OFFSCREEN_TEXTURE_SIZE_X, OFFSCREEN_TEXTURE_SIZE_Y);
        offScreenTexture_.clear(sf::Color::Transparent);
        offScreenTexture_.setRepeated(true);

        //draw background texture to offscreen texture (tile)
        sf::RenderStates states;
        const sf::Rect<unsigned> TILE_RECT(0,
                                           0,
                                           offScreenTexture_.getSize().x,
                                           offScreenTexture_.getSize().y);

        sfml_util::Tile2(sfml_util::ConvertRect<unsigned, float>(TILE_RECT),
                         offScreenSprite_,
                         offScreenTexture_,
                         states);

        offScreenTexture_.display();

        //setup offscreen drawing sprite
        offScreenSprite_.setTexture(offScreenTexture_.getTexture());
        offScreenSprite_.setTextureRect( sfml_util::ConvertRect<float, int>(battlefieldRect_) );
        offScreenSprite_.setPosition(battlefieldRect_.left, battlefieldRect_.top);
    }


    void CombatDisplay::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        target.draw(offScreenSprite_, STATES);
        target.draw( * boxUPtr_, STATES);
        Stage::Draw(target, STATES);
        summaryViewUPtr_->Draw(target, STATES);
    }


    void CombatDisplay::SetMouseHover(const sf::Vector2f & MOUSE_POS,
                                      const bool           IS_MOUSE_HOVERING)
    {
        Stage::SetMouseHover(MOUSE_POS, IS_MOUSE_HOVERING);
    }


    bool CombatDisplay::StartSummaryView(const sf::Vector2f & MOUSE_POS)
    {
        auto combatNodePtr(combatTree_.GetNode(MOUSE_POS.x, MOUSE_POS.y));

        if (isSummaryViewAllowed_ &&
            (combatNodePtr != nullptr) &&
            (combatNodePtr->GetEntityWillDraw() == true) &&
            (false == isMouseHeldDownInBF_) &&
            isPlayerTurn_ &&
            (GetIsStatusMessageAnimating() == false))
        {
            combatAnimationPtr_->ShakeAnimTemporaryStop(combatNodePtr->Creature());
            summaryViewUPtr_->SetupAndStartTransition(combatNodePtr, battlefieldRect_);
            SetIsSummaryViewInProgress(true);
            return true;
        }
        else
        {
            return false;
        }
    }


    float CombatDisplay::SetZoomLevel(const float ZOOM_LEVEL)
    {
        const float ORIG_ZOOM_LEVEL(zoomLevel_);
        zoomLevel_ = ZOOM_LEVEL;

        if (misc::IsRealClose(ZOOM_LEVEL, ORIG_ZOOM_LEVEL))
        {
            return zoomLevel_;
        }

        //prevent zoom level from going too low
        const float MIN_ZOOM_LEVEL(0.1f);
        if (zoomLevel_ < MIN_ZOOM_LEVEL)
        {
            zoomLevel_ = MIN_ZOOM_LEVEL;
        }

        nameCharSizeCurr_ = static_cast<unsigned int>(
            static_cast<float>(NAME_CHAR_SIZE_ORIG_) * ZOOM_LEVEL);

        auto const BATTLEFIELD_CENTER_BEFORE_V{ GetCenterOfAllNodes() };
        PositionCombatTreeCells(false);
        auto const BATTLEFIELD_CENTER_AFTER_V{ GetCenterOfAllNodes() };

        auto const HORIZ_DIFF{ BATTLEFIELD_CENTER_BEFORE_V.x - BATTLEFIELD_CENTER_AFTER_V.x };
        auto const VERT_DIFF{ BATTLEFIELD_CENTER_BEFORE_V.y - BATTLEFIELD_CENTER_AFTER_V.y };

        //move nodes to keep the battlefield centered
        {
            CombatNodePVec_t combatNodesPVec;
            combatNodesPVec.reserve(combatTree_.VertexCount());
            combatTree_.GetCombatNodes(combatNodesPVec);
            for (auto const NEXT_COMBATNODE_PTR : combatNodesPVec)
            {
                NEXT_COMBATNODE_PTR->MoveEntityPos(HORIZ_DIFF, VERT_DIFF);
            }
        }

        //if all creatures are already visible then prevent further zoom out
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

        CombatNodePVec_t combatNodePVec;
        combatNodePVec.reserve(combatTree_.VertexCount());
        combatTree_.GetCombatNodes(combatNodePVec);
        for (auto const nextCombatNodePtrC : combatNodePVec)
        {
            const sf::FloatRect NEXT_VERT_RECT(nextCombatNodePtrC->GetEntityRegion());

            if ((battlefieldRect_.contains(NEXT_VERT_RECT.left, NEXT_VERT_RECT.top)) &&
                (battlefieldRect_.contains(NEXT_VERT_RECT.left + NEXT_VERT_RECT.width, NEXT_VERT_RECT.top)) &&
                (battlefieldRect_.contains(NEXT_VERT_RECT.left, NEXT_VERT_RECT.top + NEXT_VERT_RECT.height)) &&
                (battlefieldRect_.contains(NEXT_VERT_RECT.left + NEXT_VERT_RECT.width, NEXT_VERT_RECT.top + NEXT_VERT_RECT.height)))
            {
                isAnyNodeDrawn = true;
                nextCombatNodePtrC->SetEntityWillDraw(true);
            }
            else
            {
                nextCombatNodePtrC->SetEntityWillDraw(false);
            }
        }

        return isAnyNodeDrawn;
    }


    void CombatDisplay::UpdateMousePos(const sf::Vector2f & MOUSE_POS_V)
    {
        Stage::UpdateMousePos(MOUSE_POS_V);

        if (isScrollAllowed_ && isMouseHeldDownInBF_)
        {
            MoveBattlefieldVert((prevMousePos_.y - MOUSE_POS_V.y) * BATTLEFIELD_DRAG_SPEED_);
            MoveBattlefieldHoriz((prevMousePos_.x - MOUSE_POS_V.x) * BATTLEFIELD_DRAG_SPEED_);
        }

        prevMousePos_ = MOUSE_POS_V;
    }


    void CombatDisplay::UpdateMouseDown(const sf::Vector2f & MOUSE_POS_V)
    {
        if ((GetIsSummaryViewInProgress() == false) && isScrollAllowed_)
        {
            Stage::UpdateMouseDown(MOUSE_POS_V);

            auto const COMBAT_NODE_CLICKED_ON_PTR{
                combatTree_.GetNode(MOUSE_POS_V.x, MOUSE_POS_V.y) };

            if (COMBAT_NODE_CLICKED_ON_PTR == nullptr)
            {
                if (battlefieldRect_.contains(MOUSE_POS_V))
                {
                    isMouseHeldDownInBF_ = true;
                }
            }
        }

        prevMousePos_ = MOUSE_POS_V;
    }


    sfml_util::gui::IGuiEntityPtr_t CombatDisplay::UpdateMouseUp(
        const sf::Vector2f & MOUSE_POS_V)
    {
        if (isScrollAllowed_)
        {
            isMouseHeldDownInBF_ = false;
            return Stage::UpdateMouseUp(MOUSE_POS_V);
        }
        else
        {
            return nullptr;
        }
    }


    const sf::Vector2f CombatDisplay::GetCenterOfAllNodes() const
    {
        CombatNodePVec_t combatNodesPVec;
        combatNodesPVec.reserve(combatTree_.VertexCount());
        combatTree_.GetCombatNodes(combatNodesPVec);

        auto posHorizMin{ 0.0f };
        auto posHorizMax{ 0.0f };
        auto posVertMin{ 0.0f };
        auto posVertMax{ 0.0f };
        auto hasSetAnyValuesYet{ false };
        for (auto const NEXT_COMBATNODE_PTR : combatNodesPVec)
        {
            auto const NEXT_REGION{ NEXT_COMBATNODE_PTR->GetEntityRegion() };
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

        return sf::Vector2f(posHorizMin + ((posHorizMax - posHorizMin) * 0.5f),
            posVertMin + ((posVertMax - posVertMin) * 0.5f));
    }


    void CombatDisplay::PositionCombatTreeCells(const bool WILL_DELAY)
    {
        nodePosTrackerMap_.clear();

        const CombatTree::VertexVec_t VERT_VEC(combatTree_.Vertexes());

        //find highest and lowest blocking position,
        //and the most creatures shoulder-to-shoulder,
        //and the longest displayed name
        int lowestBlockingPos(0);
        int highestBlockingPos(0);
        std::size_t shoulderToShoulderMax(0);
        float maxNameWidth(0.0f);
        for (auto const & NEXT_VERTEX : VERT_VEC)
        {
            if (NEXT_VERTEX.second->GetBlockingPos() < lowestBlockingPos)
            {
                lowestBlockingPos = NEXT_VERTEX.second->GetBlockingPos();
            }

            if (NEXT_VERTEX.second->GetBlockingPos() > highestBlockingPos)
            {
                highestBlockingPos = NEXT_VERTEX.second->GetBlockingPos();
            }

            const std::size_t SHOULDER_TO_SHOULDER_COUNT(combatTree_.VertexCountByBlockingPos(
                static_cast<int>(NEXT_VERTEX.first)));

            if (shoulderToShoulderMax < SHOULDER_TO_SHOULDER_COUNT)
            {
                shoulderToShoulderMax = SHOULDER_TO_SHOULDER_COUNT;
            }

            const float NEXT_NAME_WIDTH(creature::NameInfo::Instance()->Length(
                GetNodeTitle(NEXT_VERTEX.second.get()),
                creature::NameInfo::Instance()->DefaultFont(),
                nameCharSizeCurr_));

            if (maxNameWidth < NEXT_NAME_WIDTH)
            {
                maxNameWidth = NEXT_NAME_WIDTH;
            }
        }

        //If not already done, add a margin of extra blocking positions around the edges
        //so that creatures have room to retreat.
        //Store those as min/max blocking positions so creatures cannot move outside of them.
        if (BLOCKING_POS_INVALID_ == blockingPosMin_)
        {
            lowestBlockingPos -= BLOCKING_POS_MARGIN_;
            blockingPosMin_ = lowestBlockingPos;

            highestBlockingPos += BLOCKING_POS_MARGIN_;
            blockingPosMax_ = highestBlockingPos;

            shoulderToShoulderMax += static_cast<std::size_t>(2 * BLOCKING_POS_MARGIN_);
        }

        const float CELL_WIDTH_MIN( sfml_util::MapByRes(
            sfml_util::gui::CreatureImageManager::DimmensionMax() *
                POSITIONING_CELL_SIZE_RATIO_MIN_VERT_,
            sfml_util::gui::CreatureImageManager::DimmensionMax() *
                POSITIONING_CELL_SIZE_RATIO_MAX_VERT_) );

        const float CELL_WIDTH_ORIG(std::max(maxNameWidth, CELL_WIDTH_MIN));

        //adjust vars for zoomLevel_
        const float CELL_WIDTH_ZOOM_ADJ(CELL_WIDTH_ORIG);//already adjust for by nameCharSize_
        const float CELL_HEIGHT_ZOOM_ADJ(CELL_HEIGHT_ * zoomLevel_);

        const float POSITIONING_BETWEEN_SPACER_HORIZ_ZOOM_ADJ(
            POSITIONING_BETWEEN_SPACER_HORIZ_ * zoomLevel_);
        
        const float POSITIONING_BETWEEN_SPACER_VERT_ZOOM_ADJ(
            POSITIONING_BETWEEN_SPACER_VERT_ * zoomLevel_);

        const float POSITIONING_MARGIN_HORIZ_ZOOM_ADJ(POSITIONING_MARGIN_HORIZ_ * zoomLevel_);
        const float POSITIONING_MARGIN_VERT_ZOOM_ADJ(POSITIONING_MARGIN_VERT_ * zoomLevel_);

        const float CELL_HEIGHT_ZOOM_ADJ_WITH_SPACER(CELL_HEIGHT_ZOOM_ADJ +
            POSITIONING_BETWEEN_SPACER_VERT_ZOOM_ADJ);

        const float CELL_WIDTH_ZOOM_ADJ_WITH_SPACER(CELL_WIDTH_ZOOM_ADJ +
            POSITIONING_BETWEEN_SPACER_HORIZ_ZOOM_ADJ);

        battlefieldWidth_  = std::abs(static_cast<float>(lowestBlockingPos)  *
            CELL_WIDTH_ZOOM_ADJ_WITH_SPACER) + std::abs(static_cast<float>(highestBlockingPos) *
                CELL_WIDTH_ZOOM_ADJ_WITH_SPACER) + (POSITIONING_MARGIN_HORIZ_ZOOM_ADJ * 2.0f);

        battlefieldHeight_ = (static_cast<float>(shoulderToShoulderMax) *
            CELL_HEIGHT_ZOOM_ADJ_WITH_SPACER) + (POSITIONING_MARGIN_VERT_ZOOM_ADJ * 2.0f);

        //set battlefield positions
        std::map<int, std::size_t> shoulderToShoulderBlockingMap;
        for (auto const & NEXT_VERTEX : VERT_VEC)
        {
            const int NEXT_BLOCKING_POS(NEXT_VERTEX.second->GetBlockingPos());

            //count the number of verts are shoulder-to-shoulder (vertical on screen) with this one
            const std::size_t SHOULDER_TO_SHOULDER_COUNT(
                combatTree_.VertexCountByBlockingPos(NEXT_BLOCKING_POS));

            //keep track of which shoulder-to-shoulder this vert/NEXT_BLOCKING_POS is
            const std::size_t SHOULDER_TO_SHOULDER_POS(
                shoulderToShoulderBlockingMap[NEXT_BLOCKING_POS]++);

            const float POS_LEFT((battlefieldWidth_ * 0.5f) + (
                static_cast<float>(NEXT_BLOCKING_POS) * CELL_WIDTH_ZOOM_ADJ_WITH_SPACER));

            const float SHOULDER_TO_SHOULDER_TOTAL_HEIGHT_HALF(
                (static_cast<float>(SHOULDER_TO_SHOULDER_COUNT) * 0.5f) *
                    CELL_HEIGHT_ZOOM_ADJ_WITH_SPACER);

            const float SHOULDER_TO_SHOULDER_POS_HEIGHT(
                static_cast<float>(SHOULDER_TO_SHOULDER_POS) * CELL_HEIGHT_ZOOM_ADJ_WITH_SPACER);

            const float POS_TOP(((battlefieldHeight_ * 0.5f) -
                SHOULDER_TO_SHOULDER_TOTAL_HEIGHT_HALF) + SHOULDER_TO_SHOULDER_POS_HEIGHT);

            const sf::FloatRect RECT(POS_LEFT, POS_TOP, CELL_WIDTH_ZOOM_ADJ, CELL_HEIGHT_ZOOM_ADJ);

            if (WILL_DELAY)
            {
                nodePosTrackerMap_[NEXT_VERTEX.second.get()] =
                    NodePosTracker(NEXT_VERTEX.second.get(), POS_LEFT, POS_TOP);
            }
            else
            {
                NEXT_VERTEX.second->SetRegion(RECT);
                NEXT_VERTEX.second->SetCharacterSize(nameCharSizeCurr_);
            }
        }
    }


    bool CombatDisplay::RemoveCombatNode(const CombatNodeSPtr_t & COMBAT_NODE_SPTR)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((COMBAT_NODE_SPTR.get() != nullptr),
            "game::combat::CombatDisplay::RemoveCombatNode(nullptr) was given a "
            << "COMBAT_NODE_SPTR that was null.");

        auto const REMOVE_RESULT{ EntityRemove(combatNodeToGuiEntityMap_[COMBAT_NODE_SPTR]) };
        combatNodeToGuiEntityMap_.erase(COMBAT_NODE_SPTR);
        return REMOVE_RESULT;
    }


    void CombatDisplay::MoveBattlefieldVert(const float AMOUNT, const bool WILL_MOVE_BACKGROUND)
    {
        //keep track of all original values in case undo is required
        auto const ORIG_OFFSCREEN_SPRITE_RECT(offScreenSprite_.getTextureRect());

        if (WILL_MOVE_BACKGROUND)
        {
            offScreenPosY_ += AMOUNT;
            offScreenSprite_.setTextureRect(sfml_util::ConvertRect<float, int>(sf::FloatRect(
                offScreenPosX_,
                offScreenPosY_,
                offScreenSprite_.getLocalBounds().width,
                offScreenSprite_.getLocalBounds().height)));
        }

        //move the creature nodes
        CombatNodePVec_t combatNodePVec;
        combatTree_.GetCombatNodes(combatNodePVec);
        for (auto const nextCombatNodePtrC : combatNodePVec)
        {
            nextCombatNodePtrC->MoveEntityPos(0.0f, AMOUNT * -1.0f);
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
            //if no creature/combat nodes are drawn to the screen anymore, then move back
            if (WILL_MOVE_BACKGROUND)
            {
                offScreenPosY_ = prevScrollPosVert_;
            }

            offScreenSprite_.setTextureRect(ORIG_OFFSCREEN_SPRITE_RECT);

            for (auto const nextCombatNodePtrC : combatNodePVec)
            {
                nextCombatNodePtrC->MoveEntityPos(0.0f, AMOUNT);
            }

            UpdateWhichNodesWillDraw();
        }
    }


    void CombatDisplay::MoveBattlefieldHoriz(const float AMOUNT, const bool WILL_MOVE_BACKGROUND)
    {
        //keep track of all original values in case undo is required
        auto const ORIG_OFFSCREEN_SPRITE_RECT(offScreenSprite_.getTextureRect());

        if (WILL_MOVE_BACKGROUND)
        {
            offScreenPosX_ += AMOUNT;
            offScreenSprite_.setTextureRect(sfml_util::ConvertRect<float, int>(sf::FloatRect(
                offScreenPosX_,
                offScreenPosY_,
                offScreenSprite_.getLocalBounds().width,
                offScreenSprite_.getLocalBounds().height)));
        }

        //move the creature nodes
        CombatNodePVec_t combatNodePVec;
        combatTree_.GetCombatNodes(combatNodePVec);
        for (auto const nextCombatNodePtrC : combatNodePVec)
        {
            nextCombatNodePtrC->MoveEntityPos(AMOUNT * -1.0f, 0.0f);
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
            //if no creature/combat nodes are drawn to the screen anymore, then move back
            if (WILL_MOVE_BACKGROUND)
            {
                offScreenPosX_ = prevScrollPosHoriz_;
            }

            offScreenSprite_.setTextureRect(ORIG_OFFSCREEN_SPRITE_RECT);

            for (auto const nextCombatNodePtrC : combatNodePVec)
            {
                nextCombatNodePtrC->MoveEntityPos(AMOUNT, 0.0f);
            }

            UpdateWhichNodesWillDraw();
        }
    }


    const creature::CreaturePVec_t CombatDisplay::FindClosestLivingByType(
        creature::CreatureCPtrC_t CREATURE_CPTRC, const bool WILL_FIND_PLAYERS) const
    {
        M_ASSERT_OR_LOGANDTHROW_SS((CREATURE_CPTRC != nullptr),
            "game::combat::CombatDisplay::FindClosestLivingByType(nullptr, will_find_players="
            << std::boolalpha << WILL_FIND_PLAYERS
            << ") was given a CREATURE_CPTRC that was null.");

        return FindClosestLiving(CREATURE_CPTRC, creature::Algorithms::PlayersByType(
            WILL_FIND_PLAYERS, true));
    }


    const creature::CreaturePVec_t CombatDisplay::FindClosestLiving(
        creature::CreatureCPtrC_t        CREATURE_CPTRC,
        const creature::CreaturePVec_t & AMONG_PVEC) const
    {
        M_ASSERT_OR_LOGANDTHROW_SS((CREATURE_CPTRC != nullptr),
            "game::combat::CombatDisplay::FindClosestLiving(nullptr, among_size="
            << AMONG_PVEC.size() << ") was given a null CREATURE_CPTRC.");

        M_ASSERT_OR_LOGANDTHROW_SS((AMONG_PVEC.empty() == false),
            "game::combat::CombatDisplay::FindClosestLiving(" << CREATURE_CPTRC->Name()
            << ", among_size=" << AMONG_PVEC.size() << ") was given an empty AMONG_PVEC.");

        if (AMONG_PVEC.size() == 1)
        {
            return AMONG_PVEC;
        }
        else
        {
            auto distanceSortedPVec{ creature::Algorithms::FindByAlive(AMONG_PVEC) };
            const combat::CombatTree * const COMBAT_TREE_CPTRC{ & combatTree_ };

            std::sort(distanceSortedPVec.begin(),
                      distanceSortedPVec.end(),
                      [COMBAT_TREE_CPTRC, CREATURE_CPTRC]
                      (const creature::CreaturePtr_t A_PTR, const creature::CreaturePtr_t B_PTR)
                        {
                            return (COMBAT_TREE_CPTRC->GetBlockingDistanceBetween(
                                A_PTR, CREATURE_CPTRC)) < std::abs(
                                    COMBAT_TREE_CPTRC->GetBlockingDistanceBetween(
                                        B_PTR, CREATURE_CPTRC));
                        });

            auto const MIN_BLOCKING_DISTANCE{ std::abs(combatTree_.GetBlockingDistanceBetween(
                * distanceSortedPVec.begin(), CREATURE_CPTRC)) };

            creature::CreaturePVec_t closestCreaturesPVec;

            for (auto const NEXT_CREATURE_PTR : distanceSortedPVec)
            {
                if (std::abs(combatTree_.GetBlockingDistanceBetween(
                    NEXT_CREATURE_PTR, CREATURE_CPTRC)) == MIN_BLOCKING_DISTANCE)
                {
                    closestCreaturesPVec.push_back(NEXT_CREATURE_PTR);
                }
            }

            M_ASSERT_OR_LOGANDTHROW_SS((closestCreaturesPVec.empty() == false),
                "game::combat::CombatDisplay::FindClosestLiving(" << CREATURE_CPTRC->Name()
                << ", among_size=" << AMONG_PVEC.size() << ") reached an invalid state where there"
                << "were no closest creatures when there should be at least one of the "
                << AMONG_PVEC.size() << " total possible.");

            return closestCreaturesPVec;
        }
    }


    std::size_t CombatDisplay::FindCreaturesThatCanBeAttackedOfType(
        creature::CreaturePVec_t &     pVec_OutParam,
        const creature::CreaturePtrC_t CREATURE_CPTRC,
        const bool                     WILL_FIND_PLAYERS) const
    {
        M_ASSERT_OR_LOGANDTHROW_SS((CREATURE_CPTRC != nullptr),
            "game::combat::CombatDisplay::FindCreaturesThatCanBeAttackedOfType"
            << "(pVec_OutParam, nullptr, will_find_players=" << std::boolalpha
            << WILL_FIND_PLAYERS << ") was given a CREATURE_CPTRC that was null.");

        if (CREATURE_CPTRC->HasWeaponsHeld() == false)
        {
            return 0;
        }

        auto const IS_FLYING{ Encounter::Instance()->
            GetTurnInfoCopy(CREATURE_CPTRC).GetIsFlying() };

        if (CREATURE_CPTRC->IsHoldingProjectileWeapon() ||
            (IS_FLYING))
        {
            return creature::Algorithms::PlayersByType(pVec_OutParam, WILL_FIND_PLAYERS, true);
        }
        else
        {
            auto const ALLAROUND_CREATURES_PVEC{ [&]()
                {
                    creature::CreaturePVec_t creaturesAllAround;
                    FindCreaturesAllRoundOfType(creaturesAllAround,
                                                CREATURE_CPTRC,
                                                WILL_FIND_PLAYERS,
                                                true);
                    return creaturesAllAround;
                }() };
            
            auto const NOTFLYING_ALLAROUND_CREATURES_PVEC{
                creature::Algorithms::FindByFlying(ALLAROUND_CREATURES_PVEC, false) };

            auto const REACHABLE_CREATURES_PVEC{ ((IS_FLYING) ?
                ALLAROUND_CREATURES_PVEC :
                NOTFLYING_ALLAROUND_CREATURES_PVEC) };

            if (REACHABLE_CREATURES_PVEC.empty() == false)
            {
                std::copy(REACHABLE_CREATURES_PVEC.begin(),
                          REACHABLE_CREATURES_PVEC.end(),
                          back_inserter(pVec_OutParam));
            }

            return REACHABLE_CREATURES_PVEC.size();
        }
    }


    std::size_t CombatDisplay::FindCreaturesAllRoundOfType(
        creature::CreaturePVec_t & pVec_OutParam,
        creature::CreatureCPtrC_t  CREATURE_CPTRC,
        const bool                 WILL_FIND_PLAYERS,
        const bool                 LIVING_ONLY) const
    {
        M_ASSERT_OR_LOGANDTHROW_SS((CREATURE_CPTRC != nullptr),
            "game::combat::CombatDisplay::FindCreaturesAllRoundOfType("
            << "pVec_OutParam, nullptr, will_find_players=" << std::boolalpha << WILL_FIND_PLAYERS
            << ") was given a CREATURE_CPTRC that was null.");

        auto const COMBAT_NODE_PTR(combatTree_.GetNode(CREATURE_CPTRC));
        M_ASSERT_OR_LOGANDTHROW_SS((COMBAT_NODE_PTR != nullptr),
            "game::combat::CombatDisplay::FindCreaturesThatCanBeAttackedOfType(creature_name="
            << CREATURE_CPTRC->Name() << ", will_find_players=" << std::boolalpha
            << WILL_FIND_PLAYERS << ") was unable to find that creature node in the combatTree_.");

        CombatNodePVec_t nodePVec;
        combatTree_.GetNodesAllAroundBlockingPos(nodePVec, COMBAT_NODE_PTR->GetBlockingPos());

        std::size_t count(0);
        for (auto const NEXT_COMBATNODE_PTR : nodePVec)
        {
            if ((NEXT_COMBATNODE_PTR != nullptr) &&
                (NEXT_COMBATNODE_PTR->Creature() != CREATURE_CPTRC) &&
                (NEXT_COMBATNODE_PTR->Creature()->IsPlayerCharacter() == WILL_FIND_PLAYERS))
            {
                if ((LIVING_ONLY == false) ||
                    (LIVING_ONLY && (NEXT_COMBATNODE_PTR->Creature()->IsDead() == false)))
                {
                    ++count;
                    pVec_OutParam.push_back(NEXT_COMBATNODE_PTR->Creature());
                }
            }
        }

        return count;
    }


    std::size_t CombatDisplay::FindCreaturesInSameBlockingPosOfType(
        creature::CreaturePVec_t & pVec_OutParam,
        creature::CreatureCPtrC_t  CREATURE_CPTRC,
        const bool                 WILL_FIND_PLAYERS) const
    {
        M_ASSERT_OR_LOGANDTHROW_SS((CREATURE_CPTRC != nullptr),
            "game::combat::CombatDisplay::FindCreaturesInSameBlockingPosOfType("
            << "pVec_OutParam, nullptr, will_find_players=" << std::boolalpha << WILL_FIND_PLAYERS
            << ") was given a CREATURE_CPTRC that was null.");

        const CombatNodePtr_t COMBAT_NODE_PTR(combatTree_.GetNode(CREATURE_CPTRC));

        M_ASSERT_OR_LOGANDTHROW_SS((COMBAT_NODE_PTR != nullptr),
            "game::combat::CombatDisplay::FindCreaturesInSameBlockingPosOfType(creature_name="
            << CREATURE_CPTRC->Name() << ", will_find_players=" << std::boolalpha
            << WILL_FIND_PLAYERS << ") was unable to find that creature node in the combatTree_.");

        combat::CombatTree::IdVec_t idVec;
        combatTree_.GetNodeIDsAtBlockingPos(idVec, COMBAT_NODE_PTR->GetBlockingPos());

        std::size_t count(0);
        for (auto const NEXT_NODE_ID : idVec)
        {
            const CombatNodePtr_t NEXT_COMBATNODE_PTR(combatTree_.GetNode(NEXT_NODE_ID));

            if ((NEXT_COMBATNODE_PTR != nullptr) &&
                (NEXT_COMBATNODE_PTR->Creature() != CREATURE_CPTRC) &&
                (NEXT_COMBATNODE_PTR->Creature()->IsPlayerCharacter() == WILL_FIND_PLAYERS))
            {
                ++count;
                pVec_OutParam.push_back(NEXT_COMBATNODE_PTR->Creature());
            }
        }

        return count;
    }


    std::size_t CombatDisplay::FindCreaturesAtBlockingPosOfType(
        creature::CreaturePVec_t & pVec_OutParam,
        const int                  BLOCKING_POS,
        const bool                 WILL_FIND_PLAYERS) const
    {
        CombatNodePVec_t nodesPVec;
        combatTree_.GetNodesAtBlockingPos(nodesPVec, BLOCKING_POS);

        std::size_t count(0);
        for (auto const NEXT_COMBATNODE_PTR : nodesPVec)
        {
            if ((NEXT_COMBATNODE_PTR != nullptr) &&
                (NEXT_COMBATNODE_PTR->Creature()->IsPlayerCharacter() == WILL_FIND_PLAYERS))
            {
                ++count;
                pVec_OutParam.push_back(NEXT_COMBATNODE_PTR->Creature());
            }
        }

        return count;
    }


    int CombatDisplay::FindBlockingPos(const creature::CreaturePtrC_t CREATURE_CPTRC) const
    {
        M_ASSERT_OR_LOGANDTHROW_SS((CREATURE_CPTRC != nullptr),
            "game::combat::CombatDisplay::FindBlockingPos(nullptr) was given a"
            << "CREATURE_CPTRC that was null.");

        return combatTree_.GetNode(CREATURE_CPTRC)->GetBlockingPos();
    }


    const creature::CreaturePVec_t CombatDisplay::FindClosestAmongOfType(
        const creature::CreaturePtrC_t   CREATURE_OF_ORIGIN_CPTRC,
        const creature::CreaturePVec_t & CREATURES_TO_FIND_AMONG_PVEC,
        const bool                       WILL_FIND_PLAYERS) const
    {
        M_ASSERT_OR_LOGANDTHROW_SS((CREATURE_OF_ORIGIN_CPTRC != nullptr),
            "game::comabt::FindClosestAmongOfType() was given a CREATURE_OF_ORIGIN_CPTRC"
            << "that was nullptr.");

        M_ASSERT_OR_LOGANDTHROW_SS((CREATURES_TO_FIND_AMONG_PVEC.empty() == false),
            "game::combat::FindClosestAmongOfType(creature_of_origin_name=\""
            << CREATURE_OF_ORIGIN_CPTRC->Name()
            << "\") was given a CREATURES_TO_FIND_AMONG_PVEC that was empty.");

        auto const BLOCKING_POS_ORIGIN{ FindBlockingPos(CREATURE_OF_ORIGIN_CPTRC) };

        auto closestBlockingDistanceABS{ combatTree_.GetBlockingDistanceMax() + 1 };
        for (auto const NEXT_CREATURE_PTR : CREATURES_TO_FIND_AMONG_PVEC)
        {
            auto const NEXT_BLOCKING_DISTANCE_ABS{ std::abs(FindBlockingPos(NEXT_CREATURE_PTR) -
                BLOCKING_POS_ORIGIN) };

            if ((NEXT_CREATURE_PTR->IsPlayerCharacter() == WILL_FIND_PLAYERS) &&
                (NEXT_BLOCKING_DISTANCE_ABS < closestBlockingDistanceABS))
            {
                closestBlockingDistanceABS = NEXT_BLOCKING_DISTANCE_ABS;
            }
        }

        creature::CreaturePVec_t closestCreaturesPVec;

        for (auto const NEXT_CREATURE_PTR : CREATURES_TO_FIND_AMONG_PVEC)
        {
            auto const NEXT_BLOCKING_DISTANCE_ABS{ std::abs(FindBlockingPos(NEXT_CREATURE_PTR) -
                BLOCKING_POS_ORIGIN) };

            if ((NEXT_CREATURE_PTR->IsPlayerCharacter() == WILL_FIND_PLAYERS) &&
                (NEXT_BLOCKING_DISTANCE_ABS == closestBlockingDistanceABS))
            {
                closestCreaturesPVec.push_back(NEXT_CREATURE_PTR);
            }
        }

        M_ASSERT_OR_LOGANDTHROW_SS((closestCreaturesPVec.empty() == false),
            "game::combat::FindClosestAmongOfType(creature_of_origin_name=\""
            << CREATURE_OF_ORIGIN_CPTRC->Name() << "\", creatures_to_find_among_pvec_size="
            << CREATURES_TO_FIND_AMONG_PVEC.size() << ", will_find_players=" << std::boolalpha
            << WILL_FIND_PLAYERS << ") was unable to find a closest creature among those given.");

        return closestCreaturesPVec;
    }


    bool CombatDisplay::IsCreatureAPossibleFightTarget(
        const creature::CreaturePtrC_t CREATURE_FIGHTING_CPTRC,
        const creature::CreaturePtrC_t CREATURE_TARGETED_CPTRC) const
    {
        M_ASSERT_OR_LOGANDTHROW_SS((CREATURE_FIGHTING_CPTRC != nullptr),
            "game::combat::CombatDisplay::IsCreatureAPossibleFightTarget() was given a "
            << "CREATURE_FIGHTING_CPTRC that was null.");

        M_ASSERT_OR_LOGANDTHROW_SS((CREATURE_TARGETED_CPTRC != nullptr),
            "game::combat::CombatDisplay::IsCreatureAPossibleFightTarget() was given a "
            << "CREATURE_TARGETED_CPTRC that was null.");

        creature::CreaturePVec_t creaturesThatCanBeAttackedPVec;

        FindCreaturesThatCanBeAttackedOfType(
            creaturesThatCanBeAttackedPVec, CREATURE_FIGHTING_CPTRC, false);

        for (auto const NEXT_CREATURE_PTR : creaturesThatCanBeAttackedPVec)
        {
            if (NEXT_CREATURE_PTR == CREATURE_TARGETED_CPTRC)
            {
                return true;
            }
        }

        return false;
    }


    CombatNodePtr_t CombatDisplay::GetCombatNodeForCreature(
        creature::CreatureCPtrC_t CREATURE_CPTRC) const
    {
        M_ASSERT_OR_LOGANDTHROW_SS((CREATURE_CPTRC != nullptr),
            "game::combat::CombatDisplay::GetCombatNodeForCreature() was given a "
            << "CREATURE_CPTRC that was null.");

        return combatTree_.GetNode(combatTree_.GetNodeId(CREATURE_CPTRC));
    }


    CombatNodePVec_t CombatDisplay::GetCombatNodesForCreatures(
        const creature::CreaturePVec_t & CREATURES_PVEC) const
    {
        M_ASSERT_OR_LOGANDTHROW_SS((CREATURES_PVEC.empty() == false),
            "game::combat::GetCombatNodesForCreatures() was given a CREATURES_PVEC "
            << "that was empty.");

        CombatNodePVec_t combatNodesPVec;
        combatNodesPVec.reserve(combatTree_.VertexCount());
        combatTree_.GetCombatNodes(combatNodesPVec);

        CombatNodePVec_t creatureCombatNodesPVec;

        std::size_t creaturePtrCounter{ 0 };
        for (auto const NEXT_CREATURE_PTR : CREATURES_PVEC)
        {
            M_ASSERT_OR_LOGANDTHROW_SS((NEXT_CREATURE_PTR != nullptr),
                "game::combat::CombatDisplay::GetCombatNodesForCreatures("
                << "CREATURES_PVEC.size()=" << CREATURES_PVEC.size() << ") vector at index="
                << creaturePtrCounter << " was null.");

            ++creaturePtrCounter;

            for (auto const nextCombatNodePtrC : combatNodesPVec)
            {
                if (nextCombatNodePtrC->Creature() == NEXT_CREATURE_PTR)
                {
                    creatureCombatNodesPVec.push_back(nextCombatNodePtrC);
                }
            }
        }

        M_ASSERT_OR_LOGANDTHROW_SS((creatureCombatNodesPVec.size() == CREATURES_PVEC.size()),
            "game::combat::CombatDisplay::GetCombatNodesForCreatures(CREATURES_PVEC.size()="
            << CREATURES_PVEC.size()
            << ") was not able to find CombatNodePtr_ts for every CreaturePtr_t.");

        return creatureCombatNodesPVec;
    }


    const std::string CombatDisplay::CanAdvanceOrRetreat(
        const creature::CreaturePtr_t CREATURE_CPTRC, const bool TRYING_TO_ADVANCE) const
    {
        M_ASSERT_OR_LOGANDTHROW_SS((CREATURE_CPTRC != nullptr),
            "game::combat::CombatDisplay::CanAdvanceOrRetreat(nullptr, trying_to_advance="
            << std::boolalpha << TRYING_TO_ADVANCE
            << ") was given a CREATURE_CPTRC that was null.");

        const int BLOCKING_POS(combatTree_.GetNode(CREATURE_CPTRC)->GetBlockingPos());

        const bool ATTEMPTING_BLOCKING_POS_INCREMENT(
            CREATURE_CPTRC->IsPlayerCharacter() == TRYING_TO_ADVANCE);

        const std::string ADVANCE_OR_RETREAT_STR((TRYING_TO_ADVANCE) ? "advance" : "retreat");

        //check if attempting to move past battlefield boundaries/edges
        if (((ATTEMPTING_BLOCKING_POS_INCREMENT) && (BLOCKING_POS >= blockingPosMax_)) ||
            ((ATTEMPTING_BLOCKING_POS_INCREMENT == false) && (BLOCKING_POS <= blockingPosMin_)))
        {
            return "Cannot " + ADVANCE_OR_RETREAT_STR + " because already at limit.";
        }

        //flying creatures can always advance or retreat except if at the edge of the battlefield
        if (Encounter::Instance()->GetTurnInfoCopy(CREATURE_CPTRC).GetIsFlying())
        {
            return "";
        }

        const int BLOCKING_POS_NEW((ATTEMPTING_BLOCKING_POS_INCREMENT) ?
            (BLOCKING_POS  + 1) : (BLOCKING_POS - 1));

        creature::CreaturePVec_t ignoreMePVec;
        const std::size_t OBSTACLE_CREATURE_COUNT_AT_NEW_POS{
            GetObstacleCreaturesAtBlockingPos(ignoreMePVec, CREATURE_CPTRC, BLOCKING_POS_NEW) };

        //check if attempting to move into a shoulder-to-shoulder line
        //that already has too many creatures
        if (OBSTACLE_CREATURE_COUNT_AT_NEW_POS > SHOULDER_TO_SHOULDER_MAX_)
        {
            std::ostringstream ss;
            ss << "Cannot " << ADVANCE_OR_RETREAT_STR << "because there are too many ("
                << OBSTACLE_CREATURE_COUNT_AT_NEW_POS << ") opposing creatres in the way."
                << "  The limit is " << SHOULDER_TO_SHOULDER_MAX_ << ".";

            return ss.str();
        }

        //Check if attempting to move into a shoulder-to-shoulder line that
        //has an opposing creature blocking.
        CombatNodePVec_t combatNodesPVec;
        combatTree_.GetCombatNodes(combatNodesPVec);
        for (auto const & NEXT_COMBATNODE_PTR : combatNodesPVec)
        {
            auto const NEXT_CREATURE_PTR{ NEXT_COMBATNODE_PTR->Creature() };

            if ((NEXT_COMBATNODE_PTR->GetBlockingPos() == BLOCKING_POS_NEW) &&
                (NEXT_CREATURE_PTR->IsPlayerCharacter() != CREATURE_CPTRC->IsPlayerCharacter()) &&
                (NEXT_CREATURE_PTR->HasConditionNotAThreatPerm(true) == false) &&
                (Encounter::Instance()->GetTurnInfoCopy(NEXT_CREATURE_PTR).GetTurnActionInfo().
                    Action() == TurnAction::Block))
            {
                return "Cannot " + ADVANCE_OR_RETREAT_STR +
                    " because there is a creature blocking.";
            }
        }

        return "";
    }


    creature::CreaturePtr_t CombatDisplay::GetCreatureAtPos(const sf::Vector2f & POS_V)
    {
        CombatNodePtr_t combatNodePtr(combatTree_.GetNode(POS_V.x, POS_V.y));

        if (combatNodePtr != nullptr)
        {
            return combatNodePtr->Creature();
        }
        else
        {
            return nullptr;
        }
    }


    void CombatDisplay::MoveCreatureBlockingPosition(creature::CreatureCPtrC_t CREATURE_CPTRC,
        const bool WILL_MOVE_FORWARD)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((CREATURE_CPTRC != nullptr),
            "game::combat::CombatDisplay::MoveCreatureBlockingPosition(nullptr, will_move_forward="
            << std::boolalpha << WILL_MOVE_FORWARD
            << ") was given a CREATURE_CPTRC that was null.");

        auto const CREATURE_NODE_ID{ combatTree_.GetNodeId(CREATURE_CPTRC) };
        auto combatNodeSPtr{ combatTree_.GetNodeSPtr(CREATURE_NODE_ID) };
        auto blockingPos{ combatNodeSPtr->GetBlockingPos() };

        //Note: For player-characters (facing right), 'Forward' is moving to
        //      the right on the battlefield (increasing blocking pos), and
        //      'Backward' is moving to the left (decreasing blocking pos).
        //      Non-player-characters have the inverse concept of forward and
        //      backward.
        if (CREATURE_CPTRC->IsPlayerCharacter() == WILL_MOVE_FORWARD)
        {
            ++blockingPos;
        }
        else
        {
            --blockingPos;
        }

        combatNodeSPtr->SetBlockingPos(blockingPos);

        //move the vertecies and edges in the combatTree_
        combatTree_.RemoveVertex(CREATURE_NODE_ID, true);
        combatTree_.AddVertex(CREATURE_NODE_ID, combatNodeSPtr);//re-use the same node id and ptr
        combatTree_.ConnectAllAtPosition(blockingPos, EdgeType::ShoulderToShoulder);

        //re-position CombatNodes/Creatures on the battlefield in the slow animated way
        PositionCombatTreeCells(true);
    }


    void CombatDisplay::CancelSummaryViewAndStartTransitionBack()
    {
        summaryViewUPtr_->StartTransitionBack();
    }


    void CombatDisplay::HandleFlyingChange(const creature::CreaturePtrC_t CREATURE_CPTRC,
        const bool IS_FLYING)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((CREATURE_CPTRC != nullptr),
            "game::combat::CombatDisplay::HandleFlyingChange(nullptr, is_flying="
            << std::boolalpha << IS_FLYING << ") was given a CREATURE_CPTRC that was null.");

        combatTree_.GetNode(CREATURE_CPTRC)->IsFlying(IS_FLYING);
        Encounter::Instance()->SetIsFlying(CREATURE_CPTRC, IS_FLYING);
    }


    void CombatDisplay::HandleEndOfTurnTasks()
    {
        UpdateHealthTasks();

        //stop all creature image shaking
        M_ASSERT_OR_LOGANDTHROW_SS((combatAnimationPtr_ != nullptr),
            "game::combat::CombatDisplay::HandleEndOfTurnTasks() found combatAnimationPtr_ "
            << "to be null.");

        combatAnimationPtr_->ShakeAnimStop(nullptr);
    }


    void CombatDisplay::UpdateHealthTasks()
    {
        CombatNodePVec_t combatNodesPVec;
        combatNodesPVec.reserve(combatTree_.VertexCount());
        combatTree_.GetCombatNodes(combatNodesPVec);

        for (auto const nextCombatNodePtrC : combatNodesPVec)
        {
            nextCombatNodePtrC->HealthChangeTasks();
        }
    }


    bool CombatDisplay::IsCreatureVisible(creature::CreatureCPtrC_t CREATURE_CPTRC) const
    {
        M_ASSERT_OR_LOGANDTHROW_SS((CREATURE_CPTRC != nullptr),
            "game::combat::CombatDisplay::IsCreatureVisible(nullptr) was given a CREATURE_CPTR"
            << " that was null.");

        if (CREATURE_CPTRC == nullptr)
        {
            return false;
        }
        else
        {
            return combatTree_.GetNode(CREATURE_CPTRC)->GetEntityWillDraw();
        }
    }


    bool CombatDisplay::AreAllCreaturesVisible(
        const creature::CreaturePVec_t & CREATURES_TO_CHECK_PVEC)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((CREATURES_TO_CHECK_PVEC.empty() == false),
            "game::combat::CombatDisplay::AreAllCreaturesVisible() was given a "
            << "CREATURES_TO_CHECK_PVEC that was empty.");

        auto const COMBATNODES_PVEC{ GetCombatNodesForCreatures(CREATURES_TO_CHECK_PVEC) };

        for (auto const nextCombatNodeCPtr : COMBATNODES_PVEC)
        {
            if (nextCombatNodeCPtr->GetEntityWillDraw() == false)
            {
                return false;
            }
        }

        return true;
    }


    bool CombatDisplay::IsZoomOutRequired(const creature::CreaturePVec_t & CREATURES_PVEC) const
    {
        M_ASSERT_OR_LOGANDTHROW_SS((CREATURES_PVEC.empty() == false),
            "game::combat::CombatDisplay::IsZoomOutRequired() was given a "
            << "CREATURES_PVEC that was empty.");

        auto horizPosDiffMax{ 0.0f };
        auto vertPosDiffMax{ 0.0f };

        auto combatNodesPVec{ GetCombatNodesForCreatures(CREATURES_PVEC) };
        for (auto const outerCombatNodeCPtr : combatNodesPVec)
        {
            for (auto const innerCombatNodeCPtr : combatNodesPVec)
            {
                if (outerCombatNodeCPtr != innerCombatNodeCPtr)
                {
                    auto HORIZ_DIFF{ std::abs(outerCombatNodeCPtr->GetEntityPos().x -
                        innerCombatNodeCPtr->GetEntityPos().x) };

                    if (HORIZ_DIFF > horizPosDiffMax)
                    {
                        horizPosDiffMax = HORIZ_DIFF;
                    }

                    auto VERT_DIFF{ std::abs(outerCombatNodeCPtr->GetEntityPos().y -
                        innerCombatNodeCPtr->GetEntityPos().y) };

                    if (VERT_DIFF > vertPosDiffMax)
                    {
                        vertPosDiffMax = VERT_DIFF;
                    }
                }
            }
        }

        auto const TOLERANCE{ sfml_util::MapByRes(300.0f, 600.0f) };
        return ((horizPosDiffMax > (sfml_util::Display::Instance()->GetWinWidth()  - TOLERANCE)) ||
                (vertPosDiffMax  > (sfml_util::Display::Instance()->GetWinHeight() - TOLERANCE)));
    }


    const sf::Vector2f CombatDisplay::FindCenterOfCreatures(
        const creature::CreaturePVec_t & CREATURES_TO_CENTER_ON_PVEC) const
    {
        M_ASSERT_OR_LOGANDTHROW_SS((CREATURES_TO_CENTER_ON_PVEC.empty() == false),
            "game::combat::CombatDisplay::FindCenterOfCreatures() was given a "
            << "CREATURES_TO_CENTER_ON_PVEC that was empty.");

        std::vector<float> horizPosVec;
        std::vector<float> vertPosVec;

        auto combatNodesPVec{ GetCombatNodesForCreatures(CREATURES_TO_CENTER_ON_PVEC) };
        for (auto const nextComabtNodeCPtr : combatNodesPVec)
        {
            horizPosVec.push_back(nextComabtNodeCPtr->GetEntityPos().x +
                (nextComabtNodeCPtr->GetEntityRegion().width * 0.5f));

            vertPosVec.push_back(nextComabtNodeCPtr->GetEntityPos().y +
                (nextComabtNodeCPtr->GetEntityRegion().height * 0.5f));
        }

        auto const HORIZ_AVG_POS{ std::accumulate(horizPosVec.begin(), horizPosVec.end(), 0.0f) /
            static_cast<float>(horizPosVec.size()) };

        auto const VERT_AVG_POS{ std::accumulate(vertPosVec.begin(), vertPosVec.end(), 0.0f) /
            static_cast<float>(vertPosVec.size()) };

        return sf::Vector2f(HORIZ_AVG_POS, VERT_AVG_POS);
    }


    void CombatDisplay::SetCreatureHighlight(creature::CreatureCPtrC_t CREATURE_CPTRC,
        const bool WILL_HIGHLIGHT)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((CREATURE_CPTRC != nullptr),
            "game::combat::CombatDisplay::SetCreatureHighlight(nullptr) was given a "
            << "CREATURE_CPTRC that was null.");

        GetCombatNodeForCreature(CREATURE_CPTRC)->SetHighlight(WILL_HIGHLIGHT, false);
    }


    void CombatDisplay::InitialPlayerPartyCombatTreeSetup()
    {
        int position(-1);

        //apply the player's preferred blocking pattern if one is saved
        if (blockingMap_.empty())
        {
            //place Knights/Arhers/Beasts/Beastmasters first in the blocking order
            SetBlockingPosOfType(true, creature::role::Knight, position);
            SetBlockingPosOfType(true, creature::role::Archer, position);
            SetBlockingPosOfType(true, creature::role::Wolfen, position);
            SetBlockingPosOfType(true, creature::role::Sylavin, position);
            SetBlockingPosOfType(true, creature::role::Firebrand, position);
            SetBlockingPosOfType(true, creature::role::Beastmaster, position);
            combatTree_.ConnectAllAtPosition(position, combat::EdgeType::ShoulderToShoulder);

            //place thieves/bards are next in the blocking order
            position -= 1;
            SetBlockingPosOfType(true, creature::role::Thief, position);
            SetBlockingPosOfType(true, creature::role::Bard, position);
            combatTree_.ConnectAllAtPosition(position, combat::EdgeType::ShoulderToShoulder);

            //place sorcerers and clerics next in the blocking order
            position -= 1;
            SetBlockingPosOfType(true, creature::role::Sorcerer, position);
            SetBlockingPosOfType(true, creature::role::Cleric, position);
            combatTree_.ConnectAllAtPosition(position, combat::EdgeType::ShoulderToShoulder);
        }
        else
        {
            //place player characters in the blocking order saved in blockingMap_
            auto const CHAR_PVEC( game::Game::Instance()->State().Party().Characters() );
            for (auto const NEXT_CHARACTER_PTR : CHAR_PVEC)
            {
                const creature::UniqueTraits_t NEXT_CHARACTER_TRAITS(
                    NEXT_CHARACTER_PTR->UniqueTraits());

                const int NEXT_POSITION(blockingMap_[NEXT_CHARACTER_TRAITS]);
                const CombatTree::Id_t NEXT_NODE_ID(combatTree_.GetNodeId(NEXT_CHARACTER_PTR));
                combatTree_.GetNode(NEXT_NODE_ID)->SetBlockingPos(NEXT_POSITION);

                combatTree_.ConnectAllAtPosition(NEXT_POSITION,
                    combat::EdgeType::ShoulderToShoulder);
            }
        }

        //whatever blocking order pattern, leave the disabled characters farthest behind
        const int DISABLED_CREATURES_POSITION(position - 1);
        auto const CHAR_PVEC( game::Game::Instance()->State().Party().Characters() );
        for (auto const NEXT_CHARACTER_PTR : CHAR_PVEC)
        {
            if (NEXT_CHARACTER_PTR->CanTakeAction() == false)
            {
                const CombatTree::Id_t NEXT_NODE_ID(combatTree_.GetNodeId(NEXT_CHARACTER_PTR));
                combatTree_.GetNode(NEXT_NODE_ID)->SetBlockingPos(DISABLED_CREATURES_POSITION);
            }
        }

        combatTree_.ConnectAllAtPosition(DISABLED_CREATURES_POSITION,
            combat::EdgeType::ShoulderToShoulder);
    }


    void CombatDisplay::InitialNonPlayerPartyCombatTreeSetup()
    {
        int position(1);

        //place ignorant roles first in the blocking order
        SetBlockingPosOfType(false, creature::role::Drunk, position);
        SetBlockingPosOfType(false, creature::role::Grunt, position);
        SetBlockingPosOfType(false, creature::role::Mugger, position);
        SetBlockingPosOfType(false, creature::role::Firebrand, position);
        SetBlockingPosOfType(false, creature::role::Sylavin, position);
        SetBlockingPosOfType(false, creature::role::Wolfen, position);
        combatTree_.ConnectAllAtPosition(position, combat::EdgeType::ShoulderToShoulder);

        position += 1;
        SetBlockingPosOfType(false, creature::role::Archer, position);
        SetBlockingPosOfType(false, creature::role::Knight, position);
        SetBlockingPosOfType(false, creature::role::Beastmaster, position);
        combatTree_.ConnectAllAtPosition(position, combat::EdgeType::ShoulderToShoulder);

        position += 1;
        SetBlockingPosOfType(false, creature::role::Bard, position);
        SetBlockingPosOfType(false, creature::role::Thief, position);
        combatTree_.ConnectAllAtPosition(position, combat::EdgeType::ShoulderToShoulder);

        position += 1;
        SetBlockingPosOfType(false, creature::role::Captain, position);
        SetBlockingPosOfType(false, creature::role::Chieftain, position);
        SetBlockingPosOfType(false, creature::role::Sorcerer, position);
        SetBlockingPosOfType(false, creature::role::Cleric, position);
        combatTree_.ConnectAllAtPosition(position, combat::EdgeType::ShoulderToShoulder);

        position += 1;
        SetBlockingPosOfType(false, creature::role::Warlord, position);
        combatTree_.ConnectAllAtPosition(position, combat::EdgeType::ShoulderToShoulder);
    }


    void CombatDisplay::SortCreatureListByDisplayedPosition(
        creature::CreaturePVec_t & creaturesPVec) const
    {
        std::sort(
            creaturesPVec.begin(),
            creaturesPVec.end(),
            [this](const creature::CreaturePtr_t CPTR_A, const creature::CreaturePtr_t CPTR_B)
            {
                auto const COMBAT_NODE_PTR_A{ GetCombatNodeForCreature(CPTR_A) };
                auto const COMBAT_NODE_PTR_B{ GetCombatNodeForCreature(CPTR_B) };

                return std::tie(COMBAT_NODE_PTR_A->GetEntityPos().x,
                                COMBAT_NODE_PTR_A->GetEntityPos().y)
                        <
                       std::tie(COMBAT_NODE_PTR_B->GetEntityPos().x,
                                COMBAT_NODE_PTR_B->GetEntityPos().y);
            });
    }


    void CombatDisplay::HandleDeaths(const combat::CombatNodePVec_t & COMBATNODES_PVEC)
    {
        //remove non-player nodes from combat tree and prepare for sliding animation
        for (auto const NEXT_COMBATNODE_PTR : COMBATNODES_PVEC)
        {
            HandleCombatNodeElimination(NEXT_COMBATNODE_PTR);
        }

        //re-position CombatNodes/Creatures on the battlefield in the slow animated way
        PositionCombatTreeCells(true);
    }


    void CombatDisplay::HandleCombatNodeElimination(const creature::CreaturePtr_t CREATURE_PTR)
    {
        HandleCombatNodeElimination( GetCombatNodeForCreature(CREATURE_PTR) );
    }


    void CombatDisplay::HandleCombatNodeElimination(const combat::CombatNodePtr_t COMBATNODE_PTR)
    {
        auto const NEXT_NODE_ID{ combatTree_.GetNodeId(COMBATNODE_PTR) };
        auto const NEXT_NODE_SPTR{ combatTree_.GetNodeSPtr(NEXT_NODE_ID) };
        combatTree_.RemoveVertex(NEXT_NODE_ID, true);
        RemoveCombatNode(NEXT_NODE_SPTR);
    }


    std::size_t CombatDisplay::GetObstacleCreaturesAtBlockingPos(
        creature::CreaturePVec_t &    pVec_OutParam,
        const creature::CreaturePtr_t CREATURE_ATTEMPTING_PTR,
        const int                     BLOCKING_POS) const
    {
        CombatNodePVec_t combatNodesPVec;
        combatTree_.GetCombatNodes(combatNodesPVec);

        std::size_t count{ 0 };
        for (auto const NEXT_COMBAT_NODE_PTR : combatNodesPVec)
        {
            auto const NEXT_CREATURE_PTR{ NEXT_COMBAT_NODE_PTR->Creature() };
            if ((NEXT_COMBAT_NODE_PTR->GetBlockingPos() == BLOCKING_POS) &&
                (NEXT_CREATURE_PTR->HasConditionNotAThreatPerm(true) == false) &&
                (Encounter::Instance()->GetTurnInfoCopy(
                    NEXT_CREATURE_PTR).GetIsFlying() == false) &&
                (NEXT_CREATURE_PTR->IsPlayerCharacter() ==
                    CREATURE_ATTEMPTING_PTR->IsPlayerCharacter()))
            {
                ++count;
                pVec_OutParam.push_back(NEXT_CREATURE_PTR);
            }
        }

        return count;
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

            if (summaryViewUPtr_->GetTransitionStatus() > 0.98)
            {
                SetIsSummaryViewInProgress(false);

                M_ASSERT_OR_LOGANDTHROW_SS((combatAnimationPtr_ != nullptr),
                    "game::combat::CombatDisplay::UpdateTime() found combatAnimationPtr_ "
                    << "to be null.");

                if (combatAnimationPtr_->ShakeAnimCreatureCPtr() != nullptr)
                {
                    combatAnimationPtr_->ShakeAnimRestart();
                }
            }
        }
    }


    void CombatDisplay::CreatureToneDown(const float TONE_DOWN_VAL)
    {
        CombatNodePVec_t combatNodesPVec;
        combatNodesPVec.reserve(combatTree_.VertexCount());
        combatTree_.GetCombatNodes(combatNodesPVec);

        for (auto const nextCombatNodePtrC : combatNodesPVec)
        {
            if ((summaryViewUPtr_->CombatNodePtr() != nullptr) &&
                (summaryViewUPtr_->CombatNodePtr() != nextCombatNodePtrC))
            {
                nextCombatNodePtrC->SetToneDown(TONE_DOWN_VAL);
            }
        }
    }


    const std::string CombatDisplay::GetNodeTitle(const CombatNodePtr_t COMBAT_NODE_PTR)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((COMBAT_NODE_PTR != nullptr),
            "game::combat::CombatDisplay::GetNodeTitle(nullptr) was given a COMBAT_NODE_PTR"
            << " that was null.");

        auto creaturePtr{ COMBAT_NODE_PTR->Creature() };
        if (creaturePtr->IsPlayerCharacter())
        {
            return creaturePtr->Name();
        }
        else
        {
            return creaturePtr->Name() + " " + creaturePtr->Role().Name();
        }
    }


    void CombatDisplay::SetBlockingPosOfType(const bool                 IS_PLAYER,
                                             const creature::role::Enum ROLE,
                                             const int                  BLOCKING_POS)
    {
        CombatTree::IdVec_t idVec;
        combatTree_.GetNodeIds(idVec, ROLE);

        for (auto const NEXT_NODE_ID : idVec)
        {
            CombatNodePtr_t nextCombatNodePtr(combatTree_.GetNode(NEXT_NODE_ID));

            if (nextCombatNodePtr->Creature()->IsPlayerCharacter() == IS_PLAYER)
                nextCombatNodePtr->SetBlockingPos(BLOCKING_POS);
        }
    }


    void CombatDisplay::EndOfCombatCleanup()
    {
        isCombatOver_ = true;

        CombatNodePVec_t combatNodesPVec;
        combatTree_.GetCombatNodes(combatNodesPVec);

        for (auto const NEXT_COMBATNODE_PTR : combatNodesPVec)
        {
            auto const NEXT_NODE_ID{combatTree_.GetNodeId(NEXT_COMBATNODE_PTR) };
            auto const NEXT_NODE_SPTR{combatTree_.GetNodeSPtr(NEXT_NODE_ID) };
            combatTree_.RemoveVertex(NEXT_NODE_ID, true);
            RemoveCombatNode(NEXT_NODE_SPTR);
        }
    }

}
}
