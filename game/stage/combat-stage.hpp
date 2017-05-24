#ifndef GAME_COMBATSTAGE_INCLUDED
#define GAME_COMBATSTAGE_INCLUDED
//
// combat-stage.hpp
//  A Stage class that enables combat.
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/sfml-system.hpp"
#include "sfml-util/stage.hpp"
#include "sfml-util/i-callback-handler.hpp"
#include "sfml-util/sound-manager.hpp"
#include "sfml-util/sliders.hpp"
#include "sfml-util/color-shaker.hpp"
#include "sfml-util/gui/list-box.hpp"
#include "sfml-util/gui/four-state-button.hpp"
#include "sfml-util/gui/sliderbar.hpp"
#include "sfml-util/sparks-animation.hpp"

#include "game/i-popup-callback.hpp"
#include "game/horiz-symbol.hpp"
#include "game/combat/turn-action-enum.hpp"
#include "game/combat/turn-action-info.hpp"
#include "game/combat/fight-results.hpp"
#include "game/combat/combat-sound-effects.hpp"
#include "game/combat/combat-restore-info.hpp"

#include <memory>
#include <vector>
#include <string>


//forward declarations
namespace sfml_util
{
namespace gui
{
    class Box;
    using BoxSPtr_t = std::shared_ptr<Box>;
}
}

namespace game
{
namespace creature
{
    //forward declarations
    class Creature;
    using CreaturePtr_t   = Creature *;
    using CreatureCPtr_t  = const Creature *;
    using CreaturePtrC_t  = Creature * const;
    using CreatureCPtrC_t = const Creature * const;
    using CreaturePVec_t  = std::vector<CreaturePtr_t>;
}
namespace combat
{
    //forward declarations
    class CombatDisplay;
    using CombatDisplayPtr_t   = CombatDisplay *;
    using CombatDisplayCPtr_t  = const CombatDisplay *;
    using CombatDisplayPtrC_t  = CombatDisplay * const;
    using CombatDisplayCPtrC_t = const CombatDisplay * const;

    class Encounter;
    using EncounterSPtr_t = std::shared_ptr<Encounter>;

    class CombatAnimation;
    using CombatAnimationPtr_t = CombatAnimation *;
}
namespace stage
{

