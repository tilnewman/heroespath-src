#ifndef GAME_COMBAT_COMBATDISPLAY_INCLUDED
#define GAME_COMBAT_COMBATDISPLAY_INCLUDED
//
// combat-display.hpp
//  A class that handles drawing the combat tree.
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/stage.hpp"
#include "sfml-util/shaker.hpp"
#include "sfml-util/i-callback-handler.hpp"
#include "sfml-util/gui/gui-entity-slider.hpp"

#include "game/combat/summary-view.hpp"
#include "game/creature/creature.hpp"
#include "game/combat/combat-tree.hpp"

#include <memory>
#include <map>
#include <vector>
#include <utility>


namespace sfml_util
{
namespace gui
{
    class Box;
    using BoxSPtr_t = std::shared_ptr<Box>;

    class IGuiEntity;
}
}
namespace game
{
namespace combat
{

    //forward declarations
    class CombatNode;
    using CombatNodePtr_t  = CombatNode *;
    using CombatNodeSPtr_t = std::shared_ptr<CombatNode>;
    using CombatNodePVec_t = std::vector<CombatNodePtr_t>;


    //Assists with delayed position shifting of creature images on the battlefield,
    //allowing creature images on the battlefield to animate to new positions instead
    //of instantly appearing in new positions.
    struct NodePosTracker
    {
        NodePosTracker();

        NodePosTracker(const CombatNodePtr_t COMBAT_NODE_PTR,
                       const float           TARGET_POS_LEFT,
                       const float           TARGET_POS_TOP);

        float posHorizOrig;
        float posVertOrig;
        float horizDiff;
        float vertDiff;
    };

    using NodePosTrackerMap_t = std::map<CombatNodePtr_t, NodePosTracker>;


    //this type maps an individual creature to a specific blocking position
    //used by players/users to save a blocking pattern they prefer
    using BlockingMap_t = std::map<creature::UniqueTraits_t, int>;


    using CombatNodeToIGuiEntityMap_t = std::map<CombatNodeSPtr_t, sfml_util::gui::IGuiEntitySPtr_t>;


    //Handles drawing the combat tree
    class CombatDisplay : public sfml_util::Stage
    {
        //prevent copy constructions/
        CombatDisplay(const CombatDisplay &) =delete;

        //prevent copy assignment
        CombatDisplay & operator=(const CombatDisplay &) =delete;

    public:
        CombatDisplay(const sf::FloatRect & REGION = sf::FloatRect());
        virtual ~CombatDisplay();

        virtual void Setup();

        void Draw(sf::RenderTarget & target, sf::RenderStates states);

        virtual void SetMouseHover(const sf::Vector2f & MOUSE_POS, const bool IS_MOUSE_HOVERING);

        //value must be [0.0f, 1.0f], the constrained final zoom value is returned
        float SetZoomLevel(const float);

        //returns false if no nodes will be drawn
        bool UpdateWhichNodesWillDraw();

        virtual void UpdateMousePos(const sf::Vector2f & MOUSE_POS_V);
        virtual void UpdateMouseDown(const sf::Vector2f & MOUSE_POS_V);
        virtual sfml_util::gui::IGuiEntitySPtr_t UpdateMouseUp(const sf::Vector2f & MOUSE_POS_V);

        const sf::Vector2f GetCenterOfAllNodes() const;

        void PositionCombatTreeCells(const bool WILL_DELAY);

        bool RemoveCombatNode(const CombatNodeSPtr_t & COMBAT_NODE_SPTR);

        void MoveBattlefieldVert(const float AMOUNT, const bool WILL_MOVE_BACKGROUND = true);
        void MoveBattlefieldHoriz(const float AMOUNT, const bool WILL_MOVE_BACKGROUND = true);

        const creature::CreaturePVec_t FindClosestLivingByType(creature::CreatureCPtrC_t CREATURE_CPTRC, const bool WILL_FIND_PLAYERS) const;
        const creature::CreaturePVec_t FindClosestLiving(creature::CreatureCPtrC_t CREATURE_CPTRC, const creature::CreaturePVec_t & AMONG_PVEC) const;

