//
// combat-stage.cpp
//
#include "combat-stage.hpp"

#include "sfml-util/sfml-util.hpp"
#include "sfml-util/real-utils.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/tile.hpp"
#include "sfml-util/random.hpp"
#include "sfml-util/gui/box.hpp"
#include "sfml-util/gui/popup-manager.hpp"
#include "sfml-util/gui/list-box-item.hpp"

#include "heroespath/log-macros.hpp"
#include "heroespath/game.hpp"
#include "heroespath/game-data-file.hpp"
#include "heroespath/loop-manager.hpp"
#include "heroespath/player/party.hpp"
#include "heroespath/player/initial.hpp"
#include "heroespath/player/character.hpp"
#include "heroespath/non-player/party.hpp"
#include "heroespath/non-player/character.hpp"
#include "heroespath/state/game-state.hpp"
#include "heroespath/state/game-state-factory.hpp"
#include "heroespath/combat/encounter.hpp"
#include "heroespath/combat/combat-display.hpp"
#include "heroespath/combat/turn-decider.hpp"
#include "heroespath/combat/combat-node.hpp"
#include "heroespath/combat/fight.hpp"
#include "heroespath/creature/name-info.hpp"
#include "heroespath/creature/conditions.hpp"
#include "heroespath/creature/titles.hpp"
#include "heroespath/creature/algorithms.hpp"
#include "heroespath/item/weapon-factory.hpp"
#include "heroespath/item/algorithms.hpp"
#include "heroespath/combat/combat-text.hpp"

#include <sstream>
#include <exception>
#include <vector>
#include <string>


namespace heroespath
{
namespace stage
{

    SavedCombatInfo::SavedCombatInfo()
    :
        canTurnAdvance_       (false),
        hasRestored_          (false),
        creaturesFlyingPVec_  ()
    {}


    void SavedCombatInfo::PrepareForStageChange(const combat::CombatDisplayPtrC_t COMBAT_DISPLAY_CPTRC)
    {
        CanTurnAdvance(false);
        Save(COMBAT_DISPLAY_CPTRC);
    }


    void SavedCombatInfo::Save(const combat::CombatDisplayPtrC_t COMBAT_DISPLAY_CPTRC)
    {
        FlyingCreaturesSave(COMBAT_DISPLAY_CPTRC);
    }


    void SavedCombatInfo::Restore(combat::CombatDisplayPtrC_t combatDisplayPtr)
    {
        hasRestored_ = true;
        FlyingCreaturesRestore(combatDisplayPtr);
    }


    void SavedCombatInfo::FlyingCreaturesSave(const combat::CombatDisplayPtrC_t COMBAT_DISPLAY_CPTRC)
    {
        creaturesFlyingPVec_.clear();
        combat::CombatNodeSVec_t combatNodesSVec;
        COMBAT_DISPLAY_CPTRC->GetCombatNodes(combatNodesSVec);

        for (auto const & NEXT_COMBATNODE_SPTR : combatNodesSVec)
            if (NEXT_COMBATNODE_SPTR->GetIsFlying())
                creaturesFlyingPVec_.push_back(NEXT_COMBATNODE_SPTR->Creature().get());
    }


    void SavedCombatInfo::FlyingCreaturesRestore(combat::CombatDisplayPtrC_t combatDisplayPtr)
    {
        for (auto const nextFlyingCreaturePtr : creaturesFlyingPVec_)
            combatDisplayPtr->HandleFlyingChange(nextFlyingCreaturePtr, true);
    }


    const float CombatStage::ZOOM_SLIDER_SPEED_             (3.0f);
    const float CombatStage::POST_PAN_PAUSE_SEC_            (1.5f);
    const float CombatStage::POST_ZOOMOUT_PAUSE_SEC_        (1.5f);
    const float CombatStage::POST_ZOOMIN_PAUSE_SEC_         (1.0f);
    const float CombatStage::CREATURE_TURN_DELAY_SEC_       (6.0f);
    const float CombatStage::PRE_TURN_DELAY_SEC_            (2.0f);
    const float CombatStage::POST_TURN_DELAY_SEC_           (4.0f);
    const float CombatStage::PERFORM_TURN_DELAY_SEC_        (3.0f);
    const float CombatStage::PERFORM_REPORT_DELAY_SEC_      (6.0f);
    const float CombatStage::POST_ZOOM_TURN_DELAY_SEC_      (3.0f);
    const float CombatStage::CENTERING_SLIDER_SPEED_        (4.0f);
    const float CombatStage::INITIAL_CENTERING_SLIDER_SPEED_(1.0f);
    const float CombatStage::TEXT_COLOR_SHAKER_SPEED_       (35.0f);
    const float CombatStage::CREATURE_POS_SLIDER_SPEED_     (4.0f);
    const float CombatStage::STATUSMSG_ANIM_DURATION_SEC_   (2.0f);
    //
    const sf::Color CombatStage::LISTBOX_BACKGROUND_COLOR_   ((sfml_util::FontManager::Instance()->Color_Orange() - sf::Color(100, 100, 100, 235)));
    const sf::Color CombatStage::LISTBOX_HIGHLIGHT_COLOR_    ((sfml_util::FontManager::Instance()->Color_Orange() - sf::Color(100, 100, 100, 235)) + sf::Color(20, 20, 20, 20));
    const sf::Color CombatStage::LISTBOX_HIGHLIGHT_ALT_COLOR_((sfml_util::FontManager::Instance()->Color_Orange() - sf::Color(100, 100, 100, 235)) + sf::Color(40, 40, 40, 40));
    const sf::Color CombatStage::LISTBOX_SELECTED_COLOR_     (sf::Color::White);
    const sf::Color CombatStage::LISTBOX_NOTSELECTED_COLOR_  (sf::Color(150, 150, 150));
    const sf::Color CombatStage::LISTBOX_LINE_COLOR_         (sf::Color(255, 255, 255, 25));
    //
    SavedCombatInfo CombatStage::savedCombatInfo_;


    CombatStage::CombatStage()
    :
        Stage                      ("Combat"),
        SCREEN_WIDTH_              (sfml_util::Display::Instance()->GetWinWidth()),
        SCREEN_HEIGHT_             (sfml_util::Display::Instance()->GetWinHeight()),
        bottomSymbol_              (),
        commandBoxSPtr_            (),
        statusBoxSPtr_             (),
        statusBoxTextInfo_         ("", sfml_util::FontManager::Instance()->Font_Typical(), sfml_util::FontManager::Instance()->Size_Small(), sfml_util::FontManager::Instance()->Color_Orange(), sfml_util::Justified::Left),
        zoomSliderBarSPtr_         (),
        turnBoxSPtr_               (),
        turnBoxRegion_             (),
        isMouseHeldDownAndValid_   (false),
        isMouseDragging_           (false),
        mousePrevPosV_             (0.0f, 0.0f),
        encounterSPtr_             (combat::Encounter::Instance()),
        combatSoundEffects_        (),
        turnPhase_                 (TurnPhase::NotATurn),
        preTurnPhase_              (PreTurnPhase::Start),
        performType_               (PerformType::None),
        performReportEffectIndex_  (0),
        performReportHitIndex_     (0),
        combatDisplayPtrC_         (new combat::CombatDisplay()),
        settingsButtonSPtr_        (new sfml_util::gui::FourStateButton("CombatStage'sSettingsGears",
                                                                        0.0f,
                                                                        0.0f,
                                                                        std::string(GameDataFile::Instance()->GetMediaPath("media-images-buttons-gui")).append("gears1_normal.png"),
                                                                        "",
                                                                        std::string(GameDataFile::Instance()->GetMediaPath("media-images-buttons-gui")).append("gears1_lit.png"),
                                                                        "",
                                                                        sfml_util::gui::TextInfo(),
                                                                        sfml_util::gui::TextInfo(),
                                                                        false,
                                                                        sfml_util::MapByRes(0.6f, 2.0f),
                                                                        false)),
        isAttackAnimZoomOut_        (false),
        zoomSlider_                 (ZOOM_SLIDER_SPEED_),
        isPlayerActionAllowed_      (true),
        pauseDurationSec_           (0.0f),
        pauseElapsedSec_            (pauseDurationSec_ + 1.0f),//anything greater than pauseTimeDurationSecs_ will work here
        isPauseCanceled_            (false),
        centeringSlider_            (CENTERING_SLIDER_SPEED_),
        turnCreaturePtr_            (nullptr),
        opponentCreature_           (nullptr),
        goldTextColorShaker_        (sfml_util::FontManager::Color_Orange(), sf::Color::White, TEXT_COLOR_SHAKER_SPEED_),
        redTextColorShaker_         (sf::Color(255, 127, 127), sf::Color::White, TEXT_COLOR_SHAKER_SPEED_ * 0.65f),
        turnStateToFinish_          (combat::TurnAction::Count),
        turnActionInfo_             (),
        fightResult_                (),
        titleTBoxTextRegionSPtr_    (),
        weaponTBoxTextRegionSPtr_   (),
        armorTBoxTextRegionSPtr_    (),
        infoTBoxTextRegionSPtr_     (),
        enemyActionTBoxRegionSPtr_  (),
        enemyCondsTBoxRegionSPtr_   (),
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
        isCreaturePosSliding_       (false),
        creaturePosSlider_          (CREATURE_POS_SLIDER_SPEED_),
        statusMsgAnimTimerSec_      (STATUSMSG_ANIM_DURATION_SEC_ + 1.0f), //anything greater than STATUSMSG_ANIM_DURATION_SEC_ will work here
        statusMsgAnimColorShaker_   (LISTBOX_HIGHLIGHT_COLOR_, LISTBOX_HIGHLIGHT_ALT_COLOR_, 35.0f, false),
        creatureAttackingCPtr_      (nullptr),
        creatureAttackedCPtr_       (nullptr),
        testingTextRegionSPtr_      (),
        pauseTitle_                 ("")
    {
        savedCombatInfo_.CanTurnAdvance(false);
    }


    CombatStage::~CombatStage()
    {}


    bool CombatStage::HandleCallback(const sfml_util::gui::callback::ListBoxEventPackage &)
    {
        return false;
    }


    bool CombatStage::HandleCallback(const sfml_util::gui::callback::FourStateButtonCallbackPackage_t & PACKAGE)
    {
        if ((IsPlayerCharacterTurnValid() == false) || (TurnPhase::DetermineAction != turnPhase_))
            return false;

        if (PACKAGE.PTR_ == attackTBoxButtonSPtr_.get())
            return HandleAttack();

        if (PACKAGE.PTR_ == fightTBoxButtonSPtr_.get())
            return HandleFight();

        if (PACKAGE.PTR_ == castTBoxButtonSPtr_.get())
            return HandleCast();

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
                isSkyPounce = encounterSPtr_->GetTurnInfoCopy(turnCreaturePtr_).GetIsFlying();

            return HandlePounce(isSkyPounce);
        }

        if (PACKAGE.PTR_ == settingsButtonSPtr_.get())
        {
            savedCombatInfo_.PrepareForStageChange(combatDisplayPtrC_);
            heroespath::LoopManager::Instance()->Goto_Settings();
            return true;
        }

        return false;
    }


    bool CombatStage::HandleCallback(const sfml_util::gui::callback::SliderBarCallbackPackage_t & PACKAGE)
    {
        if ((PACKAGE.PTR_ == zoomSliderBarSPtr_.get()) && (combatDisplayPtrC_ != nullptr))
        {
            combatDisplayPtrC_->SetZoomLevel(PACKAGE.PTR_->GetCurrentValue());
            return true;
        }
        else
            return false;
    }


    bool CombatStage::HandleCallback(const heroespath::callback::PopupResponse &)
    {
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
        EntityAdd(statusBoxSPtr_);

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

        //command region background symbol
        bottomSymbol_.Setup(1.0f,
                            false,
                            SCREEN_WIDTH_,
                            SCREEN_HEIGHT_,
                            COMMAND_REGION_LEFT + COMMAND_REGION_WIDTH + COMMAND_REGION_HORIZ_SPACER,
                            (COMMAND_REGION_TOP + (COMMAND_REGION_HEIGHT * 0.5f)) - (BottomSymbol::DEFAULT_VERT_POS_OFFSET_ * 0.5f),
                            sf::Color(255, 255, 255, 255));

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
                                                              sfml_util::FontManager::Instance()->Color_Light(),
                                                              sfml_util::Justified::Center);

        sf::FloatRect turnBoxTitleTextRegion(turnBoxRegion_);
        turnBoxTitleTextRegion.height = 0.0f;
        titleTBoxTextRegionSPtr_.reset( new sfml_util::gui::TextRegion("TurnBox'sTitle", TURNBOXTITLE_TEXT_INFO, turnBoxTitleTextRegion) );

        const sfml_util::gui::TextInfo TURNBOXINFO_TEXT_INFO(" ",
                                                             sfml_util::FontManager::Instance()->Font_Default2(),
                                                             sfml_util::FontManager::Instance()->Size_Smallish(),
                                                             sfml_util::FontManager::Instance()->Color_GrayLight(),
                                                             sfml_util::Justified::Left);

