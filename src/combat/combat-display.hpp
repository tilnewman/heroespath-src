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
#ifndef HEROESPATH_GAME_COMBAT_COMBATDISPLAY_HPP_INCLUDED
#define HEROESPATH_GAME_COMBAT_COMBATDISPLAY_HPP_INCLUDED
//
// combat-display.hpp
//  A class that handles drawing the combat tree.
//
#include "combat/combat-tree.hpp"
#include "creature/creature.hpp"
#include "misc/vector-map.hpp"
#include "sfml-util/gui/gui-entity-slider.hpp"
#include "sfml-util/i-callback-handler.hpp"
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/stage.hpp"

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {
        namespace box
        {
            class Box;
            using BoxUPtr_t = std::unique_ptr<Box>;
        } // namespace box

        class IGuiEntity;
    } // namespace gui
} // namespace sfml_util

namespace combat
{

    // forward declarations
    class CombatNode;
    using CombatNodePtr_t = CombatNode *;
    using CombatNodeSPtr_t = std::shared_ptr<CombatNode>;
    using CombatNodePVec_t = std::vector<CombatNodePtr_t>;

    class SummaryView;
    using SummaryViewUPtr_t = std::unique_ptr<SummaryView>;

    class CombatAnimation;
    using CombatAnimationPtr_t = CombatAnimation *;

    // Assists with delayed position shifting of creature images on the battlefield,
    // allowing creature images on the battlefield to animate to new positions instead
    // of instantly appearing in new positions.
    struct NodePosTracker
    {
        NodePosTracker();

        NodePosTracker(
            const CombatNodePtr_t COMBAT_NODE_PTR,
            const float TARGET_POS_LEFT,
            const float TARGET_POS_TOP);

        float posHorizOrig;
        float posVertOrig;
        float horizDiff;
        float vertDiff;
    };

    using NodePosTrackerMap_t = misc::VectorMap<CombatNodePtr_t, NodePosTracker>;

    // this type maps an individual creature to a specific blocking position
    // used by players/users to save a blocking pattern they prefer
    using BlockingMap_t = misc::VectorMap<creature::UniqueTraits_t, int>;

    using CombatNodeToIGuiEntityMap_t
        = misc::VectorMap<CombatNodeSPtr_t, sfml_util::gui::IGuiEntityPtr_t>;

    // Handles drawing the combat tree
    class CombatDisplay : public sfml_util::Stage
    {
    public:
        CombatDisplay(const CombatDisplay &) = delete;
        CombatDisplay(CombatDisplay &&) = delete;
        CombatDisplay & operator=(const CombatDisplay &) = delete;
        CombatDisplay & operator=(CombatDisplay &&) = delete;

    public:
        explicit CombatDisplay(const sf::FloatRect & REGION = sf::FloatRect());
        virtual ~CombatDisplay();

        virtual void Setup();

        virtual void Draw(sf::RenderTarget & target, const sf::RenderStates & STATES);

        virtual void SetMouseHover(const sf::Vector2f & MOUSE_POS, const bool IS_MOUSE_HOVERING);

        bool StartSummaryView(const sf::Vector2f & MOUSE_POS);

        // value must be [0.0f, 1.0f], the constrained final zoom value is returned
        float SetZoomLevel(const float);

        // returns false if no nodes will be drawn
        bool UpdateWhichNodesWillDraw();

        virtual void UpdateMousePos(const sf::Vector2i & MOUSE_POS_V);
        virtual void UpdateMouseDown(const sf::Vector2f & MOUSE_POS_V);
        virtual sfml_util::gui::IGuiEntityPtr_t UpdateMouseUp(const sf::Vector2f & MOUSE_POS_V);

        const sf::Vector2f GetCenterOfAllNodes() const;

        void PositionCombatTreeCells(const bool WILL_DELAY);

        bool RemoveCombatNode(const CombatNodeSPtr_t & COMBAT_NODE_SPTR);

        void MoveBattlefieldVert(const float AMOUNT, const bool WILL_MOVE_BACKGROUND = true);
        void MoveBattlefieldHoriz(const float AMOUNT, const bool WILL_MOVE_BACKGROUND = true);

        const creature::CreaturePVec_t FindClosestLivingByType(
            creature::CreatureCPtrC_t CREATURE_CPTRC, const bool WILL_FIND_PLAYERS) const;

        const creature::CreaturePVec_t FindClosestLiving(
            creature::CreatureCPtrC_t CREATURE_CPTRC,
            const creature::CreaturePVec_t & AMONG_PVEC) const;

        std::size_t FindCreaturesThatCanBeAttackedOfType(
            creature::CreaturePVec_t & pVec_OutParam,
            const creature::CreaturePtrC_t CREATURE_CPTRC,
            const bool WILL_FIND_PLAYERS) const;

