#ifndef APPBASE_HEROESPATH_COMBAT_COMBATDISPLAY_INCLUDED
#define APPBASE_HEROESPATH_COMBAT_COMBATDISPLAY_INCLUDED
//
// combat-display.hpp
//  A class that handles drawing the combat tree.
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/stage.hpp"
#include "sfml-util/shaker.hpp"
#include "sfml-util/i-callback-handler.hpp"
#include "sfml-util/gui/gui-entity-slider.hpp"

#include "heroespath/combat/summary-view.hpp"
#include "heroespath/creature/creature.hpp"
#include "heroespath/combat/combat-tree.hpp"

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
namespace heroespath
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


    //All the info required to shake a creature image on the battlefield.
    //Allows for shaking more than one creature at once.
    struct ShakeInfo
    {
        ShakeInfo();

        static const float DURATION_SEC;
        static const float DELAY_SEC;

        const float SHAKE_DISTANCE;

        sfml_util::Shaker<float> shaker;
        float                    delay_timer_sec;
        float                    duration_timer_sec;

        void Reset();
    };

    using SakeInfoMap_t = std::map<combat::CombatNodePtr_t, ShakeInfo>;


    //this type maps an individual creature to a specific blocking position
    //used by players/users to save a blocking pattern they prefer
    using BlockingMap_t = std::map<creature::UniqueTraits_t, int>;


    using CombatNodeToIGuiEntityMap_t = std::map<CombatNodeSPtr_t, sfml_util::gui::IGuiEntitySPtr_t>;


    //Handles drawing the combat tree
    class CombatDisplay : public sfml_util::Stage
    {
        //prevent copy constructions/
        CombatDisplay(const CombatDisplay &);

        //prevent copy assignment
        CombatDisplay & operator=(const CombatDisplay &);

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

        void StartShaking(creature::CreatureCPtrC_t);
        void StopShaking(creature::CreatureCPtrC_t);

        const sf::Vector2f GetCenterOfAllNodes() const;
        
        std::size_t FindCreaturesThatCanBeAttackedOfType(creature::CreaturePVec_t & pVec_OutParam, const creature::CreaturePtrC_t CREATURE_CPTRC, const bool WILL_FIND_PLAYERS) const;
        std::size_t FindCreaturesAllRoundOfType(creature::CreaturePVec_t & pVec_OutParam, creature::CreatureCPtrC_t CREATURE_CPTRC, const bool WILL_FIND_PLAYERS) const;
        std::size_t FindCreaturesInSameBlockingPosOfType(creature::CreaturePVec_t & pVec_OutParam, creature::CreatureCPtrC_t CREATURE_CPTRC, const bool WILL_FIND_PLAYERS) const;
        std::size_t FindCreaturesAtBlockingPosOfType(creature::CreaturePVec_t & pVec_OutParam, const int BLOCKING_POS, const bool WILL_FIND_PLAYERS) const;
        int FindBlockingPos(const creature::CreaturePtrC_t CREATURE_CPTRC) const;

        //returns explanation of why not or an empty string if can
        const std::string CanAdvanceOrRetreat(creature::CreatureCPtrC_t CREATURE_CPTRC, const bool TRYING_TO_ADVANCE) const;

        creature::CreaturePtr_t GetCreatureAtPos(const sf::Vector2f &);

        bool IsCreatureVisible(creature::CreatureCPtrC_t) const;

        inline void SetIsPlayerTurn(const bool B)                               { isPlayerTurn_ = B; }

        inline bool IsCreatureDraggingAllowed() const                           { return isCreatureDragAllowed_; }
        inline void IsCreatureDraggingAllowed(const bool B)                     { isCreatureDragAllowed_ = B; }

        inline int GetBlockingDistanceBetween(const creature::CreaturePtrC_t & A_CPTRC, const creature::CreaturePtrC_t & B_CPTRC) const { return combatTree_.GetBlockingDistanceBetween(A_CPTRC, B_CPTRC); }

        inline int GetClosestBlockingDistanceByType(const creature::CreaturePtrC_t & CPTRC, const bool WILL_FIND_PLAYERS) const { return combatTree_.GetClosestBlockingDistanceByType(CPTRC, WILL_FIND_PLAYERS); }

        const creature::CreaturePVec_t GetClosestByType(creature::CreatureCPtrC_t CREATURE_CPTRC, const bool WILL_FIND_PLAYERS) const;
        const creature::CreaturePVec_t GetClosest(creature::CreatureCPtrC_t CREATURE_CPTRC, const creature::CreaturePVec_t & AMONG_PVEC) const;

        void MoveCreatureBlockingPosition(creature::CreatureCPtrC_t, const bool WILL_MOVE_FORWARD);

        void CancelSummaryViewAndStartTransitionBack();

        inline bool GetIsStatusMessageAnimating() const                         { return isStatusMessageAnim_; }
        inline void SetIsStatusMessageAnimating(const bool B)                   { isStatusMessageAnim_ = B; }

        inline bool GetIsSummaryViewInProgress() const                          { return isSummaryViewInProgress_; }

        void HandleFlyingChange(const creature::CreaturePtrC_t CREATURE_CPTRC, const bool IS_FLYING);

        inline void GetCombatNodes(CombatNodePVec_t & combatNodesPVec)          { combatTree_.GetCombatNodes(combatNodesPVec); }

        void HandleEndOfTurnTasks();

        void UpdateHealthTasks();

        bool AreAllCreaturesVisible(const creature::CreaturePVec_t &);

        bool IsZoomOutRequired(const creature::CreaturePVec_t &) const;

        const sf::Vector2f FindCenterOfCreatures(const creature::CreaturePVec_t & ) const;

        void SetCreatureHighlight(creature::CreatureCPtrC_t CREATURE_CPTRC, const bool WILL_HIGHLIGHT);

        inline void SetUserActionAllowed(const bool IS_ALLOWED)                 { isUserActionAllowed_ = IS_ALLOWED; }

        const CombatNodePtr_t GetCombatNodeForCreature(creature::CreatureCPtrC_t) const;
        const CombatNodePVec_t GetCombatNodesForCreatures(const creature::CreaturePVec_t &) const;

        inline CombatTree & CombatTree()                                        { return combatTree_; }

        void PositionCombatTreeCells(const bool WILL_DELAY);

        inline const sf::FloatRect BattlefieldRect()                            { return battlefieldRect_; }

        inline bool RemoveCombatNode(const CombatNodeSPtr_t & COMBAT_NODE_SPTR) { return EntityRemove(combatNodeToGuiEntityMap_[COMBAT_NODE_SPTR]); combatNodeToGuiEntityMap_.erase(COMBAT_NODE_SPTR); }

        void MoveBattlefieldVert(const float AMOUNT, const bool WILL_MOVE_BACKGROUND = true);
        void MoveBattlefieldHoriz(const float AMOUNT, const bool WILL_MOVE_BACKGROUND = true);

        inline const sf::Vector2f CenteringPosV()                               { return centeringToPosV_; }
        inline void CenteringPosV(const sf::Vector2f & V)                       { centeringToPosV_ = V; }

        inline NodePosTrackerMap_t NodePositionTrackerMap()                     { return nodePosTrackerMap_; }

    protected:
        inline void SetIsSummaryViewInProgress(const bool B)                    { isSummaryViewInProgress_ = B; }

        virtual void UpdateTime(const float ELAPSED_TIME_SECONDS);

        void InitialPlayerPartyCombatTreeSetup();
        void InitialNonPlayerPartyCombatTreeSetup();

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
        bool                            isUserActionAllowed_;
        float                           battlefieldWidth_;
        float                           battlefieldHeight_;
        int                             blockingPosMin_;
        int                             blockingPosMax_;
        float                           zoomLevel_;
        float                           initialZoomHorizDiff_;
        bool                            isMouseHeldDownInBF_;
        sf::Vector2f                    prevMousePos_;
        bool                            isPlayerTurn_;
        bool                            isStatusMessageAnim_;
        bool                            isSummaryViewInProgress_;
        CombatNodeToIGuiEntityMap_t     combatNodeToGuiEntityMap_;

        //members to shake a creature image on the battlefield
        creature::CreatureCPtr_t creatureThatWasShakingCPtr_;//remembers which creature to shake while in summary view
        SakeInfoMap_t            shakeInfoMap_;

        //members to manage node position shifting, put another way,
        //members that allow on battlefield creature images to slide
        //(animate) into new positions rather than instantly moving.
        NodePosTrackerMap_t nodePosTrackerMap_;

        //members that manage creature dragging
        bool isCreatureDragAllowed_;
        bool isMouseHeldDownInCreature_;

        //members that support moving the battlefield and centering
        sf::Vector2f centeringToPosV_;
    };


    using CombatDisplayPtr_t   = CombatDisplay *;
    using CombatDisplayCPtr_t  = const CombatDisplay *;
    using CombatDisplayPtrC_t  = CombatDisplay * const;
    using CombatDisplayCPtrC_t = const CombatDisplay * const;

}
}
#endif //APPBASE_HEROESPATH_COMBAT_COMBATDISPLAY_INCLUDED