        sf::FloatRect turnBoxInfoTextRegion(turnBoxRegion_);
        infoTBoxTextRegionSPtr_.reset( new sfml_util::gui::TextRegion("CombatStage'sTurnInfo", TURNBOXINFO_TEXT_INFO, turnBoxInfoTextRegion) );

        const sfml_util::gui::TextInfo TURNBOXENEMYACTION_TEXT_INFO(" ",
                                                                    sfml_util::FontManager::Instance()->Font_Default2(),
                                                                    sfml_util::FontManager::Instance()->Size_Normal(),
                                                                    sfml_util::FontManager::Instance()->Color_Light(),
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
                                                                   sfml_util::FontManager::Instance()->Color_GrayLight(),
                                                                   sfml_util::Justified::Center);

        sf::FloatRect turnBoxEnemyCondsTextRegion(turnBoxRegion_);
        enemyCondsTBoxRegionSPtr_.reset( new sfml_util::gui::TextRegion("CombatStage'sTurnEnemyConds", TURNBOXENEMYCONDS_TEXT_INFO, turnBoxEnemyCondsTextRegion) );

        sfml_util::gui::TextInfo turnBoxWeaponTextInfo(" ",
                                                       sfml_util::FontManager::Instance()->Font_Default2(),
                                                       sfml_util::FontManager::Instance()->Size_Smallish(),
                                                       sfml_util::FontManager::Instance()->Color_GrayLight(),
                                                       sfml_util::Justified::Center);

        weaponTBoxTextRegionSPtr_.reset( new sfml_util::gui::TextRegion("TurnBox'sWeapon", turnBoxWeaponTextInfo, turnBoxTitleTextRegion) );

        sfml_util::gui::TextInfo turnBoxArmorTextInfo(turnBoxWeaponTextInfo);

        armorTBoxTextRegionSPtr_.reset(new sfml_util::gui::TextRegion("TurnBox'sArmor", turnBoxWeaponTextInfo, turnBoxTitleTextRegion));

        sfml_util::gui::TextInfo turnButtonTextInfo(" ",
                                                    sfml_util::FontManager::Instance()->Font_Default2(),
                                                    sfml_util::FontManager::Instance()->Size_Largeish(),
                                                    sfml_util::FontManager::Instance()->Color_Orange(),
                                                    sfml_util::Justified::Left);

        const sfml_util::gui::TextInfo TESTING_TEXT_INFO(" ",
                                                         sfml_util::FontManager::Instance()->Font_Default2(),
                                                         sfml_util::FontManager::Instance()->Size_Smallish(),
                                                         sfml_util::FontManager::Instance()->Color_GrayLight(),
                                                         sfml_util::Justified::Left);

        sf::FloatRect testingTextRegion(turnBoxRegion_);
        testingTextRegion.left += testingTextRegion.width + 65.0f;
        testingTextRegion.top += (testingTextRegion.height - 35.0f);
        testingTextRegionSPtr_.reset( new sfml_util::gui::TextRegion("CombatStage'sTesting", TESTING_TEXT_INFO, testingTextRegion) );

        const sf::Color TURNBUTTON_DISABLED_COLOR(sfml_util::FontManager::Instance()->Color_Orange() - sf::Color(0, 0, 0, 176));
        sfml_util::gui::TextInfo turnButtonTextInfoDisabled(turnButtonTextInfo);
        turnButtonTextInfoDisabled.color = TURNBUTTON_DISABLED_COLOR;

        turnButtonTextInfo.text = "(A)ttack";
        turnButtonTextInfoDisabled.text = "(A)ttack";
        const sfml_util::gui::MouseTextInfo ATTACKBUTTON_MOUSETEXTINFO(turnButtonTextInfo, sfml_util::FontManager::Color_Light(), sf::Color::White);
        attackTBoxButtonSPtr_.reset( new sfml_util::gui::FourStateButton("CombatStage'sAttack", 0.0f, 0.0f, "", "", "", "", ATTACKBUTTON_MOUSETEXTINFO, turnButtonTextInfoDisabled) );
        attackTBoxButtonSPtr_->SetCallbackHandler(this);
        attackTBoxButtonSPtr_->SetMouseHoverText(combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_ATTACK_);
        attackTBoxButtonSPtr_->MoveEntityOffScreen();
        EntityAdd(attackTBoxButtonSPtr_);

        turnButtonTextInfo.text = "(F)ight";
        turnButtonTextInfoDisabled.text = "(F)ight";
        const sfml_util::gui::MouseTextInfo FIGHTBUTTON_MOUSETEXTINFO(turnButtonTextInfo, sfml_util::FontManager::Color_Light(), sf::Color::White);
        fightTBoxButtonSPtr_.reset( new sfml_util::gui::FourStateButton("CombatStage'sAttack", 0.0f, 0.0f, "", "", "", "", FIGHTBUTTON_MOUSETEXTINFO, turnButtonTextInfoDisabled) );
        fightTBoxButtonSPtr_->SetCallbackHandler(this);
        fightTBoxButtonSPtr_->SetMouseHoverText(combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_FIGHT_);
        fightTBoxButtonSPtr_->MoveEntityOffScreen();
        EntityAdd(fightTBoxButtonSPtr_);

        turnButtonTextInfo.text = "(C)ast";
        turnButtonTextInfoDisabled.text = "(C)ast";
        const sfml_util::gui::MouseTextInfo CASTBUTTON_MOUSETEXTINFO(turnButtonTextInfo, sfml_util::FontManager::Color_Light(), sf::Color::White);
        castTBoxButtonSPtr_.reset( new sfml_util::gui::FourStateButton("CombatStage'sCast", 0.0f, 0.0f, "", "", "", "", CASTBUTTON_MOUSETEXTINFO, turnButtonTextInfoDisabled) );
        castTBoxButtonSPtr_->SetCallbackHandler(this);
        castTBoxButtonSPtr_->SetMouseHoverText(combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_CAST_);
        castTBoxButtonSPtr_->MoveEntityOffScreen();
        EntityAdd(castTBoxButtonSPtr_);

        turnButtonTextInfo.text = "Advance";
        turnButtonTextInfoDisabled.text = "Advance";
        const sfml_util::gui::MouseTextInfo ADVANCEBUTTON_MOUSETEXTINFO(turnButtonTextInfo, sfml_util::FontManager::Color_Light(), sf::Color::White);
        advanceTBoxButtonSPtr_.reset( new sfml_util::gui::FourStateButton("CombatStage'sAdvance", 0.0f, 0.0f, "", "", "", "", ADVANCEBUTTON_MOUSETEXTINFO, turnButtonTextInfoDisabled) );
        advanceTBoxButtonSPtr_->SetCallbackHandler(this);
        advanceTBoxButtonSPtr_->SetMouseHoverText(combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_ADVANCE_);
        advanceTBoxButtonSPtr_->MoveEntityOffScreen();
        EntityAdd(advanceTBoxButtonSPtr_);

        turnButtonTextInfo.text = "Retreat";
        turnButtonTextInfoDisabled.text = "Retreat";
        const sfml_util::gui::MouseTextInfo RETREATBUTTON_MOUSETEXTINFO(turnButtonTextInfo, sfml_util::FontManager::Color_Light(), sf::Color::White);
        retreatTBoxButtonSPtr_.reset( new sfml_util::gui::FourStateButton("CombatStage'sRetreat", 0.0f, 0.0f, "", "", "", "", RETREATBUTTON_MOUSETEXTINFO, turnButtonTextInfoDisabled) );
        retreatTBoxButtonSPtr_->SetCallbackHandler(this);
        retreatTBoxButtonSPtr_->SetMouseHoverText(combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_RETREAT_);
        retreatTBoxButtonSPtr_->MoveEntityOffScreen();
        EntityAdd(retreatTBoxButtonSPtr_);

        turnButtonTextInfo.text = "(B)lock";
        turnButtonTextInfoDisabled.text = "(B)lock";
        const sfml_util::gui::MouseTextInfo BLOCKBUTTON_MOUSETEXTINFO(turnButtonTextInfo, sfml_util::FontManager::Color_Light(), sf::Color::White);
        blockTBoxButtonSPtr_.reset( new sfml_util::gui::FourStateButton("CombatStage'sBlock", 0.0f, 0.0f, "", "", "", "", BLOCKBUTTON_MOUSETEXTINFO, turnButtonTextInfoDisabled) );
        blockTBoxButtonSPtr_->SetCallbackHandler(this);
        blockTBoxButtonSPtr_->SetMouseHoverText(combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_BLOCK_);
        blockTBoxButtonSPtr_->MoveEntityOffScreen();
        EntityAdd(blockTBoxButtonSPtr_);

        turnButtonTextInfo.text = "(S)kip";
        turnButtonTextInfoDisabled.text = "(S)kip";
        const sfml_util::gui::MouseTextInfo SKIPBUTTON_MOUSETEXTINFO(turnButtonTextInfo, sfml_util::FontManager::Color_Light(), sf::Color::White);
        skipTBoxButtonSPtr_.reset(new sfml_util::gui::FourStateButton("CombatStage'sSkip", 0.0f, 0.0f, "", "", "", "", SKIPBUTTON_MOUSETEXTINFO, turnButtonTextInfoDisabled));
        skipTBoxButtonSPtr_->SetCallbackHandler(this);
        skipTBoxButtonSPtr_->SetMouseHoverText(combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_SKIP_);
        skipTBoxButtonSPtr_->MoveEntityOffScreen();
        EntityAdd(skipTBoxButtonSPtr_);

        turnButtonTextInfo.text = "Fl(y)";
        turnButtonTextInfoDisabled.text = "Fl(y)";
        const sfml_util::gui::MouseTextInfo FLYBUTTON_MOUSETEXTINFO(turnButtonTextInfo, sfml_util::FontManager::Color_Light(), sf::Color::White);
        flyTBoxButtonSPtr_.reset(new sfml_util::gui::FourStateButton("CombatStage'sFly", 0.0f, 0.0f, "", "", "", "", FLYBUTTON_MOUSETEXTINFO, turnButtonTextInfoDisabled));
        flyTBoxButtonSPtr_->SetCallbackHandler(this);
        flyTBoxButtonSPtr_->SetMouseHoverText(combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_FLY_);
        flyTBoxButtonSPtr_->MoveEntityOffScreen();
        EntityAdd(flyTBoxButtonSPtr_);

        turnButtonTextInfo.text = "(L)and";
        turnButtonTextInfoDisabled.text = "(L)and";
        const sfml_util::gui::MouseTextInfo LANDBUTTON_MOUSETEXTINFO(turnButtonTextInfo, sfml_util::FontManager::Color_Light(), sf::Color::White);
        landTBoxButtonSPtr_.reset(new sfml_util::gui::FourStateButton("CombatStage'sLand", 0.0f, 0.0f, "", "", "", "", LANDBUTTON_MOUSETEXTINFO, turnButtonTextInfoDisabled));
        landTBoxButtonSPtr_->SetCallbackHandler(this);
        landTBoxButtonSPtr_->SetMouseHoverText(combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_LAND_);
        landTBoxButtonSPtr_->MoveEntityOffScreen();
        EntityAdd(landTBoxButtonSPtr_);

        turnButtonTextInfo.text = "(R)oar";
        turnButtonTextInfoDisabled.text = "(R)oar";
        const sfml_util::gui::MouseTextInfo ROARBUTTON_MOUSETEXTINFO(turnButtonTextInfo, sfml_util::FontManager::Color_Light(), sf::Color::White);
        roarTBoxButtonSPtr_.reset(new sfml_util::gui::FourStateButton("CombatStage'sRoar", 0.0f, 0.0f, "", "", "", "", ROARBUTTON_MOUSETEXTINFO, turnButtonTextInfoDisabled));
        roarTBoxButtonSPtr_->SetCallbackHandler(this);
        roarTBoxButtonSPtr_->SetMouseHoverText(combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_ROAR_);
        roarTBoxButtonSPtr_->MoveEntityOffScreen();
        EntityAdd(roarTBoxButtonSPtr_);

        turnButtonTextInfo.text = "(P)ounce";
        turnButtonTextInfoDisabled.text = "(P)ounce";
        const sfml_util::gui::MouseTextInfo POUNCEBUTTON_MOUSETEXTINFO(turnButtonTextInfo, sfml_util::FontManager::Color_Light(), sf::Color::White);
        pounceTBoxButtonSPtr_.reset(new sfml_util::gui::FourStateButton("CombatStage'sPounce", 0.0f, 0.0f, "", "", "", "", POUNCEBUTTON_MOUSETEXTINFO, turnButtonTextInfoDisabled));
        pounceTBoxButtonSPtr_->SetCallbackHandler(this);
        pounceTBoxButtonSPtr_->SetMouseHoverText(combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_POUNCE_);
        pounceTBoxButtonSPtr_->MoveEntityOffScreen();
        EntityAdd(pounceTBoxButtonSPtr_);

