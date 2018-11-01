// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_COMBATSTAGE_HPP_INCLUDED
#define HEROESPATH_COMBATSTAGE_HPP_INCLUDED
//
// combat-stage.hpp
//
#include "combat/combat-restore-info.hpp"
#include "combat/combat-sound-effects.hpp"
#include "combat/combat-text.hpp"
#include "combat/creature-interaction.hpp"
#include "combat/fight-results.hpp"
#include "combat/turn-action-enum.hpp"
#include "combat/turn-action-info.hpp"
#include "combat/turn-decider.hpp"
#include "creature/achievement-enum.hpp"
#include "creature/title.hpp"
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"
#include "sfml-util/box-entity-info.hpp"
#include "sfml-util/callback.hpp"
#include "sfml-util/horiz-symbol.hpp"
#include "sfml-util/image-text-entity.hpp"
#include "sfml-util/list-box.hpp"
#include "sfml-util/slider-color.hpp"
#include "sfml-util/sliderbar.hpp"
#include "sfml-util/sliders.hpp"
#include "sfml-util/sound-manager.hpp"
#include "stage/stage-base.hpp"

#include <memory>
#include <set>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace heroespath
{
namespace gui
{
    class BoxEntity;
    using BoxEntityUPtr_t = std::unique_ptr<BoxEntity>;

    class TextRegion;
    using TextRegionUPtr_t = std::unique_ptr<TextRegion>;

} // namespace gui

namespace creature
{
    // forward declarations
    class Creature;
    using CreaturePtr_t = misc::NotNull<Creature *>;
    using CreaturePVec_t = std::vector<CreaturePtr_t>;
    using CreaturePtrOpt_t = boost::optional<CreaturePtr_t>;
} // namespace creature

namespace combat
{
    // forward declarations
    class CombatDisplay;
    using CombatDisplayPtr_t = misc::NotNull<CombatDisplay *>;

    class CombatAnimation;
    using CombatAnimationUPtr_t = std::unique_ptr<CombatAnimation>;
} // namespace combat

namespace stage
{
    // Responsible for wrapping all the perform report indicies.
    struct ReportIndicies
    {
        explicit ReportIndicies(const std::size_t EFFECT_INDEX = 0, const std::size_t HIT_INDEX = 0)
            : effect(EFFECT_INDEX)
            , hit(HIT_INDEX)
        {}

        std::size_t effect;
        std::size_t hit;

        friend bool operator<(const ReportIndicies &, const ReportIndicies &);
    };

    inline bool operator<(const ReportIndicies & L, const ReportIndicies & R)
    {
        return std::tie(L.effect, L.hit) < std::tie(R.effect, R.hit);
    }

    using ReportIndexesSet_t = std::set<ReportIndicies>;

    class CombatStage;
    using CombatStageListBox_t = gui::ListBox<CombatStage, gui::NoElement_t>;

    // A Stage class that allows camping characters
    class CombatStage
        : public stage::StageBase

        , public gui::PopupCallback_t::IHandler_t
        , public gui::ImageTextEntity::Callback_t::IHandler_t
        , public CombatStageListBox_t::Callback_t::IHandler_t
        , public gui::SliderBar::Callback_t::IHandler_t
    {
    public:
        CombatStage(const CombatStage &) = delete;
        CombatStage(CombatStage &&) = delete;
        CombatStage & operator=(const CombatStage &) = delete;
        CombatStage & operator=(CombatStage &&) = delete;

        // defines what phase of the initial zoom and pan currently in
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

        // defines what stage of a creature's turn we are in
        enum class TurnPhase
        {
            NotATurn = 0,

            // center on turn creature
            CenterAndZoomIn,

            PostCenterAndZoomInPause,
            Determine,
            TargetSelect,
            ConditionEffectPause,

            // center on turn creature and targets of whatever
            // action the turn creature is taking (performType_)
            CenterAndZoomOut,

            PostCenterAndZoomOutPause,

            // see enum PerformType for which anim is performed here and
            // PerformAnimPhase for which phase that anim is in
            PerformAnim,

            PerformReport,
            PostPerformPause,
            StatusAnim,
            DeathAnim,
            RepositionAnim,
            PostTurnPause,
            Achievements,
            End,
            Count
        };

        // defines what type of action a creature is performing on his/her/it turn
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
            PlaySong,
            Roar,
            Pounce,
            Run,
            Count
        };

        // definese what phase of the perform action animation is currently displaying
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
            Spell,
            PostSpellPause,
            Song,
            PostSongPause,
            Roar,
            PostRoarPause,
            Run,
            FinalPause,
            Count
        };

    public:
        explicit CombatStage(
            combat::CombatAnimationUPtr_t combatAnimUPtr,
            const combat::CombatDisplayPtr_t COMBAT_DISPLAY_STAGE_PTR,
            const bool WILL_ADVANCE_TURN);

        virtual ~CombatStage();

        bool HandleCallback(const CombatStageListBox_t::Callback_t::PacketPtr_t &) override;

        bool HandleCallback(const gui::ImageTextEntity::Callback_t::PacketPtr_t &) override;

        bool HandleCallback(const gui::SliderBar::Callback_t::PacketPtr_t &) override;
        bool HandleCallback(const gui::PopupCallback_t::PacketPtr_t &) override;

        void PreSetup();
        void Setup() override;
        void Draw(sf::RenderTarget & target, const sf::RenderStates &) override;
        void UpdateTime(const float ELAPSED_TIME_SECONDS) override;

        void UpdateMouseDown(const sf::Vector2f & MOUSE_POS_V) override;

        const gui::IEntityPtrOpt_t UpdateMouseUp(const sf::Vector2f & MOUSE_POS_V) override;

        bool IsPaused() const { return (pauseElapsedSec_ < pauseDurationSec_); }

        bool KeyRelease(const sf::Event::KeyEvent & KE) override;

        bool IsPlayerCharacterTurnValid() const;
        bool IsNonPlayerCharacterTurnValid() const;

        const sf::FloatRect CombatRegion() const { return COMBAT_REGION_; }

    private:
        const sf::FloatRect CalcCombatRegion() const;
        const sf::FloatRect CalcStatusRegion(const sf::FloatRect & COMBAT_REGION) const;
        const sf::FloatRect CalcCommandRegion() const;

        void AppendInitialStatus();
        void AppendStatusMessage(const std::string & MSG_STR, const bool WILL_ANIM = true);
        void StartPause(const float DURATION_SEC, const std::string & TITLE);
        void EndPause();
        void HandlePerformReportPhaseOverTasks();

        void HandleEnemyTurnStep1_Decide(const creature::CreaturePtr_t TURN_CREATURE_PTR);

        TurnActionPhase
            HandleEnemyTurnStep2_Perform(const creature::CreaturePtr_t TURN_CREATURE_PTR);

        void StartTurn_Step1();
        void StartTurn_Step2(const creature::CreaturePtr_t TURN_CREATURE_PTR);
        void EndTurn();
        void PositionSlideStartTasks(const creature::CreaturePtr_t TURN_CREATURE_PTR);
        bool HandleAttack();
        bool HandleFight();

        bool HandleSong_Step1_ValidatePlayAndSelectSong(
            const creature::CreaturePtr_t TURN_CREATURE_PTR);

        void HandleSong_Step2_PerformOnTargets(const creature::CreaturePtr_t TURN_CREATURE_PTR);

        bool HandleCast_Step1_ValidateCastAndSelectSpell(
            const creature::CreaturePtr_t TURN_CREATURE_PTR);

        void HandleCast_Step2_SelectTargetOrPerformOnAll();

        void HandleCast_Step3_PerformOnTargets(
            const creature::CreaturePVec_t & CREATURES_TO_CAST_UPON_PVEC);

        bool HandleAdvance();
        bool HandleRetreat();
        bool HandleBlock();
        bool HandleSkip();
        bool HandleFly();
        bool HandleLand();
        bool HandleRoar();
        bool HandlePounce(const bool IS_SKY_POUNCE);
        bool HandleWeaponChange();
        bool HandleRun();
        void MoveTurnBoxObjectsOffScreen();

        void SetupTurnBoxButtons(
            const creature::CreaturePtrOpt_t & CREATURE_PTR_OPT,
            const bool WILL_DISABLE_ALL = false);

        void QuickSmallPopup(
            const std::string & PROMPT,
            const bool IS_SOUNDEFFECT_NORMAL,
            const bool WILL_PREPEND_NEWLINE);

        void SetupTurnBox();
        void StartPerformAnim();

        const std::string TurnPhaseToString(const TurnPhase);
        const std::string TurnActionPhaseToString(const TurnActionPhase);
        const std::string PreTurnPhaseToString(const PreTurnPhase);
        const std::string AnimPhaseToString(const AnimPhase);

        void UpdateTestingText();

        void SetTurnPhase(const TurnPhase TP)
        {
            turnPhase_ = TP;
            UpdateTestingText();
        }

        void SetPreTurnPhase(const PreTurnPhase PTP)
        {
            preTurnPhase_ = PTP;
            UpdateTestingText();
        }

        void SetAnimPhase(const AnimPhase AP)
        {
            animPhase_ = AP;
            UpdateTestingText();
        }

        void SetTurnActionPhase(const TurnActionPhase TAP)
        {
            turnActionPhase_ = TAP;
            UpdateTestingText();
        }

        TurnActionPhase GetTurnActionPhaseFromWeaponType(const item::ItemPtr_t) const;
        TurnActionPhase GetTurnActionPhaseFromFightResult(const combat::FightResult &) const;

        void SetUserActionAllowed(const bool IS_ALLOWED);

        void HandleKilledCreatures();
        void HandleApplyDamageTasks();

        void HandleAttackTasks(const creature::CreaturePtr_t CREATURE_TO_ATTACK_PTR);

        bool HandleMiscCancelTasks();

        void HandlePlayingMeleeSoundEffects();

        bool HandleWin();
        bool HandleLose();
        bool DetectWinOrLose(const bool IS_DETECTING_WIN);

        void EndOfCombatCleanup();

        void SystemErrorPopup(
            const std::string & GENERAL_ERROR_MSG,
            const std::string & TECH_ERROR_MSG,
            const std::string & TITLE_MSG = "");

        // returns true if any Titles were conferred
        bool PopulateAchievementsVec();

        void HandleAchievementEnqueue(
            const creature::CreaturePtr_t CREATURE_PTR,
            const creature::AchievementType::Enum ACH_ENUM,
            const int COUNT = 1);

        bool HandleAchievementPopups();

        void PerformRoarEffects();

        void AnimationCenteringStart(const creature::CreaturePVec_t &);

        void MakeButton(
            gui::ImageTextEntityUPtr_t & buttonUPtr,
            const std::string & TEXT,
            const std::string & HOVER_TEXT);

        void SetButtonDisabledIf(gui::ImageTextEntityUPtr_t & buttonUPtr, const bool WILL_DISABLE);

    public:
        static const std::string POPUP_NAME_SPELLBOOK_;
        static const std::string POPUP_NAME_MUSICSHEET_;
        static const std::string POPUP_NAME_COMBATOVER_WIN_;
        static const std::string POPUP_NAME_COMBATOVER_LOSE_;
        static const std::string POPUP_NAME_COMBATOVER_RAN_;
        static const std::string POPUP_NAME_ACHIEVEMENT_;
        //
        static const float PAUSE_LONG_SEC_;
        static const float PAUSE_MEDIUM_SEC_;
        static const float PAUSE_SHORT_SEC_;
        static const float PAUSE_SHORTER_SEC_;
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
        static const float CONDITION_EFFECT_PAUSE_SEC_;
        static const float POST_SPELL_ANIM_PAUSE_SEC_;
        static const float ACHIEVEMENT_PAUSE_SEC_;
        static const float END_PAUSE_SEC_;
        //
        static const float DOUBLE_CLICK_WINDOW_SEC_;
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
        static const float ANIM_RUN_SLIDER_SPEED_;
        //
        static const sf::Color LISTBOX_BACKGROUND_COLOR_;
        static const sf::Color LISTBOX_HIGHLIGHT_COLOR_;
        static const sf::Color LISTBOX_HIGHLIGHT_ALT_COLOR_;
        static const sf::Color LISTBOX_SELECTED_COLOR_;
        static const sf::Color LISTBOX_NOTSELECTED_COLOR_;
        static const sf::Color LISTBOX_LINE_COLOR_;

    private:
        // stores information about the state of combat when temporarily in another stage.
        //(inventory, Setup, etc.)
        static combat::RestoreInfo restoreInfo_;
        //
        const bool WILL_ADVANCE_TURN_;
        const float COMBAT_REGION_MARGIN_;
        const float STATUS_REGION_SLIDERBAR_WIDTH_;
        const float COMMAND_REGION_PAD_;
        const sf::FloatRect COMBAT_REGION_;
        const sf::FloatRect STATUS_REGION_;
        const sf::FloatRect COMMAND_REGION_;
        //
        combat::Text combatText_;
        combat::TurnDecider turnDecider_;
        //
        gui::BoxEntityUPtr_t commandBoxUPtr_;
        gui::ListBoxUPtr_t<CombatStage, gui::NoElement_t> statusBoxUPtr_;
        gui::TextInfo statusBoxTextInfo_;
        gui::SliderBarUPtr_t zoomSliderBarUPtr_;
        gui::BoxEntityUPtr_t turnBoxUPtr_;
        sf::FloatRect turnBoxRegion_;
        combat::CombatSoundEffects combatSoundEffects_;
        ReportIndexesSet_t soundEffectsPlayedSet_;
        TurnPhase turnPhase_;
        PreTurnPhase preTurnPhase_;
        TurnActionPhase turnActionPhase_;
        AnimPhase animPhase_;
        spell::SpellPtrOpt_t spellBeingCastPtrOpt_;
        song::SongPtrOpt_t songBeingPlayedPtrOpt_;
        std::size_t performReportEffectIndex_;
        std::size_t performReportHitIndex_;
        float zoomSliderOrigPos_;
        bool willClrShkInitStatusMsg_;
        bool isShortPostZoomOutPause_;
        bool hasCombatEnded_;
        bool isRepositionAnimAfterRun_;

        // members that manage condition effects per turn
        combat::HitInfoVec_t conditionEffectsVec_;
        std::size_t conditionEffectsIndex_;
        bool conditionEffectsTookTurn_;
        sf::Vector2f conditionEffectsCenterPosV_;
        bool conditionEffectsWillSkip_;

        // A slider member that is used for various slider tasks
        gui::SliderZeroToOne slider_;

        // this member controls combat related animations
        combat::CombatAnimationUPtr_t combatAnimationUPtr_;

        // The scope of this is controlled by Loop,
        // so check before use during shutdown of the stage.
        combat::CombatDisplayPtr_t combatDisplayStagePtr_;

        gui::ImageTextEntityUPtr_t settingsButtonUPtr_;

        // members that control pausing the CombatStage
        float pauseDurationSec_;
        float pauseElapsedSec_;
        bool isPauseCanceled_;

        // members that deal with which creature's turn it is and
        // the timing of taking turns
        creature::CreaturePtrOpt_t turnCreaturePtrOpt_;
        gui::ColorSlider goldTextColorSlider_;
        gui::ColorSlider redTextColorSlider_;
        combat::TurnActionInfo turnActionInfo_;
        combat::FightResult fightResult_;
        bool willRedColorShakeWeaponText_;
        bool isFightResultCollapsed_;

        // members displaying a player character's turn
        gui::TextRegionUPtr_t titleTBoxTextRegionUPtr_;
        gui::TextRegionUPtr_t weaponTBoxTextRegionUPtr_;
        gui::TextRegionUPtr_t armorTBoxTextRegionUPtr_;
        gui::TextRegionUPtr_t infoTBoxTextRegionUPtr_;
        gui::TextRegionUPtr_t enemyActionTBoxRegionUPtr_;
        gui::TextRegionUPtr_t enemyCondsTBoxRegionUPtr_;
        gui::TextRegionUPtr_t zoomLabelTextRegionUPtr_;
        gui::ImageTextEntityUPtr_t attackTBoxButtonUPtr_;
        gui::ImageTextEntityUPtr_t fightTBoxButtonUPtr_;
        gui::ImageTextEntityUPtr_t castTBoxButtonUPtr_;
        gui::ImageTextEntityUPtr_t advanceTBoxButtonUPtr_;
        gui::ImageTextEntityUPtr_t retreatTBoxButtonUPtr_;
        gui::ImageTextEntityUPtr_t blockTBoxButtonUPtr_;
        gui::ImageTextEntityUPtr_t skipTBoxButtonUPtr_;
        gui::ImageTextEntityUPtr_t flyTBoxButtonUPtr_;
        gui::ImageTextEntityUPtr_t landTBoxButtonUPtr_;
        gui::ImageTextEntityUPtr_t roarTBoxButtonUPtr_;
        gui::ImageTextEntityUPtr_t pounceTBoxButtonUPtr_;
        gui::ImageTextEntityUPtr_t runTBoxButtonUPtr_;

        // members that manage the status message animations
        float statusMsgAnimTimerSec_;
        gui::ColorSlider statusMsgAnimColorSlider_;

        // testing display members
        gui::TextRegionUPtr_t testingTextRegionUPtr_;
        std::string pauseTitle_;

        // members supporting double-click
        float clickTimerSec_;
        sf::Vector2f clickPosV_;

        // members that support playing songs
        bool isSongAnim1Done_;
        bool isSongAnim2Done_;

        // members that support achievements and titles
        creature::TitleTransitionVec_t creatureTitlesVec_;

        combat::CreatureInteraction creatureInteraction_;
    };

} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_COMBATSTAGE_HPP_INCLUDED