        std::size_t FindCreaturesThatCanBeAttackedOfType(creature::CreaturePVec_t & pVec_OutParam, const creature::CreaturePtrC_t CREATURE_CPTRC, const bool WILL_FIND_PLAYERS) const;
        std::size_t FindCreaturesAllRoundOfType(creature::CreaturePVec_t & pVec_OutParam, creature::CreatureCPtrC_t CREATURE_CPTRC, const bool WILL_FIND_PLAYERS) const;
        std::size_t FindCreaturesInSameBlockingPosOfType(creature::CreaturePVec_t & pVec_OutParam, creature::CreatureCPtrC_t CREATURE_CPTRC, const bool WILL_FIND_PLAYERS) const;
        std::size_t FindCreaturesAtBlockingPosOfType(creature::CreaturePVec_t & pVec_OutParam, const int BLOCKING_POS, const bool WILL_FIND_PLAYERS) const;
        int FindBlockingPos(const creature::CreaturePtrC_t CREATURE_CPTRC) const;

        const creature::CreaturePVec_t FindClosestAmongOfType(const creature::CreaturePtrC_t   CREATURE_OF_ORIGIN_CPTRC,
                                                              const creature::CreaturePVec_t & CREATURES_TO_FIND_AMONG_PVEC,
                                                              const bool                       WILL_FIND_PLAYERS) const;

        bool IsCreatureAPossibleFightTarget(const creature::CreaturePtrC_t CREATURE_FIGHTING_CPTRC,
                                            const creature::CreaturePtrC_t CREATURE_TARGETED_CPTRC) const;

        CombatNodePtr_t GetCombatNodeForCreature(creature::CreatureCPtrC_t) const;
        CombatNodePVec_t GetCombatNodesForCreatures(const creature::CreaturePVec_t &) const;

        //returns explanation of why not or an empty string if can
        const std::string CanAdvanceOrRetreat(creature::CreatureCPtrC_t CREATURE_CPTRC, const bool TRYING_TO_ADVANCE) const;

        creature::CreaturePtr_t GetCreatureAtPos(const sf::Vector2f &);

        void MoveCreatureBlockingPosition(creature::CreatureCPtrC_t, const bool WILL_MOVE_FORWARD);

        void CancelSummaryViewAndStartTransitionBack();

        void HandleFlyingChange(const creature::CreaturePtrC_t CREATURE_CPTRC, const bool IS_FLYING);

        void HandleEndOfTurnTasks();

        void UpdateHealthTasks();

        bool IsCreatureVisible(creature::CreatureCPtrC_t) const;

        bool AreAllCreaturesVisible(const creature::CreaturePVec_t &);

        bool IsZoomOutRequired(const creature::CreaturePVec_t &) const;

        const sf::Vector2f FindCenterOfCreatures(const creature::CreaturePVec_t &) const;

        void SetCreatureHighlight(creature::CreatureCPtrC_t CREATURE_CPTRC, const bool WILL_HIGHLIGHT);

        void InitialPlayerPartyCombatTreeSetup();

        void InitialNonPlayerPartyCombatTreeSetup();

        void SortCreatureListByDisplayedPosition(creature::CreaturePVec_t &) const;

        inline void SetIsPlayerTurn(const bool B)                               { isPlayerTurn_ = B; }

        inline int GetBlockingDistanceBetween(const creature::CreaturePtrC_t & A_CPTRC, const creature::CreaturePtrC_t & B_CPTRC) const { return combatTree_.GetBlockingDistanceBetween(A_CPTRC, B_CPTRC); }

        inline int GetClosestBlockingDistanceByType(const creature::CreaturePtrC_t & CPTRC, const bool WILL_FIND_PLAYERS) const { return combatTree_.GetClosestBlockingDistanceByType(CPTRC, WILL_FIND_PLAYERS); }

        inline bool GetIsStatusMessageAnimating() const                         { return isStatusMessageAnim_; }

        inline void SetIsStatusMessageAnimating(const bool B)                   { isStatusMessageAnim_ = B; }

        inline bool GetIsSummaryViewInProgress() const                          { return isSummaryViewInProgress_; }

        inline void GetCombatNodes(CombatNodePVec_t & combatNodesPVec)          { combatTree_.GetCombatNodes(combatNodesPVec); }

        inline void SetSummaryViewAllowed(const bool IS_SUMMARYVIEW_ALLOWED)    { isSummaryViewAllowed_ = IS_SUMMARYVIEW_ALLOWED; }

        inline void SetScrollingAllowed(const bool IS_SCROLLING_ALLOWED)        { isScrollAllowed_ = IS_SCROLLING_ALLOWED; }