        //settings button (gears symbol)
        const float COMMAND_REGION_PAD(10.0f);
        settingsButtonSPtr_->SetEntityPos(COMMAND_REGION_LEFT + COMMAND_REGION_WIDTH - settingsButtonSPtr_->GetEntityRegion().width - COMMAND_REGION_PAD, COMMAND_REGION_TOP + COMMAND_REGION_PAD);
        settingsButtonSPtr_->SetCallbackHandler(this);
        EntityAdd(settingsButtonSPtr_);

        //TODO TEMP REMOVE
        //fake player characters until loading games starts working
        player::PartySPtr_t partySPtr(new player::Party());
        /*
        {
            const stats::StatSet KNIGHT_STATS(20 + sfml_util::rand::Int(10),
                                              15 + sfml_util::rand::Int(6),
                                              0  + sfml_util::rand::Int(6),
                                              5  + sfml_util::rand::Int(10),
                                              15 + sfml_util::rand::Int(10),
                                              0  + sfml_util::rand::Int(8));

            const std::string KNIGHT_NAME( boost::algorithm::replace_last_copy(creature::NameInfo::Instance()->LargestName(), creature::NameInfo::Instance()->LargestLetterString(), "K") );
            auto knightSPtr( new player::Character(KNIGHT_NAME,
                                                   creature::sex::Male,
                                                   creature::BodyType::Make_Humanoid(),
                                                   creature::race::Human,
                                                   creature::role::Knight,
                                                   KNIGHT_STATS));

            player::Initial::Setup(knightSPtr.get());

            creature::ConditionSPtr_t iConditionDazedSPtr( new heroespath::creature::condition::Cond_Dazed("Um...from somewhere, or somehow, whichever makes sense") );
            knightSPtr->ConditionAdd(iConditionDazedSPtr);
            creature::ConditionSPtr_t iConditionTrippedSPtr( new heroespath::creature::condition::Cond_Tripped("Um...by some creature") );
            knightSPtr->ConditionAdd(iConditionTrippedSPtr);

            creature::TitleSPtr_t titleKnightOfTheRoseSPtr( new creature::title::Title_KnightOfTheRose() );
            knightSPtr->TitleAdd(titleKnightOfTheRoseSPtr);

            knightSPtr->CoinsAdj(100);
            knightSPtr->GemsAdj(3);
            knightSPtr->MeteorShardsAdj(3);
            partySPtr->Add(knightSPtr);
        }
        */

        {
            const stats::StatSet FIREBRAND_STATS(20 + sfml_util::rand::Int(10),
                                                 15 + sfml_util::rand::Int(10),
                                                 0  + sfml_util::rand::Int(6),
                                                 5  + sfml_util::rand::Int(10),
                                                 0  + sfml_util::rand::Int(10),
                                                 10 + sfml_util::rand::Int(8));

            const std::string FIREBRAND_NAME( boost::algorithm::replace_last_copy(creature::NameInfo::Instance()->LargestName(), creature::NameInfo::Instance()->LargestLetterString(), "F") );

            auto firebrandSPtr = std::make_shared<player::Character>(FIREBRAND_NAME,
                                                                     creature::sex::Male,
                                                                     creature::BodyType::Make_Dragon(),
                                                                     creature::race::Dragon,
                                                                     creature::role::Firebrand,
                                                                     FIREBRAND_STATS);

            const stats::StatSet STATS_MOD(sfml_util::rand::Int(2) * ((sfml_util::rand::Bool()) ? -1 : 1),
                                           sfml_util::rand::Int(3) * ((sfml_util::rand::Bool()) ? -1 : 1),
                                           sfml_util::rand::Int(4) * ((sfml_util::rand::Bool()) ? -1 : 1),
                                           sfml_util::rand::Int(5) * ((sfml_util::rand::Bool()) ? -1 : 1),
                                           sfml_util::rand::Int(6) * ((sfml_util::rand::Bool()) ? -1 : 1),
                                           sfml_util::rand::Int(7) * ((sfml_util::rand::Bool()) ? -1 : 1));

            firebrandSPtr->Stats().ModifyCurrent(STATS_MOD);

            player::Initial::Setup(firebrandSPtr.get());

            /* TODO TEMP FIX
            firebrandSPtr->TitleAdd(creature::title::TitleWarehouse::NIMBLEFOOT_SPTR);
            creature::IConditionSPtr_t iConditionDazedSPtr( new heroespath::creature::condition::Cond_Dazed("Um...from somewhere, or somehow, whichever makes sense") );
            firebrandSPtr->ConditionAdd(iConditionDazedSPtr);
            creature::IConditionSPtr_t iConditionTrippedSPtr( new heroespath::creature::condition::Cond_Tripped("Um...by some creature") );
            firebrandSPtr->ConditionAdd(iConditionTrippedSPtr);
            */

            partySPtr->Add(firebrandSPtr);
        }

        /*
        {
            const stats::StatSet ARCHER_STATS(15 + sfml_util::rand::Int(10),
                                              20 + sfml_util::rand::Int(10),
                                              5  + sfml_util::rand::Int(6),
                                              10 + sfml_util::rand::Int(10),
                                              10 + sfml_util::rand::Int(8),
                                              5  + sfml_util::rand::Int(6));

            const std::string ARCHER_NAME(boost::algorithm::replace_last_copy(creature::NameInfo::Instance()->LargestName(), creature::NameInfo::Instance()->LargestLetterString(), "A"));
            player::CharacterSPtr_t archerSPtr(new player::Character(ARCHER_NAME,
                                                                     creature::sex::Female,
                                                                     creature::BodyType::Make_Humanoid(),
                                                                     creature::race::Human,
                                                                     creature::role::Archer,
                                                                     ARCHER_STATS));

            player::Initial::Setup(archerSPtr.get());

            archerSPtr->CoinsAdj(100);
            archerSPtr->GemsAdj(3);
            archerSPtr->MeteorShardsAdj(3);

            partySPtr->Add(archerSPtr);
        }
        */

        {
            const stats::StatSet WOLFEN_STATS(20 + sfml_util::rand::Int(10),
                                              20 + sfml_util::rand::Int(10),
                                              5  + sfml_util::rand::Int(6),
                                              5  + sfml_util::rand::Int(10),
                                              5  + sfml_util::rand::Int(8),
                                              5  + sfml_util::rand::Int(6));

            const std::string WOLFEN_NAME(boost::algorithm::replace_last_copy(creature::NameInfo::Instance()->LargestName(), creature::NameInfo::Instance()->LargestLetterString(), "W"));
            player::CharacterSPtr_t wolfenSPtr(new player::Character(WOLFEN_NAME,
                                                                     creature::sex::Female,
                                                                     creature::BodyType::Make_Wolfen(),
                                                                     creature::race::Wolfen,
                                                                     creature::role::Wolfen,
                                                                     WOLFEN_STATS));

            player::Initial::Setup(wolfenSPtr.get());
            partySPtr->Add(wolfenSPtr);
        }

        /*
        {
            const stats::StatSet BARD_STATS(10 + sfml_util::rand::Int(6),
                                            10 + sfml_util::rand::Int(6),
                                            10 + sfml_util::rand::Int(6),
                                            10 + sfml_util::rand::Int(6),
                                            10 + sfml_util::rand::Int(6),
                                            10 + sfml_util::rand::Int(6));

            const std::string BARD_NAME(boost::algorithm::replace_last_copy(creature::NameInfo::Instance()->LargestName(), creature::NameInfo::Instance()->LargestLetterString(), "B"));
            player::CharacterSPtr_t bardSPtr(new player::Character(BARD_NAME,
                                                                   creature::sex::Male,
                                                                   creature::BodyType::Make_Humanoid(),
                                                                   creature::race::Human,
                                                                   creature::role::Bard,
                                                                   BARD_STATS));

            player::Initial::Setup(bardSPtr.get());

            bardSPtr->CoinsAdj(100);
            bardSPtr->GemsAdj(3);
            bardSPtr->MeteorShardsAdj(3);

            partySPtr->Add(bardSPtr);
        }
        */

        {
            const stats::StatSet BEASTMASTER_STATS(10 + sfml_util::rand::Int(6),
                                                   10 + sfml_util::rand::Int(6),
                                                   10 + sfml_util::rand::Int(6),
                                                   10 + sfml_util::rand::Int(6),
                                                   10 + sfml_util::rand::Int(6),
                                                   10 + sfml_util::rand::Int(6));

            const std::string BEASTMASTER_NAME(boost::algorithm::replace_last_copy(creature::NameInfo::Instance()->LargestName(), creature::NameInfo::Instance()->LargestLetterString(), "G"));
            player::CharacterSPtr_t bmSPtr(new player::Character(BEASTMASTER_NAME,
                                                                 creature::sex::Male,
                                                                 creature::BodyType::Make_Humanoid(),
                                                                 creature::race::Human,
                                                                 creature::role::Beastmaster,
                                                                 BEASTMASTER_STATS));

            player::Initial::Setup(bmSPtr.get());

            bmSPtr->CoinsAdj(100);
            bmSPtr->GemsAdj(100);
            bmSPtr->MeteorShardsAdj(100);

            partySPtr->Add(bmSPtr);
        }

        {
            const stats::StatSet THEIF_STATS(5  + sfml_util::rand::Int(10),
                                             5  + sfml_util::rand::Int(10),
                                             5  + sfml_util::rand::Int(10),
                                             15 + sfml_util::rand::Int(15),
                                             15 + sfml_util::rand::Int(10),
                                             5  + sfml_util::rand::Int(8));

            const std::string THEIF_NAME(boost::algorithm::replace_last_copy(creature::NameInfo::Instance()->LargestName(), creature::NameInfo::Instance()->LargestLetterString(), "T"));
            player::CharacterSPtr_t thiefSPtr(new player::Character(THEIF_NAME,
                                                                    creature::sex::Male,
                                                                    creature::BodyType::Make_Humanoid(),
                                                                    creature::race::Gnome,
                                                                    creature::role::Thief,
                                                                    THEIF_STATS));

            player::Initial::Setup(thiefSPtr.get());

            thiefSPtr->CoinsAdj(100);
            thiefSPtr->GemsAdj(4);
            thiefSPtr->MeteorShardsAdj(4);

            partySPtr->Add(thiefSPtr);
        }

        {
            const stats::StatSet CLERIC_STATS(5  + sfml_util::rand::Int(8),
                                              5  + sfml_util::rand::Int(8),
                                              15 + sfml_util::rand::Int(10),
                                              10 + sfml_util::rand::Int(8),
                                              25 + sfml_util::rand::Int(8),
                                              10 + sfml_util::rand::Int(15));

            const std::string CLERIC_NAME(boost::algorithm::replace_last_copy(creature::NameInfo::Instance()->LargestName(), creature::NameInfo::Instance()->LargestLetterString(), "C"));
            player::CharacterSPtr_t clericSPtr(new player::Character(CLERIC_NAME,
                                                                     creature::sex::Female,
                                                                     creature::BodyType::Make_Pixie(),
                                                                     creature::race::Pixie,
                                                                     creature::role::Cleric,
                                                                     CLERIC_STATS));

            player::Initial::Setup(clericSPtr.get());

            clericSPtr->CoinsAdj(100);
            clericSPtr->GemsAdj(100);
            clericSPtr->MeteorShardsAdj(100);

            partySPtr->Add(clericSPtr);
        }

        /*
        {
            const stats::StatSet SORCERER_STATS(0  + sfml_util::rand::Int(8),
                                                0  + sfml_util::rand::Int(8),
                                                5  + sfml_util::rand::Int(8),
                                                10 + sfml_util::rand::Int(6),
                                                10 + sfml_util::rand::Int(6),
                                                20 + sfml_util::rand::Int(10));

            const std::string SORCERER_NAME(boost::algorithm::replace_last_copy(creature::NameInfo::Instance()->LargestName(), creature::NameInfo::Instance()->LargestLetterString(), "S"));
            player::CharacterSPtr_t sorcererSPtr(new player::Character(SORCERER_NAME,
                                                                       creature::sex::Male,
                                                                       creature::BodyType::Make_Pixie(),
                                                                       creature::race::Pixie,
                                                                       creature::role::Sorcerer,
                                                                       SORCERER_STATS));

            player::Initial::Setup(sorcererSPtr.get());

            sorcererSPtr->CoinsAdj(100);
            sorcererSPtr->GemsAdj(6);
            sorcererSPtr->MeteorShardsAdj(6);

            partySPtr->Add(sorcererSPtr);
        }
        */
        {
            const stats::StatSet SYLAVIN_STATS(20 + sfml_util::rand::Int(8),
                                               20 + sfml_util::rand::Int(8),
                                               5  + sfml_util::rand::Int(8),
                                               5  + sfml_util::rand::Int(6),
                                               50  + sfml_util::rand::Int(6),
                                               10 + sfml_util::rand::Int(10));

            const std::string SYLAVIN_NAME(boost::algorithm::replace_last_copy(creature::NameInfo::Instance()->LargestName(), creature::NameInfo::Instance()->LargestLetterString(), "S"));
            player::CharacterSPtr_t sylavinSPtr(new player::Character(SYLAVIN_NAME,
                                                                      creature::sex::Male,
                                                                      creature::BodyType::Make_Dragon(),
                                                                      creature::race::Dragon,
                                                                      creature::role::Sylavin,
                                                                      SYLAVIN_STATS));

            player::Initial::Setup(sylavinSPtr.get());

            partySPtr->Add(sylavinSPtr);
        }

