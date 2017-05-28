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
#include "game/creature/name-info.hpp"
#include "game/creature/conditions.hpp"
#include "game/creature/condition-algorithms.hpp"
#include "game/creature/title-warehouse.hpp"
#include "game/creature/algorithms.hpp"
#include "game/item/item.hpp"
#include "game/item/weapon-factory.hpp"
#include "game/item/armor-factory.hpp"
#include "game/item/algorithms.hpp"
#include "game/spell/spell-base.hpp"
#include "game/phase-enum.hpp"

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

    const std::string CombatStage::POPUP_NAME_SPELLBOOK_    { "SPELLBOOK_POPUP_WINDOW_NAME" };
    //
    const float CombatStage::PAUSE_LONG_SEC_                { 6.0f };
    const float CombatStage::PAUSE_MEDIUM_SEC_              { 3.0f };
    const float CombatStage::PAUSE_SHORT_SEC_               { 1.5f };
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
    const float CombatStage::CONDITION_WAKE_PAUSE_SEC_      { PAUSE_MEDIUM_SEC_ };
    //
    const float CombatStage::SLIDER_SPEED_SLOWEST_                  { 1.0f };
    const float CombatStage::SLIDER_SPEED_SLOW_                     { SLIDER_SPEED_SLOWEST_ * 2.0f };
    const float CombatStage::SLIDER_SPEED_NORMAL_                   { SLIDER_SPEED_SLOW_ * 2.0f };
    const float CombatStage::SLIDER_SPEED_FAST_                     { SLIDER_SPEED_NORMAL_ * 2.0f };
    const float CombatStage::SLIDER_SPEED_FASTEST_                  { SLIDER_SPEED_NORMAL_ * 10.0f };
    const float CombatStage::ZOOM_SLIDER_SPEED_                     { SLIDER_SPEED_SLOW_ };
    const float CombatStage::TEXT_COLOR_SHAKER_SPEED_               { SLIDER_SPEED_FASTEST_ };
    const float CombatStage::ANIM_CENTERING_SLIDER_SPEED_           { SLIDER_SPEED_NORMAL_ };
    const float CombatStage::ANIM_INITIAL_CENTERING_SLIDER_SPEED_   { SLIDER_SPEED_SLOWEST_ };
    const float CombatStage::ANIM_CREATURE_POS_SLIDER_SPEED_        { SLIDER_SPEED_NORMAL_ };
    const float CombatStage::ANIM_PROJECTILE_SHOOT_SLIDER_SPEED_    { SLIDER_SPEED_NORMAL_ };
    const float CombatStage::ANIM_DEATH_SLIDER_SPEED_               { SLIDER_SPEED_SLOWEST_ };
    const float CombatStage::ANIM_MELEE_MOVE_SLIDER_SPEED_          { SLIDER_SPEED_FAST_ };
    const float CombatStage::ANIM_IMPACT_SLIDER_SPEED_              { SLIDER_SPEED_NORMAL_ };
    const float CombatStage::ANIM_CREATURE_SHAKE_SLIDER_SPEED_      { SLIDER_SPEED_FASTEST_ };
    const float CombatStage::ANIM_IMPACT_SHAKE_SLIDER_SPEED_        { SLIDER_SPEED_FASTEST_ * 3.0f };
    //
    const sf::Color CombatStage::LISTBOX_BACKGROUND_COLOR_   { (sfml_util::FontManager::Color_Orange() - sf::Color(100, 100, 100, 235)) };
    const sf::Color CombatStage::LISTBOX_HIGHLIGHT_COLOR_    { (sfml_util::FontManager::Color_Orange() - sf::Color(100, 100, 100, 235)) + sf::Color(20, 20, 20, 20) };
    const sf::Color CombatStage::LISTBOX_HIGHLIGHT_ALT_COLOR_{ (sfml_util::FontManager::Color_Orange() - sf::Color(100, 100, 100, 235)) + sf::Color(40, 40, 40, 40) };
    const sf::Color CombatStage::LISTBOX_SELECTED_COLOR_     { sf::Color::White };
    const sf::Color CombatStage::LISTBOX_NOTSELECTED_COLOR_  { sf::Color(150, 150, 150) };
    const sf::Color CombatStage::LISTBOX_LINE_COLOR_         { sf::Color(255, 255, 255, 25) };
    //
    combat::RestoreInfo CombatStage::restoreInfo_;


    CombatStage::CombatStage()
    :
        Stage                      ("Combat"),
        SCREEN_WIDTH_              (sfml_util::Display::Instance()->GetWinWidth()),
        SCREEN_HEIGHT_             (sfml_util::Display::Instance()->GetWinHeight()),
        commandBoxSPtr_            (),
        statusBoxSPtr_             (),
        statusBoxTextInfo_         (" ", sfml_util::FontManager::Instance()->Font_Typical(), sfml_util::FontManager::Instance()->Size_Small(), sfml_util::FontManager::Color_Orange(), sfml_util::Justified::Left),
        zoomSliderBarSPtr_         (),
        turnBoxSPtr_               (),
        turnBoxRegion_             (),
        combatSoundEffects_        (),
        turnPhase_                 (TurnPhase::NotATurn),
        preTurnPhase_              (PreTurnPhase::Start),
        turnActionPhase_           (TurnActionPhase::None),
        animPhase_                 (AnimPhase::NotAnimating),
        spellBeingCastPtr_         (nullptr),
        performReportEffectIndex_  (0),
        performReportHitIndex_     (0),
        zoomSliderOrigPos_         (0.0f),
        willClrShkInitStatusMsg_   (false),
        isMouseHeldDown_           (false),
        isMouseHeldDownAndMoving_  (false),
        tempConditionsWakeStr_     (""),
        slider_                    (1.0f),//initiall speed ignored because speed is set before each use, any value greater than zero will work here
        combatDisplayStagePtr_     (new combat::CombatDisplay()),
        combatAnimationPtr_        (combat::CombatAnimation::Instance()),
        settingsButtonSPtr_        (new sfml_util::gui::FourStateButton("CombatStage'sSettingsGears",
                                                                        0.0f,
                                                                        0.0f,
                                                                        std::string(GameDataFile::Instance()->GetMediaPath("media-images-buttons-gui")).append("gears1_normal.png"),
                                                                        "",
                                                                        std::string(GameDataFile::Instance()->GetMediaPath("media-images-buttons-gui")).append("gears1_lit.png"),
                                                                        "",
                                                                        sfml_util::gui::MouseTextInfo(),
                                                                        sfml_util::gui::TextInfo(),
                                                                        false,
                                                                        sfml_util::MapByRes(0.6f, 2.0f),
                                                                        false)),
        pauseDurationSec_           (0.0f),
        pauseElapsedSec_            (pauseDurationSec_ + 1.0f),//anything greater than pauseTimeDurationSecs_ will work here
        isPauseCanceled_            (false),
        turnCreaturePtr_            (nullptr),
        goldTextColorShaker_        (sfml_util::FontManager::Color_Orange(), sf::Color::White, TEXT_COLOR_SHAKER_SPEED_),
        redTextColorShaker_         (sf::Color(255, 127, 127), sf::Color::White, TEXT_COLOR_SHAKER_SPEED_ * 0.65f),
        turnActionInfo_             (),
        fightResult_                (),
        willRedColorShakeWeaponText_(false),
        titleTBoxTextRegionSPtr_    (),
        weaponTBoxTextRegionSPtr_   (),
        armorTBoxTextRegionSPtr_    (),
        infoTBoxTextRegionSPtr_     (),
        enemyActionTBoxRegionSPtr_  (),
        enemyCondsTBoxRegionSPtr_   (),
        zoomLabelTextRegionSPtr_    (),
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
        statusMsgAnimTimerSec_      (STATUSMSG_ANIM_PAUSE_SEC_ + 1.0f), //anything greater than STATUSMSG_ANIM_PAUSE_SEC_ will work here
        statusMsgAnimColorShaker_   (LISTBOX_HIGHLIGHT_COLOR_, LISTBOX_HIGHLIGHT_ALT_COLOR_, 35.0f, false),
        testingTextRegionSPtr_      (),
        pauseTitle_                 (""),
        sparksAnimUPtr_             (nullptr)
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


    bool CombatStage::HandleCallback(const sfml_util::gui::callback::FourStateButtonCallbackPackage_t & PACKAGE)
    {
        if ((IsPlayerCharacterTurnValid() == false) || (TurnPhase::Determine != turnPhase_))
            return false;

        if (PACKAGE.PTR_ == attackTBoxButtonSPtr_.get())
            return HandleAttack();

        if (PACKAGE.PTR_ == fightTBoxButtonSPtr_.get())
            return HandleFight();

        if (PACKAGE.PTR_ == castTBoxButtonSPtr_.get())
            return HandleCast_Step1_ValidateCastAndSelectSpell();

        if (PACKAGE.PTR_ == advanceTBoxButtonSPtr_.get())
            return HandleAdvance();

        if (PACKAGE.PTR_ == retreatTBoxButtonSPtr_.get())
            return HandleRetreat();

        if (PACKAGE.PTR_ == blockTBoxButtonSPtr_.get())
            return HandleBlock();

        if (PACKAGE.PTR_ == skipTBoxButtonSPtr_.get())
            return HandleSkip();

        if (PACKAGE.PTR_ == flyTBoxButtonSPtr_.get())
            return HandleFly();

        if (PACKAGE.PTR_ == landTBoxButtonSPtr_.get())
            return HandleLand();

        if (PACKAGE.PTR_ == roarTBoxButtonSPtr_.get())
            return HandleRoar();

        if (PACKAGE.PTR_ == pounceTBoxButtonSPtr_.get())
        {
            auto isSkyPounce(false);
            if ((TurnPhase::NotATurn != turnPhase_) && (turnCreaturePtr_ != nullptr))
                isSkyPounce = combat::Encounter::Instance()->GetTurnInfoCopy(turnCreaturePtr_).GetIsFlying();

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


    bool CombatStage::HandleCallback(const sfml_util::gui::callback::SliderBarCallbackPackage_t & PACKAGE)
    {
        if ((PACKAGE.PTR_ == zoomSliderBarSPtr_.get()) && (combatDisplayStagePtr_ != nullptr))
        {
            //only zoom out half the distance that the slider actually shows
            auto const HALF_ZOOM_DIFFERENCE{ 1.0f - ((1.0f - PACKAGE.PTR_->GetCurrentValue()) * 0.5f) };
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
                M_ASSERT_OR_LOGANDTHROW_SS((POPUP_RESPONSE.Selection() < SPELLS_PVEC.size()), "game::stage::CombatStage::HandleCallback(POPUP_RESPONSE, selection=" << POPUP_RESPONSE.Selection() << ")  Selection was greater than SpellPVec.size=" << SPELLS_PVEC.size());

                auto const spellPtr{ SPELLS_PVEC.at(POPUP_RESPONSE.Selection()) };
                M_ASSERT_OR_LOGANDTHROW_SS((spellPtr != nullptr ), "game::stage::CombatStage::HandleCallback(POPUP_RESPONSE, selection=" << POPUP_RESPONSE.Selection() << ")  SPELLS_PVEC[selection] was null.");

                if ((spellPtr->ValidPhases() & Phase::Combat) == 0)
                {
                    QuickSmallPopup("The " + spellPtr->Name() + " spell cannot be cast during combat.", true, false);
                    return false;
                }

                if (spellPtr->ManaCost() > turnCreaturePtr_->ManaCurrent())
                {
                    std::ostringstream ss;
                    ss << turnCreaturePtr_->Name() << " does not have enough mana to cast " << spellPtr->Name() << ".  " << (spellPtr->ManaCost() - turnCreaturePtr_->ManaCurrent()) << " short.";
                    QuickSmallPopup(ss.str(), true, false);
                    return false;
                }

                restoreInfo_.LastCastSpellNum(turnCreaturePtr_, POPUP_RESPONSE.Selection());
                HandleCast_Step2_SelectTargetOrPerformOnAll(spellPtr);
                return true;
            }
            else
            {
                return false;
            }
        }

        return false;
    }


    void CombatStage::Setup()
    {
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
        statusBoxSPtr_.reset( new sfml_util::gui::ListBox("ComabtStage'sStatus",
                                                          STATUS_REGION,
                                                          sfml_util::gui::ListBoxItemSLst_t(),
                                                          this,
                                                          15.0f,
                                                          0.0f,
                                                          STATUS_BOX_INFO,
                                                          LISTBOX_LINE_COLOR_,
                                                          sfml_util::gui::ListBox::NO_LIMIT_,
                                                          this) );

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
        commandBoxSPtr_.reset( new sfml_util::gui::box::Box("CombatStage'sCommand", COMMAND_REGION_BOXINFO) );

        //turn box
        turnBoxRegion_ = sf::FloatRect(STATUS_REGION_LEFT,
                                       STATUS_REGION_TOP,
                                       STATUS_REGION_WIDTH,
                                       STATUS_REGION_HEIGHT);

        const sfml_util::gui::BackgroundInfo TURNBOX_BACKGROUNDINFO(GameDataFile::Instance()->GetMediaPath("media-images-backgrounds-tile-darkknot"), turnBoxRegion_);
        const sfml_util::gui::box::Info TURNBOX_REGION_BOXINFO(true, turnBoxRegion_, sfml_util::gui::ColorSet(), TURNBOX_BACKGROUNDINFO);
        turnBoxSPtr_.reset(new sfml_util::gui::box::Box("CombatStage'sTurnBox", TURNBOX_REGION_BOXINFO));

        //turnbox title text region
        const sfml_util::gui::TextInfo TURNBOXTITLE_TEXT_INFO(" ",
                                                              sfml_util::FontManager::Instance()->Font_BigFlavor1(),
                                                              sfml_util::FontManager::Instance()->Size_Largeish(),
                                                              sfml_util::FontManager::Color_Light(),
                                                              sfml_util::Justified::Center);

        sf::FloatRect turnBoxTitleTextRegion(turnBoxRegion_);
        turnBoxTitleTextRegion.height = 0.0f;
        titleTBoxTextRegionSPtr_.reset( new sfml_util::gui::TextRegion("TurnBox'sTitle", TURNBOXTITLE_TEXT_INFO, turnBoxTitleTextRegion) );

        const sfml_util::gui::TextInfo TURNBOXINFO_TEXT_INFO(" ",
                                                             sfml_util::FontManager::Instance()->Font_Default2(),
                                                             sfml_util::FontManager::Instance()->Size_Smallish(),
                                                             sfml_util::FontManager::Color_GrayLight(),
                                                             sfml_util::Justified::Left);

        sf::FloatRect turnBoxInfoTextRegion(turnBoxRegion_);
        infoTBoxTextRegionSPtr_.reset( new sfml_util::gui::TextRegion("CombatStage'sTurnInfo", TURNBOXINFO_TEXT_INFO, turnBoxInfoTextRegion) );

        const sfml_util::gui::TextInfo TURNBOXENEMYACTION_TEXT_INFO(" ",
                                                                    sfml_util::FontManager::Instance()->Font_Default2(),
                                                                    sfml_util::FontManager::Instance()->Size_Normal(),
                                                                    sfml_util::FontManager::Color_Light(),
                                                                    sfml_util::Justified::Center);

        sf::FloatRect turnBoxEnemyActionTextRegion(turnBoxRegion_);

        //the enemy turn action text can run long and thus needs to be constrained further
        auto const PAD{ 25.0f };
        turnBoxEnemyActionTextRegion.left += PAD;
        turnBoxEnemyActionTextRegion.width -= PAD * 2.0f;

        enemyActionTBoxRegionSPtr_.reset( new sfml_util::gui::TextRegion("CombatStage'sTurnEnemyAction", TURNBOXENEMYACTION_TEXT_INFO, turnBoxEnemyActionTextRegion) );

        const sfml_util::gui::TextInfo TURNBOXENEMYCONDS_TEXT_INFO(" ",
                                                                   sfml_util::FontManager::Instance()->Font_Default2(),
                                                                   sfml_util::FontManager::Instance()->Size_Smallish(),
                                                                   sfml_util::FontManager::Color_GrayLight(),
                                                                   sfml_util::Justified::Center);

        sf::FloatRect turnBoxEnemyCondsTextRegion(turnBoxRegion_);
        enemyCondsTBoxRegionSPtr_.reset( new sfml_util::gui::TextRegion("CombatStage'sTurnEnemyConds", TURNBOXENEMYCONDS_TEXT_INFO, turnBoxEnemyCondsTextRegion) );

        sfml_util::gui::TextInfo turnBoxWeaponTextInfo(" ",
                                                       sfml_util::FontManager::Instance()->Font_Default2(),
                                                       sfml_util::FontManager::Instance()->Size_Smallish(),
                                                       sfml_util::FontManager::Color_GrayLight(),
                                                       sfml_util::Justified::Center);

        weaponTBoxTextRegionSPtr_.reset( new sfml_util::gui::TextRegion("TurnBox'sWeapon", turnBoxWeaponTextInfo, turnBoxTitleTextRegion) );

        sfml_util::gui::TextInfo turnBoxArmorTextInfo(turnBoxWeaponTextInfo);

        armorTBoxTextRegionSPtr_.reset(new sfml_util::gui::TextRegion("TurnBox'sArmor", turnBoxWeaponTextInfo, turnBoxTitleTextRegion));

        sfml_util::gui::TextInfo turnButtonTextInfo(" ",
                                                    sfml_util::FontManager::Instance()->Font_Default2(),
                                                    sfml_util::FontManager::Instance()->Size_Largeish(),
                                                    sfml_util::FontManager::Color_Orange(),
                                                    sfml_util::Justified::Left);

        const sfml_util::gui::TextInfo TESTING_TEXT_INFO(" ",
                                                         sfml_util::FontManager::Instance()->Font_Default2(),
                                                         sfml_util::FontManager::Instance()->Size_Smallish(),
                                                         sfml_util::FontManager::Color_GrayLight(),
                                                         sfml_util::Justified::Left);

        sf::FloatRect testingTextRegion(turnBoxRegion_);
        testingTextRegion.left += testingTextRegion.width - 50.0f;
        testingTextRegion.top += (testingTextRegion.height - 400.0f);
        testingTextRegionSPtr_.reset( new sfml_util::gui::TextRegion("CombatStage'sTesting", TESTING_TEXT_INFO, testingTextRegion) );

        const sf::Color TURNBUTTON_DISABLED_COLOR(sfml_util::FontManager::Color_Orange() - sf::Color(0, 0, 0, 176));
        sfml_util::gui::TextInfo turnButtonTextInfoDisabled(turnButtonTextInfo);
        turnButtonTextInfoDisabled.color = TURNBUTTON_DISABLED_COLOR;

        turnButtonTextInfo.text = "(A)ttack";
        turnButtonTextInfoDisabled.text = "(A)ttack";
        const sfml_util::gui::MouseTextInfo ATTACKBUTTON_MOUSETEXTINFO(turnButtonTextInfo, sfml_util::FontManager::Color_Light(), sf::Color::White);
        attackTBoxButtonSPtr_.reset( new sfml_util::gui::FourStateButton("CombatStage'sAttack", 0.0f, 0.0f, "", "", "", "", ATTACKBUTTON_MOUSETEXTINFO, turnButtonTextInfoDisabled) );
        attackTBoxButtonSPtr_->SetCallbackHandler(this);
        attackTBoxButtonSPtr_->SetMouseHoverText(combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_ATTACK_);
        attackTBoxButtonSPtr_->MoveEntityOffScreen();
        EntityAdd(attackTBoxButtonSPtr_.get());

        turnButtonTextInfo.text = "(F)ight";
        turnButtonTextInfoDisabled.text = "(F)ight";
        const sfml_util::gui::MouseTextInfo FIGHTBUTTON_MOUSETEXTINFO(turnButtonTextInfo, sfml_util::FontManager::Color_Light(), sf::Color::White);
        fightTBoxButtonSPtr_.reset( new sfml_util::gui::FourStateButton("CombatStage'sAttack", 0.0f, 0.0f, "", "", "", "", FIGHTBUTTON_MOUSETEXTINFO, turnButtonTextInfoDisabled) );
        fightTBoxButtonSPtr_->SetCallbackHandler(this);
        fightTBoxButtonSPtr_->SetMouseHoverText(combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_FIGHT_);
        fightTBoxButtonSPtr_->MoveEntityOffScreen();
        EntityAdd(fightTBoxButtonSPtr_.get());

        turnButtonTextInfo.text = "(C)ast";
        turnButtonTextInfoDisabled.text = "(C)ast";
        const sfml_util::gui::MouseTextInfo CASTBUTTON_MOUSETEXTINFO(turnButtonTextInfo, sfml_util::FontManager::Color_Light(), sf::Color::White);
        castTBoxButtonSPtr_.reset( new sfml_util::gui::FourStateButton("CombatStage'sCast", 0.0f, 0.0f, "", "", "", "", CASTBUTTON_MOUSETEXTINFO, turnButtonTextInfoDisabled) );
        castTBoxButtonSPtr_->SetCallbackHandler(this);
        castTBoxButtonSPtr_->SetMouseHoverText(combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_CAST_);
        castTBoxButtonSPtr_->MoveEntityOffScreen();
        EntityAdd(castTBoxButtonSPtr_.get());

        turnButtonTextInfo.text = "Advance";
        turnButtonTextInfoDisabled.text = "Advance";
        const sfml_util::gui::MouseTextInfo ADVANCEBUTTON_MOUSETEXTINFO(turnButtonTextInfo, sfml_util::FontManager::Color_Light(), sf::Color::White);
        advanceTBoxButtonSPtr_.reset( new sfml_util::gui::FourStateButton("CombatStage'sAdvance", 0.0f, 0.0f, "", "", "", "", ADVANCEBUTTON_MOUSETEXTINFO, turnButtonTextInfoDisabled) );
        advanceTBoxButtonSPtr_->SetCallbackHandler(this);
        advanceTBoxButtonSPtr_->SetMouseHoverText(combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_ADVANCE_);
        advanceTBoxButtonSPtr_->MoveEntityOffScreen();
        EntityAdd(advanceTBoxButtonSPtr_.get());

        turnButtonTextInfo.text = "Retreat";
        turnButtonTextInfoDisabled.text = "Retreat";
        const sfml_util::gui::MouseTextInfo RETREATBUTTON_MOUSETEXTINFO(turnButtonTextInfo, sfml_util::FontManager::Color_Light(), sf::Color::White);
        retreatTBoxButtonSPtr_.reset( new sfml_util::gui::FourStateButton("CombatStage'sRetreat", 0.0f, 0.0f, "", "", "", "", RETREATBUTTON_MOUSETEXTINFO, turnButtonTextInfoDisabled) );
        retreatTBoxButtonSPtr_->SetCallbackHandler(this);
        retreatTBoxButtonSPtr_->SetMouseHoverText(combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_RETREAT_);
        retreatTBoxButtonSPtr_->MoveEntityOffScreen();
        EntityAdd(retreatTBoxButtonSPtr_.get());

        turnButtonTextInfo.text = "(B)lock";
        turnButtonTextInfoDisabled.text = "(B)lock";
        const sfml_util::gui::MouseTextInfo BLOCKBUTTON_MOUSETEXTINFO(turnButtonTextInfo, sfml_util::FontManager::Color_Light(), sf::Color::White);
        blockTBoxButtonSPtr_.reset( new sfml_util::gui::FourStateButton("CombatStage'sBlock", 0.0f, 0.0f, "", "", "", "", BLOCKBUTTON_MOUSETEXTINFO, turnButtonTextInfoDisabled) );
        blockTBoxButtonSPtr_->SetCallbackHandler(this);
        blockTBoxButtonSPtr_->SetMouseHoverText(combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_BLOCK_);
        blockTBoxButtonSPtr_->MoveEntityOffScreen();
        EntityAdd(blockTBoxButtonSPtr_.get());

        turnButtonTextInfo.text = "(S)kip";
        turnButtonTextInfoDisabled.text = "(S)kip";
        const sfml_util::gui::MouseTextInfo SKIPBUTTON_MOUSETEXTINFO(turnButtonTextInfo, sfml_util::FontManager::Color_Light(), sf::Color::White);
        skipTBoxButtonSPtr_.reset(new sfml_util::gui::FourStateButton("CombatStage'sSkip", 0.0f, 0.0f, "", "", "", "", SKIPBUTTON_MOUSETEXTINFO, turnButtonTextInfoDisabled));
        skipTBoxButtonSPtr_->SetCallbackHandler(this);
        skipTBoxButtonSPtr_->SetMouseHoverText(combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_SKIP_);
        skipTBoxButtonSPtr_->MoveEntityOffScreen();
        EntityAdd(skipTBoxButtonSPtr_.get());

        turnButtonTextInfo.text = "Fl(y)";
        turnButtonTextInfoDisabled.text = "Fl(y)";
        const sfml_util::gui::MouseTextInfo FLYBUTTON_MOUSETEXTINFO(turnButtonTextInfo, sfml_util::FontManager::Color_Light(), sf::Color::White);
        flyTBoxButtonSPtr_.reset(new sfml_util::gui::FourStateButton("CombatStage'sFly", 0.0f, 0.0f, "", "", "", "", FLYBUTTON_MOUSETEXTINFO, turnButtonTextInfoDisabled));
        flyTBoxButtonSPtr_->SetCallbackHandler(this);
        flyTBoxButtonSPtr_->SetMouseHoverText(combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_FLY_);
        flyTBoxButtonSPtr_->MoveEntityOffScreen();
        EntityAdd(flyTBoxButtonSPtr_.get());

        turnButtonTextInfo.text = "(L)and";
        turnButtonTextInfoDisabled.text = "(L)and";
        const sfml_util::gui::MouseTextInfo LANDBUTTON_MOUSETEXTINFO(turnButtonTextInfo, sfml_util::FontManager::Color_Light(), sf::Color::White);
        landTBoxButtonSPtr_.reset(new sfml_util::gui::FourStateButton("CombatStage'sLand", 0.0f, 0.0f, "", "", "", "", LANDBUTTON_MOUSETEXTINFO, turnButtonTextInfoDisabled));
        landTBoxButtonSPtr_->SetCallbackHandler(this);
        landTBoxButtonSPtr_->SetMouseHoverText(combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_LAND_);
        landTBoxButtonSPtr_->MoveEntityOffScreen();
        EntityAdd(landTBoxButtonSPtr_.get());

        turnButtonTextInfo.text = "(R)oar";
        turnButtonTextInfoDisabled.text = "(R)oar";
        const sfml_util::gui::MouseTextInfo ROARBUTTON_MOUSETEXTINFO(turnButtonTextInfo, sfml_util::FontManager::Color_Light(), sf::Color::White);
        roarTBoxButtonSPtr_.reset(new sfml_util::gui::FourStateButton("CombatStage'sRoar", 0.0f, 0.0f, "", "", "", "", ROARBUTTON_MOUSETEXTINFO, turnButtonTextInfoDisabled));
        roarTBoxButtonSPtr_->SetCallbackHandler(this);
        roarTBoxButtonSPtr_->SetMouseHoverText(combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_ROAR_);
        roarTBoxButtonSPtr_->MoveEntityOffScreen();
        EntityAdd(roarTBoxButtonSPtr_.get());

        turnButtonTextInfo.text = "(P)ounce";
        turnButtonTextInfoDisabled.text = "(P)ounce";
        const sfml_util::gui::MouseTextInfo POUNCEBUTTON_MOUSETEXTINFO(turnButtonTextInfo, sfml_util::FontManager::Color_Light(), sf::Color::White);
        pounceTBoxButtonSPtr_.reset(new sfml_util::gui::FourStateButton("CombatStage'sPounce", 0.0f, 0.0f, "", "", "", "", POUNCEBUTTON_MOUSETEXTINFO, turnButtonTextInfoDisabled));
        pounceTBoxButtonSPtr_->SetCallbackHandler(this);
        pounceTBoxButtonSPtr_->SetMouseHoverText(combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_POUNCE_);
        pounceTBoxButtonSPtr_->MoveEntityOffScreen();
        EntityAdd(pounceTBoxButtonSPtr_.get());

        //settings button (gears symbol)
        const float COMMAND_REGION_PAD(10.0f);
        settingsButtonSPtr_->SetEntityPos(COMMAND_REGION_LEFT + COMMAND_REGION_WIDTH - settingsButtonSPtr_->GetEntityRegion().width - COMMAND_REGION_PAD, COMMAND_REGION_TOP + COMMAND_REGION_PAD);
        settingsButtonSPtr_->SetCallbackHandler(this);
        EntityAdd(settingsButtonSPtr_.get());

        //TODO TEMP REMOVE
        //fake player characters until loading games starts working
        std::string errMsgIgnored{ "" };
        player::PartySPtr_t partySPtr(new player::Party());

        {
            const stats::StatSet KNIGHT_STATS(20 + misc::random::Int(10),
                                              15 + misc::random::Int(6) + 100,//TEMP TODO REMOVE Testing combat damage and combat over scenarios
                                              0  + misc::random::Int(6),
                                              5  + misc::random::Int(10),
                                              15 + misc::random::Int(10),
                                              0  + misc::random::Int(8));

            const std::string KNIGHT_NAME( boost::algorithm::replace_last_copy(creature::NameInfo::Instance()->LargestName(), creature::NameInfo::Instance()->LargestLetterString(), "K") );
            auto knightSPtr( std::make_shared<player::Character>(KNIGHT_NAME,
                                                                 creature::sex::Male,
                                                                 creature::BodyType::Make_Humanoid(),
                                                                 creature::Race(creature::race::Human),
                                                                 creature::Role(creature::role::Knight),
                                                                 KNIGHT_STATS) );

            player::Initial::Setup(knightSPtr.get());
            partySPtr->Add(knightSPtr, errMsgIgnored);
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
            partySPtr->Add(firebrandSPtr, errMsgIgnored);
        }
        */
        {
            const stats::StatSet ARCHER_STATS(15 + misc::random::Int(10),
                                              20 + misc::random::Int(10) + 100,//TEMP TODO REMOVE Testing combat damage and combat over scenarios
                                              5  + misc::random::Int(6),
                                              10 + misc::random::Int(10),
                                              10 + misc::random::Int(8),
                                              5  + misc::random::Int(6));

            const std::string ARCHER_NAME(boost::algorithm::replace_last_copy(creature::NameInfo::Instance()->LargestName(), creature::NameInfo::Instance()->LargestLetterString(), "A"));
            player::CharacterSPtr_t archerSPtr(new player::Character(ARCHER_NAME,
                                                                     creature::sex::Female,
                                                                     creature::BodyType::Make_Humanoid(),
                                                                     creature::Race(creature::race::Human),
                                                                     creature::Role(creature::role::Archer),
                                                                     ARCHER_STATS));

            player::Initial::Setup(archerSPtr.get());
            partySPtr->Add(archerSPtr, errMsgIgnored);
        }
        /*
        {
            const stats::StatSet WOLFEN_STATS(20 + misc::random::Int(10),
                                              20 + misc::random::Int(10),
                                              5  + misc::random::Int(6),
                                              5  + misc::random::Int(10),
                                              5  + misc::random::Int(8),
                                              5  + misc::random::Int(6));

            const std::string WOLFEN_NAME(boost::algorithm::replace_last_copy(creature::NameInfo::Instance()->LargestName(), creature::NameInfo::Instance()->LargestLetterString(), "W"));
            player::CharacterSPtr_t wolfenSPtr(new player::Character(WOLFEN_NAME,
                                                                     creature::sex::Female,
                                                                     creature::BodyType::Make_Wolfen(),
                                                                     creature::Race(creature::race::Wolfen),
                                                                     creature::Role(creature::role::Wolfen),
                                                                     WOLFEN_STATS));

            player::Initial::Setup(wolfenSPtr.get());
            partySPtr->Add(wolfenSPtr, errMsgIgnored);
        }
        */
        {
            const stats::StatSet BARD_STATS(10 + misc::random::Int(6),
                                            10 + misc::random::Int(6) + 100,//TEMP TODO REMOVE Testing combat damage and combat over scenarios
                                            10 + misc::random::Int(6),
                                            10 + misc::random::Int(6),
                                            10 + misc::random::Int(6),
                                            10 + misc::random::Int(6));

            const std::string BARD_NAME(boost::algorithm::replace_last_copy(creature::NameInfo::Instance()->LargestName(), creature::NameInfo::Instance()->LargestLetterString(), "B"));
            player::CharacterSPtr_t bardSPtr(new player::Character(BARD_NAME,
                                                                   creature::sex::Male,
                                                                   creature::BodyType::Make_Humanoid(),
                                                                   creature::Race(creature::race::Human),
                                                                   creature::Role(creature::role::Bard),
                                                                   BARD_STATS));

            player::Initial::Setup(bardSPtr.get());
            partySPtr->Add(bardSPtr, errMsgIgnored);
        }
        /*
        {
            const stats::StatSet BEASTMASTER_STATS(10 + misc::random::Int(6),
                                                   10 + misc::random::Int(6),
                                                   10 + misc::random::Int(6),
                                                   10 + misc::random::Int(6),
                                                   10 + misc::random::Int(6),
                                                   10 + misc::random::Int(6));

            const std::string BEASTMASTER_NAME(boost::algorithm::replace_last_copy(creature::NameInfo::Instance()->LargestName(), creature::NameInfo::Instance()->LargestLetterString(), "G"));
            player::CharacterSPtr_t bmSPtr(new player::Character(BEASTMASTER_NAME,
                                                                 creature::sex::Male,
                                                                 creature::BodyType::Make_Humanoid(),
                                                                 creature::Race(creature::race::Human),
                                                                 creature::Role(creature::role::Beastmaster),
                                                                 BEASTMASTER_STATS));

            player::Initial::Setup(bmSPtr.get());
            partySPtr->Add(bmSPtr, errMsgIgnored);
        }
        */
        {
            const stats::StatSet THEIF_STATS(5  + misc::random::Int(10),
                                             5  + misc::random::Int(10) + 100,//TEMP TODO REMOVE Testing combat damage and combat over scenarios
                                             5  + misc::random::Int(10),
                                             15 + misc::random::Int(15),
                                             15 + misc::random::Int(10),
                                             5  + misc::random::Int(8));

            const std::string THEIF_NAME(boost::algorithm::replace_last_copy(creature::NameInfo::Instance()->LargestName(), creature::NameInfo::Instance()->LargestLetterString(), "T"));
            player::CharacterSPtr_t thiefSPtr(new player::Character(THEIF_NAME,
                                                                    creature::sex::Male,
                                                                    creature::BodyType::Make_Humanoid(),
                                                                    creature::Race(creature::race::Gnome),
                                                                    creature::Role(creature::role::Thief),
                                                                    THEIF_STATS));

            player::Initial::Setup(thiefSPtr.get());
            partySPtr->Add(thiefSPtr, errMsgIgnored);
        }

        {
            const stats::StatSet CLERIC_STATS(5  + misc::random::Int(8),
                                              5  + misc::random::Int(8) + 100,//TEMP TODO REMOVE Testing combat damage and combat over scenarios
                                              15 + misc::random::Int(10),
                                              10 + misc::random::Int(8),
                                              25 + misc::random::Int(8),
                                              10 + misc::random::Int(15));

            const std::string CLERIC_NAME(boost::algorithm::replace_last_copy(creature::NameInfo::Instance()->LargestName(), creature::NameInfo::Instance()->LargestLetterString(), "C"));
            player::CharacterSPtr_t clericSPtr(new player::Character(CLERIC_NAME,
                                                                     creature::sex::Female,
                                                                     creature::BodyType::Make_Pixie(),
                                                                     creature::Race(creature::race::Pixie),
                                                                     creature::Role(creature::role::Cleric),
                                                                     CLERIC_STATS));

            player::Initial::Setup(clericSPtr.get());
            clericSPtr->ManaCurrentSet(1);
            partySPtr->Add(clericSPtr, errMsgIgnored);
        }

        {
            const stats::StatSet SORCERER_STATS(0  + misc::random::Int(8),
                                                0  + misc::random::Int(8) + 100,//TEMP TODO REMOVE Testing combat damage and combat over scenarios
                                                5  + misc::random::Int(8),
                                                10 + misc::random::Int(6),
                                                50 + misc::random::Int(6),
                                                20 + misc::random::Int(10));

            const std::string SORCERER_NAME(boost::algorithm::replace_last_copy(creature::NameInfo::Instance()->LargestName(), creature::NameInfo::Instance()->LargestLetterString(), "S"));
            player::CharacterSPtr_t sorcererSPtr(new player::Character(SORCERER_NAME,
                                                                       creature::sex::Male,
                                                                       creature::BodyType::Make_Pixie(),
                                                                       creature::Race(creature::race::Pixie),
                                                                       creature::Role(creature::role::Sorcerer),
                                                                       SORCERER_STATS));

            player::Initial::Setup(sorcererSPtr.get());
            sorcererSPtr->ManaCurrentSet(1);
            partySPtr->Add(sorcererSPtr, errMsgIgnored);
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
            partySPtr->Add(sylavinSPtr, errMsgIgnored);
        }
        */
        if (restoreInfo_.HasRestored() == false)
        {
            //TEMP TODO REMOVE create new game and player party object
            state::GameStateFactory::Instance()->NewGame(partySPtr);

            combat::Encounter::Instance()->StartTasks();

            //TODO TEMP REMOVE -test create that can't take action
            combat::Encounter::Instance()->NonPlayerParty()->Characters()[0]->ConditionAdd(creature::Conditions::Stone);
        }

        //combat display
        combatDisplayStagePtr_->StageRegionSet(COMBAT_REGION);
        combatDisplayStagePtr_->Setup();

        //combat animations
        combat::CombatAnimation::GiveCombatDisplay(combatDisplayStagePtr_);

        //give control of the CombatDisplay object lifetime to the Loop class
        LoopManager::Instance()->AddStage(combatDisplayStagePtr_);

        if (restoreInfo_.HasRestored() == false)
        {
            //set Pixie creatures to initially flying
            //while this doesn't technically make them fly, the call to restoreInfo_.Restore() will actually set them flying
            {
                combat::CombatNodePVec_t combatNodesPVec;
                combatDisplayStagePtr_->GetCombatNodes(combatNodesPVec);

                for (auto const nextComabtNodeCPtr : combatNodesPVec)
                    if ((creature::race::WillInitiallyFly(nextComabtNodeCPtr->Creature()->Race().Which())) ||
                        (creature::role::WillInitiallyFly(nextComabtNodeCPtr->Creature()->Role().Which())))
                        nextComabtNodeCPtr->IsFlying(true);

                restoreInfo_.Save(combatDisplayStagePtr_);
            }
        }

        //CombatDisplay Zoom Sliderbar
        const sfml_util::gui::TextInfo ZOOMSLIDER_LABEL_TEXT_INFO("Zoom",
                                                                  sfml_util::FontManager::Instance()->Font_Default1(),
                                                                  sfml_util::FontManager::Instance()->Size_Smallish(),
                                                                  sfml_util::FontManager::Color_Light(),
                                                                  sfml_util::Justified::Left);
        //
        const sf::FloatRect ZOOMSLIDER_LABEL_RECT(0.0f, COMMAND_REGION_TOP + COMMAND_REGION_PAD, 0.0f, 0.0f);
        //
        zoomLabelTextRegionSPtr_.reset( new sfml_util::gui::TextRegion("ZoomSlider's",
                                                                       ZOOMSLIDER_LABEL_TEXT_INFO,
                                                                       ZOOMSLIDER_LABEL_RECT) );
        //
        zoomSliderBarSPtr_.reset(new sfml_util::gui::SliderBar("CombatStageZoom",
                                                               COMMAND_REGION_LEFT + COMMAND_REGION_PAD,
                                                               zoomLabelTextRegionSPtr_->GetEntityRegion().top + zoomLabelTextRegionSPtr_->GetEntityRegion().height,
                                                               (settingsButtonSPtr_->GetEntityPos().x - COMMAND_REGION_LEFT) - (COMMAND_REGION_PAD * 2.0f),
                                                               sfml_util::gui::SliderStyle(sfml_util::Orientation::Horiz), this));
        zoomSliderBarSPtr_->SetCurrentValue(1.0f);
        zoomLabelTextRegionSPtr_->SetEntityPos((zoomSliderBarSPtr_->GetEntityPos().x + (zoomSliderBarSPtr_->GetEntityRegion().width * 0.5f)) - (zoomLabelTextRegionSPtr_->GetEntityRegion().width * 0.5f),
                                                zoomLabelTextRegionSPtr_->GetEntityPos().y);
        //
        EntityAdd(zoomLabelTextRegionSPtr_.get());
        EntityAdd(zoomSliderBarSPtr_.get());

        MoveTurnBoxObjectsOffScreen(true);
        restoreInfo_.Restore(combatDisplayStagePtr_);
        SetUserActionAllowed(false);
    }


    void CombatStage::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        target.draw( * commandBoxSPtr_, STATES);
        Stage::Draw(target, STATES);
        statusBoxSPtr_->draw(target, STATES);

        if (sparksAnimUPtr_.get() != nullptr)
        {
            sparksAnimUPtr_->draw(target, STATES);
        }

        if (((turnPhase_ >= TurnPhase::Determine) && (turnPhase_ <= TurnPhase::PostPerformPause)) ||
            (IsNonPlayerCharacterTurnValid() && (TurnPhase::PostCenterAndZoomInPause == turnPhase_)))
        {
            turnBoxSPtr_->draw(target, STATES);

            titleTBoxTextRegionSPtr_->draw(target, STATES);
            weaponTBoxTextRegionSPtr_->draw(target, STATES);
            armorTBoxTextRegionSPtr_->draw(target, STATES);
            infoTBoxTextRegionSPtr_->draw(target, STATES);
            enemyActionTBoxRegionSPtr_->draw(target, STATES);
            enemyCondsTBoxRegionSPtr_->draw(target, STATES);

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

        combatAnimationPtr_->Draw(target, STATES);
        DrawHoverText(target, STATES);
        testingTextRegionSPtr_->draw(target, STATES);
    }


    void CombatStage::UpdateTime(const float ELAPSED_TIME_SEC)
    {
        Stage::UpdateTime(ELAPSED_TIME_SEC);

        if (sparksAnimUPtr_.get() != nullptr)
        {
            sparksAnimUPtr_->Update(ELAPSED_TIME_SEC);
        }

        combatAnimationPtr_->UpdateTime(ELAPSED_TIME_SEC);

        if (willRedColorShakeWeaponText_)
        {
            weaponTBoxTextRegionSPtr_->SetEntityColorFgBoth(redTextColorShaker_.Update(ELAPSED_TIME_SEC));
        }

        if (IsNonPlayerCharacterTurnValid() && (TurnPhase::PostCenterAndZoomInPause == turnPhase_))
        {
            titleTBoxTextRegionSPtr_->SetEntityColorFgBoth(goldTextColorShaker_.Update(ELAPSED_TIME_SEC));
        }

        if (IsNonPlayerCharacterTurnValid() &&
            ((TurnPhase::CenterAndZoomOut == turnPhase_) || (TurnPhase::PostCenterAndZoomOutPause == turnPhase_)))
        {
            enemyActionTBoxRegionSPtr_->SetEntityColorFgBoth(goldTextColorShaker_.Update(ELAPSED_TIME_SEC));
        }

        //allow progress of the pause timer even if IsStatusMessageAnimating(), because they work together
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
            combatAnimationPtr_->DeathAnimUpdate(slider_.Update(ELAPSED_TIME_SEC));
            if (slider_.GetIsDone())
            {
                SetTurnPhase(TurnPhase::PostDeathAnimSlide);
                combatAnimationPtr_->DeathAnimStop();
                PositionSlideStartTasks();
            }
            return;
        }

        if ((TurnPhase::PerformAnim == turnPhase_) &&
            (AnimPhase::ProjectileShoot == animPhase_))
        {
            combatAnimationPtr_->ProjectileShootAnimUpdate( slider_.Update(ELAPSED_TIME_SEC) );
            if (slider_.GetIsDone())
            {
                HandleApplyDamageTasks();
                combatAnimationPtr_->ProjectileShootAnimStop();
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
            combatAnimationPtr_->ImpactAnimUpdate(slider_.Update(ELAPSED_TIME_SEC));
            if (slider_.GetIsDone())
            {
                combatAnimationPtr_->ImpactAnimStop();
                HandleApplyDamageTasks();
                SetAnimPhase(AnimPhase::PostImpactPause);
                StartPause(POST_IMPACT_ANIM_PAUSE_SEC_, "PostImpact");
                SetupTurnBox();
            }
            return;
        }

        if ((TurnPhase::PerformAnim == turnPhase_) &&
            (AnimPhase::MoveToward == animPhase_))
        {
            combatAnimationPtr_->MeleeMoveTowardAnimUpdate(slider_.Update(ELAPSED_TIME_SEC));
            if (slider_.GetIsDone())
            {
                combatAnimationPtr_->MeleeMoveTowardAnimStop();
                SetAnimPhase(AnimPhase::PostMoveTowardPause);
                StartPause(POST_MELEEMOVE_ANIM_PAUSE_SEC_, "PostMoveToward");
                SetupTurnBox();
            }
            return;
        }

        if ((TurnPhase::PerformAnim == turnPhase_) &&
            (AnimPhase::MoveBack == animPhase_))
        {
            combatAnimationPtr_->MeleeMoveBackAnimUpdate(slider_.Update(ELAPSED_TIME_SEC));
            if (slider_.GetIsDone())
            {
                combatAnimationPtr_->MeleeMoveBackAnimStop();
                SetAnimPhase(AnimPhase::FinalPause);
                StartPause(POST_MELEEMOVE_ANIM_PAUSE_SEC_, "Final");
                SetupTurnBox();
            }
            return;
        }

        if (TurnPhase::CenterAndZoomIn == turnPhase_)
        {
            auto const SLIDER_POS{ slider_.Update(ELAPSED_TIME_SEC) };

            auto const ZOOM_CURR_VAL(zoomSliderOrigPos_ + (SLIDER_POS * (1.0f - zoomSliderOrigPos_)));
            zoomSliderBarSPtr_->SetCurrentValue(ZOOM_CURR_VAL);

            combatAnimationPtr_->CenteringUpdate(SLIDER_POS);
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
            combatAnimationPtr_->RepositionAnimUpdate(slider_.Update(ELAPSED_TIME_SEC));
            if (slider_.GetIsDone())
            {
                combatAnimationPtr_->RepositionAnimStop();
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

            if (combatAnimationPtr_->CenteringUpdate(SLIDER_POS))
            {
                auto const ZOOM_CURR_VAL(1.0f - SLIDER_POS);
                zoomSliderBarSPtr_->SetCurrentValue(ZOOM_CURR_VAL);
            }

            if (slider_.GetIsDone())
            {
                combatAnimationPtr_->CenteringStop();
                SetTurnPhase(TurnPhase::PostCenterAndZoomOutPause);
                StartPause(POST_ZOOM_TURN_PAUSE_SEC_, "PostZoomOut");
                SetupTurnBox();
            }
            return;
        }

        if (PreTurnPhase::PanToCenter == preTurnPhase_)
        {
            //the initial pan seems to take extra time getting started so speed it up here
            auto sliderPosAdj{ 0.25f + slider_.Update(ELAPSED_TIME_SEC) };
            if (sliderPosAdj > 1.0f)
                sliderPosAdj = 1.0f;

            combatAnimationPtr_->CenteringUpdate(sliderPosAdj);
            if (slider_.GetIsDone())
            {
                combatAnimationPtr_->CenteringStop();
                SetPreTurnPhase(PreTurnPhase::PostPanPause);
                StartPause(POST_PAN_PAUSE_SEC_, "PostPan");
            }
            return;
        }

        if (PreTurnPhase::ZoomOut == preTurnPhase_)
        {
            auto const SLIDER_POS{ slider_.Update(ELAPSED_TIME_SEC) };
            auto const ZOOM_CURR_VAL(1.0f - SLIDER_POS);
            zoomSliderBarSPtr_->SetCurrentValue(ZOOM_CURR_VAL);

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
            (PreTurnPhase::End == preTurnPhase_))
        {
            StartTurn_Step1();
            return;
        }

        //initial hook for taking action before the first turn (pre-turn logic)
        if ((zoomSliderBarSPtr_.get() != nullptr) &&
            (combatDisplayStagePtr_ != nullptr) &&
            (game::LoopManager::Instance()->IsFading() == false) &&
            (TurnPhase::NotATurn == turnPhase_) &&
            (PreTurnPhase::Start == preTurnPhase_))
        {
            SetPreTurnPhase(PreTurnPhase::PanToCenter);
            slider_.Reset(ANIM_INITIAL_CENTERING_SLIDER_SPEED_);
            combatAnimationPtr_->CenteringStartTargetCenterOfBatllefield();
            return;
        }
    }


    void CombatStage::UpdateMouseDown(const sf::Vector2f & MOUSE_POS_V)
    {
        isMouseHeldDown_ = true;

        //cancel summary view if visible or even just starting
        if (combatDisplayStagePtr_->GetIsSummaryViewInProgress())
        {
            combatDisplayStagePtr_->CancelSummaryViewAndStartTransitionBack();
        }
        else if ((TurnPhase::Determine == turnPhase_) || (TurnPhase::TargetSelect == turnPhase_))
        {
            Stage::UpdateMouseDown(MOUSE_POS_V);
        }
    }


    void CombatStage::UpdateMousePos(const sf::Vector2f & MOUSE_POS_V)
    {
        if (isMouseHeldDown_)
        {
            isMouseHeldDownAndMoving_ = true;
        }
        else
        {
            isMouseHeldDownAndMoving_ = false;
        }

        Stage::UpdateMousePos(MOUSE_POS_V);
    }


    sfml_util::gui::IGuiEntityPtr_t CombatStage::UpdateMouseUp(const sf::Vector2f & MOUSE_POS_V)
    {
        auto const WAS_MOUSE_HELD_DOWN_AND_MOVING{ isMouseHeldDownAndMoving_ };
        isMouseHeldDown_ = false;
        isMouseHeldDownAndMoving_ = false;

        creature::CreaturePtr_t creatureAtPosPtr(combatDisplayStagePtr_->GetCreatureAtPos(MOUSE_POS_V));

        if (WAS_MOUSE_HELD_DOWN_AND_MOVING == false)
        {
            if (creatureAtPosPtr == nullptr)
            {
                HandleMiscCancelTasks();
            }
            else
            {
                if ((TurnPhase::Determine == turnPhase_) &&
                    creatureAtPosPtr->IsPlayerCharacter() &&
                    (combatDisplayStagePtr_->GetIsSummaryViewInProgress() == false))
                {
                    restoreInfo_.PrepareForStageChange(combatDisplayStagePtr_);
                    game::LoopManager::Instance()->Goto_Inventory(creatureAtPosPtr);
                }
                else if (TurnPhase::TargetSelect == turnPhase_)
                {
                    if (nullptr == spellBeingCastPtr_)
                    {
                        if (creatureAtPosPtr->IsPlayerCharacter())
                        {
                            QuickSmallPopup("That is one of your player characters, who cannot be attacked.  Click on an enemy creature instead.", false, true);
                        }
                        else
                        {
                            if (combatDisplayStagePtr_->IsCreatureAPossibleFightTarget(turnCreaturePtr_, creatureAtPosPtr))
                            {
                                combatAnimationPtr_->SelectAnimStart(creatureAtPosPtr);
                                HandleAttackTasks(creatureAtPosPtr);
                            }
                            else
                            {
                                QuickSmallPopup("That creature is not close enough to fight.  Try clicking on another creature.", false, true);
                            }
                        }
                    }
                    else if (spellBeingCastPtr_->Target() == TargetType::SingleOpponent)
                    {
                        if (creatureAtPosPtr->IsPlayerCharacter())
                        {
                            QuickSmallPopup("That is one of your player characters, not an enemy.  Click on an enemy creature instead.", false, true);
                        }
                        else
                        {
                            combatAnimationPtr_->SelectAnimStart(creatureAtPosPtr);
                            HandleCast_Step3_PerformOnTargets(creature::CreaturePVec_t{ creatureAtPosPtr });
                        }
                    }
                    else if ((nullptr != spellBeingCastPtr_) && (spellBeingCastPtr_->Target() == TargetType::SingleCompanion))
                    {
                        if (creatureAtPosPtr->IsPlayerCharacter() == false)
                        {
                            QuickSmallPopup("That is and enemy, not one of your character.  Click on one of your characters instead.", false, true);
                        }
                        else
                        {
                            combatAnimationPtr_->SelectAnimStart(creatureAtPosPtr);
                            HandleCast_Step3_PerformOnTargets(creature::CreaturePVec_t{ creatureAtPosPtr });
                        }
                    }
                }
            }
        }

        return Stage::UpdateMouseUp(MOUSE_POS_V);
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
                auto const MOUSEOVER_ATTACK_STR(combat::Text::MouseOverTextAttackStr(turnCreaturePtr_, combatDisplayStagePtr_));
                if (MOUSEOVER_ATTACK_STR == combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_ATTACK_)
                {
                    return HandleAttack();
                }
                else
                {
                    auto const MOUSEOVER_STR(combat::Text::MouseOverTextBlockStr(turnCreaturePtr_, combatDisplayStagePtr_));
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
                return HandleAttack();

            if (KE.code == sf::Keyboard::F)
                return HandleFight();

            if (KE.code == sf::Keyboard::C)
                return HandleCast_Step1_ValidateCastAndSelectSpell();

            if (KE.code == sf::Keyboard::Right)
                return HandleAdvance();

            if (KE.code == sf::Keyboard::Left)
                return HandleRetreat();

            if (KE.code == sf::Keyboard::Y)
                return HandleFly();

            if (KE.code == sf::Keyboard::L)
                return HandleLand();

            if (KE.code == sf::Keyboard::R)
                return HandleRoar();

            if (KE.code == sf::Keyboard::X)
                return HandleWeaponChange();

            if (nullptr != turnCreaturePtr_)
            {
                if ((KE.code == sf::Keyboard::B) ||
                    ((KE.code == sf::Keyboard::Space) && (turnCreaturePtr_->CanTakeAction())))
                {
                    return HandleBlock();
                }

                if ((KE.code == sf::Keyboard::S) ||
                    ((KE.code == sf::Keyboard::Space) && (turnCreaturePtr_->CanTakeAction() == false)))
                {
                    return HandleSkip();
                }

                if (KE.code == sf::Keyboard::I)
                {
                    if (turnCreaturePtr_->IsPlayerCharacter())
                    {
                        restoreInfo_.PrepareForStageChange(combatDisplayStagePtr_);
                        game::LoopManager::Instance()->Goto_Inventory(turnCreaturePtr_);
                    }
                }

                if (KE.code == sf::Keyboard::P)
                {
                    return HandlePounce(combat::Encounter::Instance()->GetTurnInfoCopy(turnCreaturePtr_).GetIsFlying());
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
        ss << combat::Text::InitialCombatStatusMessagePrefix() << " " << combat::Encounter::Instance()->NonPlayerParty()->Summary() << "!";

        statusBoxTextInfo_.text = ss.str();
        statusBoxSPtr_->Add(std::make_shared<sfml_util::gui::ListBoxItem>("CombatStageStatusMsg", statusBoxTextInfo_), true);

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
        statusBoxSPtr_->Add(std::make_shared<sfml_util::gui::ListBoxItem>("CombatStageStatusMsg", statusBoxTextInfo_), true);

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

        pauseElapsedSec_ = pauseDurationSec_ + 1.0f;//anything greater than pauseDurationSec_ will work here

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
            combatAnimationPtr_->MeleeMoveBackAnimStart();
            slider_.Reset(ANIM_MELEE_MOVE_SLIDER_SPEED_);
            return;
        }

        if ((TurnPhase::PerformAnim == turnPhase_) &&
            (AnimPhase::PostMoveTowardPause == animPhase_))
        {
            SetAnimPhase(AnimPhase::Impact);
            SetupTurnBox();
            combatAnimationPtr_->ImpactAnimStart(ANIM_IMPACT_SHAKE_SLIDER_SPEED_);
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

        if (TurnPhase::ConditionWakePause == turnPhase_)
        {
            SetTurnPhase(TurnPhase::PostTurnPause);
            StartPause(POST_TURN_PAUSE_SEC_, "PostTurn");
            return;
        }

        if (IsPlayerCharacterTurnValid() && (TurnPhase::PostCenterAndZoomInPause == turnPhase_))
        {
            tempConditionsWakeStr_ = RemoveSingleTurnTemporaryConditions();
            if (tempConditionsWakeStr_.empty())
            {
                SetTurnPhase(TurnPhase::Determine);
                SetUserActionAllowed(true);
            }
            else
            {
                SetTurnPhase(TurnPhase::ConditionWakePause);
                StartPause(CONDITION_WAKE_PAUSE_SEC_, "ConditionWakePause");
            }

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
            tempConditionsWakeStr_ = RemoveSingleTurnTemporaryConditions();
            if (tempConditionsWakeStr_.empty() == false)
            {
                SetTurnPhase(TurnPhase::ConditionWakePause);
                StartPause(CONDITION_WAKE_PAUSE_SEC_, "ConditionWakePause");
                SetupTurnBox();
                return;
            }

            SetTurnPhase(TurnPhase::Determine);
            HandleEnemyTurnStep1_Decide();

            if (turnActionInfo_.Action() == combat::TurnAction::Nothing)
            {
                AppendStatusMessage(combat::Text::ActionText(turnCreaturePtr_, turnActionInfo_, fightResult_, true, true), false);
                SetTurnPhase(TurnPhase::PostTurnPause);
                StartPause(POST_TURN_PAUSE_SEC_, "PostTurn");
            }
            else
            {
                //also do the perform step here so that the TurnBox can display the non-player creature's intent before showing the result
                SetTurnActionPhase( HandleEnemyTurnStep2_Perform() );

                //collect a list of all attacking and targeted creatures that need to be centered on the screen
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
                    combatAnimationPtr_->CenteringStart(allEffectedCreaturesPVec);
                    slider_.Reset(ANIM_CENTERING_SLIDER_SPEED_);
                }
            }
            return;
        }

        if (IsNonPlayerCharacterTurnValid() && (TurnPhase::PostCenterAndZoomOutPause == turnPhase_))
        {
            //See above for call to HandleEnemyTurnStep2_Perform()
            StartPerformAnim();
            SetupTurnBox();
            return;
        }

        if (TurnPhase::PerformReport == turnPhase_)
        {
            auto const CREATURE_EFFECTS_VEC{ fightResult_.Effects() };
            if (performReportEffectIndex_ < CREATURE_EFFECTS_VEC.size())
            {
                auto const HIT_INFO_VEC_SIZE{ CREATURE_EFFECTS_VEC.at(performReportEffectIndex_).GetHitInfoVec().size() };
                if ((HIT_INFO_VEC_SIZE > 0) && (performReportHitIndex_ <  (HIT_INFO_VEC_SIZE - 1)))
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

                combatSoundEffects_.PlayHitOrMiss(CREATURE_EFFECTS_VEC.at(performReportEffectIndex_).GetHitInfoVec().at(performReportHitIndex_));
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
                if ((NEXT_CREATURE_EFFECT.WasKill()) && (NEXT_CREATURE_EFFECT.GetCreature()->IsPlayerCharacter() == false))
                    killedNonPlayerCreaturesPVec.push_back(NEXT_CREATURE_EFFECT.GetCreature());

            if (killedNonPlayerCreaturesPVec.empty())
            {
                SetTurnPhase(TurnPhase::PostTurnPause);
                StartPause(POST_TURN_PAUSE_SEC_, "PostTurn");
            }
            else
            {
                SetTurnPhase(TurnPhase::DeathAnim);
                combatAnimationPtr_->DeathAnimStart(killedNonPlayerCreaturesPVec);
                slider_.Reset(ANIM_DEATH_SLIDER_SPEED_);
                //TODO SOUNDEFFECT play creature death sound effect here
            }
            return;
        }

        if (TurnPhase::PostTurnPause == turnPhase_)
        {
            HandleAfterTurnTasks();
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
                fightResult_ = combat::FightClub::Fight(turnCreaturePtr_, turnActionInfo_.Target());
                SetupTurnBox();
                return GetTurnActionPhaseFromFightResult(fightResult_);
            }

            case combat::TurnAction::Block:
            {
                fightResult_ = combat::FightResult();
                SetupTurnBox();
                AppendStatusMessage(combat::Text::ActionText(turnCreaturePtr_, turnActionInfo_, fightResult_, true, true), false);
                return TurnActionPhase::PauseAndReport;
            }

            case combat::TurnAction::Fly:
            case combat::TurnAction::Land:
            {
                fightResult_ = combat::FightResult();
                combatDisplayStagePtr_->HandleFlyingChange(turnCreaturePtr_, false);
                SetupTurnBox();
                AppendStatusMessage(combat::Text::ActionText(turnCreaturePtr_, turnActionInfo_, fightResult_, true, true), false);
                return TurnActionPhase::PauseAndReport;
            }

            case combat::TurnAction::Cast:
            {
                creature::CreaturePVec_t targetedCreaturesPVec{ turnActionInfo_.Targets() };
                combatDisplayStagePtr_->SortCreatureListByDisplayedPosition(targetedCreaturesPVec);
                fightResult_ = combat::FightClub::Cast(turnActionInfo_.Spell(), turnCreaturePtr_, targetedCreaturesPVec);
                SetupTurnBox();
                AppendStatusMessage(combat::Text::ActionText(turnCreaturePtr_, turnActionInfo_, fightResult_, true, true), false);
                return TurnActionPhase::Cast;
            }

            case combat::TurnAction::SkyPounce:
            case combat::TurnAction::LandPounce:
            {
                fightResult_ = combat::FightClub::Pounce(turnCreaturePtr_, turnActionInfo_.Target());
                SetupTurnBox();
                AppendStatusMessage(combat::Text::ActionText(turnCreaturePtr_, turnActionInfo_, fightResult_, true, true), false);

                //TODO if fightResult_ says the pounce was a success then return the TurnActionPhase::Pounce
                return TurnActionPhase::PauseAndReport;
            }

            case combat::TurnAction::Roar:
            {
                fightResult_ = combat::FightClub::Roar(turnCreaturePtr_, combatDisplayStagePtr_);
                SetupTurnBox();
                AppendStatusMessage(combat::Text::ActionText(turnCreaturePtr_, turnActionInfo_, fightResult_, true, true), false);

                //TODO if fightResult_ says the roar happened then return TurnActionPhase::Roar
                return TurnActionPhase::PauseAndReport;
            }

            case combat::TurnAction::ChangeWeapon:
            case combat::TurnAction::Nothing:
            case combat::TurnAction::Count://handle Count gracefully because it is sometimes required
            {
                fightResult_ = combat::FightResult();
                SetupTurnBox();
                AppendStatusMessage(combat::Text::ActionText(turnCreaturePtr_, turnActionInfo_, fightResult_, true, true), false);
                return TurnActionPhase::PauseAndReport;
            }

            default:
            {
                std::ostringstream ss;
                ss << "game::stage::CombatStage::HandleEnemyTurnStep2_Perform() found turnActionInfo_.Action() (which is really a combat::TurnAction) of " << turnActionInfo_.Action() << " which is invalid.";
                throw std::runtime_error(ss.str());
            }
        }
    }


    //start centering anim
    void CombatStage::StartTurn_Step1()
    {
        zoomSliderOrigPos_ = zoomSliderBarSPtr_->GetCurrentValue();

        turnCreaturePtr_ = combat::Encounter::Instance()->CurrentTurnCreature();

        SetTurnPhase(TurnPhase::CenterAndZoomIn);
        combatAnimationPtr_->CenteringStart(turnCreaturePtr_);
        slider_.Reset(ANIM_CENTERING_SLIDER_SPEED_);
    }


    //start pre-pause
    void CombatStage::StartTurn_Step2()
    {
        //TODO TEMP REMOVE -testing sparks animation
        sparksAnimUPtr_ = std::make_unique<sfml_util::animation::SparksAnimation>(true,
                                                                                  combatDisplayStagePtr_->GetCombatNodeForCreature(turnCreaturePtr_)->GetEntityRegion(),
                                                                                  0.25f,
                                                                                  sfml_util::MapByRes(0.15f, 0.45f),
                                                                                  0.75f);

        auto const IS_PLAYER_TURN{ turnCreaturePtr_->IsPlayerCharacter() };
        combatDisplayStagePtr_->SetIsPlayerTurn(IS_PLAYER_TURN);

        combatAnimationPtr_->CenteringStop();
        combatAnimationPtr_->ShakeAnimStart(turnCreaturePtr_, ANIM_CREATURE_SHAKE_SLIDER_SPEED_, false);

        goldTextColorShaker_.Reset();

        //skip PostZoomInPause if a player turn
        if (IS_PLAYER_TURN)
        {
            tempConditionsWakeStr_ = RemoveSingleTurnTemporaryConditions();
            if (tempConditionsWakeStr_.empty())
            {
                SetTurnPhase(TurnPhase::Determine);
                SetUserActionAllowed(true);
            }
            else
            {
                SetTurnPhase(TurnPhase::ConditionWakePause);
                StartPause(CONDITION_WAKE_PAUSE_SEC_, "ConditionWakePause");
            }

            SetupTurnBox();
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
            SetupTurnBox();
        }
    }


    void CombatStage::EndTurn()
    {
        //TODO TEMP REMOVE
        sparksAnimUPtr_.reset();

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
    }


    void CombatStage::PositionSlideStartTasks()
    {
        M_ASSERT_OR_LOGANDTHROW_SS((turnCreaturePtr_ != nullptr), "game::stage::CombatStage::PositionSlideStartTasks() turnCreaturePtr_ was nullptr.");
        combatAnimationPtr_->RepositionAnimStart(turnCreaturePtr_);
        combatAnimationPtr_->ShakeAnimStop(turnCreaturePtr_);
        slider_.Reset(ANIM_CREATURE_POS_SLIDER_SPEED_);
    }


    bool CombatStage::HandleAttack()
    {
        auto const MOUSEOVER_STR( combat::Text::MouseOverTextAttackStr(turnCreaturePtr_, combatDisplayStagePtr_) );
        if (MOUSEOVER_STR != combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_ATTACK_)
        {
            QuickSmallPopup(MOUSEOVER_STR, false, true);
            return false;
        }
        else
        {
            HandleAttackTasks( combat::FightClub::FindNonPlayerCreatureToAttack(turnCreaturePtr_, combatDisplayStagePtr_) );
            return true;
        }
    }


    bool CombatStage::HandleFight()
    {
        auto const MOUSEOVER_STR( combat::Text::MouseOverTextFightStr(turnCreaturePtr_, combatDisplayStagePtr_) );
        if (MOUSEOVER_STR != combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_FIGHT_)
        {
            QuickSmallPopup(MOUSEOVER_STR, false, true);
            return false;
        }
        else
        {
            //can't set turnActionInfo_ or fightResult_ or turnActionPhase_ yet because the player must select which non_player creature to fight first
            combatDisplayStagePtr_->SetSummaryViewAllowed(false);
            combatDisplayStagePtr_->SetScrollingAllowed(true);
            SetTurnPhase(TurnPhase::TargetSelect);
            SetupTurnBox();
            return true;
        }
    }


    bool CombatStage::HandleCast_Step1_ValidateCastAndSelectSpell()
    {
        auto const MOUSEOVER_STR( combat::Text::MouseOverTextCastStr(turnCreaturePtr_, combatDisplayStagePtr_)  );
        if (MOUSEOVER_STR != combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_CAST_)
        {
            QuickSmallPopup(MOUSEOVER_STR, false, true);
            return false;
        }
        else
        {
            SetUserActionAllowed(false);
            SetTurnActionPhase(TurnActionPhase::Cast);
            auto const POPUP_INFO{ sfml_util::gui::PopupManager::Instance()->CreateSpellbookPopupInfo(POPUP_NAME_SPELLBOOK_, turnCreaturePtr_, restoreInfo_.LastCastSpellNum(turnCreaturePtr_)) };
            LoopManager::Instance()->PopupWaitBegin(this, POPUP_INFO);
            return true;
        }
    }


    void CombatStage::HandleCast_Step2_SelectTargetOrPerformOnAll(spell::SpellPtr_t spellPtr)
    {
        spellBeingCastPtr_ = spellPtr;

        if ((spellPtr->Target() == TargetType::SingleCompanion) ||
            (spellPtr->Target() == TargetType::SingleOpponent))
        {
            SetUserActionAllowed(true);
            combatDisplayStagePtr_->SetSummaryViewAllowed(false);
            combatDisplayStagePtr_->SetScrollingAllowed(true);
            SetTurnPhase(TurnPhase::TargetSelect);
            SetupTurnBox();
        }
        else if (spellPtr->Target() == TargetType::AllCompanions)
        {
            HandleCast_Step3_PerformOnTargets(creature::Algorithms::Players(true));
        }
        else if (spellPtr->Target() == TargetType::AllOpponents)
        {
            HandleCast_Step3_PerformOnTargets(creature::Algorithms::NonPlayers(true));
        }
        else
        {
            std::ostringstream ssErr;
            ssErr << "game::stage::CombatStage::HandleCast_Step2_SelectTargetOrPerformOnAll(spell=" << spellPtr->Name() << ") had a target_type of " << TargetType::ToString(spellPtr->Target()) << " which is not yet supported during combat.";
            throw std::runtime_error(ssErr.str());
        }
    }


    void CombatStage::HandleCast_Step3_PerformOnTargets(creature::CreaturePVec_t creaturesToCastUponPVec)
    {
        SetUserActionAllowed(false);

        combatDisplayStagePtr_->SortCreatureListByDisplayedPosition(creaturesToCastUponPVec);
        turnActionInfo_ = combat::TurnActionInfo(spellBeingCastPtr_, creaturesToCastUponPVec);
        combat::Encounter::Instance()->SetTurnActionInfo(turnCreaturePtr_, turnActionInfo_);
        fightResult_ = combat::FightClub::Cast(spellBeingCastPtr_, turnCreaturePtr_, creaturesToCastUponPVec);
        SetTurnActionPhase(TurnActionPhase::Cast);

        SetTurnPhase(TurnPhase::CenterAndZoomOut);
        auto creaturesToCenterOnPVec{ creaturesToCastUponPVec };
        creaturesToCenterOnPVec.push_back(turnCreaturePtr_);
        combatAnimationPtr_->CenteringStart(creaturesToCenterOnPVec);
        slider_.Reset(ANIM_CENTERING_SLIDER_SPEED_);

        SetupTurnBox();
    }


    bool CombatStage::HandleAdvance()
    {
        auto const MOUSEOVER_STR( combat::Text::MouseOverTextAdvanceStr(turnCreaturePtr_, combatDisplayStagePtr_)  );
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
        auto const MOUSEOVER_STR( combat::Text::MouseOverTextRetreatStr(turnCreaturePtr_, combatDisplayStagePtr_) );
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
        auto const MOUSEOVER_STR( combat::Text::MouseOverTextBlockStr(turnCreaturePtr_, combatDisplayStagePtr_) );
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

            //no need for ZoomAndSlide or PerformAnim so skip to end of turn with AppendStatusMessage()
            AppendStatusMessage(combat::Text::ActionText(turnCreaturePtr_, turnActionInfo_, fightResult_, true, true), true);
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
        AppendStatusMessage(combat::Text::ActionText(turnCreaturePtr_, turnActionInfo_, fightResult_, true, true), true);
        return true;
    }


    bool CombatStage::HandleFly()
    {
        auto const MOUSEOVER_STR( combat::Text::MouseOverTextFlyStr(turnCreaturePtr_, combatDisplayStagePtr_) );
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

            //no need for turnActionPhase_, ZoomAndSlide, or PerformAnim so skip to end of turn with AppendStatusMessage()
            AppendStatusMessage(combat::Text::ActionText(turnCreaturePtr_, turnActionInfo_, fightResult_, true, true), true);
            return true;
        }
    }


    bool CombatStage::HandleLand()
    {
        auto const MOUSEOVER_STR( combat::Text::MouseOverTextLandStr(turnCreaturePtr_, combatDisplayStagePtr_) );
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

            //no need for turnActionPhase_, ZoomAndSlide, or PerformAnim so skip to end of turn with AppendStatusMessage()
            AppendStatusMessage(combat::Text::ActionText(turnCreaturePtr_, turnActionInfo_, fightResult_, true, true), false);
            return true;
        }
    }


    bool CombatStage::HandleRoar()
    {
        auto const MOUSEOVER_STR( combat::Text::MouseOverTextRoarStr(turnCreaturePtr_, combatDisplayStagePtr_) );
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
            AppendStatusMessage(combat::Text::ActionText(turnCreaturePtr_, turnActionInfo_, fightResult_, true, true), false);
            SetTurnActionPhase(TurnActionPhase::Roar);

            //not implemented yet so skip to end of turn with AppendStatusMessage()
            AppendStatusMessage(combat::Text::ActionText(turnCreaturePtr_, turnActionInfo_, fightResult_, true, true), false);
            return true;
        }
    }


    bool CombatStage::HandlePounce(const bool IS_SKY_POUNCE)
    {
        auto const MOUSEOVER_STR( combat::Text::MouseOverTextPounceStr(turnCreaturePtr_, combatDisplayStagePtr_) );
        if (MOUSEOVER_STR != combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_POUNCE_)
        {
            QuickSmallPopup(MOUSEOVER_STR, false, true);
            return false;
        }
        else
        {
            SetUserActionAllowed(false);

            turnActionInfo_ = combat::TurnActionInfo(((IS_SKY_POUNCE) ? combat::TurnAction::SkyPounce : combat::TurnAction::LandPounce));
            combat::Encounter::Instance()->SetTurnActionInfo(turnCreaturePtr_, turnActionInfo_);

            //not implemented yet so skip to end of turn with AppendStatusMessage()
            AppendStatusMessage(combat::Text::ActionText(turnCreaturePtr_, turnActionInfo_, fightResult_, true, true), true);
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


    void CombatStage::HandleAfterTurnTasks()
    {
        HandleKilledCreatures();
        combatDisplayStagePtr_->HandleEndOfTurnTasks();
        restoreInfo_.CanTurnAdvance(true);
        EndTurn();
        EndPause();
    }


    void CombatStage::MoveTurnBoxObjectsOffScreen(const bool WILL_MOVE_SKIP_BUTTON)
    {
        titleTBoxTextRegionSPtr_->MoveEntityOffScreen();
        weaponTBoxTextRegionSPtr_->MoveEntityOffScreen();
        armorTBoxTextRegionSPtr_->MoveEntityOffScreen();
        infoTBoxTextRegionSPtr_->MoveEntityOffScreen();
        enemyActionTBoxRegionSPtr_->MoveEntityOffScreen();
        enemyCondsTBoxRegionSPtr_->MoveEntityOffScreen();

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

            {
                const std::string MOT_ATTACK_STR(combat::Text::MouseOverTextAttackStr(CREATURE_CPTRC, combatDisplayStagePtr_));
                attackTBoxButtonSPtr_->SetIsDisabled(MOT_ATTACK_STR != combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_ATTACK_);
                attackTBoxButtonSPtr_->SetMouseHoverText(MOT_ATTACK_STR);

                if (MOT_ATTACK_STR == combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_ATTACK_)
                    tBoxStdButtonSVec_.push_back(attackTBoxButtonSPtr_);
            }

            {
                const std::string MOT_FIGHT_STR(combat::Text::MouseOverTextFightStr(CREATURE_CPTRC, combatDisplayStagePtr_));
                fightTBoxButtonSPtr_->SetIsDisabled(MOT_FIGHT_STR != combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_FIGHT_);
                fightTBoxButtonSPtr_->SetMouseHoverText(MOT_FIGHT_STR);

                if (MOT_FIGHT_STR == combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_FIGHT_)
                    tBoxStdButtonSVec_.push_back(fightTBoxButtonSPtr_);
            }

            {
                const std::string MOT_CAST_STR(combat::Text::MouseOverTextCastStr(CREATURE_CPTRC, combatDisplayStagePtr_));
                castTBoxButtonSPtr_->SetIsDisabled(MOT_CAST_STR != combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_CAST_);
                castTBoxButtonSPtr_->SetMouseHoverText(MOT_CAST_STR);

                if (MOT_CAST_STR == combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_CAST_)
                    tBoxStdButtonSVec_.push_back(castTBoxButtonSPtr_);
            }

            {
                const std::string MOT_ADVANCE_STR(combat::Text::MouseOverTextAdvanceStr(CREATURE_CPTRC, combatDisplayStagePtr_));
                advanceTBoxButtonSPtr_->SetIsDisabled(MOT_ADVANCE_STR != combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_ADVANCE_);
                advanceTBoxButtonSPtr_->SetMouseHoverText(MOT_ADVANCE_STR);
            }

            {
                const std::string MOT_RETREAT_STR(combat::Text::MouseOverTextRetreatStr(CREATURE_CPTRC, combatDisplayStagePtr_));
                retreatTBoxButtonSPtr_->SetIsDisabled(MOT_RETREAT_STR != combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_RETREAT_);
                retreatTBoxButtonSPtr_->SetMouseHoverText(MOT_RETREAT_STR);
            }

            {
                const std::string MOT_BLOCK_STR(combat::Text::MouseOverTextBlockStr(CREATURE_CPTRC, combatDisplayStagePtr_));
                blockTBoxButtonSPtr_->SetIsDisabled(MOT_BLOCK_STR != combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_BLOCK_);
                blockTBoxButtonSPtr_->SetMouseHoverText(MOT_BLOCK_STR);
            }

            {
                const std::string MOT_FLY_STR(combat::Text::MouseOverTextFlyStr(CREATURE_CPTRC, combatDisplayStagePtr_));
                flyTBoxButtonSPtr_->SetIsDisabled(MOT_FLY_STR != combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_FLY_);
                flyTBoxButtonSPtr_->SetMouseHoverText(MOT_FLY_STR);

                if (MOT_FLY_STR == combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_FLY_)
                    tBoxStdButtonSVec_.push_back(flyTBoxButtonSPtr_);
            }

            {
                const std::string MOT_LAND_STR(combat::Text::MouseOverTextLandStr(CREATURE_CPTRC, combatDisplayStagePtr_));
                landTBoxButtonSPtr_->SetIsDisabled(MOT_LAND_STR != combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_LAND_);
                landTBoxButtonSPtr_->SetMouseHoverText(MOT_LAND_STR);

                if (MOT_LAND_STR == combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_LAND_)
                    tBoxStdButtonSVec_.push_back(landTBoxButtonSPtr_);
            }

            {
                const std::string MOT_ROAR_STR(combat::Text::MouseOverTextRoarStr(CREATURE_CPTRC, combatDisplayStagePtr_));
                roarTBoxButtonSPtr_->SetIsDisabled(MOT_ROAR_STR != combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_ROAR_);
                roarTBoxButtonSPtr_->SetMouseHoverText(MOT_ROAR_STR);

                if (MOT_ROAR_STR == combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_ROAR_)
                    tBoxStdButtonSVec_.push_back(roarTBoxButtonSPtr_);
            }

            {
                const std::string MOT_POUNCE_STR(combat::Text::MouseOverTextPounceStr(CREATURE_CPTRC, combatDisplayStagePtr_));
                pounceTBoxButtonSPtr_->SetIsDisabled(MOT_POUNCE_STR != combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_POUNCE_);
                pounceTBoxButtonSPtr_->SetMouseHoverText(MOT_POUNCE_STR);

                if (MOT_POUNCE_STR == combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_POUNCE_)
                    tBoxStdButtonSVec_.push_back(pounceTBoxButtonSPtr_);
            }

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
            auto const RIGHTROW_POS_LEFT((turnBoxRegion_.left + turnBoxRegion_.width) - BUTTON_ROW_WIDTH);
            auto const RIGHTROW_VERT_POS_OFFSET(turnBoxRegion_.height / (static_cast<float>(tBoxStdButtonSVec_.size()) + 1.0f));
            const size_t NUM_RIGHTROW_STD_BUTTONS(tBoxStdButtonSVec_.size());
            for (size_t i(0); i < NUM_RIGHTROW_STD_BUTTONS; ++i)
                tBoxStdButtonSVec_[i]->SetEntityPos(RIGHTROW_POS_LEFT - (tBoxStdButtonSVec_[i]->GetEntityRegion().width * 0.5f), turnBoxRegion_.top + (RIGHTROW_VERT_POS_OFFSET * 0.5f) + (static_cast<float>(i) * RIGHTROW_VERT_POS_OFFSET));
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
        ss << ((WILL_PREPEND_NEWLINE) ? "\n" : "") << PROMPT;
        const PopupInfo POPUP_INFO(sfml_util::gui::PopupManager::Instance()->CreatePopupInfo("CombatStage'sQuickPopup",
                                                                                             ss.str(),
                                                                                             sfml_util::PopupButtons::Okay,
                                                                                             sfml_util::PopupImage::Banner,
                                                                                             sfml_util::Justified::Center,
                                                                                             ((IS_SOUNDEFFECT_NORMAL) ? sfml_util::sound_effect::PromptGeneric : sfml_util::sound_effect::PromptWarn),
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
            ((TurnPhase::PostCenterAndZoomInPause == turnPhase_) && IsNonPlayerCharacterTurnValid()) ||
            ((TurnPhase::PerformAnim == turnPhase_) && (animPhase_ < AnimPhase::Impact)))
        {
            willDrawTurnBoxButtons = false;

            if ((fightResult_.Count() > 0) ||
                ((TurnPhase::PerformAnim == turnPhase_) && (AnimPhase::AdvanceOrRetreat == animPhase_)))
            {
                infoSS.str(EMPTY_STR);
                weaponHoldingSS.str(EMPTY_STR);
                armorSS.str(EMPTY_STR);
                enemyCondsSS.str(EMPTY_STR);

                isPreambleShowing = true;

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
            willDrawTurnBoxButtons = false;

            infoSS.str(EMPTY_STR);
            weaponHoldingSS.str(EMPTY_STR);
            armorSS.str(EMPTY_STR);
            enemyCondsSS.str(EMPTY_STR);

            isPreambleShowing = true;

            preambleSS << combat::Text::ActionText(turnCreaturePtr_,
                                                   turnActionInfo_,
                                                   fightResult_,
                                                   false,
                                                   false,
                                                   false,
                                                   performReportEffectIndex_,
                                                   performReportHitIndex_);
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
        else if (TurnPhase::ConditionWakePause == turnPhase_)
        {
            willDrawTurnBoxButtons = false;

            infoSS.str(EMPTY_STR);
            weaponHoldingSS.str(EMPTY_STR);
            armorSS.str(EMPTY_STR);
            enemyCondsSS.str(EMPTY_STR);

            isPreambleShowing = true;

            if (IsPlayerCharacterTurnValid())
            {
                preambleSS << turnCreaturePtr_->Name();
            }
            else
            {
                preambleSS << creature::sex::HeSheIt(turnCreaturePtr_->Sex(), true);
            }

            preambleSS << " " << tempConditionsWakeStr_ << ".";
        }
        else
        {
            preambleSS.str(EMPTY_STR);
        }

        auto const VERT_POS_SHIFT(sfml_util::MapByRes(0.0f, 16.0f));
        titleTBoxTextRegionSPtr_->SetText(titleSS.str());
        titleTBoxTextRegionSPtr_->SetEntityPos(turnBoxRegion_.left,
                                               (turnBoxRegion_.top + (titleTBoxTextRegionSPtr_->GetEntityRegion().height * 0.5f)) - VERT_POS_SHIFT);

        weaponTBoxTextRegionSPtr_->SetText(weaponHoldingSS.str());
        weaponTBoxTextRegionSPtr_->SetEntityPos((turnBoxRegion_.left + (turnBoxRegion_.width * 0.5f)) - (weaponTBoxTextRegionSPtr_->GetEntityRegion().width * 0.5f),
                                                (titleTBoxTextRegionSPtr_->GetEntityRegion().top + titleTBoxTextRegionSPtr_->GetEntityRegion().height) - VERT_POS_SHIFT);

        auto const WEAPON_TBOXTEXT_REGION{ weaponTBoxTextRegionSPtr_->GetEntityRegion() };
        armorTBoxTextRegionSPtr_->SetText(armorSS.str());
        armorTBoxTextRegionSPtr_->SetEntityPos((turnBoxRegion_.left + (turnBoxRegion_.width * 0.5f)) - (armorTBoxTextRegionSPtr_->GetEntityRegion().width * 0.5f),
                                               (WEAPON_TBOXTEXT_REGION.top + WEAPON_TBOXTEXT_REGION.height) - VERT_POS_SHIFT);

        enemyCondsTBoxRegionSPtr_->SetText(enemyCondsSS.str());
        enemyCondsTBoxRegionSPtr_->SetEntityPos((turnBoxRegion_.left + (turnBoxRegion_.width * 0.5f)) - (enemyCondsTBoxRegionSPtr_->GetEntityRegion().width * 0.5f),
                                                (armorTBoxTextRegionSPtr_->GetEntityRegion().top + armorTBoxTextRegionSPtr_->GetEntityRegion().height) - VERT_POS_SHIFT);

        auto enemyActionPosLeft{ (turnBoxRegion_.left + (turnBoxRegion_.width * 0.5f)) - (enemyActionTBoxRegionSPtr_->GetEntityRegion().width * 0.5f) };
        auto enemyActionPosTop{ (enemyCondsTBoxRegionSPtr_->GetEntityRegion().top + enemyCondsTBoxRegionSPtr_->GetEntityRegion().height) - VERT_POS_SHIFT };

        if (enemyCondsSS.str() == EMPTY_STR)
        {
            enemyActionPosTop = (armorTBoxTextRegionSPtr_->GetEntityRegion().top + armorTBoxTextRegionSPtr_->GetEntityRegion().height) - VERT_POS_SHIFT;
        }

        enemyActionTBoxRegionSPtr_->SetText(preambleSS.str());

        if (isPreambleShowing ||
            ((TurnPhase::PerformReport == turnPhase_) ||
             (TurnPhase::PostPerformPause == turnPhase_) ||
             (TurnPhase::PerformAnim == turnPhase_)))
        {
            enemyActionTBoxRegionSPtr_->SetEntityPos(enemyActionPosLeft, weaponTBoxTextRegionSPtr_->GetEntityPos().y);
        }
        else
        {
            enemyActionTBoxRegionSPtr_->SetEntityPos(enemyActionPosLeft, enemyActionPosTop);
        }

        infoTBoxTextRegionSPtr_->SetText(infoSS.str());
        infoTBoxTextRegionSPtr_->SetEntityPos(turnBoxRegion_.left, turnBoxRegion_.top);

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
                AppendStatusMessage(combat::Text::ActionText(turnCreaturePtr_, turnActionInfo_, fightResult_, true, true), false);
                SetAnimPhase(AnimPhase::AdvanceOrRetreat);
                break;
            }

            case TurnActionPhase::Retreat:
            {
                combatDisplayStagePtr_->MoveCreatureBlockingPosition(turnCreaturePtr_, false);
                PositionSlideStartTasks();
                AppendStatusMessage(combat::Text::ActionText(turnCreaturePtr_, turnActionInfo_, fightResult_, true, true), false);
                SetAnimPhase(AnimPhase::AdvanceOrRetreat);
                break;
            }

            case TurnActionPhase::MeleeWeapon:
            {
                AppendStatusMessage(combat::Text::ActionText(turnCreaturePtr_, turnActionInfo_, fightResult_, true, true), false);
                SetAnimPhase(AnimPhase::MoveToward);

                auto const CREATURE_EFFECTS_VEC{ fightResult_.Effects() };
                M_ASSERT_OR_LOGANDTHROW_SS((CREATURE_EFFECTS_VEC.size() == 1), "game::stage::CombatStage::StartPerformAnim(turnActionPhase_=MeleeWeapon) found the fightResult.Effects().size=" << CREATURE_EFFECTS_VEC.size() << " instead of 1.");
                combatAnimationPtr_->MeleeMoveTowardAnimStart(turnCreaturePtr_, CREATURE_EFFECTS_VEC[0].GetCreature());
                slider_.Reset(ANIM_MELEE_MOVE_SLIDER_SPEED_);
                break;
            }

            case TurnActionPhase::ShootSling:
            case TurnActionPhase::ShootArrow:
            case TurnActionPhase::ShootBlowpipe:
            {
                //Note:  There should only be one CreatureEffect and one HitInfoVec when attacking with a projectile
                combat::HitInfo hitInfo;
                if (fightResult_.GetHitInfo(hitInfo))
                {
                    //at this point we are guaranteed fightResult_ contains at least one CreatureEffect and one HitInfo
                    auto const WEAPON_PTR{ hitInfo.Weapon() };
                    if (WEAPON_PTR != nullptr)
                    {
                        combatSoundEffects_.PlayShoot(WEAPON_PTR);
                        combatAnimationPtr_->ProjectileShootAnimStart(turnCreaturePtr_,
                                                                      fightResult_.Effects()[0].GetCreature(),
                                                                      WEAPON_PTR,
                                                                      fightResult_.WasHit());

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
                AppendStatusMessage(combat::Text::ActionText(turnCreaturePtr_, turnActionInfo_, fightResult_, true, true), false);
                SetTurnPhase(TurnPhase::PerformReport);
                SetAnimPhase(AnimPhase::NotAnimating);
                SetupTurnBox();
                StartPause(PERFORM_PRE_REPORT_PAUSE_SEC_, "PerformPreReport (PostCast)");
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
            case TurnPhase::ConditionWakePause:         { return "ConditionWakePause"; }
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
            default:                            { return ""; }
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
            case AnimPhase::MoveBack:            { return "MoveBack"; }
            case AnimPhase::FinalPause:          { return "FinalPause"; }
            case AnimPhase::Count:
            default:                                    { return ""; }
        }
    }


    void CombatStage::UpdateTestingText()
    {
        std::ostringstream ss;
        ss << ((PreTurnPhase::End == preTurnPhase_) ? TurnPhaseToString(turnPhase_) : PreTurnPhaseToString(preTurnPhase_))
            << ", " << pauseTitle_ << " " << ((IsPaused()) ? "D" : "A")
            << ", " << TurnActionPhaseToString(turnActionPhase_)
            << ", " << AnimPhaseToString(animPhase_);
        testingTextRegionSPtr_->SetText(ss.str());
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
        //Note: It is not yet possible for an attack to target multiple creatures
        combat::HitInfo hitInfo;
        if (FIGHT_RESULT.GetHitInfo(hitInfo))
        {
            auto const WEAPON_PTR{ hitInfo.Weapon() };
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
        //create a vec of all creatures killed this turn
        creature::CreaturePVec_t killedCreaturesPVec;
        auto const CREATURE_EFFECTS{ fightResult_.Effects() };
        for (auto const & NEXT_CREATURE_EFFECT : CREATURE_EFFECTS)
            if (NEXT_CREATURE_EFFECT.WasKill())
                killedCreaturesPVec.push_back(NEXT_CREATURE_EFFECT.GetCreature());

        for (auto nextKilledCreaturePtr : killedCreaturesPVec)
        {
            //handle other misc killed tasks
            if (nextKilledCreaturePtr->IsPlayerCharacter() == false)
            {
                combat::Encounter::Instance()->HandleKilledCreature(nextKilledCreaturePtr);
            }
        }
    }


    void CombatStage::HandleApplyDamageTasks()
    {
        //create a vec of all creatures killed this turn
        creature::CreaturePVec_t killedCreaturesPVec;
        auto const CREATURE_EFFECTS{ fightResult_.Effects() };
        for (auto const & NEXT_CREATURE_EFFECT : CREATURE_EFFECTS)
            if (NEXT_CREATURE_EFFECT.WasKill())
                killedCreaturesPVec.push_back(NEXT_CREATURE_EFFECT.GetCreature());

        //remove all conditions except for stone and dead from the killed creature
        for (auto nextKilledCreaturePtr : killedCreaturesPVec)
        {
            auto const CONDITIONS_VEC{ nextKilledCreaturePtr->Conditions() };
            for (auto const NEXT_CONDITION_ENUM : CONDITIONS_VEC)
            {
                if ((NEXT_CONDITION_ENUM != creature::Conditions::Stone) &&
                    (NEXT_CONDITION_ENUM != creature::Conditions::Dead))
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

        turnActionInfo_ = combat::TurnActionInfo(combat::TurnAction::Attack, creatureToAttackPtr);
        combat::Encounter::Instance()->SetTurnActionInfo(turnCreaturePtr_, turnActionInfo_);
        fightResult_ = combat::FightClub::Fight(turnCreaturePtr_, creatureToAttackPtr);
        SetTurnActionPhase(GetTurnActionPhaseFromFightResult(fightResult_));

        SetTurnPhase(TurnPhase::CenterAndZoomOut);

        combatAnimationPtr_->CenteringStart(creature::CreaturePVec_t{ turnCreaturePtr_, creatureToAttackPtr });
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


    const std::string CombatStage::RemoveSingleTurnTemporaryConditions()
    {
        if (turnCreaturePtr_->HasCondition(creature::Conditions::Unconscious))
        {
            return "";
        }

        if (turnCreaturePtr_->HasCondition(creature::Conditions::Tripped))
        {
            turnCreaturePtr_->ConditionRemove(creature::Conditions::Tripped);
            return "gets up after being tripped";
        }

        if ((turnCreaturePtr_->HasCondition(creature::Conditions::Dazed)) &&
            (misc::random::Int(100) < 75))
        {
            turnCreaturePtr_->ConditionRemove(creature::Conditions::Dazed);
            return "wakes from being dazed";
        }

        if ((turnCreaturePtr_->HasCondition(creature::Conditions::Frightened)) &&
            (misc::random::Int(100) < (20 + static_cast<int>(turnCreaturePtr_->Rank()))))
        {
            turnCreaturePtr_->ConditionRemove(creature::Conditions::Frightened);
            return "recovers from being frightened";
        }

        return "";
    }

}
}
