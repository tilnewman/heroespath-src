// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// character-stage.cpp
//
#include "character-stage.hpp"

#include "creature/body-type.hpp"
#include "creature/condition.hpp"
#include "creature/creature-factory.hpp"
#include "creature/creature.hpp"
#include "creature/name-info.hpp"
#include "creature/race-enum.hpp"
#include "creature/race-stats.hpp"
#include "creature/role-enum.hpp"
#include "creature/role-stats.hpp"
#include "creature/sex-enum.hpp"
#include "creature/title.hpp"
#include "game/game-data-file.hpp"
#include "game/loop-manager.hpp"
#include "item/inventory.hpp"
#include "log/log-macros.hpp"
#include "misc/random.hpp"
#include "misc/real.hpp"
#include "player/party.hpp"
#include "popup/popup-manager.hpp"
#include "popup/popup-stage-image-select.hpp"
#include "sfml-util/animation-factory.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/font-manager.hpp"
#include "sfml-util/gradient-info.hpp"
#include "sfml-util/gradient.hpp"
#include "sfml-util/gui/background-info.hpp"
#include "sfml-util/gui/box.hpp"
#include "sfml-util/gui/creature-image-loader.hpp"
#include "sfml-util/gui/gui-elements.hpp"
#include "sfml-util/gui/radio-button.hpp"
#include "sfml-util/gui/text-info.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/ouroboros.hpp"
#include "sfml-util/sfml-util.hpp"
#include "sfml-util/sound-manager.hpp"
#include "sfml-util/tile.hpp"

#include <memory>
#include <sstream>

namespace heroespath
{
namespace stage
{

    MenuButton::MenuButton(
        const std::string & NAME,
        const std::string & IMAGE_FILENAME_UP,
        const std::string & IMAGE_FILENAME_OVER,
        const CharacterStagePtr_t CHARACTER_STAGE_PTR)
        : FourStateButton(std::string(NAME).append("_MenuButton"))
        , characterStagePtr_(CHARACTER_STAGE_PTR)
    {
        auto const BUTTONS_PATH{ game::GameDataFile::Instance()->GetMediaPath(
            "media-images-buttons-mainmenu-dir") };

        FourStateButton::Setup(
            0.0f,
            0.0f,
            std::string(BUTTONS_PATH).append(IMAGE_FILENAME_UP),
            "",
            std::string(BUTTONS_PATH).append(IMAGE_FILENAME_OVER),
            "",
            sfml_util::gui::MouseTextInfo(),
            sfml_util::gui::TextInfo(),
            false);
    }

    void MenuButton::OnMousePosStateChange()
    {
        if (characterStagePtr_->AreAnyAnimNumStillMoving() == false)
        {
            FourStateButton::OnMousePosStateChange();
        }
    }

    void MenuButton::OnClick(const sf::Vector2f &) { characterStagePtr_->HandleCallback(this); }

    const sfml_util::gui::ColorSet CharacterStage::GUI_DEFAULT_COLORSET_{ sf::Color(220, 220, 220),
                                                                          sf::Color(220, 220, 220),
                                                                          sf::Color(180, 180, 180),
                                                                          sf::Color(
                                                                              180, 180, 180) };

    const sf::Color CharacterStage::LIGHT_TEXT_COLOR_{ sfml_util::FontManager::Color_Light() };

    const sf::Color CharacterStage::DESC_TEXT_COLOR_{ sfml_util::FontManager::Color_Orange() };

    const std::string CharacterStage::POPUP_NAME_BACKBUTTON_LEAVESCREENCONFIRM_{
        "BackButtonLeaveScreenComfirm"
    };

    const std::string CharacterStage::POPUP_NAME_NEXTBUTTON_LEAVESCREENCONFIRM_{
        "NextButtonLeaveScreenComfirm"
    };

    const std::string CharacterStage::POPUP_NAME_NONAMEERROR_{ "CreateNoNameError" };
    const std::string CharacterStage::POPUP_NAME_MISSINGATTRIBS_{ "CreateMissingAttributesError" };
    const std::string CharacterStage::POPUP_NAME_CREATECONFIRM_{ "CreateConfirm" };
    const std::string CharacterStage::POPUP_NAME_HELP_1_{ "HelpMessage1" };
    const std::string CharacterStage::POPUP_NAME_HELP_2_{ "HelpMessage2" };
    const std::string CharacterStage::POPUP_NAME_HELP_3_{ "HelpMessage3" };
    const std::string CharacterStage::POPUP_NAME_IMAGE_SELECTION_{ "ImageSelection" };

    const stats::Trait_t CharacterStage::STAT_INVALID_{ -1 };
    const stats::Trait_t CharacterStage::STAT_INITIAL_MAX_{ 20 };

    const double CharacterStage::SMOKE_ANIM_SPEED_MIN_{ 0.05 };
    const double CharacterStage::SMOKE_ANIM_SPEED_MAX_{ 0.5 };

    CharacterStage::CharacterStage()
        : Stage("CharacterCreation")
        , SCREEN_WIDTH_(sfml_util::Display::Instance()->GetWinWidth())
        , SCREEN_HEIGHT_(sfml_util::Display::Instance()->GetWinHeight())
        , SMALL_FONT_SIZE_(sfml_util::FontManager::Instance()->Size_Small())
        , RADIO_BOX_TEXT_SIZE_(sfml_util::FontManager::Instance()->Size_Largeish())
        , STATBOX_WIDTH_(286.0f)
        , STATBOX_HEIGHT_(290.0f)
        , STATBOX_POS_LEFT_((SCREEN_WIDTH_ * 0.5f) - (STATBOX_WIDTH_ * 0.5f))
        , STATS_POS_LEFT_(STATBOX_POS_LEFT_ + 10.0f)
        , ouroborosUPtr_(std::make_unique<sfml_util::Ouroboros>("CharacterStage's"))
        , mainMenuTitle_("create_char_button_normal.png")
        , attribVertOffset1_(0.0f)
        , attribVertOffset2_(0.0f)
        , smokeAnimDrifterX_(0.0f, 1.0f, 0.1, 1.0)
        , // these drifter values are reset below
        smokeAnimDrifterY_(0.0f, 1.0f, 0.1, 1.0)
        , // these drifter values are reset below
        backgroundImage_("media-images-backgrounds-tile-darkknot")
        , smokeAnimUPtr_()
        , backButtonUPtr_(std::make_unique<MenuButton>(
              "Back", "back_button_normal.png", "back_button_lit.png", this))
        , saveButtonUPtr_(std::make_unique<MenuButton>(
              "Save", "save_button_normal.png", "save_button_lit.png", this))
        , helpButtonUPtr_(std::make_unique<MenuButton>(
              "Help", "help_button_normal.png", "help_button_lit.png", this))
        , nextButtonUPtr_(std::make_unique<MenuButton>(
              "Next", "next_button_normal.png", "next_button_lit.png", this))
        , statSetBase_()
        , statSetRace_()
        , statSetRole_()
        , statSetFixedAnim_()
        , statModifierTextVec_()
        , willDrawStatModText_(false)
        , strLabelTextRegion_("Strength")
        , accLabelTextRegion_("Accuracy")
        , chaLabelTextRegion_("Charm")
        , lckLabelTextRegion_("Luck")
        , spdLabelTextRegion_("Speed")
        , intLabelTextRegion_("Intelligence")
        , statsLineLength_(235.0f)
        , statsLine1PosTop_(0.0f)
        , statsLine2PosTop_(0.0f)
        , statsLine3PosTop_(0.0f)
        , statsLine4PosTop_(0.0f)
        , statsLine5PosTop_(0.0f)
        , statsLine6PosTop_(0.0f)
        , statsLineVertPosDiff_(0.0f)
        , statsFirstNumPosLeft_(0.0f)
        , statsStrPosTop_(0.0f)
        , statsAccPosTop_(0.0f)
        , statsChaPosTop_(0.0f)
        , statsLckPosTop_(0.0f)
        , statsSpdPosTop_(0.0f)
        , statsIntPosTop_(0.0f)
        , statsBoxUPtr_()
        , isAnimStats_(false)
        , isWaitingForStats_(false)
        , animStatsDelayPerSec_(0.0f)
        , animStatsTimeCounterSec_(0.0f)
        , animStatsSVec_()
        , fixedStatsSVec_()
        , initialRollCounter_(0)
        , dragStartY_(-1.0f)
        , closestDragStat_(stats::Traits::Count)
        , raceRadioButtonUPtr_()
        , racetDescTextRegionUPtr_()
        , roleRadioButtonSPtr_()
        , roletDescTextRegionUPtr_()
        , sexRadioButtonUPtr_()
        , nameTextEntryBoxUPtr_()
        , attrDescTextRegionUPtr_()
        , sbInsTextRegionUPtr_()
        , sbInsTextSlider_(150, 255, 4.0f)
        , nInsTextRegionUPtr_()
        , nInsTextSlider_(150, 255, 4.0f)
        , bottomSymbol_()
        , selectedImageIndex_(0)
        , characterImageFilenamesVec_()
        , woodTexture_()
    {}

    CharacterStage::~CharacterStage() { Stage::ClearAllEntities(); }

    bool CharacterStage::HandleCallback(
        const sfml_util::callback::RadioButtonCallbackPackage_t & RADIOBUTTON_WRAPPER)
    {
        // was change to race
        if (raceRadioButtonUPtr_.get() == RADIOBUTTON_WRAPPER.PTR_)
        {
            return RaceChange(
                static_cast<creature::race::Enum>(RADIOBUTTON_WRAPPER.PTR_->GetSelectedNumber()));
        }

        // was change to role
        if (roleRadioButtonSPtr_.get() == RADIOBUTTON_WRAPPER.PTR_)
        {
            return RoleChange();
        }

        return true;
    }

    bool CharacterStage::HandleCallback(const popup::PopupResponse & POPUP_RESPONSE)
    {
        if ((POPUP_RESPONSE.Info().Name() == POPUP_NAME_CREATECONFIRM_)
            && (popup::ResponseTypes::IsAffirmative(POPUP_RESPONSE.Response())))
        {
            return CreateCharacter();
        }
        else if (
            (POPUP_RESPONSE.Info().Name() == POPUP_NAME_BACKBUTTON_LEAVESCREENCONFIRM_)
            && popup::ResponseTypes::IsAffirmative(POPUP_RESPONSE.Response()))
        {
            game::LoopManager::Instance()->SetTransitionBeforeFade(sfml_util::LoopState::MainMenu);
            return true;
        }
        else if (
            (POPUP_RESPONSE.Info().Name() == POPUP_NAME_NEXTBUTTON_LEAVESCREENCONFIRM_)
            && popup::ResponseTypes::IsAffirmative(POPUP_RESPONSE.Response()))
        {
            game::LoopManager::Instance()->SetTransitionBeforeFade(
                sfml_util::LoopState::PartyCreation);
            return true;
        }
        else if (POPUP_RESPONSE.Info().Name() == POPUP_NAME_HELP_1_)
        {
            Help2Popup();
            return false;
        }
        else if (POPUP_RESPONSE.Info().Name() == POPUP_NAME_HELP_2_)
        {
            Help3Popup();
            return true;
        }
        else if (
            (POPUP_RESPONSE.Info().Name() == POPUP_NAME_IMAGE_SELECTION_)
            && (POPUP_RESPONSE.Response() != popup::ResponseTypes::Cancel))
        {
            CharacterCreationConfirmPopup(POPUP_RESPONSE.Selection());
            return false;
        }

        return true;
    }

    bool
        CharacterStage::HandleCallback(const sfml_util::gui::callback::SliderBarCallbackPackage_t &)
    {
        return false;
    }

    bool CharacterStage::HandleCallback(
        const sfml_util::gui::callback::FourStateButtonCallbackPackage_t & PACKAGE)
    {
        if (PACKAGE.PTR_ == helpButtonUPtr_.get())
        {
            return OnHelpButton();
        }

        if (PACKAGE.PTR_ == backButtonUPtr_.get())
        {
            return OnBackButton();
        }

        if (PACKAGE.PTR_ == saveButtonUPtr_.get())
        {
            return OnSaveButton();
        }

        if (PACKAGE.PTR_ == nextButtonUPtr_.get())
        {
            return OnNextButton();
        }

        return false;
    }

    void CharacterStage::Setup()
    {
        sfml_util::Loaders::Texture(
            woodTexture_,
            game::GameDataFile::Instance()->GetMediaPath("media-images-backgrounds-tile-wood"));

        EntityAdd(ouroborosUPtr_.get());

        auto const MID_SCREEN_HORIZ{ SCREEN_WIDTH_ * 0.5f };

        Setup_Button(
            helpButtonUPtr_,
            (MID_SCREEN_HORIZ - helpButtonUPtr_->GetEntityRegion().width) - 145.0f);

        Setup_Button(saveButtonUPtr_, MID_SCREEN_HORIZ + 190.0f);

        auto const BETWEEN_SPACER{ sfml_util::MapByRes(40.0f, 600.0f) };

        Setup_Button(
            backButtonUPtr_,
            (helpButtonUPtr_->GetEntityPos().x - helpButtonUPtr_->GetEntityRegion().width)
                - BETWEEN_SPACER);

        Setup_Button(
            nextButtonUPtr_,
            saveButtonUPtr_->GetEntityPos().x + saveButtonUPtr_->GetEntityRegion().width
                + BETWEEN_SPACER);

        Setup_RaceRadioButtons();
        Setup_RoleRadioButtons();
        Setup_RaceDescriptionBox();
        Setup_RoleDescriptionBox();
        auto const ATTRIB_BOX_TOP{ Setup_AttributeDescriptionBox() };
        Setup_NameLabel();
        Setup_NameTextEntryBox();
        Setup_SexRadioButtons();
        Setup_SpacebarInstructionText();

        auto const STATBOX_POS_TOP(
            sbInsTextRegionUPtr_->GetEntityRegion().top
            + sbInsTextRegionUPtr_->GetEntityRegion().height + 8.0f);

        Setup_StatBackgroundBox(STATBOX_POS_TOP);

        sfml_util::gui::TextInfo statTextInfo(
            "Strength",
            sfml_util::FontManager::Instance()->Font_Typical(),
            38,
            LIGHT_TEXT_COLOR_,
            sfml_util::Justified::Left);

        Setup_StatLabels(STATBOX_POS_TOP, statTextInfo);
        Setup_StatNumberPositions();
        Setup_FixedStats(statTextInfo);
        Setup_SmokeAnimation(ATTRIB_BOX_TOP);

        // setup initial config of radio buttons
        AdjustRoleRadioButtonsForRace(static_cast<creature::race::Enum>(0));

        Setup_RoleDescriptionBox();
    }