        if (savedCombatInfo_.HasRestored() == false)
        {
            //TEMP TODO REMOVE create new game and player party object
            state::GameStateFactory::Instance()->NewGame(partySPtr);

            encounterSPtr_->StartTasks();

            //TODO TEMP REMOVE -test create that can't take action
            encounterSPtr_->NonPlayerParty()->Characters()[0]->ConditionAdd(creature::condition::ConditionFactory::Make(creature::condition::Stone));

            //TODO TEMP REMOVE -test projectile animations by forcing all enemy creatures to have projectile weapons
            auto projIndex{ 0 };
            for (auto & nextCharacterSPtr : encounterSPtr_->NonPlayerParty()->Characters())
            {
                for (auto const & NEXT_ITEM_SPTR : nextCharacterSPtr->Inventory().ItemsEquipped())
                {
                    if (NEXT_ITEM_SPTR->IsWeapon() && (NEXT_ITEM_SPTR->IsBodypart() == false))
                    {
                        nextCharacterSPtr->ItemUnEquip(NEXT_ITEM_SPTR);
                        nextCharacterSPtr->ItemRemove(NEXT_ITEM_SPTR);
                    }
                }

                item::weapon::projectile_type::Enum projWeaponEnum{ item::weapon::projectile_type::Blowpipe };
                if (projIndex == 1)
                {
                    projWeaponEnum = item::weapon::projectile_type::Longbow;
                }
                else if (projIndex == 2)
                {
                    projWeaponEnum = item::weapon::projectile_type::Crossbow;
                }

                if (++projIndex > 2)
                    projIndex = 0;

                auto const PROJ_WEAPON_SPTR{ heroespath::item::weapon::WeaponFactory::Instance()->Make_Projectile(projWeaponEnum, item::material::Nothing) };
                auto const ITEM_ADD_STR{ nextCharacterSPtr->ItemAdd(PROJ_WEAPON_SPTR) };
                M_ASSERT_OR_LOGANDTHROW_SS((ITEM_ADD_STR.empty()), "\t ********** Unable to ItemAdd() because : \"" << ITEM_ADD_STR << "\"");
                auto const ITEM_EQUIP_STR{ nextCharacterSPtr->ItemEquip(PROJ_WEAPON_SPTR) };
                M_ASSERT_OR_LOGANDTHROW_SS((ITEM_EQUIP_STR.empty()), "\t ********** Unable to ItemEquip() because : \"" << ITEM_EQUIP_STR << "\"");

                nextCharacterSPtr->SetCurrentWeaponsToBest();
            }
        }

        //combat display
        combatDisplayPtrC_->StageRegionSet(COMBAT_REGION);
        combatDisplayPtrC_->Setup();

        //give control of the CombatDisplay object lifetime to the Loop class
        sfml_util::IStageSPtr_t combatDisplayStageSPtr(combatDisplayPtrC_);
        LoopManager::Instance()->AddStage(combatDisplayStageSPtr);

        if (savedCombatInfo_.HasRestored() == false)
        {
            //set Pixie creatures to initially flying
            //while this doesn't technically make them fly, the call to savedCombatInfo_.Restore() will actually set them flying
            {
                combat::CombatNodeSVec_t combatNodesSVec;
                combatDisplayPtrC_->GetCombatNodes(combatNodesSVec);

                for (auto const & NEXT_COMBATNODE_SPTR : combatNodesSVec)
                    if ((creature::race::WillInitiallyFly(NEXT_COMBATNODE_SPTR->Creature()->Race().Which())) ||
                        (creature::role::WillInitiallyFly(NEXT_COMBATNODE_SPTR->Creature()->Role().Which())))
                        NEXT_COMBATNODE_SPTR->SetIsFlying(true);

                savedCombatInfo_.Save(combatDisplayPtrC_);
            }
        }

        //CombatDisplay Zoom Sliderbar
        const sfml_util::gui::TextInfo ZOOMSLIDER_LABEL_TEXT_INFO("Zoom",
                                                                  sfml_util::FontManager::Instance()->Font_Default1(),
                                                                  sfml_util::FontManager::Instance()->Size_Smallish(),
                                                                  sfml_util::FontManager::Instance()->Color_Light(),
                                                                  sfml_util::Justified::Left);
        //
        const sf::FloatRect ZOOMSLIDER_LABEL_RECT(0.0f, COMMAND_REGION_TOP + COMMAND_REGION_PAD, 0.0f, 0.0f);
        //
        sfml_util::gui::TextRegionSPtr_t zoomSliderLabelTextRegionSPtr( new sfml_util::gui::TextRegion("ZoomSlider's",
                                                                                                        ZOOMSLIDER_LABEL_TEXT_INFO,
                                                                                                        ZOOMSLIDER_LABEL_RECT) );
        //
        zoomSliderBarSPtr_.reset(new sfml_util::gui::SliderBar("CombatStageZoom",
                                                               COMMAND_REGION_LEFT + COMMAND_REGION_PAD,
                                                               zoomSliderLabelTextRegionSPtr->GetEntityRegion().top + zoomSliderLabelTextRegionSPtr->GetEntityRegion().height,
                                                               (settingsButtonSPtr_->GetEntityPos().x - COMMAND_REGION_LEFT) - (COMMAND_REGION_PAD * 2.0f),
                                                               sfml_util::gui::SliderStyle(sfml_util::Orientation::Horiz), this));
        zoomSliderBarSPtr_->SetCurrentValue(1.0f);
        zoomSliderLabelTextRegionSPtr->SetEntityPos((zoomSliderBarSPtr_->GetEntityPos().x + (zoomSliderBarSPtr_->GetEntityRegion().width * 0.5f)) - (zoomSliderLabelTextRegionSPtr->GetEntityRegion().width * 0.5f),
                                                    zoomSliderLabelTextRegionSPtr->GetEntityPos().y);
        //
        EntityAdd(zoomSliderLabelTextRegionSPtr);
        EntityAdd(zoomSliderBarSPtr_);

        MoveTurnBoxObjectsOffScreen(true);
        SetIsPlayerActionAllowed(false);