        std::size_t FindCreaturesAllRoundOfType(
            creature::CreaturePVec_t & pVec_OutParam,
            creature::CreatureCPtrC_t CREATURE_CPTRC,
            const bool WILL_FIND_PLAYERS,
            const bool LIVING_ONLY = false) const;

        std::size_t FindCreaturesInSameBlockingPosOfType(
            creature::CreaturePVec_t & pVec_OutParam,
            creature::CreatureCPtrC_t CREATURE_CPTRC,
            const bool WILL_FIND_PLAYERS) const;

        std::size_t FindCreaturesAtBlockingPosOfType(
            creature::CreaturePVec_t & pVec_OutParam,
            const int BLOCKING_POS,
            const bool WILL_FIND_PLAYERS) const;

        int FindBlockingPos(const creature::CreaturePtrC_t CREATURE_CPTRC) const;

        const creature::CreaturePVec_t FindClosestAmongOfType(
            const creature::CreaturePtrC_t CREATURE_OF_ORIGIN_CPTRC,
            const creature::CreaturePVec_t & CREATURES_TO_FIND_AMONG_PVEC,
            const bool WILL_FIND_PLAYERS) const;

        bool IsCreatureAPossibleFightTarget(
            const creature::CreaturePtrC_t CREATURE_FIGHTING_CPTRC,
            const creature::CreaturePtrC_t CREATURE_TARGETED_CPTRC) const;

        CombatNodePtr_t GetCombatNodeForCreature(creature::CreatureCPtrC_t) const;
        CombatNodePVec_t GetCombatNodesForCreatures(const creature::CreaturePVec_t &) const;

        // returns explanation of why not or an empty string if can
        const std::string CanAdvanceOrRetreat(
            const creature::CreaturePtr_t CREATURE_CPTRC, const bool TRYING_TO_ADVANCE) const;

        creature::CreaturePtr_t GetCreatureAtPos(const sf::Vector2f &);

        void MoveCreatureBlockingPosition(
            creature::CreatureCPtrC_t CREATURE_PTR, const bool WILL_MOVE_FORWARD);

        void CancelSummaryViewAndStartTransitionBack();

        void
            HandleFlyingChange(const creature::CreaturePtrC_t CREATURE_CPTRC, const bool IS_FLYING);

        void HandleEndOfTurnTasks();

        void UpdateHealthTasks();

        bool IsCreatureVisible(creature::CreatureCPtrC_t) const;

        bool AreAllCreaturesVisible(const creature::CreaturePVec_t &);

        bool IsZoomOutRequired(const creature::CreaturePVec_t &) const;

        const sf::Vector2f FindCenterOfCreatures(const creature::CreaturePVec_t &) const;

        void SetCreatureHighlight(
            creature::CreatureCPtrC_t CREATURE_CPTRC, const bool WILL_HIGHLIGHT);

        void InitialPlayerPartyCombatTreeSetup();

        void InitialNonPlayerPartyCombatTreeSetup();

        void SortCreatureListByDisplayedPosition(creature::CreaturePVec_t &) const;

        void SetIsPlayerTurn(const bool B) { isPlayerTurn_ = B; }

        int GetBlockingDistanceBetween(
            const creature::CreaturePtrC_t & A_CPTRC,
            const creature::CreaturePtrC_t & B_CPTRC) const
        {
            return combatTree_.GetBlockingDistanceBetween(A_CPTRC, B_CPTRC);
        }

        int GetClosestBlockingDistanceByType(
            const creature::CreaturePtrC_t & CPTRC, const bool WILL_FIND_PLAYERS) const
        {
            return combatTree_.GetClosestBlockingDistanceByType(CPTRC, WILL_FIND_PLAYERS);
        }

        bool GetIsStatusMessageAnimating() const { return isStatusMessageAnim_; }

        void SetIsStatusMessageAnimating(const bool B) { isStatusMessageAnim_ = B; }

        bool GetIsSummaryViewInProgress() const { return isSummaryViewInProgress_; }

        void GetCombatNodes(CombatNodePVec_t & combatNodesPVec)
        {
            combatTree_.GetCombatNodes(combatNodesPVec);
        }

        void SetSummaryViewAllowed(const bool IS_SUMMARYVIEW_ALLOWED)
        {
            isSummaryViewAllowed_ = IS_SUMMARYVIEW_ALLOWED;
        }

        void SetScrollingAllowed(const bool IS_SCROLLING_ALLOWED)
        {
            isScrollAllowed_ = IS_SCROLLING_ALLOWED;
        }

        CombatTree & CombatTreeObj() { return combatTree_; }

        const sf::FloatRect BattlefieldRect() const { return battlefieldRect_; }

        const sf::Vector2f CenteringPosV() const { return centeringToPosV_; }
        void CenteringPosV(const sf::Vector2f & V) { centeringToPosV_ = V; }

        NodePosTrackerMap_t NodePositionTrackerMap() { return nodePosTrackerMap_; }