    void CharacterStage::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        Stage::UpdateTime(ELAPSED_TIME_SECONDS);

        // oscillate the spacebar instruction text's color to help players know what to do initially
        if (AreAnyStatsIgnored() && (false == AreAnyAnimNumStillMoving()))
        {
            auto const NEW_COLOR_VAL{ static_cast<sf::Uint8>(
                sbInsTextSlider_.Update(ELAPSED_TIME_SECONDS)) };

            auto color{ sbInsTextRegionUPtr_->GetEntityColorForeground() };
            color.r = 255;
            color.g = NEW_COLOR_VAL;
            color.b = NEW_COLOR_VAL;
            sbInsTextRegionUPtr_->SetEntityColorFgBoth(color);
        }
        else
        {
            sbInsTextRegionUPtr_->SetEntityColorFgBoth(sf::Color::White);
        }

        // oscillate the spacebar instruction text's color to help players know what to do initially
        auto const CHARACTER_NAME{ boost::algorithm::trim_copy(nameTextEntryBoxUPtr_->GetText()) };
        if (CHARACTER_NAME.empty())
        {
            auto const NEW_COLOR_VAL{ static_cast<sf::Uint8>(
                nInsTextSlider_.Update(ELAPSED_TIME_SECONDS)) };

            auto color{ nInsTextRegionUPtr_->GetEntityColorForeground() };
            color.r = 255;
            color.g = NEW_COLOR_VAL;
            color.b = NEW_COLOR_VAL;
            nInsTextRegionUPtr_->SetEntityColorFgBoth(color);
        }
        else
        {
            nInsTextRegionUPtr_->SetEntityColorFgBoth(sf::Color::White);
        }

        // drift the position of the smoke anim
        smokeAnimUPtr_->SetEntityPos(
            smokeAnimDrifterX_.Update(ELAPSED_TIME_SECONDS),
            smokeAnimDrifterY_.Update(ELAPSED_TIME_SECONDS));

        // ramp the smoke anim speed up and down when the spacebar is held
        if (isAnimStats_)
        {
            if (smokeAnimUPtr_->TimePerFrame() >= 0.01f)
            {
                smokeAnimUPtr_->TimePerFrameAdj(ELAPSED_TIME_SECONDS * 0.02f * -1.0f);
            }
        }
        else
        {
            if (smokeAnimUPtr_->TimePerFrame() <= 0.05f)
            {
                smokeAnimUPtr_->TimePerFrameAdj(ELAPSED_TIME_SECONDS * 0.01f);
            }
        }

        ProduceAnimatingDigits(ELAPSED_TIME_SECONDS);

        // draw the stat modifier text as soon as the number animations stop
        if ((false == AreAnyAnimNumStillMoving()) && (false == willDrawStatModText_)
            && (false == AreAnyStatsIgnored()))
        {
            willDrawStatModText_ = true;
            HandleChangedStatModifiers();

            sfml_util::SoundManager::Instance()->PlaySfx_AckMinor();
        }
        else
        {
            if (AreAnyAnimNumStillMoving())
            {
                willDrawStatModText_ = false;
            }
        }

        HandleStuckAnims(ELAPSED_TIME_SECONDS);
    }

    void CharacterStage::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        target.draw(backgroundImage_, STATES);
        target.draw(mainMenuTitle_, STATES);
        target.draw(bottomSymbol_, STATES);

        Stage::Draw(target, STATES);

        // Don't draw statBox_.  That is in entitySSet_.

        target.draw(strLabelTextRegion_, STATES);
        target.draw(accLabelTextRegion_, STATES);
        target.draw(chaLabelTextRegion_, STATES);
        target.draw(lckLabelTextRegion_, STATES);
        target.draw(spdLabelTextRegion_, STATES);
        target.draw(intLabelTextRegion_, STATES);

        // draw lines between stats
        sf::Vertex line1[]
            = { sf::Vector2f(STATS_POS_LEFT_ + 0.0f, statsLine1PosTop_),
                sf::Vector2f(STATS_POS_LEFT_ + 10.0f + statsLineLength_, statsLine1PosTop_) };

        sf::Vertex line2[]
            = { sf::Vector2f(STATS_POS_LEFT_ + 0.0f, statsLine2PosTop_),
                sf::Vector2f(STATS_POS_LEFT_ + 10.0f + statsLineLength_, statsLine2PosTop_) };

        sf::Vertex line3[]
            = { sf::Vector2f(STATS_POS_LEFT_ + 0.0f, statsLine3PosTop_),
                sf::Vector2f(STATS_POS_LEFT_ + 10.0f + statsLineLength_, statsLine3PosTop_) };

        sf::Vertex line4[]
            = { sf::Vector2f(STATS_POS_LEFT_ + 0.0f, statsLine4PosTop_),
                sf::Vector2f(STATS_POS_LEFT_ + 10.0f + statsLineLength_, statsLine4PosTop_) };

        sf::Vertex line5[]
            = { sf::Vector2f(STATS_POS_LEFT_ + 0.0f, statsLine5PosTop_),
                sf::Vector2f(STATS_POS_LEFT_ + 10.0f + statsLineLength_, statsLine5PosTop_) };

        sf::Vertex line6[]
            = { sf::Vector2f(STATS_POS_LEFT_ + 0.0f, statsLine6PosTop_),
                sf::Vector2f(STATS_POS_LEFT_ + 10.0f + statsLineLength_, statsLine6PosTop_) };

        const sf::Color LINE_TO_COLOR{ 255, 255, 255, 20 };
        line1[1].color = LINE_TO_COLOR;
        line2[1].color = LINE_TO_COLOR;
        line3[1].color = LINE_TO_COLOR;
        line4[1].color = LINE_TO_COLOR;
        line5[1].color = LINE_TO_COLOR;
        line6[1].color = LINE_TO_COLOR;
        //
        target.draw(line1, 2, sf::Lines);
        target.draw(line2, 2, sf::Lines);
        target.draw(line3, 2, sf::Lines);
        target.draw(line4, 2, sf::Lines);
        target.draw(line5, 2, sf::Lines);
        target.draw(line6, 2, sf::Lines);

        // draw stat modifier texts
        auto const NUM_STAT_MODS{ statModifierTextVec_.size() };
        for (std::size_t i(0); i < NUM_STAT_MODS; ++i)
        {
            target.draw(statModifierTextVec_[i], STATES);
        }

        // draw animating digits
        auto const NUM_DIGITS{ animStatsSVec_.size() };
        for (std::size_t i(0); i < NUM_DIGITS; ++i)
        {
            if (false == animStatsSVec_[i]->IgnoreMe())
            {
                target.draw(*animStatsSVec_[i], STATES);
            }
        }

