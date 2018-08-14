// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// combat-stage.cpp
//
#include "combat-stage.hpp"

#include "combat/combat-anim.hpp"
#include "combat/combat-display.hpp"
#include "combat/combat-node.hpp"
#include "combat/combat-over-enum.hpp"
#include "combat/encounter.hpp"
#include "combat/turn-action-enum.hpp"
#include "creature/algorithms.hpp"
#include "creature/condition-algorithms.hpp"
#include "creature/condition.hpp"
#include "creature/creature.hpp"
#include "creature/stats.hpp"
#include "game/game-data-file.hpp"
#include "game/game-state-factory.hpp"
#include "game/game-state.hpp"
#include "game/game.hpp"
#include "game/loop-manager.hpp"
#include "game/phase-enum.hpp"
#include "item/algorithms.hpp"
#include "item/item.hpp"
#include "log/log-macros.hpp"
#include "misc/random.hpp"
#include "misc/real.hpp"
#include "misc/vectors.hpp"
#include "popup/popup-manager.hpp"
#include "popup/popup-stage-combat-over.hpp"
#include "popup/popup-stage-image-fade.hpp"
#include "popup/popup-stage-musicsheet.hpp"
#include "popup/popup-stage-spellbook.hpp"
#include "popup/popup-stage-system-error.hpp"
#include "sfml-util/animation-base.hpp"
#include "sfml-util/cloud-animation.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/font-enum.hpp"
#include "sfml-util/font-manager.hpp"
#include "sfml-util/gui/box-entity.hpp"
#include "sfml-util/gui/image-entity.hpp"
#include "sfml-util/gui/text-entity.hpp"
#include "sfml-util/song-animation.hpp"
#include "sfml-util/sparkle-animation.hpp"
#include "sfml-util/sparks-animation.hpp"
#include "sfml-util/text-animation.hpp"
#include "song/song.hpp"
#include "spell/spell.hpp"
#include "stage/achievement-handler.hpp"

#include <exception>
#include <sstream>
#include <string>
#include <vector>

namespace heroespath
{
namespace stage
{

    const std::string CombatStage::POPUP_NAME_SPELLBOOK_ { "SPELLBOOK_POPUP_WINDOW_NAME" };
    const std::string CombatStage::POPUP_NAME_MUSICSHEET_ { "MUSICSHEE_POPUP_WINDOW_NAME" };
    const std::string CombatStage::POPUP_NAME_COMBATOVER_WIN_ {
        "COMBAT_OVER_WIN_POPUP_WINDOW_NAME"
    };
    const std::string CombatStage::POPUP_NAME_COMBATOVER_LOSE_ {
        "COMBAT_OVER_LOSE_POPUP_WINDOW_NAME"
    };
    const std::string CombatStage::POPUP_NAME_COMBATOVER_RAN_ {
        "COMBAT_OVER_RAN_POPUP_WINDOW_NAME"
    };
    const std::string CombatStage::POPUP_NAME_ACHIEVEMENT_ { "ACHIEVEMENT_POPUP_WINDOW_NAME" };
    //
    const float CombatStage::PAUSE_LONG_SEC_ { 6.0f };
    const float CombatStage::PAUSE_MEDIUM_SEC_ { 3.0f };
    const float CombatStage::PAUSE_SHORT_SEC_ { 1.5f };
    const float CombatStage::PAUSE_SHORTER_SEC_ { 0.75f };
    const float CombatStage::PAUSE_ZERO_SEC_ { 0.1f };
    const float CombatStage::POST_PAN_PAUSE_SEC_ { PAUSE_SHORT_SEC_ };
    const float CombatStage::POST_ZOOMOUT_PAUSE_SEC_ { PAUSE_SHORT_SEC_ };
    const float CombatStage::POST_ZOOMIN_PAUSE_SEC_ { PAUSE_SHORT_SEC_ };
    const float CombatStage::CREATURE_TURN_PAUSE_SEC_ { PAUSE_LONG_SEC_ };
    const float CombatStage::PRE_TURN_PAUSE_SEC_ { PAUSE_SHORT_SEC_ };
    const float CombatStage::POST_TURN_PAUSE_SEC_ { PAUSE_ZERO_SEC_ };
    const float CombatStage::PERFORM_PRE_REPORT_PAUSE_SEC_ { PAUSE_ZERO_SEC_ };
    const float CombatStage::PERFORM_REPORT_PAUSE_SEC_ { PAUSE_LONG_SEC_ };
    const float CombatStage::POST_PERFORM_REPORT_PAUSE_SEC_ { PAUSE_LONG_SEC_ };
    const float CombatStage::POST_ZOOM_TURN_PAUSE_SEC_ { PAUSE_MEDIUM_SEC_ };
    const float CombatStage::STATUSMSG_ANIM_PAUSE_SEC_ { PAUSE_SHORT_SEC_ };
    const float CombatStage::POST_MELEEMOVE_ANIM_PAUSE_SEC_ { PAUSE_ZERO_SEC_ };
    const float CombatStage::POST_IMPACT_ANIM_PAUSE_SEC_ { PAUSE_ZERO_SEC_ };
    const float CombatStage::CONDITION_EFFECT_PAUSE_SEC_ { PAUSE_LONG_SEC_ };
    const float CombatStage::POST_SPELL_ANIM_PAUSE_SEC_ { PAUSE_SHORT_SEC_ };
    const float CombatStage::ACHIEVEMENT_PAUSE_SEC_ { PAUSE_ZERO_SEC_ };
    const float CombatStage::END_PAUSE_SEC_ { PAUSE_ZERO_SEC_ };
    //
    const float CombatStage::DOUBLE_CLICK_WINDOW_SEC_ { 0.4f };
    //
    const float CombatStage::SLIDER_SPEED_SLOWEST_ { 1.0f };
    const float CombatStage::SLIDER_SPEED_SLOW_ { SLIDER_SPEED_SLOWEST_ * 2.0f };
    const float CombatStage::SLIDER_SPEED_NORMAL_ { SLIDER_SPEED_SLOW_ * 2.0f };
    const float CombatStage::SLIDER_SPEED_FAST_ { SLIDER_SPEED_NORMAL_ * 2.0f };
    const float CombatStage::SLIDER_SPEED_FASTEST_ { SLIDER_SPEED_NORMAL_ * 10.0f };
    const float CombatStage::ZOOM_SLIDER_SPEED_ { SLIDER_SPEED_SLOW_ };
    const float CombatStage::TEXT_COLOR_SHAKER_SPEED_ { SLIDER_SPEED_FASTEST_ };
    const float CombatStage::ANIM_CENTERING_SLIDER_SPEED_ { SLIDER_SPEED_NORMAL_ };
    const float CombatStage::ANIM_INITIAL_CENTERING_SLIDER_SPEED_ { SLIDER_SPEED_SLOWEST_ };
    const float CombatStage::ANIM_CREATURE_POS_SLIDER_SPEED_ { SLIDER_SPEED_NORMAL_ };
    const float CombatStage::ANIM_PROJECTILE_SHOOT_SLIDER_SPEED_ { SLIDER_SPEED_NORMAL_ };
    const float CombatStage::ANIM_DEATH_SLIDER_SPEED_ { SLIDER_SPEED_SLOWEST_ };
    const float CombatStage::ANIM_MELEE_MOVE_SLIDER_SPEED_ { SLIDER_SPEED_FAST_ };
    const float CombatStage::ANIM_IMPACT_SLIDER_SPEED_ { SLIDER_SPEED_NORMAL_ };
    const float CombatStage::ANIM_CREATURE_SHAKE_SLIDER_SPEED_ { SLIDER_SPEED_FASTEST_ };
    const float CombatStage::ANIM_IMPACT_SHAKE_SLIDER_SPEED_ { SLIDER_SPEED_FASTEST_ * 3.0f };
    const float CombatStage::ANIM_RUN_SLIDER_SPEED_ { SLIDER_SPEED_FAST_ };
    //
    const sf::Color CombatStage::LISTBOX_BACKGROUND_COLOR_ { (
        sfml_util::defaults::Orange - sf::Color(100, 100, 100, 235)) };

    const sf::Color CombatStage::LISTBOX_HIGHLIGHT_COLOR_ {
        (sfml_util::defaults::Orange - sf::Color(100, 100, 100, 235)) + sf::Color(20, 20, 20, 20)
    };

    const sf::Color CombatStage::LISTBOX_HIGHLIGHT_ALT_COLOR_ {
        (sfml_util::defaults::Orange - sf::Color(100, 100, 100, 235)) + sf::Color(40, 40, 40, 40)
    };

    const sf::Color CombatStage::LISTBOX_SELECTED_COLOR_ { sf::Color::White };
    const sf::Color CombatStage::LISTBOX_NOTSELECTED_COLOR_ { sf::Color(150, 150, 150) };
    const sf::Color CombatStage::LISTBOX_LINE_COLOR_ { sf::Color(255, 255, 255, 25) };
    //
    combat::RestoreInfo CombatStage::restoreInfo_;

    CombatStage::CombatStage(const bool WILL_ADVANCE_TURN)
        : Stage(
              "Combat",
              { sfml_util::GuiFont::Default,
                sfml_util::GuiFont::System,
                sfml_util::GuiFont::SystemCondensed,
                sfml_util::GuiFont::Number,
                sfml_util::GuiFont::DefaultBoldFlavor,
                sfml_util::GuiFont::Handwriting },
              true)
        , WILL_ADVANCE_TURN_(WILL_ADVANCE_TURN)
        , SCREEN_WIDTH_(sfml_util::Display::Instance()->GetWinWidth())
        , SCREEN_HEIGHT_(sfml_util::Display::Instance()->GetWinHeight())
        , combatText_()
        , turnDecider_()
        , commandBoxUPtr_()
        , statusBoxUPtr_()
        , statusBoxTextInfo_(
              " ",
              sfml_util::FontManager::Instance()->GetFont(sfml_util::GuiFont::SystemCondensed),
              sfml_util::FontManager::Instance()->Size_Small(),
              sfml_util::defaults::Orange,
              sfml_util::Justified::Left)
        , zoomSliderBarUPtr_()
        , turnBoxUPtr_()
        , turnBoxRegion_()
        , combatSoundEffects_()
        , soundEffectsPlayedSet_()
        , turnPhase_(TurnPhase::NotATurn)
        , preTurnPhase_(PreTurnPhase::Start)
        , turnActionPhase_(TurnActionPhase::None)
        , animPhase_(AnimPhase::NotAnimating)
        , spellBeingCastPtrOpt_()
        , songBeingPlayedPtrOpt_()
        , performReportEffectIndex_(0)
        , performReportHitIndex_(0)
        , zoomSliderOrigPos_(0.0f)
        , willClrShkInitStatusMsg_(false)
        , isShortPostZoomOutPause_(false)
        , hasCombatEnded_(false)
        , isRepositionAnimAfterRun_(false)
        , conditionEffectsVec_()
        , conditionEffectsIndex_(0)
        , conditionEffectsTookTurn_(false)
        , conditionEffectsCenterPosV_(0.0f, 0.0f)
        , conditionEffectsWillSkip_(false)
        ,

        // initiall speed ignored because speed is set before each use,
        // any value greater than zero will work here
        slider_(1.0f)

        , combatAnimationUPtr_(std::make_unique<combat::CombatAnimation>())
        , combatDisplayStagePtr_(new combat::CombatDisplay(combatAnimationUPtr_.get()))
        , settingsButtonUPtr_()
        , pauseDurationSec_(0.0f)
        ,

        // anything greater than pauseTimeDurationSecs_ will work here
        pauseElapsedSec_(pauseDurationSec_ + 1.0f)
        ,

        isPauseCanceled_(false)
        , turnCreaturePtrOpt_()
        ,

        goldTextColorShaker_(
            sfml_util::defaults::Orange, sf::Color::White, TEXT_COLOR_SHAKER_SPEED_)
        ,

        redTextColorShaker_(
            sf::Color(255, 127, 127), sf::Color::White, TEXT_COLOR_SHAKER_SPEED_ * 0.65f)
        , turnActionInfo_()
        , fightResult_()
        , willRedColorShakeWeaponText_(false)
        , isFightResultCollapsed_(false)
        , titleTBoxTextRegionUPtr_()
        , weaponTBoxTextRegionUPtr_()
        , armorTBoxTextRegionUPtr_()
        , infoTBoxTextRegionUPtr_()
        , enemyActionTBoxRegionUPtr_()
        , enemyCondsTBoxRegionUPtr_()
        , zoomLabelTextRegionUPtr_()
        , attackTBoxButtonUPtr_()
        , fightTBoxButtonUPtr_()
        , castTBoxButtonUPtr_()
        , advanceTBoxButtonUPtr_()
        , retreatTBoxButtonUPtr_()
        , blockTBoxButtonUPtr_()
        , skipTBoxButtonUPtr_()
        , flyTBoxButtonUPtr_()
        , landTBoxButtonUPtr_()
        , roarTBoxButtonUPtr_()
        , pounceTBoxButtonUPtr_()
        , runTBoxButtonUPtr_()
        ,

        // anything greater than STATUSMSG_ANIM_PAUSE_SEC_ will work here
        statusMsgAnimTimerSec_(STATUSMSG_ANIM_PAUSE_SEC_ + 1.0f)
        ,

        statusMsgAnimColorShaker_(
            LISTBOX_HIGHLIGHT_COLOR_, LISTBOX_HIGHLIGHT_ALT_COLOR_, 35.0f, false)
        ,

        testingTextRegionUPtr_()
        , pauseTitle_("")
        , clickTimerSec_(-1.0f)
        , // any negative value will work here
        clickPosV_(0.0f, 0.0f)
        , isSongAnim1Done_(false)
        , isSongAnim2Done_(false)
        , creatureTitlesVec_()
        , creatureInteraction_()
    {
        restoreInfo_.CanTurnAdvance(false);
    }

    CombatStage::~CombatStage() { Stage::ClearAllEntities(); }

    bool CombatStage::HandleCallback(const CombatStageListBox_t::Callback_t::PacketPtr_t &)
    {
        return false;
    }

    bool CombatStage::HandleCallback(
        const sfml_util::gui::ImageTextEntity::Callback_t::PacketPtr_t & PACKET_PTR)
    {
        if ((IsPlayerCharacterTurnValid() == false) || (TurnPhase::Determine != turnPhase_))
        {
            return false;
        }

        if (PACKET_PTR->entity_ptr == attackTBoxButtonUPtr_.get())
        {
            return HandleAttack();
        }

        if (PACKET_PTR->entity_ptr == fightTBoxButtonUPtr_.get())
        {
            return HandleFight();
        }

        if (PACKET_PTR->entity_ptr == castTBoxButtonUPtr_.get())
        {
            if (turnCreaturePtrOpt_)
            {
                auto const TURN_CREATURE_PTR { turnCreaturePtrOpt_.value() };
                if (TURN_CREATURE_PTR->Role() == creature::role::Bard)
                {
                    return HandleSong_Step1_ValidatePlayAndSelectSong(TURN_CREATURE_PTR);
                }
                else
                {
                    return HandleCast_Step1_ValidateCastAndSelectSpell(TURN_CREATURE_PTR);
                }
            }
            else
            {
                return false;
            }
        }

        if (PACKET_PTR->entity_ptr == advanceTBoxButtonUPtr_.get())
        {
            return HandleAdvance();
        }

        if (PACKET_PTR->entity_ptr == retreatTBoxButtonUPtr_.get())
        {
            return HandleRetreat();
        }

        if (PACKET_PTR->entity_ptr == blockTBoxButtonUPtr_.get())
        {
            return HandleBlock();
        }

        if (PACKET_PTR->entity_ptr == skipTBoxButtonUPtr_.get())
        {
            return HandleSkip();
        }

        if (PACKET_PTR->entity_ptr == flyTBoxButtonUPtr_.get())
        {
            return HandleFly();
        }

        if (PACKET_PTR->entity_ptr == landTBoxButtonUPtr_.get())
        {
            return HandleLand();
        }

        if (PACKET_PTR->entity_ptr == roarTBoxButtonUPtr_.get())
        {
            return HandleRoar();
        }

        if (PACKET_PTR->entity_ptr == pounceTBoxButtonUPtr_.get())
        {
            auto isSkyPounce(false);

            if (turnCreaturePtrOpt_)
            {
                isSkyPounce = combat::Encounter::Instance()
                                  ->GetTurnInfoCopy(turnCreaturePtrOpt_.value())
                                  .GetIsFlying();
            }

            return HandlePounce(isSkyPounce);
        }

        if (PACKET_PTR->entity_ptr == settingsButtonUPtr_.get())
        {
            restoreInfo_.PrepareForStageChange(combatDisplayStagePtr_);
            game::LoopManager::Instance()->TransitionTo_Settings();
            return true;
        }

        if (PACKET_PTR->entity_ptr == runTBoxButtonUPtr_.get())
        {
            return HandleRun();
        }

        return false;
    }

    bool CombatStage::HandleCallback(
        const sfml_util::gui::SliderBar::Callback_t::PacketPtr_t & PACKET_PTR)
    {
        if (PACKET_PTR == zoomSliderBarUPtr_.get())
        {
            // only zoom out half the distance that the slider actually shows
            auto const HALF_ZOOM_DIFFERENCE { 1.0f
                                              - ((1.0f - PACKET_PTR->PositionRatio()) * 0.5f) };

            combatDisplayStagePtr_->SetZoomLevel(HALF_ZOOM_DIFFERENCE);
            return true;
        }
        else
        {
            return false;
        }
    }

    bool
        CombatStage::HandleCallback(const sfml_util::gui::PopupCallback_t::PacketPtr_t & PACKET_PTR)
    {
        if (PACKET_PTR->Name() == POPUP_NAME_ACHIEVEMENT_)
        {
            return HandleAchievementPopups();
        }
        else if ((PACKET_PTR->Name() == POPUP_NAME_MUSICSHEET_) && turnCreaturePtrOpt_)
        {
            auto const CREATURE_PTR { turnCreaturePtrOpt_.value() };

            if ((PACKET_PTR->Response() == popup::ResponseTypes::Select)
                && PACKET_PTR->SelectionOpt())
            {
                auto const SONGS_PVEC { CREATURE_PTR->SongsPVec() };

                auto const SELECTED_INDEX { PACKET_PTR->SelectionOpt().value() };

                M_ASSERT_OR_LOGANDTHROW_SS(
                    (SELECTED_INDEX < SONGS_PVEC.size()),
                    "stage::CombatStage::HandleCallback(SONG_POPUP_RESPONSE, selection="
                        << SELECTED_INDEX
                        << ") Selection was greater than SongPVec.size=" << SONGS_PVEC.size());

                CREATURE_PTR->LastSongPlayedNum(SELECTED_INDEX);
                songBeingPlayedPtrOpt_ = SONGS_PVEC.at(SELECTED_INDEX);
                HandleSong_Step2_PerformOnTargets(CREATURE_PTR);
                return true;
            }
            else
            {
                return false;
            }
        }
        if (PACKET_PTR->Name() == POPUP_NAME_SPELLBOOK_)
        {
            if ((PACKET_PTR->Response() == popup::ResponseTypes::Select)
                && PACKET_PTR->SelectionOpt())
            {
                auto const SPELLS_PVEC { turnCreaturePtrOpt_.value()->SpellsPVec() };

                auto const SELECTED_INDEX { PACKET_PTR->SelectionOpt().value() };

                M_ASSERT_OR_LOGANDTHROW_SS(
                    (SELECTED_INDEX < SPELLS_PVEC.size()),
                    "stage::CombatStage::HandleCallback(SPELL_POPUP_RESPONSE, selection="
                        << SELECTED_INDEX
                        << ") Selection was greater than SpellPVec.size=" << SPELLS_PVEC.size());

                spellBeingCastPtrOpt_ = SPELLS_PVEC.at(SELECTED_INDEX);
                turnCreaturePtrOpt_.value()->LastSpellCastNum(SELECTED_INDEX);
                HandleCast_Step2_SelectTargetOrPerformOnAll();
                return true;
            }
            else
            {
                return false;
            }
        }
        else if (PACKET_PTR->Name() == POPUP_NAME_COMBATOVER_WIN_)
        {
            // TODO If popup response is YES, then goto the loot stage, and if NO,
            //     then goto the adventure stage.
            game::LoopManager::Instance()->TransitionTo_Credits();
        }
        else if (PACKET_PTR->Name() == POPUP_NAME_COMBATOVER_LOSE_)
        {
            // TODO if popup response is YES, then load last saved game
            game::LoopManager::Instance()->TransitionTo_Credits();
        }
        else if (PACKET_PTR->Name() == POPUP_NAME_COMBATOVER_RAN_)
        {
            // TODO go directly back to adventure stage
            game::LoopManager::Instance()->TransitionTo_Credits();
        }

        return false;
    }