        inline CombatTree & CombatTreeObj()                                     { return combatTree_; }

        inline const sf::FloatRect BattlefieldRect()                            { return battlefieldRect_; }

        inline const sf::Vector2f CenteringPosV()                               { return centeringToPosV_; }
        inline void CenteringPosV(const sf::Vector2f & V)                       { centeringToPosV_ = V; }

        inline NodePosTrackerMap_t NodePositionTrackerMap()                     { return nodePosTrackerMap_; }

    protected:
        inline void SetIsSummaryViewInProgress(const bool B)                    { isSummaryViewInProgress_ = B; }

        virtual void UpdateTime(const float ELAPSED_TIME_SECONDS);

        void CreatureToneDown(const float TONE_DOWN_VAL);

        const std::string GetNodeTitle(const CombatNodePtr_t COMBAT_NODE_PTR);

        void SetBlockingPosOfType(const bool                 IS_PLAYER,
                                  const creature::role::Enum ROLE,
                                  const int                  BLOCKING_POS);

    public:
        static const float       BATTLEFIELD_MARGIN_;
        static BlockingMap_t     blockingMap_;
        static const float       POSITIONING_CELL_SIZE_RATIO_MIN_HORIZ_;
        static const float       POSITIONING_CELL_SIZE_RATIO_MAX_HORIZ_;
        static const float       POSITIONING_CELL_SIZE_RATIO_MIN_VERT_;
        static const float       POSITIONING_CELL_SIZE_RATIO_MAX_VERT_;
        static const float       CREATURE_MOVE_SLIDER_SPEED_;
        static const float       BATTLEFIELD_DRAG_SPEED_;
        static const int         BLOCKING_POS_MARGIN_;
        static const int         BLOCKING_POS_INVALID_;
        static const std::size_t SHOULDER_TO_SHOULDER_MAX_;
        static const std::size_t SHOULDER_TO_SHOULDER_OPPOSITE_TYPE_MAX_;

    private:
        const float                     POSITIONING_MARGIN_HORIZ_;
        const float                     POSITIONING_MARGIN_VERT_;
        const float                     POSITIONING_BETWEEN_SPACER_HORIZ_;
        const float                     POSITIONING_BETWEEN_SPACER_VERT_;
        const float                     CELL_HEIGHT_;
        const sfml_util::FontSPtr_t     NAME_FONT_SPTR_;
        const unsigned int              NAME_CHAR_SIZE_ORIG_;
        const float                     SCREEN_WIDTH_;
        const float                     SCREEN_HEIGHT_;
        //
        unsigned int                    nameCharSizeCurr_;
        sf::FloatRect                   battlefieldRect_;
        sfml_util::gui::box::BoxSPtr_t  boxSPtr_;
        sf::Texture                     bgTexture_;
        sf::RenderTexture               offScreenTexture_;
        sf::Sprite                      offScreenSprite_;
        float                           offScreenPosX_;
        float                           offScreenPosY_;
        combat::CombatTree              combatTree_;
        float                           prevScrollPosVert_;
        float                           prevScrollPosHoriz_;
        SummaryView                     summaryView_;
        bool                            isSummaryViewAllowed_;
        bool                            isScrollAllowed_;
        float                           battlefieldWidth_;
        float                           battlefieldHeight_;
        int                             blockingPosMin_;
        int                             blockingPosMax_;
        float                           zoomLevel_;
        bool                            isMouseHeldDownInBF_;
        sf::Vector2f                    prevMousePos_;
        bool                            isPlayerTurn_;
        bool                            isStatusMessageAnim_;
        bool                            isSummaryViewInProgress_;
        CombatNodeToIGuiEntityMap_t     combatNodeToGuiEntityMap_;

        //members to manage node position shifting, put another way,
        //members that allow on battlefield creature images to slide
        //(animate) into new positions rather than instantly moving.
        NodePosTrackerMap_t nodePosTrackerMap_;

        //members that support moving the battlefield and centering
        sf::Vector2f centeringToPosV_;
    };


    using CombatDisplayPtr_t   = CombatDisplay *;
    using CombatDisplayCPtr_t  = const CombatDisplay *;
    using CombatDisplayPtrC_t  = CombatDisplay * const;
    using CombatDisplayCPtrC_t = const CombatDisplay * const;

}
}
#endif //GAME_COMBAT_COMBATDISPLAY_INCLUDED