        for (misc::EnumUnderlying_t i(0); i < stats::Traits::StatCount; ++i)
        {
            if (false == fixedStatsSVec_[i]->IgnoreMe())
            {
                target.draw(*fixedStatsSVec_[i], STATES);
            }
        }
    }

    bool CharacterStage::KeyPress(const sf::Event::KeyEvent & KEY_EVENT)
    {
        auto const RESULT{ Stage::KeyPress(KEY_EVENT) };

        if ((KEY_EVENT.code == sf::Keyboard::Space) && (false == isAnimStats_)
            && (false == nameTextEntryBoxUPtr_->HasFocus()))
        {
            sfml_util::SoundManager::Instance()
                ->Getsound_effect_set(sfml_util::sound_effect_set::Wind)
                .PlayRandom();

            animStatsTimeCounterSec_ = 0.0f;
            animStatsDelayPerSec_ = 0.01f; // any fraction of a second will work here
            isAnimStats_ = true;
            isWaitingForStats_ = false;
            UndoAndClearStatModifierChanges();
            return true;
        }
        else
        {
            return RESULT;
        }
    }

    bool CharacterStage::KeyRelease(const sf::Event::KeyEvent & KEY_EVENT)
    {
        if (Stage::KeyRelease(KEY_EVENT))
        {
            return true;
        }

        if (nameTextEntryBoxUPtr_->HasFocus())
        {
            if ((KEY_EVENT.code == sf::Keyboard::Return) || (KEY_EVENT.code == sf::Keyboard::Tab))
            {
                nameTextEntryBoxUPtr_->SetHasFocus(false);
                statsBoxUPtr_->SetHasFocus(true);
                Stage::SetFocus(statsBoxUPtr_.get());
            }

            return true;
        }

        if (KEY_EVENT.code == sf::Keyboard::Space)
        {
            isAnimStats_ = false;
            isWaitingForStats_ = true;
            return true;
        }
        else
        {
            return HandleMenuNavigationKeyRelease(KEY_EVENT);
        }
    }

    void CharacterStage::UpdateMouseDown(const sf::Vector2f & MOUSE_POS_V)
    {
        if (false == AreAnyAnimNumStillMoving())
        {
            Stage::UpdateMouseDown(MOUSE_POS_V);
        }

        if ((initialRollCounter_ >= 6) && (false == AreAnyAnimNumStillMoving()))
        {
            auto isNumberHeldDown{ false };
            for (misc::EnumUnderlying_t i(0); i < stats::Traits::StatCount; ++i)
            {
                if (fixedStatsSVec_[i]->MouseDown(MOUSE_POS_V.x, MOUSE_POS_V.y))
                {
                    isNumberHeldDown = true;
                }
            }

            // you can hold down the mouse button on a stat to see all stats at their base values
            if (isNumberHeldDown)
            {
                UndoAndClearStatModifierChanges();
                SetVisibleStatsToStatSetBase();
            }
        }
    }

    void CharacterStage::Setup_Button(MenuButtonUPtr_t & buttonUPtr, const float VERT_POS)
    {
        buttonUPtr->SetScaleToRes();
        buttonUPtr->SetVertPositionToBottomOfScreenByRes(VERT_POS);
        EntityAdd(buttonUPtr.get());
    }

    void CharacterStage::Setup_RaceRadioButtons()
    {
        sfml_util::gui::TextInfo raceRadioButtonSetTextInfo(
            " ",
            sfml_util::FontManager::Instance()->Font_Default2(),
            RADIO_BOX_TEXT_SIZE_,
            LIGHT_TEXT_COLOR_,
            sfml_util::Justified::Left);

        std::vector<std::string> raceNameVec;
        for (misc::EnumUnderlying_t i(0); i < creature::race::Count_PlayerRaces; ++i)
        {
            raceNameVec.emplace_back(creature::race::Name(static_cast<creature::race::Enum>(i)));
        }

        const sf::FloatRect REGION(
            sfml_util::MapByRes(40.0f, 1525.0f), mainMenuTitle_.Bottom(), 0.0f, 0.0f);

        const sfml_util::gui::BackgroundInfo BG_INFO(woodTexture_, REGION);

        const sfml_util::gui::box::Info BOX_INFO(true, REGION, GUI_DEFAULT_COLORSET_, BG_INFO);

        raceRadioButtonUPtr_ = std::make_unique<sfml_util::gui::RadioButtonSet>(
            sfml_util::callback::RadioButtonSetCallbackHandlerPtr_t(this),
            "RaceSelection",
            REGION.left,
            REGION.top,
            raceRadioButtonSetTextInfo,
            raceNameVec,
            0,
            sfml_util::Brightness::Bright,
            misc::SizetVec_t(),
            BOX_INFO,
            sfml_util::gui::RadioButtonSet::BETWEEN_PAD_DEFAULT_,
            0.0f);

        EntityAdd(raceRadioButtonUPtr_.get());
    }

    void CharacterStage::Setup_RoleRadioButtons()
    {
        sfml_util::gui::TextInfo roleRadioButtonSetTextInfo(
            " ",
            sfml_util::FontManager::Instance()->Font_Default2(),
            RADIO_BOX_TEXT_SIZE_,
            LIGHT_TEXT_COLOR_,
            sfml_util::Justified::Left);

        std::vector<std::string> roleNameVec;
        for (std::size_t i(0); i < creature::role::PlayerRoleCount; ++i)
        {
            auto const ROLE{ static_cast<creature::role::Enum>(i) };
            if (ROLE != creature::role::Wolfen)
            {
                roleNameVec.emplace_back(creature::role::Name(ROLE));
            }
        }

        auto const POS_TOP{ raceRadioButtonUPtr_->GetEntityRegion().top
                            + raceRadioButtonUPtr_->GetEntityRegion().height
                            + sfml_util::MapByRes(35.0f, 75.0f) };

        const sf::FloatRect REGION(sfml_util::MapByRes(15.0f, 1300.0f), POS_TOP, 0.0f, 0.0f);

        const sfml_util::gui::BackgroundInfo BG_INFO(woodTexture_, REGION);

        const sfml_util::gui::box::Info BOX_INFO(true, REGION, GUI_DEFAULT_COLORSET_, BG_INFO);

        roleRadioButtonSPtr_ = std::make_unique<sfml_util::gui::RadioButtonSet>(
            sfml_util::callback::RadioButtonSetCallbackHandlerPtr_t(this),
            "RoleSelection",
            REGION.left,
            REGION.top,
            roleRadioButtonSetTextInfo,
            roleNameVec,
            0,
            sfml_util::Brightness::Bright,
            misc::SizetVec_t(),
            BOX_INFO,
            sfml_util::gui::RadioButtonSet::BETWEEN_PAD_DEFAULT_,
            -5.0f);

        EntityAdd(roleRadioButtonSPtr_.get());
    }

    void CharacterStage::Setup_RaceDescriptionBox()
    {
        auto const RACE_ENUM{ static_cast<creature::race::Enum>(
            raceRadioButtonUPtr_->GetSelectedNumber()) };

        const std::string RACE_DESC(creature::race::Desc(RACE_ENUM));

        const sf::FloatRect REGION(
            raceRadioButtonUPtr_->GetEntityRegion().left
                + raceRadioButtonUPtr_->GetEntityRegion().width + 15.0f,
            raceRadioButtonUPtr_->GetEntityPos().y + sfml_util::MapByRes(15.0f, 45.0f),
            sfml_util::MapByRes(232.0f, 1500.0f),
            raceRadioButtonUPtr_->GetEntityRegion().height - 30.0f);

        sfml_util::gui::TextInfo raceDescTextInfo(
            RACE_DESC,
            sfml_util::FontManager::Instance()->Font_Default2(),
            30,
            DESC_TEXT_COLOR_,
            sfml_util::Justified::Left);

        if (!racetDescTextRegionUPtr_)
        {
            racetDescTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
                "RaceDescription",
                raceDescTextInfo,
                REGION,
                sfml_util::IStagePtr_t(this),
                SMALL_FONT_SIZE_);

            EntityAdd(racetDescTextRegionUPtr_.get());
        }
        else
        {
            racetDescTextRegionUPtr_->Setup(
                raceDescTextInfo, REGION, sfml_util::IStagePtr_t(this), SMALL_FONT_SIZE_);
        }
    }

    void CharacterStage::Setup_RoleDescriptionBox()
    {
        auto const RACE_ENUM{ static_cast<creature::race::Enum>(
            raceRadioButtonUPtr_->GetSelectedNumber()) };

        auto const ROLE_SELECTED{ static_cast<creature::role::Enum>(
            roleRadioButtonSPtr_->GetSelectedNumber()) };

        auto const ROLE_ENUM{ (
            (RACE_ENUM == creature::race::Wolfen) ? creature::role::Wolfen : ROLE_SELECTED) };

        auto const ROLE_RADIOBUTTON_REGION{ roleRadioButtonSPtr_->GetEntityRegion() };

        const sf::FloatRect REGION(
            ROLE_RADIOBUTTON_REGION.left + ROLE_RADIOBUTTON_REGION.width + 15.0f,
            ROLE_RADIOBUTTON_REGION.top + sfml_util::MapByRes(25.0f, 75.0f),
            sfml_util::MapByRes(240.0f, 1750.0f),
            roleRadioButtonSPtr_->GetEntityRegion().height - 30.0f);

        sfml_util::gui::TextInfo roleDescTextInfo(
            creature::role::Desc(ROLE_ENUM),
            sfml_util::FontManager::Instance()->Font_Dialog2(),
            30,
            DESC_TEXT_COLOR_,
            sfml_util::Justified::Left);

        if (!roletDescTextRegionUPtr_)
        {
            roletDescTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
                "RoleDescription",
                roleDescTextInfo,
                REGION,
                sfml_util::IStagePtr_t(this),
                SMALL_FONT_SIZE_);

            EntityAdd(roletDescTextRegionUPtr_.get());
        }
        else
        {
            roletDescTextRegionUPtr_->Setup(
                roleDescTextInfo, REGION, sfml_util::IStagePtr_t(this), SMALL_FONT_SIZE_);
        }
    }

    void CharacterStage::Setup_NameLabel()
    {
        const sf::FloatRect REGION(
            (SCREEN_WIDTH_ * 0.5f) - 150.0f, mainMenuTitle_.Bottom() - 20.0f, 0.0f, 0.0f);

        const sfml_util::gui::TextInfo NAME_LABEL_TEXT_INFO(
            "(name your character here)",
            sfml_util::FontManager::Instance()->Font_Default2(),
            sfml_util::FontManager::Instance()->Size_Small(),
            LIGHT_TEXT_COLOR_,
            sf::BlendAlpha,
            sf::Text::Italic);

        nInsTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "NameLabel", NAME_LABEL_TEXT_INFO, REGION);

        nInsTextRegionUPtr_->SetEntityPos(
            (SCREEN_WIDTH_ * 0.5f) - (nInsTextRegionUPtr_->GetEntityRegion().width * 0.5f) + 45.0f,
            nInsTextRegionUPtr_->GetEntityPos().y);

        EntityAdd(nInsTextRegionUPtr_.get());
    }

    void CharacterStage::Setup_NameTextEntryBox()
    {
        creature::NameInfo creatureNameInfo;

        auto const WIDTH{ creatureNameInfo.DefaultTextEntryBoxWidth() };

        const sf::FloatRect REGION(
            (SCREEN_WIDTH_ * 0.5f) - (WIDTH * 0.5f),
            nInsTextRegionUPtr_->GetEntityRegion().top
                + nInsTextRegionUPtr_->GetEntityRegion().height,
            WIDTH,
            55.0f);

        const sfml_util::gui::BackgroundInfo BG_INFO(woodTexture_, REGION);

        const sfml_util::gui::box::Info BOX_INFO(true, REGION, GUI_DEFAULT_COLORSET_, BG_INFO);

        nameTextEntryBoxUPtr_ = std::make_unique<sfml_util::gui::TextEntryBox>(
            sfml_util::gui::callback::ITextEntryBoxCallbackHandlerPtr_t(this),
            "CharacterName",
            REGION,
            creatureNameInfo.MakeTextInfo(" ", DESC_TEXT_COLOR_),
            LIGHT_TEXT_COLOR_,
            BOX_INFO);

        EntityAdd(nameTextEntryBoxUPtr_.get());
    }

    void CharacterStage::Setup_SexRadioButtons()
    {
        sfml_util::gui::TextInfo sexRadioButtonSetTextInfo(
            " ",
            sfml_util::FontManager::Instance()->Font_Default2(),
            RADIO_BOX_TEXT_SIZE_,
            LIGHT_TEXT_COLOR_,
            sfml_util::Justified::Left);

        std::vector<std::string> sexNameVec;
        sexNameVec.emplace_back("Male");
        sexNameVec.emplace_back("Female");

        const sf::FloatRect TEMP_EMPTY_REGION(0.0f, 0.0f, 0.0f, 0.0f);

        const sfml_util::gui::BackgroundInfo BG_INFO(woodTexture_, TEMP_EMPTY_REGION);

        const sfml_util::gui::box::Info BOX_INFO(
            true, TEMP_EMPTY_REGION, GUI_DEFAULT_COLORSET_, BG_INFO);

        sexRadioButtonUPtr_ = std::make_unique<sfml_util::gui::RadioButtonSet>(
            sfml_util::callback::RadioButtonSetCallbackHandlerPtr_t(this),
            "SexSelection",
            0.0f,
            0.0f,
            sexRadioButtonSetTextInfo,
            sexNameVec,
            0,
            sfml_util::Brightness::Bright,
            misc::SizetVec_t(),
            BOX_INFO,
            5.0f,
            0.0f);

        sexRadioButtonUPtr_->SetEntityPos(
            (SCREEN_WIDTH_ * 0.5f) - (sexRadioButtonUPtr_->GetEntityRegion().width * 0.5f),
            nameTextEntryBoxUPtr_->GetEntityPos().y
                + nameTextEntryBoxUPtr_->GetEntityRegion().height
                + sfml_util::MapByRes(25.0f, 70.0f));

        EntityAdd(sexRadioButtonUPtr_.get());
    }

    void CharacterStage::Setup_SpacebarInstructionText()
    {
        std::stringstream spacebarInstrTextSS;
        spacebarInstrTextSS
            << "(click the box below then hold the spacebar to change "
            << "attributes)\n(drag numbers up and down to customize)                ";

        sfml_util::gui::TextInfo insTextInfo(
            spacebarInstrTextSS.str(),
            sfml_util::FontManager::Instance()->Font_Typical(),
            sfml_util::FontManager::Instance()->Size_Small(),
            LIGHT_TEXT_COLOR_,
            sf::BlendAlpha,
            sf::Text::Italic,
            sfml_util::Justified::Center);

        sbInsTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "SpacebarInstructions", insTextInfo, sf::FloatRect());

        sbInsTextRegionUPtr_->SetEntityPos(
            ((SCREEN_WIDTH_ * 0.5f) - (sbInsTextRegionUPtr_->GetEntityRegion().width * 0.5f))
                + 100.0f,
            sexRadioButtonUPtr_->GetEntityRegion().top
                + sexRadioButtonUPtr_->GetEntityRegion().height
                + sfml_util::MapByRes(30.0f, 90.0f));

        EntityAdd(sbInsTextRegionUPtr_.get());
    }

    void CharacterStage::Setup_StatBackgroundBox(const float STATBOX_POS_TOP)
    {
        const sf::FloatRect RECT(
            STATBOX_POS_LEFT_, STATBOX_POS_TOP, STATBOX_WIDTH_, STATBOX_HEIGHT_);

        const sfml_util::gui::BackgroundInfo BG_INFO(woodTexture_, RECT);

        sfml_util::gui::box::Info boxInfo(
            true,
            RECT,
            sfml_util::gui::ColorSet(
                sf::Color(220, 220, 220),
                sf::Color(220, 220, 220),
                sf::Color(180, 180, 180),
                sf::Color(180, 180, 180)),
            BG_INFO);

        statsBoxUPtr_ = std::make_unique<sfml_util::gui::box::Box>("CharacterStageStats", boxInfo);

        EntityAdd(statsBoxUPtr_.get());
    }

    void CharacterStage::Setup_StatLabels(
        const float STATBOX_POS_TOP, const sfml_util::gui::TextInfo & STAT_TEXT_INFO)
    {
        auto const STATS_TEXT_VERT_OFFSET{ -13.0f };
        auto const STATS_LINE_VERT_OFFSET{ -7.0f };

        sfml_util::gui::TextInfo tempTextInfo{ STAT_TEXT_INFO };

        tempTextInfo.text = "Strength";
        sf::FloatRect rect(STATS_POS_LEFT_, STATBOX_POS_TOP, 0.0f, 0.0f);
        strLabelTextRegion_.Setup(tempTextInfo, rect);

        tempTextInfo.text = "Accuracy";
        rect.top += strLabelTextRegion_.GetEntityRegion().height + STATS_TEXT_VERT_OFFSET;
        statsLine1PosTop_ = rect.top + STATS_LINE_VERT_OFFSET;
        accLabelTextRegion_.Setup(tempTextInfo, rect);

        tempTextInfo.text = "Charm";
        rect.top += accLabelTextRegion_.GetEntityRegion().height + STATS_TEXT_VERT_OFFSET;
        statsLine2PosTop_ = rect.top + STATS_LINE_VERT_OFFSET;
        chaLabelTextRegion_.Setup(tempTextInfo, rect);

        tempTextInfo.text = "Luck";
        rect.top += chaLabelTextRegion_.GetEntityRegion().height + STATS_TEXT_VERT_OFFSET;
        statsLine3PosTop_ = rect.top + STATS_LINE_VERT_OFFSET;
        lckLabelTextRegion_.Setup(tempTextInfo, rect);

        tempTextInfo.text = "Speed";
        rect.top += lckLabelTextRegion_.GetEntityRegion().height + STATS_TEXT_VERT_OFFSET;
        statsLine4PosTop_ = rect.top + STATS_LINE_VERT_OFFSET;
        spdLabelTextRegion_.Setup(tempTextInfo, rect);

        tempTextInfo.text = "Intelligence";
        rect.top += spdLabelTextRegion_.GetEntityRegion().height + STATS_TEXT_VERT_OFFSET;
        statsLine5PosTop_ = rect.top + STATS_LINE_VERT_OFFSET;
        intLabelTextRegion_.Setup(tempTextInfo, rect);

        rect.top += intLabelTextRegion_.GetEntityRegion().height + STATS_TEXT_VERT_OFFSET;
        statsLine6PosTop_ = rect.top + STATS_LINE_VERT_OFFSET;
    }

    void CharacterStage::Setup_StatNumberPositions()
    {
        auto const STAT_NUM_HORIZ_OFFSET{ statsLineLength_ - 40.0f };

        statsFirstNumPosLeft_ = STATS_POS_LEFT_ + STAT_NUM_HORIZ_OFFSET;
        attribVertOffset1_ = -40.0f;
        attribVertOffset2_ = (statsLine2PosTop_ - statsLine1PosTop_);

        statsStrPosTop_ = GetAttributeNumPosTop(stats::Traits::Strength);
        statsAccPosTop_ = GetAttributeNumPosTop(stats::Traits::Accuracy);
        statsChaPosTop_ = GetAttributeNumPosTop(stats::Traits::Charm);
        statsLckPosTop_ = GetAttributeNumPosTop(stats::Traits::Luck);
        statsSpdPosTop_ = GetAttributeNumPosTop(stats::Traits::Speed);
        statsIntPosTop_ = GetAttributeNumPosTop(stats::Traits::Intelligence);

        statsLineVertPosDiff_ = statsIntPosTop_ - statsSpdPosTop_;
    }

    void CharacterStage::Setup_FixedStats(const sfml_util::gui::TextInfo & STAT_TEXT_INFO)
    {
        // fill the fixed stats vector with dummy values initially...
        const std::size_t NUM_TRAITS(stats::Traits::StatCount);
        for (std::size_t i(0); i < NUM_TRAITS; ++i)
        {
            fixedStatsSVec_.emplace_back(AnimNumSPtr_t());
        }

        //...then assign valid AnimNumSPtr_t objects with a value of zero
        fixedStatsSVec_[stats::Traits::Strength] = std::make_shared<AnimNum>(
            STAT_INVALID_,
            stats::Traits::Strength,
            statsFirstNumPosLeft_,
            statsStrPosTop_,
            statsFirstNumPosLeft_,
            statsStrPosTop_,
            STAT_TEXT_INFO);

        fixedStatsSVec_[stats::Traits::Accuracy] = std::make_shared<AnimNum>(
            STAT_INVALID_,
            stats::Traits::Accuracy,
            statsFirstNumPosLeft_,
            statsAccPosTop_,
            statsFirstNumPosLeft_,
            statsAccPosTop_,
            STAT_TEXT_INFO);

        fixedStatsSVec_[stats::Traits::Charm] = std::make_shared<AnimNum>(
            STAT_INVALID_,
            stats::Traits::Charm,
            statsFirstNumPosLeft_,
            statsChaPosTop_,
            statsFirstNumPosLeft_,
            statsChaPosTop_,
            STAT_TEXT_INFO);

        fixedStatsSVec_[stats::Traits::Luck] = std::make_shared<AnimNum>(
            STAT_INVALID_,
            stats::Traits::Luck,
            statsFirstNumPosLeft_,
            statsLckPosTop_,
            statsFirstNumPosLeft_,
            statsLckPosTop_,
            STAT_TEXT_INFO);

        fixedStatsSVec_[stats::Traits::Speed] = std::make_shared<AnimNum>(
            STAT_INVALID_,
            stats::Traits::Speed,
            statsFirstNumPosLeft_,
            statsSpdPosTop_,
            statsFirstNumPosLeft_,
            statsSpdPosTop_,
            STAT_TEXT_INFO);

        fixedStatsSVec_[stats::Traits::Intelligence] = std::make_shared<AnimNum>(
            STAT_INVALID_,
            stats::Traits::Intelligence,
            statsFirstNumPosLeft_,
            statsIntPosTop_,
            statsFirstNumPosLeft_,
            statsIntPosTop_,
            STAT_TEXT_INFO);
    }

    void CharacterStage::Setup_SmokeAnimation(const float ATTRIB_BOX_TOP)
    {
        smokeAnimUPtr_ = sfml_util::AnimationFactory::Make(
            sfml_util::Animations::SmokeSwirl,
            sfml_util::MapByRes(1.0f, 3.0f),
            0.035f,
            sf::Color::White,
            sf::BlendAlpha);

        EntityAdd(smokeAnimUPtr_.get());

        auto const DRIFT_LIMIT_LEFT{ SCREEN_WIDTH_ * 0.65f };
        auto const DRIFT_LIMIT_RIGHT{ SCREEN_WIDTH_ - smokeAnimUPtr_->GetEntityRegion().width };

        smokeAnimDrifterX_.Reset(
            DRIFT_LIMIT_LEFT,
            DRIFT_LIMIT_RIGHT,
            SMOKE_ANIM_SPEED_MIN_,
            SMOKE_ANIM_SPEED_MAX_,
            misc::random::Float(DRIFT_LIMIT_LEFT, DRIFT_LIMIT_RIGHT),
            misc::random::Float(DRIFT_LIMIT_LEFT, DRIFT_LIMIT_RIGHT));

        auto const VERT_OVERLAP{ sfml_util::MapByRes(30.0f, 150.0f) };

        auto const DRIFT_LIMIT_TOP{ mainMenuTitle_.Bottom(false) - VERT_OVERLAP };

        auto const DRIFT_LIMIT_BOTTOM{ (ATTRIB_BOX_TOP - smokeAnimUPtr_->GetEntityRegion().height)
                                       + (VERT_OVERLAP * 2.0f) };

        smokeAnimDrifterY_.Reset(
            DRIFT_LIMIT_TOP,
            DRIFT_LIMIT_BOTTOM,
            SMOKE_ANIM_SPEED_MIN_,
            SMOKE_ANIM_SPEED_MAX_,
            misc::random::Float(DRIFT_LIMIT_TOP, DRIFT_LIMIT_BOTTOM),
            misc::random::Float(DRIFT_LIMIT_TOP, DRIFT_LIMIT_BOTTOM));
    }

    float CharacterStage::Setup_AttributeDescriptionBox()
    {
        auto const ATTR_DESC_WIDTH{ sfml_util::MapByRes(335.0f, 3000.0f) };

        auto const REGION_TOP{ sfml_util::MapByRes(450.0f, 1600.0f) };

        const sf::FloatRect REGION(
            (SCREEN_WIDTH_ - ATTR_DESC_WIDTH) - sfml_util::MapByRes(15.0f, 300.0f),
            REGION_TOP,
            ATTR_DESC_WIDTH,
            sfml_util::MapByRes(310.0f, 2100.0f));

        sfml_util::gui::TextInfo descTextInfo(
            "", // see below for where this is set to a valid value
            sfml_util::FontManager::Instance()->Font_Default2(),
            SMALL_FONT_SIZE_,
            DESC_TEXT_COLOR_,
            sfml_util::Justified::Center);

        sfml_util::gui::TextRegionUVec_t textRegionUVec;

        // Setup the strength attribute here because it is special.
        // See below where it is added by hand.
        std::ostringstream ss;
        ss << stats::Traits::Name(stats::Traits::Strength) << " - "
           << game::GameDataFile::Instance()->GetCopyStr("heroespath-stats-stat-desc_Strength")
           << "\n\n";

        auto const STRENGTH_BASE_TEXT{ ss.str() };

        sfml_util::gui::TextInfo strHelpTextInfo(descTextInfo);

        Setup_AttributeHelpText(stats::Traits::Strength, REGION, strHelpTextInfo, textRegionUVec);

        Setup_Attribute(
            stats::Traits::Accuracy,
            "heroespath-stats-stat-desc_Accuracy",
            REGION,
            descTextInfo,
            textRegionUVec);

        Setup_Attribute(
            stats::Traits::Charm,
            "heroespath-stats-stat-desc_Charm",
            REGION,
            descTextInfo,
            textRegionUVec);

        Setup_Attribute(
            stats::Traits::Luck,
            "heroespath-stats-stat-desc_Luck",
            REGION,
            descTextInfo,
            textRegionUVec);

        Setup_Attribute(
            stats::Traits::Speed,
            "heroespath-stats-stat-desc_Speed",
            REGION,
            descTextInfo,
            textRegionUVec);

        Setup_Attribute(
            stats::Traits::Intelligence,
            "heroespath-stats-stat-desc_Intelligence",
            REGION,
            descTextInfo,
            textRegionUVec);

        // setup the final TextRegion that will be drawn to the screen
        descTextInfo.text = STRENGTH_BASE_TEXT;
        if (!attrDescTextRegionUPtr_)
        {
            attrDescTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
                "AttributeDescription", descTextInfo, REGION, sfml_util::IStagePtr_t(this));

            EntityAdd(attrDescTextRegionUPtr_.get());
        }
        else
        {
            attrDescTextRegionUPtr_->Setup(descTextInfo, REGION, sfml_util::IStagePtr_t(this));
        }

        for (auto const & NEXT_TR_UPTR : textRegionUVec)
        {
            attrDescTextRegionUPtr_->Append(*NEXT_TR_UPTR);
        }

        return REGION_TOP;
    }

    void CharacterStage::Setup_Attribute(
        const stats::Traits::Enum TRAIT_ENUM,
        const std::string & DESC_KEY,
        const sf::FloatRect & REGION,
        sfml_util::gui::TextInfo & descTextInfo,
        sfml_util::gui::TextRegionUVec_t & textRegionUVec)
    {
        std::ostringstream ss;

        ss << stats::Traits::Name(TRAIT_ENUM) << " - "
           << game::GameDataFile::Instance()->GetCopyStr(DESC_KEY) << "\n\n";

        descTextInfo.text = ss.str();
        textRegionUVec.emplace_back(std::make_unique<sfml_util::gui::TextRegion>(
            stats::Traits::ToString(TRAIT_ENUM) + "StatDesc",
            descTextInfo,
            REGION,
            sfml_util::gui::TextRegion::DEFAULT_NO_RESIZE_,
            sfml_util::gui::box::Info(),
            sfml_util::Margins()));

        sfml_util::gui::TextInfo helpTextInfo(descTextInfo);

        Setup_AttributeHelpText(TRAIT_ENUM, REGION, helpTextInfo, textRegionUVec);
    }

    void CharacterStage::Setup_AttributeHelpText(
        const stats::Traits::Enum TRAIT_ENUM,
        const sf::FloatRect & REGION,
        sfml_util::gui::TextInfo & helpTextInfo,
        sfml_util::gui::TextRegionUVec_t & textRegionUVec)
    {
        GetStatHelpText(TRAIT_ENUM, helpTextInfo);

        if (helpTextInfo.text.empty() == false)
        {
            textRegionUVec.emplace_back(std::make_unique<sfml_util::gui::TextRegion>(
                stats::Traits::ToString(TRAIT_ENUM) + "AttributeDesc",
                helpTextInfo,
                REGION,
                sfml_util::gui::TextRegion::DEFAULT_NO_RESIZE_,
                sfml_util::gui::box::Info(),
                sfml_util::Margins()));
        }
    }

    bool CharacterStage::OnSaveButton()
    {
        if (AreAnyAnimNumStillMoving())
        {
            return false;
        }

        if (AreAnyStatsIgnored())
        {
            MissingAttributesPopup();
            return false;
        }

        // verify name is not blank/empty
        auto const CHARACTER_NAME{ boost::algorithm::trim_copy(nameTextEntryBoxUPtr_->GetText()) };
        if (CHARACTER_NAME.empty())
        {
            CharacterNameMissingPopup();
            return false;
        }

        CharacterImageSelectionPopup(CHARACTER_NAME);
        return true;
    }

    bool CharacterStage::OnBackButton()
    {
        if (AreAnyAnimNumStillMoving())
        {
            return false;
        }
        else
        {
            LeaveStageConfirmPopup(boost::algorithm::trim_copy(nameTextEntryBoxUPtr_->GetText()));

            return true;
        }
    }

    bool CharacterStage::OnHelpButton()
    {
        Help1Popup();
        return true;
    }

    bool CharacterStage::OnNextButton()
    {
        // verify all stats are ready
        if (AreAnyAnimNumStillMoving())
        {
            return false;
        }

        auto const NAME{ boost::algorithm::trim_copy(nameTextEntryBoxUPtr_->GetText()) };

        // if it seems the user has been working on a character, prompt before losing the data
        if ((false == AreAnyStatsIgnored()) || (false == NAME.empty()))
        {
            std::ostringstream ss;
            ss << "\n\nLeaving will cause ";

            if (NAME.empty())
            {
                ss << "this character ";
            }
            else
            {
                ss << "the character \"" << NAME << "\" ";
            }

            ss << "to be lost.  Are you sure?";

            game::LoopManager::Instance()->PopupWaitBegin(
                this,
                popup::PopupManager::Instance()->CreatePopupInfo(
                    POPUP_NAME_NEXTBUTTON_LEAVESCREENCONFIRM_,
                    ss.str(),
                    popup::PopupButtons::YesNo,
                    popup::PopupImage::Regular,
                    sfml_util::Justified::Center,
                    sfml_util::sound_effect::PromptWarn));

            return true;
        }
        else
        {
            game::LoopManager::Instance()->TransitionTo_PartyCreation();
        }

        return true;
    }

    void CharacterStage::MissingAttributesPopup()
    {
        std::ostringstream ss;
        ss << "The following attributes are blank: ";

        bool foundAnyToIgnore(false);
        for (misc::EnumUnderlying_t i(0); i < stats::Traits::StatCount; ++i)
        {
            if (fixedStatsSVec_[i]->IgnoreMe())
            {
                ss << ((false == foundAnyToIgnore) ? "" : ", ")
                   << stats::Traits::Name(static_cast<stats::Traits::Enum>(i));

                foundAnyToIgnore = true;
            }
        }

        ss << ".  All attributes must have a value before your character can be created.  "
           << "Hold down the spacebar until all attributes have values.";

        auto const POPUP_INFO{ popup::PopupManager::Instance()->CreatePopupInfo(
            POPUP_NAME_MISSINGATTRIBS_,
            ss.str(),
            popup::PopupButtons::Okay,
            popup::PopupImage::RegularSidebar,
            sfml_util::Justified::Left,
            sfml_util::sound_effect::PromptQuestion) };

        game::LoopManager::Instance()->PopupWaitBegin(this, POPUP_INFO);
    }

    void CharacterStage::CharacterNameMissingPopup()
    {
        game::LoopManager::Instance()->PopupWaitBegin(
            this,
            popup::PopupManager::Instance()->CreatePopupInfo(
                POPUP_NAME_NONAMEERROR_,
                "\n\nThe name box is empty.  You must name your character to continue.",
                popup::PopupButtons::Okay,
                popup::PopupImage::Regular,
                sfml_util::Justified::Center,
                sfml_util::sound_effect::PromptWarn));
    }

    void CharacterStage::CharacterImageSelectionPopup(const std::string & CHARACTER_NAME)
    {
        auto const SEX{ GetCurrentSelectedSex() };

        auto const RACE{ static_cast<creature::race::Enum>(
            raceRadioButtonUPtr_->GetSelectedNumber()) };

        auto const ROLE{ (
            (RACE == creature::race::Wolfen)
                ? creature::role::Wolfen
                : static_cast<creature::role::Enum>(roleRadioButtonSPtr_->GetSelectedNumber())) };

        sfml_util::gui::CreatureImageLoader creatureImageLoader;

        characterImageFilenamesVec_ = creatureImageLoader.GetFilenames(RACE, ROLE, SEX);

        sfml_util::TextureVec_t characterTextureVec;
        for (auto const & NEXT_FILENAME_STR : characterImageFilenamesVec_)
        {
            sf::Texture texture;

            creatureImageLoader.GetImageFromFilename(texture, NEXT_FILENAME_STR, true);

            characterTextureVec.emplace_back(texture);
        }

        std::ostringstream ss;
        ss << "Choose an image for \"" << CHARACTER_NAME << "\"";

        auto const POPUP_INFO{ popup::PopupManager::Instance()->CreateImageSelectionPopupInfo(
            POPUP_NAME_IMAGE_SELECTION_, ss.str(), characterTextureVec, true) };

        game::LoopManager::Instance()->PopupWaitBeginSpecific<popup::PopupStageImageSelect>(
            this, POPUP_INFO);
    }

    void CharacterStage::LeaveStageConfirmPopup(const std::string & CHARACTER_NAME)
    {
        // if it seems the user has been working on a character, prompt before losing the data
        if ((AreAnyStatsIgnored() == false) || (CHARACTER_NAME.empty() == false))
        {
            std::ostringstream ss;
            ss << "\nLeaving will cause ";

            if (CHARACTER_NAME.empty())
            {
                ss << "this character ";
            }
            else
            {
                ss << "the character \"" << CHARACTER_NAME << "\" ";
            }

            ss << "to be lost.  Are you sure?";

            game::LoopManager::Instance()->PopupWaitBegin(
                this,
                popup::PopupManager::Instance()->CreatePopupInfo(
                    POPUP_NAME_BACKBUTTON_LEAVESCREENCONFIRM_,
                    ss.str(),
                    popup::PopupButtons::YesNo,
                    popup::PopupImage::Regular,
                    sfml_util::Justified::Center,
                    sfml_util::sound_effect::PromptWarn));
        }
        else
        {
            game::LoopManager::Instance()->TransitionTo_MainMenu();
        }
    }

    void CharacterStage::Help1Popup()
    {
        game::LoopManager::Instance()->PopupWaitBegin(
            this,
            popup::PopupManager::Instance()->CreatePopupInfo(
                POPUP_NAME_HELP_1_,
                std::string(
                    "You play Heroes' Path with a group of six characters called a party.  ")
                    + "This is where you create those characters one at a time. "
                    + "The first thing to do is decide what Roles you want those characters to "
                      "have.  "
                    + "Role is simply a character's occupation, or type.  You should choose the "
                    + "combination of Roles in your party carefully, so they work well together as "
                    + "a team." + "\n\n"
                    + "If you are new to the game, there are four recommended sets of "
                    + "Roles that are known to be effective and fun to play.  Click continue "
                    + "below to see them.",
                popup::PopupButtons::Continue,
                popup::PopupImage::Large));
    }

    void CharacterStage::Help2Popup()
    {
        game::LoopManager::Instance()->PopupWaitBegin(
            this,
            popup::PopupManager::Instance()->CreatePopupInfo(
                POPUP_NAME_HELP_2_,
                std::string("Classic Set:\n\nKnight\nArcher\nBard\nThief\nCleric\nSorcerer")
                    + "\n\n\nPet Set:\n\nKnight\nBeastmaster\nDragon or Wolfen\nBard\nThief\nCleric"
                    + "\n\n\nWing Set:\n\nBeastmaster\nFirebrand Dragon\nSylavin Dragon\nSorcerer "
                      "or "
                    + "Bard\nThief\nCleric"
                    + "\n\n\nHowler Set:\n\nBeastmaster\nWolfen\nWolfen\nWolfen\nThief\nCleric"
                    + "\n\n\n"
                    + "Note that one of your characters must have the role of Beastmaster "
                    + "if you want a Dragon or Wolfen in your party.",
                popup::PopupButtons::Continue,
                popup::PopupImage::Large));
    }

    void CharacterStage::Help3Popup()
    {
        game::LoopManager::Instance()->PopupWaitBegin(
            this,
            popup::PopupManager::Instance()->CreatePopupInfo(
                POPUP_NAME_HELP_3_,
                std::string("To make a character, first select the Race and Role.  ")
                    + "Use the text descriptions to guide you.  Then hold down the space bar to "
                    + "summon a random set of attributes.  \n\nDrag the attributes up and down to"
                    + " customize them for the character you are crafting.  Use the attribute "
                    + "description text on the right side of the screen to guide you.  If you "
                    + "don't like the attribute values you have, simply hold down the space bar "
                    + "again to randomly summon more.\n\nFinally, give your character a name and "
                    + "then click the Save button.  Once you have created all the characters you "
                    + "need, click the Next button.  You can always return and make more "
                    + "characters.",
                popup::PopupButtons::Okay,
                popup::PopupImage::Large));
    }

    void CharacterStage::CharacterCreationConfirmPopup(const std::size_t IMAGE_INDEX)
    {
        selectedImageIndex_ = IMAGE_INDEX;

        auto const CHARACTER_NAME{ boost::algorithm::trim_copy(nameTextEntryBoxUPtr_->GetText()) };

        const bool IS_MALE{ GetCurrentSelectedSex() == creature::sex::Male };

        auto const RACE{ static_cast<creature::race::Enum>(
            raceRadioButtonUPtr_->GetSelectedNumber()) };

        auto const ROLE{ (
            (RACE == creature::race::Wolfen)
                ? creature::role::Wolfen
                : static_cast<creature::role::Enum>(roleRadioButtonSPtr_->GetSelectedNumber())) };

        // setup the final StatSet for the character
        stats::StatSet statSetFinal;
        for (misc::EnumUnderlying_t i(0); i < stats::Traits::StatCount; ++i)
        {
            auto const NEXT_ENUM{ static_cast<stats::Traits::Enum>(i) };
            statSetFinal.Set(NEXT_ENUM, fixedStatsSVec_[NEXT_ENUM]->Value());
        }

        std::ostringstream ss;
        ss << "\n\nCreate " << CHARACTER_NAME << "?\nA " << ((IS_MALE) ? "male " : "female ");

        if (RACE == creature::race::Dragon)
        {
            ss << creature::role::Name(ROLE) << " " << creature::race::Name(RACE);
        }
        else
        {
            ss << creature::race::Name(RACE) << " ";

            if (creature::race::RaceRoleMatch(RACE, ROLE) == false)
            {
                ss << creature::role::Name(ROLE);
            }
        }

        // const std::string PAD(" ");
        ss << " with:\n\n"
           << "   Strength=" << statSetFinal.Str() << "\n"
           << "   Accuracy=" << statSetFinal.Acc() << "\n"
           << "   Charm=" << statSetFinal.Cha() << "\n"
           << "   Luck=" << statSetFinal.Lck() << "\n"
           << "   Speed=" << statSetFinal.Spd() << "\n"
           << "   Intelligence=" << statSetFinal.Int() << "\n";

        game::LoopManager::Instance()->PopupWaitBegin(
            this,
            popup::PopupManager::Instance()->CreatePopupInfo(
                POPUP_NAME_CREATECONFIRM_,
                ss.str(),
                popup::PopupButtons::YesNo,
                popup::PopupImage::Large,
                sfml_util::Justified::Center,
                sfml_util::sound_effect::PromptQuestion));
    }

    void CharacterStage::AdjustRoleRadioButtonsForRace(const creature::race::Enum WHICH_RACE)
    {
        auto const VALID_ROLES{ creature::race::Roles(WHICH_RACE) };

        misc::SizetVec_t invalidRoleIndexes;
        invalidRoleIndexes.reserve(creature::role::PlayerRoleCount);

        auto const NUM_PLAYER_ROLES{ static_cast<std::size_t>(creature::role::PlayerRoleCount) };
        for (std::size_t i(0); i < NUM_PLAYER_ROLES; ++i)
        {
            auto const ROLE{ static_cast<creature::role::Enum>(i) };

            auto const FOUND_ITER{ std::find(
                std::begin(VALID_ROLES), std::end(VALID_ROLES), ROLE) };

            if (FOUND_ITER == std::end(VALID_ROLES))
            {
                invalidRoleIndexes.emplace_back(i);
            }
        }

        roleRadioButtonSPtr_->SetInvalidSelections(invalidRoleIndexes);
    }

    float CharacterStage::GetAttributeNumPosTop(const stats::Traits::Enum TRAIT_ENUM)
    {
        if (TRAIT_ENUM == stats::Traits::Strength)
        {
            return statsLine1PosTop_ + attribVertOffset1_ + (0 * attribVertOffset2_);
        }

        if (TRAIT_ENUM == stats::Traits::Accuracy)
        {
            return statsLine1PosTop_ + attribVertOffset1_ + (1 * attribVertOffset2_);
        }

        if (TRAIT_ENUM == stats::Traits::Charm)
        {
            return statsLine1PosTop_ + attribVertOffset1_ + (2 * attribVertOffset2_);
        }

        if (TRAIT_ENUM == stats::Traits::Luck)
        {
            return statsLine1PosTop_ + attribVertOffset1_ + (3 * attribVertOffset2_);
        }

        if (TRAIT_ENUM == stats::Traits::Speed)
        {
            return statsLine1PosTop_ + attribVertOffset1_ + (4 * attribVertOffset2_);
        }

        return statsLine1PosTop_ + attribVertOffset1_ + (5 * attribVertOffset2_);
    }

    bool CharacterStage::GetStatHelpText(
        const stats::Traits::Enum WHICH_STAT, sfml_util::gui::TextInfo & textInfo) const
    {
        // bail if no values yet
        if ((static_cast<std::size_t>(WHICH_STAT) >= fixedStatsSVec_.size())
            || (AreAnyAnimNumStillMoving()) || (AreAnyStatsIgnored()))
        {
            textInfo.text.clear();
            textInfo.color = DESC_TEXT_COLOR_;
            return false;
        }

        auto const RECC_NORMAL_STR{
            "It would be a good idea to increase this attribute if you can."
        };

        auto const RECC_STRONG_STR{
            "It is strongly reccomended that you increase this attribute."
        };

        sf::Color reccNormalColor(DESC_TEXT_COLOR_);
        reccNormalColor.g = static_cast<sf::Uint8>(static_cast<int>(reccNormalColor.g) + 20);
        reccNormalColor.b = static_cast<sf::Uint8>(static_cast<int>(reccNormalColor.b) + 40);

        sf::Color reccStrongColor(255, 200, 181);

        auto const RACE_ENUM{ static_cast<creature::race::Enum>(
            raceRadioButtonUPtr_->GetSelectedNumber()) };

        auto const ROLE_ENUM{ static_cast<creature::role::Enum>(
            roleRadioButtonSPtr_->GetSelectedNumber()) };

        auto const WHICH_ROLE{ (
            (RACE_ENUM == creature::race::Wolfen) ? creature::role::Wolfen : ROLE_ENUM) };

        auto const STAT_VALUE{ fixedStatsSVec_[static_cast<std::size_t>(WHICH_STAT)]->Value() };

        std::ostringstream sss;
        sss << "Your current " << stats::Traits::ToString(WHICH_STAT) << " is only " << STAT_VALUE
            << ".";

        auto const IS_ONLY_STR{ sss.str() };

        const stats::Trait_t STAT_VALUE_LOW{ 10 };
        const stats::Trait_t STAT_VALUE_MED{ 15 };
        const stats::Trait_t STAT_VALUE_HIGH{ 20 };
        const stats::Trait_t STAT_VALUE_HIGHEST{ 25 };

        std::ostringstream ss;
        if (WHICH_ROLE == creature::role::Archer)
        {
            if (WHICH_STAT == stats::Traits::Accuracy)
            {
                if (STAT_VALUE < STAT_VALUE_MED)
                {
                    ss << "Archers need Accuracy the most, and your current Accuracy is only "
                       << STAT_VALUE << ".  " << RECC_STRONG_STR;

                    textInfo.color = reccStrongColor;
                }
                else if (STAT_VALUE < STAT_VALUE_HIGH)
                {
                    ss << "Archers need Accuracy the most, and your current Accuracy is only "
                       << STAT_VALUE << ".  " << RECC_NORMAL_STR;

                    textInfo.color = reccNormalColor;
                }
            }

            if (WHICH_STAT == stats::Traits::Strength)
            {
                if (STAT_VALUE < STAT_VALUE_MED)
                {
                    ss << "An Archer is primarily a fighter, so the Strength attribute is "
                       << "important.  Your current Strength is only " << STAT_VALUE << ".  "
                       << RECC_STRONG_STR;

                    textInfo.color = reccStrongColor;
                }
                else if (STAT_VALUE < STAT_VALUE_HIGH)
                {
                    ss << "An Archer is primarily a fighter, so the Strength attribute is "
                       << "important.  Your current Strength is only " << STAT_VALUE << ".  "
                       << RECC_NORMAL_STR;

                    textInfo.color = reccNormalColor;
                }
            }

            if (WHICH_STAT == stats::Traits::Speed)
            {
                if (STAT_VALUE < STAT_VALUE_LOW)
                {
                    ss << "An Archer is primarily a fighter, so the Speed attribute is "
                       << "important for dodging attacks.  Your current Speed is only "
                       << STAT_VALUE << ".  " << RECC_STRONG_STR;

                    textInfo.color = reccStrongColor;
                }
                else if (STAT_VALUE < STAT_VALUE_MED)
                {
                    ss << "An Archer is primarily a fighter, so the Speed attribute is "
                       << "important for dodging attacks.  Your current Speed is only "
                       << STAT_VALUE << ".  " << RECC_NORMAL_STR;

                    textInfo.color = reccNormalColor;
                }
            }
        }
        else if (WHICH_ROLE == creature::role::Bard)
        {
            if (WHICH_STAT == stats::Traits::Charm)
            {
                auto const REASON_STR{ "Charm is important to Bards for magical song." };

                if (STAT_VALUE < STAT_VALUE_MED)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_STRONG_STR;
                    textInfo.color = reccStrongColor;
                }
                else if (STAT_VALUE < STAT_VALUE_HIGHEST)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_NORMAL_STR;
                    textInfo.color = reccNormalColor;
                }
            }

            if (WHICH_STAT == stats::Traits::Intelligence)
            {
                auto const REASON_STR{ "Inteflligence is important to Bards for magical song." };

                if (STAT_VALUE < STAT_VALUE_MED)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_STRONG_STR;
                    textInfo.color = reccStrongColor;
                }
                else if (STAT_VALUE < STAT_VALUE_HIGHEST)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_NORMAL_STR;
                    textInfo.color = reccNormalColor;
                }
            }

            if (WHICH_STAT == stats::Traits::Strength)
            {
                auto const REASON_STR{ std::string("Strength is sometimes important to Bards")
                                       + " if you want them to also be good fighters." };

                if (STAT_VALUE < STAT_VALUE_MED)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_NORMAL_STR;
                    textInfo.color = reccStrongColor;
                }
            }
        }
        else if (WHICH_ROLE == creature::role::Beastmaster)
        {
            if (WHICH_STAT == stats::Traits::Intelligence)
            {
                auto const REASON_STR{
                    std::string("A Beastmaster's primary attribute is ")
                    + "Intelligence, for the magical bond they have to their animals."
                };

                if (STAT_VALUE < STAT_VALUE_MED)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_STRONG_STR;
                    textInfo.color = reccStrongColor;
                }
                else if (STAT_VALUE < STAT_VALUE_HIGH)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_NORMAL_STR;
                    textInfo.color = reccNormalColor;
                }
            }

            if (WHICH_STAT == stats::Traits::Strength)
            {
                auto const REASON_STR{ std::string("Strength is often important to Beastmasters")
                                       + " because they can be good fighters." };

                if (STAT_VALUE < STAT_VALUE_MED)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_NORMAL_STR;
                    textInfo.color = reccStrongColor;
                }
            }
        }
        else if (WHICH_ROLE == creature::role::Cleric)
        {
            if (WHICH_STAT == stats::Traits::Charm)
            {
                auto const REASON_STR{ std::string("A Cleric's primary attribute is Charm ")
                                       + "so they can heal and be good diplomats." };

                if (STAT_VALUE < STAT_VALUE_MED)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_STRONG_STR;
                    textInfo.color = reccStrongColor;
                }
                else if (STAT_VALUE < STAT_VALUE_HIGH)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_NORMAL_STR;
                    textInfo.color = reccNormalColor;
                }
            }

            if (WHICH_STAT == stats::Traits::Intelligence)
            {
                auto const REASON_STR{
                    "A Cleric is a kind of spell caster, so Intelligence is very important."
                };

                if (STAT_VALUE < STAT_VALUE_MED)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_STRONG_STR;
                    textInfo.color = reccStrongColor;
                }
                else if (STAT_VALUE < STAT_VALUE_HIGH)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_NORMAL_STR;
                    textInfo.color = reccNormalColor;
                }
            }
        }
        else if (
            (WHICH_ROLE == creature::role::Firebrand) || (WHICH_ROLE == creature::role::Sylavin))
        {
            if (WHICH_STAT == stats::Traits::Strength)
            {
                auto const REASON_STR{
                    std::string("Dragons may be magical creatures, but make")
                    + " no mistake, they are fighters.  Strength is important to all "
                    + "types of Dragons."
                };

                if (STAT_VALUE < STAT_VALUE_MED)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_STRONG_STR;
                    textInfo.color = reccStrongColor;
                }
                else if (STAT_VALUE < STAT_VALUE_HIGH)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_NORMAL_STR;
                    textInfo.color = reccNormalColor;
                }
            }

            if (WHICH_STAT == stats::Traits::Intelligence)
            {
                auto const REASON_STR{
                    "Dragons are magical creatures so Intelligence is important."
                };

                if (STAT_VALUE < STAT_VALUE_MED)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_STRONG_STR;
                    textInfo.color = reccStrongColor;
                }
                else if (STAT_VALUE < STAT_VALUE_HIGH)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_NORMAL_STR;
                    textInfo.color = reccNormalColor;
                }
            }
        }
        else if (WHICH_ROLE == creature::role::Knight)
        {
            auto const REASON_STR{ std::string("Knights are fighters, so they need Strength,")
                                   + " Accuracy, and Speed -in that order." };

            if (WHICH_STAT == stats::Traits::Strength)
            {
                if (STAT_VALUE < STAT_VALUE_MED)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_STRONG_STR;
                    textInfo.color = reccStrongColor;
                }
                else if (STAT_VALUE < STAT_VALUE_HIGH)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_NORMAL_STR;
                    textInfo.color = reccNormalColor;
                }
            }

            if (WHICH_STAT == stats::Traits::Speed)
            {
                if (STAT_VALUE < STAT_VALUE_MED)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_STRONG_STR;
                    textInfo.color = reccStrongColor;
                }
                else if (STAT_VALUE < STAT_VALUE_HIGH)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_NORMAL_STR;
                    textInfo.color = reccNormalColor;
                }
            }

            if (WHICH_STAT == stats::Traits::Accuracy)
            {
                if (STAT_VALUE < STAT_VALUE_MED)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_STRONG_STR;
                    textInfo.color = reccStrongColor;
                }
                else if (STAT_VALUE < STAT_VALUE_HIGH)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_NORMAL_STR;
                    textInfo.color = reccNormalColor;
                }
            }
        }
        else if (WHICH_ROLE == creature::role::Sorcerer)
        {
            if (WHICH_STAT == stats::Traits::Intelligence)
            {
                auto const REASON_STR{ std::string("A Sorcerer is first and foremost a spell ")
                                       + "caster, so Intelligence is very important." };

                if (STAT_VALUE < STAT_VALUE_MED)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_STRONG_STR;
                    textInfo.color = reccStrongColor;
                }
                else if (STAT_VALUE < STAT_VALUE_HIGH)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_NORMAL_STR;
                    textInfo.color = reccNormalColor;
                }
            }

            if (WHICH_STAT == stats::Traits::Speed)
            {
                auto const REASON_STR{ std::string("Sorcerers are typically physically weaker")
                                       + " than the other adventurers.  Speed will be important "
                                         "for dodging attacks." };

                if (STAT_VALUE < STAT_VALUE_MED)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_STRONG_STR;
                    textInfo.color = reccStrongColor;
                }
                else if (STAT_VALUE < STAT_VALUE_HIGH)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_NORMAL_STR;
                    textInfo.color = reccNormalColor;
                }
            }
        }
        else if (WHICH_ROLE == creature::role::Thief)
        {
            if (WHICH_STAT == stats::Traits::Speed)
            {
                auto const REASON_STR{
                    std::string("Thieves are typically physically weaker ")
                    + "than the other adventurers.  Speed will be important for dodging attacks"
                    + " and for having fast fingers."
                };

                if (STAT_VALUE < STAT_VALUE_MED)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_STRONG_STR;
                    textInfo.color = reccStrongColor;
                }
                else if (STAT_VALUE < STAT_VALUE_HIGH)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_NORMAL_STR;
                    textInfo.color = reccNormalColor;
                }
            }

            if (WHICH_STAT == stats::Traits::Luck)
            {
                auto const REASON_STR{ "Thieves need Luck more than any other attribute." };

                if (STAT_VALUE < STAT_VALUE_MED)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_STRONG_STR;
                    textInfo.color = reccStrongColor;
                }
                else if (STAT_VALUE < STAT_VALUE_HIGH)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_NORMAL_STR;
                    textInfo.color = reccNormalColor;
                }
            }
        }
        else if (WHICH_ROLE == creature::role::Wolfen)
        {
            if (WHICH_STAT == stats::Traits::Strength)
            {
                auto const REASON_STR{
                    std::string("Wolfens are not in your party of ")
                    + "adventurers for their looks, they are there to defense.  Strength for"
                    + " fighting is most important."
                };

                if (STAT_VALUE < STAT_VALUE_HIGH)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_STRONG_STR;
                    textInfo.color = reccStrongColor;
                }
                else if (STAT_VALUE < 25)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_NORMAL_STR;
                    textInfo.color = reccNormalColor;
                }
            }

            if (WHICH_STAT == stats::Traits::Accuracy)
            {
                auto const REASON_STR{
                    std::string("A Wolfen's strong jaws are useless if ")
                    + "they can't bite anything with them.  Accuracy is important."
                };

                if (STAT_VALUE < STAT_VALUE_MED)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_STRONG_STR;
                    textInfo.color = reccStrongColor;
                }
                else if (STAT_VALUE < STAT_VALUE_HIGH)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_NORMAL_STR;
                    textInfo.color = reccNormalColor;
                }
            }

            if (WHICH_STAT == stats::Traits::Speed)
            {
                auto const REASON_STR{
                    std::string("Wolfen's are fast for a reason, they need ")
                    + "to avoid being hit with attacks.  Speed will be of primary importance for "
                    + "those on the front line of a fight."
                };

                if (STAT_VALUE < STAT_VALUE_MED)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_STRONG_STR;
                    textInfo.color = reccStrongColor;
                }
                else if (STAT_VALUE < STAT_VALUE_HIGH)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_NORMAL_STR;
                    textInfo.color = reccNormalColor;
                }
            }
        }
        else
        {
            ss.clear();
            textInfo.color = DESC_TEXT_COLOR_;
        }

        if (ss.str().empty() == false)
        {
            ss << "\n\n";
        }

        textInfo.text = ss.str();
        return !textInfo.text.empty();
    }

    void CharacterStage::UndoAndClearStatModifierChanges()
    {
        // undo modifier changes to statSetBase
        for (std::size_t s(0); s < stats::Traits::StatCount; ++s)
        {
            auto const NEXT_TRAIT_ENUM{ static_cast<stats::Traits::Enum>(s) };
            auto const NUM_MODIFIERS{ statModifierTextVec_.size() };
            for (std::size_t m(0); m < NUM_MODIFIERS; ++m)
            {
                if (NEXT_TRAIT_ENUM == statModifierTextVec_[m].stat)
                {
                    statSetBase_.Set(
                        NEXT_TRAIT_ENUM,
                        (statSetBase_.Get(NEXT_TRAIT_ENUM) + (-1 * statModifierTextVec_[m].value)));
                }
            }
        }

        // eliminate all modifiers
        statModifierTextVec_.clear();
    }

    void CharacterStage::SetVisibleStatsToStatSetBase()
    {
        for (misc::EnumUnderlying_t i(0); i < stats::Traits::StatCount; ++i)
        {
            auto const NEXT_TRAIT_ENUM{ static_cast<stats::Traits::Enum>(i) };
            auto const NEW_VAL{ statSetBase_.Get(NEXT_TRAIT_ENUM) };

            // prevent visible stats from showing negative numbers, but keep them in statSetBase_
            if (NEW_VAL < 0)
            {
                fixedStatsSVec_[i]->ValueSet(0);
            }
            else
            {
                fixedStatsSVec_[i]->ValueSet(NEW_VAL);
            }

            fixedStatsSVec_[i]->CreateNewTextRegion();
        }
    }

    void CharacterStage::HandleChangedStatModifiers()
    {
        // undo race and role changes to base and eliminate all stat modifiers
        // -already done before this is called

        CreateStatModifers();
        ApplyStatModifiersToStatSetBase();
        SetVisibleStatsToStatSetBase();
        Setup_AttributeDescriptionBox();
    }

    void CharacterStage::CreateStatModifers()
    {
        statModifierTextVec_.clear();

        auto const RACE{ static_cast<creature::race::Enum>(
            raceRadioButtonUPtr_->GetSelectedNumber()) };

        statSetRace_ = creature::RaceStatModifier::Get(RACE);

        auto const HORIZ_OFFSET{ sfml_util::MapByRes(90.0f, 150.0f) };
        auto const VERT_OFFSET{ 5.0f };
        auto const RACE_NAME_ABBR{ creature::race::Abbr(RACE) };

        std::vector<stats::Traits::Enum> preExistingStatVec;

        for (misc::EnumUnderlying_t i(0); i < stats::Traits::StatCount; ++i)
        {
            auto const NEXT_TRAIT_ENUM{ static_cast<stats::Traits::Enum>(i) };
            auto const NEXT_STAT_VAL{ statSetRace_.Get(NEXT_TRAIT_ENUM) };

            if ((NEXT_STAT_VAL != 0) && (false == fixedStatsSVec_[NEXT_TRAIT_ENUM]->IgnoreMe())
                && (false == fixedStatsSVec_[NEXT_TRAIT_ENUM]->IsHeldDown())
                && (initialRollCounter_ >= 6))
            {
                preExistingStatVec.emplace_back(NEXT_TRAIT_ENUM);
                statModifierTextVec_.emplace_back(StatModText(
                    NEXT_TRAIT_ENUM,
                    RACE_NAME_ABBR,
                    NEXT_STAT_VAL,
                    statsFirstNumPosLeft_ + HORIZ_OFFSET,
                    GetAttributeNumPosTop(NEXT_TRAIT_ENUM) + VERT_OFFSET));
            }
        }

        auto const RACE_ENUM{ static_cast<creature::race::Enum>(
            raceRadioButtonUPtr_->GetSelectedNumber()) };

        auto const ROLE_SELECTED{ static_cast<creature::role::Enum>(
            roleRadioButtonSPtr_->GetSelectedNumber()) };

        auto const ROLE_ENUM{ (
            (RACE_ENUM == creature::race::Wolfen) ? creature::role::Wolfen : ROLE_SELECTED) };

        auto const ROLE_NAME_ABBR{ creature::role::Abbr(ROLE_ENUM) };

        statSetRole_ = creature::RoleStatModifier::Get(ROLE_ENUM);

        for (misc::EnumUnderlying_t i(0); i < stats::Traits::StatCount; ++i)
        {
            auto const NEXT_TRAIT_ENUM{ static_cast<stats::Traits::Enum>(i) };
            auto const NEXT_STAT_VAL{ statSetRole_.Get(NEXT_TRAIT_ENUM) };

            if ((NEXT_STAT_VAL != 0) && (false == fixedStatsSVec_[NEXT_TRAIT_ENUM]->IgnoreMe())
                && (false == fixedStatsSVec_[NEXT_TRAIT_ENUM]->IsHeldDown())
                && (initialRollCounter_ >= 6))
            {
                auto extraHorizOffset{ 0.0f };

                if (std::find(preExistingStatVec.begin(), preExistingStatVec.end(), NEXT_TRAIT_ENUM)
                    == preExistingStatVec.end())
                {
                    extraHorizOffset = 0.0f;
                }
                else
                {
                    extraHorizOffset = sfml_util::MapByRes(60.0f, 200.0f);
                }

                statModifierTextVec_.emplace_back(StatModText(
                    NEXT_TRAIT_ENUM,
                    ROLE_NAME_ABBR,
                    NEXT_STAT_VAL,
                    statsFirstNumPosLeft_ + HORIZ_OFFSET + extraHorizOffset,
                    GetAttributeNumPosTop(NEXT_TRAIT_ENUM) + VERT_OFFSET));
            }
        }
    }

    void CharacterStage::ApplyStatModifiersToStatSetBase()
    {
        auto const NUM_MODIFIERS{ statModifierTextVec_.size() };
        for (std::size_t i(0); i < NUM_MODIFIERS; ++i)
        {
            statSetBase_.Set(
                statModifierTextVec_[i].stat,
                statSetBase_.Get(statModifierTextVec_[i].stat) + statModifierTextVec_[i].value);
        }
    }

    stats::Traits::Enum CharacterStage::GetHeldDownStat() const
    {
        if (fixedStatsSVec_[stats::Traits::Strength]->IsHeldDown())
        {
            return stats::Traits::Strength;
        }

        if (fixedStatsSVec_[stats::Traits::Accuracy]->IsHeldDown())
        {
            return stats::Traits::Accuracy;
        }

        if (fixedStatsSVec_[stats::Traits::Charm]->IsHeldDown())
        {
            return stats::Traits::Charm;
        }

        if (fixedStatsSVec_[stats::Traits::Luck]->IsHeldDown())
        {
            return stats::Traits::Luck;
        }

        if (fixedStatsSVec_[stats::Traits::Speed]->IsHeldDown())
        {
            return stats::Traits::Speed;
        }

        if (fixedStatsSVec_[stats::Traits::Intelligence]->IsHeldDown())
        {
            return stats::Traits::Intelligence;
        }

        return stats::Traits::Count;
    }

    stats::Traits::Enum CharacterStage::GetStatAbove(const stats::Traits::Enum STAT) const
    {
        if (STAT == stats::Traits::Accuracy)
            return stats::Traits::Strength;
        if (STAT == stats::Traits::Charm)
            return stats::Traits::Accuracy;
        if (STAT == stats::Traits::Luck)
            return stats::Traits::Charm;
        if (STAT == stats::Traits::Speed)
            return stats::Traits::Luck;
        if (STAT == stats::Traits::Intelligence)
            return stats::Traits::Speed;

        return stats::Traits::Count;
    }

    stats::Traits::Enum CharacterStage::GetStatBelow(const stats::Traits::Enum STAT) const
    {
        if (STAT == stats::Traits::Strength)
            return stats::Traits::Accuracy;
        if (STAT == stats::Traits::Accuracy)
            return stats::Traits::Charm;
        if (STAT == stats::Traits::Charm)
            return stats::Traits::Luck;
        if (STAT == stats::Traits::Luck)
            return stats::Traits::Speed;
        if (STAT == stats::Traits::Speed)
            return stats::Traits::Intelligence;

        return stats::Traits::Count;
    }

    float CharacterStage::GetStatPosTop(const stats::Traits::Enum TRAIT_ENUM) const
    {
        if (TRAIT_ENUM == stats::Traits::Strength)
        {
            return statsStrPosTop_;
        }
        if (TRAIT_ENUM == stats::Traits::Accuracy)
        {
            return statsAccPosTop_;
        }
        if (TRAIT_ENUM == stats::Traits::Charm)
        {
            return statsChaPosTop_;
        }
        if (TRAIT_ENUM == stats::Traits::Luck)
        {
            return statsLckPosTop_;
        }
        if (TRAIT_ENUM == stats::Traits::Speed)
        {
            return statsSpdPosTop_;
        }
        if (TRAIT_ENUM == stats::Traits::Intelligence)
        {
            return statsIntPosTop_;
        }

        return -1.0f;
    }

    const sfml_util::gui::IGuiEntityPtrOpt_t
        CharacterStage::UpdateMouseUp(const sf::Vector2f & MOUSE_POS_V)
    {
        if (AreAnyAnimNumStillMoving())
        {
            return boost::none;
        }

        // process MouseUp() on all other entitys
        auto entityWithFocusPtrOpt{ Stage::UpdateMouseUp(MOUSE_POS_V) };

        // remove animations that are finished from the vector
        animStatsSVec_.clear();

        // snap to closest position when drag stops
        auto const HELD_DOWN_STAT{ GetHeldDownStat() };
        if (HELD_DOWN_STAT != stats::Traits::Count)
        {
            sfml_util::SoundManager::Instance()->PlaySfx_AckMinor();

            fixedStatsSVec_[static_cast<std::size_t>(HELD_DOWN_STAT)]->SetPosY(
                GetStatPosTop(HELD_DOWN_STAT));
        }

        dragStartY_ = -1.0f; // anything less than zero here will work

        // clear isHeldDown status from all stat anim objects, and see if any were
        auto wereAnyHeldDown{ false };
        for (misc::EnumUnderlying_t i(0); i < stats::Traits::StatCount; ++i)
        {
            if (fixedStatsSVec_[i]->IsHeldDown())
            {
                wereAnyHeldDown = true;
            }

            fixedStatsSVec_[i]->MouseUp();
        }

        if (wereAnyHeldDown)
        {
            UndoAndClearStatModifierChanges();
            HandleChangedStatModifiers();

            sfml_util::SoundManager::Instance()->PlaySfx_AckMajor();
        }

        return entityWithFocusPtrOpt;
    }

    void CharacterStage::UpdateMousePos(const sf::Vector2i & NEW_MOUSE_POS_V)
    {
        Stage::UpdateMousePos(NEW_MOUSE_POS_V);
        auto const NEW_MOUSE_POS_VF{ sfml_util::ConvertVector<int, float>(NEW_MOUSE_POS_V) };
        HandleAttributeDragging(NEW_MOUSE_POS_VF);
    }

    bool CharacterStage::AreAnyAnimNumStillMoving() const
    {
        for (auto const & ANIM_NUM_SPTR : animStatsSVec_)
        {
            if ((ANIM_NUM_SPTR->IgnoreMe() == false) && (ANIM_NUM_SPTR->IsDoneMoving() == false))
            {
                return true;
            }
        }

        for (auto const & FIXED_NUM_SPTR : fixedStatsSVec_)
        {
            if ((FIXED_NUM_SPTR->IgnoreMe() == false) && (FIXED_NUM_SPTR->IsDoneMoving() == false))
            {
                return true;
            }
        }

        return false;
    }

    bool CharacterStage::AreAnyStatsIgnored() const
    {
        for (misc::EnumUnderlying_t i(0); i < stats::Traits::StatCount; ++i)
        {
            if (fixedStatsSVec_[i]->IgnoreMe())
            {
                return true;
            }
        }

        return false;
    }

    void CharacterStage::HandleAttributeDragging(const sf::Vector2f & MOUSE_POS_V)
    {
        // check there is a number being held down at all
        auto const STAT_HELD{ GetHeldDownStat() };
        if (STAT_HELD == stats::Traits::Count)
        {
            return;
        }

        // can't drag if numbers are still animating
        if (isAnimStats_)
        {
            return;
        }

        if (AreAnyAnimNumStillMoving())
        {
            return;
        }

        // can't drag without a full set of attributes
        if (AreAnyStatsIgnored())
        {
            return;
        }

        // can't drag past the edges of the numbers box
        const float NEW_POS_Y(MOUSE_POS_V.y - 10.0f);
        if ((NEW_POS_Y > statsIntPosTop_) || (NEW_POS_Y < statsStrPosTop_))
        {
            return;
        }

        if (dragStartY_ < 0.0f)
        {
            dragStartY_ = fixedStatsSVec_[STAT_HELD]->GetPos().y;
        }

        auto const DIFF{ dragStartY_ - NEW_POS_Y };
        auto const DIFF_MAX{ statsLineVertPosDiff_ - (statsLineVertPosDiff_ / 3.0f) };
        auto const IS_MOVING_UP{ DIFF > 0.0f };

        fixedStatsSVec_[STAT_HELD]->SetPosY(NEW_POS_Y);

        if (IS_MOVING_UP)
        {
            auto const STAT_ABOVE{ GetStatAbove(STAT_HELD) };
            if ((STAT_ABOVE != stats::Traits::Count) && (DIFF > DIFF_MAX))
            {
                dragStartY_ -= statsLineVertPosDiff_;

                fixedStatsSVec_[STAT_ABOVE]->SetPosY(
                    fixedStatsSVec_[STAT_ABOVE]->GetPos().y + statsLineVertPosDiff_);

                // track the closest stat enum
                closestDragStat_ = STAT_ABOVE;

                SwapAttributes(STAT_HELD, STAT_ABOVE);
            }
        }
        else
        {
            auto const STAT_BELOW{ GetStatBelow(STAT_HELD) };
            if ((STAT_BELOW != stats::Traits::Count) && (fabs(DIFF) > DIFF_MAX))
            {
                dragStartY_ += statsLineVertPosDiff_;

                fixedStatsSVec_[STAT_BELOW]->SetPosY(
                    fixedStatsSVec_[STAT_BELOW]->GetPos().y - statsLineVertPosDiff_);

                // track the closest stat enum
                closestDragStat_ = STAT_BELOW;

                SwapAttributes(STAT_HELD, STAT_BELOW);
            }
        }
    }

    void CharacterStage::ProduceAnimatingDigits(const float ELAPSED_TIME_SECONDS)
    {
        if ((false == isAnimStats_) && (false == isWaitingForStats_))
        {
            return;
        }

        if (isAnimStats_)
        {
            animStatsTimeCounterSec_ += ELAPSED_TIME_SECONDS;
        }

        if (isAnimStats_ && (animStatsTimeCounterSec_ > animStatsDelayPerSec_))
        {
            // reset timer to random value
            animStatsTimeCounterSec_ = 0.0f;
            animStatsDelayPerSec_ = misc::random::Float(0.05f, 0.25f);

            sfml_util::gui::TextInfo textInfo(
                " ",
                sfml_util::FontManager::Instance()->Font_NumbersDefault1(),
                40,
                sf::Color::White,
                sfml_util::Justified::Left);

            auto const NEXT_VAL{ misc::random::Int(1, STAT_INITIAL_MAX_) };

            auto numToUse{ misc::random::Int(1, 6) };
            if (initialRollCounter_ <= 6)
            {
                numToUse = initialRollCounter_++;
            }

            auto const SMOKE_ANIM_MID_X{ smokeAnimUPtr_->GetEntityPos().x
                                         + (smokeAnimUPtr_->GetEntityRegion().width * 0.5f)
                                         - 10.0f };

            auto const SMOKE_ANIM_MID_Y{ smokeAnimUPtr_->GetEntityPos().y
                                         + (smokeAnimUPtr_->GetEntityRegion().height * 0.5f)
                                         - 10.0f };

            auto const ANIM_NUM_TARGET_X{ statsFirstNumPosLeft_ + ((NEXT_VAL < 10) ? 10.0f : 0.0f)
                                          - 10.0f };

            switch (numToUse)
            {
                case 1:
                {
                    auto ansp = std::make_shared<AnimNum>(
                        NEXT_VAL,
                        stats::Traits::Strength,
                        SMOKE_ANIM_MID_X,
                        SMOKE_ANIM_MID_Y,
                        ANIM_NUM_TARGET_X,
                        statsStrPosTop_,
                        textInfo);
                    animStatsSVec_.emplace_back(ansp);

                    AnimNumSPtr_t toFadeSPtr(fixedStatsSVec_[stats::Traits::Strength]);
                    toFadeSPtr->WillFadeSet(true);
                    //
                    AnimNumSPtr_t holdFadeSPtr(animStatsSVec_[animStatsSVec_.size() - 1]);
                    holdFadeSPtr->CreateNewTextRegion();
                    holdFadeSPtr->WillFadeSet(false);
                    //
                    fixedStatsSVec_[stats::Traits::Strength] = holdFadeSPtr;
                    statSetBase_.Set(stats::Traits::Strength, NEXT_VAL);
                    animStatsSVec_.emplace_back(toFadeSPtr);
                    break;
                }

                case 2:
                {
                    auto ansp = std::make_shared<AnimNum>(
                        NEXT_VAL,
                        stats::Traits::Accuracy,
                        SMOKE_ANIM_MID_X,
                        SMOKE_ANIM_MID_Y,
                        ANIM_NUM_TARGET_X,
                        statsAccPosTop_,
                        textInfo);

                    animStatsSVec_.emplace_back(ansp);

                    AnimNumSPtr_t toFade(fixedStatsSVec_[stats::Traits::Accuracy]);
                    toFade->WillFadeSet(true);
                    //
                    AnimNumSPtr_t holdFade(animStatsSVec_[animStatsSVec_.size() - 1]);
                    holdFade->CreateNewTextRegion();
                    holdFade->WillFadeSet(false);
                    //
                    fixedStatsSVec_[stats::Traits::Accuracy] = holdFade;
                    statSetBase_.Set(stats::Traits::Accuracy, NEXT_VAL);
                    animStatsSVec_.emplace_back(toFade);
                    break;
                }

                case 3:
                {
                    auto ansp = std::make_shared<AnimNum>(
                        NEXT_VAL,
                        stats::Traits::Charm,
                        SMOKE_ANIM_MID_X,
                        SMOKE_ANIM_MID_Y,
                        ANIM_NUM_TARGET_X,
                        statsChaPosTop_,
                        textInfo);

                    animStatsSVec_.emplace_back(ansp);

                    AnimNumSPtr_t toFade(fixedStatsSVec_[stats::Traits::Charm]);
                    toFade->WillFadeSet(true);
                    //
                    AnimNumSPtr_t holdFade(animStatsSVec_[animStatsSVec_.size() - 1]);
                    holdFade->CreateNewTextRegion();
                    holdFade->WillFadeSet(false);
                    //
                    fixedStatsSVec_[stats::Traits::Charm] = holdFade;
                    statSetBase_.Set(stats::Traits::Charm, NEXT_VAL);
                    animStatsSVec_.emplace_back(toFade);
                    break;
                }

                case 4:
                {
                    auto ansp = std::make_shared<AnimNum>(
                        NEXT_VAL,
                        stats::Traits::Luck,
                        SMOKE_ANIM_MID_X,
                        SMOKE_ANIM_MID_Y,
                        ANIM_NUM_TARGET_X,
                        statsLckPosTop_,
                        textInfo);

                    animStatsSVec_.emplace_back(ansp);

                    AnimNumSPtr_t toFade(fixedStatsSVec_[stats::Traits::Luck]);
                    toFade->WillFadeSet(true);
                    //
                    AnimNumSPtr_t holdFade(animStatsSVec_[animStatsSVec_.size() - 1]);
                    holdFade->CreateNewTextRegion();
                    holdFade->WillFadeSet(false);
                    //
                    fixedStatsSVec_[stats::Traits::Luck] = holdFade;
                    statSetBase_.Set(stats::Traits::Luck, NEXT_VAL);
                    animStatsSVec_.emplace_back(toFade);
                    break;
                }

                case 5:
                {
                    auto ansp = std::make_shared<AnimNum>(
                        NEXT_VAL,
                        stats::Traits::Speed,
                        SMOKE_ANIM_MID_X,
                        SMOKE_ANIM_MID_Y,
                        ANIM_NUM_TARGET_X,
                        statsSpdPosTop_,
                        textInfo);

                    animStatsSVec_.emplace_back(ansp);

                    AnimNumSPtr_t toFade(fixedStatsSVec_[stats::Traits::Speed]);
                    toFade->WillFadeSet(true);
                    //
                    AnimNumSPtr_t holdFade(animStatsSVec_[animStatsSVec_.size() - 1]);
                    holdFade->CreateNewTextRegion();
                    holdFade->WillFadeSet(false);
                    //
                    fixedStatsSVec_[stats::Traits::Speed] = holdFade;
                    statSetBase_.Set(stats::Traits::Speed, NEXT_VAL);
                    animStatsSVec_.emplace_back(toFade);
                    break;
                }

                case 6:
                default:
                {
                    auto ansp = std::make_shared<AnimNum>(
                        NEXT_VAL,
                        stats::Traits::Intelligence,
                        SMOKE_ANIM_MID_X,
                        SMOKE_ANIM_MID_Y,
                        ANIM_NUM_TARGET_X,
                        statsIntPosTop_,
                        textInfo);

                    animStatsSVec_.emplace_back(ansp);

                    AnimNumSPtr_t toFade(fixedStatsSVec_[stats::Traits::Intelligence]);
                    toFade->WillFadeSet(true);
                    //
                    AnimNumSPtr_t holdFade(animStatsSVec_[animStatsSVec_.size() - 1]);
                    holdFade->CreateNewTextRegion();
                    holdFade->WillFadeSet(false);
                    //
                    fixedStatsSVec_[stats::Traits::Intelligence] = holdFade;
                    statSetBase_.Set(stats::Traits::Intelligence, NEXT_VAL);
                    animStatsSVec_.emplace_back(toFade);
                    break;
                }
            } // end switch()
        }

        // update each active animating fixed number, and check for any still moving
        auto areAnyFixedStillMoving{ false };
        for (std::size_t f(0); f < stats::Traits::StatCount; ++f)
        {
            fixedStatsSVec_[f]->UpdateTime(ELAPSED_TIME_SECONDS);

            if (false == fixedStatsSVec_[f]->IsDoneMoving())
            {
                // don't break here because UpdateTime() needs to be called on each
                areAnyFixedStillMoving = true;
            }
        }

        // update each active animating number
        auto const NUM_DIGITS{ animStatsSVec_.size() };
        for (std::size_t i(0); i < NUM_DIGITS; ++i)
        {
            if (false == animStatsSVec_[i]->IsDoneFading())
            {
                animStatsSVec_[i]->UpdateTime(ELAPSED_TIME_SECONDS);
            }
        }

        if (areAnyFixedStillMoving)
        {
            isWaitingForStats_ = true;
        }
    }

    void CharacterStage::SwapAttributes(const stats::Traits::Enum A, const stats::Traits::Enum B)
    {
        // swap the anim objects
        AnimNumSPtr_t tempSPtr(fixedStatsSVec_[static_cast<std::size_t>(B)]);
        fixedStatsSVec_[static_cast<std::size_t>(B)] = fixedStatsSVec_[static_cast<std::size_t>(A)];
        fixedStatsSVec_[static_cast<std::size_t>(A)] = tempSPtr;

        // swap the underlying stat values
        auto const TEMP_TRAIT{ statSetBase_.Get(B) };
        statSetBase_.Set(B, statSetBase_.Get(A));
        statSetBase_.Set(A, TEMP_TRAIT);

        sfml_util::SoundManager::Instance()->PlaySfx_TickOn();
    }

    void CharacterStage::HandleStuckAnims(const float ELAPSED_TIME_SEC)
    {
        std::vector<std::size_t> stuckAnimIndexVec;
        auto const NUM_ANIMS{ animStatsSVec_.size() };
        for (std::size_t i(0); i < NUM_ANIMS; ++i)
        {
            if (animStatsSVec_[i]->UpdateTimer(ELAPSED_TIME_SEC))
            {
                stuckAnimIndexVec.emplace_back(i);
            }
        }

        auto const NUM_STUCK_ANIMS{ stuckAnimIndexVec.size() };
        for (std::size_t i(0); i < NUM_STUCK_ANIMS; ++i)
        {
            const typename AnimNumSVec_t::iterator ITR_TO_ERASE{
                animStatsSVec_.begin() + AnimNumSVec_t::iterator::difference_type(i)
            };

            animStatsSVec_.erase(ITR_TO_ERASE);
        }
    }

    bool CharacterStage::HandleMenuNavigationKeyRelease(const sf::Event::KeyEvent & KEY_EVENT)
    {
        if (KEY_EVENT.code == sf::Keyboard::B)
        {
            backButtonUPtr_->SetMouseState(sfml_util::MouseState::Over);
            sfml_util::SoundManager::Instance()->PlaySfx_Keypress();
            OnBackButton();
            return true;
        }
        else if (KEY_EVENT.code == sf::Keyboard::S)
        {
            saveButtonUPtr_->SetMouseState(sfml_util::MouseState::Over);
            sfml_util::SoundManager::Instance()->PlaySfx_Keypress();
            OnSaveButton();
            return true;
        }
        else if (KEY_EVENT.code == sf::Keyboard::N)
        {
            nextButtonUPtr_->SetMouseState(sfml_util::MouseState::Over);
            sfml_util::SoundManager::Instance()->PlaySfx_Keypress();
            OnNextButton();
            return true;
        }
        else if (KEY_EVENT.code == sf::Keyboard::H)
        {
            helpButtonUPtr_->SetMouseState(sfml_util::MouseState::Over);
            sfml_util::SoundManager::Instance()->PlaySfx_Keypress();
            OnHelpButton();
            return true;
        }
        else
        {
            return false;
        }
    }

    void CharacterStage::ResetForNewCharacterCreation()
    {
        initialRollCounter_ = 0;
        raceRadioButtonUPtr_->SetSelectNumber(0);
        roleRadioButtonSPtr_->SetSelectNumber(0);
        nameTextEntryBoxUPtr_->SetText("");
        statModifierTextVec_.clear();

        for (misc::EnumUnderlying_t i(0); i < stats::Traits::StatCount; ++i)
        {
            fixedStatsSVec_[i]->SetIgnoreMe();
        }

        Setup_AttributeDescriptionBox();
    }

    bool CharacterStage::RaceChange(const creature::race::Enum NEW_RACE)
    {
        UndoAndClearStatModifierChanges();
        Setup_RaceDescriptionBox();
        AdjustRoleRadioButtonsForRace(NEW_RACE);
        Setup_RoleDescriptionBox();
        HandleChangedStatModifiers();
        return true;
    }

    bool CharacterStage::RoleChange()
    {
        UndoAndClearStatModifierChanges();
        Setup_RoleDescriptionBox();
        HandleChangedStatModifiers();
        return true;
    }

    bool CharacterStage::CreateCharacter()
    {
        M_ASSERT_OR_LOGANDTHROW_SS(
            (selectedImageIndex_ < characterImageFilenamesVec_.size()),
            "stage::CharacterStage::CreateCharacter() called when selectedImageIndex_="
                << selectedImageIndex_ << " but characterImageFilenamesVec_.size()="
                << characterImageFilenamesVec_.size() << ".");

        auto const NAME{ boost::algorithm::trim_copy(nameTextEntryBoxUPtr_->GetText()) };

        auto const SEX_ENUM{ static_cast<creature::sex::Enum>(
            sexRadioButtonUPtr_->GetSelectedNumber() + 1) };

        auto const RACE_ENUM{ static_cast<creature::race::Enum>(
            raceRadioButtonUPtr_->GetSelectedNumber()) };

        auto const ROLE_ENUM{ (
            (RACE_ENUM == creature::race::Wolfen)
                ? creature::role::Wolfen
                : static_cast<creature::role::Enum>(roleRadioButtonSPtr_->GetSelectedNumber())) };

        stats::StatSet statSetFinal(statSetBase_);

        creature::CreatureFactory creatureFactory;
        creatureFactory.MakeEquipSaveAndFreePlayer(
            NAME,
            SEX_ENUM,
            RACE_ENUM,
            ROLE_ENUM,
            statSetFinal,
            characterImageFilenamesVec_.at(selectedImageIndex_));

        ResetForNewCharacterCreation();
        characterImageFilenamesVec_.clear();
        return false;
    }

    creature::sex::Enum CharacterStage::GetCurrentSelectedSex() const
    {
        return (
            (sexRadioButtonUPtr_->GetSelectedNumber() == 0) ? creature::sex::Male
                                                            : creature::sex::Female);
    }

} // namespace stage
} // namespace heroespath