    void CombatStage::Setup()
    {
        // define combat region
        const float COMBAT_REGION_MARGIN(25.0f);
        const float COMBAT_REGION_TOP(COMBAT_REGION_MARGIN);
        const float COMBAT_REGION_LEFT(COMBAT_REGION_MARGIN);
        const float COMBAT_REGION_WIDTH(SCREEN_WIDTH_ - (COMBAT_REGION_MARGIN * 2.0f) + 10.0f);

        const float COMBAT_REGION_HEIGHT(
            (SCREEN_HEIGHT_ - (COMBAT_REGION_MARGIN * 2.0f)) - sfml_util::MapByRes(200.0f, 800.0f));

        const sf::FloatRect COMBAT_REGION(
            COMBAT_REGION_LEFT, COMBAT_REGION_TOP, COMBAT_REGION_WIDTH, COMBAT_REGION_HEIGHT);

        // status box
        const float SLIDERBAR_VERT_SPACER(25.0f);
        const float STATUS_REGION_WIDTH_RATIO(0.666f);
        const float STATUS_REGION_SLIDERBAR_WIDTH(35.0f);
        const float STATUS_REGION_LEFT(COMBAT_REGION_LEFT);

        const float STATUS_REGION_TOP(
            COMBAT_REGION_TOP + COMBAT_REGION_HEIGHT + SLIDERBAR_VERT_SPACER);

        const float STATUS_REGION_WIDTH(
            (SCREEN_WIDTH_ * STATUS_REGION_WIDTH_RATIO) - STATUS_REGION_SLIDERBAR_WIDTH);

        const float STATUS_REGION_HEIGHT(
            (((SCREEN_HEIGHT_ - COMBAT_REGION_TOP) - COMBAT_REGION_HEIGHT) - SLIDERBAR_VERT_SPACER)
            - 25.0f);

        const sf::FloatRect STATUS_REGION(
            STATUS_REGION_LEFT, STATUS_REGION_TOP, STATUS_REGION_WIDTH, STATUS_REGION_HEIGHT);

        sfml_util::gui::BoxEntityInfo statusBoxInfo;

        statusBoxInfo.SetupImage(
            sfml_util::CachedTexture(
                "media-images-backgrounds-tile-darkknot",
                sfml_util::ImageOpt::Default | sfml_util::ImageOpt::Repeated),
            sfml_util::ScreenRatioToPixelsHoriz(0.06f));

        statusBoxInfo.SetupColor(LISTBOX_BACKGROUND_COLOR_);

        statusBoxInfo.SetupBorder(true);

        statusBoxInfo.focus_colors
            = sfml_util::gui::FocusColors(LISTBOX_SELECTED_COLOR_, LISTBOX_NOTSELECTED_COLOR_);

        statusBoxUPtr_
            = std::make_unique<sfml_util::gui::ListBox<CombatStage, sfml_util::gui::NoElement_t>>(
                "ComabtStage'sStatus",
                this,
                this,
                sfml_util::gui::ListBoxPacket(
                    STATUS_REGION,
                    statusBoxInfo,
                    LISTBOX_LINE_COLOR_,
                    sfml_util::gui::ListBoxPacket::DEFAULT_IMAGE_COLOR_,
                    LISTBOX_HIGHLIGHT_COLOR_));

        EntityAdd(statusBoxUPtr_.get());

        // command box
        const float COMMAND_REGION_HORIZ_SPACER(25.0f);
        const float COMMAND_REGION_VERT_SPACER(25.0f);

        const float COMMAND_REGION_LEFT(
            STATUS_REGION_LEFT + STATUS_REGION_WIDTH + COMMAND_REGION_HORIZ_SPACER
            + STATUS_REGION_SLIDERBAR_WIDTH);

        const float COMMAND_REGION_TOP(
            COMBAT_REGION_TOP + COMBAT_REGION_HEIGHT + COMMAND_REGION_VERT_SPACER);

        const float COMMAND_REGION_WIDTH(
            ((((SCREEN_WIDTH_ - COMMAND_REGION_HORIZ_SPACER) - COMBAT_REGION_MARGIN)
              - STATUS_REGION_WIDTH)
             - 15.0f)
            - STATUS_REGION_SLIDERBAR_WIDTH);

        const float COMMAND_REGION_HEIGHT(STATUS_REGION_HEIGHT);

        const sf::FloatRect COMMAND_REGION(
            COMMAND_REGION_LEFT, COMMAND_REGION_TOP, COMMAND_REGION_WIDTH, COMMAND_REGION_HEIGHT);

        sfml_util::gui::BoxEntityInfo commandBoxInfo;

        commandBoxInfo.SetupImage(
            sfml_util::CachedTexture(
                "media-images-backgrounds-tile-darkknot",
                sfml_util::ImageOpt::Default | sfml_util::ImageOpt::Repeated),
            sfml_util::ScreenRatioToPixelsHoriz(0.06f));

        commandBoxInfo.SetupBorder(true);

        commandBoxUPtr_ = std::make_unique<sfml_util::gui::BoxEntity>(
            "CombatStage'sCommand", COMMAND_REGION, commandBoxInfo);

        // turn box
        turnBoxRegion_ = sf::FloatRect(
            STATUS_REGION_LEFT, STATUS_REGION_TOP, STATUS_REGION_WIDTH, STATUS_REGION_HEIGHT);

        sfml_util::gui::BoxEntityInfo turnBoxInfo;

        turnBoxInfo.SetupImage(
            sfml_util::CachedTexture(
                "media-images-backgrounds-tile-darkknot",
                sfml_util::ImageOpt::Default | sfml_util::ImageOpt::Repeated),
            sfml_util::ScreenRatioToPixelsHoriz(0.06f));

        turnBoxInfo.SetupBorder(true);

        turnBoxUPtr_ = std::make_unique<sfml_util::gui::BoxEntity>(
            "CombatStage'sTurnBox", turnBoxRegion_, turnBoxInfo);

        // turnbox title text region
        const sfml_util::gui::TextInfo TURNBOXTITLE_TEXT_INFO(
            " ",
            sfml_util::FontManager::Instance()->GetFont(sfml_util::GuiFont::DefaultBoldFlavor),
            sfml_util::FontManager::Instance()->Size_Largeish(),
            sfml_util::defaults::Light,
            sfml_util::Justified::Center);

        sf::FloatRect turnBoxTitleTextRegion(turnBoxRegion_);
        turnBoxTitleTextRegion.height = 0.0f;

        titleTBoxTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "TurnBox'sTitle", TURNBOXTITLE_TEXT_INFO, turnBoxTitleTextRegion);

        const sfml_util::gui::TextInfo TURNBOXINFO_TEXT_INFO(
            " ",
            sfml_util::FontManager::Instance()->GetFont(sfml_util::GuiFont::System),
            sfml_util::FontManager::Instance()->Size_Smallish(),
            sfml_util::defaults::GrayLight,
            sfml_util::Justified::Left);

        sf::FloatRect turnBoxInfoTextRegion(turnBoxRegion_);

        infoTBoxTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "CombatStage'sTurnInfo", TURNBOXINFO_TEXT_INFO, turnBoxInfoTextRegion);

        const sfml_util::gui::TextInfo TURNBOXENEMYACTION_TEXT_INFO(
            " ",
            sfml_util::FontManager::Instance()->GetFont(sfml_util::GuiFont::System),
            sfml_util::FontManager::Instance()->Size_Normal(),
            sfml_util::defaults::Light,
            sfml_util::Justified::Center);

        sf::FloatRect turnBoxEnemyActionTextRegion(turnBoxRegion_);

        // the enemy turn action text can run long and thus needs to be constrained
        auto const PAD { 40.0f };
        turnBoxEnemyActionTextRegion.left += PAD;
        turnBoxEnemyActionTextRegion.width -= PAD * 2.0f;

        enemyActionTBoxRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "CombatStage'sTurnEnemyAction",
            TURNBOXENEMYACTION_TEXT_INFO,
            turnBoxEnemyActionTextRegion);

        const sfml_util::gui::TextInfo TURNBOXENEMYCONDS_TEXT_INFO(
            " ",
            sfml_util::FontManager::Instance()->GetFont(sfml_util::GuiFont::System),
            sfml_util::FontManager::Instance()->Size_Smallish(),
            sfml_util::defaults::GrayLight,
            sfml_util::Justified::Center);

        sf::FloatRect turnBoxEnemyCondsTextRegion(turnBoxRegion_);
        enemyCondsTBoxRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "CombatStage'sTurnEnemyConds",
            TURNBOXENEMYCONDS_TEXT_INFO,
            turnBoxEnemyCondsTextRegion);

        sfml_util::gui::TextInfo turnBoxWeaponTextInfo(
            " ",
            sfml_util::FontManager::Instance()->GetFont(sfml_util::GuiFont::System),
            sfml_util::FontManager::Instance()->Size_Smallish(),
            sfml_util::defaults::GrayLight,
            sfml_util::Justified::Center);

        weaponTBoxTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "TurnBox'sWeapon", turnBoxWeaponTextInfo, turnBoxTitleTextRegion);

        sfml_util::gui::TextInfo turnBoxArmorTextInfo(turnBoxWeaponTextInfo);

        armorTBoxTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "TurnBox'sArmor", turnBoxWeaponTextInfo, turnBoxTitleTextRegion);

        const sfml_util::gui::TextInfo TESTING_TEXT_INFO(
            " ",
            sfml_util::FontManager::Instance()->GetFont(sfml_util::GuiFont::System),
            sfml_util::FontManager::Instance()->Size_Smallish(),
            sfml_util::defaults::GrayLight,
            sfml_util::Justified::Left);

        sf::FloatRect testingTextRegion(turnBoxRegion_);
        testingTextRegion.left += testingTextRegion.width - 50.0f;
        testingTextRegion.top += (testingTextRegion.height - 400.0f);

        testingTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "CombatStage'sTesting", TESTING_TEXT_INFO, testingTextRegion);

        MakeButton(
            attackTBoxButtonUPtr_, "(A)ttack", combatText_.TBOX_BUTTON_MOUSEHOVER_TEXT_ATTACK_);
        MakeButton(fightTBoxButtonUPtr_, "(F)ight", combatText_.TBOX_BUTTON_MOUSEHOVER_TEXT_FIGHT_);
        MakeButton(castTBoxButtonUPtr_, "(S)pell", combatText_.TBOX_BUTTON_MOUSEHOVER_TEXT_CAST_);
        MakeButton(
            advanceTBoxButtonUPtr_, "A(d)vance", combatText_.TBOX_BUTTON_MOUSEHOVER_TEXT_ADVANCE_);
        MakeButton(
            retreatTBoxButtonUPtr_, "R(e)treat", combatText_.TBOX_BUTTON_MOUSEHOVER_TEXT_RETREAT_);
        MakeButton(blockTBoxButtonUPtr_, "(B)lock", combatText_.TBOX_BUTTON_MOUSEHOVER_TEXT_BLOCK_);
        MakeButton(skipTBoxButtonUPtr_, "S(k)ip", combatText_.TBOX_BUTTON_MOUSEHOVER_TEXT_SKIP_);
        MakeButton(flyTBoxButtonUPtr_, "Fl(y)", combatText_.TBOX_BUTTON_MOUSEHOVER_TEXT_FLY_);
        MakeButton(landTBoxButtonUPtr_, "(L)and", combatText_.TBOX_BUTTON_MOUSEHOVER_TEXT_LAND_);
        MakeButton(roarTBoxButtonUPtr_, "(R)oar", combatText_.TBOX_BUTTON_MOUSEHOVER_TEXT_ROAR_);
        MakeButton(
            pounceTBoxButtonUPtr_, "(P)ounce", combatText_.TBOX_BUTTON_MOUSEHOVER_TEXT_POUNCE_);
        MakeButton(runTBoxButtonUPtr_, "R(u)n", combatText_.TBOX_BUTTON_MOUSEHOVER_TEXT_RUN_);

        // settings button (gears symbol)
        const float COMMAND_REGION_PAD(10.0f);

        const sf::Vector2f GEAR_IMAGE_SIZE_V { sfml_util::ScreenRatioToPixelsHoriz(0.052f),
                                               sfml_util::ScreenRatioToPixelsVert(0.0853f) };

        const sf::Vector2f GEAR_IMAGE_POS_V { ((COMMAND_REGION_LEFT + COMMAND_REGION_WIDTH)
                                               - GEAR_IMAGE_SIZE_V.x)
                                                  - COMMAND_REGION_PAD,
                                              COMMAND_REGION_TOP + COMMAND_REGION_PAD };

        const sf::FloatRect GEAR_IMAGE_REGION(GEAR_IMAGE_POS_V, GEAR_IMAGE_SIZE_V);

        const sfml_util::gui::EntityImageInfo GEAR_IMAGE_INFO_UP(
            sfml_util::CachedTexture("media-images-buttons-gui-gears-normal"), GEAR_IMAGE_REGION);

        const sfml_util::gui::EntityImageInfo GEAR_IMAGE_INFO_OVER(
            sfml_util::CachedTexture("media-images-buttons-gui-gears-lit"), GEAR_IMAGE_REGION);

        settingsButtonUPtr_ = std::make_unique<sfml_util::gui::ImageTextEntity>(
            "CombatStage'sSettingsGears",
            sfml_util::gui::MouseImageInfo(
                true, GEAR_IMAGE_INFO_UP, sfml_util::gui::EntityImageInfo(), GEAR_IMAGE_INFO_OVER),
            sfml_util::gui::MouseTextInfo(),
            sfml_util::gui::ImageTextEntity::Callback_t::IHandlerPtr_t(this),
            sfml_util::gui::ImageTextEntity::MouseStateSync::Image);

        EntityAdd(settingsButtonUPtr_.get());

        // position turn buttons
        auto const LEFT_ALIGN_PAD { COMMAND_REGION.width * 0.25f };
        auto const COLUMN_1_LEFT { (COMMAND_REGION.left + (LEFT_ALIGN_PAD * 1.0f)) - 15.0f };
        auto const COLUMN_2_LEFT { COMMAND_REGION.left + (LEFT_ALIGN_PAD * 2.0f) };
        auto const COLUMN_3_LEFT { (COMMAND_REGION.left + (LEFT_ALIGN_PAD * 3.0f)) + 15.0f };

        auto const COLUMN_TOP { settingsButtonUPtr_->GetEntityPos().y
                                + settingsButtonUPtr_->GetEntityRegion().height
                                - sfml_util::MapByRes(10.0f, 75.0f) };

        auto const COLUMN_VERT_PAD { attackTBoxButtonUPtr_->GetEntityRegion().height / 1.5f };

        attackTBoxButtonUPtr_->SetEntityPos(
            COLUMN_1_LEFT - (attackTBoxButtonUPtr_->GetEntityRegion().width * 0.5f),
            COLUMN_TOP + (COLUMN_VERT_PAD * 0.0f));

        fightTBoxButtonUPtr_->SetEntityPos(
            COLUMN_1_LEFT - (fightTBoxButtonUPtr_->GetEntityRegion().width * 0.5f),
            COLUMN_TOP + (COLUMN_VERT_PAD * 1.0f));

        castTBoxButtonUPtr_->SetEntityPos(
            COLUMN_1_LEFT - (castTBoxButtonUPtr_->GetEntityRegion().width * 0.5f),
            COLUMN_TOP + (COLUMN_VERT_PAD * 2.0f));

        runTBoxButtonUPtr_->SetEntityPos(
            COLUMN_1_LEFT - (runTBoxButtonUPtr_->GetEntityRegion().width * 0.5f),
            COLUMN_TOP + (COLUMN_VERT_PAD * 3.0f));

        advanceTBoxButtonUPtr_->SetEntityPos(
            COLUMN_2_LEFT - (advanceTBoxButtonUPtr_->GetEntityRegion().width * 0.5f),
            COLUMN_TOP + (COLUMN_VERT_PAD * 0.0f));

        retreatTBoxButtonUPtr_->SetEntityPos(
            COLUMN_2_LEFT - (retreatTBoxButtonUPtr_->GetEntityRegion().width * 0.5f),
            COLUMN_TOP + (COLUMN_VERT_PAD * 1.0f));

        landTBoxButtonUPtr_->SetEntityPos(
            COLUMN_2_LEFT - (landTBoxButtonUPtr_->GetEntityRegion().width * 0.5f),
            COLUMN_TOP + (COLUMN_VERT_PAD * 2.0f));

        flyTBoxButtonUPtr_->SetEntityPos(
            COLUMN_2_LEFT - (flyTBoxButtonUPtr_->GetEntityRegion().width * 0.5f),
            COLUMN_TOP + (COLUMN_VERT_PAD * 3.0f));

        pounceTBoxButtonUPtr_->SetEntityPos(
            COLUMN_3_LEFT - (pounceTBoxButtonUPtr_->GetEntityRegion().width * 0.5f),
            COLUMN_TOP + (COLUMN_VERT_PAD * 0.0f));

        roarTBoxButtonUPtr_->SetEntityPos(
            COLUMN_3_LEFT - (roarTBoxButtonUPtr_->GetEntityRegion().width * 0.5f),
            COLUMN_TOP + (COLUMN_VERT_PAD * 1.0f));

        skipTBoxButtonUPtr_->SetEntityPos(
            COLUMN_3_LEFT - (skipTBoxButtonUPtr_->GetEntityRegion().width * 0.5f),
            COLUMN_TOP + (COLUMN_VERT_PAD * 2.0f));

        blockTBoxButtonUPtr_->SetEntityPos(
            COLUMN_3_LEFT - (blockTBoxButtonUPtr_->GetEntityRegion().width * 0.5f),
            COLUMN_TOP + (COLUMN_VERT_PAD * 3.0f));

        if (restoreInfo_.HasRestored())
        {
            preTurnPhase_ = PreTurnPhase::End;
        }
        else
        {
            // TEMP TODO set to Start when done testing
            preTurnPhase_ = PreTurnPhase::End;

            combat::Encounter::Instance()->BeginCombatTasks();
        }

        // combat display
        combatDisplayStagePtr_->StageRegionSet(COMBAT_REGION);
        combatDisplayStagePtr_->Setup();

        // give control of the CombatDisplay object lifetime to the Loop class
        game::LoopManager::Instance()->AddStage(combatDisplayStagePtr_.Ptr());

        if (restoreInfo_.HasRestored())
        {
            conditionEffectsWillSkip_ = true;
        }
        else
        {
            // This is just so that restoreInfo_.Save() will save the correct initial flying state.
            // This doesn't set the flying state in Encounter, but
            // the call to restoreInfo_.Restore() below will.
            combatDisplayStagePtr_->SetInitiallyFlyingCreaturesToFlying();
            restoreInfo_.Save(combatDisplayStagePtr_);
        }

        // CombatDisplay Zoom Sliderbar
        const sfml_util::gui::TextInfo ZOOMSLIDER_LABEL_TEXT_INFO(
            "Zoom",
            sfml_util::FontManager::Instance()->GetFont(sfml_util::GuiFont::Default),
            sfml_util::FontManager::Instance()->Size_Smallish(),
            sfml_util::defaults::Light,
            sfml_util::Justified::Left);

        const sf::FloatRect ZOOMSLIDER_LABEL_RECT(
            0.0f, COMMAND_REGION_TOP + COMMAND_REGION_PAD, 0.0f, 0.0f);

        zoomLabelTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "ZoomSlider's", ZOOMSLIDER_LABEL_TEXT_INFO, ZOOMSLIDER_LABEL_RECT);

        zoomSliderBarUPtr_ = std::make_unique<sfml_util::gui::SliderBar>(
            "CombatStageZoom",
            COMMAND_REGION_LEFT + COMMAND_REGION_PAD,
            zoomLabelTextRegionUPtr_->GetEntityRegion().top
                + zoomLabelTextRegionUPtr_->GetEntityRegion().height,
            (settingsButtonUPtr_->GetEntityPos().x - COMMAND_REGION_LEFT)
                - (COMMAND_REGION_PAD * 2.0f),
            sfml_util::gui::SliderStyle(sfml_util::Orientation::Horiz),
            sfml_util::gui::SliderBar::Callback_t::IHandlerPtr_t(this));

        zoomSliderBarUPtr_->PositionRatio(1.0f);
        zoomLabelTextRegionUPtr_->SetEntityPos(
            (zoomSliderBarUPtr_->GetEntityPos().x
             + (zoomSliderBarUPtr_->GetEntityRegion().width * 0.5f))
                - (zoomLabelTextRegionUPtr_->GetEntityRegion().width * 0.5f),
            zoomLabelTextRegionUPtr_->GetEntityPos().y);

        EntityAdd(zoomLabelTextRegionUPtr_.get());
        EntityAdd(zoomSliderBarUPtr_.get());

        SetupTurnBoxButtons(boost::none, true);
        MoveTurnBoxObjectsOffScreen();
        restoreInfo_.Restore(combatDisplayStagePtr_);
        SetUserActionAllowed(false);

        if (WILL_ADVANCE_TURN_)
        {
            combat::Encounter::Instance()->IncrementTurn();
        }
    }

    void CombatStage::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        target.draw(*commandBoxUPtr_, STATES);
        Stage::Draw(target, STATES);

        // statusBoxUPtr_->draw(target, STATES);

        if (((turnPhase_ >= TurnPhase::Determine) && (turnPhase_ <= TurnPhase::PostPerformPause))
            || (IsNonPlayerCharacterTurnValid()
                && (TurnPhase::PostCenterAndZoomInPause == turnPhase_)))
        {
            turnBoxUPtr_->draw(target, STATES);

            titleTBoxTextRegionUPtr_->draw(target, STATES);
            weaponTBoxTextRegionUPtr_->draw(target, STATES);
            armorTBoxTextRegionUPtr_->draw(target, STATES);
            infoTBoxTextRegionUPtr_->draw(target, STATES);
            enemyActionTBoxRegionUPtr_->draw(target, STATES);
            enemyCondsTBoxRegionUPtr_->draw(target, STATES);

            attackTBoxButtonUPtr_->draw(target, STATES);
            fightTBoxButtonUPtr_->draw(target, STATES);
            castTBoxButtonUPtr_->draw(target, STATES);
            runTBoxButtonUPtr_->draw(target, STATES);

            advanceTBoxButtonUPtr_->draw(target, STATES);
            retreatTBoxButtonUPtr_->draw(target, STATES);
            flyTBoxButtonUPtr_->draw(target, STATES);
            landTBoxButtonUPtr_->draw(target, STATES);

            blockTBoxButtonUPtr_->draw(target, STATES);
            skipTBoxButtonUPtr_->draw(target, STATES);
            roarTBoxButtonUPtr_->draw(target, STATES);
            pounceTBoxButtonUPtr_->draw(target, STATES);
        }

        if (false == hasCombatEnded_)
        {
            combatAnimationUPtr_->Draw(target, STATES);
        }

        // DrawHoverText(target, STATES);
        // testingTextRegionUPtr_->draw(target, STATES);
    }

    void CombatStage::UpdateTime(const float ELAPSED_TIME_SEC)
    {
        Stage::UpdateTime(ELAPSED_TIME_SEC);

        if (hasCombatEnded_)
        {
            return;
        }

        combatAnimationUPtr_->UpdateTime(ELAPSED_TIME_SEC);

        if (TurnPhase::ConditionEffectPause == turnPhase_)
        {
            if (enemyActionTBoxRegionUPtr_)
            {
                enemyActionTBoxRegionUPtr_->SetEntityColorFgBoth(
                    goldTextColorShaker_.Update(ELAPSED_TIME_SEC));
            }
        }

        if (willRedColorShakeWeaponText_)
        {
            weaponTBoxTextRegionUPtr_->SetEntityColorFgBoth(
                redTextColorShaker_.Update(ELAPSED_TIME_SEC));
        }

        if (IsNonPlayerCharacterTurnValid() && (TurnPhase::PostCenterAndZoomInPause == turnPhase_))
        {
            titleTBoxTextRegionUPtr_->SetEntityColorFgBoth(
                goldTextColorShaker_.Update(ELAPSED_TIME_SEC));
        }

        if (IsNonPlayerCharacterTurnValid() && enemyActionTBoxRegionUPtr_
            && ((TurnPhase::CenterAndZoomOut == turnPhase_)
                || (TurnPhase::PostCenterAndZoomOutPause == turnPhase_)))
        {
            enemyActionTBoxRegionUPtr_->SetEntityColorFgBoth(
                goldTextColorShaker_.Update(ELAPSED_TIME_SEC));
        }

        // single-click triggers summary view;
        if (clickTimerSec_ > 0.0f)
        {
            clickTimerSec_ -= ELAPSED_TIME_SEC;

            if (clickTimerSec_ < 0.0f)
            {
                combatDisplayStagePtr_->StartSummaryView(clickPosV_);
            }
        }

        // allow progress of the pause timer even if IsStatusMessageAnimating(),
        // because they work together
        if (IsPaused() || isPauseCanceled_)
        {
            pauseElapsedSec_ += ELAPSED_TIME_SEC;
            if ((pauseElapsedSec_ > pauseDurationSec_) || isPauseCanceled_)
            {
                isPauseCanceled_ = false;
                EndPause();
            }
        }

        if (TurnPhase::DeathAnim == turnPhase_)
        {
            combatAnimationUPtr_->DeathAnimUpdate(slider_.Update(ELAPSED_TIME_SEC));
            if (slider_.IsDone())
            {
                SetTurnPhase(TurnPhase::RepositionAnim);
                combatAnimationUPtr_->DeathAnimStop(combatDisplayStagePtr_);
                PositionSlideStartTasks(turnCreaturePtrOpt_.value());
            }
            return;
        }

        if ((TurnPhase::PerformAnim == turnPhase_) && (AnimPhase::Run == animPhase_))
        {
            combatAnimationUPtr_->RunAnimUpdate(slider_.Update(ELAPSED_TIME_SEC));
            if (slider_.IsDone())
            {
                auto const CREATURE_PTR { combatAnimationUPtr_->RunAnimStop() };

                combatDisplayStagePtr_->HandleCombatNodeElimination(CREATURE_PTR);
                combat::Encounter::Instance()->Runaway(CREATURE_PTR);

                SetAnimPhase(AnimPhase::NotAnimating);
                SetTurnPhase(TurnPhase::RepositionAnim);
                isRepositionAnimAfterRun_ = true;
                PositionSlideStartTasks(turnCreaturePtrOpt_.value());
            }
            return;
        }

        if ((TurnPhase::PerformAnim == turnPhase_) && (AnimPhase::ProjectileShoot == animPhase_))
        {
            combatAnimationUPtr_->ProjectileShootAnimUpdate(
                slider_.Update(ELAPSED_TIME_SEC), combatDisplayStagePtr_->BattlefieldRect());

            if (slider_.IsDone())
            {
                HandleApplyDamageTasks();
                combatAnimationUPtr_->ProjectileShootAnimStop();
                SetTurnPhase(TurnPhase::PerformReport);
                SetAnimPhase(AnimPhase::NotAnimating);
                SetupTurnBox();
                StartPause(PERFORM_PRE_REPORT_PAUSE_SEC_, "PerformPreReport (PostProjAnim)");
            }
            return;
        }

        if ((TurnPhase::PerformAnim == turnPhase_) && (AnimPhase::Impact == animPhase_))
        {
            combatAnimationUPtr_->ImpactAnimUpdate(slider_.Update(ELAPSED_TIME_SEC));
            if (slider_.IsDone())
            {
                combatAnimationUPtr_->ImpactAnimStop();
                HandleApplyDamageTasks();
                SetAnimPhase(AnimPhase::PostImpactPause);
                StartPause(POST_IMPACT_ANIM_PAUSE_SEC_, "PostImpact");
                SetupTurnBox();
            }
            return;
        }

        if ((TurnPhase::PerformAnim == turnPhase_) && (AnimPhase::Spell == animPhase_))
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (!!spellBeingCastPtrOpt_),
                "stage::CombatStage::UpdateTime() turnPhase_=PerformAnim and animPhase_=Spell but "
                "spellBeingCastPtrOpt_ was uninitialized.");

            if (combatAnimationUPtr_->SparkleAnimIsDone() == false)
            {
                combatAnimationUPtr_->SparkleAnimUpdate(ELAPSED_TIME_SEC);
            }

            if (combatAnimationUPtr_->SpellAnimUpdate(
                    spellBeingCastPtrOpt_.value(), ELAPSED_TIME_SEC))
            {
                combatAnimationUPtr_->SparkleAnimStop();
                combatAnimationUPtr_->SpellAnimStop(spellBeingCastPtrOpt_.value());
                HandleApplyDamageTasks();
                SetAnimPhase(AnimPhase::PostSpellPause);
                StartPause(POST_SPELL_ANIM_PAUSE_SEC_, "PostSpell");
            }
        }

        if ((TurnPhase::PerformAnim == turnPhase_) && (AnimPhase::Song == animPhase_))
        {
            if (false == isSongAnim1Done_)
            {
                if (combatAnimationUPtr_->SongAnimUpdate(ELAPSED_TIME_SEC))
                {
                    isSongAnim1Done_ = true;
                }
            }

            if (false == isSongAnim2Done_)
            {
                if (combatAnimationUPtr_->SparkleAnimUpdate(ELAPSED_TIME_SEC))
                {
                    isSongAnim2Done_ = true;
                }
            }

            if (isSongAnim1Done_ && isSongAnim2Done_)
            {
                isSongAnim1Done_ = false;
                isSongAnim2Done_ = false;

                HandleApplyDamageTasks();
                SetAnimPhase(AnimPhase::PostSongPause);

                // use the same pause length that spells use
                StartPause(POST_SPELL_ANIM_PAUSE_SEC_, "PostSong");
            }
        }

        if ((TurnPhase::PerformAnim == turnPhase_) && (AnimPhase::Roar == animPhase_)
            && (TurnActionPhase::Roar == turnActionPhase_))
        {
            slider_.Update(ELAPSED_TIME_SEC);
            if (slider_.IsDone())
            {
                combatAnimationUPtr_->ShakeAnimStop(boost::none);
                PerformRoarEffects();
                SetAnimPhase(AnimPhase::PostRoarPause);

                // use the same pause length that spells use
                StartPause(POST_SPELL_ANIM_PAUSE_SEC_, "PostRoar");
            }
            return;
        }

        if ((TurnPhase::PerformAnim == turnPhase_) && (AnimPhase::MoveToward == animPhase_))
        {
            combatAnimationUPtr_->MeleeMoveTowardAnimUpdate(slider_.Update(ELAPSED_TIME_SEC));
            if (slider_.IsDone())
            {
                combatAnimationUPtr_->MeleeMoveTowardAnimStop();
                SetAnimPhase(AnimPhase::PostMoveTowardPause);
                StartPause(POST_MELEEMOVE_ANIM_PAUSE_SEC_, "PostMoveToward");
                SetupTurnBox();
            }
            return;
        }

        if ((TurnPhase::PerformAnim == turnPhase_) && (AnimPhase::MoveBack == animPhase_))
        {
            combatAnimationUPtr_->MeleeMoveBackAnimUpdate(slider_.Update(ELAPSED_TIME_SEC));
            if (slider_.IsDone())
            {
                combatAnimationUPtr_->MeleeMoveBackAnimStop();
                SetAnimPhase(AnimPhase::FinalPause);
                StartPause(POST_MELEEMOVE_ANIM_PAUSE_SEC_, "Final");
                SetupTurnBox();
            }
            return;
        }

        if (TurnPhase::CenterAndZoomIn == turnPhase_)
        {
            auto const SLIDER_POS { slider_.Update(ELAPSED_TIME_SEC) };

            zoomSliderBarUPtr_->PositionRatio(
                zoomSliderOrigPos_ + (SLIDER_POS * (1.0f - zoomSliderOrigPos_)));

            combatAnimationUPtr_->CenteringUpdate(SLIDER_POS, combatDisplayStagePtr_);

            if (slider_.IsDone())
            {
                StartTurn_Step2(turnCreaturePtrOpt_.value());
            }
            return;
        }

        if ((TurnPhase::RepositionAnim == turnPhase_)
            || ((TurnPhase::PerformAnim == turnPhase_)
                && (AnimPhase::AdvanceOrRetreat == animPhase_)
                && ((TurnActionPhase::Advance == turnActionPhase_)
                    || (TurnActionPhase::Retreat == turnActionPhase_))))
        {
            combatAnimationUPtr_->RepositionAnimUpdate(
                slider_.Update(ELAPSED_TIME_SEC), combatDisplayStagePtr_);

            if (slider_.IsDone())
            {
                combatAnimationUPtr_->RepositionAnimStop();
                SetTurnPhase(TurnPhase::PostTurnPause);
                StartPause(POST_TURN_PAUSE_SEC_, "PostTurn");
            }
            return;
        }

        if (willClrShkInitStatusMsg_ || (TurnPhase::StatusAnim == turnPhase_))
        {
            statusMsgAnimTimerSec_ += ELAPSED_TIME_SEC;
            // statusBoxUPtr_->SetHighlightColor(statusMsgAnimColorShaker_.Update(ELAPSED_TIME_SEC));

            if (statusMsgAnimTimerSec_ > STATUSMSG_ANIM_PAUSE_SEC_)
            {
                // statusBoxUPtr_->SetHighlightColor(LISTBOX_HIGHLIGHT_COLOR_);
                combatDisplayStagePtr_->SetIsStatusMessageAnimating(false);

                if (willClrShkInitStatusMsg_)
                {
                    willClrShkInitStatusMsg_ = false;
                }
                else
                {
                    SetTurnPhase(TurnPhase::PostTurnPause);
                    StartPause(POST_TURN_PAUSE_SEC_, "PostTurn");
                }
            }
            return;
        }

        if (TurnPhase::CenterAndZoomOut == turnPhase_)
        {
            auto const SLIDER_POS { slider_.Update(ELAPSED_TIME_SEC) };

            if (combatAnimationUPtr_->CenteringUpdate(SLIDER_POS, combatDisplayStagePtr_))
            {
                auto const ZOOM_CURR_VAL(1.0f - SLIDER_POS);
                zoomSliderBarUPtr_->PositionRatio(ZOOM_CURR_VAL);
            }

            if (slider_.IsDone())
            {
                combatAnimationUPtr_->CenteringStop();
                SetTurnPhase(TurnPhase::PostCenterAndZoomOutPause);

                if (isShortPostZoomOutPause_)
                {
                    isShortPostZoomOutPause_ = false;
                    StartPause(PAUSE_SHORTER_SEC_, "PostZoomOut");
                }
                else
                {
                    StartPause(POST_ZOOM_TURN_PAUSE_SEC_, "PostZoomOut");
                }

                SetupTurnBox();
            }
            return;
        }

        if (PreTurnPhase::PanToCenter == preTurnPhase_)
        {
            // the initial pan seems to take extra time getting started so speed it up here
            auto sliderPosAdj { 0.25f + slider_.Update(ELAPSED_TIME_SEC) };
            if (sliderPosAdj > 1.0f)
            {
                sliderPosAdj = 1.0f;
            }

            combatAnimationUPtr_->CenteringUpdate(sliderPosAdj, combatDisplayStagePtr_);

            if (slider_.IsDone())
            {
                combatAnimationUPtr_->CenteringStop();
                SetPreTurnPhase(PreTurnPhase::PostPanPause);
                StartPause(POST_PAN_PAUSE_SEC_, "PostPan");
            }
            return;
        }

        if (PreTurnPhase::ZoomOut == preTurnPhase_)
        {
            auto const SLIDER_POS { slider_.Update(ELAPSED_TIME_SEC) };
            auto const ZOOM_CURR_VAL(1.0f - SLIDER_POS);
            zoomSliderBarUPtr_->PositionRatio(ZOOM_CURR_VAL);

            if (slider_.IsDone())
            {
                SetPreTurnPhase(PreTurnPhase::PostZoomOutPause);
                StartPause(POST_ZOOMOUT_PAUSE_SEC_, "PostZOut");
                AppendInitialStatus();
            }
            return;
        }

        // handle creature turn start hook, catches the start of a new turn
        if ((combat::Encounter::Instance()->HasStarted()) && (IsPaused() == false)
            && !turnCreaturePtrOpt_ && (TurnPhase::NotATurn == turnPhase_)
            && (PreTurnPhase::End == preTurnPhase_)
            && (game::LoopManager::Instance()->IsFading() == false))
        {
            if ((HandleWin() == false) && (HandleLose() == false))
            {
                StartTurn_Step1();
            }
            return;
        }

        // initial hook for taking action before the first turn (pre-turn logic)
        if ((zoomSliderBarUPtr_) && (game::LoopManager::Instance()->IsFading() == false)
            && (TurnPhase::NotATurn == turnPhase_) && (PreTurnPhase::Start == preTurnPhase_))
        {
            SetPreTurnPhase(PreTurnPhase::PanToCenter);
            slider_.Reset(ANIM_INITIAL_CENTERING_SLIDER_SPEED_);

            combatAnimationUPtr_->CenteringStart(
                combatDisplayStagePtr_->GetCenterOfAllNodes(), combatDisplayStagePtr_);

            return;
        }
    }

    void CombatStage::UpdateMouseDown(const sf::Vector2f & MOUSE_POS_V)
    {
        Stage::UpdateMouseDown(MOUSE_POS_V);

        // cancel summary view if visible or even just starting
        if (combatDisplayStagePtr_->GetIsSummaryViewInProgress())
        {
            combatDisplayStagePtr_->CancelSummaryViewAndStartTransitionBack();
            isMouseHeldDown_ = false;
        }
        else if ((TurnPhase::Determine != turnPhase_) && (TurnPhase::TargetSelect != turnPhase_))
        {
            isMouseHeldDown_ = false;
        }
    }

    const sfml_util::gui::IEntityPtrOpt_t
        CombatStage::UpdateMouseUp(const sf::Vector2f & MOUSE_POS_V)
    {
        auto const WAS_MOUSE_HELD_DOWN_AND_MOVING { isMouseHeldDownAndMoving_ };

        auto const GUI_ENTITY_WITH_FOCUS_PTR_OPT { Stage::UpdateMouseUp(MOUSE_POS_V) };

        if (WAS_MOUSE_HELD_DOWN_AND_MOVING)
        {
            return GUI_ENTITY_WITH_FOCUS_PTR_OPT;
        }

        auto const CREATURE_AT_POS_PTR_OPT { combatDisplayStagePtr_->GetCreatureAtPosPtrOpt(
            MOUSE_POS_V) };

        if (!CREATURE_AT_POS_PTR_OPT)
        {
            HandleMiscCancelTasks();
            return GUI_ENTITY_WITH_FOCUS_PTR_OPT;
        }

        auto const CREATURE_AT_POS_PTR { CREATURE_AT_POS_PTR_OPT.value() };

        if ((TurnPhase::Determine == turnPhase_)
            && (combatDisplayStagePtr_->GetIsSummaryViewInProgress() == false))
        {
            if (clickTimerSec_ < 0.0f)
            {
                clickPosV_ = MOUSE_POS_V;
                clickTimerSec_ = DOUBLE_CLICK_WINDOW_SEC_;
            }
            else if (CREATURE_AT_POS_PTR->IsPlayerCharacter())
            {
                clickTimerSec_ = -1.0f; // any negative value will work here

                restoreInfo_.PrepareForStageChange(combatDisplayStagePtr_);

                game::LoopManager::Instance()->TransitionTo_Inventory(
                    turnCreaturePtrOpt_.value(), CREATURE_AT_POS_PTR, game::Phase::Combat);

                return GUI_ENTITY_WITH_FOCUS_PTR_OPT;
            }
        }

        if (TurnPhase::TargetSelect != turnPhase_)
        {
            return GUI_ENTITY_WITH_FOCUS_PTR_OPT;
        }

        if (!spellBeingCastPtrOpt_)
        {
            if (CREATURE_AT_POS_PTR->IsPlayerCharacter())
            {
                QuickSmallPopup(
                    std::string("That is one of your player characters,")
                        .append("who cannot be attacked.  Click on an enemy creature instead."),
                    false,
                    true);
            }
            else
            {
                if (combatDisplayStagePtr_->IsCreatureAPossibleFightTarget(
                        turnCreaturePtrOpt_.value(), CREATURE_AT_POS_PTR))
                {
                    combatAnimationUPtr_->SelectAnimStart(
                        combatDisplayStagePtr_->GetCombatNodeForCreature(CREATURE_AT_POS_PTR));
                    HandleAttackTasks(CREATURE_AT_POS_PTR);
                }
                else
                {
                    QuickSmallPopup(
                        std::string("That creature is not close enough to fight.")
                            .append("  Try clicking on another creature."),
                        false,
                        true);
                }
            }
        }
        else if (spellBeingCastPtrOpt_.value()->Target() == combat::TargetType::SingleOpponent)
        {
            if (CREATURE_AT_POS_PTR->IsPlayerCharacter())
            {
                QuickSmallPopup(
                    std::string("That is one of your player characters,")
                        .append(" not an enemy.  Click on an enemy creature instead."),
                    false,
                    true);
            }
            else
            {
                combatAnimationUPtr_->SelectAnimStart(
                    combatDisplayStagePtr_->GetCombatNodeForCreature(CREATURE_AT_POS_PTR));
                HandleCast_Step3_PerformOnTargets(creature::CreaturePVec_t { CREATURE_AT_POS_PTR });
            }
        }
        else if (spellBeingCastPtrOpt_.value()->Target() == combat::TargetType::SingleCompanion)
        {
            if (CREATURE_AT_POS_PTR->IsPlayerCharacter() == false)
            {
                QuickSmallPopup(
                    std::string("That is and enemy, not one of your character.")
                        .append("  Click on one of your characters instead."),
                    false,
                    true);
            }
            else
            {
                combatAnimationUPtr_->SelectAnimStart(
                    combatDisplayStagePtr_->GetCombatNodeForCreature(CREATURE_AT_POS_PTR));
                HandleCast_Step3_PerformOnTargets(creature::CreaturePVec_t { CREATURE_AT_POS_PTR });
            }
        }

        return GUI_ENTITY_WITH_FOCUS_PTR_OPT;
    }

    bool CombatStage::KeyRelease(const sf::Event::KeyEvent & KE)
    {
        if (HandleMiscCancelTasks())
        {
            return true;
        }

        // esc can cancel fight target selection phase
        if ((TurnPhase::TargetSelect == turnPhase_)
            && ((KE.code == sf::Keyboard::Escape) || (KE.code == sf::Keyboard::X)))
        {
            SetTurnPhase(TurnPhase::Determine);
            SetUserActionAllowed(true);
            SetupTurnBox();
            return true;
        }

        if (IsPlayerCharacterTurnValid() && (TurnPhase::Determine == turnPhase_))
        {
            if (KE.code == sf::Keyboard::Return)
            {
                auto const MOUSEOVER_ATTACK_STR(combatText_.MouseOverTextAttackStr(
                    turnCreaturePtrOpt_.value(), combatDisplayStagePtr_));

                if (MOUSEOVER_ATTACK_STR == combatText_.TBOX_BUTTON_MOUSEHOVER_TEXT_ATTACK_)
                {
                    return HandleAttack();
                }
                else
                {
                    auto const MOUSEOVER_STR(combatText_.MouseOverTextBlockStr(
                        turnCreaturePtrOpt_.value(), combatDisplayStagePtr_));

                    if (MOUSEOVER_STR == combatText_.TBOX_BUTTON_MOUSEHOVER_TEXT_BLOCK_)
                    {
                        return HandleBlock();
                    }
                    else
                    {
                        return HandleSkip();
                    }
                }
            }

            if (KE.code == sf::Keyboard::A)
            {
                return HandleAttack();
            }

            if (KE.code == sf::Keyboard::F)
            {
                return HandleFight();
            }

            if ((KE.code == sf::Keyboard::S) && turnCreaturePtrOpt_)
            {
                auto const TURN_CREATURE_PTR { turnCreaturePtrOpt_.value() };

                if (TURN_CREATURE_PTR->Role() == creature::role::Bard)
                {
                    return HandleSong_Step1_ValidatePlayAndSelectSong(TURN_CREATURE_PTR);
                }
                else
                {
                    return HandleCast_Step1_ValidateCastAndSelectSpell(TURN_CREATURE_PTR);
                }
            }

            if ((KE.code == sf::Keyboard::Right) || (KE.code == sf::Keyboard::D))
            {
                return HandleAdvance();
            }

            if ((KE.code == sf::Keyboard::Left) || (KE.code == sf::Keyboard::E))
            {
                return HandleRetreat();
            }

            if (KE.code == sf::Keyboard::Y)
            {
                return HandleFly();
            }

            if (KE.code == sf::Keyboard::L)
            {
                return HandleLand();
            }

            if (KE.code == sf::Keyboard::R)
            {
                return HandleRoar();
            }

            if (KE.code == sf::Keyboard::C)
            {
                return HandleWeaponChange();
            }

            if (KE.code == sf::Keyboard::U)
            {
                return HandleRun();
            }

            if (turnCreaturePtrOpt_)
            {
                auto const TURN_CREATURE_PTR { turnCreaturePtrOpt_.value() };

                if (KE.code == sf::Keyboard::B)
                {
                    return HandleBlock();
                }

                if (KE.code == sf::Keyboard::K)
                {
                    return HandleSkip();
                }

                if (KE.code == sf::Keyboard::I)
                {
                    if (TURN_CREATURE_PTR->IsPlayerCharacter())
                    {
                        restoreInfo_.PrepareForStageChange(combatDisplayStagePtr_);

                        game::LoopManager::Instance()->TransitionTo_Inventory(
                            TURN_CREATURE_PTR, TURN_CREATURE_PTR, game::Phase::Combat);
                    }
                }

                if (KE.code == sf::Keyboard::P)
                {
                    return HandlePounce(combat::Encounter::Instance()
                                            ->GetTurnInfoCopy(TURN_CREATURE_PTR)
                                            .GetIsFlying());
                }
            }
        }

        return Stage::KeyRelease(KE);
    }

    bool CombatStage::IsPlayerCharacterTurnValid() const
    {
        return (
            (TurnPhase::NotATurn != turnPhase_) && turnCreaturePtrOpt_
            && turnCreaturePtrOpt_.value()->IsPlayerCharacter());
    }

    bool CombatStage::IsNonPlayerCharacterTurnValid() const
    {
        return (
            (TurnPhase::NotATurn != turnPhase_) && turnCreaturePtrOpt_
            && (turnCreaturePtrOpt_.value()->IsPlayerCharacter() == false));
    }

    void CombatStage::AppendInitialStatus()
    {
        std::ostringstream ss;
        ss << combatText_.InitialCombatStatusMessagePrefix() << " "
           << combat::Encounter::Instance()->LivingNonPlayersSummary() << "!";

        statusBoxTextInfo_.text = ss.str();

        statusBoxUPtr_->Append(
            std::make_unique<sfml_util::gui::ListElement<sfml_util::gui::NoElement_t>>(
                boost::none, statusBoxTextInfo_, boost::none));

        statusBoxUPtr_->SelectNext();
        MoveTurnBoxObjectsOffScreen();
        statusMsgAnimColorShaker_.Reset();
        statusMsgAnimColorShaker_.Start();
        statusMsgAnimTimerSec_ = 0.0f;
        combatDisplayStagePtr_->SetIsStatusMessageAnimating(true);
        willClrShkInitStatusMsg_ = true;
    }

    void CombatStage::AppendStatusMessage(const std::string & MSG_STR, const bool WILL_ANIM)
    {
        statusBoxTextInfo_.text = MSG_STR;

        statusBoxUPtr_->Append(
            std::make_unique<sfml_util::gui::ListElement<sfml_util::gui::NoElement_t>>(
                boost::none, statusBoxTextInfo_, boost::none));

        statusBoxUPtr_->SelectNext();

        if (WILL_ANIM)
        {
            SetTurnPhase(TurnPhase::StatusAnim);
            MoveTurnBoxObjectsOffScreen();
            statusMsgAnimColorShaker_.Reset();
            statusMsgAnimColorShaker_.Start();
            statusMsgAnimTimerSec_ = 0.0f;
            combatDisplayStagePtr_->SetIsStatusMessageAnimating(true);
        }
    }

    void CombatStage::StartPause(const float DURATION_SEC, const std::string & TITLE)
    {
        pauseElapsedSec_ = 0.0f;
        pauseDurationSec_ = DURATION_SEC;
        pauseTitle_ = TITLE;
        UpdateTestingText();
    }

    void CombatStage::EndPause()
    {
        UpdateTestingText();

        // anything greater than pauseDurationSec_ will work here
        pauseElapsedSec_ = pauseDurationSec_ + 1.0f;

        if ((TurnPhase::PerformAnim == turnPhase_) && (AnimPhase::FinalPause == animPhase_))
        {
            SetAnimPhase(AnimPhase::NotAnimating);
            SetTurnPhase(TurnPhase::PerformReport);
            StartPause(PERFORM_PRE_REPORT_PAUSE_SEC_, "PerformPreReport (PostPerformAnim)");
            SetupTurnBox();
            return;
        }

        if ((TurnPhase::PerformAnim == turnPhase_) && (AnimPhase::PostImpactPause == animPhase_))
        {
            SetAnimPhase(AnimPhase::MoveBack);
            SetupTurnBox();
            combatAnimationUPtr_->MeleeMoveBackAnimStart();
            slider_.Reset(ANIM_MELEE_MOVE_SLIDER_SPEED_);
            return;
        }

        if ((TurnPhase::PerformAnim == turnPhase_)
            && ((AnimPhase::PostSpellPause == animPhase_)
                || (AnimPhase::PostSongPause == animPhase_)
                || (AnimPhase::PostRoarPause == animPhase_)))
        {
            SetAnimPhase(AnimPhase::NotAnimating);
            SetTurnPhase(TurnPhase::PerformReport);
            StartPause(PERFORM_PRE_REPORT_PAUSE_SEC_, "PerformPreReport (PostPerformAnim)");
            SetupTurnBox();
            return;
        }

        if ((TurnPhase::PerformAnim == turnPhase_)
            && (AnimPhase::PostMoveTowardPause == animPhase_))
        {
            SetAnimPhase(AnimPhase::Impact);
            SetupTurnBox();
            combatAnimationUPtr_->ImpactAnimStart(ANIM_IMPACT_SHAKE_SLIDER_SPEED_);
            slider_.Reset(ANIM_IMPACT_SLIDER_SPEED_);
            return;
        }

        if (PreTurnPhase::PostPanPause == preTurnPhase_)
        {
            SetPreTurnPhase(PreTurnPhase::ZoomOut);
            slider_.Reset(ZOOM_SLIDER_SPEED_);
            return;
        }

        if (PreTurnPhase::PostZoomOutPause == preTurnPhase_)
        {
            SetPreTurnPhase(PreTurnPhase::End);
            return;
        }

        if (TurnPhase::ConditionEffectPause == turnPhase_)
        {
            // check for death by condition effect
            // use the condEffFightResult_
            auto const & CREATURE_EFFECTS_VEC { fightResult_.Effects() };
            if ((CREATURE_EFFECTS_VEC.empty() == false) && (CREATURE_EFFECTS_VEC[0].WasKill())
                && (CREATURE_EFFECTS_VEC[0].GetCreature()->IsPlayerCharacter() == false))
            {
                SetTurnPhase(TurnPhase::DeathAnim);

                // Note:  This creature ptr should be the same as turnCreaturePtrOpt_
                auto const CREATURE_PTR { CREATURE_EFFECTS_VEC[0].GetCreature() };

                if (CREATURE_PTR != turnCreaturePtrOpt_.value())
                {
                    M_HP_LOG_ERR("stage::CombatStage::EndPause(turnPhase_=ConditionEffectPause) "
                                 "found a death by condition but the creature that died was not "
                                 "the current turnCreaturePtrOpt_.");
                }

                auto const COMBAT_NODE_PTR { combatDisplayStagePtr_->GetCombatNodeForCreature(
                    CREATURE_PTR) };

                conditionEffectsCenterPosV_ = COMBAT_NODE_PTR->GetEntityPos();
                combatAnimationUPtr_->ShakeAnimStop(COMBAT_NODE_PTR);
                combatAnimationUPtr_->DeathAnimStart({ COMBAT_NODE_PTR });
                slider_.Reset(ANIM_DEATH_SLIDER_SPEED_);

                // This the first of two places where non-player death sfx is played,
                // so it can coincide with the non-player death animation start.
                // Player death sfx is played in HandleApplyDamageTasks().
                combatSoundEffects_.PlayDeath(CREATURE_PTR);

                return;
            }

            if (++conditionEffectsIndex_ < conditionEffectsVec_.size())
            {
                conditionEffectsCenterPosV_ = sf::Vector2f(0.0f, 0.0f);

                fightResult_ = combat::FightResult(combat::CreatureEffect(
                    turnCreaturePtrOpt_.value(),
                    combat::HitInfoVec_t(1, conditionEffectsVec_[conditionEffectsIndex_])));

                SetTurnPhase(TurnPhase::ConditionEffectPause);
                HandleApplyDamageTasks();
                StartPause(CONDITION_EFFECT_PAUSE_SEC_, "ConditionEffectPause");
            }
            else
            {
                fightResult_ = combat::FightResult();

                if (conditionEffectsTookTurn_)
                {
                    SetTurnPhase(TurnPhase::PostTurnPause);
                    StartPause(POST_TURN_PAUSE_SEC_, "PostTurn");
                }
                else
                {
                    if (turnCreaturePtrOpt_.value()->IsPlayerCharacter())
                    {
                        SetTurnPhase(TurnPhase::Determine);
                        SetUserActionAllowed(true);
                    }
                    else
                    {
                        SetTurnPhase(TurnPhase::PostCenterAndZoomInPause);

                        auto pauseToUse { PRE_TURN_PAUSE_SEC_ };
                        if (turnCreaturePtrOpt_.value()->CanTakeAction() == false)
                        {
                            pauseToUse *= 3.0f;
                        }

                        StartPause(pauseToUse, "PostZoomIn");
                    }
                }
            }

            SetupTurnBox();
            return;
        }

        if (IsPlayerCharacterTurnValid() && (TurnPhase::PostCenterAndZoomInPause == turnPhase_))
        {
            SetTurnPhase(TurnPhase::Determine);
            SetUserActionAllowed(true);
            SetupTurnBox();
            return;
        }

        if (IsPlayerCharacterTurnValid() && (TurnPhase::PostCenterAndZoomOutPause == turnPhase_))
        {
            StartPerformAnim();
            return;
        }

        if (IsNonPlayerCharacterTurnValid() && (TurnPhase::PostCenterAndZoomInPause == turnPhase_))
        {
            auto const TURN_CREATURE_PTR { turnCreaturePtrOpt_.value() };

            SetTurnPhase(TurnPhase::Determine);
            HandleEnemyTurnStep1_Decide(TURN_CREATURE_PTR);

            if (turnActionInfo_.Action() == combat::TurnAction::Nothing)
            {
                AppendStatusMessage(
                    combatText_.ActionText(
                        TURN_CREATURE_PTR, turnActionInfo_, fightResult_, true, true),
                    false);

                SetTurnPhase(TurnPhase::PostTurnPause);
                StartPause(POST_TURN_PAUSE_SEC_, "PostTurn");
            }
            else
            {
                // also do the perform step here so that the TurnBox can display the non-player
                // creature's intent before showing the result
                SetTurnActionPhase(HandleEnemyTurnStep2_Perform(TURN_CREATURE_PTR));

                // collect a list of all attacking and targeted creatures that need to be
                // centered on the screen
                creature::CreaturePVec_t allEffectedCreaturesPVec { turnCreaturePtrOpt_.value() };
                fightResult_.EffectedCreatures(allEffectedCreaturesPVec);

                if (combatDisplayStagePtr_->AreAllCreaturesVisible(allEffectedCreaturesPVec))
                {
                    SetTurnPhase(TurnPhase::PerformAnim);
                    StartPerformAnim();
                    SetupTurnBox();
                }
                else
                {
                    SetTurnPhase(TurnPhase::CenterAndZoomOut);
                    AnimationCenteringStart(allEffectedCreaturesPVec);
                    slider_.Reset(ANIM_CENTERING_SLIDER_SPEED_);
                }
            }
            return;
        }

        if (IsNonPlayerCharacterTurnValid() && (TurnPhase::PostCenterAndZoomOutPause == turnPhase_))
        {
            // See above for call to HandleEnemyTurnStep2_Perform()
            StartPerformAnim();
            SetupTurnBox();
            return;
        }

        if (TurnPhase::PerformReport == turnPhase_)
        {
            auto const CREATURE_EFFECTS_VEC { fightResult_.Effects() };
            if ((isFightResultCollapsed_ == false)
                && (performReportEffectIndex_ < CREATURE_EFFECTS_VEC.size()))
            {
                auto const HIT_INFO_VEC_SIZE {
                    CREATURE_EFFECTS_VEC.at(performReportEffectIndex_).GetHitInfoVec().size()
                };

                if ((HIT_INFO_VEC_SIZE > 0) && (performReportHitIndex_ < (HIT_INFO_VEC_SIZE - 1)))
                {
                    ++performReportHitIndex_;
                }
                else
                {
                    performReportHitIndex_ = 0;
                    ++performReportEffectIndex_;

                    if (performReportEffectIndex_ >= CREATURE_EFFECTS_VEC.size())
                    {
                        HandlePerformReportPhaseOverTasks();
                        return;
                    }
                }

                SetupTurnBox();
                StartPause(PERFORM_REPORT_PAUSE_SEC_, "PerformReport");
                return;
            }
            else
            {
                HandlePerformReportPhaseOverTasks();
                return;
            }
        }

        if (TurnPhase::PostPerformPause == turnPhase_)
        {
            creature::CreaturePVec_t killedNonPlayerCreaturesPVec;
            auto const CREATURE_EFFECTS { fightResult_.Effects() };
            for (auto const & NEXT_CREATURE_EFFECT : CREATURE_EFFECTS)
            {
                if ((NEXT_CREATURE_EFFECT.WasKill())
                    && (NEXT_CREATURE_EFFECT.GetCreature()->IsPlayerCharacter() == false))
                {
                    killedNonPlayerCreaturesPVec.emplace_back(NEXT_CREATURE_EFFECT.GetCreature());
                }
            }

            if (killedNonPlayerCreaturesPVec.empty())
            {
                SetTurnPhase(TurnPhase::PostTurnPause);
                StartPause(POST_TURN_PAUSE_SEC_, "PostTurn");
            }
            else
            {
                SetTurnPhase(TurnPhase::DeathAnim);

                combatAnimationUPtr_->DeathAnimStart(
                    combatDisplayStagePtr_->GetCombatNodesForCreatures(
                        killedNonPlayerCreaturesPVec));

                slider_.Reset(ANIM_DEATH_SLIDER_SPEED_);

                // This is the second of two places where non-player death sfx is played,
                // so it can coincide with the non-player death animation start.
                // Player death sfx is played in HandleApplyDamageTasks().
                for (auto const & NEXT_CREATURE_PTR : killedNonPlayerCreaturesPVec)
                {
                    combatSoundEffects_.PlayDeath(NEXT_CREATURE_PTR);
                }
            }
            return;
        }

        if (TurnPhase::PostTurnPause == turnPhase_)
        {
            if (PopulateAchievementsVec())
            {
                SetTurnPhase(TurnPhase::Achievements);
                StartPause(ACHIEVEMENT_PAUSE_SEC_, "Achievement");
            }
            else
            {
                SetTurnPhase(TurnPhase::End);
                StartPause(END_PAUSE_SEC_, "End");
            }

            return;
        }

        if (TurnPhase::Achievements == turnPhase_)
        {
            HandleAchievementPopups();
            return;
        }

        if (TurnPhase::End == turnPhase_)
        {
            // end-of-turn tasks here
            HandleKilledCreatures();
            combatDisplayStagePtr_->HandleEndOfTurnTasks();
            restoreInfo_.CanTurnAdvance(true);
            EndTurn();
            EndPause();
            return;
        }
    }

    void CombatStage::HandlePerformReportPhaseOverTasks()
    {
        performReportHitIndex_ = 0;
        performReportEffectIndex_ = 0;
        SetTurnPhase(TurnPhase::PostPerformPause);
        StartPause(POST_PERFORM_REPORT_PAUSE_SEC_, "PostPerformReport");
        SetupTurnBox();
    }

    void CombatStage::HandleEnemyTurnStep1_Decide(const creature::CreaturePtr_t TURN_CREATURE_PTR)
    {
        turnActionInfo_ = turnDecider_.Decide(TURN_CREATURE_PTR, combatDisplayStagePtr_);
        combat::Encounter::Instance()->SetTurnActionInfo(TURN_CREATURE_PTR, turnActionInfo_);
        SetupTurnBox();
    }

    CombatStage::TurnActionPhase
        CombatStage::HandleEnemyTurnStep2_Perform(const creature::CreaturePtr_t TURN_CREATURE_PTR)
    {
        switch (turnActionInfo_.Action())
        {
            case combat::TurnAction::Advance:
            {
                fightResult_ = combat::FightResult();
                SetupTurnBox();
                return TurnActionPhase::Advance;
            }

            case combat::TurnAction::Retreat:
            {
                fightResult_ = combat::FightResult();
                SetupTurnBox();
                return TurnActionPhase::Retreat;
            }

            case combat::TurnAction::Attack:
            {
                auto const TARGETS_PVEC { turnActionInfo_.Targets() };

                M_ASSERT_OR_LOGANDTHROW_SS(
                    (TARGETS_PVEC.empty() == false),
                    "stage::CombatStage::HandleEnemyTurnStep2_Perform(turn_action_info="
                        << turnActionInfo_.ToString()
                        << ", turn_creature=" << TURN_CREATURE_PTR->ToString()
                        << ") turn_action_info.Action()==SkyPounce||LandPounce but "
                           "turn_action_info.Targets() was empty.");

                fightResult_ = creatureInteraction_.Fight(TURN_CREATURE_PTR, TARGETS_PVEC.at(0));

                SetupTurnBox();
                return GetTurnActionPhaseFromFightResult(fightResult_);
            }

            case combat::TurnAction::Block:
            {
                fightResult_ = combat::FightResult();
                SetupTurnBox();

                AppendStatusMessage(
                    combatText_.ActionText(
                        TURN_CREATURE_PTR, turnActionInfo_, fightResult_, true, true),
                    false);

                return TurnActionPhase::PauseAndReport;
            }

            case combat::TurnAction::Fly:
            case combat::TurnAction::Land:
            {
                fightResult_ = combat::FightResult();
                combatDisplayStagePtr_->HandleFlyingChange(TURN_CREATURE_PTR, false);
                SetupTurnBox();

                AppendStatusMessage(
                    combatText_.ActionText(
                        TURN_CREATURE_PTR, turnActionInfo_, fightResult_, true, true),
                    false);

                return TurnActionPhase::PauseAndReport;
            }

            case combat::TurnAction::Cast:
            {
                creature::CreaturePVec_t targetedCreaturesPVec { turnActionInfo_.Targets() };
                combatDisplayStagePtr_->SortCreatureListByDisplayedPosition(targetedCreaturesPVec);

                fightResult_ = creatureInteraction_.Cast(
                    turnActionInfo_.Spell().value(), TURN_CREATURE_PTR, targetedCreaturesPVec);

                SetupTurnBox();

                AppendStatusMessage(
                    combatText_.ActionText(
                        TURN_CREATURE_PTR, turnActionInfo_, fightResult_, true, true),
                    false);

                return TurnActionPhase::Cast;
            }

            case combat::TurnAction::PlaySong:
            {
                creature::CreaturePVec_t targetedCreaturesPVec { turnActionInfo_.Targets() };
                combatDisplayStagePtr_->SortCreatureListByDisplayedPosition(targetedCreaturesPVec);

                fightResult_ = creatureInteraction_.PlaySong(
                    turnActionInfo_.Song().value(), TURN_CREATURE_PTR, targetedCreaturesPVec);

                SetupTurnBox();

                AppendStatusMessage(
                    combatText_.ActionText(
                        TURN_CREATURE_PTR, turnActionInfo_, fightResult_, true, true),
                    false);

                return TurnActionPhase::Cast; // reuse the cast TurnActionPhase intentionally here
            }

            case combat::TurnAction::SkyPounce:
            case combat::TurnAction::LandPounce:
            {
                auto const TARGETS_PVEC { turnActionInfo_.Targets() };

                M_ASSERT_OR_LOGANDTHROW_SS(
                    (TARGETS_PVEC.empty() == false),
                    "stage::CombatStage::HandleEnemyTurnStep2_Perform(turn_action_info="
                        << turnActionInfo_.ToString()
                        << ", turn_creature=" << TURN_CREATURE_PTR->ToString()
                        << ") turn_action_info.Action()==SkyPounce||LandPounce but "
                           "turn_action_info.Targets() was empty.");

                fightResult_ = creatureInteraction_.Pounce(TURN_CREATURE_PTR, TARGETS_PVEC.at(0));

                SetupTurnBox();

                AppendStatusMessage(
                    combatText_.ActionText(
                        TURN_CREATURE_PTR, turnActionInfo_, fightResult_, true, true),
                    false);

                // TODO If fightResult_ says the pounce was a success then
                //     return the TurnActionPhase::Pounce
                return TurnActionPhase::PauseAndReport;
            }

            case combat::TurnAction::Roar:
            {
                fightResult_ = creatureInteraction_.Roar(TURN_CREATURE_PTR, combatDisplayStagePtr_);
                SetupTurnBox();

                AppendStatusMessage(
                    combatText_.ActionText(
                        TURN_CREATURE_PTR, turnActionInfo_, fightResult_, true, true),
                    false);

                // TODO if fightResult_ says the roar happened then return TurnActionPhase::Roar
                return TurnActionPhase::PauseAndReport;
            }

            case combat::TurnAction::Run: // TODO

            case combat::TurnAction::ChangeWeapon:
            case combat::TurnAction::Nothing:
            case combat::TurnAction::Count: // handle Count gracefully because it is sometimes
                                            // required
            {
                fightResult_ = combat::FightResult();
                SetupTurnBox();

                AppendStatusMessage(
                    combatText_.ActionText(
                        TURN_CREATURE_PTR, turnActionInfo_, fightResult_, true, true),
                    false);

                return TurnActionPhase::PauseAndReport;
            }
            case combat::TurnAction::TreasureUnlock:
            default:
            {
                std::ostringstream ss;
                ss << "stage::CombatStage::HandleEnemyTurnStep2_Perform() found "
                   << "turnActionInfo_.Action() (which is really a combat::TurnAction) of "
                   << turnActionInfo_.Action() << " which is invalid.";

                throw std::runtime_error(ss.str());
            }
        }
    }

    // start centering anim
    void CombatStage::StartTurn_Step1()
    {
        zoomSliderOrigPos_ = zoomSliderBarUPtr_->PositionRatio();
        turnCreaturePtrOpt_ = combat::Encounter::Instance()->CurrentTurnCreaturePtrOpt();

        M_ASSERT_OR_LOGANDTHROW_SS(
            (!!turnCreaturePtrOpt_),
            "stage::CombatStage::StartTurn_Step1() Encounter::CurrentTurnCreaturePtrOpt() returned "
            "something uninitialized.");

        SetTurnPhase(TurnPhase::CenterAndZoomIn);

        combatAnimationUPtr_->CenteringStart(
            combatDisplayStagePtr_->GetCombatNodeForCreature(turnCreaturePtrOpt_.value()));

        slider_.Reset(ANIM_CENTERING_SLIDER_SPEED_);
    }

    // start pre-pause
    void CombatStage::StartTurn_Step2(const creature::CreaturePtr_t TURN_CREATURE_PTR)
    {
        auto const IS_PLAYER_TURN { TURN_CREATURE_PTR->IsPlayerCharacter() };
        combatDisplayStagePtr_->SetIsPlayerTurn(IS_PLAYER_TURN);

        combatAnimationUPtr_->CenteringStop();

        combatAnimationUPtr_->ShakeAnimStart(
            combatDisplayStagePtr_->GetCombatNodeForCreature(TURN_CREATURE_PTR),
            ANIM_CREATURE_SHAKE_SLIDER_SPEED_,
            false);

        goldTextColorShaker_.Reset();

        if (conditionEffectsWillSkip_)
        {
            conditionEffectsWillSkip_ = false;
        }
        else
        {
            conditionEffectsTookTurn_ = creatureInteraction_.ProcessConditionEffects(
                game::Phase::Combat, TURN_CREATURE_PTR, conditionEffectsVec_);
        }

        if (conditionEffectsVec_.empty())
        {
            if (IS_PLAYER_TURN)
            {
                SetTurnPhase(TurnPhase::Determine);
                SetUserActionAllowed(true);
            }
            else
            {
                SetTurnPhase(TurnPhase::PostCenterAndZoomInPause);

                auto pauseToUse { PRE_TURN_PAUSE_SEC_ };
                if (TURN_CREATURE_PTR->CanTakeAction() == false)
                {
                    pauseToUse *= 3.0f;
                }

                StartPause(pauseToUse, "PostZoomIn");
            }
        }
        else
        {
            conditionEffectsIndex_ = 0;

            fightResult_ = combat::FightResult(combat::CreatureEffect(
                TURN_CREATURE_PTR,
                combat::HitInfoVec_t(1, conditionEffectsVec_[conditionEffectsIndex_])));

            HandleApplyDamageTasks();

            SetTurnPhase(TurnPhase::ConditionEffectPause);
            StartPause(CONDITION_EFFECT_PAUSE_SEC_, "ConditionEffectPause");
        }

        SetupTurnBox();
    }

    void CombatStage::EndTurn()
    {
        isFightResultCollapsed_ = false;

        soundEffectsPlayedSet_.clear();

        conditionEffectsVec_.clear();
        conditionEffectsIndex_ = 0;
        conditionEffectsTookTurn_ = false;

        if (restoreInfo_.CanTurnAdvance())
        {
            combat::Encounter::Instance()->IncrementTurn();
            restoreInfo_.CanTurnAdvance(false);
        }

        performReportHitIndex_ = 0;
        performReportEffectIndex_ = 0;

        turnActionInfo_ = combat::TurnActionInfo();

        fightResult_ = combat::FightResult();

        SetPreTurnPhase(PreTurnPhase::End);
        SetTurnPhase(TurnPhase::NotATurn);
        SetTurnActionPhase(TurnActionPhase::None);
        SetAnimPhase(AnimPhase::NotAnimating);

        turnCreaturePtrOpt_ = boost::none;
        spellBeingCastPtrOpt_ = boost::none;
        songBeingPlayedPtrOpt_ = boost::none;

        MoveTurnBoxObjectsOffScreen();

        goldTextColorShaker_.Reset();
        redTextColorShaker_.Reset();

        conditionEffectsCenterPosV_ = sf::Vector2f(0.0f, 0.0f);

        conditionEffectsWillSkip_ = false;

        creatureTitlesVec_.clear();

        HandleWin();
        HandleLose();
    }

    void CombatStage::PositionSlideStartTasks(const creature::CreaturePtr_t TURN_CREATURE_PTR)
    {
        if (isRepositionAnimAfterRun_)
        {
            isRepositionAnimAfterRun_ = false;

            combatAnimationUPtr_->RepositionAnimStart(
                combatDisplayStagePtr_->GetCenterOfAllNodes(), combatDisplayStagePtr_);
        }
        else
        {
            if ((misc::IsRealZero(conditionEffectsCenterPosV_.x))
                && (misc::IsRealZero(conditionEffectsCenterPosV_.y)))
            {
                combatAnimationUPtr_->RepositionAnimStart(
                    combatDisplayStagePtr_->GetCombatNodeForCreature(TURN_CREATURE_PTR));
            }
            else
            {
                combatAnimationUPtr_->RepositionAnimStart(
                    conditionEffectsCenterPosV_, combatDisplayStagePtr_);
            }
        }

        combatAnimationUPtr_->ShakeAnimStop(
            combatDisplayStagePtr_->GetCombatNodeForCreature(TURN_CREATURE_PTR));
        slider_.Reset(ANIM_CREATURE_POS_SLIDER_SPEED_);
    }

    bool CombatStage::HandleAttack()
    {
        auto const MOUSEOVER_STR(combatText_.MouseOverTextAttackStr(
            turnCreaturePtrOpt_.value(), combatDisplayStagePtr_));

        if (MOUSEOVER_STR != combatText_.TBOX_BUTTON_MOUSEHOVER_TEXT_ATTACK_)
        {
            QuickSmallPopup(MOUSEOVER_STR, false, true);
            return false;
        }
        else
        {
            HandleAttackTasks(creatureInteraction_.FindNonPlayerCreatureToAttack(
                turnCreaturePtrOpt_.value(), combatDisplayStagePtr_));

            return true;
        }
    }

    bool CombatStage::HandleFight()
    {
        auto const MOUSEOVER_STR(
            combatText_.MouseOverTextFightStr(turnCreaturePtrOpt_.value(), combatDisplayStagePtr_));

        if (MOUSEOVER_STR != combatText_.TBOX_BUTTON_MOUSEHOVER_TEXT_FIGHT_)
        {
            QuickSmallPopup(MOUSEOVER_STR, false, true);
            return false;
        }
        else
        {
            // Can't set turnActionInfo_ or fightResult_ or turnActionPhase_ yet because
            // the player must select which non-player creature to fight first.
            combatDisplayStagePtr_->SetSummaryViewAllowed(false);
            combatDisplayStagePtr_->SetScrollingAllowed(true);
            SetTurnPhase(TurnPhase::TargetSelect);
            SetupTurnBox();
            return true;
        }
    }

    bool CombatStage::HandleSong_Step1_ValidatePlayAndSelectSong(
        const creature::CreaturePtr_t TURN_CREATURE_PTR)
    {
        auto const MOUSEOVER_STR(
            combatText_.MouseOverTextPlayStr(TURN_CREATURE_PTR, combatDisplayStagePtr_));

        if (MOUSEOVER_STR != combatText_.TBOX_BUTTON_MOUSEHOVER_TEXT_PLAY_)
        {
            QuickSmallPopup(MOUSEOVER_STR, false, true);
            return false;
        }
        else
        {
            SetUserActionAllowed(false);
            SetTurnActionPhase(TurnActionPhase::PlaySong);

            auto const POPUP_INFO { popup::PopupManager::Instance()->CreateMusicPopupInfo(
                POPUP_NAME_MUSICSHEET_,
                TURN_CREATURE_PTR,
                TURN_CREATURE_PTR->LastSongPlayedNum()) };

            game::LoopManager::Instance()->PopupWaitBeginSpecific<popup::PopupStageMusicSheet>(
                this, POPUP_INFO);

            return true;
        }
    }

    void CombatStage::HandleSong_Step2_PerformOnTargets(
        const creature::CreaturePtr_t TURN_CREATURE_PTR)
    {
        auto const SONG_TARGET { songBeingPlayedPtrOpt_.value()->Target() };

        if ((SONG_TARGET == combat::TargetType::Item)
            || (SONG_TARGET == combat::TargetType::QuestSpecific))
        {
            std::ostringstream ssErr;
            ssErr << "stage::CombatStage::HandleSong_Step2_SelectTargetOrPerformOnAll("
                  << "song=" << songBeingPlayedPtrOpt_.value()->Name() << ") had a target_type of "
                  << combat::TargetType::ToString(SONG_TARGET)
                  << " which is not yet supported in combat stage.";

            SystemErrorPopup("Playing this type of song is not yet supported.", ssErr.str());

            SetTurnPhase(TurnPhase::PostTurnPause);
            SetAnimPhase(AnimPhase::NotAnimating);
            SetTurnActionPhase(TurnActionPhase::None);
        }
        else
        {
            SetUserActionAllowed(false);

            auto creaturesListeningPVec { (
                (SONG_TARGET == combat::TargetType::AllCompanions)
                    ? creature::Algorithms::Players(creature::Algorithms::Living)
                    : creature::Algorithms::NonPlayers(creature::Algorithms::Living)) };

            combatDisplayStagePtr_->SortCreatureListByDisplayedPosition(creaturesListeningPVec);

            turnActionInfo_
                = combat::TurnActionInfo(songBeingPlayedPtrOpt_.value(), creaturesListeningPVec);

            combat::Encounter::Instance()->SetTurnActionInfo(TURN_CREATURE_PTR, turnActionInfo_);

            fightResult_ = creatureInteraction_.PlaySong(
                songBeingPlayedPtrOpt_.value(), TURN_CREATURE_PTR, creaturesListeningPVec);

            SetTurnActionPhase(TurnActionPhase::PlaySong);
            SetTurnPhase(TurnPhase::CenterAndZoomOut);
            isShortPostZoomOutPause_ = true;
            creaturesListeningPVec.emplace_back(TURN_CREATURE_PTR);
            AnimationCenteringStart(creaturesListeningPVec);
            slider_.Reset(ANIM_CENTERING_SLIDER_SPEED_);
            SetupTurnBox();
        }
    }

    bool CombatStage::HandleCast_Step1_ValidateCastAndSelectSpell(
        const creature::CreaturePtr_t TURN_CREATURE_PTR)
    {
        auto const MOUSEOVER_STR(
            combatText_.MouseOverTextCastStr(TURN_CREATURE_PTR, combatDisplayStagePtr_));

        if (MOUSEOVER_STR != combatText_.TBOX_BUTTON_MOUSEHOVER_TEXT_CAST_)
        {
            QuickSmallPopup(MOUSEOVER_STR, false, true);
            return false;
        }
        else
        {
            SetUserActionAllowed(false);
            SetTurnActionPhase(TurnActionPhase::Cast);

            auto const POPUP_INFO { popup::PopupManager::Instance()->CreateSpellbookPopupInfo(
                POPUP_NAME_SPELLBOOK_, TURN_CREATURE_PTR, TURN_CREATURE_PTR->LastSpellCastNum()) };

            game::LoopManager::Instance()->PopupWaitBeginSpecific<popup::PopupStageSpellbook>(
                this, POPUP_INFO);

            return true;
        }
    }

    void CombatStage::HandleCast_Step2_SelectTargetOrPerformOnAll()
    {
        auto const SPELL_TARGET { spellBeingCastPtrOpt_.value()->Target() };

        if ((SPELL_TARGET == combat::TargetType::SingleCompanion)
            || (SPELL_TARGET == combat::TargetType::SingleOpponent))
        {
            SetUserActionAllowed(true);
            combatDisplayStagePtr_->SetSummaryViewAllowed(false);
            combatDisplayStagePtr_->SetScrollingAllowed(true);
            SetTurnPhase(TurnPhase::TargetSelect);
            SetupTurnBox();
        }
        else if (SPELL_TARGET == combat::TargetType::AllCompanions)
        {
            HandleCast_Step3_PerformOnTargets(creature::Algorithms::Players());
        }
        else if (SPELL_TARGET == combat::TargetType::AllOpponents)
        {
            HandleCast_Step3_PerformOnTargets(
                creature::Algorithms::NonPlayers(creature::Algorithms::Living));
        }
        else
        {
            std::ostringstream ssErr;
            ssErr << "stage::CombatStage::HandleCast Step2 SelectTargetOrPerformOnAll("
                  << "spell=" << spellBeingCastPtrOpt_.value()->Name() << ") had a target_type of "
                  << combat::TargetType::ToString(SPELL_TARGET)
                  << " which is not yet supported in combat stage.";

            SystemErrorPopup("Casting this type of spell is not yet supported.", ssErr.str());

            SetTurnPhase(TurnPhase::PostTurnPause);
            SetAnimPhase(AnimPhase::NotAnimating);
            SetTurnActionPhase(TurnActionPhase::None);
        }
    }

    void CombatStage::HandleCast_Step3_PerformOnTargets(
        const creature::CreaturePVec_t & CREATURES_TO_CAST_UPON_PVEC)
    {
        auto const TURN_CREATURE_PTR { turnCreaturePtrOpt_.value() };

        SetUserActionAllowed(false);

        auto creaturesToCastUponCopyPVec { CREATURES_TO_CAST_UPON_PVEC };
        combatDisplayStagePtr_->SortCreatureListByDisplayedPosition(creaturesToCastUponCopyPVec);

        turnActionInfo_
            = combat::TurnActionInfo(spellBeingCastPtrOpt_.value(), creaturesToCastUponCopyPVec);

        combat::Encounter::Instance()->SetTurnActionInfo(TURN_CREATURE_PTR, turnActionInfo_);

        fightResult_ = creatureInteraction_.Cast(
            spellBeingCastPtrOpt_.value(), TURN_CREATURE_PTR, creaturesToCastUponCopyPVec);

        SetTurnActionPhase(TurnActionPhase::Cast);
        SetTurnPhase(TurnPhase::CenterAndZoomOut);
        isShortPostZoomOutPause_ = true;
        creaturesToCastUponCopyPVec.emplace_back(TURN_CREATURE_PTR);
        AnimationCenteringStart(creaturesToCastUponCopyPVec);
        slider_.Reset(ANIM_CENTERING_SLIDER_SPEED_);
        SetupTurnBox();
    }

    bool CombatStage::HandleAdvance()
    {
        auto const TURN_CREATURE_PTR { turnCreaturePtrOpt_.value() };

        auto const MOUSEOVER_STR(
            combatText_.MouseOverTextAdvanceStr(TURN_CREATURE_PTR, combatDisplayStagePtr_));

        if (MOUSEOVER_STR != combatText_.TBOX_BUTTON_MOUSEHOVER_TEXT_ADVANCE_)
        {
            QuickSmallPopup(MOUSEOVER_STR, false, true);
            return false;
        }
        else
        {
            SetUserActionAllowed(false);

            turnActionInfo_ = combat::TurnActionInfo(combat::TurnAction::Advance);
            combat::Encounter::Instance()->SetTurnActionInfo(TURN_CREATURE_PTR, turnActionInfo_);

            AppendStatusMessage(
                combatText_.ActionText(
                    TURN_CREATURE_PTR, turnActionInfo_, fightResult_, true, true),
                false);

            SetTurnActionPhase(TurnActionPhase::Advance);
            StartPerformAnim();
            return true;
        }
    }

    bool CombatStage::HandleRetreat()
    {
        auto const TURN_CREATURE_PTR { turnCreaturePtrOpt_.value() };

        auto const MOUSEOVER_STR(
            combatText_.MouseOverTextRetreatStr(TURN_CREATURE_PTR, combatDisplayStagePtr_));

        if (MOUSEOVER_STR != combatText_.TBOX_BUTTON_MOUSEHOVER_TEXT_RETREAT_)
        {
            QuickSmallPopup(MOUSEOVER_STR, false, true);
            return false;
        }
        else
        {
            SetUserActionAllowed(false);

            turnActionInfo_ = combat::TurnActionInfo(combat::TurnAction::Retreat);

            combat::Encounter::Instance()->SetTurnActionInfo(TURN_CREATURE_PTR, turnActionInfo_);

            AppendStatusMessage(
                combatText_.ActionText(
                    TURN_CREATURE_PTR, turnActionInfo_, fightResult_, true, true),
                false);

            SetTurnActionPhase(TurnActionPhase::Retreat);
            StartPerformAnim();
            return true;
        }
    }

    bool CombatStage::HandleBlock()
    {
        auto const TURN_CREATURE_PTR { turnCreaturePtrOpt_.value() };

        auto const MOUSEOVER_STR(
            combatText_.MouseOverTextBlockStr(TURN_CREATURE_PTR, combatDisplayStagePtr_));

        if (MOUSEOVER_STR != combatText_.TBOX_BUTTON_MOUSEHOVER_TEXT_BLOCK_)
        {
            QuickSmallPopup(MOUSEOVER_STR, false, true);
            return false;
        }
        else
        {
            SetUserActionAllowed(false);

            turnActionInfo_ = combat::TurnActionInfo(combat::TurnAction::Block);

            combat::Encounter::Instance()->SetTurnActionInfo(TURN_CREATURE_PTR, turnActionInfo_);

            // no need for ZoomAndSlide or PerformAnim so skip to end of turn
            // with AppendStatusMessage()
            AppendStatusMessage(
                combatText_.ActionText(
                    TURN_CREATURE_PTR, turnActionInfo_, fightResult_, true, true),
                true);

            return true;
        }
    }

    bool CombatStage::HandleSkip()
    {
        if (skipTBoxButtonUPtr_->GetMouseState() == sfml_util::MouseState::Disabled)
        {
            return false;
        }

        auto const TURN_CREATURE_PTR { turnCreaturePtrOpt_.value() };

        SetUserActionAllowed(false);

        turnActionInfo_ = combat::TurnActionInfo(combat::TurnAction::Nothing);

        combat::Encounter::Instance()->SetTurnActionInfo(TURN_CREATURE_PTR, turnActionInfo_);

        // no need for ZoomAndSlide or PerformAnim so skip to end of turn with AppendStatusMessage()
        AppendStatusMessage(
            combatText_.ActionText(TURN_CREATURE_PTR, turnActionInfo_, fightResult_, true, true),
            true);

        return true;
    }

    bool CombatStage::HandleFly()
    {
        auto const TURN_CREATURE_PTR { turnCreaturePtrOpt_.value() };

        auto const MOUSEOVER_STR(
            combatText_.MouseOverTextFlyStr(TURN_CREATURE_PTR, combatDisplayStagePtr_));

        if (MOUSEOVER_STR != combatText_.TBOX_BUTTON_MOUSEHOVER_TEXT_FLY_)
        {
            QuickSmallPopup(MOUSEOVER_STR, false, true);
            return false;
        }
        else
        {
            SetUserActionAllowed(false);

            turnActionInfo_ = combat::TurnActionInfo(combat::TurnAction::Fly);

            combat::Encounter::Instance()->SetTurnActionInfo(TURN_CREATURE_PTR, turnActionInfo_);

            combat::Encounter::Instance()->SetIsFlying(TURN_CREATURE_PTR, true);

            combatDisplayStagePtr_->HandleFlyingChange(TURN_CREATURE_PTR, true);

            // no need for turnActionPhase_, ZoomAndSlide, or PerformAnim so skip
            // to end of turn with AppendStatusMessage()
            AppendStatusMessage(
                combatText_.ActionText(
                    TURN_CREATURE_PTR, turnActionInfo_, fightResult_, true, true),
                true);

            return true;
        }
    }

    bool CombatStage::HandleLand()
    {
        auto const TURN_CREATURE_PTR { turnCreaturePtrOpt_.value() };

        auto const MOUSEOVER_STR(
            combatText_.MouseOverTextLandStr(TURN_CREATURE_PTR, combatDisplayStagePtr_));

        if (MOUSEOVER_STR != combatText_.TBOX_BUTTON_MOUSEHOVER_TEXT_LAND_)
        {
            QuickSmallPopup(MOUSEOVER_STR, false, true);
            return false;
        }
        else
        {
            SetUserActionAllowed(false);

            turnActionInfo_ = combat::TurnActionInfo(combat::TurnAction::Land);

            combat::Encounter::Instance()->SetTurnActionInfo(TURN_CREATURE_PTR, turnActionInfo_);

            combat::Encounter::Instance()->SetIsFlying(TURN_CREATURE_PTR, false);

            combatDisplayStagePtr_->HandleFlyingChange(TURN_CREATURE_PTR, false);

            // no need for turnActionPhase_, ZoomAndSlide, or PerformAnim so skip
            // to end of turn with AppendStatusMessage()
            AppendStatusMessage(
                combatText_.ActionText(
                    TURN_CREATURE_PTR, turnActionInfo_, fightResult_, true, true),
                true);

            return true;
        }
    }

    bool CombatStage::HandleRoar()
    {
        auto const TURN_CREATURE_PTR { turnCreaturePtrOpt_.value() };

        auto const MOUSEOVER_STR(
            combatText_.MouseOverTextRoarStr(TURN_CREATURE_PTR, combatDisplayStagePtr_));

        auto creaturesToCenterOnPVec { combatDisplayStagePtr_->GetCreaturesInRoaringDistance(
            TURN_CREATURE_PTR) };

        if (MOUSEOVER_STR != combatText_.TBOX_BUTTON_MOUSEHOVER_TEXT_ROAR_)
        {
            QuickSmallPopup(MOUSEOVER_STR, false, true);
            return false;
        }
        else if (creaturesToCenterOnPVec.empty())
        {
            QuickSmallPopup("There are no creatures in range to roar at.", false, true);
            return false;
        }
        else
        {
            SetUserActionAllowed(false);

            turnActionInfo_ = combat::TurnActionInfo(combat::TurnAction::Roar);

            combat::Encounter::Instance()->SetTurnActionInfo(TURN_CREATURE_PTR, turnActionInfo_);

            fightResult_ = combat::FightResult();

            AppendStatusMessage(
                combatText_.ActionText(
                    TURN_CREATURE_PTR, turnActionInfo_, fightResult_, true, true),
                false);

            creaturesToCenterOnPVec.emplace_back(TURN_CREATURE_PTR);
            AnimationCenteringStart(creaturesToCenterOnPVec);
            slider_.Reset(ANIM_CENTERING_SLIDER_SPEED_);
            SetTurnActionPhase(TurnActionPhase::Roar);
            SetTurnPhase(TurnPhase::CenterAndZoomOut);
            isShortPostZoomOutPause_ = true;
            SetupTurnBox();
            return true;
        }
    }

    bool CombatStage::HandlePounce(const bool IS_SKY_POUNCE)
    {
        auto const TURN_CREATURE_PTR { turnCreaturePtrOpt_.value() };

        auto const MOUSEOVER_STR(
            combatText_.MouseOverTextPounceStr(TURN_CREATURE_PTR, combatDisplayStagePtr_));

        if (MOUSEOVER_STR != combatText_.TBOX_BUTTON_MOUSEHOVER_TEXT_POUNCE_)
        {
            QuickSmallPopup(MOUSEOVER_STR, false, true);
            return false;
        }
        else
        {
            SetUserActionAllowed(false);

            turnActionInfo_ = combat::TurnActionInfo(
                ((IS_SKY_POUNCE) ? combat::TurnAction::SkyPounce : combat::TurnAction::LandPounce));

            combat::Encounter::Instance()->SetTurnActionInfo(TURN_CREATURE_PTR, turnActionInfo_);

            // not implemented yet so skip to end of turn with AppendStatusMessage()
            AppendStatusMessage(
                combatText_.ActionText(
                    TURN_CREATURE_PTR, turnActionInfo_, fightResult_, true, true),
                true);

            return true;
        }
    }

    bool CombatStage::HandleWeaponChange()
    {
        auto const TURN_CREATURE_PTR { turnCreaturePtrOpt_.value() };

        turnActionInfo_ = combat::TurnActionInfo(combat::TurnAction::ChangeWeapon);

        combat::Encounter::Instance()->SetTurnActionInfo(TURN_CREATURE_PTR, turnActionInfo_);

        TURN_CREATURE_PTR->IncrementHeldWeapons();
        SetupTurnBox();
        return true;
    }

    bool CombatStage::HandleRun()
    {
        auto const TURN_CREATURE_PTR { turnCreaturePtrOpt_.value() };

        auto const MOUSEOVER_STR(
            combatText_.MouseOverTextRunStr(TURN_CREATURE_PTR, combatDisplayStagePtr_));

        if (MOUSEOVER_STR != combatText_.TBOX_BUTTON_MOUSEHOVER_TEXT_RUN_)
        {
            QuickSmallPopup(MOUSEOVER_STR, false, true);
            return false;
        }
        else
        {
            // run away works if flying, and if not flying it is a test of speed
            if ((creature::Stats::Test(
                    TURN_CREATURE_PTR,
                    creature::Traits::Speed,
                    static_cast<creature::Stats::With>(
                        creature::Stats::With::Luck | creature::Stats::With::RaceRoleBonus
                        | creature::Stats::With::StandardBonus)))
                || (combat::Encounter::Instance()
                        ->GetTurnInfoCopy(TURN_CREATURE_PTR)
                        .GetIsFlying()))
            {
                SetUserActionAllowed(false);

                turnActionInfo_ = combat::TurnActionInfo(combat::TurnAction::Run);

                combat::Encounter::Instance()->SetTurnActionInfo(
                    TURN_CREATURE_PTR, turnActionInfo_);

                AppendStatusMessage(
                    combatText_.ActionText(
                        TURN_CREATURE_PTR, turnActionInfo_, fightResult_, true, true),
                    false);

                SetTurnActionPhase(TurnActionPhase::Run);
                StartPerformAnim();
                return true;
            }
            else
            {
                AppendStatusMessage(
                    TURN_CREATURE_PTR->Name() + " tried to run away but was blocked.", true);

                return false;
            }
        }
    }

    void CombatStage::MoveTurnBoxObjectsOffScreen()
    {
        titleTBoxTextRegionUPtr_->MoveEntityOffScreen();
        weaponTBoxTextRegionUPtr_->MoveEntityOffScreen();
        armorTBoxTextRegionUPtr_->MoveEntityOffScreen();
        infoTBoxTextRegionUPtr_->MoveEntityOffScreen();
        enemyActionTBoxRegionUPtr_->MoveEntityOffScreen();
        enemyCondsTBoxRegionUPtr_->MoveEntityOffScreen();
    }

    void CombatStage::SetupTurnBoxButtons(
        const creature::CreaturePtrOpt_t & CREATURE_PTR_OPT, const bool WILL_DISABLE_ALL)
    {
        if (CREATURE_PTR_OPT && CREATURE_PTR_OPT.value()->IsPlayerCharacter()
            && (WILL_DISABLE_ALL == false))
        {
            auto const CREATURE_PTR { CREATURE_PTR_OPT.value() };

            // attack button
            auto const MOT_ATTACK_STR(
                combatText_.MouseOverTextAttackStr(CREATURE_PTR, combatDisplayStagePtr_));

            SetButtonDisabledIf(
                attackTBoxButtonUPtr_,
                (MOT_ATTACK_STR != combatText_.TBOX_BUTTON_MOUSEHOVER_TEXT_ATTACK_));

            attackTBoxButtonUPtr_->SetMouseHoverText(MOT_ATTACK_STR);

            // fight button
            auto const MOT_FIGHT_STR(
                combatText_.MouseOverTextFightStr(CREATURE_PTR, combatDisplayStagePtr_));

            SetButtonDisabledIf(
                fightTBoxButtonUPtr_,
                (MOT_FIGHT_STR != combatText_.TBOX_BUTTON_MOUSEHOVER_TEXT_FIGHT_));

            fightTBoxButtonUPtr_->SetMouseHoverText(MOT_FIGHT_STR);

            // cast/play button
            if (turnCreaturePtrOpt_
                && (turnCreaturePtrOpt_.value()->Role() == creature::role::Bard))
            {
                castTBoxButtonUPtr_->TextEntity()->SetText("(S)ong");
                castTBoxButtonUPtr_->Sync();

                auto const MOT_PLAY_STR(
                    combatText_.MouseOverTextPlayStr(CREATURE_PTR, combatDisplayStagePtr_));

                SetButtonDisabledIf(
                    castTBoxButtonUPtr_,
                    (MOT_PLAY_STR != combatText_.TBOX_BUTTON_MOUSEHOVER_TEXT_PLAY_));

                castTBoxButtonUPtr_->SetMouseHoverText(MOT_PLAY_STR);
            }
            else
            {
                castTBoxButtonUPtr_->TextEntity()->SetText("(S)pell");
                castTBoxButtonUPtr_->Sync();

                auto const MOT_CAST_STR(
                    combatText_.MouseOverTextCastStr(CREATURE_PTR, combatDisplayStagePtr_));

                SetButtonDisabledIf(
                    castTBoxButtonUPtr_,
                    (MOT_CAST_STR != combatText_.TBOX_BUTTON_MOUSEHOVER_TEXT_CAST_));

                castTBoxButtonUPtr_->SetMouseHoverText(MOT_CAST_STR);
            }

            // advance button
            auto const MOT_ADVANCE_STR(
                combatText_.MouseOverTextAdvanceStr(CREATURE_PTR, combatDisplayStagePtr_));

            SetButtonDisabledIf(
                advanceTBoxButtonUPtr_,
                (MOT_ADVANCE_STR != combatText_.TBOX_BUTTON_MOUSEHOVER_TEXT_ADVANCE_));

            advanceTBoxButtonUPtr_->SetMouseHoverText(MOT_ADVANCE_STR);

            // retreat button
            auto const MOT_RETREAT_STR(
                combatText_.MouseOverTextRetreatStr(CREATURE_PTR, combatDisplayStagePtr_));

            SetButtonDisabledIf(
                retreatTBoxButtonUPtr_,
                (MOT_RETREAT_STR != combatText_.TBOX_BUTTON_MOUSEHOVER_TEXT_RETREAT_));

            retreatTBoxButtonUPtr_->SetMouseHoverText(MOT_RETREAT_STR);

            // block button
            const std::string MOT_BLOCK_STR(
                combatText_.MouseOverTextBlockStr(CREATURE_PTR, combatDisplayStagePtr_));

            SetButtonDisabledIf(
                blockTBoxButtonUPtr_,
                (MOT_BLOCK_STR != combatText_.TBOX_BUTTON_MOUSEHOVER_TEXT_BLOCK_));

            blockTBoxButtonUPtr_->SetMouseHoverText(MOT_BLOCK_STR);

            // fly button
            auto const MOT_FLY_STR(
                combatText_.MouseOverTextFlyStr(CREATURE_PTR, combatDisplayStagePtr_));

            SetButtonDisabledIf(
                flyTBoxButtonUPtr_, (MOT_FLY_STR != combatText_.TBOX_BUTTON_MOUSEHOVER_TEXT_FLY_));

            flyTBoxButtonUPtr_->SetMouseHoverText(MOT_FLY_STR);

            // land
            auto const MOT_LAND_STR(
                combatText_.MouseOverTextLandStr(CREATURE_PTR, combatDisplayStagePtr_));

            SetButtonDisabledIf(
                landTBoxButtonUPtr_,
                (MOT_LAND_STR != combatText_.TBOX_BUTTON_MOUSEHOVER_TEXT_LAND_));

            landTBoxButtonUPtr_->SetMouseHoverText(MOT_LAND_STR);

            // roar button
            auto const MOT_ROAR_STR(
                combatText_.MouseOverTextRoarStr(CREATURE_PTR, combatDisplayStagePtr_));

            SetButtonDisabledIf(
                roarTBoxButtonUPtr_,
                (MOT_ROAR_STR != combatText_.TBOX_BUTTON_MOUSEHOVER_TEXT_ROAR_));

            roarTBoxButtonUPtr_->SetMouseHoverText(MOT_ROAR_STR);

            // pounce button
            auto const MOT_POUNCE_STR(
                combatText_.MouseOverTextPounceStr(CREATURE_PTR, combatDisplayStagePtr_));

            SetButtonDisabledIf(
                pounceTBoxButtonUPtr_,
                (MOT_POUNCE_STR != combatText_.TBOX_BUTTON_MOUSEHOVER_TEXT_POUNCE_));

            pounceTBoxButtonUPtr_->SetMouseHoverText(MOT_POUNCE_STR);

            // skip button
            auto const MOT_SKIP_STR(
                combatText_.MouseOverTextSkipStr(CREATURE_PTR, combatDisplayStagePtr_));

            SetButtonDisabledIf(
                skipTBoxButtonUPtr_,
                (MOT_SKIP_STR != combatText_.TBOX_BUTTON_MOUSEHOVER_TEXT_SKIP_));

            skipTBoxButtonUPtr_->SetMouseHoverText(MOT_SKIP_STR);
        }
        else
        {
            SetButtonDisabledIf(attackTBoxButtonUPtr_, true);
            SetButtonDisabledIf(fightTBoxButtonUPtr_, true);
            SetButtonDisabledIf(castTBoxButtonUPtr_, true);
            SetButtonDisabledIf(runTBoxButtonUPtr_, true);
            SetButtonDisabledIf(advanceTBoxButtonUPtr_, true);
            SetButtonDisabledIf(retreatTBoxButtonUPtr_, true);
            SetButtonDisabledIf(landTBoxButtonUPtr_, true);
            SetButtonDisabledIf(flyTBoxButtonUPtr_, true);
            SetButtonDisabledIf(pounceTBoxButtonUPtr_, true);
            SetButtonDisabledIf(roarTBoxButtonUPtr_, true);
            SetButtonDisabledIf(skipTBoxButtonUPtr_, true);
            SetButtonDisabledIf(blockTBoxButtonUPtr_, true);

            attackTBoxButtonUPtr_->SetMouseHoverText("");
            fightTBoxButtonUPtr_->SetMouseHoverText("");
            castTBoxButtonUPtr_->SetMouseHoverText("");
            runTBoxButtonUPtr_->SetMouseHoverText("");
            advanceTBoxButtonUPtr_->SetMouseHoverText("");
            retreatTBoxButtonUPtr_->SetMouseHoverText("");
            landTBoxButtonUPtr_->SetMouseHoverText("");
            flyTBoxButtonUPtr_->SetMouseHoverText("");
            pounceTBoxButtonUPtr_->SetMouseHoverText("");
            roarTBoxButtonUPtr_->SetMouseHoverText("");
            blockTBoxButtonUPtr_->SetMouseHoverText("");
            skipTBoxButtonUPtr_->SetMouseHoverText("");
        }
    }

    void CombatStage::QuickSmallPopup(
        const std::string & PROMPT,
        const bool IS_SOUNDEFFECT_NORMAL,
        const bool WILL_PREPEND_NEWLINE)
    {
        std::ostringstream ss;
        ss << "\n" << ((WILL_PREPEND_NEWLINE) ? "\n" : "") << PROMPT;
        auto const POPUP_INFO { popup::PopupManager::Instance()->CreatePopupInfo(
            "CombatStage'sQuickPopup",
            ss.str(),
            popup::PopupButtons::Okay,
            popup::PopupImage::Regular,
            sfml_util::Justified::Center,
            ((IS_SOUNDEFFECT_NORMAL) ? sfml_util::sound_effect::PromptGeneric
                                     : sfml_util::sound_effect::PromptWarn),
            sfml_util::FontManager::Instance()->Size_Normal()) };

        game::LoopManager::Instance()->PopupWaitBegin(this, POPUP_INFO);
    }

    void CombatStage::SetupTurnBox()
    {
        auto const TURN_CREATURE_PTR { turnCreaturePtrOpt_.value() };
        auto const EMPTY_STR { " " }; // any short all-whitespace non-empty string will work here
        auto const CAN_TAKE_ACTION_STR { TURN_CREATURE_PTR->CanTakeActionStr() };
        auto const CURR_WEAPONS_STR { TURN_CREATURE_PTR->WeaponsString() };

        std::ostringstream weaponsSS;
        if (CURR_WEAPONS_STR.empty())
        {
            weaponsSS << "(holding no weapons, can't attack)";
        }
        else
        {
            weaponsSS << "Weapon";
            if (boost::algorithm::contains(CURR_WEAPONS_STR, ","))
            {
                weaponsSS << "s";
            }

            weaponsSS << ":  " << CURR_WEAPONS_STR;
        }

        const std::string HOLDING_WEAPON_STR(weaponsSS.str());

        std::ostringstream titleSS;
        std::ostringstream weaponHoldingSS;
        std::ostringstream armorSS;
        std::ostringstream infoSS;
        std::ostringstream preambleSS;
        std::ostringstream enemyCondsSS;

        auto isPreambleShowing { false };

        if (IsPlayerCharacterTurnValid())
        {
            willRedColorShakeWeaponText_ = false;

            titleSS << TURN_CREATURE_PTR->Name() << "'s Turn";

            weaponHoldingSS << HOLDING_WEAPON_STR;

            infoSS << " Str:   " << TURN_CREATURE_PTR->TraitCurrent(creature::Traits::Strength)
                   << " "
                   << TURN_CREATURE_PTR->TraitModifiedString(creature::Traits::Strength, true)
                   << "\n Acc:  " << TURN_CREATURE_PTR->TraitCurrent(creature::Traits::Accuracy)
                   << " "
                   << TURN_CREATURE_PTR->TraitModifiedString(creature::Traits::Accuracy, true)
                   << "\n Cha:  " << TURN_CREATURE_PTR->TraitCurrent(creature::Traits::Charm) << " "
                   << TURN_CREATURE_PTR->TraitModifiedString(creature::Traits::Charm, true)
                   << "\n Lck:  " << TURN_CREATURE_PTR->TraitCurrent(creature::Traits::Luck) << " "
                   << TURN_CREATURE_PTR->TraitModifiedString(creature::Traits::Luck, true)
                   << "\n Spd:  " << TURN_CREATURE_PTR->TraitCurrent(creature::Traits::Speed) << " "
                   << TURN_CREATURE_PTR->TraitModifiedString(creature::Traits::Speed, true)
                   << "\n Int:    "
                   << TURN_CREATURE_PTR->TraitCurrent(creature::Traits::Intelligence) << " "
                   << TURN_CREATURE_PTR->TraitModifiedString(creature::Traits::Intelligence, true)
                   << "\nHealth:  " << TURN_CREATURE_PTR->HealthCurrent() << "/"
                   << TURN_CREATURE_PTR->HealthNormal() << "\nCondition";

            if (TURN_CREATURE_PTR->Conditions().size() > 1)
            {
                infoSS << "s";
            }

            infoSS << ":  " << TURN_CREATURE_PTR->ConditionNames(6);

            armorSS << "Armor Rating: " << TURN_CREATURE_PTR->ArmorRating();

            auto const MANA_NORMAL { TURN_CREATURE_PTR->ManaNormal() };
            if (MANA_NORMAL > 0_mana)
            {
                armorSS << "\n\nMana: " << TURN_CREATURE_PTR->Mana() << "/" << MANA_NORMAL;
            }

            preambleSS.str(EMPTY_STR);
            enemyCondsSS.str(EMPTY_STR);
        }
        else if (IsNonPlayerCharacterTurnValid())
        {
            titleSS << TURN_CREATURE_PTR->RaceName();

            if (creature::race::RaceRoleMatch(TURN_CREATURE_PTR->Race(), TURN_CREATURE_PTR->Role())
                == false)
            {
                titleSS << " " << TURN_CREATURE_PTR->RoleName();
            }

            // turn box weapon text (or text that indicates that a creature cannot take their turn)
            if (CAN_TAKE_ACTION_STR.empty())
            {
                willRedColorShakeWeaponText_ = false;
                weaponHoldingSS << HOLDING_WEAPON_STR;
            }
            else
            {
                willRedColorShakeWeaponText_ = true;
                weaponHoldingSS << "Cannot take "
                                << creature::sex::HisHerIts(TURN_CREATURE_PTR->Sex(), false, false)
                                << " turn because " << TURN_CREATURE_PTR->CanTakeActionStr(false)
                                << "!";
            }

            infoSS.str(EMPTY_STR); // any short all-whitespace non-empty string will work here

            armorSS << "Armor:  ";

            auto const ARMOR_STR(TURN_CREATURE_PTR->ArmorString());
            if (ARMOR_STR.empty())
            {
                armorSS << "None";
            }
            else
            {
                armorSS << ARMOR_STR;
            }

            if (TURN_CREATURE_PTR->HasCondition(creature::Conditions::Good))
            {
                enemyCondsSS << " ";
            }
            else
            {
                auto const CONDITION_LIST_STR { TURN_CREATURE_PTR->ConditionNames(6) };
                enemyCondsSS << "Condition";

                if (boost::algorithm::contains(CONDITION_LIST_STR, ","))
                {
                    enemyCondsSS << "s";
                }

                enemyCondsSS << ": " << CONDITION_LIST_STR;
            }
        }

        if ((TurnPhase::CenterAndZoomOut == turnPhase_)
            || (TurnPhase::PostCenterAndZoomOutPause == turnPhase_)
            || ((TurnPhase::PostCenterAndZoomInPause == turnPhase_)
                && IsNonPlayerCharacterTurnValid())
            || ((TurnPhase::PerformAnim == turnPhase_) && (animPhase_ < AnimPhase::Impact)))
        {
            if ((fightResult_.Count() > 0)
                || ((TurnPhase::PerformAnim == turnPhase_)
                    && (AnimPhase::AdvanceOrRetreat == animPhase_)))
            {
                infoSS.str(EMPTY_STR);
                weaponHoldingSS.str(EMPTY_STR);
                armorSS.str(EMPTY_STR);
                enemyCondsSS.str(EMPTY_STR);

                isPreambleShowing = true;

                // preamble version of action text
                preambleSS << combatText_.ActionText(
                    TURN_CREATURE_PTR, turnActionInfo_, fightResult_, false, false, true);
            }
            else
            {
                preambleSS.str(EMPTY_STR);
            }
        }
        else if (
            (TurnPhase::PerformReport == turnPhase_) || (TurnPhase::PostPerformPause == turnPhase_)
            || ((TurnPhase::PerformAnim == turnPhase_) && (animPhase_ >= AnimPhase::Impact)))
        {
            HandlePlayingMeleeSoundEffects();

            infoSS.str(EMPTY_STR);
            weaponHoldingSS.str(EMPTY_STR);
            armorSS.str(EMPTY_STR);
            enemyCondsSS.str(EMPTY_STR);

            isPreambleShowing = true;

            // perform report version of action text
            preambleSS << combatText_.ActionTextIndexed(
                TURN_CREATURE_PTR,
                turnActionInfo_,
                fightResult_,
                false,
                performReportEffectIndex_,
                performReportHitIndex_,
                isFightResultCollapsed_);
        }
        else if (TurnPhase::TargetSelect == turnPhase_)
        {
            infoSS.str(EMPTY_STR);
            weaponHoldingSS.str(EMPTY_STR);
            armorSS.str(EMPTY_STR);
            enemyCondsSS.str(EMPTY_STR);

            isPreambleShowing = true;

            if (!spellBeingCastPtrOpt_)
            {
                preambleSS << "Click to select who to fight...\n\n"
                           << "(Press Escape or X to Cancel)";
            }
            else
            {
                preambleSS << "Click on the ";

                if (spellBeingCastPtrOpt_.value()->Target() == combat::TargetType::SingleOpponent)
                {
                    preambleSS << "enemy creature";
                }
                else if (
                    spellBeingCastPtrOpt_.value()->Target() == combat::TargetType::SingleCompanion)
                {
                    preambleSS << "character";
                }

                preambleSS << " to cast " << spellBeingCastPtrOpt_.value()->Name() << " on...\n\n"
                           << "(Press Escape or X to Cancel)";
            }
        }
        else if (
            (conditionEffectsIndex_ < conditionEffectsVec_.size())
            && (TurnPhase::ConditionEffectPause == turnPhase_))
        {
            infoSS.str(EMPTY_STR);
            weaponHoldingSS.str(EMPTY_STR);
            armorSS.str(EMPTY_STR);
            enemyCondsSS.str(EMPTY_STR);

            isPreambleShowing = true;

            auto const & HIT_INFO { conditionEffectsVec_[conditionEffectsIndex_] };

            preambleSS << HIT_INFO.ActionPhrase().Compose(
                HIT_INFO.ConditionPtrOpt().value()->Name(), TURN_CREATURE_PTR->Name());
        }
        else
        {
            preambleSS.str(EMPTY_STR);
        }

        auto const VERT_POS_SHIFT(sfml_util::MapByRes(0.0f, 16.0f));
        titleTBoxTextRegionUPtr_->SetText(titleSS.str());
        titleTBoxTextRegionUPtr_->SetEntityPos(
            turnBoxRegion_.left,
            (turnBoxRegion_.top + (titleTBoxTextRegionUPtr_->GetEntityRegion().height * 0.5f))
                - VERT_POS_SHIFT);

        weaponTBoxTextRegionUPtr_->SetText(weaponHoldingSS.str());

        weaponTBoxTextRegionUPtr_->SetEntityPos(
            (turnBoxRegion_.left + (turnBoxRegion_.width * 0.5f))
                - (weaponTBoxTextRegionUPtr_->GetEntityRegion().width * 0.5f),
            (titleTBoxTextRegionUPtr_->GetEntityRegion().top
             + titleTBoxTextRegionUPtr_->GetEntityRegion().height)
                - VERT_POS_SHIFT);

        auto const WEAPON_TBOXTEXT_REGION { weaponTBoxTextRegionUPtr_->GetEntityRegion() };
        armorTBoxTextRegionUPtr_->SetText(armorSS.str());

        armorTBoxTextRegionUPtr_->SetEntityPos(
            (turnBoxRegion_.left + (turnBoxRegion_.width * 0.5f))
                - (armorTBoxTextRegionUPtr_->GetEntityRegion().width * 0.5f),
            (WEAPON_TBOXTEXT_REGION.top + WEAPON_TBOXTEXT_REGION.height) - VERT_POS_SHIFT);

        enemyCondsTBoxRegionUPtr_->SetText(enemyCondsSS.str());

        enemyCondsTBoxRegionUPtr_->SetEntityPos(
            (turnBoxRegion_.left + (turnBoxRegion_.width * 0.5f))
                - (enemyCondsTBoxRegionUPtr_->GetEntityRegion().width * 0.5f),
            (armorTBoxTextRegionUPtr_->GetEntityRegion().top
             + armorTBoxTextRegionUPtr_->GetEntityRegion().height)
                - VERT_POS_SHIFT);

        auto enemyActionPosLeft { (turnBoxRegion_.left + (turnBoxRegion_.width * 0.5f))
                                  - (enemyActionTBoxRegionUPtr_->GetEntityRegion().width * 0.5f) };

        auto enemyActionPosTop { (enemyCondsTBoxRegionUPtr_->GetEntityRegion().top
                                  + enemyCondsTBoxRegionUPtr_->GetEntityRegion().height)
                                 - VERT_POS_SHIFT };

        if (enemyCondsSS.str() == EMPTY_STR)
        {
            enemyActionPosTop = (armorTBoxTextRegionUPtr_->GetEntityRegion().top
                                 + armorTBoxTextRegionUPtr_->GetEntityRegion().height)
                - VERT_POS_SHIFT;
        }

        enemyActionTBoxRegionUPtr_->SetText(preambleSS.str());

        if (isPreambleShowing
            || ((TurnPhase::PerformReport == turnPhase_)
                || (TurnPhase::PostPerformPause == turnPhase_)
                || (TurnPhase::PerformAnim == turnPhase_)))
        {
            enemyActionTBoxRegionUPtr_->SetEntityPos(
                enemyActionPosLeft, weaponTBoxTextRegionUPtr_->GetEntityPos().y);
        }
        else
        {
            enemyActionTBoxRegionUPtr_->SetEntityPos(enemyActionPosLeft, enemyActionPosTop);
        }

        infoTBoxTextRegionUPtr_->SetText(infoSS.str());
        infoTBoxTextRegionUPtr_->SetEntityPos(turnBoxRegion_.left, turnBoxRegion_.top);

        SetupTurnBoxButtons(TURN_CREATURE_PTR, (TURN_CREATURE_PTR->IsPlayerCharacter() == false));
    }

    void CombatStage::StartPerformAnim()
    {
        auto const TURN_CREATURE_PTR { turnCreaturePtrOpt_.value() };

        SetTurnPhase(TurnPhase::PerformAnim);

        switch (turnActionPhase_)
        {
            case TurnActionPhase::Run:
            {
                combatAnimationUPtr_->RunAnimStart(
                    combatDisplayStagePtr_->GetCombatNodeForCreature(TURN_CREATURE_PTR));

                slider_.Reset(ANIM_RUN_SLIDER_SPEED_);
                SetAnimPhase(AnimPhase::Run);
                break;
            }

            case TurnActionPhase::Advance:
            {
                combatDisplayStagePtr_->MoveCreatureBlockingPosition(TURN_CREATURE_PTR, true);
                PositionSlideStartTasks(TURN_CREATURE_PTR);
                SetAnimPhase(AnimPhase::AdvanceOrRetreat);
                break;
            }

            case TurnActionPhase::Retreat:
            {
                combatDisplayStagePtr_->MoveCreatureBlockingPosition(TURN_CREATURE_PTR, false);
                PositionSlideStartTasks(TURN_CREATURE_PTR);
                SetAnimPhase(AnimPhase::AdvanceOrRetreat);
                break;
            }

            case TurnActionPhase::MeleeWeapon:
            {
                SetAnimPhase(AnimPhase::MoveToward);

                auto const CREATURE_EFFECTS_VEC { fightResult_.Effects() };

                M_ASSERT_OR_LOGANDTHROW_SS(
                    (CREATURE_EFFECTS_VEC.size() == 1),
                    "stage::CombatStage::StartPerformAnim(turnActionPhase_=MeleeWeapon) "
                        << "found the fightResult.Effects().size=" << CREATURE_EFFECTS_VEC.size()
                        << " instead of 1.");

                combatAnimationUPtr_->MeleeMoveTowardAnimStart(
                    combatDisplayStagePtr_->GetCombatNodeForCreature(TURN_CREATURE_PTR),
                    combatDisplayStagePtr_->GetCombatNodeForCreature(
                        CREATURE_EFFECTS_VEC[0].GetCreature()));

                slider_.Reset(ANIM_MELEE_MOVE_SLIDER_SPEED_);
                break;
            }

            case TurnActionPhase::ShootSling:
            case TurnActionPhase::ShootArrow:
            case TurnActionPhase::ShootBlowpipe:
            {
                // Note:  There should only be one CreatureEffect and one HitInfoVec when
                // attacking with a projectile
                auto const HIT_INFO { fightResult_.GetHitInfo(0, 0) };
                if (HIT_INFO.IsValid())
                {
                    // at this point we are guaranteed fightResult_ contains at least one
                    // CreatureEffect and one HitInfo
                    auto const WEAPON_PTR_OPT { HIT_INFO.Weapon() };
                    if (WEAPON_PTR_OPT)
                    {
                        combatSoundEffects_.PlayShoot(WEAPON_PTR_OPT.value());

                        auto const CREATURE_ATTACKING_CENTER_POSV {
                            combatDisplayStagePtr_->GetCombatNodeCenter(TURN_CREATURE_PTR)
                        };

                        auto const CREATURE_DEFENDING_CENTER_POSV {
                            combatDisplayStagePtr_->GetCombatNodeCenter(
                                fightResult_.Effects()[0].GetCreature())
                        };

                        combatAnimationUPtr_->ProjectileShootAnimStart(
                            CREATURE_ATTACKING_CENTER_POSV,
                            CREATURE_DEFENDING_CENTER_POSV,
                            WEAPON_PTR_OPT.value(),
                            HIT_INFO.WasHit());

                        slider_.Reset(ANIM_PROJECTILE_SHOOT_SLIDER_SPEED_);
                        SetAnimPhase(AnimPhase::ProjectileShoot);
                        break;
                    }
                }

                SetAnimPhase(AnimPhase::NotAnimating);
                SetTurnPhase(TurnPhase::PostPerformPause);
                break;
            }

            case TurnActionPhase::PauseAndReport:
            {
                SetTurnPhase(TurnPhase::PerformReport);
                SetTurnActionPhase(TurnActionPhase::None);
                SetAnimPhase(AnimPhase::NotAnimating);
                StartPause(PERFORM_PRE_REPORT_PAUSE_SEC_, "PerformPreReport (PauseAndReport)");
                break;
            }

            case TurnActionPhase::Cast:
            {
                AppendStatusMessage(
                    combatText_.ActionText(
                        TURN_CREATURE_PTR, turnActionInfo_, fightResult_, true, true),
                    false);

                combatSoundEffects_.PlaySpell(spellBeingCastPtrOpt_.value());

                // start sparkle anim for creature doing the casting
                combatAnimationUPtr_->SparkleAnimStart(
                    { combatDisplayStagePtr_->GetCombatNodeForCreature(TURN_CREATURE_PTR) });

                // start anim for creatures being cast upon
                creature::CreaturePVec_t creaturesCastUponPVec;
                fightResult_.EffectedCreatures(creaturesCastUponPVec);

                combatAnimationUPtr_->SpellAnimStart(
                    spellBeingCastPtrOpt_.value(),
                    TURN_CREATURE_PTR,
                    combatDisplayStagePtr_->GetCombatNodesForCreatures(creaturesCastUponPVec));

                SetAnimPhase(AnimPhase::Spell);
                break;
            }

            case TurnActionPhase::Roar:
            {
                combatSoundEffects_.PlayRoar(TURN_CREATURE_PTR);

                creature::CreaturePVec_t creaturesToShakePVec { creature::Algorithms::NonPlayers(
                    creature::Algorithms::Living) };

                creaturesToShakePVec.emplace_back(TURN_CREATURE_PTR);

                combatAnimationUPtr_->ShakeAnimStart(
                    combatDisplayStagePtr_->GetCombatNodesForCreatures(creaturesToShakePVec),
                    ANIM_IMPACT_SHAKE_SLIDER_SPEED_,
                    true);

                slider_.Reset(ANIM_IMPACT_SLIDER_SPEED_);

                SetAnimPhase(AnimPhase::Roar);
                break;
            }

            case TurnActionPhase::PlaySong:
            {
                AppendStatusMessage(
                    combatText_.ActionText(
                        TURN_CREATURE_PTR, turnActionInfo_, fightResult_, true, true),
                    false);

                combatSoundEffects_.PlaySong(songBeingPlayedPtrOpt_.value());

                // start the song animation for the bard playing the music
                combatAnimationUPtr_->SongAnimStart(
                    { combatDisplayStagePtr_->GetCombatNodeForCreature(TURN_CREATURE_PTR) });

                // start the sparkle animation for all creatures listening to the song
                creature::CreaturePVec_t creaturesListeningPVec;
                fightResult_.EffectedCreatures(creaturesListeningPVec);

                combatAnimationUPtr_->SparkleAnimStart(
                    combatDisplayStagePtr_->GetCombatNodesForCreatures(creaturesListeningPVec));

                SetAnimPhase(AnimPhase::Song);
                break;
            }

            case TurnActionPhase::Pounce:
            case TurnActionPhase::None:
            case TurnActionPhase::Count:
            default:
            {
                break;
            }
        }
    }

    const std::string CombatStage::TurnPhaseToString(const TurnPhase ENUM)
    {
        switch (ENUM)
        {
            case TurnPhase::NotATurn:
            {
                return "NotATurn";
            }
            case TurnPhase::CenterAndZoomIn:
            {
                return "CenterAndZoomIn";
            }
            case TurnPhase::PostCenterAndZoomInPause:
            {
                return "PostZInPause";
            }
            case TurnPhase::Determine:
            {
                return "Determine";
            }
            case TurnPhase::TargetSelect:
            {
                return "TargetSelect";
            }
            case TurnPhase::ConditionEffectPause:
            {
                return "ConditionEffectPause";
            }
            case TurnPhase::CenterAndZoomOut:
            {
                return "CenterAndZoomOut";
            }
            case TurnPhase::PostCenterAndZoomOutPause:
            {
                return "PostZOutPause";
            }
            case TurnPhase::PerformAnim:
            {
                return "PerformAnim";
            }
            case TurnPhase::PerformReport:
            {
                return "PerformReport";
            }
            case TurnPhase::PostPerformPause:
            {
                return "PostPerformPause";
            }
            case TurnPhase::StatusAnim:
            {
                return "StatusAnim";
            }
            case TurnPhase::DeathAnim:
            {
                return "DeathAnim";
            }
            case TurnPhase::RepositionAnim:
            {
                return "RepositionAnim";
            }
            case TurnPhase::PostTurnPause:
            {
                return "PostTurnPause";
            }
            case TurnPhase::Achievements:
            {
                return "Achievements";
            }
            case TurnPhase::End:
            {
                return "End";
            }
            case TurnPhase::Count:
            default:
            {
                return "";
            }
        }
    }

    const std::string CombatStage::TurnActionPhaseToString(const TurnActionPhase ENUM)
    {
        switch (ENUM)
        {
            case TurnActionPhase::None:
            {
                return "None";
            }
            case TurnActionPhase::PauseAndReport:
            {
                return "PauseAndReport";
            }
            case TurnActionPhase::MeleeWeapon:
            {
                return "MeleeWeapon";
            }
            case TurnActionPhase::ShootSling:
            {
                return "ShootSling";
            }
            case TurnActionPhase::ShootArrow:
            {
                return "ShootArrow";
            }
            case TurnActionPhase::ShootBlowpipe:
            {
                return "ShootBlowpipe";
            }
            case TurnActionPhase::Advance:
            {
                return "Advance";
            }
            case TurnActionPhase::Retreat:
            {
                return "Retreat";
            }
            case TurnActionPhase::Cast:
            {
                return "Cast";
            }
            case TurnActionPhase::PlaySong:
            {
                return "PlaySong";
            }
            case TurnActionPhase::Roar:
            {
                return "Roar";
            }
            case TurnActionPhase::Pounce:
            {
                return "Pounce";
            }
            case TurnActionPhase::Run:
            {
                return "Run";
            }
            case TurnActionPhase::Count:
            default:
            {
                return "";
            }
        }
    }

    const std::string CombatStage::PreTurnPhaseToString(const PreTurnPhase ENUM)
    {
        switch (ENUM)
        {
            case PreTurnPhase::Start:
            {
                return "Start";
            }
            case PreTurnPhase::PanToCenter:
            {
                return "PanToCenter";
            }
            case PreTurnPhase::PostPanPause:
            {
                return "PostPanPause";
            }
            case PreTurnPhase::ZoomOut:
            {
                return "ZOut";
            }
            case PreTurnPhase::PostZoomOutPause:
            {
                return "PostZOutPause";
            }
            case PreTurnPhase::End:
            {
                return "End";
            }
            case PreTurnPhase::Count:
            default:
            {
                return "";
            }
        }
    }

    const std::string CombatStage::AnimPhaseToString(const AnimPhase ENUM)
    {
        switch (ENUM)
        {
            case AnimPhase::NotAnimating:
            {
                return "NotAnimating";
            }
            case AnimPhase::AdvanceOrRetreat:
            {
                return "AdvanceOrRetreat";
            }
            case AnimPhase::ProjectileShoot:
            {
                return "ProjShoot";
            }
            case AnimPhase::MoveToward:
            {
                return "MoveToward";
            }
            case AnimPhase::PostMoveTowardPause:
            {
                return "PostTowardPause";
            }
            case AnimPhase::Impact:
            {
                return "Impact";
            }
            case AnimPhase::PostImpactPause:
            {
                return "PostImpactPause";
            }
            case AnimPhase::Spell:
            {
                return "Spell";
            }
            case AnimPhase::PostSpellPause:
            {
                return "PostSpellPause";
            }
            case AnimPhase::Song:
            {
                return "Song";
            }
            case AnimPhase::PostSongPause:
            {
                return "PostSongPause";
            }
            case AnimPhase::MoveBack:
            {
                return "MoveBack";
            }
            case AnimPhase::Roar:
            {
                return "Roar";
            }
            case AnimPhase::PostRoarPause:
            {
                return "PostRoarPause";
            }
            case AnimPhase::Run:
            {
                return "Run";
            }
            case AnimPhase::FinalPause:
            {
                return "FinalPause";
            }
            case AnimPhase::Count:
            default:
            {
                return "";
            }
        }
    }

    void CombatStage::UpdateTestingText()
    {
        std::ostringstream ss;
        ss << ((PreTurnPhase::End == preTurnPhase_) ? TurnPhaseToString(turnPhase_)
                                                    : PreTurnPhaseToString(preTurnPhase_))
           << ", " << pauseTitle_ << " " << ((IsPaused()) ? "D" : "A") << ", "
           << TurnActionPhaseToString(turnActionPhase_) << ", " << AnimPhaseToString(animPhase_);

        testingTextRegionUPtr_->SetText(ss.str());
    }

    CombatStage::TurnActionPhase
        CombatStage::GetTurnActionPhaseFromWeaponType(const item::ItemPtr_t WEAPON_PTR) const
    {
        if (WEAPON_PTR->WeaponInfo().ProjectileType() == item::weapon::projectile_type::Sling)
        {
            return TurnActionPhase::ShootSling;
        }
        else if (
            WEAPON_PTR->WeaponInfo().IsBow()
            || WEAPON_PTR->WeaponInfo().ProjectileType() == item::weapon::projectile_type::Crossbow)
        {
            return TurnActionPhase::ShootArrow;
        }
        else if (
            WEAPON_PTR->WeaponInfo().ProjectileType() == item::weapon::projectile_type::Blowpipe)
        {
            return TurnActionPhase::ShootBlowpipe;
        }
        else
        {
            return TurnActionPhase::MeleeWeapon;
        }
    }

    CombatStage::TurnActionPhase CombatStage::GetTurnActionPhaseFromFightResult(
        const combat::FightResult & FIGHT_RESULT) const
    {
        // Note: Assume it is not possible for a weapon attack to target multiple creatures
        auto const HIT_INFO { FIGHT_RESULT.GetHitInfo(0, 0) };
        if (HIT_INFO.IsValid())
        {
            auto const WEAPON_PTR_OPT { HIT_INFO.Weapon() };
            if (WEAPON_PTR_OPT)
            {
                return GetTurnActionPhaseFromWeaponType(WEAPON_PTR_OPT.value());
            }
        }

        return TurnActionPhase::None;
    }

    void CombatStage::SetUserActionAllowed(const bool IS_ALLOWED)
    {
        combatDisplayStagePtr_->SetSummaryViewAllowed(IS_ALLOWED);
        combatDisplayStagePtr_->SetScrollingAllowed(IS_ALLOWED);
    }

    void CombatStage::HandleKilledCreatures()
    {
        for (auto const & NEXT_CREATURE_EFFECT : fightResult_.Effects())
        {
            if (NEXT_CREATURE_EFFECT.WasKill())
            {
                combat::Encounter::Instance()->HandleKilledCreature(
                    NEXT_CREATURE_EFFECT.GetCreature());
            }
        }
    }

    void CombatStage::HandleApplyDamageTasks()
    {
        // create a vec of all creatures damaged and killed this turn
        std::vector<Health_t> damageVec;
        combat::CombatNodePVec_t combatNodePVec;
        creature::CreaturePVec_t killedCreaturesPVec;

        for (auto const & NEXT_CREATURE_EFFECT : fightResult_.Effects())
        {
            if (NEXT_CREATURE_EFFECT.WasKill())
            {
                killedCreaturesPVec.emplace_back(NEXT_CREATURE_EFFECT.GetCreature());
            }

            auto const DAMAGE { NEXT_CREATURE_EFFECT.GetDamageTotal() };
            if (DAMAGE.IsNonZero())
            {
                damageVec.emplace_back(DAMAGE);

                combatNodePVec.emplace_back(combatDisplayStagePtr_->GetCombatNodeForCreature(
                    NEXT_CREATURE_EFFECT.GetCreature()));
            }
        }

        combatAnimationUPtr_->TextAnimStart(damageVec, combatNodePVec);

        // remove all conditions except for dead from the killed creatures
        auto wasPlayerKilled { false };
        for (auto const & KILLED_CREATURE_PTR : killedCreaturesPVec)
        {
            if (KILLED_CREATURE_PTR->IsDead())
            {
                // This is where player death sfx is played.
                // Non-player death sfx is played elsewhere in this file
                //(in two other places actually) where the enemy death anim starts.
                if (KILLED_CREATURE_PTR->IsPlayerCharacter())
                {
                    wasPlayerKilled = true;
                    combatSoundEffects_.PlayDeath(KILLED_CREATURE_PTR);
                }
            }

            for (auto const NEXT_CONDITION_ENUM : KILLED_CREATURE_PTR->Conditions())
            {
                if (NEXT_CONDITION_ENUM != creature::Conditions::Dead)
                {
                    KILLED_CREATURE_PTR->ConditionRemove(NEXT_CONDITION_ENUM);
                }
            }
        }

        if (wasPlayerKilled)
        {
            sfml_util::SoundManager::Instance()->SoundEffectPlay(
                sfml_util::sound_effect::CharacterDeath);
        }

        combatDisplayStagePtr_->UpdateHealthTasks();
    }

    void CombatStage::HandleAttackTasks(const creature::CreaturePtr_t CREATURE_TO_ATTACK_PTR)
    {
        auto const TURN_CREATURE_PTR { turnCreaturePtrOpt_.value() };

        SetUserActionAllowed(false);

        turnActionInfo_
            = combat::TurnActionInfo(combat::TurnAction::Attack, { CREATURE_TO_ATTACK_PTR });

        combat::Encounter::Instance()->SetTurnActionInfo(TURN_CREATURE_PTR, turnActionInfo_);

        fightResult_ = creatureInteraction_.Fight(TURN_CREATURE_PTR, CREATURE_TO_ATTACK_PTR);

        AppendStatusMessage(
            combatText_.ActionText(TURN_CREATURE_PTR, turnActionInfo_, fightResult_, true, true),
            false);

        SetTurnActionPhase(GetTurnActionPhaseFromFightResult(fightResult_));

        SetTurnPhase(TurnPhase::CenterAndZoomOut);

        AnimationCenteringStart(
            creature::CreaturePVec_t { TURN_CREATURE_PTR, CREATURE_TO_ATTACK_PTR });

        slider_.Reset(ANIM_CENTERING_SLIDER_SPEED_);

        SetupTurnBox();
    }

    bool CombatStage::HandleMiscCancelTasks()
    {
        // cancel summary view if visible or even just starting
        if (combatDisplayStagePtr_->GetIsSummaryViewInProgress())
        {
            combatDisplayStagePtr_->CancelSummaryViewAndStartTransitionBack();
            return true;
        }

        // cancel pause if paused
        if (IsPaused())
        {
            isPauseCanceled_ = true;
            return true;
        }

        // this interrupts a status animation in progress...
        if (willClrShkInitStatusMsg_)
        {
            willClrShkInitStatusMsg_ = false;
            combatDisplayStagePtr_->SetIsStatusMessageAnimating(false);
            return true;
        }

        //...and so does this
        if (TurnPhase::StatusAnim == turnPhase_)
        {
            SetTurnPhase(TurnPhase::PostTurnPause);
            StartPause(POST_TURN_PAUSE_SEC_, "PostTurn");
            combatDisplayStagePtr_->SetIsStatusMessageAnimating(false);
            return true;
        }

        return false;
    }

    void CombatStage::HandlePlayingMeleeSoundEffects()
    {
        auto const CREATURE_EFFECTS_VEC(fightResult_.Effects());
        if (performReportEffectIndex_ >= CREATURE_EFFECTS_VEC.size())
        {
            return;
        }

        auto const HIT_INFO_VEC { CREATURE_EFFECTS_VEC[performReportEffectIndex_].GetHitInfoVec() };
        if (performReportHitIndex_ >= HIT_INFO_VEC.size())
        {
            return;
        }

        auto const HIT_INFO { HIT_INFO_VEC[performReportHitIndex_] };

        if (HIT_INFO.IsWeapon())
        {
            auto const REPORT_INDICIES { ReportIndicies(
                performReportEffectIndex_, performReportHitIndex_) };

            if (soundEffectsPlayedSet_.find(REPORT_INDICIES) == soundEffectsPlayedSet_.end())
            {
                soundEffectsPlayedSet_.insert(REPORT_INDICIES);
                combatSoundEffects_.PlayHitOrMiss(turnCreaturePtrOpt_.value(), HIT_INFO);
            }
        }
    }

    bool CombatStage::HandleWin() { return DetectWinOrLose(true); }

    bool CombatStage::HandleLose() { return DetectWinOrLose(false); }

    bool CombatStage::DetectWinOrLose(const bool IS_DETECTING_WIN)
    {
        // TODO
        // Don't bother checking for the case where all enemies ran away,
        // because they cannot run away yet.

        // detect player runaway case
        if (IS_DETECTING_WIN == false)
        {
            auto const ALL_LIVING_PVEC { creature::Algorithms::PlayersByType(
                creature::Algorithms::TypeOpt::Player, creature::Algorithms::Runaway) };

            auto didAnyPlayersRunAway { false };
            auto areAllNonRunawaysIncapacitated { true };
            for (auto const & NEXT_LIVING_PTR : ALL_LIVING_PVEC)
            {
                if (combat::Encounter::Instance()->IsRunaway(NEXT_LIVING_PTR))
                {
                    didAnyPlayersRunAway = true;
                }
                else if (
                    NEXT_LIVING_PTR->HasConditionNotAThreatPerm(creature::UnconOpt::Include)
                    == false)
                {
                    areAllNonRunawaysIncapacitated = false;
                }
            }

            if (didAnyPlayersRunAway && areAllNonRunawaysIncapacitated)
            {
                game::LoopManager::Instance()->PopupWaitBeginSpecific<popup::PopupStageCombatOver>(
                    this,
                    popup::PopupManager::Instance()->CreateCombatOverPopupInfo(
                        POPUP_NAME_COMBATOVER_RAN_, combat::CombatEnd::Ran));

                EndOfCombatCleanup();
                return true;
            }
        }

        // detect all incapacitated cases
        auto const ALL_LIVING_PVEC { creature::Algorithms::PlayersByType(
            ((IS_DETECTING_WIN) ? creature::Algorithms::TypeOpt::NonPlayer
                                : creature::Algorithms::TypeOpt::Player)) };

        auto areAllIncapacitated { true };
        for (auto const & NEXT_LIVING_PTR : ALL_LIVING_PVEC)
        {
            if (NEXT_LIVING_PTR->HasConditionNotAThreatPerm(creature::UnconOpt::Include) == false)
            {
                areAllIncapacitated = false;
                break;
            }
        }

        if (areAllIncapacitated)
        {
            game::LoopManager::Instance()->PopupWaitBeginSpecific<popup::PopupStageCombatOver>(
                this,
                popup::PopupManager::Instance()->CreateCombatOverPopupInfo(
                    ((IS_DETECTING_WIN) ? POPUP_NAME_COMBATOVER_WIN_ : POPUP_NAME_COMBATOVER_LOSE_),
                    ((IS_DETECTING_WIN) ? combat::CombatEnd::Win : combat::CombatEnd::Lose)));

            EndOfCombatCleanup();
        }

        return hasCombatEnded_;
    }

    void CombatStage::EndOfCombatCleanup()
    {
        hasCombatEnded_ = true;
        combat::Encounter::Instance()->EndCombatTasks();
        turnCreaturePtrOpt_ = boost::none;
        combatAnimationUPtr_->EndOfCombatCleanup();
        combatDisplayStagePtr_->EndOfCombatCleanup();
    }

    void CombatStage::SystemErrorPopup(
        const std::string & GENERAL_ERROR_MSG,
        const std::string & TECH_ERROR_MSG,
        const std::string & TITLE_MSG)
    {
        game::LoopManager::Instance()->PopupWaitBeginSpecific<popup::PopupStageSystemError>(
            this,
            popup::PopupManager::Instance()->CreateSystemErrorPopupInfo(
                "Stage'sSystemErrorPopupName", GENERAL_ERROR_MSG, TECH_ERROR_MSG, TITLE_MSG));
    }

    bool CombatStage::PopulateAchievementsVec()
    {
        auto const TURN_CREATURE_PTR { turnCreaturePtrOpt_.value() };

        creatureTitlesVec_.clear();

        // BeastMindLinks
        // MoonHowls
        // PackActions
        // TODO -these achievements are probably not going to be implemented

        // LocksPicked
        // BattlesSurvived
        // EnemiesFaced
        // TODO -these achievements must be handled elsewhere

        // HealthGiven
        // HealthTraded
        // BackstabsHits
        // SpiritsLifted
        // TODO -these achievements still need to be implemented

        // gather achievements from fightResult_
        auto const IS_TURN_CREATURE_FLYING {
            combat::Encounter::Instance()->GetTurnInfoCopy(TURN_CREATURE_PTR).GetIsFlying()
        };

        auto flyingAttackHits { 0 };
        auto meleeHits { 0 };
        auto projectileHits { 0 };
        creature::CreaturePVec_t playersDodgedStandingPVec;
        creature::CreaturePVec_t playersDodgedFlyingPVec;
        auto didCastSpell { false };
        auto didPlaySong { false };
        auto didRoar { false };

        for (auto const & NEXT_CREATURE_EFFECT : fightResult_.Effects())
        {
            auto const NEXT_EFFECTED_CREATURE_PTR { NEXT_CREATURE_EFFECT.GetCreature() };

            auto const IS_EFFECTED_CREATURE_FLYING { combat::Encounter::Instance()
                                                         ->GetTurnInfoCopy(
                                                             NEXT_EFFECTED_CREATURE_PTR)
                                                         .GetIsFlying() };

            auto const & HIT_INFO_VEC { NEXT_CREATURE_EFFECT.GetHitInfoVec() };
            for (auto const & NEXT_HIT_INFO : HIT_INFO_VEC)
            {
                if (TURN_CREATURE_PTR->IsPlayerCharacter())
                {
                    if (NEXT_HIT_INFO.TypeOfHit() == combat::HitType::Spell)
                    {
                        didCastSpell = true;
                    }
                    else if (NEXT_HIT_INFO.TypeOfHit() == combat::HitType::Song)
                    {
                        didPlaySong = true;
                    }
                    else if (NEXT_HIT_INFO.TypeOfHit() == combat::HitType::Roar)
                    {
                        didRoar = true;
                    }
                    else if (
                        (NEXT_EFFECTED_CREATURE_PTR->IsPlayerCharacter() == false)
                        && (NEXT_HIT_INFO.TypeOfHit() == combat::HitType::Weapon)
                        && NEXT_HIT_INFO.Weapon() && NEXT_HIT_INFO.WasHit())
                    {
                        if (IS_TURN_CREATURE_FLYING)
                        {
                            ++flyingAttackHits;
                        }

                        if (NEXT_HIT_INFO.Weapon().value()->WeaponType()
                            & item::weapon_type::Projectile)
                        {
                            ++projectileHits;
                        }
                        else if (
                            NEXT_HIT_INFO.Weapon().value()->WeaponType() & item::weapon_type::Melee)
                        {
                            ++meleeHits;
                        }
                    }
                }
                else if (
                    (TURN_CREATURE_PTR->IsPlayerCharacter() == false)
                    && NEXT_EFFECTED_CREATURE_PTR->IsPlayerCharacter()
                    && (NEXT_HIT_INFO.WasHit() == false))
                {
                    if (IS_EFFECTED_CREATURE_FLYING)
                    {
                        playersDodgedFlyingPVec.emplace_back(NEXT_EFFECTED_CREATURE_PTR);
                    }
                    else
                    {
                        playersDodgedStandingPVec.emplace_back(NEXT_EFFECTED_CREATURE_PTR);
                    }
                }
            }
        }

        HandleAchievementEnqueue(
            TURN_CREATURE_PTR, creature::AchievementType::FlyingAttackHits, flyingAttackHits);

        HandleAchievementEnqueue(
            TURN_CREATURE_PTR, creature::AchievementType::MeleeHits, meleeHits);

        HandleAchievementEnqueue(
            TURN_CREATURE_PTR, creature::AchievementType::ProjectileHits, projectileHits);

        for (auto const & NEXT_DODGING_CREATURE_PTR : playersDodgedStandingPVec)
        {
            HandleAchievementEnqueue(
                NEXT_DODGING_CREATURE_PTR, creature::AchievementType::DodgedStanding);
        }

        for (auto const & NEXT_DODGING_CREATURE_PTR : playersDodgedFlyingPVec)
        {
            HandleAchievementEnqueue(
                NEXT_DODGING_CREATURE_PTR, creature::AchievementType::DodgedFlying);
        }

        if (didPlaySong)
        {
            HandleAchievementEnqueue(TURN_CREATURE_PTR, creature::AchievementType::SongsPlayed);
        }

        if (didRoar)
        {
            HandleAchievementEnqueue(TURN_CREATURE_PTR, creature::AchievementType::BeastRoars);
        }

        if (TURN_CREATURE_PTR->IsPlayerCharacter() && IS_TURN_CREATURE_FLYING)
        {
            HandleAchievementEnqueue(TURN_CREATURE_PTR, creature::AchievementType::TurnsInFlight);
        }

        if (didCastSpell)
        {
            HandleAchievementEnqueue(TURN_CREATURE_PTR, creature::AchievementType::SpellsCast);
        }

        return (creatureTitlesVec_.empty() == false);
    }

    void CombatStage::HandleAchievementEnqueue(
        const creature::CreaturePtr_t CREATURE_PTR,
        const creature::AchievementType::Enum ACH_ENUM,
        const int COUNT)
    {
        for (int i(0); i < COUNT; ++i)
        {
            creature::TitlePtrOpt_t fromTitlePtrOpt { boost::none };
            creature::TitlePtrOpt_t toTitlePtrOpt { boost::none };

            if (stage::IncrementAchievementAndReturnTrueIfProcessingRequired(
                    CREATURE_PTR, ACH_ENUM, fromTitlePtrOpt, toTitlePtrOpt))
            {
                creatureTitlesVec_.emplace_back(
                    creature::TitleTransition(CREATURE_PTR, fromTitlePtrOpt, toTitlePtrOpt.get()));
            }
        }
    }

    bool CombatStage::HandleAchievementPopups()
    {
        if (creatureTitlesVec_.empty())
        {
            SetTurnPhase(TurnPhase::End);
            StartPause(END_PAUSE_SEC_, "End");

            // return true because a popup will NOT be raised
            return true;
        }
        else
        {
            auto const TITLE_TRANSITION { creatureTitlesVec_[0] };

            creatureTitlesVec_.erase(
                std::remove(creatureTitlesVec_.begin(), creatureTitlesVec_.end(), TITLE_TRANSITION),
                creatureTitlesVec_.end());

            stage::ApplyTitleChangesToCreature(
                TITLE_TRANSITION.creaturePtr, TITLE_TRANSITION.toTitlePtr);

            stage::TitleTransitionPopup(
                this,
                POPUP_NAME_ACHIEVEMENT_,
                TITLE_TRANSITION.creaturePtr,
                TITLE_TRANSITION.fromTitlePtrOpt,
                TITLE_TRANSITION.toTitlePtr);

            // return false because a popup will follow a popup
            return false;
        }
    }

    void CombatStage::PerformRoarEffects()
    {
        fightResult_
            = creatureInteraction_.Roar(turnCreaturePtrOpt_.value(), combatDisplayStagePtr_);
    }

    void CombatStage::AnimationCenteringStart(const creature::CreaturePVec_t & CREATURE_PVEC)
    {
        combatAnimationUPtr_->CenteringStart(
            CREATURE_PVEC,
            combatDisplayStagePtr_->FindCenterOfCreatures(CREATURE_PVEC),
            combatDisplayStagePtr_->IsZoomOutRequired(CREATURE_PVEC),
            combatDisplayStagePtr_);
    }

    void CombatStage::MakeButton(
        sfml_util::gui::ImageTextEntityUPtr_t & buttonUPtr,
        const std::string & TEXT,
        const std::string & HOVER_TEXT)
    {
        sfml_util::gui::TextInfo turnButtonTextInfo(
            " ",
            sfml_util::FontManager::Instance()->GetFont(sfml_util::GuiFont::System),
            sfml_util::FontManager::Instance()->Size_Normal(),
            sfml_util::defaults::Orange,
            sfml_util::Justified::Left);

        const auto BUTTON_COLOR_OVER { sf::Color::White };
        const auto BUTTON_COLOR_DOWN { sfml_util::defaults::Light };
        const auto BUTTON_COLOR_DISABLED { sfml_util::defaults::Orange - sf::Color(0, 0, 0, 176) };

        buttonUPtr = std::make_unique<sfml_util::gui::ImageTextEntity>(
            "CombatStage's_" + TEXT,
            sfml_util::gui::MouseImageInfo(),
            sfml_util::gui::MouseTextInfo(
                sfml_util::gui::TextInfo(turnButtonTextInfo, TEXT),
                BUTTON_COLOR_DOWN,
                BUTTON_COLOR_OVER,
                BUTTON_COLOR_DISABLED),
            sfml_util::gui::ImageTextEntity::Callback_t::IHandlerPtr_t(this),
            sfml_util::gui::ImageTextEntity::MouseStateSync::Text);

        buttonUPtr->SetMouseHoverText(HOVER_TEXT);
        EntityAdd(buttonUPtr.get());
    }

    void CombatStage::SetButtonDisabledIf(
        sfml_util::gui::ImageTextEntityUPtr_t & buttonUPtr, const bool WILL_DISABLE)
    {
        if (WILL_DISABLE)
        {
            buttonUPtr->SetMouseState(sfml_util::MouseState::Disabled);
        }
        else
        {
            buttonUPtr->SetMouseState(sfml_util::MouseState::Up);
        }
    }

} // namespace stage
} // namespace heroespath
