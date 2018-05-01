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
#ifndef HEROESPATH_COMBATSTAGE_HPP_INCLUDED
#define HEROESPATH_COMBATSTAGE_HPP_INCLUDED
//
// combat-stage.hpp
//
#include "combat/combat-restore-info.hpp"
#include "combat/combat-sound-effects.hpp"
#include "combat/fight-results.hpp"
#include "combat/turn-action-enum.hpp"
#include "combat/turn-action-info.hpp"
#include "creature/achievement-enum.hpp"
#include "creature/title.hpp"
#include "misc/boost-optional-that-throws.hpp"
#include "misc/handy-types.hpp"
#include "misc/not-null.hpp"
#include "popup/i-popup-callback.hpp"
#include "sfml-util/color-shaker.hpp"
#include "sfml-util/gui/four-state-button.hpp"
#include "sfml-util/gui/list-box.hpp"
#include "sfml-util/gui/sliderbar.hpp"
#include "sfml-util/horiz-symbol.hpp"
#include "sfml-util/i-callback-handler.hpp"
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/sfml-system.hpp"
#include "sfml-util/sliders.hpp"
#include "sfml-util/sound-manager.hpp"
#include "sfml-util/stage.hpp"

#include <memory>
#include <set>
#include <string>
#include <tuple>
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

        class TextRegion;
        using TextRegionUPtr_t = std::unique_ptr<TextRegion>;
    } // namespace gui
} // namespace sfml_util

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
        ReportIndicies(const std::size_t EFFECT_INDEX = 0, const std::size_t HIT_INDEX = 0)
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

    // A Stage class that allows camping characters
    class CombatStage
        : public sfml_util::Stage
        , public popup::IPopupHandler_t
        , public sfml_util::gui::callback::IFourStateButtonCallbackHandler_t
        , public sfml_util::gui::callback::IListBoxCallbackHandler
        , public sfml_util::gui::callback::ISliderBarCallbackHandler_t
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
        explicit CombatStage(const bool WILL_ADVANCE_TURN);
        virtual ~CombatStage();

        virtual const std::string HandlerName() const { return GetStageName(); }
        virtual bool HandleCallback(const sfml_util::gui::callback::ListBoxEventPackage &);

        virtual bool
            HandleCallback(const sfml_util::gui::callback::FourStateButtonCallbackPackage_t &);

        virtual bool HandleCallback(const sfml_util::gui::callback::SliderBarCallbackPackage_t &);
        virtual bool HandleCallback(const popup::PopupResponse &);

        virtual void Setup();
        virtual void Draw(sf::RenderTarget & target, const sf::RenderStates &);
        virtual void UpdateTime(const float ELAPSED_TIME_SECONDS);

        virtual void UpdateMouseDown(const sf::Vector2f & MOUSE_POS_V);
        virtual const sfml_util::gui::IGuiEntityPtrOpt_t
            UpdateMouseUp(const sf::Vector2f & MOUSE_POS_V);

        bool IsPaused() const { return (pauseElapsedSec_ < pauseDurationSec_); }

        virtual bool KeyRelease(const sf::Event::KeyEvent & KE);

        bool IsPlayerCharacterTurnValid() const;
        bool IsNonPlayerCharacterTurnValid() const;

    private:
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
            const creature::CreaturePtrOpt_t CREATURE_PTR_OPT, const bool WILL_DISABLE_ALL = false);

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
        const float SCREEN_WIDTH_;
        const float SCREEN_HEIGHT_;
        //
        sfml_util::gui::box::BoxUPtr_t commandBoxUPtr_;
        sfml_util::gui::ListBoxUPtr_t statusBoxUPtr_;
        sfml_util::gui::TextInfo statusBoxTextInfo_;
        sfml_util::gui::SliderBarUPtr_t zoomSliderBarUPtr_;
        sfml_util::gui::box::BoxUPtr_t turnBoxUPtr_;
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
        sfml_util::sliders::ZeroSliderOnce<float> slider_;

        // this member controls combat related animations
        combat::CombatAnimationUPtr_t combatAnimationUPtr_;

        // The scope of this is controlled by Loop,
        // so check before use during shutdown of the stage.
        combat::CombatDisplayPtr_t combatDisplayStagePtr_;

        sfml_util::gui::FourStateButtonUPtr_t settingsButtonUPtr_;

        // members that control pausing the CombatStage
        float pauseDurationSec_;
        float pauseElapsedSec_;
        bool isPauseCanceled_;

        // members that deal with which creature's turn it is and
        // the timing of taking turns
        creature::CreaturePtrOpt_t turnCreaturePtrOpt_;
        sfml_util::ColorShaker goldTextColorShaker_;
        sfml_util::ColorShaker redTextColorShaker_;
        combat::TurnActionInfo turnActionInfo_;
        combat::FightResult fightResult_;
        bool willRedColorShakeWeaponText_;
        bool isFightResultCollapsed_;

        // members displaying a player character's turn
        sfml_util::gui::TextRegionUPtr_t titleTBoxTextRegionUPtr_;
        sfml_util::gui::TextRegionUPtr_t weaponTBoxTextRegionUPtr_;
        sfml_util::gui::TextRegionUPtr_t armorTBoxTextRegionUPtr_;
        sfml_util::gui::TextRegionUPtr_t infoTBoxTextRegionUPtr_;
        sfml_util::gui::TextRegionUPtr_t enemyActionTBoxRegionUPtr_;
        sfml_util::gui::TextRegionUPtr_t enemyCondsTBoxRegionUPtr_;
        sfml_util::gui::TextRegionUPtr_t zoomLabelTextRegionUPtr_;
        sfml_util::gui::FourStateButtonUPtr_t attackTBoxButtonUPtr_;
        sfml_util::gui::FourStateButtonUPtr_t fightTBoxButtonUPtr_;
        sfml_util::gui::FourStateButtonUPtr_t castTBoxButtonUPtr_;
        sfml_util::gui::FourStateButtonUPtr_t advanceTBoxButtonUPtr_;
        sfml_util::gui::FourStateButtonUPtr_t retreatTBoxButtonUPtr_;
        sfml_util::gui::FourStateButtonUPtr_t blockTBoxButtonUPtr_;
        sfml_util::gui::FourStateButtonUPtr_t skipTBoxButtonUPtr_;
        sfml_util::gui::FourStateButtonUPtr_t flyTBoxButtonUPtr_;
        sfml_util::gui::FourStateButtonUPtr_t landTBoxButtonUPtr_;
        sfml_util::gui::FourStateButtonUPtr_t roarTBoxButtonUPtr_;
        sfml_util::gui::FourStateButtonUPtr_t pounceTBoxButtonUPtr_;
        sfml_util::gui::FourStateButtonUPtr_t runTBoxButtonUPtr_;

        // members that manage the status message animations
        float statusMsgAnimTimerSec_;
        sfml_util::ColorShaker statusMsgAnimColorShaker_;

        // testing display members
        sfml_util::gui::TextRegionUPtr_t testingTextRegionUPtr_;
        std::string pauseTitle_;

        // members supporting double-click
        float clickTimerSec_;
        sf::Vector2f clickPosV_;

        // members that support playing songs
        bool isSongAnim1Done_;
        bool isSongAnim2Done_;

        // members that support achievements and titles
        creature::TitleTransitionVec_t creatureTitlesVec_;
    };

} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_COMBATSTAGE_HPP_INCLUDED
