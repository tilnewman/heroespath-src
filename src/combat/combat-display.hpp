// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GAME_COMBAT_COMBATDISPLAY_HPP_INCLUDED
#define HEROESPATH_GAME_COMBAT_COMBATDISPLAY_HPP_INCLUDED
//
// combat-display.hpp
//  A class that handles drawing the combat tree.
//
#include "combat/combat-tree.hpp"
#include "gui/cached-texture.hpp"
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"
#include "misc/vector-map.hpp"
#include "stage/stage-base.hpp"

#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace heroespath
{
namespace creature
{
    class Creature;
    using CreaturePtr_t = misc::NotNull<Creature *>;
    using CreaturePtrOpt_t = boost::optional<CreaturePtr_t>;
    using CreaturePVec_t = std::vector<CreaturePtr_t>;
} // namespace creature
namespace gui
{
    class BoxEntity;
    using BoxEntityUPtr_t = std::unique_ptr<BoxEntity>;

} // namespace gui

namespace combat
{

    class CombatNode;
    using CombatNodePtr_t = misc::NotNull<CombatNode *>;
    using CombatNodePVec_t = std::vector<CombatNodePtr_t>;

    class SummaryView;
    using SummaryViewUPtr_t = std::unique_ptr<SummaryView>;

    class CombatAnimation;
    using CombatAnimationPtr_t = misc::NotNull<CombatAnimation *>;

    using CreatureBlockingPosMap_t = misc::VectorMap<creature::CreaturePtr_t, int>;

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

    // Handles drawing the combat tree
    class CombatDisplay : public stage::StageBase

    {
    public:
        CombatDisplay(const CombatDisplay &) = delete;
        CombatDisplay(CombatDisplay &&) = delete;
        CombatDisplay & operator=(const CombatDisplay &) = delete;
        CombatDisplay & operator=(CombatDisplay &&) = delete;

        explicit CombatDisplay(const CombatAnimationPtr_t);

        virtual ~CombatDisplay();

        void Setup() override;

        void Draw(sf::RenderTarget & target, const sf::RenderStates & STATES) override;

        bool StartSummaryView(const sf::Vector2f & MOUSE_POS);

        // value must be [0.0f, 1.0f], the constrained final zoom value is returned
        float SetZoomLevel(const float);

        // returns false if no nodes will be drawn
        bool UpdateWhichNodesWillDraw();

        void UpdateMousePos(const sf::Vector2i & MOUSE_POS_V) override;
        void UpdateMouseDown(const sf::Vector2f & MOUSE_POS_V) override;

        const gui::IEntityPtrOpt_t UpdateMouseUp(const sf::Vector2f & MOUSE_POS_V) override;

        const sf::Vector2f GetCenterOfAllNodes() const;

        void MoveBattlefieldVert(const float AMOUNT, const bool WILL_MOVE_BACKGROUND = true);
        void MoveBattlefieldHoriz(const float AMOUNT, const bool WILL_MOVE_BACKGROUND = true);

        const creature::CreaturePVec_t FindClosestLivingByType(
            const creature::CreaturePtr_t CREATURE_PTR, const bool WILL_FIND_PLAYERS) const;

        const creature::CreaturePVec_t FindClosestLiving(
            const creature::CreaturePtr_t CREATURE_PTR,
            const creature::CreaturePVec_t & AMONG_PVEC) const;

        const creature::CreaturePVec_t FindCreaturesThatCanBeAttackedOfType(
            const creature::CreaturePtr_t CREATURE_PTR, const bool WILL_FIND_PLAYERS) const;

        const creature::CreaturePVec_t FindCreaturesAllRoundOfType(
            const creature::CreaturePtr_t CREATURE_PTR,
            const bool WILL_FIND_PLAYERS,
            const bool LIVING_ONLY = false) const;

        const creature::CreaturePVec_t FindCreaturesAtBlockingPosOfType(
            const int BLOCKING_POS, const bool WILL_FIND_PLAYERS) const;

        int FindBlockingPos(const creature::CreaturePtr_t CREATURE_PTR) const;

        const creature::CreaturePVec_t FindClosestAmongOfType(
            const creature::CreaturePtr_t CREATURE_OF_ORIGIN_PTR,
            const creature::CreaturePVec_t & CREATURES_TO_FIND_AMONG_PVEC,
            const bool WILL_FIND_PLAYERS) const;

        bool IsCreatureAPossibleFightTarget(
            const creature::CreaturePtr_t CREATURE_FIGHTING_PTR,
            const creature::CreaturePtr_t CREATURE_TARGETED_PTR) const;

        CombatNodePtr_t GetCombatNodeForCreature(const creature::CreaturePtr_t) const;
        const CombatNodePVec_t GetCombatNodesForCreatures(const creature::CreaturePVec_t &) const;

        // returns explanation of why not or an empty string if can
        const std::string CanAdvanceOrRetreat(
            const creature::CreaturePtr_t CREATURE_PTR, const bool TRYING_TO_ADVANCE) const;

        const creature::CreaturePtrOpt_t GetCreatureAtPosPtrOpt(const sf::Vector2f &);

        void MoveCreatureBlockingPosition(
            const creature::CreaturePtr_t CREATURE_PTR, const bool WILL_MOVE_FORWARD);

        void CancelSummaryViewAndStartTransitionBack();

        void HandleFlyingChange(const creature::CreaturePtr_t CREATURE_PTR, const bool IS_FLYING);

        void HandleEndOfTurnTasks();

        void UpdateHealthTasks();

        bool IsCreatureVisible(const creature::CreaturePtr_t) const;

        bool AreAllCreaturesVisible(const creature::CreaturePVec_t &);

        bool IsZoomOutRequired(const creature::CreaturePVec_t &) const;

        const sf::Vector2f FindCenterOfCreatures(const creature::CreaturePVec_t &) const;

        void SetCreatureHighlight(
            const creature::CreaturePtr_t CREATURE_PTR, const bool WILL_HIGHLIGHT);

        void InitialPlayerPartyCombatTreeSetup();

        void InitialNonPlayerPartyCombatTreeSetup();

        void SortCreatureListByDisplayedPosition(creature::CreaturePVec_t &) const;

        void SetIsPlayerTurn(const bool B) { isPlayerTurn_ = B; }

        int GetBlockingDistanceBetween(
            const creature::CreaturePtr_t & A_PTR, const creature::CreaturePtr_t & B_PTR) const
        {
            return combatTree_.GetBlockingDistanceBetween(A_PTR, B_PTR);
        }

        int GetClosestBlockingDistanceByType(
            const creature::CreaturePtr_t & CPTRC, const bool WILL_FIND_PLAYERS) const
        {
            return combatTree_.GetClosestBlockingDistanceByType(CPTRC, WILL_FIND_PLAYERS);
        }

        bool GetIsStatusMessageAnimating() const { return isStatusMessageAnim_; }

        void SetIsStatusMessageAnimating(const bool B) { isStatusMessageAnim_ = B; }

        bool GetIsSummaryViewInProgress() const { return isSummaryViewInProgress_; }

        void SetSummaryViewAllowed(const bool IS_SUMMARYVIEW_ALLOWED)
        {
            isSummaryViewAllowed_ = IS_SUMMARYVIEW_ALLOWED;
        }

        void SetScrollingAllowed(const bool IS_SCROLLING_ALLOWED)
        {
            isScrollAllowed_ = IS_SCROLLING_ALLOWED;
        }

        const sf::FloatRect BattlefieldRect() const { return battlefieldRect_; }

        const sf::Vector2f CenteringPosV() const { return centeringToPosV_; }
        void CenteringPosV(const sf::Vector2f & V) { centeringToPosV_ = V; }

        void EndOfCombatCleanup();

        void HandleCombatNodeElimination(const combat::CombatNodePVec_t &);

        void HandleCombatNodeElimination(const creature::CreaturePtr_t);

        const creature::CreaturePVec_t GetObstacleCreaturesAtBlockingPos(
            const creature::CreaturePtr_t CREATURE_ATTEMPTING_PTR, const int BLOCKING_POS) const;

        const creature::CreaturePVec_t
            GetCreaturesInRoaringDistance(const creature::CreaturePtr_t CREATURE_ROARING_PTR) const;

        const creature::CreaturePVec_t GetCreaturesFlying() const;

        const CreatureBlockingPosMap_t MakeCreatureBlockingMap() const;

        void SetBlockingPositions(const CreatureBlockingPosMap_t &);

        const sf::Vector2f GetCombatNodeCenter(const creature::CreaturePtr_t) const;

        void SetInitiallyFlyingCreaturesToFlying();

        void RepositionCombatNodesBasedOnSliderPosition(const float SLIDER_POS);

    protected:
        void PositionCombatTreeCells(const bool WILL_DELAY);

        void RemoveCombatNodes(const IDVec_t & COMBAT_TREE_IDS_TO_REMOVE_VEC);

        void SetIsSummaryViewInProgress(const bool B) { isSummaryViewInProgress_ = B; }

        void UpdateTime(const float ELAPSED_TIME_SECONDS) override;

        void CreatureToneDown(const float TONE_DOWN_VAL);

        void InitialCreaturePositionsSetup(const bool WILL_POSITION_PLAYERS);

        void MoveCombatNodesOfGivenRolesIntoTargetVec(
            const creature::RoleVec_t & ROLES_VEC,
            CombatNodePVec_t & sourceCNodePVec,
            CombatNodePVec_t & targetCNodePVec);

    public:
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
        gui::BoxEntityUPtr_t boxUPtr_;
        gui::CachedTexture bgCachedTexture_;
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
        CombatAnimationPtr_t combatAnimationPtr_;
        bool isCombatOver_;

        // members to manage node position shifting, put another way,
        // members that allow on battlefield creature images to slide
        //(animate) into new positions rather than instantly moving.
        NodePosTrackerMap_t nodePosTrackerMap_;

        // members that support moving the battlefield and centering
        sf::Vector2f centeringToPosV_;
    };

    using CombatDisplayPtr_t = misc::NotNull<CombatDisplay *>;

} // namespace combat
} // namespace heroespath

#endif // HEROESPATH_GAME_COMBAT_COMBATDISPLAY_HPP_INCLUDED
