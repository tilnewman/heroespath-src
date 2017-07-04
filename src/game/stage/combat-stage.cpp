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
// combat-stage.cpp
//
#include "combat-stage.hpp"

#include "sfml-util/sfml-util.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/tile.hpp"
#include "sfml-util/gui/box.hpp"
#include "sfml-util/gui/popup-manager.hpp"
#include "sfml-util/gui/list-box-item.hpp"
#include "sfml-util/sparks-animation.hpp"
#include "sfml-util/cloud-animation.hpp"
#include "sfml-util/song-animation.hpp"
#include "sfml-util/sparkle-animation.hpp"
#include "sfml-util/text-animation.hpp"
#include "sfml-util/animation.hpp"

#include "game/log-macros.hpp"
#include "game/game.hpp"
#include "game/game-data-file.hpp"
#include "game/loop-manager.hpp"
#include "game/player/party.hpp"
#include "game/player/initial.hpp"
#include "game/player/character.hpp"
#include "game/non-player/party.hpp"
#include "game/non-player/character.hpp"
#include "game/state/game-state.hpp"
#include "game/state/game-state-factory.hpp"
#include "game/combat/encounter.hpp"
#include "game/combat/combat-display.hpp"
#include "game/combat/turn-decider.hpp"
#include "game/combat/combat-node.hpp"
#include "game/combat/fight.hpp"
#include "game/combat/turn-action-enum.hpp"
#include "game/combat/combat-anim.hpp"
#include "game/combat/combat-text.hpp"
#include "game/combat/condition-effects.hpp"
#include "game/creature/name-info.hpp"
#include "game/creature/conditions.hpp"
#include "game/creature/condition-algorithms.hpp"
#include "game/creature/title-warehouse.hpp"
#include "game/creature/algorithms.hpp"
#include "game/creature/stats.hpp"
#include "game/item/item.hpp"
#include "game/item/weapon-factory.hpp"
#include "game/item/armor-factory.hpp"
#include "game/item/algorithms.hpp"
#include "game/spell/spell-base.hpp"
#include "game/phase-enum.hpp"
#include "game/stats/stat-enum.hpp"

#include "misc/real.hpp"
#include "misc/random.hpp"
#include "misc/vectors.hpp"

#include <sstream>
#include <exception>
#include <vector>
#include <string>


namespace game
{
namespace stage
{

    const std::string CombatStage::POPUP_NAME_SPELLBOOK_        { "SPELLBOOK_POPUP_WINDOW_NAME" };
    const std::string CombatStage::POPUP_NAME_COMBATOVER_WIN_   { "COMBAT_OVER_WIN_POPUP_WINDOW_NAME" };
    const std::string CombatStage::POPUP_NAME_COMBATOVER_LOSE_  { "COMBAT_OVER_LOSE_POPUP_WINDOW_NAME" };
    //
    const float CombatStage::PAUSE_LONG_SEC_                { 6.0f };
    const float CombatStage::PAUSE_MEDIUM_SEC_              { 3.0f };
    const float CombatStage::PAUSE_SHORT_SEC_               { 1.5f };
    const float CombatStage::PAUSE_SHORTER_SEC_             { 0.75f };
    const float CombatStage::PAUSE_ZERO_SEC_                { 0.1f };
    const float CombatStage::POST_PAN_PAUSE_SEC_            { PAUSE_SHORT_SEC_ };
    const float CombatStage::POST_ZOOMOUT_PAUSE_SEC_        { PAUSE_SHORT_SEC_ };
    const float CombatStage::POST_ZOOMIN_PAUSE_SEC_         { PAUSE_SHORT_SEC_ };
    const float CombatStage::CREATURE_TURN_PAUSE_SEC_       { PAUSE_LONG_SEC_ };
    const float CombatStage::PRE_TURN_PAUSE_SEC_            { PAUSE_SHORT_SEC_ };
    const float CombatStage::POST_TURN_PAUSE_SEC_           { PAUSE_ZERO_SEC_ };
    const float CombatStage::PERFORM_PRE_REPORT_PAUSE_SEC_  { PAUSE_ZERO_SEC_ };
    const float CombatStage::PERFORM_REPORT_PAUSE_SEC_      { PAUSE_LONG_SEC_ };
    const float CombatStage::POST_PERFORM_REPORT_PAUSE_SEC_ { PAUSE_LONG_SEC_ };
    const float CombatStage::POST_ZOOM_TURN_PAUSE_SEC_      { PAUSE_MEDIUM_SEC_ };
    const float CombatStage::STATUSMSG_ANIM_PAUSE_SEC_      { PAUSE_SHORT_SEC_ };
    const float CombatStage::POST_MELEEMOVE_ANIM_PAUSE_SEC_ { PAUSE_ZERO_SEC_ };
    const float CombatStage::POST_IMPACT_ANIM_PAUSE_SEC_    { PAUSE_ZERO_SEC_ };
    const float CombatStage::CONDITION_EFFECT_PAUSE_SEC_    { PAUSE_LONG_SEC_ };
    const float CombatStage::POST_SPELL_ANIM_PAUSE_SEC_     { PAUSE_SHORT_SEC_ };
    //
    const float CombatStage::DOUBLE_CLICK_WINDOW_SEC_{ 0.4f };
    //
    const float CombatStage::SLIDER_SPEED_SLOWEST_               { 1.0f };
    const float CombatStage::SLIDER_SPEED_SLOW_                  { SLIDER_SPEED_SLOWEST_ * 2.0f };
    const float CombatStage::SLIDER_SPEED_NORMAL_                { SLIDER_SPEED_SLOW_ * 2.0f };
    const float CombatStage::SLIDER_SPEED_FAST_                  { SLIDER_SPEED_NORMAL_ * 2.0f };
    const float CombatStage::SLIDER_SPEED_FASTEST_               { SLIDER_SPEED_NORMAL_ * 10.0f };
    const float CombatStage::ZOOM_SLIDER_SPEED_                  { SLIDER_SPEED_SLOW_ };
    const float CombatStage::TEXT_COLOR_SHAKER_SPEED_            { SLIDER_SPEED_FASTEST_ };
    const float CombatStage::ANIM_CENTERING_SLIDER_SPEED_        { SLIDER_SPEED_NORMAL_ };
    const float CombatStage::ANIM_INITIAL_CENTERING_SLIDER_SPEED_{ SLIDER_SPEED_SLOWEST_ };
    const float CombatStage::ANIM_CREATURE_POS_SLIDER_SPEED_     { SLIDER_SPEED_NORMAL_ };
    const float CombatStage::ANIM_PROJECTILE_SHOOT_SLIDER_SPEED_ { SLIDER_SPEED_NORMAL_ };
    const float CombatStage::ANIM_DEATH_SLIDER_SPEED_            { SLIDER_SPEED_SLOWEST_ };
    const float CombatStage::ANIM_MELEE_MOVE_SLIDER_SPEED_       { SLIDER_SPEED_FAST_ };
    const float CombatStage::ANIM_IMPACT_SLIDER_SPEED_           { SLIDER_SPEED_NORMAL_ };
    const float CombatStage::ANIM_CREATURE_SHAKE_SLIDER_SPEED_   { SLIDER_SPEED_FASTEST_ };
    const float CombatStage::ANIM_IMPACT_SHAKE_SLIDER_SPEED_     { SLIDER_SPEED_FASTEST_ * 3.0f };
    //
    const sf::Color CombatStage::LISTBOX_BACKGROUND_COLOR_
    { (sfml_util::FontManager::Color_Orange() - sf::Color(100, 100, 100, 235)) };

    const sf::Color CombatStage::LISTBOX_HIGHLIGHT_COLOR_
    { (sfml_util::FontManager::Color_Orange() - sf::Color(100, 100, 100, 235)) +
        sf::Color(20, 20, 20, 20) };

    const sf::Color CombatStage::LISTBOX_HIGHLIGHT_ALT_COLOR_
    { (sfml_util::FontManager::Color_Orange() - sf::Color(100, 100, 100, 235)) +
        sf::Color(40, 40, 40, 40) };

    const sf::Color CombatStage::LISTBOX_SELECTED_COLOR_   { sf::Color::White };
    const sf::Color CombatStage::LISTBOX_NOTSELECTED_COLOR_{ sf::Color(150, 150, 150) };
    const sf::Color CombatStage::LISTBOX_LINE_COLOR_       { sf::Color(255, 255, 255, 25) };
    //
    combat::RestoreInfo CombatStage::restoreInfo_;


    CombatStage::CombatStage(const bool WILL_ADVANCE_TURN)
    :
        Stage                       ("Combat"),
        WILL_ADVANCE_TURN_          (WILL_ADVANCE_TURN),
        SCREEN_WIDTH_               (sfml_util::Display::Instance()->GetWinWidth()),
        SCREEN_HEIGHT_              (sfml_util::Display::Instance()->GetWinHeight()),
        commandBoxUPtr_             (),
        statusBoxSPtr_              (),

        statusBoxTextInfo_          (" ",
                                     sfml_util::FontManager::Instance()->Font_Typical(),
                                     sfml_util::FontManager::Instance()->Size_Small(),
                                     sfml_util::FontManager::Color_Orange(),
                                     sfml_util::Justified::Left),

        zoomSliderBarUPtr_          (),
        turnBoxUPtr_                (),
        turnBoxRegion_              (),
        combatSoundEffects_         (),
        soundEffectsPlayedSet_      (),
        turnPhase_                  (TurnPhase::NotATurn),
        preTurnPhase_               (PreTurnPhase::Start),
        turnActionPhase_            (TurnActionPhase::None),
        animPhase_                  (AnimPhase::NotAnimating),
        spellBeingCastPtr_          (nullptr),
        performReportEffectIndex_   (0),
        performReportHitIndex_      (0),
        zoomSliderOrigPos_          (0.0f),
        willClrShkInitStatusMsg_    (false),
        isShortPostZoomOutPause_    (false),
        hasCombatEnded_             (false),
        conditionEffectsVec_        (),
        conditionEffectsIndex_      (0),
        conditionEffectsTookTurn_   (false),
        conditionEffectsCenterPosV_ (0.0f, 0.0f),
        conditionEffectsWillSkip_   (false),
        
        //initiall speed ignored because speed is set before each use,
        //any value greater than zero will work here
        slider_                     (1.0f),

        combatDisplayStagePtr_      (new combat::CombatDisplay()),

        combatAnimationUPtr_        ( std::make_unique<combat::CombatAnimation>(
                                        combatDisplayStagePtr_) ),
        settingsButtonSPtr_         (),
        pauseDurationSec_           (0.0f),

        //anything greater than pauseTimeDurationSecs_ will work here
        pauseElapsedSec_            (pauseDurationSec_ + 1.0f),

        isPauseCanceled_            (false),
        turnCreaturePtr_            (nullptr),

        goldTextColorShaker_        (sfml_util::FontManager::Color_Orange(),
                                     sf::Color::White,
                                     TEXT_COLOR_SHAKER_SPEED_),

        redTextColorShaker_         (sf::Color(255, 127, 127),
                                     sf::Color::White,
                                     TEXT_COLOR_SHAKER_SPEED_ * 0.65f),
        turnActionInfo_             (),
        fightResult_                (),
        willRedColorShakeWeaponText_(false),
        isFightResultCollapsed_     (false),
        titleTBoxTextRegionUPtr_    (),
        weaponTBoxTextRegionUPtr_   (),
        armorTBoxTextRegionUPtr_    (),
        infoTBoxTextRegionUPtr_     (),
        enemyActionTBoxRegionUPtr_  (),
        enemyCondsTBoxRegionUPtr_   (),
        zoomLabelTextRegionUPtr_    (),
        attackTBoxButtonSPtr_       (),
        fightTBoxButtonSPtr_        (),
        castTBoxButtonSPtr_         (),
        advanceTBoxButtonSPtr_      (),
        retreatTBoxButtonSPtr_      (),
        blockTBoxButtonSPtr_        (),
        skipTBoxButtonSPtr_         (),
        flyTBoxButtonSPtr_          (),
        landTBoxButtonSPtr_         (),
        roarTBoxButtonSPtr_         (),
        pounceTBoxButtonSPtr_       (),
        tBoxStdButtonSVec_          (),
        tBoxBeastButtonSVec_        (),

        //anything greater than STATUSMSG_ANIM_PAUSE_SEC_ will work here
        statusMsgAnimTimerSec_      (STATUSMSG_ANIM_PAUSE_SEC_ + 1.0f),

        statusMsgAnimColorShaker_   (LISTBOX_HIGHLIGHT_COLOR_,
                                     LISTBOX_HIGHLIGHT_ALT_COLOR_,
                                     35.0f,
                                     false),

        testingTextRegionUPtr_      (),
        pauseTitle_                 (""),
        clickTimerSec_              (-1.0f),//any negative value will work here
        clickPosV_                  (0.0f, 0.0f)
    {
        restoreInfo_.CanTurnAdvance(false);
    }


    CombatStage::~CombatStage()
    {
        ClearAllEntities();
    }


    bool CombatStage::HandleCallback(const sfml_util::gui::callback::ListBoxEventPackage &)
    {
        return false;
    }


    bool CombatStage::HandleCallback(
        const sfml_util::gui::callback::FourStateButtonCallbackPackage_t & PACKAGE)
    {
        if ((IsPlayerCharacterTurnValid() == false) ||
            (TurnPhase::Determine != turnPhase_))
        {
            return false;
        }

        if (PACKAGE.PTR_ == attackTBoxButtonSPtr_.get())
        {
            return HandleAttack();
        }

        if (PACKAGE.PTR_ == fightTBoxButtonSPtr_.get())
        {
            return HandleFight();
        }

        if (PACKAGE.PTR_ == castTBoxButtonSPtr_.get())
        {
            if (turnCreaturePtr_ == nullptr)
            {
                return false;
            }
            else
            {
                if (turnCreaturePtr_->Role().Which() == creature::role::Bard)
                {
                    return HandleSong_Step1_ValidatePlayAndSelectSong();
                }
                else
                {
                    return HandleCast_Step1_ValidateCastAndSelectSpell();
                }
            }
        }

        if (PACKAGE.PTR_ == advanceTBoxButtonSPtr_.get())
        {
            return HandleAdvance();
        }

        if (PACKAGE.PTR_ == retreatTBoxButtonSPtr_.get())
        {
            return HandleRetreat();
        }

        if (PACKAGE.PTR_ == blockTBoxButtonSPtr_.get())
        {
            return HandleBlock();
        }

        if (PACKAGE.PTR_ == skipTBoxButtonSPtr_.get())
        {
            return HandleSkip();
        }

        if (PACKAGE.PTR_ == flyTBoxButtonSPtr_.get())
        {
            return HandleFly();
        }

        if (PACKAGE.PTR_ == landTBoxButtonSPtr_.get())
        {
            return HandleLand();
        }

        if (PACKAGE.PTR_ == roarTBoxButtonSPtr_.get())
        {
            return HandleRoar();
        }

        if (PACKAGE.PTR_ == pounceTBoxButtonSPtr_.get())
        {
            auto isSkyPounce(false);
            if ((TurnPhase::NotATurn != turnPhase_) && (turnCreaturePtr_ != nullptr))
            {
                isSkyPounce = combat::Encounter::Instance()->
                    GetTurnInfoCopy(turnCreaturePtr_).GetIsFlying();
            }

            return HandlePounce(isSkyPounce);
        }

        if (PACKAGE.PTR_ == settingsButtonSPtr_.get())
        {
            restoreInfo_.PrepareForStageChange(combatDisplayStagePtr_);
            game::LoopManager::Instance()->Goto_Settings();
            return true;
        }

        return false;
    }


    bool CombatStage::HandleCallback(
        const sfml_util::gui::callback::SliderBarCallbackPackage_t & PACKAGE)
    {
        if ((PACKAGE.PTR_ == zoomSliderBarUPtr_.get()) &&
            (combatDisplayStagePtr_ != nullptr))
        {
            //only zoom out half the distance that the slider actually shows
            auto const HALF_ZOOM_DIFFERENCE{ 1.0f -
                ((1.0f - PACKAGE.PTR_->GetCurrentValue()) * 0.5f) };

            combatDisplayStagePtr_->SetZoomLevel(HALF_ZOOM_DIFFERENCE);
            return true;
        }
        else
        {
            return false;
        }
    }


    bool CombatStage::HandleCallback(const game::callback::PopupResponse & POPUP_RESPONSE)
    {
        if (POPUP_RESPONSE.Info().Name() == POPUP_NAME_SPELLBOOK_)
        {
            if (POPUP_RESPONSE.Response() == sfml_util::Response::Select)
            {
                const spell::SpellPVec_t SPELLS_PVEC( turnCreaturePtr_->SpellsPVec() );
                M_ASSERT_OR_LOGANDTHROW_SS((POPUP_RESPONSE.Selection() < SPELLS_PVEC.size()),
                    "game::stage::CombatStage::HandleCallback(POPUP_RESPONSE, selection="
                    << POPUP_RESPONSE.Selection() << ") Selection was greater than SpellPVec.size="
                    << SPELLS_PVEC.size());

                auto const spellPtr{ SPELLS_PVEC.at(POPUP_RESPONSE.Selection()) };
                M_ASSERT_OR_LOGANDTHROW_SS((spellPtr != nullptr ),
                    "game::stage::CombatStage::HandleCallback(POPUP_RESPONSE, selection="
                    << POPUP_RESPONSE.Selection() << ")  SPELLS_PVEC[selection] was null.");

                turnCreaturePtr_->LastSpellCastNum(POPUP_RESPONSE.Selection());

                spellBeingCastPtr_ = spellPtr;
                HandleCast_Step2_SelectTargetOrPerformOnAll();
                return true;
            }
            else
            {
                return false;
            }
        }
        else if (POPUP_RESPONSE.Info().Name() == POPUP_NAME_COMBATOVER_WIN_)
        {
            //TODO If popup response is YES, then goto the loot stage, and if NO,
            //     then goto the adventure stage.
            game::LoopManager::Instance()->Goto_Credits();
        }
        else if (POPUP_RESPONSE.Info().Name() == POPUP_NAME_COMBATOVER_LOSE_)
        {
            //TODO if popup response is YES, then load last saved game
            game::LoopManager::Instance()->Goto_Credits();
        }

        return false;
    }