    //A Stage class that allows camping characters
    class CombatStage
    :
        public sfml_util::Stage,
        public game::callback::IPopupHandler_t,
        public sfml_util::gui::callback::IFourStateButtonCallbackHandler_t,
        public sfml_util::gui::callback::IListBoxCallbackHandler,
        public sfml_util::gui::callback::ISliderBarCallbackHandler_t
    {
        //prevent copy construction
        CombatStage(const CombatStage &) =delete;

        //prevent copy assignment
        CombatStage & operator=(const CombatStage &) =delete;

        //defines what phase of the initial zoom and pan currently in
        enum class PreTurnPhase
        {
            Start = 0,
            PanToCenter,
            PostPanPause,
            ZoomOut,
            PostZoomOutPause,
            End,
            Count
        };

        //defines what stage of a creature's turn we are in
        enum class TurnPhase
        {
            NotATurn = 0,
            CenterAndZoomIn,//center on turn creature
            PostCenterAndZoomInPause,
            Determine,
            TargetSelect,
            ConditionWakePause,
            CenterAndZoomOut,//center on turn creature and targets of whatever action the turn creature is taking (performType_)
            PostCenterAndZoomOutPause,
            PerformAnim, //see enum PerformType for which anim is performed here and PerformAnimPhase for which phase that anim is in
            PerformReport,
            PostPerformPause,
            StatusAnim,
            DeathAnim,
            PostDeathAnimSlide,
            PostTurnPause,
            Count
        };

        //defines what type of action a creature is performing on his/her/it turn
        enum class TurnActionPhase
        {
            None = 0,
            PauseAndReport,
            MeleeWeapon,
            ShootSling,
            ShootArrow,
            ShootBlowpipe,
            Advance,
            Retreat,
            Cast,
            Roar,
            Pounce,
            Count
        };

        //definese what phase of the perform action animation is currently displaying
        enum class AnimPhase
        {
            NotAnimating = 0,
            AdvanceOrRetreat,
            ProjectileShoot,
            MoveToward,
            PostMoveTowardPause,
            Impact,
            PostImpactPause,
            MoveBack,
            FinalPause,
            Count
        };

    public:
        CombatStage();
        virtual ~CombatStage();

        inline virtual const std::string HandlerName() const { return GetStageName(); }
        virtual bool HandleCallback(const sfml_util::gui::callback::ListBoxEventPackage &);
        virtual bool HandleCallback(const sfml_util::gui::callback::FourStateButtonCallbackPackage_t &);
        virtual bool HandleCallback(const sfml_util::gui::callback::SliderBarCallbackPackage_t &);
        virtual bool HandleCallback(const game::callback::PopupResponse &);

        virtual void Setup();
        virtual void Draw(sf::RenderTarget & target, const sf::RenderStates &);
        virtual void UpdateTime(const float ELAPSED_TIME_SECONDS);

        virtual void UpdateMouseDown(const sf::Vector2f & MOUSE_POS_V);
        virtual void UpdateMousePos(const sf::Vector2f & MOUSE_POS_V);
        virtual sfml_util::gui::IGuiEntitySPtr_t UpdateMouseUp(const sf::Vector2f & MOUSE_POS_V);

        inline bool IsPaused() const { return (pauseElapsedSec_ < pauseDurationSec_); }

        virtual bool KeyRelease(const sf::Event::KeyEvent & KE);

        bool IsPlayerCharacterTurnValid() const;
        bool IsNonPlayerCharacterTurnValid() const;

    private:
        void AppendInitialStatus();
        void AppendStatusMessage(const std::string & MSG_STR, const bool WILL_ANIM = true);
        void StartPause(const float DURATION_SEC, const std::string & TITLE);
        void EndPause();
        void HandlePerformReportPhaseOverTasks();
        void HandleEnemyTurnStep1_Decide();
        TurnActionPhase HandleEnemyTurnStep2_Perform();
        void StartTurn_Step1();
        void StartTurn_Step2();
        void EndTurn();
        void PositionSlideStartTasks();
        bool HandleAttack();
        bool HandleFight();
        bool HandleCast_Step1_ValidateCastAndSelectSpell();
        void HandleCast_Step2_SelectTargetOrPerformOnAll(spell::SpellPtr_t);
        void HandleCast_Step3_PerformOnTargets(creature::CreaturePVec_t creaturesToCastUponPVec);
        bool HandleAdvance();
        bool HandleRetreat();
        bool HandleBlock();
        bool HandleSkip();
        bool HandleFly();
        bool HandleLand();
        bool HandleRoar();
        bool HandlePounce(const bool IS_SKY_POUNCE);
        bool HandleWeaponChange();
        void HandleAfterTurnTasks();
        void MoveTurnBoxObjectsOffScreen(const bool WILL_MOVE_SKIP_BUTTON);
        void MoveTurnBoxButtonsOffScreen(const bool WILL_MOVE_SKIP_BUTTON);
        void SetupTurnBoxButtons(const creature::CreaturePtrC_t);

        void QuickSmallPopup(const std::string & PROMPT,
                             const bool          IS_SOUNDEFFECT_NORMAL,
                             const bool          WILL_PREPEND_NEWLINE);

        void SetupTurnBox();
        void StartPerformAnim();

        const std::string TurnPhaseToString(const TurnPhase);
        const std::string TurnActionPhaseToString(const TurnActionPhase);
        const std::string PreTurnPhaseToString(const PreTurnPhase);
        const std::string AnimPhaseToString(const AnimPhase);

        void UpdateTestingText();

        inline void SetTurnPhase(const TurnPhase TP)                { turnPhase_ = TP; UpdateTestingText(); }
        inline void SetPreTurnPhase(const PreTurnPhase PTP)         { preTurnPhase_ = PTP; UpdateTestingText(); }
        inline void SetAnimPhase(const AnimPhase AP)                { animPhase_ = AP; UpdateTestingText(); }
        inline void SetTurnActionPhase(const TurnActionPhase TAP)   { turnActionPhase_ = TAP; UpdateTestingText(); }

        TurnActionPhase GetTurnActionPhaseFromWeaponType(const item::ItemSPtr_t &) const;
        TurnActionPhase GetTurnActionPhaseFromFightResult(const combat::FightResult &) const;

        void SetUserActionAllowed(const bool IS_ALLOWED);

        void HandleKilledCreatures();
        void HandleApplyDamageTasks();

        void HandleAttackTasks(creature::CreaturePtr_t creatureToAttackPtr);

        bool HandleMiscCancelTasks();

        const std::string RemoveSingleTurnTemporaryConditions();

    public:
        static const std::string POPUP_NAME_SPELLBOOK_;
        //
        static const float PAUSE_LONG_SEC_;
        static const float PAUSE_MEDIUM_SEC_;
        static const float PAUSE_SHORT_SEC_;
        static const float PAUSE_ZERO_SEC_;
        static const float POST_PAN_PAUSE_SEC_;
        static const float POST_ZOOMOUT_PAUSE_SEC_;
        static const float POST_ZOOMIN_PAUSE_SEC_;
        static const float CREATURE_TURN_PAUSE_SEC_;
        static const float PRE_TURN_PAUSE_SEC_;
        static const float POST_ZOOM_TURN_PAUSE_SEC_;
        static const float PERFORM_PRE_REPORT_PAUSE_SEC_;
        static const float PERFORM_REPORT_PAUSE_SEC_;
        static const float POST_PERFORM_REPORT_PAUSE_SEC_;
        static const float POST_TURN_PAUSE_SEC_;
        static const float STATUSMSG_ANIM_PAUSE_SEC_;
        static const float POST_MELEEMOVE_ANIM_PAUSE_SEC_;
        static const float POST_IMPACT_ANIM_PAUSE_SEC_;
        static const float CONDITION_WAKE_PAUSE_SEC_;
        //
        static const float SLIDER_SPEED_SLOWEST_;
        static const float SLIDER_SPEED_SLOW_;
        static const float SLIDER_SPEED_NORMAL_;
        static const float SLIDER_SPEED_FAST_;
        static const float SLIDER_SPEED_FASTEST_;
        static const float ZOOM_SLIDER_SPEED_;
        static const float TEXT_COLOR_SHAKER_SPEED_;
        static const float ANIM_CENTERING_SLIDER_SPEED_;
        static const float ANIM_INITIAL_CENTERING_SLIDER_SPEED_;
        static const float ANIM_CREATURE_POS_SLIDER_SPEED_;
        static const float ANIM_PROJECTILE_SHOOT_SLIDER_SPEED_;
        static const float ANIM_DEATH_SLIDER_SPEED_;
        static const float ANIM_MELEE_MOVE_SLIDER_SPEED_;
        static const float ANIM_IMPACT_SLIDER_SPEED_;
        static const float ANIM_CREATURE_SHAKE_SLIDER_SPEED_;
        static const float ANIM_IMPACT_SHAKE_SLIDER_SPEED_;
        //
        static const sf::Color LISTBOX_BACKGROUND_COLOR_;
        static const sf::Color LISTBOX_HIGHLIGHT_COLOR_;
        static const sf::Color LISTBOX_HIGHLIGHT_ALT_COLOR_;
        static const sf::Color LISTBOX_SELECTED_COLOR_;
        static const sf::Color LISTBOX_NOTSELECTED_COLOR_;
        static const sf::Color LISTBOX_LINE_COLOR_;

    private:
        //stores information about the state of combat when temporarily in another stage.  (inventory, Setup, etc.)
        static combat::RestoreInfo restoreInfo_;
        //
        const float SCREEN_WIDTH_;
        const float SCREEN_HEIGHT_;
        //
        sfml_util::gui::box::BoxSPtr_t   commandBoxSPtr_;
        sfml_util::gui::ListBoxSPtr_t    statusBoxSPtr_;
        sfml_util::gui::TextInfo         statusBoxTextInfo_;
        sfml_util::gui::SliderBarSPtr_t  zoomSliderBarSPtr_;
        sfml_util::gui::box::BoxSPtr_t   turnBoxSPtr_;
        sf::FloatRect                    turnBoxRegion_;
        combat::EncounterSPtr_t          encounterSPtr_;
        combat::CombatSoundEffects       combatSoundEffects_;
        TurnPhase                        turnPhase_;
        PreTurnPhase                     preTurnPhase_;
        TurnActionPhase                  turnActionPhase_;
        AnimPhase                        animPhase_;
        spell::SpellPtr_t                spellBeingCastPtr_;
        std::size_t                      performReportEffectIndex_;
        std::size_t                      performReportHitIndex_;
        float                            zoomSliderOrigPos_;
        bool                             willClrShkInitStatusMsg_;
        bool                             isMouseHeldDown_;
        bool                             isMouseHeldDownAndMoving_;
        std::string                      tempConditionsWakeStr_;

        //A slider member that is used for various slider tasks
        sfml_util::sliders::ZeroSliderOnce<float> slider_;

        //The scope of this is controlled by Loop, so check before use during shutdown of the stage.
        combat::CombatDisplayPtr_t combatDisplayStagePtr_;

        //this member controls combat related animations
        combat::CombatAnimationPtr_t combatAnimationPtr_;

        sfml_util::gui::FourStateButtonSPtr_t settingsButtonSPtr_;

        //members that control pausing the CombatStage
        float pauseDurationSec_;
        float pauseElapsedSec_;
        bool isPauseCanceled_;

        //members that deal with which creature's turn it is and the timing of taking turns
        creature::CreaturePtr_t turnCreaturePtr_;
        sfml_util::ColorShaker goldTextColorShaker_;
        sfml_util::ColorShaker redTextColorShaker_;
        combat::TurnActionInfo turnActionInfo_;
        combat::FightResult fightResult_;
        bool willRedColorShakeWeaponText_;

        //members displaying a player character's turn
        sfml_util::gui::TextRegionSPtr_t titleTBoxTextRegionSPtr_;
        sfml_util::gui::TextRegionSPtr_t weaponTBoxTextRegionSPtr_;
        sfml_util::gui::TextRegionSPtr_t armorTBoxTextRegionSPtr_;
        sfml_util::gui::TextRegionSPtr_t infoTBoxTextRegionSPtr_;
        sfml_util::gui::TextRegionSPtr_t enemyActionTBoxRegionSPtr_;
        sfml_util::gui::TextRegionSPtr_t enemyCondsTBoxRegionSPtr_;
        sfml_util::gui::FourStateButtonSPtr_t attackTBoxButtonSPtr_;
        sfml_util::gui::FourStateButtonSPtr_t fightTBoxButtonSPtr_;
        sfml_util::gui::FourStateButtonSPtr_t castTBoxButtonSPtr_;
        sfml_util::gui::FourStateButtonSPtr_t advanceTBoxButtonSPtr_;
        sfml_util::gui::FourStateButtonSPtr_t retreatTBoxButtonSPtr_;
        sfml_util::gui::FourStateButtonSPtr_t blockTBoxButtonSPtr_;
        sfml_util::gui::FourStateButtonSPtr_t skipTBoxButtonSPtr_;
        sfml_util::gui::FourStateButtonSPtr_t flyTBoxButtonSPtr_;
        sfml_util::gui::FourStateButtonSPtr_t landTBoxButtonSPtr_;
        sfml_util::gui::FourStateButtonSPtr_t roarTBoxButtonSPtr_;
        sfml_util::gui::FourStateButtonSPtr_t pounceTBoxButtonSPtr_;
        sfml_util::gui::FourStateButtonSVec_t tBoxStdButtonSVec_;
        sfml_util::gui::FourStateButtonSVec_t tBoxBeastButtonSVec_;

        //members that manage the status message animations
        float statusMsgAnimTimerSec_;
        sfml_util::ColorShaker statusMsgAnimColorShaker_;

        //testing display members
        sfml_util::gui::TextRegionSPtr_t testingTextRegionSPtr_;
        std::string pauseTitle_;

        sfml_util::animation::SparksAnimationUPtr_t sparksAnimUPtr_;
    };

}
}
#endif //GAME_COMBATSTAGE_INCLUDED