        void SetCombatAnimationPtr(const CombatAnimationPtr_t ptr) { combatAnimationPtr_ = ptr; }

        void EndOfCombatCleanup();

        void HandleDeaths(const combat::CombatNodePVec_t &);

        void HandleCombatNodeElimination(const creature::CreaturePtr_t);
        void HandleCombatNodeElimination(const combat::CombatNodePtr_t);

        std::size_t GetObstacleCreaturesAtBlockingPos(
            creature::CreaturePVec_t & pVec_OutParam,
            const creature::CreaturePtr_t CREATURE_ATTEMPTING_PTR,
            const int BLOCKING_POS) const;

        std::size_t GetCreaturesInRoaringDistance(
            const creature::CreaturePtr_t CREATURE_ROARING_PTR,
            creature::CreaturePVec_t & creaturesInRoaringDistancePVec_OutParam) const;

    protected:
        void SetIsSummaryViewInProgress(const bool B) { isSummaryViewInProgress_ = B; }

        virtual void UpdateTime(const float ELAPSED_TIME_SECONDS);

        void CreatureToneDown(const float TONE_DOWN_VAL);

        const std::string GetNodeTitle(const CombatNodePtr_t COMBAT_NODE_PTR);

        bool SetBlockingPosOfType(
            const bool IS_PLAYER, const creature::role::Enum ROLE, const int BLOCKING_POS);

        void InitialCreaturePositionsSetup(const bool WILL_POSITION_PLAYERS);

        void MoveCombatNodesOfGivenRolesIntoTargetVec(
            const creature::RoleVec_t & ROLES_VEC,
            CombatNodePVec_t & sourceCNodePVec,
            CombatNodePVec_t & targetCNodePVec);

    public:
        static const float BATTLEFIELD_MARGIN_;
        static BlockingMap_t blockingMap_;
        static const float POSITIONING_CELL_SIZE_RATIO_MIN_HORIZ_;
        static const float POSITIONING_CELL_SIZE_RATIO_MAX_HORIZ_;
        static const float POSITIONING_CELL_SIZE_RATIO_MIN_VERT_;
        static const float POSITIONING_CELL_SIZE_RATIO_MAX_VERT_;
        static const float CREATURE_MOVE_SLIDER_SPEED_;
        static const float BATTLEFIELD_DRAG_SPEED_;
        static const int BLOCKING_POS_MARGIN_;
        static const int BLOCKING_POS_INVALID_;
        static const std::size_t SHOULDER_TO_SHOULDER_MAX_;

    private:
        const float POSITIONING_MARGIN_HORIZ_;
        const float POSITIONING_MARGIN_VERT_;
        const float POSITIONING_BETWEEN_SPACER_HORIZ_;
        const float POSITIONING_BETWEEN_SPACER_VERT_;
        const float CELL_HEIGHT_;
        const unsigned int NAME_CHAR_SIZE_ORIG_;
        const float SCREEN_WIDTH_;
        const float SCREEN_HEIGHT_;
        //
        unsigned int nameCharSizeCurr_;
        sf::FloatRect battlefieldRect_;
        sfml_util::gui::box::BoxUPtr_t boxUPtr_;
        sf::Texture bgTexture_;
        sf::RenderTexture offScreenTexture_;
        sf::Sprite offScreenSprite_;
        float offScreenPosX_;
        float offScreenPosY_;
        combat::CombatTree combatTree_;
        float prevScrollPosVert_;
        float prevScrollPosHoriz_;
        SummaryViewUPtr_t summaryViewUPtr_;
        bool isSummaryViewAllowed_;
        bool isScrollAllowed_;
        float battlefieldWidth_;
        float battlefieldHeight_;
        int blockingPosMin_;
        int blockingPosMax_;
        float zoomLevel_;
        bool isMouseHeldDownInBF_;
        sf::Vector2f prevMousePos_;
        bool isPlayerTurn_;
        bool isStatusMessageAnim_;
        bool isSummaryViewInProgress_;
        CombatNodeToIGuiEntityMap_t combatNodeToGuiEntityMap_;
        CombatAnimationPtr_t combatAnimationPtr_;
        bool isCombatOver_;

        // members to manage node position shifting, put another way,
        // members that allow on battlefield creature images to slide
        //(animate) into new positions rather than instantly moving.
        NodePosTrackerMap_t nodePosTrackerMap_;

        // members that support moving the battlefield and centering
        sf::Vector2f centeringToPosV_;
    };

    using CombatDisplayPtr_t = CombatDisplay *;
    using CombatDisplayCPtr_t = const CombatDisplay *;
    using CombatDisplayPtrC_t = CombatDisplay * const;
    using CombatDisplayCPtrC_t = const CombatDisplay * const;

} // namespace combat
} // namespace heroespath

#endif // HEROESPATH_GAME_COMBAT_COMBATDISPLAY_HPP_INCLUDED