    void CombatStage::Setup()
    {
        combatDisplayStagePtr_->SetCombatAnimationPtr(combatAnimationUPtr_.get());

        //define combat region
        const float COMBAT_REGION_MARGIN(25.0f);
        const float COMBAT_REGION_TOP(COMBAT_REGION_MARGIN);
        const float COMBAT_REGION_LEFT(COMBAT_REGION_MARGIN);
        const float COMBAT_REGION_WIDTH(SCREEN_WIDTH_ - (COMBAT_REGION_MARGIN * 2.0f) + 10.0f);
        const float COMBAT_REGION_HEIGHT((SCREEN_HEIGHT_ - (COMBAT_REGION_MARGIN * 2.0f)) - sfml_util::MapByRes(200.0f, 800.0f));
        const sf::FloatRect COMBAT_REGION(COMBAT_REGION_LEFT,
                                          COMBAT_REGION_TOP,
                                          COMBAT_REGION_WIDTH,
                                          COMBAT_REGION_HEIGHT);

        //status box
        const float SLIDERBAR_VERT_SPACER(25.0f);
        const float STATUS_REGION_WIDTH_RATIO(0.666f);
        const float STATUS_REGION_SLIDERBAR_WIDTH(35.0f);
        const float STATUS_REGION_LEFT(COMBAT_REGION_LEFT);
        const float STATUS_REGION_TOP(COMBAT_REGION_TOP + COMBAT_REGION_HEIGHT + SLIDERBAR_VERT_SPACER);
        const float STATUS_REGION_WIDTH((SCREEN_WIDTH_ * STATUS_REGION_WIDTH_RATIO) - STATUS_REGION_SLIDERBAR_WIDTH);
        const float STATUS_REGION_HEIGHT((((SCREEN_HEIGHT_ - COMBAT_REGION_TOP) - COMBAT_REGION_HEIGHT) - SLIDERBAR_VERT_SPACER) - 25.0f);
        const sf::FloatRect STATUS_REGION(STATUS_REGION_LEFT,
                                          STATUS_REGION_TOP,
                                          STATUS_REGION_WIDTH,
                                          STATUS_REGION_HEIGHT);

        const sfml_util::gui::BackgroundInfo STATUS_BACKGROUNDINFO(GameDataFile::Instance()->GetMediaPath("media-images-backgrounds-tile-darkknot"), STATUS_REGION, LISTBOX_BACKGROUND_COLOR_);
        const sfml_util::gui::ColorSet STATUS_COLORSET(LISTBOX_SELECTED_COLOR_, LISTBOX_NOTSELECTED_COLOR_);
        const sfml_util::gui::box::Info STATUS_BOX_INFO(true, STATUS_REGION, STATUS_COLORSET, STATUS_BACKGROUNDINFO);
        
        statusBoxSPtr_ = std::make_shared<sfml_util::gui::ListBox>(
            "ComabtStage'sStatus",
            STATUS_REGION,
            sfml_util::gui::ListBoxItemSLst_t(),
            this,
            15.0f,
            0.0f,
            STATUS_BOX_INFO,
            LISTBOX_LINE_COLOR_,
            sfml_util::gui::ListBox::NO_LIMIT_,
            this);

        statusBoxSPtr_->SetHighlightColor(LISTBOX_HIGHLIGHT_COLOR_);
        EntityAdd(statusBoxSPtr_.get());

        //command box
        const float COMMAND_REGION_HORIZ_SPACER(25.0f);
        const float COMMAND_REGION_VERT_SPACER(25.0f);
        const float COMMAND_REGION_LEFT(STATUS_REGION_LEFT + STATUS_REGION_WIDTH + COMMAND_REGION_HORIZ_SPACER + STATUS_REGION_SLIDERBAR_WIDTH);
        const float COMMAND_REGION_TOP(COMBAT_REGION_TOP + COMBAT_REGION_HEIGHT + COMMAND_REGION_VERT_SPACER);
        const float COMMAND_REGION_WIDTH(((((SCREEN_WIDTH_ - COMMAND_REGION_HORIZ_SPACER) - COMBAT_REGION_MARGIN)- STATUS_REGION_WIDTH) - 15.0f) - STATUS_REGION_SLIDERBAR_WIDTH);
        const float COMMAND_REGION_HEIGHT(STATUS_REGION_HEIGHT);
        const sf::FloatRect COMMAND_REGION(COMMAND_REGION_LEFT,
                                           COMMAND_REGION_TOP,
                                           COMMAND_REGION_WIDTH,
                                           COMMAND_REGION_HEIGHT);
        const sfml_util::gui::BackgroundInfo COMMAND_BACKGROUNDINFO(GameDataFile::Instance()->GetMediaPath("media-images-backgrounds-tile-darkknot"), COMMAND_REGION);
        const sfml_util::gui::box::Info COMMAND_REGION_BOXINFO(true, COMMAND_REGION, sfml_util::gui::ColorSet(), COMMAND_BACKGROUNDINFO);
        commandBoxUPtr_ = std::make_unique<sfml_util::gui::box::Box>("CombatStage'sCommand", COMMAND_REGION_BOXINFO);

        //turn box
        turnBoxRegion_ = sf::FloatRect(STATUS_REGION_LEFT,
                                       STATUS_REGION_TOP,
                                       STATUS_REGION_WIDTH,
                                       STATUS_REGION_HEIGHT);

        const sfml_util::gui::BackgroundInfo TURNBOX_BACKGROUNDINFO(GameDataFile::Instance()->GetMediaPath("media-images-backgrounds-tile-darkknot"), turnBoxRegion_);
        const sfml_util::gui::box::Info TURNBOX_REGION_BOXINFO(true, turnBoxRegion_, sfml_util::gui::ColorSet(), TURNBOX_BACKGROUNDINFO);
        turnBoxUPtr_ = std::make_unique<sfml_util::gui::box::Box>("CombatStage'sTurnBox", TURNBOX_REGION_BOXINFO);

        //turnbox title text region
        const sfml_util::gui::TextInfo TURNBOXTITLE_TEXT_INFO(" ",
                                                              sfml_util::FontManager::Instance()->Font_BigFlavor1(),
                                                              sfml_util::FontManager::Instance()->Size_Largeish(),
                                                              sfml_util::FontManager::Color_Light(),
                                                              sfml_util::Justified::Center);

        sf::FloatRect turnBoxTitleTextRegion(turnBoxRegion_);
        turnBoxTitleTextRegion.height = 0.0f;

        titleTBoxTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "TurnBox'sTitle", TURNBOXTITLE_TEXT_INFO, turnBoxTitleTextRegion);

        const sfml_util::gui::TextInfo TURNBOXINFO_TEXT_INFO(
            " ",
            sfml_util::FontManager::Instance()->Font_Default2(),
            sfml_util::FontManager::Instance()->Size_Smallish(),
            sfml_util::FontManager::Color_GrayLight(),
            sfml_util::Justified::Left);

        sf::FloatRect turnBoxInfoTextRegion(turnBoxRegion_);

        infoTBoxTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "CombatStage'sTurnInfo", TURNBOXINFO_TEXT_INFO, turnBoxInfoTextRegion);

        const sfml_util::gui::TextInfo TURNBOXENEMYACTION_TEXT_INFO(
            " ",
            sfml_util::FontManager::Instance()->Font_Default2(),
            sfml_util::FontManager::Instance()->Size_Normal(),
            sfml_util::FontManager::Color_Light(),
            sfml_util::Justified::Center);

        sf::FloatRect turnBoxEnemyActionTextRegion(turnBoxRegion_);

        //the enemy turn action text can run long and thus needs to be constrained further
        auto const PAD{ 25.0f };
        turnBoxEnemyActionTextRegion.left += PAD;
        turnBoxEnemyActionTextRegion.width -= PAD * 2.0f;

        enemyActionTBoxRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "CombatStage'sTurnEnemyAction",
            TURNBOXENEMYACTION_TEXT_INFO,
            turnBoxEnemyActionTextRegion);

        const sfml_util::gui::TextInfo TURNBOXENEMYCONDS_TEXT_INFO(
            " ",
            sfml_util::FontManager::Instance()->Font_Default2(),
            sfml_util::FontManager::Instance()->Size_Smallish(),
            sfml_util::FontManager::Color_GrayLight(),
            sfml_util::Justified::Center);

        sf::FloatRect turnBoxEnemyCondsTextRegion(turnBoxRegion_);
        enemyCondsTBoxRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "CombatStage'sTurnEnemyConds",
            TURNBOXENEMYCONDS_TEXT_INFO,
            turnBoxEnemyCondsTextRegion);

        sfml_util::gui::TextInfo turnBoxWeaponTextInfo(
            " ",
            sfml_util::FontManager::Instance()->Font_Default2(),
            sfml_util::FontManager::Instance()->Size_Smallish(),
            sfml_util::FontManager::Color_GrayLight(),
            sfml_util::Justified::Center);

        weaponTBoxTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "TurnBox'sWeapon", turnBoxWeaponTextInfo, turnBoxTitleTextRegion);

        sfml_util::gui::TextInfo turnBoxArmorTextInfo(turnBoxWeaponTextInfo);

        armorTBoxTextRegionUPtr_.reset(new sfml_util::gui::TextRegion(
            "TurnBox'sArmor", turnBoxWeaponTextInfo, turnBoxTitleTextRegion));

        sfml_util::gui::TextInfo turnButtonTextInfo(
            " ",
            sfml_util::FontManager::Instance()->Font_Default2(),
            sfml_util::FontManager::Instance()->Size_Largeish(),
            sfml_util::FontManager::Color_Orange(),
            sfml_util::Justified::Left);

        const sfml_util::gui::TextInfo TESTING_TEXT_INFO(
            " ",
            sfml_util::FontManager::Instance()->Font_Default2(),
            sfml_util::FontManager::Instance()->Size_Smallish(),
            sfml_util::FontManager::Color_GrayLight(),
            sfml_util::Justified::Left);

        sf::FloatRect testingTextRegion(turnBoxRegion_);
        testingTextRegion.left += testingTextRegion.width - 50.0f;
        testingTextRegion.top += (testingTextRegion.height - 400.0f);

        testingTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "CombatStage'sTesting", TESTING_TEXT_INFO, testingTextRegion);

        const sf::Color TURNBUTTON_DISABLED_COLOR(sfml_util::FontManager::Color_Orange() - sf::Color(0, 0, 0, 176));
        sfml_util::gui::TextInfo turnButtonTextInfoDisabled(turnButtonTextInfo);
        turnButtonTextInfoDisabled.color = TURNBUTTON_DISABLED_COLOR;

        turnButtonTextInfo.text = "(A)ttack";
        turnButtonTextInfoDisabled.text = "(A)ttack";
        const sfml_util::gui::MouseTextInfo ATTACKBUTTON_MOUSETEXTINFO(
            turnButtonTextInfo,
            sfml_util::FontManager::Color_Light(),
            sf::Color::White);
        
        attackTBoxButtonSPtr_ = std::make_shared<sfml_util::gui::FourStateButton>(
            "CombatStage'sAttack",
            0.0f,
            0.0f,
            "",
            "",
            "",
            "",
            ATTACKBUTTON_MOUSETEXTINFO, turnButtonTextInfoDisabled);

        attackTBoxButtonSPtr_->SetCallbackHandler(this);

        attackTBoxButtonSPtr_->SetMouseHoverText(
            combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_ATTACK_);
        
        attackTBoxButtonSPtr_->MoveEntityOffScreen();
        EntityAdd(attackTBoxButtonSPtr_.get());

        turnButtonTextInfo.text = "(F)ight";
        turnButtonTextInfoDisabled.text = "(F)ight";
        const sfml_util::gui::MouseTextInfo FIGHTBUTTON_MOUSETEXTINFO(
            turnButtonTextInfo,
            sfml_util::FontManager::Color_Light(),
            sf::Color::White);
        
        fightTBoxButtonSPtr_ = std::make_shared<sfml_util::gui::FourStateButton>(
            "CombatStage'sAttack",
            0.0f,
            0.0f,
            "",
            "",
            "",
            "",
            FIGHTBUTTON_MOUSETEXTINFO,
            turnButtonTextInfoDisabled);

        fightTBoxButtonSPtr_->SetCallbackHandler(this);
        
        fightTBoxButtonSPtr_->SetMouseHoverText(
            combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_FIGHT_);
        
        fightTBoxButtonSPtr_->MoveEntityOffScreen();
        EntityAdd(fightTBoxButtonSPtr_.get());

        turnButtonTextInfo.text = "(S)pell";
        turnButtonTextInfoDisabled.text = "(S)pell";
        const sfml_util::gui::MouseTextInfo CASTBUTTON_MOUSETEXTINFO(
            turnButtonTextInfo,
            sfml_util::FontManager::Color_Light(),
            sf::Color::White);
        
        castTBoxButtonSPtr_ = std::make_shared<sfml_util::gui::FourStateButton>(
            "CombatStage'sCast",
            0.0f,
            0.0f,
            "",
            "",
            "",
            "",
            CASTBUTTON_MOUSETEXTINFO,
            turnButtonTextInfoDisabled);
        
        castTBoxButtonSPtr_->SetCallbackHandler(this);
        
        castTBoxButtonSPtr_->SetMouseHoverText(
            combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_CAST_);
        
        castTBoxButtonSPtr_->MoveEntityOffScreen();
        EntityAdd(castTBoxButtonSPtr_.get());

        turnButtonTextInfo.text = "Advance";
        turnButtonTextInfoDisabled.text = "Advance";
        const sfml_util::gui::MouseTextInfo ADVANCEBUTTON_MOUSETEXTINFO(
            turnButtonTextInfo,
            sfml_util::FontManager::Color_Light(),
            sf::Color::White);
        
        advanceTBoxButtonSPtr_ = std::make_shared<sfml_util::gui::FourStateButton>(
            "CombatStage'sAdvance",
            0.0f,
            0.0f,
            "", 
            "", 
            "",
            "",
            ADVANCEBUTTON_MOUSETEXTINFO,
            turnButtonTextInfoDisabled);
        
        advanceTBoxButtonSPtr_->SetCallbackHandler(this);
        
        advanceTBoxButtonSPtr_->SetMouseHoverText(
            combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_ADVANCE_);
        
        advanceTBoxButtonSPtr_->MoveEntityOffScreen();
        EntityAdd(advanceTBoxButtonSPtr_.get());

        turnButtonTextInfo.text = "Retreat";
        turnButtonTextInfoDisabled.text = "Retreat";
        const sfml_util::gui::MouseTextInfo RETREATBUTTON_MOUSETEXTINFO(
            turnButtonTextInfo,
            sfml_util::FontManager::Color_Light(),
            sf::Color::White);
        
        retreatTBoxButtonSPtr_ = std::make_shared<sfml_util::gui::FourStateButton>(
            "CombatStage'sRetreat",
            0.0f,
            0.0f,
            "",
            "",
            "",
            "",
            RETREATBUTTON_MOUSETEXTINFO,
            turnButtonTextInfoDisabled);
        
        retreatTBoxButtonSPtr_->SetCallbackHandler(this);
        
        retreatTBoxButtonSPtr_->SetMouseHoverText(
            combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_RETREAT_);
        
        retreatTBoxButtonSPtr_->MoveEntityOffScreen();
        EntityAdd(retreatTBoxButtonSPtr_.get());

        turnButtonTextInfo.text = "(B)lock";
        turnButtonTextInfoDisabled.text = "(B)lock";
        const sfml_util::gui::MouseTextInfo BLOCKBUTTON_MOUSETEXTINFO(
            turnButtonTextInfo,
            sfml_util::FontManager::Color_Light(),
            sf::Color::White);
        
        blockTBoxButtonSPtr_ = std::make_shared<sfml_util::gui::FourStateButton>(
            "CombatStage'sBlock",
            0.0f,
            0.0f,
            "",
            "",
            "",
            "",
            BLOCKBUTTON_MOUSETEXTINFO,
            turnButtonTextInfoDisabled);
        
        blockTBoxButtonSPtr_->SetCallbackHandler(this);
        
        blockTBoxButtonSPtr_->SetMouseHoverText(
            combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_BLOCK_);
        
        blockTBoxButtonSPtr_->MoveEntityOffScreen();
        EntityAdd(blockTBoxButtonSPtr_.get());

        turnButtonTextInfo.text = "S(k)ip";
        turnButtonTextInfoDisabled.text = "S(k)ip";
        const sfml_util::gui::MouseTextInfo SKIPBUTTON_MOUSETEXTINFO(
            turnButtonTextInfo,
            sfml_util::FontManager::Color_Light(),
            sf::Color::White);
        
        skipTBoxButtonSPtr_ = std::make_unique<sfml_util::gui::FourStateButton>(
            "CombatStage'sSkip",
            0.0f,
            0.0f,
            "",
            "",
            "",
            "",
            SKIPBUTTON_MOUSETEXTINFO,
            turnButtonTextInfoDisabled);
        
        skipTBoxButtonSPtr_->SetCallbackHandler(this);
        
        skipTBoxButtonSPtr_->SetMouseHoverText(
            combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_SKIP_);
        
        skipTBoxButtonSPtr_->MoveEntityOffScreen();
        EntityAdd(skipTBoxButtonSPtr_.get());

        turnButtonTextInfo.text = "Fl(y)";
        turnButtonTextInfoDisabled.text = "Fl(y)";
        const sfml_util::gui::MouseTextInfo FLYBUTTON_MOUSETEXTINFO(
            turnButtonTextInfo,
            sfml_util::FontManager::Color_Light(),
            sf::Color::White);
        
        flyTBoxButtonSPtr_ = std::make_shared<sfml_util::gui::FourStateButton>(
            "CombatStage'sFly",
            0.0f,
            0.0f,
            "",
            "",
            "",
            "",
            FLYBUTTON_MOUSETEXTINFO,
            turnButtonTextInfoDisabled);
        
        flyTBoxButtonSPtr_->SetCallbackHandler(this);

        flyTBoxButtonSPtr_->SetMouseHoverText(
            combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_FLY_);
        
        flyTBoxButtonSPtr_->MoveEntityOffScreen();
        EntityAdd(flyTBoxButtonSPtr_.get());

        turnButtonTextInfo.text = "(L)and";
        turnButtonTextInfoDisabled.text = "(L)and";
        const sfml_util::gui::MouseTextInfo LANDBUTTON_MOUSETEXTINFO(
            turnButtonTextInfo,
            sfml_util::FontManager::Color_Light(),
            sf::Color::White);
        
        landTBoxButtonSPtr_ = std::make_shared<sfml_util::gui::FourStateButton>(
            "CombatStage'sLand",
            0.0f,
            0.0f,
            "",
            "",
            "",
            "",
            LANDBUTTON_MOUSETEXTINFO,
            turnButtonTextInfoDisabled);
        
        landTBoxButtonSPtr_->SetCallbackHandler(this);
        landTBoxButtonSPtr_->SetMouseHoverText(combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_LAND_);
        landTBoxButtonSPtr_->MoveEntityOffScreen();
        EntityAdd(landTBoxButtonSPtr_.get());

        turnButtonTextInfo.text = "(R)oar";
        turnButtonTextInfoDisabled.text = "(R)oar";
        const sfml_util::gui::MouseTextInfo ROARBUTTON_MOUSETEXTINFO(
            turnButtonTextInfo,
            sfml_util::FontManager::Color_Light(),
            sf::Color::White);
        
        roarTBoxButtonSPtr_ = std::make_shared<sfml_util::gui::FourStateButton>(
            "CombatStage'sRoar",
            0.0f,
            0.0f,
            "",
            "",
            "",
            "",
            ROARBUTTON_MOUSETEXTINFO,
            turnButtonTextInfoDisabled);
        
        roarTBoxButtonSPtr_->SetCallbackHandler(this);
        
        roarTBoxButtonSPtr_->SetMouseHoverText(
            combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_ROAR_);
        
        roarTBoxButtonSPtr_->MoveEntityOffScreen();
        EntityAdd(roarTBoxButtonSPtr_.get());

        turnButtonTextInfo.text = "(P)ounce";
        turnButtonTextInfoDisabled.text = "(P)ounce";
        const sfml_util::gui::MouseTextInfo POUNCEBUTTON_MOUSETEXTINFO(
            turnButtonTextInfo,
            sfml_util::FontManager::Color_Light(),
            sf::Color::White);
        
        pounceTBoxButtonSPtr_ = std::make_shared<sfml_util::gui::FourStateButton>(
            "CombatStage'sPounce",
            0.0f,
            0.0f,
            "",
            "",
            "",
            "",
            POUNCEBUTTON_MOUSETEXTINFO,
            turnButtonTextInfoDisabled);
        
        pounceTBoxButtonSPtr_->SetCallbackHandler(this);
        
        pounceTBoxButtonSPtr_->SetMouseHoverText(
            combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_POUNCE_);
        
        pounceTBoxButtonSPtr_->MoveEntityOffScreen();
        EntityAdd(pounceTBoxButtonSPtr_.get());

        //settings button (gears symbol)
        const float COMMAND_REGION_PAD(10.0f);
        settingsButtonSPtr_ = std::make_shared<sfml_util::gui::FourStateButton>(
            "CombatStage'sSettingsGears",
            0.0f,
            0.0f,
            std::string(GameDataFile::Instance()->GetMediaPath(
                "media-images-buttons-gui")).append("gears1_normal.png"),
            "",
            std::string(GameDataFile::Instance()->GetMediaPath(
                "media-images-buttons-gui")).append("gears1_lit.png"),
            "",
            sfml_util::gui::MouseTextInfo(),
            sfml_util::gui::TextInfo(),
            false,
            sfml_util::MapByRes(0.6f, 2.0f),
            false);

        settingsButtonSPtr_->SetEntityPos(((COMMAND_REGION_LEFT + COMMAND_REGION_WIDTH) -
            settingsButtonSPtr_->GetEntityRegion().width) - COMMAND_REGION_PAD,
            COMMAND_REGION_TOP + COMMAND_REGION_PAD);

        settingsButtonSPtr_->SetCallbackHandler(this);
        EntityAdd(settingsButtonSPtr_.get());

        //TODO TEMP REMOVE
        //fake player characters until loading games starts working
        std::string errMsgIgnored{ "" };
        player::PartyPtr_t partyPtr{ new player::Party() };

        {
            const stats::StatSet KNIGHT_STATS(15 + misc::random::Int(10),
                                              15 + misc::random::Int(6),
                                              0  + misc::random::Int(6),
                                              5  + misc::random::Int(10),
                                              15 + misc::random::Int(10),
                                              0  + misc::random::Int(8));

            const std::string KNIGHT_NAME( boost::algorithm::replace_last_copy(
                creature::NameInfo::Instance()->LargestName(),
                creature::NameInfo::Instance()->LargestLetterString(),
                "K") );

            auto knightPtr{ new player::Character(KNIGHT_NAME,
                                                  creature::sex::Male,
                                                  creature::BodyType::Make_Humanoid(),
                                                  creature::Race(creature::race::Human),
                                                  creature::Role(creature::role::Knight),
                                                  KNIGHT_STATS) };
            
            player::Initial::Setup(knightPtr);
            partyPtr->Add(knightPtr, errMsgIgnored);
        }
        /*
        {
            const stats::StatSet FIREBRAND_STATS(20 + misc::random::Int(10),
                                                 15 + misc::random::Int(10),
                                                 0 + misc::random::Int(6),
                                                 5 + misc::random::Int(10),
                                                 0 + misc::random::Int(10),
                                                 10 + misc::random::Int(8));

            const std::string FIREBRAND_NAME(boost::algorithm::replace_last_copy(creature::NameInfo::Instance()->LargestName(), creature::NameInfo::Instance()->LargestLetterString(), "F"));

            auto firebrandSPtr = std::make_shared<player::Character>(FIREBRAND_NAME,
                                                                     creature::sex::Male,
                                                                     creature::BodyType::Make_Dragon(),
                                                                     creature::Race(creature::race::Dragon),
                                                                     creature::Role(creature::role::Firebrand),
                                                                     FIREBRAND_STATS);

            const stats::StatSet STATS_MOD(misc::random::Int(2) * ((misc::random::Bool()) ? -1 : 1),
                                           misc::random::Int(3) * ((misc::random::Bool()) ? -1 : 1),
                                           misc::random::Int(4) * ((misc::random::Bool()) ? -1 : 1),
                                           misc::random::Int(5) * ((misc::random::Bool()) ? -1 : 1),
                                           misc::random::Int(6) * ((misc::random::Bool()) ? -1 : 1),
                                           misc::random::Int(7) * ((misc::random::Bool()) ? -1 : 1));

            firebrandSPtr->Stats().ModifyCurrent(STATS_MOD);

            player::Initial::Setup(firebrandSPtr.get());
            partyPtr->Add(firebrandSPtr, errMsgIgnored);
        }
        */
        {
            const stats::StatSet ARCHER_STATS(15 + misc::random::Int(6),
                                              15 + misc::random::Int(10),
                                              5  + misc::random::Int(6),
                                              10 + misc::random::Int(6),
                                              10 + misc::random::Int(8),
                                              5  + misc::random::Int(6));

            const std::string ARCHER_NAME(boost::algorithm::replace_last_copy(
                creature::NameInfo::Instance()->LargestName(),
                creature::NameInfo::Instance()->LargestLetterString(),
                "A"));

            auto archerPtr{ new player::Character(ARCHER_NAME,
                                                  creature::sex::Female,
                                                  creature::BodyType::Make_Humanoid(),
                                                  creature::Race(creature::race::Human),
                                                  creature::Role(creature::role::Archer),
                                                  ARCHER_STATS) };

            player::Initial::Setup(archerPtr);
            partyPtr->Add(archerPtr, errMsgIgnored);
        }
        /*
        {
            const stats::StatSet WOLFEN_STATS(20 + misc::random::Int(10),
                                              20 + misc::random::Int(10),
                                              5  + misc::random::Int(6),
                                              5  + misc::random::Int(10),
                                              5  + misc::random::Int(8),
                                              5  + misc::random::Int(6));

            const std::string WOLFEN_NAME(boost::algorithm::replace_last_copy(
                creature::NameInfo::Instance()->LargestName(),
                creature::NameInfo::Instance()->LargestLetterString(),
                "W"));

            auto wolfenPtr{ new player::Character(WOLFEN_NAME,
                                                  creature::sex::Female,
                                                  creature::BodyType::Make_Wolfen(),
                                                  creature::Race(creature::race::Wolfen),
                                                  creature::Role(creature::role::Wolfen),
                                                  WOLFEN_STATS) };

            player::Initial::Setup(wolfenPtr);
            partyPtr->Add(wolfenPtr, errMsgIgnored);
        }*/
        
        {
            const stats::StatSet BARD_STATS(10 + misc::random::Int(6),
                                            10 + misc::random::Int(6),
                                            10 + misc::random::Int(6),
                                            5  + misc::random::Int(6),
                                            10 + misc::random::Int(6),
                                            10 + misc::random::Int(6));

            const std::string BARD_NAME(boost::algorithm::replace_last_copy(
                creature::NameInfo::Instance()->LargestName(),
                creature::NameInfo::Instance()->LargestLetterString(),
                "B"));

            auto bardPtr{ new player::Character(BARD_NAME,
                                                creature::sex::Male,
                                                creature::BodyType::Make_Humanoid(),
                                                creature::Race(creature::race::Human),
                                                creature::Role(creature::role::Bard),
                                                BARD_STATS) };

            player::Initial::Setup(bardPtr);
            partyPtr->Add(bardPtr, errMsgIgnored);
        }
        /*
        {
            const stats::StatSet BEASTMASTER_STATS(10 + misc::random::Int(6),
                                                   10 + misc::random::Int(6),
                                                   10 + misc::random::Int(6),
                                                   10 + misc::random::Int(6),
                                                   10 + misc::random::Int(6),
                                                   10 + misc::random::Int(6));

            const std::string BEASTMASTER_NAME(boost::algorithm::replace_last_copy(
                creature::NameInfo::Instance()->LargestName(),
                creature::NameInfo::Instance()->LargestLetterString(),
                "G"));

            auto bmPtr{ new player::Character(BEASTMASTER_NAME,
                                              creature::sex::Male,
                                              creature::BodyType::Make_Humanoid(),
                                              creature::Race(creature::race::Human),
                                              creature::Role(creature::role::Beastmaster),
                                              BEASTMASTER_STATS) };

            player::Initial::Setup(bmPtr);
            partyPtr->Add(bmPtr, errMsgIgnored);
        }
        */
        {
            const stats::StatSet THEIF_STATS(5  + misc::random::Int(10),
                                             5  + misc::random::Int(10),
                                             5  + misc::random::Int(10),
                                             15 + misc::random::Int(15),
                                             15 + misc::random::Int(10),
                                             5  + misc::random::Int(8));

            const std::string THEIF_NAME(boost::algorithm::replace_last_copy(
                creature::NameInfo::Instance()->LargestName(),
                creature::NameInfo::Instance()->LargestLetterString(),
                "T"));

            auto thiefPtr{ new player::Character(THEIF_NAME,
                                                 creature::sex::Male,
                                                 creature::BodyType::Make_Humanoid(),
                                                 creature::Race(creature::race::Gnome),
                                                 creature::Role(creature::role::Thief),
                                                 THEIF_STATS) };

            player::Initial::Setup(thiefPtr);
            partyPtr->Add(thiefPtr, errMsgIgnored);
        }
        {
            const stats::StatSet CLERIC_STATS(5  + misc::random::Int(8),
                                              5  + misc::random::Int(8),
                                              15 + misc::random::Int(10),
                                              10 + misc::random::Int(8),
                                              30 + misc::random::Int(8),
                                              10 + misc::random::Int(15));

            const std::string CLERIC_NAME(boost::algorithm::replace_last_copy(
                creature::NameInfo::Instance()->LargestName(),
                creature::NameInfo::Instance()->LargestLetterString(),
                "C"));

            auto clericPtr{ new player::Character(CLERIC_NAME,
                                                  creature::sex::Female,
                                                  creature::BodyType::Make_Pixie(),
                                                  creature::Race(creature::race::Pixie),
                                                  creature::Role(creature::role::Cleric),
                                                  CLERIC_STATS) };

            player::Initial::Setup(clericPtr);
            partyPtr->Add(clericPtr, errMsgIgnored);
        }

        {
            const stats::StatSet SORCERER_STATS(1  + misc::random::Int(8),
                                                1  + misc::random::Int(8),
                                                5  + misc::random::Int(8),
                                                10 + misc::random::Int(6),
                                                30 + misc::random::Int(6),
                                                20 + misc::random::Int(10));

            const std::string SORCERER_NAME(boost::algorithm::replace_last_copy(
                creature::NameInfo::Instance()->LargestName(),
                creature::NameInfo::Instance()->LargestLetterString(),
                "S"));

            auto sorcererPtr{ new player::Character(SORCERER_NAME,
                                                    creature::sex::Male,
                                                    creature::BodyType::Make_Pixie(),
                                                    creature::Race(creature::race::Pixie),
                                                    creature::Role(creature::role::Sorcerer),
                                                    SORCERER_STATS) };

            player::Initial::Setup(sorcererPtr);
            partyPtr->Add(sorcererPtr, errMsgIgnored);
        }
        /*
        {
            const stats::StatSet SYLAVIN_STATS(20 + misc::random::Int(8),
                                               20 + misc::random::Int(8),
                                               5  + misc::random::Int(8),
                                               5  + misc::random::Int(6),
                                               50 + misc::random::Int(6),
                                               10 + misc::random::Int(10));

            const std::string SYLAVIN_NAME(boost::algorithm::replace_last_copy(creature::NameInfo::Instance()->LargestName(), creature::NameInfo::Instance()->LargestLetterString(), "S"));
            player::CharacterSPtr_t sylavinSPtr(new player::Character(SYLAVIN_NAME,
                                                                      creature::sex::Male,
                                                                      creature::BodyType::Make_Dragon(),
                                                                      creature::Race(creature::race::Dragon),
                                                                      creature::Role(creature::role::Sylavin),
                                                                      SYLAVIN_STATS));

            player::Initial::Setup(sylavinSPtr.get());
            partyPtr->Add(sylavinSPtr, errMsgIgnored);
        }
        */
        if (restoreInfo_.HasRestored())
        {
            preTurnPhase_ = PreTurnPhase::End;
        }
        else
        {
            //TODO TEMP -change back to ::Start when inventory testing is over
            preTurnPhase_ = PreTurnPhase::End;

            //TEMP TODO REMOVE create new game and player party object
            state::GameStateFactory::Instance()->NewGame(partyPtr);

            combat::Encounter::Instance()->StartTasks();

            //TODO TEMP REMOVE -test create that can't take action
            combat::Encounter::Instance()->NonPlayerParty().Characters()[0]->
                ConditionAdd(creature::Conditions::Stone);
        }

        //combat display
        combatDisplayStagePtr_->StageRegionSet(COMBAT_REGION);
        combatDisplayStagePtr_->Setup();

        //give control of the CombatDisplay object lifetime to the Loop class
        LoopManager::Instance()->AddStage(combatDisplayStagePtr_);

        if (restoreInfo_.HasRestored())
        {
            conditionEffectsWillSkip_ = true;
        }
        else
        {
            //set Pixie creatures to initially flying
            //while this doesn't technically make them fly, the call to restoreInfo_.Restore()
            //will actually set them flying
            {
                combat::CombatNodePVec_t combatNodesPVec;
                combatDisplayStagePtr_->GetCombatNodes(combatNodesPVec);

                for (auto const nextComabtNodeCPtr : combatNodesPVec)
                {
                    if ((creature::race::WillInitiallyFly(
                            nextComabtNodeCPtr->Creature()->Race().Which())) ||
                        (creature::role::WillInitiallyFly(
                            nextComabtNodeCPtr->Creature()->Role().Which())))
                    {
                        nextComabtNodeCPtr->IsFlying(true);
                    }
                }
                restoreInfo_.Save(combatDisplayStagePtr_);
            }
        }

        //CombatDisplay Zoom Sliderbar
        const sfml_util::gui::TextInfo ZOOMSLIDER_LABEL_TEXT_INFO(
            "Zoom",
            sfml_util::FontManager::Instance()->Font_Default1(),
            sfml_util::FontManager::Instance()->Size_Smallish(),
            sfml_util::FontManager::Color_Light(),
            sfml_util::Justified::Left);
        //
        const sf::FloatRect ZOOMSLIDER_LABEL_RECT(0.0f,
                                                  COMMAND_REGION_TOP + COMMAND_REGION_PAD,
                                                  0.0f,
                                                  0.0f);
        //
        zoomLabelTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "ZoomSlider's",
            ZOOMSLIDER_LABEL_TEXT_INFO,
            ZOOMSLIDER_LABEL_RECT);
        //
        zoomSliderBarUPtr_ = std::make_unique<sfml_util::gui::SliderBar>(
            "CombatStageZoom",
            COMMAND_REGION_LEFT + COMMAND_REGION_PAD,
            zoomLabelTextRegionUPtr_->GetEntityRegion().top +
            zoomLabelTextRegionUPtr_->GetEntityRegion().height,
            (settingsButtonSPtr_->GetEntityPos().x - COMMAND_REGION_LEFT) -
                (COMMAND_REGION_PAD * 2.0f),
            sfml_util::gui::SliderStyle(sfml_util::Orientation::Horiz), this);

        zoomSliderBarUPtr_->SetCurrentValue(1.0f);
        zoomLabelTextRegionUPtr_->SetEntityPos(
            (zoomSliderBarUPtr_->GetEntityPos().x + 
                (zoomSliderBarUPtr_->GetEntityRegion().width * 0.5f)) -
            (zoomLabelTextRegionUPtr_->GetEntityRegion().width * 0.5f),
            zoomLabelTextRegionUPtr_->GetEntityPos().y);
        //
        EntityAdd(zoomLabelTextRegionUPtr_.get());
        EntityAdd(zoomSliderBarUPtr_.get());

        MoveTurnBoxObjectsOffScreen(true);
        restoreInfo_.Restore(combatDisplayStagePtr_);
        SetUserActionAllowed(false);

        if (WILL_ADVANCE_TURN_)
        {
            combat::Encounter::Instance()->IncrementTurn();
        }
    }


    void CombatStage::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        target.draw( * commandBoxUPtr_, STATES);
        Stage::Draw(target, STATES);
        statusBoxSPtr_->draw(target, STATES);

        if (((turnPhase_ >= TurnPhase::Determine) &&
                (turnPhase_ <= TurnPhase::PostPerformPause)) ||
            (IsNonPlayerCharacterTurnValid() &&
                (TurnPhase::PostCenterAndZoomInPause == turnPhase_)))
        {
            turnBoxUPtr_->draw(target, STATES);

            titleTBoxTextRegionUPtr_->draw(target, STATES);
            weaponTBoxTextRegionUPtr_->draw(target, STATES);
            armorTBoxTextRegionUPtr_->draw(target, STATES);
            infoTBoxTextRegionUPtr_->draw(target, STATES);
            enemyActionTBoxRegionUPtr_->draw(target, STATES);
            enemyCondsTBoxRegionUPtr_->draw(target, STATES);

            attackTBoxButtonSPtr_->draw(target, STATES);
            fightTBoxButtonSPtr_->draw(target, STATES);
            castTBoxButtonSPtr_->draw(target, STATES);
            advanceTBoxButtonSPtr_->draw(target, STATES);
            retreatTBoxButtonSPtr_->draw(target, STATES);
            blockTBoxButtonSPtr_->draw(target, STATES);
            skipTBoxButtonSPtr_->draw(target, STATES);
            flyTBoxButtonSPtr_->draw(target, STATES);
            landTBoxButtonSPtr_->draw(target, STATES);
            roarTBoxButtonSPtr_->draw(target, STATES);
            pounceTBoxButtonSPtr_->draw(target, STATES);
        }

        if (false == hasCombatEnded_)
        {
            combatAnimationUPtr_->Draw(target, STATES);
        }

        DrawHoverText(target, STATES);
        testingTextRegionUPtr_->draw(target, STATES);
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
            if (enemyActionTBoxRegionUPtr_.get() != nullptr)
            {
                enemyActionTBoxRegionUPtr_->SetEntityColorFgBoth(
                    goldTextColorShaker_.Update(ELAPSED_TIME_SEC) );
            }
        }

        if (willRedColorShakeWeaponText_)
        {
            weaponTBoxTextRegionUPtr_->SetEntityColorFgBoth(
                redTextColorShaker_.Update(ELAPSED_TIME_SEC));
        }

        if (IsNonPlayerCharacterTurnValid() &&
            (TurnPhase::PostCenterAndZoomInPause == turnPhase_))
        {
            titleTBoxTextRegionUPtr_->SetEntityColorFgBoth(
                goldTextColorShaker_.Update(ELAPSED_TIME_SEC));
        }

        if (IsNonPlayerCharacterTurnValid() &&
            ((TurnPhase::CenterAndZoomOut == turnPhase_) ||
             (TurnPhase::PostCenterAndZoomOutPause == turnPhase_)))
        {
            enemyActionTBoxRegionUPtr_->SetEntityColorFgBoth(
                goldTextColorShaker_.Update(ELAPSED_TIME_SEC));
        }

        //single-click triggers summary view;
        if (clickTimerSec_ > 0.0f)
        {
            clickTimerSec_ -= ELAPSED_TIME_SEC;

            if (clickTimerSec_ < 0.0f)
            {
                combatDisplayStagePtr_->StartSummaryView(clickPosV_);
            }
        }

        //allow progress of the pause timer even if IsStatusMessageAnimating(),
        //because they work together
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
            if (slider_.GetIsDone())
            {
                SetTurnPhase(TurnPhase::PostDeathAnimSlide);
                combatAnimationUPtr_->DeathAnimStop();
                PositionSlideStartTasks();
            }
            return;
        }

        if ((TurnPhase::PerformAnim == turnPhase_) &&
            (AnimPhase::ProjectileShoot == animPhase_))
        {
            combatAnimationUPtr_->ProjectileShootAnimUpdate( slider_.Update(ELAPSED_TIME_SEC) );
            if (slider_.GetIsDone())
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

        if ((TurnPhase::PerformAnim == turnPhase_) &&
            (AnimPhase::Impact == animPhase_))
        {
            combatAnimationUPtr_->ImpactAnimUpdate(slider_.Update(ELAPSED_TIME_SEC));
            if (slider_.GetIsDone())
            {
                combatAnimationUPtr_->ImpactAnimStop();
                HandleApplyDamageTasks();
                SetAnimPhase(AnimPhase::PostImpactPause);
                StartPause(POST_IMPACT_ANIM_PAUSE_SEC_, "PostImpact");
                SetupTurnBox();
            }
            return;
        }

        if ((TurnPhase::PerformAnim == turnPhase_) &&
            (AnimPhase::Spell == animPhase_))
        {
            if (combatAnimationUPtr_->SpellAnimUpdate(spellBeingCastPtr_, ELAPSED_TIME_SEC))
            {
                combatAnimationUPtr_->SpellAnimStop(spellBeingCastPtr_);
                HandleApplyDamageTasks();
                SetAnimPhase(AnimPhase::PostSpellPause);
                StartPause(POST_SPELL_ANIM_PAUSE_SEC_, "PostSpell");
            }
        }

        if ((TurnPhase::PerformAnim == turnPhase_) &&
            (AnimPhase::MoveToward == animPhase_))
        {
            combatAnimationUPtr_->MeleeMoveTowardAnimUpdate(slider_.Update(ELAPSED_TIME_SEC));
            if (slider_.GetIsDone())
            {
                combatAnimationUPtr_->MeleeMoveTowardAnimStop();
                SetAnimPhase(AnimPhase::PostMoveTowardPause);
                StartPause(POST_MELEEMOVE_ANIM_PAUSE_SEC_, "PostMoveToward");
                SetupTurnBox();
            }
            return;
        }

        if ((TurnPhase::PerformAnim == turnPhase_) &&
            (AnimPhase::MoveBack == animPhase_))
        {
            combatAnimationUPtr_->MeleeMoveBackAnimUpdate(slider_.Update(ELAPSED_TIME_SEC));
            if (slider_.GetIsDone())
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
            auto const SLIDER_POS{ slider_.Update(ELAPSED_TIME_SEC) };

            zoomSliderBarUPtr_->SetCurrentValue(zoomSliderOrigPos_ +
                (SLIDER_POS * (1.0f - zoomSliderOrigPos_)));
            
            combatAnimationUPtr_->CenteringUpdate(SLIDER_POS);
            if (slider_.GetIsDone())
            {
                StartTurn_Step2();
            }
            return;
        }

        if ( (TurnPhase::PostDeathAnimSlide == turnPhase_) ||
             ((TurnPhase::PerformAnim == turnPhase_) &&
              (AnimPhase::AdvanceOrRetreat == animPhase_) &&
              ((TurnActionPhase::Advance == turnActionPhase_) || (TurnActionPhase::Retreat == turnActionPhase_))) )
        {
            combatAnimationUPtr_->RepositionAnimUpdate(slider_.Update(ELAPSED_TIME_SEC));
            if (slider_.GetIsDone())
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
            statusBoxSPtr_->SetHighlightColor( statusMsgAnimColorShaker_.Update(ELAPSED_TIME_SEC) );

            if (statusMsgAnimTimerSec_ > STATUSMSG_ANIM_PAUSE_SEC_)
            {
                statusBoxSPtr_->SetHighlightColor(LISTBOX_HIGHLIGHT_COLOR_);
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
            auto const SLIDER_POS{ slider_.Update(ELAPSED_TIME_SEC) };

            if (combatAnimationUPtr_->CenteringUpdate(SLIDER_POS))
            {
                auto const ZOOM_CURR_VAL(1.0f - SLIDER_POS);
                zoomSliderBarUPtr_->SetCurrentValue(ZOOM_CURR_VAL);
            }

            if (slider_.GetIsDone())
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
            //the initial pan seems to take extra time getting started so speed it up here
            auto sliderPosAdj{ 0.25f + slider_.Update(ELAPSED_TIME_SEC) };
            if (sliderPosAdj > 1.0f)
            {
                sliderPosAdj = 1.0f;
            }

            combatAnimationUPtr_->CenteringUpdate(sliderPosAdj);
            if (slider_.GetIsDone())
            {
                combatAnimationUPtr_->CenteringStop();
                SetPreTurnPhase(PreTurnPhase::PostPanPause);
                StartPause(POST_PAN_PAUSE_SEC_, "PostPan");
            }
            return;
        }

        if (PreTurnPhase::ZoomOut == preTurnPhase_)
        {
            auto const SLIDER_POS{ slider_.Update(ELAPSED_TIME_SEC) };
            auto const ZOOM_CURR_VAL(1.0f - SLIDER_POS);
            zoomSliderBarUPtr_->SetCurrentValue(ZOOM_CURR_VAL);

            if (slider_.GetIsDone())
            {
                SetPreTurnPhase(PreTurnPhase::PostZoomOutPause);
                StartPause(POST_ZOOMOUT_PAUSE_SEC_, "PostZOut");
                AppendInitialStatus();
            }
            return;
        }

        //handle creature turn start hook, catches the start of a new turn
        if ((combat::Encounter::Instance()->HasStarted()) &&
            (IsPaused() == false) &&
            (turnCreaturePtr_ == nullptr) &&
            (TurnPhase::NotATurn == turnPhase_) &&
            (PreTurnPhase::End == preTurnPhase_) &&
            (game::LoopManager::Instance()->IsFading() == false))
        {
            if ((HandleWin() == false) && (HandleLose() == false))
            {
                StartTurn_Step1();
            }
            return;
        }

        //initial hook for taking action before the first turn (pre-turn logic)
        if ((zoomSliderBarUPtr_.get() != nullptr) &&
            (combatDisplayStagePtr_ != nullptr) &&
            (game::LoopManager::Instance()->IsFading() == false) &&
            (TurnPhase::NotATurn == turnPhase_) &&
            (PreTurnPhase::Start == preTurnPhase_))
        {
            SetPreTurnPhase(PreTurnPhase::PanToCenter);
            slider_.Reset(ANIM_INITIAL_CENTERING_SLIDER_SPEED_);
            combatAnimationUPtr_->CenteringStartTargetCenterOfBatllefield();
            return;
        }
    }


    void CombatStage::UpdateMouseDown(const sf::Vector2f & MOUSE_POS_V)
    {
        Stage::UpdateMouseDown(MOUSE_POS_V);
        
        //cancel summary view if visible or even just starting
        if (combatDisplayStagePtr_->GetIsSummaryViewInProgress())
        {
            combatDisplayStagePtr_->CancelSummaryViewAndStartTransitionBack();
            isMouseHeldDown_ = false;
        }
        else if ((TurnPhase::Determine != turnPhase_) &&
                 (TurnPhase::TargetSelect != turnPhase_))
        {
            isMouseHeldDown_ = false;
        }
    }


    sfml_util::gui::IGuiEntityPtr_t CombatStage::UpdateMouseUp(const sf::Vector2f & MOUSE_POS_V)
    {
        auto const WAS_MOUSE_HELD_DOWN_AND_MOVING{ isMouseHeldDownAndMoving_ };
        
        auto const GUI_ENTITY_WITH_FOCUS{ Stage::UpdateMouseUp(MOUSE_POS_V) };

        if (WAS_MOUSE_HELD_DOWN_AND_MOVING)
        {
            return GUI_ENTITY_WITH_FOCUS;
        }

        creature::CreaturePtr_t creatureAtPosPtr(
            combatDisplayStagePtr_->GetCreatureAtPos(MOUSE_POS_V));

        if (creatureAtPosPtr == nullptr)
        {
            HandleMiscCancelTasks();
            return GUI_ENTITY_WITH_FOCUS;
        }

        if ((TurnPhase::Determine == turnPhase_) &&
            (combatDisplayStagePtr_->GetIsSummaryViewInProgress() == false))
        {
            if (clickTimerSec_ < 0.0f)
            {
                clickPosV_ = MOUSE_POS_V;
                clickTimerSec_ = DOUBLE_CLICK_WINDOW_SEC_;
            }
            else if (creatureAtPosPtr->IsPlayerCharacter())
            {
                clickTimerSec_ = -1.0f;//any negative value will work here

                restoreInfo_.PrepareForStageChange(combatDisplayStagePtr_);
                game::LoopManager::Instance()->Goto_Inventory(turnCreaturePtr_, 
                                                              creatureAtPosPtr,
                                                              Phase::Combat);
                return GUI_ENTITY_WITH_FOCUS;
            }
        }

        if (TurnPhase::TargetSelect != turnPhase_)
        {
            return GUI_ENTITY_WITH_FOCUS;
        }

        if (nullptr == spellBeingCastPtr_)
        {
            if (creatureAtPosPtr->IsPlayerCharacter())
            {
                QuickSmallPopup(std::string("That is one of your player characters,").append(
                    "who cannot be attacked.  Click on an enemy creature instead."), false, true);
            }
            else
            {
                if (combatDisplayStagePtr_->IsCreatureAPossibleFightTarget(turnCreaturePtr_,
                                                                           creatureAtPosPtr))
                {
                    combatAnimationUPtr_->SelectAnimStart(creatureAtPosPtr);
                    HandleAttackTasks(creatureAtPosPtr);
                }
                else
                {
                    QuickSmallPopup(std::string("That creature is not close enough to fight.").
                        append("  Try clicking on another creature."), false, true);
                }
            }
        }
        else if (spellBeingCastPtr_->Target() == TargetType::SingleOpponent)
        {
            if (creatureAtPosPtr->IsPlayerCharacter())
            {
                QuickSmallPopup(std::string("That is one of your player characters,").
                    append(" not an enemy.  Click on an enemy creature instead."), false, true);
            }
            else
            {
                combatAnimationUPtr_->SelectAnimStart(creatureAtPosPtr);
                HandleCast_Step3_PerformOnTargets(creature::CreaturePVec_t{ creatureAtPosPtr });
            }
        }
        else if ((nullptr != spellBeingCastPtr_) &&
                 (spellBeingCastPtr_->Target() == TargetType::SingleCompanion))
        {
            if (creatureAtPosPtr->IsPlayerCharacter() == false)
            {
                QuickSmallPopup(std::string("That is and enemy, not one of your character.").
                    append("  Click on one of your characters instead."), false, true);
            }
            else
            {
                combatAnimationUPtr_->SelectAnimStart(creatureAtPosPtr);
                HandleCast_Step3_PerformOnTargets(creature::CreaturePVec_t{ creatureAtPosPtr });
            }
        }
        
        return GUI_ENTITY_WITH_FOCUS;
    }


    bool CombatStage::KeyRelease(const sf::Event::KeyEvent & KE)
    {
        if (HandleMiscCancelTasks())
        {
            return true;
        }

        //esc can cancel fight target selection phase
        if ((TurnPhase::TargetSelect == turnPhase_) &&
            ((KE.code == sf::Keyboard::Escape) || (KE.code == sf::Keyboard::X)))
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
                auto const MOUSEOVER_ATTACK_STR(combat::Text::MouseOverTextAttackStr(
                    turnCreaturePtr_, combatDisplayStagePtr_));

                if (MOUSEOVER_ATTACK_STR == combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_ATTACK_)
                {
                    return HandleAttack();
                }
                else
                {
                    auto const MOUSEOVER_STR(combat::Text::MouseOverTextBlockStr(
                        turnCreaturePtr_, combatDisplayStagePtr_));

                    if (MOUSEOVER_STR == combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_BLOCK_)
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

            if (KE.code == sf::Keyboard::S)
            {
                if ((turnCreaturePtr_ != nullptr) &&
                    (turnCreaturePtr_->Role().Which() == creature::role::Bard))
                {
                    return HandleSong_Step1_ValidatePlayAndSelectSong();
                }
                else
                {
                    return HandleCast_Step1_ValidateCastAndSelectSpell();
                }
            }

            if (KE.code == sf::Keyboard::Right)
            {
                return HandleAdvance();
            }

            if (KE.code == sf::Keyboard::Left)
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

            if (KE.code == sf::Keyboard::X)
            {
                return HandleWeaponChange();
            }

            if (nullptr != turnCreaturePtr_)
            {
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
                    if (turnCreaturePtr_->IsPlayerCharacter())
                    {
                        restoreInfo_.PrepareForStageChange(combatDisplayStagePtr_);
                        game::LoopManager::Instance()->Goto_Inventory(turnCreaturePtr_,
                                                                      turnCreaturePtr_,
                                                                      Phase::Combat);
                    }
                }

                if (KE.code == sf::Keyboard::P)
                {
                    return HandlePounce(combat::Encounter::Instance()->GetTurnInfoCopy(
                        turnCreaturePtr_).GetIsFlying());
                }
            }
        }

        return Stage::KeyRelease(KE);
    }


    bool CombatStage::IsPlayerCharacterTurnValid() const
    {
        return ((TurnPhase::NotATurn != turnPhase_) &&
                (turnCreaturePtr_ != nullptr) &&
                turnCreaturePtr_->IsPlayerCharacter());
    }


    bool CombatStage::IsNonPlayerCharacterTurnValid() const
    {
        return ((TurnPhase::NotATurn != turnPhase_) &&
                (turnCreaturePtr_ != nullptr) &&
                (turnCreaturePtr_->IsPlayerCharacter() == false));
    }


    void CombatStage::AppendInitialStatus()
    {
        std::ostringstream ss;
        ss << combat::Text::InitialCombatStatusMessagePrefix() << " "
            << combat::Encounter::Instance()->NonPlayerParty().Summary() << "!";

        statusBoxTextInfo_.text = ss.str();
        statusBoxSPtr_->Add(std::make_shared<sfml_util::gui::ListBoxItem>(
            "CombatStageStatusMsg", statusBoxTextInfo_), true);

        MoveTurnBoxObjectsOffScreen(true);
        statusMsgAnimColorShaker_.Reset();
        statusMsgAnimColorShaker_.Start();
        statusMsgAnimTimerSec_ = 0.0f;
        combatDisplayStagePtr_->SetIsStatusMessageAnimating(true);

        willClrShkInitStatusMsg_ = true;
    }


    void CombatStage::AppendStatusMessage(const std::string & MSG_STR, const bool WILL_ANIM)
    {
        statusBoxTextInfo_.text = MSG_STR;
        statusBoxSPtr_->Add(std::make_shared<sfml_util::gui::ListBoxItem>(
            "CombatStageStatusMsg", statusBoxTextInfo_), true);

        if (WILL_ANIM)
        {
            SetTurnPhase(TurnPhase::StatusAnim);
            MoveTurnBoxObjectsOffScreen(true);
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

        //anything greater than pauseDurationSec_ will work here
        pauseElapsedSec_ = pauseDurationSec_ + 1.0f;

        if ((TurnPhase::PerformAnim == turnPhase_) &&
            (AnimPhase::FinalPause == animPhase_))
        {
            SetAnimPhase(AnimPhase::NotAnimating);
            SetTurnPhase(TurnPhase::PerformReport);
            StartPause(PERFORM_PRE_REPORT_PAUSE_SEC_, "PerformPreReport (PostPerformAnim)");
            SetupTurnBox();
            return;
        }

        if ((TurnPhase::PerformAnim == turnPhase_) &&
            (AnimPhase::PostImpactPause == animPhase_))
        {
            SetAnimPhase(AnimPhase::MoveBack);
            SetupTurnBox();
            combatAnimationUPtr_->MeleeMoveBackAnimStart();
            slider_.Reset(ANIM_MELEE_MOVE_SLIDER_SPEED_);
            return;
        }

        if ((TurnPhase::PerformAnim == turnPhase_) &&
            (AnimPhase::PostSpellPause == animPhase_))
        {
            SetAnimPhase(AnimPhase::NotAnimating);
            SetTurnPhase(TurnPhase::PerformReport);
            StartPause(PERFORM_PRE_REPORT_PAUSE_SEC_, "PerformPreReport (PostPerformAnim)");
            SetupTurnBox();
            return;
        }

        if ((TurnPhase::PerformAnim == turnPhase_) &&
            (AnimPhase::PostMoveTowardPause == animPhase_))
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
            //check for death by condition effect
            auto const & CREATURE_EFFECTS_VEC{ fightResult_.Effects() };
            if ((CREATURE_EFFECTS_VEC.empty() == false) &&
                (CREATURE_EFFECTS_VEC[0].WasKill()) &&
                (CREATURE_EFFECTS_VEC[0].GetCreature()->IsPlayerCharacter() == false))
            {
                SetTurnPhase(TurnPhase::DeathAnim);

                //Note:  This creature ptr should be the same as turnCreaturePtr_
                auto const CREATURE_PTR{ CREATURE_EFFECTS_VEC[0].GetCreature() };

                conditionEffectsCenterPosV_ = combatDisplayStagePtr_->
                    GetCombatNodeForCreature(CREATURE_PTR)->GetEntityPos();

                combatAnimationUPtr_->ShakeAnimStop(CREATURE_PTR);
                combatAnimationUPtr_->DeathAnimStart( { CREATURE_PTR } );

                slider_.Reset(ANIM_DEATH_SLIDER_SPEED_);
                //TODO SOUNDEFFECT play creature death sound effect here
                return;
            }

            if (++conditionEffectsIndex_ < conditionEffectsVec_.size())
            {
                conditionEffectsCenterPosV_ = sf::Vector2f(0.0f, 0.0f);

                fightResult_ = combat::FightResult(combat::CreatureEffect(turnCreaturePtr_,
                    combat::HitInfoVec_t(1, conditionEffectsVec_[conditionEffectsIndex_])));

                HandleApplyDamageTasks();

                SetTurnPhase(TurnPhase::ConditionEffectPause);
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
                    if (turnCreaturePtr_->IsPlayerCharacter())
                    {
                        SetTurnPhase(TurnPhase::Determine);
                        SetUserActionAllowed(true);
                    }
                    else
                    {
                        SetTurnPhase(TurnPhase::PostCenterAndZoomInPause);

                        auto pauseToUse{ PRE_TURN_PAUSE_SEC_ };
                        if (turnCreaturePtr_->CanTakeAction() == false)
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

        if (IsPlayerCharacterTurnValid() &&
            (TurnPhase::PostCenterAndZoomOutPause == turnPhase_))
        {
            StartPerformAnim();
            return;
        }

        if (IsNonPlayerCharacterTurnValid() &&
            (TurnPhase::PostCenterAndZoomInPause == turnPhase_))
        {
            SetTurnPhase(TurnPhase::Determine);
            HandleEnemyTurnStep1_Decide();

            if (turnActionInfo_.Action() == combat::TurnAction::Nothing)
            {
                AppendStatusMessage(combat::Text::ActionText(turnCreaturePtr_,
                                                             turnActionInfo_,
                                                             fightResult_,
                                                             true,
                                                             true), false);

                SetTurnPhase(TurnPhase::PostTurnPause);
                StartPause(POST_TURN_PAUSE_SEC_, "PostTurn");
            }
            else
            {
                //also do the perform step here so that the TurnBox can display the non-player
                //creature's intent before showing the result
                SetTurnActionPhase( HandleEnemyTurnStep2_Perform() );

                //collect a list of all attacking and targeted creatures that need to be
                //centered on the screen
                creature::CreaturePVec_t allEffectedCreaturesPVec{ turnCreaturePtr_ };
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
                    combatAnimationUPtr_->CenteringStart(allEffectedCreaturesPVec);
                    slider_.Reset(ANIM_CENTERING_SLIDER_SPEED_);
                }
            }
            return;
        }

        if (IsNonPlayerCharacterTurnValid() &&
            (TurnPhase::PostCenterAndZoomOutPause == turnPhase_))
        {
            //See above for call to HandleEnemyTurnStep2_Perform()
            StartPerformAnim();
            SetupTurnBox();
            return;
        }

        if (TurnPhase::PerformReport == turnPhase_)
        {
            auto const CREATURE_EFFECTS_VEC{ fightResult_.Effects() };
            if ((isFightResultCollapsed_ == false) &&
                (performReportEffectIndex_ < CREATURE_EFFECTS_VEC.size()))
            {
                auto const HIT_INFO_VEC_SIZE{ CREATURE_EFFECTS_VEC.at(
                    performReportEffectIndex_).GetHitInfoVec().size() };

                if ((HIT_INFO_VEC_SIZE > 0) &&
                    (performReportHitIndex_ <  (HIT_INFO_VEC_SIZE - 1)))
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
            auto const CREATURE_EFFECTS{ fightResult_.Effects() };
            for (auto const & NEXT_CREATURE_EFFECT : CREATURE_EFFECTS)
            {
                if ((NEXT_CREATURE_EFFECT.WasKill()) &&
                    (NEXT_CREATURE_EFFECT.GetCreature()->IsPlayerCharacter() == false))
                {
                    killedNonPlayerCreaturesPVec.push_back(NEXT_CREATURE_EFFECT.GetCreature());
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
                combatAnimationUPtr_->DeathAnimStart(killedNonPlayerCreaturesPVec);
                slider_.Reset(ANIM_DEATH_SLIDER_SPEED_);
                //TODO SOUNDEFFECT play creature death sound effect here
            }
            return;
        }

        if (TurnPhase::PostTurnPause == turnPhase_)
        {
            //end-of-turn tasks here
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


    void CombatStage::HandleEnemyTurnStep1_Decide()
    {
        turnActionInfo_ = combat::TurnDecider::Decide(turnCreaturePtr_, combatDisplayStagePtr_);
        combat::Encounter::Instance()->SetTurnActionInfo(turnCreaturePtr_, turnActionInfo_);
        SetupTurnBox();
    }


    CombatStage::TurnActionPhase CombatStage::HandleEnemyTurnStep2_Perform()
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
                fightResult_ = combat::FightClub::Fight(turnCreaturePtr_,
                                                        turnActionInfo_.Target());

                SetupTurnBox();
                return GetTurnActionPhaseFromFightResult(fightResult_);
            }

            case combat::TurnAction::Block:
            {
                fightResult_ = combat::FightResult();
                SetupTurnBox();
                AppendStatusMessage(combat::Text::ActionText(turnCreaturePtr_,
                                                             turnActionInfo_,
                                                             fightResult_,
                                                             true,
                                                             true), false);

                return TurnActionPhase::PauseAndReport;
            }

            case combat::TurnAction::Fly:
            case combat::TurnAction::Land:
            {
                fightResult_ = combat::FightResult();
                combatDisplayStagePtr_->HandleFlyingChange(turnCreaturePtr_, false);
                SetupTurnBox();

                AppendStatusMessage(combat::Text::ActionText(turnCreaturePtr_,
                                                             turnActionInfo_,
                                                             fightResult_,
                                                             true,
                                                             true), false);
                
                return TurnActionPhase::PauseAndReport;
            }

            case combat::TurnAction::Cast:
            {
                creature::CreaturePVec_t targetedCreaturesPVec{ turnActionInfo_.Targets() };
                combatDisplayStagePtr_->SortCreatureListByDisplayedPosition(targetedCreaturesPVec);

                fightResult_ = combat::FightClub::Cast(turnActionInfo_.Spell(),
                                                       turnCreaturePtr_,
                                                       targetedCreaturesPVec);
                
                SetupTurnBox();

                AppendStatusMessage(combat::Text::ActionText(turnCreaturePtr_,
                                                             turnActionInfo_,
                                                             fightResult_,
                                                             true,
                                                             true), false);
                
                return TurnActionPhase::Cast;
            }

            case combat::TurnAction::PlaySong:
            {
                creature::CreaturePVec_t targetedCreaturesPVec{ turnActionInfo_.Targets() };
                combatDisplayStagePtr_->SortCreatureListByDisplayedPosition(targetedCreaturesPVec);

                fightResult_ = combat::FightClub::PlaySong(turnActionInfo_.Song(),
                                                           turnCreaturePtr_,
                                                           targetedCreaturesPVec);
                
                SetupTurnBox();
                
                AppendStatusMessage(combat::Text::ActionText(turnCreaturePtr_,
                                                             turnActionInfo_,
                                                             fightResult_,
                                                             true,
                                                             true), false);

                return TurnActionPhase::Cast;//reuse the cast TurnActionPhase intentionally here
            }

            case combat::TurnAction::SkyPounce:
            case combat::TurnAction::LandPounce:
            {
                fightResult_ = combat::FightClub::Pounce(turnCreaturePtr_,
                                                         turnActionInfo_.Target());

                SetupTurnBox();

                AppendStatusMessage(combat::Text::ActionText(turnCreaturePtr_,
                                                             turnActionInfo_,
                                                             fightResult_,
                                                             true,
                                                             true), false);

                //TODO If fightResult_ says the pounce was a success then
                //     return the TurnActionPhase::Pounce
                return TurnActionPhase::PauseAndReport;
            }

            case combat::TurnAction::Roar:
            {
                fightResult_ = combat::FightClub::Roar(turnCreaturePtr_, combatDisplayStagePtr_);
                SetupTurnBox();

                AppendStatusMessage(combat::Text::ActionText(turnCreaturePtr_,
                                                             turnActionInfo_,
                                                             fightResult_,
                                                             true,
                                                             true), false);

                //TODO if fightResult_ says the roar happened then return TurnActionPhase::Roar
                return TurnActionPhase::PauseAndReport;
            }

            case combat::TurnAction::ChangeWeapon:
            case combat::TurnAction::Nothing:
            case combat::TurnAction::Count://handle Count gracefully because it is sometimes required
            {
                fightResult_ = combat::FightResult();
                SetupTurnBox();

                AppendStatusMessage(combat::Text::ActionText(turnCreaturePtr_,
                                                             turnActionInfo_,
                                                             fightResult_,
                                                             true,
                                                             true), false);
                
                return TurnActionPhase::PauseAndReport;
            }

            default:
            {
                std::ostringstream ss;
                ss << "game::stage::CombatStage::HandleEnemyTurnStep2_Perform() found "
                    << "turnActionInfo_.Action() (which is really a combat::TurnAction) of "
                    << turnActionInfo_.Action() << " which is invalid.";

                throw std::runtime_error(ss.str());
            }
        }
    }


    //start centering anim
    void CombatStage::StartTurn_Step1()
    {
        zoomSliderOrigPos_ = zoomSliderBarUPtr_->GetCurrentValue();
        turnCreaturePtr_ = combat::Encounter::Instance()->CurrentTurnCreature();
        SetTurnPhase(TurnPhase::CenterAndZoomIn);
        combatAnimationUPtr_->CenteringStart(turnCreaturePtr_);
        slider_.Reset(ANIM_CENTERING_SLIDER_SPEED_);
    }


    //start pre-pause
    void CombatStage::StartTurn_Step2()
    {
        auto const IS_PLAYER_TURN{ turnCreaturePtr_->IsPlayerCharacter() };
        combatDisplayStagePtr_->SetIsPlayerTurn(IS_PLAYER_TURN);

        combatAnimationUPtr_->CenteringStop();

        combatAnimationUPtr_->ShakeAnimStart(turnCreaturePtr_,
                                             ANIM_CREATURE_SHAKE_SLIDER_SPEED_,
                                             false);

        goldTextColorShaker_.Reset();

        if (conditionEffectsWillSkip_)
        {
            conditionEffectsWillSkip_ = false;
        }
        else
        {
            conditionEffectsTookTurn_ = combat::ConditionEffects::Process(Phase::Combat,
                                                                          turnCreaturePtr_,
                                                                          conditionEffectsVec_);
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

                auto pauseToUse{ PRE_TURN_PAUSE_SEC_ };
                if (turnCreaturePtr_->CanTakeAction() == false)
                {
                    pauseToUse *= 3.0f;
                }

                StartPause(pauseToUse, "PostZoomIn");
            }
        }
        else
        {
            conditionEffectsIndex_ = 0;
            
            fightResult_ = combat::FightResult( combat::CreatureEffect(turnCreaturePtr_,
                combat::HitInfoVec_t(1, conditionEffectsVec_[conditionEffectsIndex_])) );

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

        turnCreaturePtr_ = nullptr;
        spellBeingCastPtr_ = nullptr;

        MoveTurnBoxObjectsOffScreen(true);

        goldTextColorShaker_.Reset();
        redTextColorShaker_.Reset();

        conditionEffectsCenterPosV_ = sf::Vector2f(0.0f, 0.0f);

        conditionEffectsWillSkip_ = false;

        HandleWin();
        HandleLose();
    }


    void CombatStage::PositionSlideStartTasks()
    {
        if ((misc::IsRealZero(conditionEffectsCenterPosV_.x)) &&
            (misc::IsRealZero(conditionEffectsCenterPosV_.y)))
        {
            combatAnimationUPtr_->RepositionAnimStart(turnCreaturePtr_);
        }
        else
        {
            combatAnimationUPtr_->RepositionAnimStart(conditionEffectsCenterPosV_);
        }

        combatAnimationUPtr_->ShakeAnimStop(turnCreaturePtr_);
        slider_.Reset(ANIM_CREATURE_POS_SLIDER_SPEED_);
    }


    bool CombatStage::HandleAttack()
    {
        auto const MOUSEOVER_STR( combat::Text::MouseOverTextAttackStr(turnCreaturePtr_,
                                                                       combatDisplayStagePtr_) );

        if (MOUSEOVER_STR != combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_ATTACK_)
        {
            QuickSmallPopup(MOUSEOVER_STR, false, true);
            return false;
        }
        else
        {
            HandleAttackTasks( combat::FightClub::FindNonPlayerCreatureToAttack(
                turnCreaturePtr_, combatDisplayStagePtr_) );
            return true;
        }
    }


    bool CombatStage::HandleFight()
    {
        auto const MOUSEOVER_STR( combat::Text::MouseOverTextFightStr(
            turnCreaturePtr_, combatDisplayStagePtr_) );

        if (MOUSEOVER_STR != combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_FIGHT_)
        {
            QuickSmallPopup(MOUSEOVER_STR, false, true);
            return false;
        }
        else
        {
            //can't set turnActionInfo_ or fightResult_ or turnActionPhase_ yet because the player
            //must select which non_player creature to fight first
            combatDisplayStagePtr_->SetSummaryViewAllowed(false);
            combatDisplayStagePtr_->SetScrollingAllowed(true);
            SetTurnPhase(TurnPhase::TargetSelect);
            SetupTurnBox();
            return true;
        }
    }


    bool CombatStage::HandleSong_Step1_ValidatePlayAndSelectSong()
    {
        //TODO
        return false;
    }


    bool CombatStage::HandleCast_Step1_ValidateCastAndSelectSpell()
    {
        auto const MOUSEOVER_STR( combat::Text::MouseOverTextCastStr(turnCreaturePtr_,
                                                                     combatDisplayStagePtr_) );

        if (MOUSEOVER_STR != combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_CAST_)
        {
            QuickSmallPopup(MOUSEOVER_STR, false, true);
            return false;
        }
        else
        {
            SetUserActionAllowed(false);
            SetTurnActionPhase(TurnActionPhase::Cast);
            
            auto const POPUP_INFO{
                sfml_util::gui::PopupManager::Instance()->CreateSpellbookPopupInfo(
                    POPUP_NAME_SPELLBOOK_,
                    turnCreaturePtr_,
                    turnCreaturePtr_->LastSpellCastNum()) };

            LoopManager::Instance()->PopupWaitBegin(this, POPUP_INFO);
            return true;
        }
    }


    void CombatStage::HandleCast_Step2_SelectTargetOrPerformOnAll()
    {
        if ((spellBeingCastPtr_->Target() == TargetType::SingleCompanion) ||
            (spellBeingCastPtr_->Target() == TargetType::SingleOpponent))
        {
            SetUserActionAllowed(true);
            combatDisplayStagePtr_->SetSummaryViewAllowed(false);
            combatDisplayStagePtr_->SetScrollingAllowed(true);
            SetTurnPhase(TurnPhase::TargetSelect);
            SetupTurnBox();
        }
        else if (spellBeingCastPtr_->Target() == TargetType::AllCompanions)
        {
            HandleCast_Step3_PerformOnTargets(creature::Algorithms::Players(false));
        }
        else if (spellBeingCastPtr_->Target() == TargetType::AllOpponents)
        {
            HandleCast_Step3_PerformOnTargets(creature::Algorithms::NonPlayers(true));
        }
        else
        {
            std::ostringstream ssErr;
            ssErr << "game::stage::CombatStage::HandleCast_Step2_SelectTargetOrPerformOnAll(spell="
                << spellBeingCastPtr_->Name() << ") had a target_type of "
                << TargetType::ToString(spellBeingCastPtr_->Target())
                << " which is not yet supported in combat stage.";

            throw std::runtime_error(ssErr.str());
        }
    }


    void CombatStage::HandleCast_Step3_PerformOnTargets(
        creature::CreaturePVec_t creaturesToCastUponPVec)
    {
        SetUserActionAllowed(false);

        combatDisplayStagePtr_->SortCreatureListByDisplayedPosition(creaturesToCastUponPVec);
        turnActionInfo_ = combat::TurnActionInfo(spellBeingCastPtr_, creaturesToCastUponPVec);
        combat::Encounter::Instance()->SetTurnActionInfo(turnCreaturePtr_, turnActionInfo_);

        fightResult_ = combat::FightClub::Cast(spellBeingCastPtr_,
                                               turnCreaturePtr_,
                                               creaturesToCastUponPVec);

        SetTurnActionPhase(TurnActionPhase::Cast);

        SetTurnPhase(TurnPhase::CenterAndZoomOut);
        isShortPostZoomOutPause_ = true;
        auto creaturesToCenterOnPVec{ creaturesToCastUponPVec };
        creaturesToCenterOnPVec.push_back(turnCreaturePtr_);
        combatAnimationUPtr_->CenteringStart(creaturesToCenterOnPVec);
        slider_.Reset(ANIM_CENTERING_SLIDER_SPEED_);

        SetupTurnBox();
    }


    bool CombatStage::HandleAdvance()
    {
        auto const MOUSEOVER_STR( combat::Text::MouseOverTextAdvanceStr(turnCreaturePtr_,
                                                                        combatDisplayStagePtr_) );

        if (MOUSEOVER_STR != combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_ADVANCE_)
        {
            QuickSmallPopup(MOUSEOVER_STR, false, true);
            return false;
        }
        else
        {
            SetUserActionAllowed(false);

            turnActionInfo_ = combat::TurnActionInfo(combat::TurnAction::Advance);
            combat::Encounter::Instance()->SetTurnActionInfo(turnCreaturePtr_, turnActionInfo_);
            SetTurnActionPhase(TurnActionPhase::Advance);
            StartPerformAnim();
            return true;
        }
    }


    bool CombatStage::HandleRetreat()
    {
        auto const MOUSEOVER_STR( combat::Text::MouseOverTextRetreatStr(turnCreaturePtr_,
                                                                        combatDisplayStagePtr_) );

        if (MOUSEOVER_STR != combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_RETREAT_)
        {
            QuickSmallPopup(MOUSEOVER_STR, false, true);
            return false;
        }
        else
        {
            SetUserActionAllowed(false);

            turnActionInfo_ = combat::TurnActionInfo(combat::TurnAction::Retreat);
            combat::Encounter::Instance()->SetTurnActionInfo(turnCreaturePtr_, turnActionInfo_);
            SetTurnActionPhase(TurnActionPhase::Retreat);
            StartPerformAnim();
            return true;
        }
    }


    bool CombatStage::HandleBlock()
    {
        auto const MOUSEOVER_STR( combat::Text::MouseOverTextBlockStr(turnCreaturePtr_,
                                                                      combatDisplayStagePtr_) );

        if (MOUSEOVER_STR != combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_BLOCK_)
        {
            QuickSmallPopup(MOUSEOVER_STR, false, true);
            return false;
        }
        else
        {
            SetUserActionAllowed(false);

            turnActionInfo_ = combat::TurnActionInfo(combat::TurnAction::Block);
            combat::Encounter::Instance()->SetTurnActionInfo(turnCreaturePtr_, turnActionInfo_);

            //no need for ZoomAndSlide or PerformAnim so skip to end of turn
            //with AppendStatusMessage()
            AppendStatusMessage(combat::Text::ActionText(turnCreaturePtr_,
                                                         turnActionInfo_,
                                                         fightResult_,
                                                         true,
                                                         true), true);
            return true;
        }
    }


    bool CombatStage::HandleSkip()
    {
        if (skipTBoxButtonSPtr_->IsDisabled())
            return false;

        SetUserActionAllowed(false);

        turnActionInfo_ = combat::TurnActionInfo(combat::TurnAction::Nothing);
        combat::Encounter::Instance()->SetTurnActionInfo(turnCreaturePtr_, turnActionInfo_);

        //no need for ZoomAndSlide or PerformAnim so skip to end of turn with AppendStatusMessage()
        AppendStatusMessage(combat::Text::ActionText(turnCreaturePtr_,
                                                     turnActionInfo_,
                                                     fightResult_,
                                                     true,
                                                     true), true);

        return true;
    }


    bool CombatStage::HandleFly()
    {
        auto const MOUSEOVER_STR( combat::Text::MouseOverTextFlyStr(turnCreaturePtr_,
                                                                    combatDisplayStagePtr_) );

        if (MOUSEOVER_STR != combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_FLY_)
        {
            QuickSmallPopup(MOUSEOVER_STR, false, true);
            return false;
        }
        else
        {
            SetUserActionAllowed(false);

            turnActionInfo_ = combat::TurnActionInfo(combat::TurnAction::Fly);
            combat::Encounter::Instance()->SetTurnActionInfo(turnCreaturePtr_, turnActionInfo_);
            combat::Encounter::Instance()->SetIsFlying(turnCreaturePtr_, true);

            combatDisplayStagePtr_->HandleFlyingChange(turnCreaturePtr_, true);

            //no need for turnActionPhase_, ZoomAndSlide, or PerformAnim so skip
            //to end of turn with AppendStatusMessage()
            AppendStatusMessage(combat::Text::ActionText(turnCreaturePtr_,
                                                         turnActionInfo_,
                                                         fightResult_,
                                                         true,
                                                         true), true);

            return true;
        }
    }


    bool CombatStage::HandleLand()
    {
        auto const MOUSEOVER_STR( combat::Text::MouseOverTextLandStr(turnCreaturePtr_,
                                                                     combatDisplayStagePtr_) );

        if (MOUSEOVER_STR != combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_LAND_)
        {
            QuickSmallPopup(MOUSEOVER_STR, false, true);
            return false;
        }
        else
        {
            SetUserActionAllowed(false);

            turnActionInfo_ = combat::TurnActionInfo(combat::TurnAction::Land);
            combat::Encounter::Instance()->SetTurnActionInfo(turnCreaturePtr_, turnActionInfo_);
            combat::Encounter::Instance()->SetIsFlying(turnCreaturePtr_, false);

            combatDisplayStagePtr_->HandleFlyingChange(turnCreaturePtr_, false);

            //no need for turnActionPhase_, ZoomAndSlide, or PerformAnim so skip
            //to end of turn with AppendStatusMessage()
            AppendStatusMessage(combat::Text::ActionText(turnCreaturePtr_,
                                                         turnActionInfo_,
                                                         fightResult_,
                                                         true,
                                                         true), false);

            return true;
        }
    }


    bool CombatStage::HandleRoar()
    {
        auto const MOUSEOVER_STR( combat::Text::MouseOverTextRoarStr(turnCreaturePtr_,
                                                                     combatDisplayStagePtr_) );

        if (MOUSEOVER_STR != combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_ROAR_)
        {
            QuickSmallPopup(MOUSEOVER_STR, false, true);
            return false;
        }
        else
        {
            SetUserActionAllowed(false);

            turnActionInfo_ = combat::TurnActionInfo(combat::TurnAction::Roar);
            combat::Encounter::Instance()->SetTurnActionInfo(turnCreaturePtr_, turnActionInfo_);
            fightResult_ = combat::FightClub::Roar(turnCreaturePtr_, combatDisplayStagePtr_);

            AppendStatusMessage(combat::Text::ActionText(turnCreaturePtr_,
                                                         turnActionInfo_,
                                                         fightResult_,
                                                         true,
                                                         true), false);

            SetTurnActionPhase(TurnActionPhase::Roar);

            //not implemented yet so skip to end of turn with AppendStatusMessage()
            AppendStatusMessage(combat::Text::ActionText(turnCreaturePtr_,
                                                         turnActionInfo_,
                                                         fightResult_,
                                                         true,
                                                         true), false);

            return true;
        }
    }


    bool CombatStage::HandlePounce(const bool IS_SKY_POUNCE)
    {
        auto const MOUSEOVER_STR( combat::Text::MouseOverTextPounceStr(turnCreaturePtr_,
                                                                       combatDisplayStagePtr_) );

        if (MOUSEOVER_STR != combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_POUNCE_)
        {
            QuickSmallPopup(MOUSEOVER_STR, false, true);
            return false;
        }
        else
        {
            SetUserActionAllowed(false);

            turnActionInfo_ = combat::TurnActionInfo(((IS_SKY_POUNCE) ?
                combat::TurnAction::SkyPounce : combat::TurnAction::LandPounce));

            combat::Encounter::Instance()->SetTurnActionInfo(turnCreaturePtr_, turnActionInfo_);

            //not implemented yet so skip to end of turn with AppendStatusMessage()
            AppendStatusMessage(combat::Text::ActionText(turnCreaturePtr_,
                                                         turnActionInfo_,
                                                         fightResult_,
                                                         true,
                                                         true), true);

            return true;
        }
    }


    bool CombatStage::HandleWeaponChange()
    {
        turnActionInfo_ = combat::TurnActionInfo(combat::TurnAction::ChangeWeapon);
        combat::Encounter::Instance()->SetTurnActionInfo(turnCreaturePtr_, turnActionInfo_);
        turnCreaturePtr_->CurrentWeaponsInc();
        SetupTurnBox();
        return true;
    }


    void CombatStage::MoveTurnBoxObjectsOffScreen(const bool WILL_MOVE_SKIP_BUTTON)
    {
        titleTBoxTextRegionUPtr_->MoveEntityOffScreen();
        weaponTBoxTextRegionUPtr_->MoveEntityOffScreen();
        armorTBoxTextRegionUPtr_->MoveEntityOffScreen();
        infoTBoxTextRegionUPtr_->MoveEntityOffScreen();
        enemyActionTBoxRegionUPtr_->MoveEntityOffScreen();
        enemyCondsTBoxRegionUPtr_->MoveEntityOffScreen();

        MoveTurnBoxButtonsOffScreen(WILL_MOVE_SKIP_BUTTON);
    }


    void CombatStage::MoveTurnBoxButtonsOffScreen(const bool WILL_MOVE_SKIP_BUTTON)
    {
        attackTBoxButtonSPtr_->MoveEntityOffScreen();
        fightTBoxButtonSPtr_->MoveEntityOffScreen();
        castTBoxButtonSPtr_->MoveEntityOffScreen();
        advanceTBoxButtonSPtr_->MoveEntityOffScreen();
        retreatTBoxButtonSPtr_->MoveEntityOffScreen();
        blockTBoxButtonSPtr_->MoveEntityOffScreen();
        skipTBoxButtonSPtr_->MoveEntityOffScreen();
        flyTBoxButtonSPtr_->MoveEntityOffScreen();
        landTBoxButtonSPtr_->MoveEntityOffScreen();
        roarTBoxButtonSPtr_->MoveEntityOffScreen();
        pounceTBoxButtonSPtr_->MoveEntityOffScreen();

        if (WILL_MOVE_SKIP_BUTTON)
        {
            skipTBoxButtonSPtr_->MoveEntityOffScreen();
        }
    }


    void CombatStage::SetupTurnBoxButtons(const creature::CreaturePtrC_t CREATURE_CPTRC)
    {
        if (CREATURE_CPTRC->IsPlayerCharacter())
        {
            tBoxStdButtonSVec_.clear();

            //attack button
            const std::string MOT_ATTACK_STR(
                combat::Text::MouseOverTextAttackStr(CREATURE_CPTRC,
                    combatDisplayStagePtr_));

            attackTBoxButtonSPtr_->SetIsDisabled(MOT_ATTACK_STR !=
                combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_ATTACK_);

            attackTBoxButtonSPtr_->SetMouseHoverText(MOT_ATTACK_STR);

            if (MOT_ATTACK_STR == combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_ATTACK_)
            {
                tBoxStdButtonSVec_.push_back(attackTBoxButtonSPtr_);
            }

            //fight button
            const std::string MOT_FIGHT_STR(combat::Text::MouseOverTextFightStr(
                CREATURE_CPTRC, combatDisplayStagePtr_));

            fightTBoxButtonSPtr_->SetIsDisabled(MOT_FIGHT_STR !=
                combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_FIGHT_);

            fightTBoxButtonSPtr_->SetMouseHoverText(MOT_FIGHT_STR);

            if (MOT_FIGHT_STR == combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_FIGHT_)
            {
                tBoxStdButtonSVec_.push_back(fightTBoxButtonSPtr_);
            }

            //cast/play button
            if ((turnCreaturePtr_ != nullptr) &&
                (turnCreaturePtr_->Role().Which() == creature::role::Bard))
            {
                castTBoxButtonSPtr_->SetText("(S)ong");

                const std::string MOT_PLAY_STR(combat::Text::MouseOverTextPlayStr(
                    CREATURE_CPTRC, combatDisplayStagePtr_));

                castTBoxButtonSPtr_->SetIsDisabled(MOT_PLAY_STR !=
                    combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_CAST_);

                castTBoxButtonSPtr_->SetMouseHoverText(MOT_PLAY_STR);

                if (MOT_PLAY_STR == combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_PLAY_)
                {
                    tBoxStdButtonSVec_.push_back(castTBoxButtonSPtr_);
                }
            }
            else
            {
                castTBoxButtonSPtr_->SetText("(S)pell");

                const std::string MOT_CAST_STR(combat::Text::MouseOverTextCastStr(
                    CREATURE_CPTRC, combatDisplayStagePtr_));

                castTBoxButtonSPtr_->SetIsDisabled(MOT_CAST_STR !=
                    combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_CAST_);

                castTBoxButtonSPtr_->SetMouseHoverText(MOT_CAST_STR);

                if (MOT_CAST_STR == combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_CAST_)
                {
                    tBoxStdButtonSVec_.push_back(castTBoxButtonSPtr_);
                }
            }

            //advance button
            const std::string MOT_ADVANCE_STR(combat::Text::MouseOverTextAdvanceStr(
                CREATURE_CPTRC, combatDisplayStagePtr_));

            advanceTBoxButtonSPtr_->SetIsDisabled(MOT_ADVANCE_STR !=
                combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_ADVANCE_);

            advanceTBoxButtonSPtr_->SetMouseHoverText(MOT_ADVANCE_STR);

            //retreat button
            const std::string MOT_RETREAT_STR(combat::Text::MouseOverTextRetreatStr(
                CREATURE_CPTRC, combatDisplayStagePtr_));

            retreatTBoxButtonSPtr_->SetIsDisabled(MOT_RETREAT_STR !=
                combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_RETREAT_);

            retreatTBoxButtonSPtr_->SetMouseHoverText(MOT_RETREAT_STR);

            //block button
            const std::string MOT_BLOCK_STR(combat::Text::MouseOverTextBlockStr(
                CREATURE_CPTRC, combatDisplayStagePtr_));

            blockTBoxButtonSPtr_->SetIsDisabled(MOT_BLOCK_STR !=
                combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_BLOCK_);

            blockTBoxButtonSPtr_->SetMouseHoverText(MOT_BLOCK_STR);

            //fly button
            const std::string MOT_FLY_STR(combat::Text::MouseOverTextFlyStr(
                CREATURE_CPTRC, combatDisplayStagePtr_));

            flyTBoxButtonSPtr_->SetIsDisabled(MOT_FLY_STR !=
                combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_FLY_);

            flyTBoxButtonSPtr_->SetMouseHoverText(MOT_FLY_STR);

            if (MOT_FLY_STR == combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_FLY_)
            {
                tBoxStdButtonSVec_.push_back(flyTBoxButtonSPtr_);
            }

            //land
            const std::string MOT_LAND_STR(combat::Text::MouseOverTextLandStr(
                CREATURE_CPTRC, combatDisplayStagePtr_));

            landTBoxButtonSPtr_->SetIsDisabled(MOT_LAND_STR !=
                combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_LAND_);

            landTBoxButtonSPtr_->SetMouseHoverText(MOT_LAND_STR);

            if (MOT_LAND_STR == combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_LAND_)
            {
                tBoxStdButtonSVec_.push_back(landTBoxButtonSPtr_);
            }

            //roar button
            const std::string MOT_ROAR_STR(combat::Text::MouseOverTextRoarStr(
                CREATURE_CPTRC, combatDisplayStagePtr_));

            roarTBoxButtonSPtr_->SetIsDisabled(MOT_ROAR_STR !=
                combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_ROAR_);

            roarTBoxButtonSPtr_->SetMouseHoverText(MOT_ROAR_STR);

            if (MOT_ROAR_STR == combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_ROAR_)
            {
                tBoxStdButtonSVec_.push_back(roarTBoxButtonSPtr_);
            }

            //pounce button
            const std::string MOT_POUNCE_STR(combat::Text::MouseOverTextPounceStr(
                CREATURE_CPTRC, combatDisplayStagePtr_));

            pounceTBoxButtonSPtr_->SetIsDisabled(MOT_POUNCE_STR !=
                combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_POUNCE_);

            pounceTBoxButtonSPtr_->SetMouseHoverText(MOT_POUNCE_STR);

            if (MOT_POUNCE_STR == combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_POUNCE_)
            {
                tBoxStdButtonSVec_.push_back(pounceTBoxButtonSPtr_);
            }

            //'can take action' adjustments
            auto const CAN_TAKE_ACTION_STR(CREATURE_CPTRC->CanTakeActionStr());
            if (CAN_TAKE_ACTION_STR.empty())
            {
                skipTBoxButtonSPtr_->SetIsDisabled(true);
                skipTBoxButtonSPtr_->MoveEntityOffScreen();
                //blockTBoxButtonSPtr_ position already set above
                tBoxStdButtonSVec_.push_back(blockTBoxButtonSPtr_);
            }
            else
            {
                skipTBoxButtonSPtr_->SetIsDisabled(false);
                blockTBoxButtonSPtr_->MoveEntityOffScreen();
                tBoxStdButtonSVec_.push_back(skipTBoxButtonSPtr_);
            }

            //position turn buttons
            auto const BUTTON_ROW_WIDTH(sfml_util::MapByRes(55.0f, 175.0f));

            auto const RIGHTROW_POS_LEFT((turnBoxRegion_.left + turnBoxRegion_.width) -
                BUTTON_ROW_WIDTH);

            auto const RIGHTROW_VERT_POS_OFFSET(turnBoxRegion_.height /
                (static_cast<float>(tBoxStdButtonSVec_.size()) + 1.0f));

            const size_t NUM_RIGHTROW_STD_BUTTONS(tBoxStdButtonSVec_.size());

            for (size_t i(0); i < NUM_RIGHTROW_STD_BUTTONS; ++i)
            {
                auto const POS_LEFT{ RIGHTROW_POS_LEFT -
                    (tBoxStdButtonSVec_[i]->GetEntityRegion().width * 0.5f) };

                auto const POS_TOP{ turnBoxRegion_.top + (RIGHTROW_VERT_POS_OFFSET * 0.5f) +
                    (static_cast<float>(i) * RIGHTROW_VERT_POS_OFFSET) };

                tBoxStdButtonSVec_[i]->SetEntityPos(POS_LEFT, POS_TOP);
            }
        }
        else
        {
            MoveTurnBoxButtonsOffScreen(true);
        }
    }


    void CombatStage::QuickSmallPopup(const std::string & PROMPT,
                                      const bool          IS_SOUNDEFFECT_NORMAL,
                                      const bool          WILL_PREPEND_NEWLINE)
    {
        std::ostringstream ss;
        ss << "\n" << ((WILL_PREPEND_NEWLINE) ? "\n" : "") << PROMPT;
        const PopupInfo POPUP_INFO(sfml_util::gui::PopupManager::Instance()->CreatePopupInfo(
            "CombatStage'sQuickPopup",
            ss.str(),
            sfml_util::PopupButtons::Okay,
            sfml_util::PopupImage::Regular,
            sfml_util::Justified::Center,
            ((IS_SOUNDEFFECT_NORMAL) ?
                sfml_util::sound_effect::PromptGeneric : sfml_util::sound_effect::PromptWarn),
            Popup::Generic,
            sfml_util::FontManager::Instance()->Size_Normal()));

        LoopManager::Instance()->PopupWaitBegin(this, POPUP_INFO);
    }


    void CombatStage::SetupTurnBox()
    {
        auto const EMPTY_STR(" ");//any short all-whitespace non-empty string will work here
        auto const CAN_TAKE_ACTION_STR(turnCreaturePtr_->CanTakeActionStr());
        auto const CURR_WEAPONS_STR(turnCreaturePtr_->WeaponsString());

        std::ostringstream weaponsSS;
        if (CURR_WEAPONS_STR.empty())
        {
            weaponsSS << "(holding no weapons, can't attack)";
        }
        else
        {
            weaponsSS << "Weapon";
            if (boost::algorithm::contains(CURR_WEAPONS_STR, ","))
                weaponsSS << "s";

            weaponsSS << ":  " << CURR_WEAPONS_STR;
        }
        const std::string HOLDING_WEAPON_STR(weaponsSS.str());

        std::ostringstream titleSS;
        std::ostringstream weaponHoldingSS;
        std::ostringstream armorSS;
        std::ostringstream infoSS;
        std::ostringstream preambleSS;
        std::ostringstream enemyCondsSS;

        auto isPreambleShowing{ false };

        if (IsPlayerCharacterTurnValid())
        {
            willRedColorShakeWeaponText_ = false;

            titleSS << turnCreaturePtr_->Name() << "'s Turn";

            weaponHoldingSS << HOLDING_WEAPON_STR;

            const stats::StatSet STATS(turnCreaturePtr_->Stats());

            infoSS << " "   << STATS.Str().Abbr(true) << ":   "  << STATS.Str().Normal() << " " << STATS.Str().ModifiedCurrentStr(true)
                   << "\n " << STATS.Acc().Abbr(true) << ":  "   << STATS.Acc().Normal() << " " << STATS.Acc().ModifiedCurrentStr(true)
                   << "\n " << STATS.Cha().Abbr(true) << ":  "   << STATS.Cha().Normal() << " " << STATS.Cha().ModifiedCurrentStr(true)
                   << "\n " << STATS.Lck().Abbr(true) << ":  "   << STATS.Lck().Normal() << " " << STATS.Lck().ModifiedCurrentStr(true)
                   << "\n " << STATS.Spd().Abbr(true) << ":  "   << STATS.Spd().Normal() << " " << STATS.Spd().ModifiedCurrentStr(true)
                   << "\n " << STATS.Int().Abbr(true) << ":    " << STATS.Int().Normal() << " " << STATS.Int().ModifiedCurrentStr(true)
                   << "\nHealth:  " << turnCreaturePtr_->HealthCurrent() << "/" << turnCreaturePtr_->HealthNormal()
                   << "\nCondition";

            if (turnCreaturePtr_->Conditions().size() > 1)
                infoSS << "s";

            infoSS << ":  " << turnCreaturePtr_->ConditionNames(6);

            armorSS << "Armor Rating: " << turnCreaturePtr_->ArmorRating();

            if (turnCreaturePtr_->ManaNormal() > 0)
            {
                armorSS << "\n\nMana: " << turnCreaturePtr_->ManaCurrent() << "/" << turnCreaturePtr_->ManaNormal();
            }

            preambleSS.str(EMPTY_STR);
            enemyCondsSS.str(EMPTY_STR);
        }
        else if (IsNonPlayerCharacterTurnValid())
        {
            titleSS << turnCreaturePtr_->Race().Name();

            if (turnCreaturePtr_->Race().Which() != creature::race::Wolfen)
            {
                titleSS << " " << turnCreaturePtr_->Role().Name();
            }

            //turn box weapon text (or text that indicates that a creature cannot take their turn)
            if (CAN_TAKE_ACTION_STR.empty())
            {
                willRedColorShakeWeaponText_ = false;
                weaponHoldingSS << HOLDING_WEAPON_STR;
            }
            else
            {
                willRedColorShakeWeaponText_ = true;
                weaponHoldingSS << "Cannot take " << creature::sex::HisHerIts(turnCreaturePtr_->Sex(), false, false) << " turn because " << turnCreaturePtr_->CanTakeActionStr(false) << "!";
            }

            infoSS.str(EMPTY_STR);//any short all-whitespace non-empty string will work here

            armorSS << "Armor:  ";

            auto const ARMOR_STR(turnCreaturePtr_->ArmorString());
            if (ARMOR_STR.empty())
            {
                armorSS << "None";
            }
            else
            {
                armorSS << ARMOR_STR;
            }

            if (turnCreaturePtr_->HasCondition(creature::Conditions::Good))
            {
                enemyCondsSS << " ";
            }
            else
            {
                auto const CONDITION_LIST_STR{ turnCreaturePtr_->ConditionNames(6) };
                enemyCondsSS << "Condition";

                if (boost::algorithm::contains(CONDITION_LIST_STR, ","))
                {
                    enemyCondsSS << "s";
                }

                enemyCondsSS << ": " << CONDITION_LIST_STR;
            }
        }

        auto willDrawTurnBoxButtons{ true };
        
        if ((TurnPhase::CenterAndZoomOut == turnPhase_) ||
            (TurnPhase::PostCenterAndZoomOutPause == turnPhase_) ||
            ((TurnPhase::PostCenterAndZoomInPause == turnPhase_) &&
                IsNonPlayerCharacterTurnValid()) ||
            ((TurnPhase::PerformAnim == turnPhase_) &&
                (animPhase_ < AnimPhase::Impact)))
        {
            willDrawTurnBoxButtons = false;

            if ((fightResult_.Count() > 0) ||
                ((TurnPhase::PerformAnim == turnPhase_) &&
                    (AnimPhase::AdvanceOrRetreat == animPhase_)))
            {
                
                infoSS.str(EMPTY_STR);
                weaponHoldingSS.str(EMPTY_STR);
                armorSS.str(EMPTY_STR);
                enemyCondsSS.str(EMPTY_STR);

                isPreambleShowing = true;

                //preamble version of action text
                preambleSS << combat::Text::ActionText(turnCreaturePtr_,
                                                       turnActionInfo_,
                                                       fightResult_,
                                                       false,
                                                       false,
                                                       true);
            }
            else
            {
                preambleSS.str(EMPTY_STR);
            }
        }
        else if ((TurnPhase::PerformReport == turnPhase_) ||
                 (TurnPhase::PostPerformPause == turnPhase_) ||
                 ((TurnPhase::PerformAnim == turnPhase_) &&  (animPhase_ >= AnimPhase::Impact)))
        {
            HandlePlayingMeleeSoundEffects();

            willDrawTurnBoxButtons = false;

            infoSS.str(EMPTY_STR);
            weaponHoldingSS.str(EMPTY_STR);
            armorSS.str(EMPTY_STR);
            enemyCondsSS.str(EMPTY_STR);

            isPreambleShowing = true;

            //perform report version of action text
            preambleSS << combat::Text::ActionTextIndexed(
                turnCreaturePtr_,
                turnActionInfo_,
                fightResult_,
                false,
                performReportEffectIndex_,
                performReportHitIndex_,
                isFightResultCollapsed_);
        }
        else if (TurnPhase::TargetSelect == turnPhase_)
        {
            willDrawTurnBoxButtons = false;

            infoSS.str(EMPTY_STR);
            weaponHoldingSS.str(EMPTY_STR);
            armorSS.str(EMPTY_STR);
            enemyCondsSS.str(EMPTY_STR);

            isPreambleShowing = true;

            if (nullptr == spellBeingCastPtr_)
            {
                preambleSS << "Click to select who to fight...\n\n"
                           << "(Press Escape or X to Cancel)";
            }
            else
            {
                preambleSS << "Click on the ";

                if (spellBeingCastPtr_->Target() == TargetType::SingleOpponent)
                {
                    preambleSS << "enemy creature";
                }
                else if (spellBeingCastPtr_->Target() == TargetType::SingleCompanion)
                {
                    preambleSS << "character";
                }

                preambleSS << " to cast " << spellBeingCastPtr_->Name() << " on...\n\n"
                           << "(Press Escape or X to Cancel)";
            }
        }
        else if ((conditionEffectsIndex_ < conditionEffectsVec_.size()) &&
                 (TurnPhase::ConditionEffectPause == turnPhase_))
        {
            willDrawTurnBoxButtons = false;

            infoSS.str(EMPTY_STR);
            weaponHoldingSS.str(EMPTY_STR);
            armorSS.str(EMPTY_STR);
            enemyCondsSS.str(EMPTY_STR);

            isPreambleShowing = true;

            auto const & HIT_INFO{ conditionEffectsVec_[conditionEffectsIndex_] };

            preambleSS << HIT_INFO.ActionPhrase().Compose(HIT_INFO.ConditionPtr()->Name(),
                turnCreaturePtr_->Name());
        }
        else
        {
            preambleSS.str(EMPTY_STR);
        }

        auto const VERT_POS_SHIFT(sfml_util::MapByRes(0.0f, 16.0f));
        titleTBoxTextRegionUPtr_->SetText(titleSS.str());
        titleTBoxTextRegionUPtr_->SetEntityPos(turnBoxRegion_.left,
                                               (turnBoxRegion_.top + (titleTBoxTextRegionUPtr_->GetEntityRegion().height * 0.5f)) - VERT_POS_SHIFT);

        weaponTBoxTextRegionUPtr_->SetText(weaponHoldingSS.str());
        weaponTBoxTextRegionUPtr_->SetEntityPos((turnBoxRegion_.left + (turnBoxRegion_.width * 0.5f)) - (weaponTBoxTextRegionUPtr_->GetEntityRegion().width * 0.5f),
                                                (titleTBoxTextRegionUPtr_->GetEntityRegion().top + titleTBoxTextRegionUPtr_->GetEntityRegion().height) - VERT_POS_SHIFT);

        auto const WEAPON_TBOXTEXT_REGION{ weaponTBoxTextRegionUPtr_->GetEntityRegion() };
        armorTBoxTextRegionUPtr_->SetText(armorSS.str());
        armorTBoxTextRegionUPtr_->SetEntityPos((turnBoxRegion_.left + (turnBoxRegion_.width * 0.5f)) - (armorTBoxTextRegionUPtr_->GetEntityRegion().width * 0.5f),
                                               (WEAPON_TBOXTEXT_REGION.top + WEAPON_TBOXTEXT_REGION.height) - VERT_POS_SHIFT);

        enemyCondsTBoxRegionUPtr_->SetText(enemyCondsSS.str());
        enemyCondsTBoxRegionUPtr_->SetEntityPos((turnBoxRegion_.left + (turnBoxRegion_.width * 0.5f)) - (enemyCondsTBoxRegionUPtr_->GetEntityRegion().width * 0.5f),
                                                (armorTBoxTextRegionUPtr_->GetEntityRegion().top + armorTBoxTextRegionUPtr_->GetEntityRegion().height) - VERT_POS_SHIFT);

        auto enemyActionPosLeft{ (turnBoxRegion_.left + (turnBoxRegion_.width * 0.5f)) - (enemyActionTBoxRegionUPtr_->GetEntityRegion().width * 0.5f) };
        auto enemyActionPosTop{ (enemyCondsTBoxRegionUPtr_->GetEntityRegion().top + enemyCondsTBoxRegionUPtr_->GetEntityRegion().height) - VERT_POS_SHIFT };

        if (enemyCondsSS.str() == EMPTY_STR)
        {
            enemyActionPosTop = (armorTBoxTextRegionUPtr_->GetEntityRegion().top +
                armorTBoxTextRegionUPtr_->GetEntityRegion().height) - VERT_POS_SHIFT;
        }

        enemyActionTBoxRegionUPtr_->SetText(preambleSS.str());

        if (isPreambleShowing ||
            ((TurnPhase::PerformReport == turnPhase_) ||
             (TurnPhase::PostPerformPause == turnPhase_) ||
             (TurnPhase::PerformAnim == turnPhase_)))
        {
            enemyActionTBoxRegionUPtr_->SetEntityPos(enemyActionPosLeft,
                                                     weaponTBoxTextRegionUPtr_->GetEntityPos().y);
        }
        else
        {
            enemyActionTBoxRegionUPtr_->SetEntityPos(enemyActionPosLeft, enemyActionPosTop);
        }

        infoTBoxTextRegionUPtr_->SetText(infoSS.str());
        infoTBoxTextRegionUPtr_->SetEntityPos(turnBoxRegion_.left, turnBoxRegion_.top);

        if (willDrawTurnBoxButtons)
        {
            SetupTurnBoxButtons(turnCreaturePtr_);
        }
        else
        {
            MoveTurnBoxButtonsOffScreen(true);
        }
    }


    void CombatStage::StartPerformAnim()
    {
        SetTurnPhase(TurnPhase::PerformAnim);

        switch (turnActionPhase_)
        {
            case TurnActionPhase::Advance:
            {
                combatDisplayStagePtr_->MoveCreatureBlockingPosition(turnCreaturePtr_, true);
                PositionSlideStartTasks();

                AppendStatusMessage(combat::Text::ActionText(turnCreaturePtr_,
                                                             turnActionInfo_,
                                                             fightResult_,
                                                             true,
                                                             true), false);
                
                SetAnimPhase(AnimPhase::AdvanceOrRetreat);
                break;
            }

            case TurnActionPhase::Retreat:
            {
                combatDisplayStagePtr_->MoveCreatureBlockingPosition(turnCreaturePtr_, false);
                PositionSlideStartTasks();

                AppendStatusMessage(combat::Text::ActionText(turnCreaturePtr_,
                                                             turnActionInfo_,
                                                             fightResult_,
                                                             true,
                                                             true), false);
                
                SetAnimPhase(AnimPhase::AdvanceOrRetreat);
                break;
            }

            case TurnActionPhase::MeleeWeapon:
            {
                AppendStatusMessage(combat::Text::ActionText(turnCreaturePtr_,
                                                             turnActionInfo_,
                                                             fightResult_,
                                                             true,
                                                             true), false);
                
                SetAnimPhase(AnimPhase::MoveToward);

                auto const CREATURE_EFFECTS_VEC{ fightResult_.Effects() };

                M_ASSERT_OR_LOGANDTHROW_SS((CREATURE_EFFECTS_VEC.size() == 1),
                    "game::stage::CombatStage::StartPerformAnim(turnActionPhase_=MeleeWeapon) "
                    << "found the fightResult.Effects().size=" << CREATURE_EFFECTS_VEC.size()
                    << " instead of 1.");
                
                combatAnimationUPtr_->MeleeMoveTowardAnimStart(turnCreaturePtr_,
                    CREATURE_EFFECTS_VEC[0].GetCreature());

                slider_.Reset(ANIM_MELEE_MOVE_SLIDER_SPEED_);
                break;
            }

            case TurnActionPhase::ShootSling:
            case TurnActionPhase::ShootArrow:
            case TurnActionPhase::ShootBlowpipe:
            {
                //Note:  There should only be one CreatureEffect and one HitInfoVec when
                //attacking with a projectile
                auto const HIT_INFO{ fightResult_.GetHitInfo(0, 0) };
                if (HIT_INFO.IsValid())
                {
                    //at this point we are guaranteed fightResult_ contains at least one
                    //CreatureEffect and one HitInfo
                    auto const WEAPON_PTR{ HIT_INFO.Weapon() };
                    if (WEAPON_PTR != nullptr)
                    {
                        combatSoundEffects_.PlayShoot(WEAPON_PTR);
                        combatAnimationUPtr_->ProjectileShootAnimStart(
                            turnCreaturePtr_,
                            fightResult_.Effects()[0].GetCreature(),
                            WEAPON_PTR,
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
                AppendStatusMessage(combat::Text::ActionText(turnCreaturePtr_,
                                                             turnActionInfo_,
                                                             fightResult_,
                                                             true,
                                                             true), false);

                combatSoundEffects_.PlaySpell(spellBeingCastPtr_);

                //get a vec of CombatNodePtr_t for each creature being cast upon
                creature::CreaturePVec_t creaturesCastUponPVec;
                fightResult_.EffectedCreatures(creaturesCastUponPVec);
                combat::CombatNodePVec_t combatNodesCastUponPVec;
                for (auto const NEXT_CREATURE_PTR : creaturesCastUponPVec)
                {
                    combatNodesCastUponPVec.push_back(
                        combatDisplayStagePtr_->GetCombatNodeForCreature(NEXT_CREATURE_PTR) );
                }

                combatAnimationUPtr_->SpellAnimStart(spellBeingCastPtr_,
                                                     turnCreaturePtr_,
                                                     combatNodesCastUponPVec);

                SetAnimPhase(AnimPhase::Spell);
                //SetupTurnBox();
                break;
            }
            case TurnActionPhase::Roar:
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
            case TurnPhase::NotATurn:                   { return "NotATurn"; }
            case TurnPhase::CenterAndZoomIn:            { return "CenterAndZoomIn"; }
            case TurnPhase::PostCenterAndZoomInPause:   { return "PostZInPause"; }
            case TurnPhase::Determine:                  { return "Determine"; }
            case TurnPhase::TargetSelect:               { return "TargetSelect"; }
            case TurnPhase::ConditionEffectPause:       { return "ConditionEffectPause"; }
            case TurnPhase::CenterAndZoomOut:           { return "CenterAndZoomOut"; }
            case TurnPhase::PostCenterAndZoomOutPause:  { return "PostZOutPause"; }
            case TurnPhase::PerformAnim:                { return "PerformAnim"; }
            case TurnPhase::PerformReport:              { return "PerformReport"; }
            case TurnPhase::PostPerformPause:           { return "PostPerformPause"; }
            case TurnPhase::StatusAnim:                 { return "StatusAnim"; }
            case TurnPhase::DeathAnim:                  { return "DeathAnim"; }
            case TurnPhase::PostDeathAnimSlide:         { return "PostDeathAnimSlide"; }
            case TurnPhase::PostTurnPause:              { return "PostTurnPause"; }
            case TurnPhase::Count:
            default:                                    { return ""; }
        }
    }


    const std::string CombatStage::TurnActionPhaseToString(const TurnActionPhase ENUM)
    {
        switch (ENUM)
        {
            case TurnActionPhase::None:             { return "None"; }
            case TurnActionPhase::PauseAndReport:   { return "PauseAndReport"; }
            case TurnActionPhase::MeleeWeapon:      { return "MeleeWeapon"; }
            case TurnActionPhase::ShootSling:       { return "ShootSling"; }
            case TurnActionPhase::ShootArrow:       { return "ShootArrow"; }
            case TurnActionPhase::ShootBlowpipe:    { return "ShootBlowpipe"; }
            case TurnActionPhase::Advance:          { return "Advance"; }
            case TurnActionPhase::Retreat:          { return "Retreat"; }
            case TurnActionPhase::Cast:             { return "Cast"; }
            case TurnActionPhase::Roar:             { return "Roar"; }
            case TurnActionPhase::Pounce:           { return "Pounce"; }
            case TurnActionPhase::Count:
            default:                                { return ""; }
        }
    }


    const std::string CombatStage::PreTurnPhaseToString(const PreTurnPhase ENUM)
    {
        switch (ENUM)
        {
            case PreTurnPhase::Start:             { return "Start"; }
            case PreTurnPhase::PanToCenter:       { return "PanToCenter"; }
            case PreTurnPhase::PostPanPause:      { return "PostPanPause"; }
            case PreTurnPhase::ZoomOut:           { return "ZOut"; }
            case PreTurnPhase::PostZoomOutPause:  { return "PostZOutPause"; }
            case PreTurnPhase::End:               { return "End"; }
            case PreTurnPhase::Count:
            default:                              { return ""; }
        }
    }


    const std::string CombatStage::AnimPhaseToString(const AnimPhase ENUM)
    {
        switch (ENUM)
        {
            case AnimPhase::NotAnimating:        { return "NotAnimating"; }
            case AnimPhase::AdvanceOrRetreat:    { return "AdvanceOrRetreat"; }
            case AnimPhase::ProjectileShoot:     { return "ProjShoot"; }
            case AnimPhase::MoveToward:          { return "MoveToward"; }
            case AnimPhase::PostMoveTowardPause: { return "PostTowardPause"; }
            case AnimPhase::Impact:              { return "Impact"; }
            case AnimPhase::PostImpactPause:     { return "PostImpactPause"; }
            case AnimPhase::Spell:               { return "Spell"; }
            case AnimPhase::PostSpellPause:      { return "PostSpellPause"; }
            case AnimPhase::MoveBack:            { return "MoveBack"; }
            case AnimPhase::FinalPause:          { return "FinalPause"; }
            case AnimPhase::Count:
            default:                             { return ""; }
        }
    }


    void CombatStage::UpdateTestingText()
    {
        std::ostringstream ss;
        ss << ((PreTurnPhase::End == preTurnPhase_) ? TurnPhaseToString(turnPhase_) : PreTurnPhaseToString(preTurnPhase_))
            << ", " << pauseTitle_ << " " << ((IsPaused()) ? "D" : "A")
            << ", " << TurnActionPhaseToString(turnActionPhase_)
            << ", " << AnimPhaseToString(animPhase_);

        testingTextRegionUPtr_->SetText(ss.str());
    }


    CombatStage::TurnActionPhase CombatStage::GetTurnActionPhaseFromWeaponType(const item::ItemPtr_t WEAPON_PTR) const
    {
        if (WEAPON_PTR->WeaponType() & item::weapon_type::Sling)
        {
            return TurnActionPhase::ShootSling;
        }
        else if ((WEAPON_PTR->WeaponType() & item::weapon_type::Bow) ||
                 (WEAPON_PTR->WeaponType() & item::weapon_type::Crossbow))
        {
            return TurnActionPhase::ShootArrow;
        }
        else if ((WEAPON_PTR->WeaponType() & item::weapon_type::Blowpipe))
        {
            return TurnActionPhase::ShootBlowpipe;
        }
        else
        {
            return TurnActionPhase::MeleeWeapon;
        }
    }


    CombatStage::TurnActionPhase CombatStage::GetTurnActionPhaseFromFightResult(const combat::FightResult & FIGHT_RESULT) const
    {
        //Note: Assume it is not possible for a weapon attack to target multiple creatures
        auto const HIT_INFO{ FIGHT_RESULT.GetHitInfo(0, 0) };
        if (HIT_INFO.IsValid())
        {
            auto const WEAPON_PTR{ HIT_INFO.Weapon() };
            if (WEAPON_PTR != nullptr)
            {
                return GetTurnActionPhaseFromWeaponType(WEAPON_PTR);
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
        auto const & CREATURE_EFFECTS_VEC{ fightResult_.Effects() };
        for (auto const & NEXT_CREATURE_EFFECT : CREATURE_EFFECTS_VEC)
        {
            if (NEXT_CREATURE_EFFECT.WasKill())
            {
                if (NEXT_CREATURE_EFFECT.GetCreature()->IsPlayerCharacter() == false)
                {
                    combat::Encounter::Instance()->HandleKilledCreature(
                        NEXT_CREATURE_EFFECT.GetCreature());
                }
            }
        }
    }


    void CombatStage::HandleApplyDamageTasks()
    {
        //create a vec of all creatures damaged and killed this turn
        std::vector<stats::Health_t> damageVec;
        combat::CombatNodePVec_t combatNodePVec;
        creature::CreaturePVec_t killedCreaturesPVec;
        auto const CREATURE_EFFECTS{ fightResult_.Effects() };
        for (auto const & NEXT_CREATURE_EFFECT : CREATURE_EFFECTS)
        {
            if (NEXT_CREATURE_EFFECT.GetCreature() != nullptr)
            {
                if (NEXT_CREATURE_EFFECT.WasKill())
                {
                    killedCreaturesPVec.push_back(NEXT_CREATURE_EFFECT.GetCreature());
                }

                auto const DAMAGE{ NEXT_CREATURE_EFFECT.GetDamageTotal() };
                if (DAMAGE != 0)
                {
                    damageVec.push_back(DAMAGE);
                    combatNodePVec.push_back(combatDisplayStagePtr_->
                        GetCombatNodeForCreature(NEXT_CREATURE_EFFECT.GetCreature()));
                }
            }
        }

        combatAnimationUPtr_->TextAnimStart(damageVec, combatNodePVec);

        //remove all conditions except for dead from the killed creatures
        for (auto nextKilledCreaturePtr : killedCreaturesPVec)
        {
            auto const CONDITIONS_VEC{ nextKilledCreaturePtr->Conditions() };
            for (auto const NEXT_CONDITION_ENUM : CONDITIONS_VEC)
            {
                if (NEXT_CONDITION_ENUM != creature::Conditions::Dead)
                {
                    nextKilledCreaturePtr->ConditionRemove(NEXT_CONDITION_ENUM);
                }
            }
        }

        combatDisplayStagePtr_->UpdateHealthTasks();
    }


    void CombatStage::HandleAttackTasks(creature::CreaturePtr_t creatureToAttackPtr)
    {
        SetUserActionAllowed(false);

        turnActionInfo_ = combat::TurnActionInfo(combat::TurnAction::Attack,
                                                 { creatureToAttackPtr });

        combat::Encounter::Instance()->SetTurnActionInfo(turnCreaturePtr_, turnActionInfo_);
        fightResult_ = combat::FightClub::Fight(turnCreaturePtr_, creatureToAttackPtr);
        SetTurnActionPhase(GetTurnActionPhaseFromFightResult(fightResult_));

        SetTurnPhase(TurnPhase::CenterAndZoomOut);

        combatAnimationUPtr_->CenteringStart(
            creature::CreaturePVec_t{ turnCreaturePtr_, creatureToAttackPtr });

        slider_.Reset(ANIM_CENTERING_SLIDER_SPEED_);

        SetupTurnBox();
    }


    bool CombatStage::HandleMiscCancelTasks()
    {
        //cancel summary view if visible or even just starting
        if (combatDisplayStagePtr_->GetIsSummaryViewInProgress())
        {
            combatDisplayStagePtr_->CancelSummaryViewAndStartTransitionBack();
            return true;
        }

        //cancel pause if paused
        if (IsPaused())
        {
            isPauseCanceled_ = true;
            return true;
        }

        //this interrupts a status animation in progress...
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

        auto const HIT_INFO_VEC{ CREATURE_EFFECTS_VEC[performReportEffectIndex_].GetHitInfoVec() };
        if (performReportHitIndex_ >= HIT_INFO_VEC.size())
        {
            return;
        }

        auto const HIT_INFO{ HIT_INFO_VEC[performReportHitIndex_] };

        if (HIT_INFO.IsWeapon())
        {
            auto const EFFECT_HIT_PAIR{ std::make_pair(performReportEffectIndex_,
                performReportHitIndex_) };

            if (soundEffectsPlayedSet_.find(EFFECT_HIT_PAIR) == soundEffectsPlayedSet_.end())
            {
                soundEffectsPlayedSet_.insert(EFFECT_HIT_PAIR);
                combatSoundEffects_.PlayHitOrMiss(HIT_INFO);
            }
        }
    }


    bool CombatStage::HandleWin()
    {
        auto areAllEnemiesIncapacitated{ true };
        auto const ALL_LIVING_ENEMIES_PVEC{ creature::Algorithms::NonPlayers(true) };
        for (auto const NEXT_ENEMY_PTR : ALL_LIVING_ENEMIES_PVEC)
        {
            if (NEXT_ENEMY_PTR->HasConditionNotAThreatPerm() == false)
            {
                areAllEnemiesIncapacitated = false;
                break;
            }
        }

        if (areAllEnemiesIncapacitated)
        {
            game::LoopManager::Instance()->PopupWaitBegin(this,
                sfml_util::gui::PopupManager::Instance()->CreateCombatOverPopupInfo(
                    POPUP_NAME_COMBATOVER_WIN_, true));

            EndOfCombatCleanup();
        }

        return hasCombatEnded_;
    }


    bool CombatStage::HandleLose()
    {
        auto areAllCharactersIncapacitated{ true };
        auto const ALL_LIVING_PLAYERS_PVEC{ creature::Algorithms::Players(true) };
        for (auto const NEXT_PLAYER_PTR : ALL_LIVING_PLAYERS_PVEC)
        {
            if (NEXT_PLAYER_PTR->HasConditionNotAThreatPerm() == false)
            {
                areAllCharactersIncapacitated = false;
                break;
            }
        }

        if (areAllCharactersIncapacitated)
        {
            game::LoopManager::Instance()->PopupWaitBegin(this,
                sfml_util::gui::PopupManager::Instance()->CreateCombatOverPopupInfo(
                    POPUP_NAME_COMBATOVER_LOSE_, false));

            EndOfCombatCleanup();
        }

        return hasCombatEnded_;
    }


    void CombatStage::EndOfCombatCleanup()
    {
        hasCombatEnded_ = true;
        combat::Encounter::Instance()->EndTasks();
        turnCreaturePtr_ = nullptr;
        combatAnimationUPtr_->EndOfCombatCleanup();
        combatDisplayStagePtr_->EndOfCombatCleanup();
    }

}
}