        savedCombatInfo_.Restore(combatDisplayPtrC_);
    }


    void CombatStage::Draw(sf::RenderTarget & target, sf::RenderStates states)
    {
        bottomSymbol_.Draw(target, states);
        target.draw( * commandBoxSPtr_, states);
        Stage::Draw(target, states);
        statusBoxSPtr_->draw(target, states);

        if ((TurnPhase::NotATurn != turnPhase_) && (TurnPhase::StatusAnim != turnPhase_))
        {
            turnBoxSPtr_->draw(target, states);

            titleTBoxTextRegionSPtr_->draw(target, states);
            weaponTBoxTextRegionSPtr_->draw(target, states);
            armorTBoxTextRegionSPtr_->draw(target, states);
            infoTBoxTextRegionSPtr_->draw(target, states);
            enemyActionTBoxRegionSPtr_->draw(target, states);
            enemyCondsTBoxRegionSPtr_->draw(target, states);

            attackTBoxButtonSPtr_->draw(target, states);
            fightTBoxButtonSPtr_->draw(target, states);
            castTBoxButtonSPtr_->draw(target, states);
            advanceTBoxButtonSPtr_->draw(target, states);
            retreatTBoxButtonSPtr_->draw(target, states);
            blockTBoxButtonSPtr_->draw(target, states);
            skipTBoxButtonSPtr_->draw(target, states);
            flyTBoxButtonSPtr_->draw(target, states);
            landTBoxButtonSPtr_->draw(target, states);
            roarTBoxButtonSPtr_->draw(target, states);
            pounceTBoxButtonSPtr_->draw(target, states);
        }

        DrawHoverText(target, states);

        testingTextRegionSPtr_->draw(target, states);
    }


    void CombatStage::UpdateTime(const float ELAPSED_TIME_SEC)
    {
        Stage::UpdateTime(ELAPSED_TIME_SEC);

        if (IsNonPlayerCharacterTurnValid() && (TurnPhase::PrePause == turnPhase_))
        {
            titleTBoxTextRegionSPtr_->SetEntityColorFgBoth(goldTextColorShaker_.Update(ELAPSED_TIME_SEC));
        }

        if (IsNonPlayerCharacterTurnValid() &&
            ((TurnPhase::ZoomAndSlideAnim == turnPhase_) || (TurnPhase::PostZoomPause == turnPhase_)))
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

        //handle creature turns in order of speed stat by starting the centering anim
        if ((IsAttackAnimating() == false) &&
            (encounterSPtr_->HasStarted()) &&
            (IsPaused() == false) &&
            (turnCreaturePtr_ == nullptr) &&
            (false == isCreaturePosSliding_) &&
            (TurnPhase::NotATurn == turnPhase_) &&
            (PreTurnPhase::End == preTurnPhase_))
        {
            StartTurn_Step1();
            return;
        }

        if (TurnPhase::CenteringAnim == turnPhase_)
        {
            combatDisplayPtrC_->CenteringUpdate( centeringSlider_.Update(ELAPSED_TIME_SEC) );

            if (centeringSlider_.GetIsDone())
            {
                StartTurn_Step2();
            }

            return;
        }

        if (isCreaturePosSliding_)
        {
            combatDisplayPtrC_->PositionSlideUpdate(creaturePosSlider_.Update(ELAPSED_TIME_SEC));
            if (creaturePosSlider_.GetIsDone())
            {
                isCreaturePosSliding_ = false;
                combatDisplayPtrC_->PositionSlideStop();
                AfterHandleTurnTasks();
            }

            return;
        }

        if (TurnPhase::StatusAnim == turnPhase_)
        {
            statusMsgAnimTimerSec_ += ELAPSED_TIME_SEC;
            statusBoxSPtr_->SetHighlightColor( statusMsgAnimColorShaker_.Update(ELAPSED_TIME_SEC) );

            if (statusMsgAnimTimerSec_ > STATUSMSG_ANIM_DURATION_SEC_)
            {
                statusBoxSPtr_->SetHighlightColor(LISTBOX_HIGHLIGHT_COLOR_);
                combatDisplayPtrC_->SetIsStatusMessageAnimating(false);
                AfterHandleTurnTasks();
            }

            return;
        }

        if (IsAttackAnimating() && (false == isAttackAnimZoomOut_))
        {
            if (combatDisplayPtrC_->UpdateAttackAnim(ELAPSED_TIME_SEC))
            {
                HandleAttackStage2();
            }

            return;
        }

        if (TurnPhase::ZoomAndSlideAnim == turnPhase_)
        {
            if (combatDisplayPtrC_->ZoomAndSlide_Update(ELAPSED_TIME_SEC))
            {
                combatDisplayPtrC_->ZoomAndSlide_Stop();
                SetTurnPhase(TurnPhase::PostZoomPause);
                StartPause(POST_ZOOM_TURN_DELAY_SEC_, "PostZoomPause");
            }

            return;
        }

        if (PreTurnPhase::PanToCenter == preTurnPhase_)
        {
            //the initial pan seems to take extra time getting started so speed it up here
            auto sliderPosAdj{ 0.25f + centeringSlider_.Update(ELAPSED_TIME_SEC) };
            if (sliderPosAdj > 1.0f)
                sliderPosAdj = 1.0f;

            combatDisplayPtrC_->CenteringUpdate(sliderPosAdj);
            if (centeringSlider_.GetIsDone())
            {
                combatDisplayPtrC_->CenteringStop();
                SetPreTurnPhase(PreTurnPhase::PostPanPause);
                StartPause(POST_PAN_PAUSE_SEC_, "PostPanPause");
                AppendInitialStatus();
            }

            return;
        }

        if (PreTurnPhase::ZoomOut == preTurnPhase_)
        {
            auto const SLIDER_POS{ zoomSlider_.Update(ELAPSED_TIME_SEC) };
            auto const ZOOM_CURR_VAL(1.0f - (SLIDER_POS * 0.5f));//only zoom out half way
            zoomSliderBarSPtr_->SetCurrentValue(ZOOM_CURR_VAL);
            
            if (zoomSlider_.GetIsDone() || (IsAttackAnimating() && combatDisplayPtrC_->IsAttackedCreatureVisible()))
            {
                SetPreTurnPhase(PreTurnPhase::PostZoomOutPause);
                StartPause(POST_ZOOMOUT_PAUSE_SEC_, "PostZoomOutPause");
            }

            return;
        }

        if (PreTurnPhase::ZoomIn == preTurnPhase_)
        {
            auto const SLIDER_POS(zoomSlider_.Update(ELAPSED_TIME_SEC));
            auto const ZOOM_CURR_VAL(0.5f + (SLIDER_POS * 0.5f));//only zoomed out half way so only zoom back in half way
            zoomSliderBarSPtr_->SetCurrentValue(ZOOM_CURR_VAL);

            if (zoomSlider_.GetIsDone())
            {
                SetPreTurnPhase(PreTurnPhase::PostZoomInPause);
                StartPause(POST_ZOOMIN_PAUSE_SEC_, "PostZoomInPause");
            }

            return;
        }

        if ((zoomSliderBarSPtr_.get() != nullptr) &&
            (combatDisplayPtrC_ != nullptr) &&
            (heroespath::LoopManager::Instance()->IsFading() == false) &&
            (TurnPhase::NotATurn == turnPhase_) &&
            (PreTurnPhase::Start == preTurnPhase_))
        {
            SetPreTurnPhase(PreTurnPhase::PanToCenter);
            SetIsPlayerActionAllowed(false);
            centeringSlider_.Reset(INITIAL_CENTERING_SLIDER_SPEED_);
            combatDisplayPtrC_->CenteringStartTargetCenterOfBatllefield();
            return;
        }
    }


    void CombatStage::UpdateMouseDown(const sf::Vector2f & MOUSE_POS_V)
    {
        if ((IsPaused() == false) &&
            (TurnPhase::DetermineAction == turnPhase_) &&
            (IsAttackAnimating() == false))
        {
            isMouseHeldDownAndValid_ = true;
            Stage::UpdateMouseDown(MOUSE_POS_V);
        }
    }


    void CombatStage::UpdateMousePos(const sf::Vector2f & MOUSE_POS_V)
    {
        if (isMouseHeldDownAndValid_ && (mousePrevPosV_ != MOUSE_POS_V))
            isMouseDragging_ = true;

        mousePrevPosV_ = MOUSE_POS_V;
    }


    sfml_util::gui::IGuiEntitySPtr_t CombatStage::UpdateMouseUp(const sf::Vector2f & MOUSE_POS_V)
    {
        isMouseHeldDownAndValid_ = false;

        if (isPlayerActionAllowed_ &&
            (IsPaused() == false) &&
            (TurnPhase::DetermineAction == turnPhase_))
        {
            creature::CreaturePtr_t creatureAtPosPtr(combatDisplayPtrC_->GetCreatureAtPos(MOUSE_POS_V).get());

            if ((false == isMouseDragging_) &&
                (creatureAtPosPtr != nullptr) &&
                creatureAtPosPtr->IsPlayerCharacter())
            {
                savedCombatInfo_.PrepareForStageChange(combatDisplayPtrC_);
                heroespath::LoopManager::Instance()->Goto_Inventory(creatureAtPosPtr);
            }

            isMouseDragging_ = false;
            return Stage::UpdateMouseUp(MOUSE_POS_V);
        }
        else
        {
            isMouseDragging_ = false;
            return sfml_util::gui::IGuiEntitySPtr_t();
        }
    }


    bool CombatStage::KeyRelease(const sf::Event::KeyEvent & KE)
    {
        //cancel summary view if visible or even just starting
        if (combatDisplayPtrC_->GetIsSummaryViewInProgress())
        {
            combatDisplayPtrC_->CancelSummaryViewAndStartTransitionBack();
            return true;
        }

        //cancel pause if paused
        if (IsPaused())
        {
            isPauseCanceled_ = true;
            return true;
        }

        if (IsPlayerCharacterTurnValid() && (TurnPhase::DetermineAction == turnPhase_))
        {
            if ((KE.code == sf::Keyboard::Return) || (KE.code == sf::Keyboard::A))
                return HandleAttack();

            if (KE.code == sf::Keyboard::F)
                return HandleFight();

            if (KE.code == sf::Keyboard::C)
                return HandleCast();

            if (KE.code == sf::Keyboard::Right)
                return HandleAdvance();

            if (KE.code == sf::Keyboard::Left)
                return HandleRetreat();

            if ((KE.code == sf::Keyboard::B) || ((KE.code == sf::Keyboard::Space) && (turnCreaturePtr_->CanTakeAction())))
                return HandleBlock();

            if ((KE.code == sf::Keyboard::S) || ((KE.code == sf::Keyboard::Space) && (turnCreaturePtr_->CanTakeAction() == false)))
                return HandleSkip();

            if (KE.code == sf::Keyboard::I)
            {
                if ((turnCreaturePtr_ != nullptr) && turnCreaturePtr_->IsPlayerCharacter())
                {
                    savedCombatInfo_.PrepareForStageChange(combatDisplayPtrC_);
                    heroespath::LoopManager::Instance()->Goto_Inventory(turnCreaturePtr_);
                }
            }

            if (KE.code == sf::Keyboard::Y)
                return HandleFly();

            if (KE.code == sf::Keyboard::L)
                return HandleLand();

            if (KE.code == sf::Keyboard::R)
                return HandleRoar();

            if (KE.code == sf::Keyboard::P)
                return HandlePounce(encounterSPtr_->GetTurnInfoCopy(turnCreaturePtr_).GetIsFlying());

            if (KE.code == sf::Keyboard::X)
                return HandleWeaponChange();
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


    void CombatStage::AppendStatusMessage(const std::string & MSG_STR, const bool WILL_ANIM)
    {
        statusBoxTextInfo_.text = MSG_STR;
        sfml_util::gui::ListBoxItemSPtr_t entrySPtr(new sfml_util::gui::ListBoxItem("CombatStageStatusMsg", statusBoxTextInfo_));
        statusBoxSPtr_->Add(entrySPtr, true);

        if (WILL_ANIM)
        {
            SetTurnPhase(TurnPhase::StatusAnim);
            MoveTurnBoxObjectsOffScreen(true);
            statusMsgAnimColorShaker_.Reset();
            statusMsgAnimColorShaker_.Start();
            statusMsgAnimTimerSec_ = 0.0f;
            combatDisplayPtrC_->SetIsStatusMessageAnimating(true);
        }
    }


    void CombatStage::AppendInitialStatus()
    {
        std::ostringstream ss;
        switch (sfml_util::rand::Int(0, 4))
        {
            case 1:  { ss << "You face "; break; }
            case 2:  { ss << "Before you rage "; break; }
            case 3:  { ss << "Before you stand "; break; }
            case 4:  { ss << "Attacking you are "; break; }
            case 0:
            default: { ss << "You encounter "; break; }
        }
        ss << encounterSPtr_->NonPlayerParty()->Summary() << "!";
        AppendStatusMessage(ss.str(), true);
    }


    void CombatStage::SetIsPlayerActionAllowed(const bool IS_ALLOWED)
    {
        isPlayerActionAllowed_ = IS_ALLOWED;

        if (combatDisplayPtrC_)
            combatDisplayPtrC_->IsPlayerActionAllowed(IS_ALLOWED);

        heroespath::LoopManager::Instance()->SetIgnoreMouse( ! IS_ALLOWED);
    }


    void CombatStage::StartPause(const float DURATION_SEC, const std::string TITLE)
    {
        SetIsPlayerActionAllowed(false);
        pauseElapsedSec_ = 0.0f;
        pauseDurationSec_ = DURATION_SEC;
        pauseTitle_ = TITLE;
        UpdateTestingText();
    }


    void CombatStage::EndPause()
    {
        UpdateTestingText();

        pauseElapsedSec_ = pauseDurationSec_ + 1.0f;//anything greater than pauseDurationSec_ will work here

        SetIsPlayerActionAllowed(true);

        if (PreTurnPhase::PostPanPause == preTurnPhase_)
        {
            SetPreTurnPhase(PreTurnPhase::ZoomOut);
            SetIsPlayerActionAllowed(false);
            zoomSlider_.Reset(ZOOM_SLIDER_SPEED_);
            return;
        }

        if (PreTurnPhase::PostZoomOutPause == preTurnPhase_)
        {
            SetIsPlayerActionAllowed(false);
         
            if (isAttackAnimZoomOut_)
            {
                SetPreTurnPhase(PreTurnPhase::End);
                isAttackAnimZoomOut_ = false;
                combatDisplayPtrC_->StartAttackAnim();
            }
            else
            {
                SetPreTurnPhase(PreTurnPhase::ZoomIn);
                zoomSlider_.Reset(ZOOM_SLIDER_SPEED_);
            }

            return;
        }

        if (PreTurnPhase::PostZoomInPause == preTurnPhase_)
        {
            SetPreTurnPhase(PreTurnPhase::End);
            isAttackAnimZoomOut_ = false;
            SetIsPlayerActionAllowed(false);
            combatDisplayPtrC_->SetSummaryViewAllowed();
            return;
        }

        if (IsPlayerCharacterTurnValid() && (TurnPhase::PrePause == turnPhase_))
        {
            SetTurnPhase(TurnPhase::DetermineAction);
            return;
        }

        if (IsPlayerCharacterTurnValid() && (TurnPhase::PostZoomPause == turnPhase_))
        {
            SetTurnPhase(TurnPhase::PerformAnim);
            StartPerformAnim();
            return;
        }

        if (IsNonPlayerCharacterTurnValid() && (TurnPhase::PrePause == turnPhase_))
        {
            SetTurnPhase(TurnPhase::DetermineAction);
            HandleEnemyTurnStep1_Decide();

            //also do the perform step here so that the TurnBox can dispay the non-player creature's intent before showing the result
            performType_ = HandleEnemyTurnStep2_Perform();

            SetTurnPhase(TurnPhase::ZoomAndSlideAnim);
            combatDisplayPtrC_->ZoomAndSlide_Start();
            return;
        }

        if (IsNonPlayerCharacterTurnValid() && (TurnPhase::PostZoomPause == turnPhase_))
        {
            //See above for call to HandleEnemyTurnStep2_Perform()
            SetTurnPhase(TurnPhase::PerformReport);
            StartPause(PERFORM_REPORT_DELAY_SEC_, "PerformReportPre");
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
                        performReportHitIndex_ = 0;
                        performReportEffectIndex_ = 0;
                        AfterHandleTurnTasks();
                        return;
                    }
                }
                
                SetupTurnBox();
            }
            else
            {
                performReportHitIndex_ = 0;
                performReportEffectIndex_ = 0;
                AfterHandleTurnTasks();
                return;
            }
            
            StartPause(PERFORM_TURN_DELAY_SEC_, "PerformReportTurn");
            return;
        }

        if (TurnPhase::PerformPause == turnPhase_)
        {
            AfterHandleTurnTasks();
            return;
        }

        if (combat::TurnAction::Attack == turnStateToFinish_)
            turnStateToFinish_ = combat::TurnAction::Count;
    }


    void CombatStage::HandleEnemyTurnStep1_Decide()
    {
        turnActionInfo_ = combat::TurnDecider::Decide(turnCreaturePtr_, combatDisplayPtrC_);
        encounterSPtr_->SetTurnActionInfo(turnCreaturePtr_, turnActionInfo_);
        SetupTurnBox();
    }


    CombatStage::PerformType CombatStage::HandleEnemyTurnStep2_Perform()
    {
        switch (turnActionInfo_.Action())
        {
            case combat::TurnAction::Advance:
            {
                combatDisplayPtrC_->MoveCreatureBlockingPosition(turnCreaturePtr_, true);
                PositionSlideStartTasks();
                fightResult_ = combat::FightResult();
                SetupTurnBox();
                AppendStatusMessage(combat::Text::ActionText(turnCreaturePtr_, turnActionInfo_, fightResult_, true, true), false);
                return PerformType::Advance;
            }

            case combat::TurnAction::Retreat:
            {
                combatDisplayPtrC_->MoveCreatureBlockingPosition(turnCreaturePtr_, false);
                PositionSlideStartTasks();
                fightResult_ = combat::FightResult();
                SetupTurnBox();
                AppendStatusMessage(combat::Text::ActionText(turnCreaturePtr_, turnActionInfo_, fightResult_, true, true), false);
                return PerformType::Retreat;
            }

            case combat::TurnAction::Attack:
            {
                fightResult_ = combat::FightClub::Fight(turnCreaturePtr_, turnActionInfo_.Target());
                SetupTurnBox();
                AppendStatusMessage(combat::Text::ActionText(turnCreaturePtr_, turnActionInfo_, fightResult_, true, true), false);
                return PerformType::Attack;
            }

            case combat::TurnAction::Block:
            {
                fightResult_ = combat::FightResult();
                SetupTurnBox();
                AppendStatusMessage(combat::Text::ActionText(turnCreaturePtr_, turnActionInfo_, fightResult_, true, true), false);
                return PerformType::Pause;
            }

            case combat::TurnAction::Fly:
            case combat::TurnAction::Land:
            {
                fightResult_ = combat::FightResult();
                combatDisplayPtrC_->HandleFlyingChange(turnCreaturePtr_, false);
                SetupTurnBox();
                AppendStatusMessage(combat::Text::ActionText(turnCreaturePtr_, turnActionInfo_, fightResult_, true, true), false);
                return PerformType::Pause;
            }

            case combat::TurnAction::Cast:
            {
                fightResult_ = combat::FightClub::Cast(turnActionInfo_.Spell(), turnCreaturePtr_, turnActionInfo_.Target());
                SetupTurnBox();
                AppendStatusMessage(combat::Text::ActionText(turnCreaturePtr_, turnActionInfo_, fightResult_, true, true), false);
                return PerformType::Cast;
            }

            case combat::TurnAction::SkyPounce:
            case combat::TurnAction::LandPounce:
            {
                fightResult_ = combat::FightClub::Pounce(turnCreaturePtr_, turnActionInfo_.Target());
                SetupTurnBox();
                AppendStatusMessage(combat::Text::ActionText(turnCreaturePtr_, turnActionInfo_, fightResult_, true, true), false);

                //TODO if fightResult_ says the pounce was a success then return the PerformType::Pounce
                return PerformType::Pause;
            }

            case combat::TurnAction::Roar:
            {
                fightResult_ = combat::FightClub::Roar(turnCreaturePtr_, combatDisplayPtrC_);
                SetupTurnBox();
                AppendStatusMessage(combat::Text::ActionText(turnCreaturePtr_, turnActionInfo_, fightResult_, true, true), false);

                //TODO if fightResult_ says the roar happened then return PerformType::Roar
                return PerformType::Pause;
            }

            case combat::TurnAction::ChangeWeapon:
            case combat::TurnAction::Nothing:
            case combat::TurnAction::Count://handle Count gracefully because it is sometimes returned
            {
                fightResult_ = combat::FightResult();
                SetupTurnBox();
                AppendStatusMessage(combat::Text::ActionText(turnCreaturePtr_, turnActionInfo_, fightResult_, true, true), false);
                return PerformType::Pause;
            }

            default:
            {
                std::ostringstream ss;
                ss << "heroespath::stage::CombatStage::HandleEnemyTurnStep2_Perform() found turnActionInfo_.Action() (which is really a combat::TurnAction) of " << turnActionInfo_.Action() << " which is invalid.";
                throw std::runtime_error(ss.str());
            }
        }
    }


    //start centering anim
    void CombatStage::StartTurn_Step1()
    {
        SetTurnPhase(TurnPhase::CenteringAnim);
        turnCreaturePtr_ = encounterSPtr_->CurrentTurnCreature();

        performReportEffectIndex_ = 0;
        performReportHitIndex_ = 0;

        SetIsPlayerActionAllowed(false);
        centeringSlider_.Reset(CENTERING_SLIDER_SPEED_);
        combatDisplayPtrC_->CenteringStart(turnCreaturePtr_);
    }


    //start pre-pause
    void CombatStage::StartTurn_Step2()
    {
        auto const IS_PLAYER_TURN{ turnCreaturePtr_->IsPlayerCharacter() };
        combatDisplayPtrC_->SetIsPlayerTurn(IS_PLAYER_TURN);
        SetIsPlayerActionAllowed(IS_PLAYER_TURN);

        combatDisplayPtrC_->CenteringStop();
        combatDisplayPtrC_->StartShaking(turnCreaturePtr_);

        goldTextColorShaker_.Reset();

        SetupTurnBox();

        //skip PrePause if a player turn
        if (IS_PLAYER_TURN)
        {
            SetTurnPhase(TurnPhase::DetermineAction);
        }
        else
        {
            SetTurnPhase(TurnPhase::PrePause);
            StartPause(PRE_TURN_DELAY_SEC_, "PreTurn");
        }
    }


    void CombatStage::EndTurn()
    {
        if (savedCombatInfo_.CanTurnAdvance())
        {
            encounterSPtr_->IncrementTurn();
            savedCombatInfo_.CanTurnAdvance(false);
        }

        turnActionInfo_ = combat::TurnActionInfo();
        turnCreaturePtr_ = nullptr;
        MoveTurnBoxObjectsOffScreen(true);
        combatDisplayPtrC_->UpdateAllConditionText();

        //TODO TEMP REMOVE
        for (auto const & NEXT_CHARACTER_SPTR : encounterSPtr_->NonPlayerParty()->Characters())
        {
            M_ASSERT_OR_LOGANDTHROW_SS((NEXT_CHARACTER_SPTR->Stats().Str().Normal()  >= 0), "herospath::stage::CombatStage::EndTurn() NonPlayer " << NEXT_CHARACTER_SPTR->Name() << " has invalid Normal Stat: Str="  << NEXT_CHARACTER_SPTR->Stats().Str().Normal());
            M_ASSERT_OR_LOGANDTHROW_SS((NEXT_CHARACTER_SPTR->Stats().Acc().Normal()  >= 0), "herospath::stage::CombatStage::EndTurn() NonPlayer " << NEXT_CHARACTER_SPTR->Name() << " has invalid Normal Stat: Acc="  << NEXT_CHARACTER_SPTR->Stats().Acc().Normal());
            M_ASSERT_OR_LOGANDTHROW_SS((NEXT_CHARACTER_SPTR->Stats().Cha().Normal()  >= 0), "herospath::stage::CombatStage::EndTurn() NonPlayer " << NEXT_CHARACTER_SPTR->Name() << " has invalid Normal Stat: Cha="  << NEXT_CHARACTER_SPTR->Stats().Cha().Normal());
            M_ASSERT_OR_LOGANDTHROW_SS((NEXT_CHARACTER_SPTR->Stats().Lck().Normal()  >= 0), "herospath::stage::CombatStage::EndTurn() NonPlayer " << NEXT_CHARACTER_SPTR->Name() << " has invalid Normal Stat: Lck="  << NEXT_CHARACTER_SPTR->Stats().Lck().Normal());
            M_ASSERT_OR_LOGANDTHROW_SS((NEXT_CHARACTER_SPTR->Stats().Spd().Normal()  >= 0), "herospath::stage::CombatStage::EndTurn() NonPlayer " << NEXT_CHARACTER_SPTR->Name() << " has invalid Normal Stat: Spd="  << NEXT_CHARACTER_SPTR->Stats().Spd().Normal());
            M_ASSERT_OR_LOGANDTHROW_SS((NEXT_CHARACTER_SPTR->Stats().Int().Normal()  >= 0), "herospath::stage::CombatStage::EndTurn() NonPlayer " << NEXT_CHARACTER_SPTR->Name() << " has invalid Normal Stat: Int="  << NEXT_CHARACTER_SPTR->Stats().Int().Normal());
            M_ASSERT_OR_LOGANDTHROW_SS((NEXT_CHARACTER_SPTR->Stats().Str().Current() >= 0), "herospath::stage::CombatStage::EndTurn() NonPlayer " << NEXT_CHARACTER_SPTR->Name() << " has invalid Current Stat: Str=" << NEXT_CHARACTER_SPTR->Stats().Str().Current());
            M_ASSERT_OR_LOGANDTHROW_SS((NEXT_CHARACTER_SPTR->Stats().Acc().Current() >= 0), "herospath::stage::CombatStage::EndTurn() NonPlayer " << NEXT_CHARACTER_SPTR->Name() << " has invalid Current Stat: Acc=" << NEXT_CHARACTER_SPTR->Stats().Acc().Current());
            M_ASSERT_OR_LOGANDTHROW_SS((NEXT_CHARACTER_SPTR->Stats().Cha().Current() >= 0), "herospath::stage::CombatStage::EndTurn() NonPlayer " << NEXT_CHARACTER_SPTR->Name() << " has invalid Current Stat: Cha=" << NEXT_CHARACTER_SPTR->Stats().Cha().Current());
            M_ASSERT_OR_LOGANDTHROW_SS((NEXT_CHARACTER_SPTR->Stats().Lck().Current() >= 0), "herospath::stage::CombatStage::EndTurn() NonPlayer " << NEXT_CHARACTER_SPTR->Name() << " has invalid Current Stat: Lck=" << NEXT_CHARACTER_SPTR->Stats().Lck().Current());
            M_ASSERT_OR_LOGANDTHROW_SS((NEXT_CHARACTER_SPTR->Stats().Spd().Current() >= 0), "herospath::stage::CombatStage::EndTurn() NonPlayer " << NEXT_CHARACTER_SPTR->Name() << " has invalid Current Stat: Spd=" << NEXT_CHARACTER_SPTR->Stats().Spd().Current());
            M_ASSERT_OR_LOGANDTHROW_SS((NEXT_CHARACTER_SPTR->Stats().Int().Current() >= 0), "herospath::stage::CombatStage::EndTurn() NonPlayer " << NEXT_CHARACTER_SPTR->Name() << " has invalid Current Stat: Int=" << NEXT_CHARACTER_SPTR->Stats().Int().Current());
        }

        for (auto const & NEXT_CHARACTER_SPTR : player::Party().Characters())
        {
            M_ASSERT_OR_LOGANDTHROW_SS((NEXT_CHARACTER_SPTR->Stats().Str().Normal()  >= 0), "herospath::stage::CombatStage::EndTurn() Player " << NEXT_CHARACTER_SPTR->Name() << " has invalid Normal Stat: Str="  << NEXT_CHARACTER_SPTR->Stats().Str().Normal());
            M_ASSERT_OR_LOGANDTHROW_SS((NEXT_CHARACTER_SPTR->Stats().Acc().Normal()  >= 0), "herospath::stage::CombatStage::EndTurn() Player " << NEXT_CHARACTER_SPTR->Name() << " has invalid Normal Stat: Acc="  << NEXT_CHARACTER_SPTR->Stats().Acc().Normal());
            M_ASSERT_OR_LOGANDTHROW_SS((NEXT_CHARACTER_SPTR->Stats().Cha().Normal()  >= 0), "herospath::stage::CombatStage::EndTurn() Player " << NEXT_CHARACTER_SPTR->Name() << " has invalid Normal Stat: Cha="  << NEXT_CHARACTER_SPTR->Stats().Cha().Normal());
            M_ASSERT_OR_LOGANDTHROW_SS((NEXT_CHARACTER_SPTR->Stats().Lck().Normal()  >= 0), "herospath::stage::CombatStage::EndTurn() Player " << NEXT_CHARACTER_SPTR->Name() << " has invalid Normal Stat: Lck="  << NEXT_CHARACTER_SPTR->Stats().Lck().Normal());
            M_ASSERT_OR_LOGANDTHROW_SS((NEXT_CHARACTER_SPTR->Stats().Spd().Normal()  >= 0), "herospath::stage::CombatStage::EndTurn() Player " << NEXT_CHARACTER_SPTR->Name() << " has invalid Normal Stat: Spd="  << NEXT_CHARACTER_SPTR->Stats().Spd().Normal());
            M_ASSERT_OR_LOGANDTHROW_SS((NEXT_CHARACTER_SPTR->Stats().Int().Normal()  >= 0), "herospath::stage::CombatStage::EndTurn() Player " << NEXT_CHARACTER_SPTR->Name() << " has invalid Normal Stat: Int="  << NEXT_CHARACTER_SPTR->Stats().Int().Normal());
            M_ASSERT_OR_LOGANDTHROW_SS((NEXT_CHARACTER_SPTR->Stats().Str().Current() >= 0), "herospath::stage::CombatStage::EndTurn() Player " << NEXT_CHARACTER_SPTR->Name() << " has invalid Current Stat: Str=" << NEXT_CHARACTER_SPTR->Stats().Str().Current());
            M_ASSERT_OR_LOGANDTHROW_SS((NEXT_CHARACTER_SPTR->Stats().Acc().Current() >= 0), "herospath::stage::CombatStage::EndTurn() Player " << NEXT_CHARACTER_SPTR->Name() << " has invalid Current Stat: Acc=" << NEXT_CHARACTER_SPTR->Stats().Acc().Current());
            M_ASSERT_OR_LOGANDTHROW_SS((NEXT_CHARACTER_SPTR->Stats().Cha().Current() >= 0), "herospath::stage::CombatStage::EndTurn() Player " << NEXT_CHARACTER_SPTR->Name() << " has invalid Current Stat: Cha=" << NEXT_CHARACTER_SPTR->Stats().Cha().Current());
            M_ASSERT_OR_LOGANDTHROW_SS((NEXT_CHARACTER_SPTR->Stats().Lck().Current() >= 0), "herospath::stage::CombatStage::EndTurn() Player " << NEXT_CHARACTER_SPTR->Name() << " has invalid Current Stat: Lck=" << NEXT_CHARACTER_SPTR->Stats().Lck().Current());
            M_ASSERT_OR_LOGANDTHROW_SS((NEXT_CHARACTER_SPTR->Stats().Spd().Current() >= 0), "herospath::stage::CombatStage::EndTurn() Player " << NEXT_CHARACTER_SPTR->Name() << " has invalid Current Stat: Spd=" << NEXT_CHARACTER_SPTR->Stats().Spd().Current());
            M_ASSERT_OR_LOGANDTHROW_SS((NEXT_CHARACTER_SPTR->Stats().Int().Current() >= 0), "herospath::stage::CombatStage::EndTurn() Player " << NEXT_CHARACTER_SPTR->Name() << " has invalid Current Stat: Int=" << NEXT_CHARACTER_SPTR->Stats().Int().Current());

        }
    }


    void CombatStage::PositionSlideStartTasks()
    {
        isCreaturePosSliding_ = true;

        if (turnCreaturePtr_ != nullptr)
            combatDisplayPtrC_->StopShaking(turnCreaturePtr_);

        combatDisplayPtrC_->PositionSlideStart();
        creaturePosSlider_.Reset(CREATURE_POS_SLIDER_SPEED_);
    }


    bool CombatStage::HandleAttack()
    {
        auto const MOUSEOVER_STR( combat::Text::MouseOverTextAttackStr(turnCreaturePtr_, combatDisplayPtrC_) );
        if (MOUSEOVER_STR != combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_ATTACK_)
        {
            QuickSmallPopup(MOUSEOVER_STR, false, true);
            return false;
        }
        else
        {
            opponentCreature_ = combat::FightClub::FindNonPlayerCreatureToAttack(turnCreaturePtr_, combatDisplayPtrC_);
            turnActionInfo_ = combat::TurnActionInfo(combat::TurnAction::Attack, opponentCreature_, nullptr);
            encounterSPtr_->SetTurnActionInfo(turnCreaturePtr_, turnActionInfo_);

            performType_ = PerformType::Attack;
            SetTurnPhase(TurnPhase::ZoomAndSlideAnim);
            combatDisplayPtrC_->ZoomAndSlide_Start();
            return true;
        }
    }


    bool CombatStage::HandleFight()
    {
        auto const MOUSEOVER_STR( combat::Text::MouseOverTextFightStr(turnCreaturePtr_, combatDisplayPtrC_) );
        if (MOUSEOVER_STR != combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_FIGHT_)
        {
            QuickSmallPopup(MOUSEOVER_STR, false, true);
            return false;
        }
        else
        {
            //can't set turnActionInfo_ yet because the player must select which non_player creature to fight first
            //turnActionInfo_ =

            performType_ = PerformType::Attack;
            SetTurnPhase(TurnPhase::ZoomAndSlideAnim);
            combatDisplayPtrC_->ZoomAndSlide_Start();
            return true;
        }
    }


    bool CombatStage::HandleCast()
    {
        auto const MOUSEOVER_STR( combat::Text::MouseOverTextCastStr(turnCreaturePtr_, combatDisplayPtrC_)  );
        if (MOUSEOVER_STR != combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_CAST_)
        {
            QuickSmallPopup(MOUSEOVER_STR, false, true);
            return false;
        }
        else
        {
            //can't set turnActionInfo_ yet because the player must select which spell to cast and which creature to cast it on
            //turnActionInfo_ =

            performType_ = PerformType::Cast;
            SetTurnPhase(TurnPhase::ZoomAndSlideAnim);
            combatDisplayPtrC_->ZoomAndSlide_Start();
            return true;
        }
    }


    bool CombatStage::HandleAdvance()
    {
        auto const MOUSEOVER_STR( combat::Text::MouseOverTextAdvanceStr(turnCreaturePtr_, combatDisplayPtrC_)  );
        if (MOUSEOVER_STR != combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_ADVANCE_)
        {
            QuickSmallPopup(MOUSEOVER_STR, false, true);
            return false;
        }
        else
        {
            turnActionInfo_ = combat::TurnActionInfo(combat::TurnAction::Advance);
            encounterSPtr_->SetTurnActionInfo(turnCreaturePtr_, turnActionInfo_);

            performType_ = PerformType::Advance;

            //no need for ZoomAndSlide
            StartPerformAnim();
            return true;
        }
    }


    bool CombatStage::HandleRetreat()
    {
        auto const MOUSEOVER_STR( combat::Text::MouseOverTextRetreatStr(turnCreaturePtr_, combatDisplayPtrC_) );
        if (MOUSEOVER_STR != combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_RETREAT_)
        {
            QuickSmallPopup(MOUSEOVER_STR, false, true);
            return false;
        }
        else
        {
            turnActionInfo_ = combat::TurnActionInfo(combat::TurnAction::Retreat);
            encounterSPtr_->SetTurnActionInfo(turnCreaturePtr_, turnActionInfo_);

            //no need for ZoomAndSlide
            StartPerformAnim();
            return true;
        }
    }


    bool CombatStage::HandleBlock()
    {
        auto const MOUSEOVER_STR( combat::Text::MouseOverTextBlockStr(turnCreaturePtr_, combatDisplayPtrC_) );
        if (MOUSEOVER_STR != combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_BLOCK_)
        {
            QuickSmallPopup(MOUSEOVER_STR, false, true);
            return false;
        }
        else
        {
            turnActionInfo_ = combat::TurnActionInfo(combat::TurnAction::Block);
            encounterSPtr_->SetTurnActionInfo(turnCreaturePtr_, turnActionInfo_);
            AppendStatusMessage(combat::Text::ActionText(turnCreaturePtr_, turnActionInfo_, fightResult_, true, true), true);
            //no need for ZoomAndSlide or PerformAnim
            return true;
        }
    }


    bool CombatStage::HandleSkip()
    {
        if (skipTBoxButtonSPtr_->IsDisabled())
            return false;

        turnActionInfo_ = combat::TurnActionInfo(combat::TurnAction::Nothing);
        encounterSPtr_->SetTurnActionInfo(turnCreaturePtr_, turnActionInfo_);
        AppendStatusMessage(combat::Text::ActionText(turnCreaturePtr_, turnActionInfo_, fightResult_, true, true), true);
        //no need for ZoomAndSlide or PerformAnim
        return true;
    }


    bool CombatStage::HandleFly()
    {
        auto const MOUSEOVER_STR( combat::Text::MouseOverTextFlyStr(turnCreaturePtr_, combatDisplayPtrC_) );
        if (MOUSEOVER_STR != combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_FLY_)
        {
            QuickSmallPopup(MOUSEOVER_STR, false, true);
            return false;
        }
        else
        {
            turnActionInfo_ = combat::TurnActionInfo(combat::TurnAction::Fly);
            encounterSPtr_->SetTurnActionInfo(turnCreaturePtr_, turnActionInfo_);
            encounterSPtr_->SetIsFlying(turnCreaturePtr_, true);

            combatDisplayPtrC_->HandleFlyingChange(turnCreaturePtr_, true);
            AppendStatusMessage(combat::Text::ActionText(turnCreaturePtr_, turnActionInfo_, fightResult_, true, true), true);
            //no need for ZoomAndSlide or PerformAnim
            return true;
        }
    }


    bool CombatStage::HandleLand()
    {
        auto const MOUSEOVER_STR( combat::Text::MouseOverTextLandStr(turnCreaturePtr_, combatDisplayPtrC_) );
        if (MOUSEOVER_STR != combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_LAND_)
        {
            QuickSmallPopup(MOUSEOVER_STR, false, true);
            return false;
        }
        else
        {
            turnActionInfo_ = combat::TurnActionInfo(combat::TurnAction::Land);
            encounterSPtr_->SetTurnActionInfo(turnCreaturePtr_, turnActionInfo_);
            encounterSPtr_->SetIsFlying(turnCreaturePtr_, false);

            combatDisplayPtrC_->HandleFlyingChange(turnCreaturePtr_, false);
            AppendStatusMessage(combat::Text::ActionText(turnCreaturePtr_, turnActionInfo_, fightResult_, true, true), false);
            //no need for ZoomAndSlide or PerformAnim
            return true;
        }
    }


    bool CombatStage::HandleRoar()
    {
        auto const MOUSEOVER_STR( combat::Text::MouseOverTextRoarStr(turnCreaturePtr_, combatDisplayPtrC_) );
        if (MOUSEOVER_STR != combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_ROAR_)
        {
            QuickSmallPopup(MOUSEOVER_STR, false, true);
            return false;
        }
        else
        {
            turnActionInfo_ = combat::TurnActionInfo(combat::TurnAction::Roar);
            encounterSPtr_->SetTurnActionInfo(turnCreaturePtr_, turnActionInfo_);

            AppendStatusMessage(combat::Text::ActionText(turnCreaturePtr_, turnActionInfo_, fightResult_, true, true), false);

            //TODO replace with actual Roar action
            AfterHandleTurnTasks();

            return true;
        }
    }


    bool CombatStage::HandlePounce(const bool IS_SKY_POUNCE)
    {
        auto const MOUSEOVER_STR( combat::Text::MouseOverTextPounceStr(turnCreaturePtr_, combatDisplayPtrC_) );
        if (MOUSEOVER_STR != combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_POUNCE_)
        {
            QuickSmallPopup(MOUSEOVER_STR, false, true);
            return false;
        }
        else
        {
            turnActionInfo_ = combat::TurnActionInfo(((IS_SKY_POUNCE) ? combat::TurnAction::SkyPounce : combat::TurnAction::LandPounce));
            encounterSPtr_->SetTurnActionInfo(turnCreaturePtr_, turnActionInfo_);

            AppendStatusMessage(combat::Text::ActionText(turnCreaturePtr_, turnActionInfo_, fightResult_, true, true), false);

            //TODO replace with actual Pounce action
            AfterHandleTurnTasks();

            return true;
        }
    }


    bool CombatStage::HandleWeaponChange()
    {
        turnActionInfo_ = combat::TurnActionInfo(combat::TurnAction::ChangeWeapon);
        encounterSPtr_->SetTurnActionInfo(turnCreaturePtr_, turnActionInfo_);
        turnCreaturePtr_->CurrentWeaponsInc();
        SetupTurnBox();
        return true;
    }


    void CombatStage::AfterHandleTurnTasks()
    {
        SetTurnPhase(TurnPhase::NotATurn);
        savedCombatInfo_.CanTurnAdvance(true);
        combatDisplayPtrC_->HandleEndOfTurnTasks();
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
            skipTBoxButtonSPtr_->MoveEntityOffScreen();
    }


    void CombatStage::SetupTurnBoxButtons(const creature::CreaturePtrC_t CREATURE_CPTRC)
    {
        if (CREATURE_CPTRC->IsPlayerCharacter())
        {
            tBoxStdButtonSVec_.clear();

            {
                const std::string MOT_ATTACK_STR(combat::Text::MouseOverTextAttackStr(CREATURE_CPTRC, combatDisplayPtrC_));
                attackTBoxButtonSPtr_->SetIsDisabled(MOT_ATTACK_STR != combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_ATTACK_);
                attackTBoxButtonSPtr_->SetMouseHoverText(MOT_ATTACK_STR);

                if (MOT_ATTACK_STR == combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_ATTACK_)
                    tBoxStdButtonSVec_.push_back(attackTBoxButtonSPtr_);
            }

            {
                const std::string MOT_FIGHT_STR(combat::Text::MouseOverTextFightStr(CREATURE_CPTRC, combatDisplayPtrC_));
                fightTBoxButtonSPtr_->SetIsDisabled(MOT_FIGHT_STR != combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_FIGHT_);
                fightTBoxButtonSPtr_->SetMouseHoverText(MOT_FIGHT_STR);

                if (MOT_FIGHT_STR == combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_FIGHT_)
                    tBoxStdButtonSVec_.push_back(fightTBoxButtonSPtr_);
            }

            {
                const std::string MOT_CAST_STR(combat::Text::MouseOverTextCastStr(CREATURE_CPTRC, combatDisplayPtrC_));
                castTBoxButtonSPtr_->SetIsDisabled(MOT_CAST_STR != combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_CAST_);
                castTBoxButtonSPtr_->SetMouseHoverText(MOT_CAST_STR);

                if (MOT_CAST_STR == combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_CAST_)
                    tBoxStdButtonSVec_.push_back(castTBoxButtonSPtr_);
            }

            {
                const std::string MOT_ADVANCE_STR(combat::Text::MouseOverTextAdvanceStr(CREATURE_CPTRC, combatDisplayPtrC_));
                advanceTBoxButtonSPtr_->SetIsDisabled(MOT_ADVANCE_STR != combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_ADVANCE_);
                advanceTBoxButtonSPtr_->SetMouseHoverText(MOT_ADVANCE_STR);
            }

            {
                const std::string MOT_RETREAT_STR(combat::Text::MouseOverTextRetreatStr(CREATURE_CPTRC, combatDisplayPtrC_));
                retreatTBoxButtonSPtr_->SetIsDisabled(MOT_RETREAT_STR != combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_RETREAT_);
                retreatTBoxButtonSPtr_->SetMouseHoverText(MOT_RETREAT_STR);
            }

            {
                const std::string MOT_BLOCK_STR(combat::Text::MouseOverTextBlockStr(CREATURE_CPTRC, combatDisplayPtrC_));
                blockTBoxButtonSPtr_->SetIsDisabled(MOT_BLOCK_STR != combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_BLOCK_);
                blockTBoxButtonSPtr_->SetMouseHoverText(MOT_BLOCK_STR);
            }

            {
                const std::string MOT_FLY_STR(combat::Text::MouseOverTextFlyStr(CREATURE_CPTRC, combatDisplayPtrC_));
                flyTBoxButtonSPtr_->SetIsDisabled(MOT_FLY_STR != combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_FLY_);
                flyTBoxButtonSPtr_->SetMouseHoverText(MOT_FLY_STR);

                if (MOT_FLY_STR == combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_FLY_)
                    tBoxStdButtonSVec_.push_back(flyTBoxButtonSPtr_);
            }

            {
                const std::string MOT_LAND_STR(combat::Text::MouseOverTextLandStr(CREATURE_CPTRC, combatDisplayPtrC_));
                landTBoxButtonSPtr_->SetIsDisabled(MOT_LAND_STR != combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_LAND_);
                landTBoxButtonSPtr_->SetMouseHoverText(MOT_LAND_STR);

                if (MOT_LAND_STR == combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_LAND_)
                    tBoxStdButtonSVec_.push_back(landTBoxButtonSPtr_);
            }

            {
                const std::string MOT_ROAR_STR(combat::Text::MouseOverTextRoarStr(CREATURE_CPTRC, combatDisplayPtrC_));
                roarTBoxButtonSPtr_->SetIsDisabled(MOT_ROAR_STR != combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_ROAR_);
                roarTBoxButtonSPtr_->SetMouseHoverText(MOT_ROAR_STR);

                if (MOT_ROAR_STR == combat::Text::TBOX_BUTTON_MOUSEHOVER_TEXT_ROAR_)
                    tBoxStdButtonSVec_.push_back(roarTBoxButtonSPtr_);
            }

            {
                const std::string MOT_POUNCE_STR(combat::Text::MouseOverTextPounceStr(CREATURE_CPTRC, combatDisplayPtrC_));
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
            auto const BUTTON_ROW_WIDTH(sfml_util::MapByRes(55.0f, 135.0f));
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
        std::ostringstream enemyActionSS;
        std::ostringstream enemyCondsSS;

        auto isEnemyTurnPreambleShowing{ false };

        if (IsPlayerCharacterTurnValid())
        {
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

            infoSS << ":  " << turnCreaturePtr_->ConditionList();

            armorSS << "Armor Rating: " << turnCreaturePtr_->ArmorRating();

            enemyActionSS.str(EMPTY_STR);
            enemyCondsSS.str(EMPTY_STR);
        }
        else if (IsNonPlayerCharacterTurnValid())
        {
            titleSS << turnCreaturePtr_->Race().Name();

            if (turnCreaturePtr_->Race().Which() != creature::race::Wolfen)
                titleSS << " " << turnCreaturePtr_->Role().Name();

            //turn box weapon text (or text that indicates that a creature cannot take their turn)
            if (CAN_TAKE_ACTION_STR.empty())
            {   
                weaponHoldingSS << HOLDING_WEAPON_STR;
            }
            else
            {
                weaponHoldingSS << "Cannot take " << creature::sex::HisHerIts(turnCreaturePtr_->Sex(), false, false) << " turn because " << turnCreaturePtr_->CanTakeActionStr(false) << "!\n";
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

            if (turnCreaturePtr_->HasCondition(creature::condition::Good))
            {
                enemyCondsSS << " ";
            }
            else
            {
                auto const CONDITION_LIST_STR{ turnCreaturePtr_->ConditionList() };
                enemyCondsSS << "Condition";

                if (boost::algorithm::contains(CONDITION_LIST_STR, ","))
                    enemyCondsSS << "s";

                enemyCondsSS << ": " << CONDITION_LIST_STR;
            }

            if ((TurnPhase::DetermineAction == turnPhase_) ||
                (TurnPhase::ZoomAndSlideAnim == turnPhase_) ||
                (TurnPhase::PostZoomPause == turnPhase_) ||
                (TurnPhase::PerformAnim == turnPhase_))
            {
                if (fightResult_.Count() > 0)
                {
                    weaponHoldingSS.str(EMPTY_STR);
                    armorSS.str(EMPTY_STR);
                    enemyCondsSS.str(EMPTY_STR);

                    isEnemyTurnPreambleShowing = true;
                    
                    enemyActionSS << combat::Text::ActionText(turnCreaturePtr_,
                                                              turnActionInfo_,
                                                              fightResult_,
                                                              false,
                                                              false,
                                                              true);
                }
                else
                {
                    enemyActionSS.str(EMPTY_STR);
                }
            }
            else if (TurnPhase::PerformReport == turnPhase_)
            {
                weaponHoldingSS.str(EMPTY_STR);
                armorSS.str(EMPTY_STR);
                enemyCondsSS.str(EMPTY_STR);
                enemyActionSS << combat::Text::ActionText(turnCreaturePtr_,
                                                          turnActionInfo_,
                                                          fightResult_,
                                                          IsPlayerCharacterTurnValid(),
                                                          false,
                                                          false,
                                                          performReportEffectIndex_,
                                                          performReportHitIndex_);
            }
            else
            {
                enemyActionSS.str(EMPTY_STR);
            }
        }

        auto const VERT_POS_SHIFT(12.0f);
        titleTBoxTextRegionSPtr_->SetText(titleSS.str());
        titleTBoxTextRegionSPtr_->SetEntityPos(turnBoxRegion_.left,
                                               (turnBoxRegion_.top + (titleTBoxTextRegionSPtr_->GetEntityRegion().height * 0.5f)) - VERT_POS_SHIFT);

        weaponTBoxTextRegionSPtr_->SetText(weaponHoldingSS.str());
        weaponTBoxTextRegionSPtr_->SetEntityPos((turnBoxRegion_.left + (turnBoxRegion_.width * 0.5f)) - (weaponTBoxTextRegionSPtr_->GetEntityRegion().width * 0.5f),
                                                (titleTBoxTextRegionSPtr_->GetEntityRegion().top + titleTBoxTextRegionSPtr_->GetEntityRegion().height) - VERT_POS_SHIFT);

        armorTBoxTextRegionSPtr_->SetText(armorSS.str());
        armorTBoxTextRegionSPtr_->SetEntityPos((turnBoxRegion_.left + (turnBoxRegion_.width * 0.5f)) - (armorTBoxTextRegionSPtr_->GetEntityRegion().width * 0.5f),
                                               (weaponTBoxTextRegionSPtr_->GetEntityRegion().top + weaponTBoxTextRegionSPtr_->GetEntityRegion().height) - VERT_POS_SHIFT);

        enemyCondsTBoxRegionSPtr_->SetText(enemyCondsSS.str());
        enemyCondsTBoxRegionSPtr_->SetEntityPos((turnBoxRegion_.left + (turnBoxRegion_.width * 0.5f)) - (enemyCondsTBoxRegionSPtr_->GetEntityRegion().width * 0.5f),
                                                (armorTBoxTextRegionSPtr_->GetEntityRegion().top + armorTBoxTextRegionSPtr_->GetEntityRegion().height) - VERT_POS_SHIFT);

        auto enemyActionPosLeft{ (turnBoxRegion_.left + (turnBoxRegion_.width * 0.5f)) - (enemyActionTBoxRegionSPtr_->GetEntityRegion().width * 0.5f) };
        auto enemyActionPosTop{ (enemyCondsTBoxRegionSPtr_->GetEntityRegion().top + enemyCondsTBoxRegionSPtr_->GetEntityRegion().height) - VERT_POS_SHIFT };

        if (enemyCondsSS.str() == EMPTY_STR)
        {
            enemyActionPosTop = (armorTBoxTextRegionSPtr_->GetEntityRegion().top + armorTBoxTextRegionSPtr_->GetEntityRegion().height) - VERT_POS_SHIFT;
        }

        enemyActionTBoxRegionSPtr_->SetText(enemyActionSS.str());

        if (isEnemyTurnPreambleShowing || (TurnPhase::PerformReport == turnPhase_))
        {
            enemyActionTBoxRegionSPtr_->SetEntityPos(enemyActionPosLeft, weaponTBoxTextRegionSPtr_->GetEntityPos().y);
        }
        else
        {
            enemyActionTBoxRegionSPtr_->SetEntityPos(enemyActionPosLeft, enemyActionPosTop);
        }

        infoTBoxTextRegionSPtr_->SetText(infoSS.str());
        infoTBoxTextRegionSPtr_->SetEntityPos(turnBoxRegion_.left, turnBoxRegion_.top);

        SetupTurnBoxButtons(turnCreaturePtr_);
    }


    void CombatStage::StartAttackAnimation(creature::CreatureCPtrC_t CREATURE_ATTACKING_CPTRC,
                                           creature::CreatureCPtrC_t CREATURE_ATTACKED_CPTRC)
    {
        StartAttackAnimZoom();
        creatureAttackingCPtr_ = CREATURE_ATTACKING_CPTRC;
        creatureAttackedCPtr_ = CREATURE_ATTACKED_CPTRC;
        combatDisplayPtrC_->SetupAttackAnim(CREATURE_ATTACKING_CPTRC, CREATURE_ATTACKED_CPTRC);
    }


    void CombatStage::StopAttackAnimation()
    {
        creatureAttackingCPtr_ = nullptr;
        creatureAttackedCPtr_ = nullptr;
        combatDisplayPtrC_->StopAttackAnim();
    }


    void CombatStage::StartAttackAnimZoom()
    {
        isAttackAnimZoomOut_ = true;
        SetIsPlayerActionAllowed(false);
        zoomSlider_.Reset(ZOOM_SLIDER_SPEED_);
    }


    void CombatStage::HandleAttackStage2()
    {
        turnStateToFinish_ = combat::TurnAction::Attack;
        StopAttackAnimation();
    }


    void CombatStage::StartPerformAnim()
    {
        switch (performType_)
        {
            case PerformType::Advance:
            {
                combatDisplayPtrC_->MoveCreatureBlockingPosition(turnCreaturePtr_, true);
                PositionSlideStartTasks();
                AppendStatusMessage(combat::Text::ActionText(turnCreaturePtr_, turnActionInfo_, fightResult_, true, true), false);
                performType_ = PerformType::None;
                break;
            }

            case PerformType::Retreat:
            {
                combatDisplayPtrC_->MoveCreatureBlockingPosition(turnCreaturePtr_, false);
                PositionSlideStartTasks();
                AppendStatusMessage(combat::Text::ActionText(turnCreaturePtr_, turnActionInfo_, fightResult_, true, true), false);
                performType_ = PerformType::None;
                break;
            }

            case PerformType::Pause:
            case PerformType::Attack:
            case PerformType::ShootSling:
            case PerformType::ShootArrow:
            case PerformType::Cast:
            case PerformType::Roar:
            case PerformType::Pounce:
            case PerformType::None:
            case PerformType::Count:
            default:
            {
                break;
            }
        }

        SetTurnPhase(TurnPhase::PerformPause);
        StartPause(PERFORM_TURN_DELAY_SEC_, "StartPerformAnim");
    }


    const std::string CombatStage::TurnPhaseToString(const TurnPhase TURN_PHASE)
    {
        switch (TURN_PHASE)
        {
            case TurnPhase::NotATurn:          { return "NotATurn"; }
            case TurnPhase::CenteringAnim:     { return "CenteringAnim"; }
            case TurnPhase::PrePause:          { return "PrePause"; }
            case TurnPhase::DetermineAction:   { return "DetermineAction"; }
            case TurnPhase::ZoomAndSlideAnim:  { return "ZoomAndSlideAnim"; }
            case TurnPhase::PostZoomPause:     { return "PostZoomPause"; }
            case TurnPhase::PerformAnim:       { return "PerformAnim"; }
            case TurnPhase::PerformReport:     { return "PerformReport"; }
            case TurnPhase::PerformPause:      { return "PerformPause"; }
            case TurnPhase::StatusAnim:        { return "StatusAnim"; }
            case TurnPhase::Count:
            default:                           { return ""; }
        }
    }


    const std::string CombatStage::PerformTypeToString(const PerformType PERFORM_TYPE)
    {
        switch (PERFORM_TYPE)
        {
            case PerformType::None:       { return "None"; }
            case PerformType::Pause:      { return "Pause"; }
            case PerformType::Attack:     { return "Attack"; }
            case PerformType::ShootSling: { return "ShootSling"; }
            case PerformType::ShootArrow: { return "ShootArrow"; }
            case PerformType::Advance:    { return "Advance"; }
            case PerformType::Retreat:    { return "Retreat"; }
            case PerformType::Cast:       { return "Cast"; }
            case PerformType::Roar:       { return "Roar"; }
            case PerformType::Pounce:     { return "Pounce"; }
            case PerformType::Count:
            default:                      { return ""; }
        }
    }


    const std::string CombatStage::PreTurnPhaseToString(const PreTurnPhase PRE_TURN_PHASE)
    {
        switch (PRE_TURN_PHASE)
        {
            case PreTurnPhase::Start:             { return "Start"; }
            case PreTurnPhase::PanToCenter:       { return "PanToCenter"; }
            case PreTurnPhase::PostPanPause:      { return "PostPanPause"; }
            case PreTurnPhase::ZoomOut:           { return "ZoomOut"; }
            case PreTurnPhase::PostZoomOutPause:  { return "PostZoomOutPause"; }
            case PreTurnPhase::ZoomIn:            { return "ZoomIn"; }
            case PreTurnPhase::PostZoomInPause:   { return "PostZoomInPause"; }
            case PreTurnPhase::End:               { return "End"; }
            case PreTurnPhase::Count:
            default:                              { return ""; }
        }
    }


    void CombatStage::UpdateTestingText()
    {
        std::ostringstream ss;
        ss << "Phase=" << ((PreTurnPhase::End == preTurnPhase_) ? TurnPhaseToString(turnPhase_) : PreTurnPhaseToString(preTurnPhase_)) << ", Pause=" << pauseTitle_ << " " << ((IsPaused()) ? "D" : "A" );
        testingTextRegionSPtr_->SetText(ss.str());
    }

}
}
