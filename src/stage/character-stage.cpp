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

#include "creature/condition.hpp"
#include "creature/creature-factory.hpp"
#include "creature/creature.hpp"
#include "creature/name-info.hpp"
#include "creature/player-party.hpp"
#include "creature/race-enum.hpp"
#include "creature/race-stats.hpp"
#include "creature/role-enum.hpp"
#include "creature/role-stats.hpp"
#include "creature/sex-enum.hpp"
#include "creature/title.hpp"
#include "gui/animation-factory.hpp"
#include "gui/box-entity.hpp"
#include "gui/creature-image-paths.hpp"
#include "gui/display.hpp"
#include "gui/even-spacer.hpp"
#include "gui/font-manager.hpp"
#include "gui/gui-images.hpp"
#include "gui/ouroboros.hpp"
#include "gui/radio-or-check-entity.hpp"
#include "gui/radio-or-check-set.hpp"
#include "gui/sound-manager.hpp"
#include "item/inventory.hpp"
#include "misc/config-file.hpp"
#include "misc/log-macros.hpp"
#include "misc/random.hpp"
#include "misc/real.hpp"
#include "popup/popup-manager.hpp"
#include "popup/popup-response.hpp"
#include "popup/popup-stage-image-select.hpp"
#include "sfutil/center.hpp"
#include "sfutil/color.hpp"
#include "sfutil/display.hpp"
#include "sfutil/position.hpp"
#include "sfutil/primitives.hpp"
#include "sfutil/size-and-scale.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

#include <memory>
#include <sstream>

namespace heroespath
{
namespace stage
{

    CharacterStage::CharacterStage()
        : StageBase(
            "CharacterCreation",
            { gui::GuiFont::Default,
              gui::GuiFont::System,
              gui::GuiFont::SystemCondensed,
              gui::GuiFont::Number,
              gui::GuiFont::Handwriting })
        //, STAT_INVALID_(-1)
        //, STAT_INITIAL_MAX_(20)
        , LIGHT_TEXT_COLOR_(sfutil::color::Light)
        , DESC_TEXT_COLOR_(sfutil::color::Orange)
        , POPUP_NAME_NONAMEERROR_("CreateNoNameError")
        , POPUP_NAME_MISSINGATTRIBS_("CreateMissingAttributesError")
        , POPUP_NAME_CREATECONFIRM_("CreateConfirm")
        , POPUP_NAME_BACKBUTTON_LEAVESCREENCONFIRM_("BackButtonLeaveScreenComfirm")
        , POPUP_NAME_NEXTBUTTON_LEAVESCREENCONFIRM_("NextButtonLeaveScreenComfirm")
        , POPUP_NAME_HELP_1_("HelpMessage1")
        , POPUP_NAME_HELP_2_("HelpMessage2")
        , POPUP_NAME_HELP_3_("HelpMessage3")
        , POPUP_NAME_IMAGE_SELECTION_("ImageSelection")
        , SMOKE_ANIM_SPEED_MIN_(0.05f)
        , SMOKE_ANIM_SPEED_MAX_(0.5f)
        , DESC_TEXT_FONT_SIZE_(gui::FontManager::Instance()->Size_Small())
        , RADIO_BUTTON_TEXT_SIZE_(gui::FontManager::Instance()->Size_Largeish())
        , statBox_(
              stage::IStagePtr_t(this),
              sfutil::ScreenRatioToPixelsHoriz(0.125f),
              (4.0f / 3.5f),
              LIGHT_TEXT_COLOR_)
        , ouroborosUPtr_(std::make_unique<gui::Ouroboros>("CharacterStage's"))
        , stageTitle_(
              gui::MenuImage::CreateCharacters,
              true,
              gui::StageTitle::DEFAULT_SYMBOL_HEIGHT_SCREEN_RATIO_,
              (gui::StageTitle::DEFAULT_SYMBOL_TO_TITLE_HEIGHT_RATIO_ * 0.8f))
        , smokeAnimSliderDriftX_()
        , smokeAnimSliderDriftY_()
        , background_()
        , smokeAnimUPtr_(gui::AnimationFactory::Make(
              gui::Animations::SmokeSwirl, sfutil::MapByRes(1.0f, 3.0f)))
        , backButtonUPtr_(std::make_unique<gui::MainMenuButton>(
              gui::MenuImage::Back,
              stage::Stage::Previous,
              gui::ImageTextEntity::Callback_t::IHandlerPtr_t(this)))
        , saveButtonUPtr_(std::make_unique<gui::MainMenuButton>(
              gui::MenuImage::Save,
              stage::Stage::Save,
              gui::ImageTextEntity::Callback_t::IHandlerPtr_t(this)))
        , helpButtonUPtr_(std::make_unique<gui::MainMenuButton>(
              gui::MenuImage::Help,
              stage::Stage::Help,
              gui::ImageTextEntity::Callback_t::IHandlerPtr_t(this)))
        , nextButtonUPtr_(std::make_unique<gui::MainMenuButton>(
              gui::MenuImage::Next,
              stage::Stage::Next,
              gui::ImageTextEntity::Callback_t::IHandlerPtr_t(this)))
        , statSetBase_()
        , raceRadioButtonUPtr_()
        , racetDescTextRegionUPtr_()
        , roleRadioButtonUPtr_()
        , roletDescTextRegionUPtr_()
        , sexRadioButtonUPtr_()
        , nameTextEntryBoxUPtr_()
        , attrDescTextRegionUPtr_()
        , sbInsTextRegionUPtr_()
        , sbInsTextSlider_(150, 255, 4.0f)
        , nInsTextRegionUPtr_()
        , nInsTextSlider_(150, 255, 4.0f)
        , bottomSymbol_()
        //, selectedImageIndex_(0)
        , characterImageFilenamesVec_()
        , woodCachedTexture_(
              "media-image-background-tile-wood",
              (gui::ImageOpt::Default | gui::ImageOpt::Repeated))
    {}

    CharacterStage::~CharacterStage() { StageBase::ClearAllEntities(); }

    const std::string CharacterStage::HandleCallback(
        const gui::RadioOrCheckSet::Callback_t::Packet_t &, const std::string &)
    {
        // TODO
        return "";

        //// was change to race
        // if (raceRadioButtonUPtr_.get() == RADIOBUTTON_WRAPPER.PTR_)
        //{
        //    return RaceChange(
        //        static_cast<creature::race::Enum>(RADIOBUTTON_WRAPPER.PTR_->GetSelectedNumber()));
        //}
        //
        //// was change to role
        // if (roleRadioButtonUPtr_.get() == RADIOBUTTON_WRAPPER.PTR_)
        //{
        //    return RoleChange();
        //}
    }

    const std::string CharacterStage::HandleCallback(
        const misc::PopupCallback_t::Packet_t & PACKET, const std::string & PACKET_DESCRIPTION)
    {
        if ((PACKET.curently_open_popup_name == POPUP_NAME_CREATECONFIRM_)
            && (popup::PopupButtons::IsAffirmative(PACKET.type)))
        {
            CreateCharacter();

            return MakeCallbackHandlerMessage(
                PACKET_DESCRIPTION,
                "player confirmed new character name and the new character was created and the "
                "stage reset to be ready to make another");
        }
        else if (
            (PACKET.curently_open_popup_name == POPUP_NAME_BACKBUTTON_LEAVESCREENCONFIRM_)
            && popup::PopupButtons::IsAffirmative(PACKET.type))
        {
            TransitionTo(stage::Stage::Menu);

            return MakeCallbackHandlerMessage(
                PACKET_DESCRIPTION,
                "player confirmed that they want to leave this character creation stage and go "
                "back to the menu, stage transitioned");
        }
        else if (
            (PACKET.curently_open_popup_name == POPUP_NAME_NEXTBUTTON_LEAVESCREENCONFIRM_)
            && popup::PopupButtons::IsAffirmative(PACKET.type))
        {
            TransitionTo(stage::Stage::Party);

            return MakeCallbackHandlerMessage(
                PACKET_DESCRIPTION,
                "player confirmed that they want to leave this character creation stage and go "
                "on to the party creation stage, stage transitioned");
        }
        else if (PACKET.curently_open_popup_name == POPUP_NAME_HELP_1_)
        {
            Help2Popup();

            return MakeCallbackHandlerMessage(
                PACKET_DESCRIPTION,
                "player finished reading help popup #1, spawning help popup #2");
        }
        else if (PACKET.curently_open_popup_name == POPUP_NAME_HELP_2_)
        {
            Help3Popup();

            return MakeCallbackHandlerMessage(
                PACKET_DESCRIPTION,
                "player finished reading help popup #2, spawning help popup #3");
        }
        else if (
            (PACKET.curently_open_popup_name == POPUP_NAME_IMAGE_SELECTION_)
            && (PACKET.type != popup::PopupButtons::Cancel) && PACKET.selection_opt)
        {
            CharacterCreationConfirmPopup(PACKET.selection_opt.value());

            return MakeCallbackHandlerMessage(
                PACKET_DESCRIPTION,
                "player picked an image for the new character and spawned the final creation "
                "confirmation popup");
        }

        return MakeCallbackHandlerMessage(PACKET_DESCRIPTION, "popup callback NOT HANDLED");
    }

    const std::string CharacterStage::HandleCallback(
        const gui::ImageTextEntity::Callback_t::Packet_t & PACKET,
        const std::string & PACKET_DESCRIPTION)
    {
        if (PACKET.entity_ptr == helpButtonUPtr_.get())
        {
            Help1Popup();
            PACKET.entity_ptr->SetMouseState(gui::MouseState::Up);

            return MakeCallbackHandlerMessage(
                PACKET_DESCRIPTION, "help button clicked and spawned help popup #1");
        }

        if (PACKET.entity_ptr == backButtonUPtr_.get())
        {
            PACKET.entity_ptr->SetMouseState(gui::MouseState::Up);

            if (OnBackButton())
            {
                return MakeCallbackHandlerMessage(
                    PACKET_DESCRIPTION,
                    "back button clicked and leave stage confirm popup spawned");
            }
            else
            {
                return MakeCallbackHandlerMessage(
                    PACKET_DESCRIPTION,
                    "back button clicked but ignored because number anims are still moving around");
            }
        }

        if (PACKET.entity_ptr == saveButtonUPtr_.get())
        {
            PACKET.entity_ptr->SetMouseState(gui::MouseState::Up);

            return MakeCallbackHandlerMessage(
                PACKET_DESCRIPTION, "save button clicked " + OnSaveButton());
        }

        if (PACKET.entity_ptr == nextButtonUPtr_.get())
        {
            PACKET.entity_ptr->SetMouseState(gui::MouseState::Up);

            return MakeCallbackHandlerMessage(
                PACKET_DESCRIPTION, "next button clicked " + OnNextButton());
        }

        return MakeCallbackHandlerMessage(PACKET_DESCRIPTION, "popup callback NOT HANDLED");
    }

    const std::string CharacterStage::HandleCallback(
        const gui::SliderBar::Callback_t::Packet_t &, const std::string &)
    {
        // TODO
        return "";
    }

    const std::string CharacterStage::HandleCallback(
        const gui::TextEntryBox::Callback_t::Packet_t &, const std::string &)
    {
        // TODO
        return "";
    }

    void CharacterStage::Setup()
    {
        EntityAdd(ouroborosUPtr_);

        Setup_MenuButtons();
        Setup_RaceRadioButtons();
        Setup_RoleRadioButtons();
        Setup_RaceDescriptionBox();
        Setup_RoleDescriptionBox();
        const auto ATTRIB_BOX_TOP { Setup_AttributeDescriptionBox() };
        Setup_NameLabel();
        Setup_NameTextEntryBox();
        Setup_SexRadioButtons();
        Setup_SpacebarInstructionText();

        // statBox_.SetVerticalPos(
        //    sfutil::Bottom(sbInsTextRegionUPtr_->GetEntityRegion())
        //    + sfutil::ScreenRatioToPixelsVert(0.01f));

        Setup_SmokeAnimation(ATTRIB_BOX_TOP);

        // setup initial config of radio buttons
        AdjustRoleRadioButtonsForRace(static_cast<creature::race::Enum>(0));

        Setup_RoleDescriptionBox();
    }

    void CharacterStage::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        StageBase::UpdateTime(ELAPSED_TIME_SECONDS);
        /*
        // oscillate the spacebar instruction text's color to help players know what to do initially
        if (AreAnyStatsIgnored() && (false == AreAnyAnimNumStillMoving()))
        {
            const auto NEW_COLOR_VAL { static_cast<sf::Uint8>(
                sbInsTextSlider_.Update(ELAPSED_TIME_SECONDS)) };

            auto color { sbInsTextRegionUPtr_->GetEntityColorForeground() };
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
        const auto CHARACTER_NAME { misc::TrimWhitespaceCopy(nameTextEntryBoxUPtr_->GetText()) };
        if (CHARACTER_NAME.empty())
        {
            const auto NEW_COLOR_VAL { static_cast<sf::Uint8>(
                nInsTextSlider_.Update(ELAPSED_TIME_SECONDS)) };

            auto color { nInsTextRegionUPtr_->GetEntityColorForeground() };
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
            smokeAnimSliderDriftX_.Update(ELAPSED_TIME_SECONDS),
            smokeAnimSliderDriftY_.Update(ELAPSED_TIME_SECONDS));

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

            gui::SoundManager::Instance()->PlaySfx_AckMinor();
        }
        else
        {
            if (AreAnyAnimNumStillMoving())
            {
                willDrawStatModText_ = false;
            }
        }

        HandleStuckAnims(ELAPSED_TIME_SECONDS);
        */
    }

    void CharacterStage::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        target.draw(background_, states);
        target.draw(stageTitle_, states);
        target.draw(bottomSymbol_, states);
        StageBase::draw(target, states);
        target.draw(statBox_, states);
        target.draw(*sexRadioButtonUPtr_, states);
        target.draw(*raceRadioButtonUPtr_, states);
        target.draw(*roleRadioButtonUPtr_, states);
    }

    bool CharacterStage::KeyPress(const sf::Event::KeyEvent & KEY_EVENT)
    {
        const auto RESULT { StageBase::KeyPress(KEY_EVENT) };
        return RESULT;
        /*if ((KEY_EVENT.code == sf::Keyboard::Space) && (false == isAnimStats_)
            && (false == nameTextEntryBoxUPtr_->HasFocus()))
        {
            gui::SoundManager::Instance()
                ->GetSoundEffectSet(gui::sound_effect_set::Wind)
                .PlayRandom();

            animStatsTimeCounterSec_ = 0.0f;
            animStatsDelayPerSec_ = 0.01f; // any fraction of a second will work here
            isAnimStats_ = true;
            SetMenuButtonsDisabledWhileStatsAreAnimating(true);
            isWaitingForStats_ = false;
            UndoAndClearStatModifierChanges();
            return true;
        }
        else
        {
            return RESULT;
        }*/
    }

    bool CharacterStage::KeyRelease(const sf::Event::KeyEvent & KEY_EVENT)
    {
        if (StageBase::KeyRelease(KEY_EVENT))
        {
            return true;
        }

        if (nameTextEntryBoxUPtr_->HasFocus())
        {
            if ((KEY_EVENT.code == sf::Keyboard::Return) || (KEY_EVENT.code == sf::Keyboard::Tab))
            {
                nameTextEntryBoxUPtr_->SetHasFocus(false);
                statBox_.Focus();
            }

            return true;
        }

        if (KEY_EVENT.code == sf::Keyboard::Space)
        {
            /*isAnimStats_ = false;
            isWaitingForStats_ = true;*/
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
            StageBase::UpdateMouseDown(MOUSE_POS_V);
        }

        /*if ((initialRollCounter_ >= 6) && (false == AreAnyAnimNumStillMoving()))
        {
            auto isNumberHeldDown { false };
            for (EnumUnderlying_t i(0); i < creature::Traits::StatCount; ++i)
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
        }*/
    }

    void CharacterStage::Setup_MenuButtons()
    {
        const std::size_t MENU_BUTTON_COUNT { 4 };
        const auto MENU_BUTTON_EXTRA_SPACER_RATIO { 0.0125f };

        const gui::EvenSpacer SPACER(
            MENU_BUTTON_COUNT, StageRegion().width, MENU_BUTTON_EXTRA_SPACER_RATIO);

        auto calcButtonPos = [&](const std::size_t BUTTON_INDEX_LEFT_TO_RIGHT,
                                 const sf::Vector2f & BUTTON_SIZE_V) {
            return sf::Vector2f(
                SPACER.Position(BUTTON_INDEX_LEFT_TO_RIGHT, BUTTON_SIZE_V.x),
                bottomSymbol_.VisibleVerticalCenter() - (BUTTON_SIZE_V.y * 0.5f));
        };

        std::size_t buttonCounter { 0 };

        auto setupButton = [&](gui::MainMenuButtonUPtr_t & buttonUPtr) {
            buttonUPtr->SetEntityPos(
                calcButtonPos(buttonCounter++, sfutil::Size(buttonUPtr->GetEntityRegion())));

            EntityAdd(buttonUPtr);
        };

        setupButton(backButtonUPtr_);
        setupButton(helpButtonUPtr_);
        setupButton(saveButtonUPtr_);
        setupButton(nextButtonUPtr_);
    }

    void CharacterStage::Setup_RaceRadioButtons()
    {
        std::vector<std::string> raceNameVec;
        for (EnumUnderlying_t i(0); i < creature::race::Count_PlayerRaces; ++i)
        {
            raceNameVec.emplace_back(creature::race::Name(static_cast<creature::race::Enum>(i)));
        }

        raceRadioButtonUPtr_ = std::make_unique<gui::RadioOrCheckSet>(
            "RaceSelection",
            gui::RadioOrCheckSet::Callback_t::IHandlerPtr_t(this),
            true,
            gui::Brightness::Bright,
            gui::MouseTextInfo("", gui::GuiFont::System, RADIO_BUTTON_TEXT_SIZE_),
            raceNameVec,
            MakeGuiBoxEntityInfo());

        raceRadioButtonUPtr_->SetEntityPos(
            sfutil::ScreenRatioToPixelsHoriz(0.1f), sfutil::Bottom(stageTitle_.Region()));

        EntityAdd(raceRadioButtonUPtr_);
    }

    void CharacterStage::Setup_RoleRadioButtons()
    {
        std::vector<std::string> roleNameVec;
        for (std::size_t i(0); i < creature::role::PlayerRoleCount; ++i)
        {
            const auto ROLE { static_cast<creature::role::Enum>(i) };
            if (ROLE != creature::role::Wolfen)
            {
                roleNameVec.emplace_back(creature::role::Name(ROLE));
            }
        }

        roleRadioButtonUPtr_ = std::make_unique<gui::RadioOrCheckSet>(
            "RoleSelection",
            gui::RadioOrCheckSet::Callback_t::IHandlerPtr_t(this),
            true,
            gui::Brightness::Bright,
            gui::MouseTextInfo("", gui::GuiFont::System, RADIO_BUTTON_TEXT_SIZE_),
            roleNameVec,
            MakeGuiBoxEntityInfo());

        roleRadioButtonUPtr_->SetEntityPos(
            sfutil::ScreenRatioToPixelsHoriz(0.1f),
            (sfutil::Bottom(raceRadioButtonUPtr_->GetEntityRegion())
             + sfutil::ScreenRatioToPixelsVert(0.1f)));

        EntityAdd(roleRadioButtonUPtr_);
    }

    void CharacterStage::Setup_RaceDescriptionBox()
    {
        /*
        const auto RACE_ENUM { static_cast<creature::race::Enum>(
            raceRadioButtonUPtr_->GetSelectedNumber()) };

        const std::string RACE_DESC(creature::race::Desc(RACE_ENUM));

        const sf::FloatRect REGION(
            raceRadioButtonUPtr_->GetEntityRegion().left
                + raceRadioButtonUPtr_->GetEntityRegion().width + 15.0f,
            raceRadioButtonUPtr_->GetEntityPos().y + sfutil::MapByRes(15.0f, 45.0f),
            sfutil::MapByRes(232.0f, 1500.0f),
            raceRadioButtonUPtr_->GetEntityRegion().height - 30.0f);

        gui::TextInfo raceDescTextInfo(
            RACE_DESC,
            gui::GuiFont::System,
            30,
            DESC_TEXT_COLOR_,
            gui::Justified::Left);

        if (!racetDescTextRegionUPtr_)
        {
            racetDescTextRegionUPtr_ = std::make_unique<gui::TextRegion>(
                "RaceDescription",
                raceDescTextInfo,
                REGION,
                stage::IStagePtr_t(this),
                DESC_TEXT_FONT_SIZE_);

            EntityAdd(racetDescTextRegionUPtr_);
        }
        else
        {
            racetDescTextRegionUPtr_->Setup(
                raceDescTextInfo, REGION, stage::IStagePtr_t(this), DESC_TEXT_FONT_SIZE_);
        }
        */
    }

    void CharacterStage::Setup_RoleDescriptionBox()
    {
        /*
        const auto RACE_ENUM { static_cast<creature::race::Enum>(
            raceRadioButtonUPtr_->GetSelectedNumber()) };

        const auto ROLE_SELECTED { static_cast<creature::role::Enum>(
            roleRadioButtonUPtr_->GetSelectedNumber()) };

        const auto ROLE_ENUM { (
            (RACE_ENUM == creature::race::Wolfen) ? creature::role::Wolfen : ROLE_SELECTED) };

        const auto ROLE_RADIOBUTTON_REGION { roleRadioButtonUPtr_->GetEntityRegion() };

        const sf::FloatRect REGION(
            ROLE_RADIOBUTTON_REGION.left + ROLE_RADIOBUTTON_REGION.width + 15.0f,
            ROLE_RADIOBUTTON_REGION.top + sfutil::MapByRes(25.0f, 75.0f),
            sfutil::MapByRes(240.0f, 1750.0f),
            roleRadioButtonUPtr_->GetEntityRegion().height - 30.0f);

        gui::TextInfo roleDescTextInfo(
            creature::role::Desc(ROLE_ENUM),
            gui::GuiFont::System,
            30,
            DESC_TEXT_COLOR_,
            gui::Justified::Left);

        if (!roletDescTextRegionUPtr_)
        {
            roletDescTextRegionUPtr_ = std::make_unique<gui::TextRegion>(
                "RoleDescription",
                roleDescTextInfo,
                REGION,
                stage::IStagePtr_t(this),
                DESC_TEXT_FONT_SIZE_);

            EntityAdd(roletDescTextRegionUPtr_);
        }
        else
        {
            roletDescTextRegionUPtr_->Setup(
                roleDescTextInfo, REGION, stage::IStagePtr_t(this), DESC_TEXT_FONT_SIZE_);
        }
        */
    }

    void CharacterStage::Setup_NameLabel()
    {
        const gui::TextInfo NAME_LABEL_TEXT_INFO(
            "(name your character here)",
            gui::GuiFont::System,
            gui::FontManager::Instance()->Size_Small(),
            LIGHT_TEXT_COLOR_,
            gui::Justified::Left,
            sf::Text::Italic);

        nInsTextRegionUPtr_
            = std::make_unique<gui::TextRegion>("NameLabel", NAME_LABEL_TEXT_INFO, sf::FloatRect());

        nInsTextRegionUPtr_->SetEntityPos(
            sfutil::DisplayCenterHoriz(nInsTextRegionUPtr_->GetEntityRegion().width),
            sfutil::Bottom(stageTitle_.Region()));

        EntityAdd(nInsTextRegionUPtr_);
    }

    void CharacterStage::Setup_NameTextEntryBox()
    {
        creature::NameInfo creatureNameInfo;

        gui::TextInfo nameEntryTextInfo(creatureNameInfo.MakeTextInfo());
        nameEntryTextInfo.text = "";
        nameEntryTextInfo.color = DESC_TEXT_COLOR_;

        const sf::FloatRect REGION(
            sfutil::DisplayCenterHoriz(creatureNameInfo.DefaultTextEntryBoxWidth()),
            (sfutil::Bottom(nInsTextRegionUPtr_->GetEntityRegion())
             + sfutil::ScreenRatioToPixelsVert(0.0075f)),
            creatureNameInfo.DefaultTextEntryBoxWidth(),
            sfutil::ScreenRatioToPixelsVert(0.03f));

        nameTextEntryBoxUPtr_ = std::make_unique<gui::TextEntryBox>(
            gui::TextEntryBox::Callback_t::IHandlerPtr_t(this),
            "CharacterName",
            REGION,
            nameEntryTextInfo,
            LIGHT_TEXT_COLOR_,
            MakeGuiBoxEntityInfo());

        EntityAdd(nameTextEntryBoxUPtr_);
    }

    void CharacterStage::Setup_SexRadioButtons()
    {
        sexRadioButtonUPtr_ = std::make_unique<gui::RadioOrCheckSet>(
            "SexSelection",
            gui::RadioOrCheckSet::Callback_t::IHandlerPtr_t(this),
            true,
            gui::Brightness::Bright,
            gui::MouseTextInfo("", gui::GuiFont::System, RADIO_BUTTON_TEXT_SIZE_),
            std::vector<std::string> { "Male", "Female" },
            MakeGuiBoxEntityInfo());

        sexRadioButtonUPtr_->SetEntityPos(
            sfutil::DisplayCenterHoriz(sexRadioButtonUPtr_->GetEntityRegion().width),
            (sfutil::Bottom(nameTextEntryBoxUPtr_->GetEntityRegion())
             + sfutil::ScreenRatioToPixelsVert(0.05f)));

        EntityAdd(sexRadioButtonUPtr_);
    }

    void CharacterStage::Setup_SpacebarInstructionText()
    {
        /*
        std::stringstream spacebarInstrTextSS;
        spacebarInstrTextSS
            << "(click the box below then hold the spacebar to change "
            << "attributes)\n(drag numbers up and down to customize)                ";

        gui::TextInfo insTextInfo(
            spacebarInstrTextSS.str(),
            gui::GuiFont::SystemCondensed,
            gui::FontManager::Instance()->Size_Small(),
            LIGHT_TEXT_COLOR_,
            gui::Justified::Center,
            sf::Text::Italic);

        sbInsTextRegionUPtr_ = std::make_unique<gui::TextRegion>(
            "SpacebarInstructions", insTextInfo, sf::FloatRect());

        sbInsTextRegionUPtr_->SetEntityPos(
            ((StageRegion().width * 0.5f) - (sbInsTextRegionUPtr_->GetEntityRegion().width * 0.5f))
                + 100.0f,
            sexRadioButtonUPtr_->GetEntityRegion().top
                + sexRadioButtonUPtr_->GetEntityRegion().height
                + sfutil::MapByRes(30.0f, 90.0f));

        EntityAdd(sbInsTextRegionUPtr_);
        */
    }

    /*void CharacterStage::Setup_FixedStats(const gui::TextInfo & STAT_TEXT_INFO)
    {
        // fill the fixed stats vector with dummy values initially...
        const std::size_t NUM_TRAITS(creature::Traits::StatCount);
        for (std::size_t i(0); i < NUM_TRAITS; ++i)
        {
            fixedStatsSVec_.emplace_back(AnimNumSPtr_t());
        }

        const auto TARGET_POS_LEFT { statBox_.NumberPosLeft(); };

        //...then assign valid AnimNumSPtr_t objects with a value of zero
        fixedStatsSVec_[creature::Traits::Strength] = std::make_shared<AnimNum>(
            STAT_INVALID_,
            creature::Traits::Strength,
            TARGET_POS_LEFT,
            statsStrPosTop_,
            TARGET_POS_LEFT,
            statsStrPosTop_,
            STAT_TEXT_INFO);

        fixedStatsSVec_[creature::Traits::Accuracy] = std::make_shared<AnimNum>(
            STAT_INVALID_,
            creature::Traits::Accuracy,
            TARGET_POS_LEFT,
            statsAccPosTop_,
            TARGET_POS_LEFT,
            statsAccPosTop_,
            STAT_TEXT_INFO);

        fixedStatsSVec_[creature::Traits::Charm] = std::make_shared<AnimNum>(
            STAT_INVALID_,
            creature::Traits::Charm,
            TARGET_POS_LEFT,
            statsChaPosTop_,
            TARGET_POS_LEFT,
            statsChaPosTop_,
            STAT_TEXT_INFO);

        fixedStatsSVec_[creature::Traits::Luck] = std::make_shared<AnimNum>(
            STAT_INVALID_,
            creature::Traits::Luck,
            TARGET_POS_LEFT,
            statsLckPosTop_,
            TARGET_POS_LEFT,
            statsLckPosTop_,
            STAT_TEXT_INFO);

        fixedStatsSVec_[creature::Traits::Speed] = std::make_shared<AnimNum>(
            STAT_INVALID_,
            creature::Traits::Speed,
            TARGET_POS_LEFT,
            statsSpdPosTop_,
            TARGET_POS_LEFT,
            statsSpdPosTop_,
            STAT_TEXT_INFO);

        fixedStatsSVec_[creature::Traits::Intelligence] = std::make_shared<AnimNum>(
            STAT_INVALID_,
            creature::Traits::Intelligence,
            TARGET_POS_LEFT,
            statsIntPosTop_,
            TARGET_POS_LEFT,
            statsIntPosTop_,
            STAT_TEXT_INFO);
    }*/

    void CharacterStage::Setup_SmokeAnimation(const float ATTRIB_BOX_TOP)
    {
        EntityAdd(smokeAnimUPtr_);

        const auto DRIFT_LIMIT_LEFT { StageRegion().width * 0.65f };
        const auto DRIFT_LIMIT_RIGHT { StageRegion().width
                                       - smokeAnimUPtr_->GetEntityRegion().width };

        smokeAnimSliderDriftX_ = gui::SliderDrift<float>(
            DRIFT_LIMIT_LEFT,
            DRIFT_LIMIT_RIGHT,
            SMOKE_ANIM_SPEED_MIN_,
            SMOKE_ANIM_SPEED_MAX_,
            misc::random::Float(DRIFT_LIMIT_LEFT, DRIFT_LIMIT_RIGHT),
            misc::random::Float(DRIFT_LIMIT_LEFT, DRIFT_LIMIT_RIGHT));

        const auto VERT_OVERLAP { sfutil::ScreenRatioToPixelsVert(0.0333f) };

        const auto DRIFT_LIMIT_TOP { sfutil::Bottom(stageTitle_.Region()) - VERT_OVERLAP };

        const auto DRIFT_LIMIT_BOTTOM { (ATTRIB_BOX_TOP - smokeAnimUPtr_->GetEntityRegion().height)
                                        + (VERT_OVERLAP * 2.0f) };

        smokeAnimSliderDriftY_ = gui::SliderDrift<float>(
            DRIFT_LIMIT_TOP,
            DRIFT_LIMIT_BOTTOM,
            SMOKE_ANIM_SPEED_MIN_,
            SMOKE_ANIM_SPEED_MAX_,
            misc::random::Float(DRIFT_LIMIT_TOP, DRIFT_LIMIT_BOTTOM),
            misc::random::Float(DRIFT_LIMIT_TOP, DRIFT_LIMIT_BOTTOM));
    }

    float CharacterStage::Setup_AttributeDescriptionBox()
    {
        const auto ATTR_DESC_WIDTH { sfutil::MapByRes(335.0f, 3000.0f) };

        const auto REGION_TOP { sfutil::MapByRes(450.0f, 1600.0f) };

        const sf::FloatRect REGION(
            (StageRegion().width - ATTR_DESC_WIDTH) - sfutil::MapByRes(15.0f, 300.0f),
            REGION_TOP,
            ATTR_DESC_WIDTH,
            sfutil::MapByRes(310.0f, 2100.0f));

        gui::TextInfo descTextInfo(
            "", // see below for where this is set to a valid value
            gui::GuiFont::System,
            DESC_TEXT_FONT_SIZE_,
            DESC_TEXT_COLOR_,
            gui::Justified::Center);

        gui::TextRegionUVec_t textRegionUVec;

        // Setup the strength attribute here because it is special.
        // See below where it is added by hand.
        std::ostringstream ss;
        ss << creature::Traits::Name(creature::Traits::Strength) << " - "
           << misc::ConfigFile::Instance()->Value("stats-stat-desc_Strength") << "\n\n";

        const auto STRENGTH_BASE_TEXT { ss.str() };

        gui::TextInfo strHelpTextInfo(descTextInfo);

        Setup_AttributeHelpText(
            creature::Traits::Strength, REGION, strHelpTextInfo, textRegionUVec);

        Setup_Attribute(
            creature::Traits::Accuracy,
            "stats-stat-desc_Accuracy",
            REGION,
            descTextInfo,
            textRegionUVec);

        Setup_Attribute(
            creature::Traits::Charm, "stats-stat-desc_Charm", REGION, descTextInfo, textRegionUVec);

        Setup_Attribute(
            creature::Traits::Luck, "stats-stat-desc_Luck", REGION, descTextInfo, textRegionUVec);

        Setup_Attribute(
            creature::Traits::Speed, "stats-stat-desc_Speed", REGION, descTextInfo, textRegionUVec);

        Setup_Attribute(
            creature::Traits::Intelligence,
            "stats-stat-desc_Intelligence",
            REGION,
            descTextInfo,
            textRegionUVec);

        // setup the final TextRegion that will be drawn to the screen
        descTextInfo.text = STRENGTH_BASE_TEXT;
        if (!attrDescTextRegionUPtr_)
        {
            attrDescTextRegionUPtr_ = std::make_unique<gui::TextRegion>(
                "AttributeDescription",
                descTextInfo,
                REGION,
                gui::BoxEntityInfo(),
                stage::IStagePtr_t(this));

            EntityAdd(attrDescTextRegionUPtr_);
        }
        else
        {
            attrDescTextRegionUPtr_->Setup(
                descTextInfo, REGION, gui::BoxEntityInfo(), stage::IStagePtr_t(this));
        }

        // for (const auto & NEXT_TR_UPTR : textRegionUVec)
        //{
        //    attrDescTextRegionUPtr_->Append(*NEXT_TR_UPTR);
        //}

        return REGION_TOP;
    }

    void CharacterStage::Setup_Attribute(
        const creature::Traits::Enum TRAIT_ENUM,
        const std::string & DESC_KEY,
        const sf::FloatRect & REGION,
        gui::TextInfo & descTextInfo,
        gui::TextRegionUVec_t & textRegionUVec)
    {
        std::ostringstream ss;

        ss << creature::Traits::Name(TRAIT_ENUM) << " - "
           << misc::ConfigFile::Instance()->Value(DESC_KEY) << "\n\n";

        descTextInfo.text = ss.str();

        textRegionUVec.emplace_back(std::make_unique<gui::TextRegion>(
            creature::Traits::ToString(TRAIT_ENUM) + "StatDesc", descTextInfo, REGION));

        gui::TextInfo helpTextInfo(descTextInfo);

        Setup_AttributeHelpText(TRAIT_ENUM, REGION, helpTextInfo, textRegionUVec);
    }

    void CharacterStage::Setup_AttributeHelpText(
        const creature::Traits::Enum TRAIT_ENUM,
        const sf::FloatRect & REGION,
        gui::TextInfo & helpTextInfo,
        gui::TextRegionUVec_t & textRegionUVec)
    {
        GetStatHelpText(TRAIT_ENUM, helpTextInfo);

        if (helpTextInfo.text.empty() == false)
        {
            textRegionUVec.emplace_back(std::make_unique<gui::TextRegion>(
                creature::Traits::ToString(TRAIT_ENUM) + "AttributeDesc", helpTextInfo, REGION));
        }
    }

    const std::string CharacterStage::OnSaveButton()
    {
        if (AreAnyAnimNumStillMoving())
        {
            return "ignored because number anims are still moving around";
        }

        if (AreAnyStatsIgnored())
        {
            MissingAttributesPopup();

            return "but attributes were missing (without numbers) so spawned the missing "
                   "attributes popup";
        }

        // verify name is not blank/empty
        const auto CHARACTER_NAME { misc::TrimWhitespaceCopy(nameTextEntryBoxUPtr_->GetText()) };
        if (CHARACTER_NAME.empty())
        {
            CharacterNameMissingPopup();
            return "but the character name text entry box was empty so spawned a popup for that";
        }

        CharacterImageSelectionPopup(CHARACTER_NAME);
        return "and so the character image selection popup was spawned";
    }

    bool CharacterStage::OnBackButton()
    {
        if (AreAnyAnimNumStillMoving())
        {
            return false;
        }
        else
        {
            LeaveStageConfirmPopup(misc::TrimWhitespaceCopy(nameTextEntryBoxUPtr_->GetText()));
            return true;
        }
    }

    const std::string CharacterStage::OnNextButton()
    {
        // verify all stats are ready
        if (AreAnyAnimNumStillMoving())
        {
            return "but ignored because number anims are still moving around";
        }

        const auto NAME { misc::TrimWhitespaceCopy(nameTextEntryBoxUPtr_->GetText()) };

        // if it seems the user has been working on a character, prompt before losing the data
        if (false == NAME.empty())
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

            SpawnPopup(
                misc::MakeNotNull(this),
                popup::PopupManager::Instance()->CreatePopupInfo(
                    POPUP_NAME_NEXTBUTTON_LEAVESCREENCONFIRM_,
                    ss.str(),
                    popup::PopupButtons::YesNo,
                    popup::PopupImage::Regular,
                    gui::Justified::Center,
                    gui::sound_effect::PromptWarn));

            return "and leave stage and partially completed character popup spawned";
        }
        else
        {
            TransitionTo(stage::Stage::Party);

            return "and transitioned to party stage";
        }
    }

    void CharacterStage::MissingAttributesPopup()
    {
        std::ostringstream ss;
        ss << "The following attributes are blank: ";

        /*bool foundAnyToIgnore(false);
        for (EnumUnderlying_t i(0); i < creature::Traits::StatCount; ++i)
        {
            if (fixedStatsSVec_[i]->IgnoreMe())
            {
                ss << ((false == foundAnyToIgnore) ? "" : ", ")
                   << creature::Traits::Name(static_cast<creature::Traits::Enum>(i));

                foundAnyToIgnore = true;
            }
        }*/

        ss << ".  All attributes must have a value before your character can be created.  "
           << "Hold down the spacebar until all attributes have values.";

        const auto POPUP_INFO { popup::PopupManager::Instance()->CreatePopupInfo(
            POPUP_NAME_MISSINGATTRIBS_,
            ss.str(),
            popup::PopupButtons::Okay,
            popup::PopupImage::RegularSidebar,
            gui::Justified::Left,
            gui::sound_effect::PromptQuestion) };

        SpawnPopup(misc::MakeNotNull(this), POPUP_INFO);
    }

    void CharacterStage::CharacterNameMissingPopup()
    {
        SpawnPopup(
            misc::MakeNotNull(this),
            popup::PopupManager::Instance()->CreatePopupInfo(
                POPUP_NAME_NONAMEERROR_,
                "\n\nThe name box is empty.  You must name your character to continue.",
                popup::PopupButtons::Okay,
                popup::PopupImage::Regular,
                gui::Justified::Center,
                gui::sound_effect::PromptWarn));
    }

    void CharacterStage::CharacterImageSelectionPopup(const std::string &)
    {
        /*
        const auto SEX { GetCurrentSelectedSex() };

        const auto RACE { static_cast<creature::race::Enum>(
            raceRadioButtonUPtr_->GetSelectedNumber()) };

        const auto ROLE { (
            (RACE == creature::race::Wolfen)
                ? creature::role::Wolfen
                : static_cast<creature::role::Enum>(roleRadioButtonUPtr_->GetSelectedNumber())) };

        gui::CreatureImagePaths creatureImageLoader;
        characterImageFilenamesVec_ = creatureImageLoader.Filenames(RACE, ROLE, SEX);

        gui::CachedTextureVec_t characterTextureVec;
        for (const auto & FILENAME : characterImageFilenamesVec_)
        {
            characterTextureVec.emplace_back(gui::CachedTexture(
                PathWrapper(creatureImageLoader.Path(FILENAME)),
                gui::ImageOptions(
                    gui::ImageOpt::Default | gui::ImageOpt::FlipHoriz)));
        }

        std::ostringstream ss;
        ss << "Choose an image for \"" << CHARACTER_NAME << "\"";

        const auto POPUP_INFO { popup::PopupManager::Instance()->CreateImageSelectionPopupInfo(
            POPUP_NAME_IMAGE_SELECTION_, ss.str(), characterTextureVec, true) };

        SpawnPopup(misc::MakeNotNull(this), POPUP_INFO);
            */
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

            SpawnPopup(
                misc::MakeNotNull(this),
                popup::PopupManager::Instance()->CreatePopupInfo(
                    POPUP_NAME_BACKBUTTON_LEAVESCREENCONFIRM_,
                    ss.str(),
                    popup::PopupButtons::YesNo,
                    popup::PopupImage::Regular,
                    gui::Justified::Center,
                    gui::sound_effect::PromptWarn));
        }
        else
        {
            TransitionTo(stage::Stage::Menu);
        }
    }

    void CharacterStage::Help1Popup()
    {
        SpawnPopup(
            misc::MakeNotNull(this),
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
        SpawnPopup(
            misc::MakeNotNull(this),
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
        SpawnPopup(
            misc::MakeNotNull(this),
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

    void CharacterStage::CharacterCreationConfirmPopup(const std::size_t)
    {
        /*selectedImageIndex_ = IMAGE_INDEX;

        const auto CHARACTER_NAME { misc::TrimWhitespaceCopy(nameTextEntryBoxUPtr_->GetText()) };

        const bool IS_MALE { GetCurrentSelectedSex() == creature::sex::Male };

        const auto RACE { static_cast<creature::race::Enum>(
            raceRadioButtonUPtr_->GetSelectedNumber()) };

        const auto ROLE { (
            (RACE == creature::race::Wolfen)
                ? creature::role::Wolfen
                : static_cast<creature::role::Enum>(roleRadioButtonUPtr_->GetSelectedNumber())) };

        // setup the final StatSet for the character
        creature::StatSet statSetFinal;
        for (EnumUnderlying_t i(0); i < creature::Traits::StatCount; ++i)
        {
            const auto NEXT_ENUM { static_cast<creature::Traits::Enum>(i) };
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

        SpawnPopup(misc::MakeNotNull(this),
            popup::PopupManager::Instance()->CreatePopupInfo(
                POPUP_NAME_CREATECONFIRM_,
                ss.str(),
                popup::PopupButtons::YesNo,
                popup::PopupImage::Large,
                gui::Justified::Center,
                gui::sound_effect::PromptQuestion));*/
    }

    void CharacterStage::AdjustRoleRadioButtonsForRace(const creature::race::Enum)
    {
        /*const auto VALID_ROLES { creature::race::Roles(WHICH_RACE) };

        std::vector<std::size_t> invalidRoleIndexes;
        invalidRoleIndexes.reserve(creature::role::PlayerRoleCount);

        const auto NUM_PLAYER_ROLES { static_cast<std::size_t>(creature::role::PlayerRoleCount) };
        for (std::size_t i(0); i < NUM_PLAYER_ROLES; ++i)
        {
            const auto ROLE { static_cast<creature::role::Enum>(i) };

            const auto FOUND_ITER { std::find(
                std::begin(VALID_ROLES), std::end(VALID_ROLES), ROLE) };

            if (FOUND_ITER == std::end(VALID_ROLES))
            {
                invalidRoleIndexes.emplace_back(i);
            }
        }

        roleRadioButtonUPtr_->SetInvalidSelections(invalidRoleIndexes);*/
    }

    bool CharacterStage::GetStatHelpText(const creature::Traits::Enum, gui::TextInfo &) const
    {
        return false;
        /*// bail if no values yet
        if ((static_cast<std::size_t>(WHICH_STAT) >= fixedStatsSVec_.size())
            || (AreAnyAnimNumStillMoving()) || (AreAnyStatsIgnored()))
        {
            textInfo.text.clear();
            textInfo.color = DESC_TEXT_COLOR_;
            return false;
        }

        const auto RECC_NORMAL_STR {
            "It would be a good idea to increase this attribute if you can."
        };

        const auto RECC_STRONG_STR {
            "It is strongly recommended that you increase this attribute."
        };

        sf::Color reccNormalColor(DESC_TEXT_COLOR_);
        reccNormalColor.g = static_cast<sf::Uint8>(static_cast<int>(reccNormalColor.g) + 20);
        reccNormalColor.b = static_cast<sf::Uint8>(static_cast<int>(reccNormalColor.b) + 40);

        sf::Color reccStrongColor(255, 200, 181);

        const auto RACE_ENUM { static_cast<creature::race::Enum>(
            raceRadioButtonUPtr_->GetSelectedNumber()) };

        const auto ROLE_ENUM { static_cast<creature::role::Enum>(
            roleRadioButtonUPtr_->GetSelectedNumber()) };

        const auto WHICH_ROLE { (
            (RACE_ENUM == creature::race::Wolfen) ? creature::role::Wolfen : ROLE_ENUM) };

        const auto STAT_VALUE { fixedStatsSVec_[static_cast<std::size_t>(WHICH_STAT)]->Value() };

        std::ostringstream sss;
        sss << "Your current " << creature::Traits::ToString(WHICH_STAT) << " is only "
            << STAT_VALUE << ".";

        const auto IS_ONLY_STR { sss.str() };

        const creature::Trait_t STAT_VALUE_LOW { 10 };
        const creature::Trait_t STAT_VALUE_MED { 15 };
        const creature::Trait_t STAT_VALUE_HIGH { 20 };
        const creature::Trait_t STAT_VALUE_HIGHEST { 25 };

        std::ostringstream ss;
        if (WHICH_ROLE == creature::role::Archer)
        {
            if (WHICH_STAT == creature::Traits::Accuracy)
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

            if (WHICH_STAT == creature::Traits::Strength)
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

            if (WHICH_STAT == creature::Traits::Speed)
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
            if (WHICH_STAT == creature::Traits::Charm)
            {
                const auto REASON_STR { "Charm is important to Bards for magical song." };

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

            if (WHICH_STAT == creature::Traits::Intelligence)
            {
                const auto REASON_STR { "Intelligence is important to Bards for magical song." };

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

            if (WHICH_STAT == creature::Traits::Strength)
            {
                const auto REASON_STR { std::string("Strength is sometimes important to Bards")
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
            if (WHICH_STAT == creature::Traits::Intelligence)
            {
                const auto REASON_STR {
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

            if (WHICH_STAT == creature::Traits::Strength)
            {
                const auto REASON_STR { std::string("Strength is often important to Beastmasters")
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
            if (WHICH_STAT == creature::Traits::Charm)
            {
                const auto REASON_STR { std::string("A Cleric's primary attribute is Charm ")
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

            if (WHICH_STAT == creature::Traits::Intelligence)
            {
                const auto REASON_STR {
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
            if (WHICH_STAT == creature::Traits::Strength)
            {
                const auto REASON_STR {
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

            if (WHICH_STAT == creature::Traits::Intelligence)
            {
                const auto REASON_STR {
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
            const auto REASON_STR { std::string("Knights are fighters, so they need Strength,")
                                    + " Accuracy, and Speed -in that order." };

            if (WHICH_STAT == creature::Traits::Strength)
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

            if (WHICH_STAT == creature::Traits::Speed)
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

            if (WHICH_STAT == creature::Traits::Accuracy)
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
            if (WHICH_STAT == creature::Traits::Intelligence)
            {
                const auto REASON_STR { std::string("A Sorcerer is first and foremost a spell ")
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

            if (WHICH_STAT == creature::Traits::Speed)
            {
                const auto REASON_STR { std::string("Sorcerers are typically physically weaker")
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
            if (WHICH_STAT == creature::Traits::Speed)
            {
                const auto REASON_STR {
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

            if (WHICH_STAT == creature::Traits::Luck)
            {
                const auto REASON_STR { "Thieves need Luck more than any other attribute." };

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
            if (WHICH_STAT == creature::Traits::Strength)
            {
                const auto REASON_STR {
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

            if (WHICH_STAT == creature::Traits::Accuracy)
            {
                const auto REASON_STR {
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

            if (WHICH_STAT == creature::Traits::Speed)
            {
                const auto REASON_STR {
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
        return !textInfo.text.empty();*/
    }

    void CharacterStage::UndoAndClearStatModifierChanges()
    {
        /*// undo modifier changes to statSetBase
        for (std::size_t s(0); s < creature::Traits::StatCount; ++s)
        {
            const auto NEXT_TRAIT_ENUM { static_cast<creature::Traits::Enum>(s) };
            const auto NUM_MODIFIERS { statModifierTextVec_.size() };
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
        statModifierTextVec_.clear();*/
    }

    void CharacterStage::SetVisibleStatsToStatSetBase()
    {
        /*for (EnumUnderlying_t i(0); i < creature::Traits::StatCount; ++i)
        {
            const auto NEXT_TRAIT_ENUM { static_cast<creature::Traits::Enum>(i) };
            const auto NEW_VAL { statSetBase_.Get(NEXT_TRAIT_ENUM) };

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
        }*/
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
        /*statModifierTextVec_.clear();

        const auto RACE { static_cast<creature::race::Enum>(
            raceRadioButtonUPtr_->GetSelectedNumber()) };

        const auto RACE_STAT_MODIFIERS{ creature::RaceStatModifier::Get(RACE) };

        const auto HORIZ_OFFSET { sfutil::MapByRes(90.0f, 150.0f) };
        const auto VERT_OFFSET { 5.0f };
        const auto RACE_NAME_ABBR { creature::race::Abbr(RACE) };

        std::vector<creature::Traits::Enum> preExistingStatVec;

        for (EnumUnderlying_t i(0); i < creature::Traits::StatCount; ++i)
        {
            const auto NEXT_TRAIT_ENUM { static_cast<creature::Traits::Enum>(i) };
            const auto NEXT_STAT_VAL { RACE_STAT_MODIFIERS.Get(NEXT_TRAIT_ENUM) };

            if ((NEXT_STAT_VAL != 0) && (false == fixedStatsSVec_[NEXT_TRAIT_ENUM]->IgnoreMe())
                && (false == fixedStatsSVec_[NEXT_TRAIT_ENUM]->IsHeldDown())
                && (initialRollCounter_ >= 6))
            {
                preExistingStatVec.emplace_back(NEXT_TRAIT_ENUM);
                statModifierTextVec_.emplace_back(StatModText(
                    NEXT_TRAIT_ENUM,
                    RACE_NAME_ABBR,
                    NEXT_STAT_VAL,
                    TARGET_POS_LEFT + HORIZ_OFFSET,
                    GetAttributeNumPosTop(NEXT_TRAIT_ENUM) + VERT_OFFSET));
            }
        }

        const auto RACE_ENUM { static_cast<creature::race::Enum>(
            raceRadioButtonUPtr_->GetSelectedNumber()) };

        const auto ROLE_SELECTED { static_cast<creature::role::Enum>(
            roleRadioButtonUPtr_->GetSelectedNumber()) };

        const auto ROLE_ENUM { (
            (RACE_ENUM == creature::race::Wolfen) ? creature::role::Wolfen : ROLE_SELECTED) };

        const auto ROLE_NAME_ABBR { creature::role::Abbr(ROLE_ENUM) };

        const auto ROLE_STAT_MODIFIERS{ creature::RoleStatModifier::Get(ROLE_ENUM) };

        for (EnumUnderlying_t i(0); i < creature::Traits::StatCount; ++i)
        {
            const auto NEXT_TRAIT_ENUM { static_cast<creature::Traits::Enum>(i) };
            const auto NEXT_STAT_VAL { ROLE_STAT_MODIFIERS.Get(NEXT_TRAIT_ENUM) };

            if ((NEXT_STAT_VAL != 0) && (false == fixedStatsSVec_[NEXT_TRAIT_ENUM]->IgnoreMe())
                && (false == fixedStatsSVec_[NEXT_TRAIT_ENUM]->IsHeldDown())
                && (initialRollCounter_ >= 6))
            {
                auto extraHorizOffset { 0.0f };

                if (std::find(preExistingStatVec.begin(), preExistingStatVec.end(), NEXT_TRAIT_ENUM)
                    == preExistingStatVec.end())
                {
                    extraHorizOffset = 0.0f;
                }
                else
                {
                    extraHorizOffset = sfutil::MapByRes(60.0f, 200.0f);
                }

                statModifierTextVec_.emplace_back(StatModText(
                    NEXT_TRAIT_ENUM,
                    ROLE_NAME_ABBR,
                    NEXT_STAT_VAL,
                    TARGET_POS_LEFT + HORIZ_OFFSET + extraHorizOffset,
                    GetAttributeNumPosTop(NEXT_TRAIT_ENUM) + VERT_OFFSET));
            }
        }*/
    }

    void CharacterStage::ApplyStatModifiersToStatSetBase()
    {
        /*const auto NUM_MODIFIERS { statModifierTextVec_.size() };
        for (std::size_t i(0); i < NUM_MODIFIERS; ++i)
        {
            statSetBase_.Set(
                statModifierTextVec_[i].stat,
                statSetBase_.Get(statModifierTextVec_[i].stat) + statModifierTextVec_[i].value);
        }*/
    }

    const gui::IEntityPtrOpt_t CharacterStage::UpdateMouseUp(const sf::Vector2f & MOUSE_POS_V)
    {
        /*if (AreAnyAnimNumStillMoving())
        {
            return boost::none;
        }*/

        // process MouseUp() on all other entity's
        auto entityWithFocusPtrOpt { StageBase::UpdateMouseUp(MOUSE_POS_V) };

        // remove animations that are finished from the vector
        /*animStatsSVec_.clear();

        // snap to closest position when drag stops
        const auto HELD_DOWN_STAT { GetHeldDownStat() };
        if (HELD_DOWN_STAT != statBox_.StatInvalid())
        {
            gui::SoundManager::Instance()->PlaySfx_AckMinor();

            fixedStatsSVec_[static_cast<std::size_t>(HELD_DOWN_STAT)]->SetPosY(
                GetStatPosTop(HELD_DOWN_STAT));
        }

        dragStartY_ = -1.0f; // anything less than zero here will work

        // clear isHeldDown status from all stat anim objects, and see if any were
        auto wereAnyHeldDown { false };
        for (EnumUnderlying_t i(0); i < creature::Traits::StatCount; ++i)
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

            gui::SoundManager::Instance()->PlaySfx_AckMajor();
        }
        */
        return entityWithFocusPtrOpt;
    }

    void CharacterStage::UpdateMousePos(const sf::Vector2i & NEW_MOUSE_POS_V)
    {
        StageBase::UpdateMousePos(NEW_MOUSE_POS_V);
        HandleAttributeDragging(sf::Vector2f(NEW_MOUSE_POS_V));
    }

    bool CharacterStage::AreAnyAnimNumStillMoving() const
    {
        /*for (const auto & ANIM_NUM_SPTR : animStatsSVec_)
        {
            if ((ANIM_NUM_SPTR->IgnoreMe() == false) && (ANIM_NUM_SPTR->IsDoneMoving() == false))
            {
                return true;
            }
        }

        for (const auto & FIXED_NUM_SPTR : fixedStatsSVec_)
        {
            if ((FIXED_NUM_SPTR->IgnoreMe() == false) && (FIXED_NUM_SPTR->IsDoneMoving() == false))
            {
                return true;
            }
        }
        */
        return false;
    }

    bool CharacterStage::AreAnyStatsIgnored() const
    {
        /*for (EnumUnderlying_t i(0); i < creature::Traits::StatCount; ++i)
        {
            if (fixedStatsSVec_[i]->IgnoreMe())
            {
                return true;
            }
        }
        */
        return false;
    }

    void CharacterStage::HandleAttributeDragging(const sf::Vector2f &)
    {
        // check there is a number being held down at all
        /*const auto STAT_HELD { GetHeldDownStat() };
        if (STAT_HELD == statBox_.StatInvalid())
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

        const auto STAT_BOX_ROW_HEIGHT { statBox_.RowHeight(); };
        const auto DIFF { dragStartY_ - NEW_POS_Y };
        const auto DIFF_MAX { STAT_BOX_ROW_HEIGHT - (STAT_BOX_ROW_HEIGHT / 3.0f) };
        const auto IS_MOVING_UP { DIFF > 0.0f };

        fixedStatsSVec_[STAT_HELD]->SetPosY(NEW_POS_Y);

        if (IS_MOVING_UP)
        {
            const auto STAT_ABOVE { GetStatAbove(STAT_HELD) };
            if ((STAT_ABOVE != statBox_.StatInvalid()) && (DIFF > DIFF_MAX))
            {
                dragStartY_ -= STAT_BOX_ROW_HEIGHT;

                fixedStatsSVec_[STAT_ABOVE]->SetPosY(
                    fixedStatsSVec_[STAT_ABOVE]->GetPos().y + STAT_BOX_ROW_HEIGHT);

                SwapAttributes(STAT_HELD, STAT_ABOVE);
            }
        }
        else
        {
            const auto STAT_BELOW { GetStatBelow(STAT_HELD) };
            if ((STAT_BELOW != statBox_.StatInvalid()) && (fabs(DIFF) > DIFF_MAX))
            {
                dragStartY_ += STAT_BOX_ROW_HEIGHT;

                fixedStatsSVec_[STAT_BELOW]->SetPosY(
                    fixedStatsSVec_[STAT_BELOW]->GetPos().y - STAT_BOX_ROW_HEIGHT);

                SwapAttributes(STAT_HELD, STAT_BELOW);
            }
        }*/
    }

    void CharacterStage::ProduceAnimatingDigits(const float)
    {
        /*if ((false == isAnimStats_) && (false == isWaitingForStats_))
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

            gui::TextInfo textInfo(
                "",
                gui::GuiFont::Number,
                40,
                sf::Color::White,
                gui::Justified::Left);

            const auto NEXT_VAL { misc::random::Int(1, STAT_INITIAL_MAX_) };

            auto numToUse { misc::random::Int(1, 6) };
            if (initialRollCounter_ <= 6)
            {
                numToUse = initialRollCounter_++;
            }

            const auto SMOKE_ANIM_MID_X { smokeAnimUPtr_->GetEntityPos().x
                                          + (smokeAnimUPtr_->GetEntityRegion().width * 0.5f)
                                          - 10.0f };

            const auto SMOKE_ANIM_MID_Y { smokeAnimUPtr_->GetEntityPos().y
                                          + (smokeAnimUPtr_->GetEntityRegion().height * 0.5f)
                                          - 10.0f };

            const auto ANIM_NUM_TARGET_X { TARGET_POS_LEFT + ((NEXT_VAL < 10) ? 10.0f : 0.0f)
                                           - 10.0f };

            switch (numToUse)
            {
                case 1:
                {
                    auto ansp = std::make_shared<AnimNum>(
                        NEXT_VAL,
                        creature::Traits::Strength,
                        SMOKE_ANIM_MID_X,
                        SMOKE_ANIM_MID_Y,
                        ANIM_NUM_TARGET_X,
                        statsStrPosTop_,
                        textInfo);
                    animStatsSVec_.emplace_back(ansp);

                    AnimNumSPtr_t toFadeSPtr(fixedStatsSVec_[creature::Traits::Strength]);
                    toFadeSPtr->WillFadeSet(true);
                    //
                    AnimNumSPtr_t holdFadeSPtr(animStatsSVec_[animStatsSVec_.size() - 1]);
                    holdFadeSPtr->CreateNewTextRegion();
                    holdFadeSPtr->WillFadeSet(false);
                    //
                    fixedStatsSVec_[creature::Traits::Strength] = holdFadeSPtr;
                    statSetBase_.Set(creature::Traits::Strength, NEXT_VAL);
                    animStatsSVec_.emplace_back(toFadeSPtr);
                    break;
                }

                case 2:
                {
                    auto ansp = std::make_shared<AnimNum>(
                        NEXT_VAL,
                        creature::Traits::Accuracy,
                        SMOKE_ANIM_MID_X,
                        SMOKE_ANIM_MID_Y,
                        ANIM_NUM_TARGET_X,
                        statsAccPosTop_,
                        textInfo);

                    animStatsSVec_.emplace_back(ansp);

                    AnimNumSPtr_t toFade(fixedStatsSVec_[creature::Traits::Accuracy]);
                    toFade->WillFadeSet(true);
                    //
                    AnimNumSPtr_t holdFade(animStatsSVec_[animStatsSVec_.size() - 1]);
                    holdFade->CreateNewTextRegion();
                    holdFade->WillFadeSet(false);
                    //
                    fixedStatsSVec_[creature::Traits::Accuracy] = holdFade;
                    statSetBase_.Set(creature::Traits::Accuracy, NEXT_VAL);
                    animStatsSVec_.emplace_back(toFade);
                    break;
                }

                case 3:
                {
                    auto ansp = std::make_shared<AnimNum>(
                        NEXT_VAL,
                        creature::Traits::Charm,
                        SMOKE_ANIM_MID_X,
                        SMOKE_ANIM_MID_Y,
                        ANIM_NUM_TARGET_X,
                        statsChaPosTop_,
                        textInfo);

                    animStatsSVec_.emplace_back(ansp);

                    AnimNumSPtr_t toFade(fixedStatsSVec_[creature::Traits::Charm]);
                    toFade->WillFadeSet(true);
                    //
                    AnimNumSPtr_t holdFade(animStatsSVec_[animStatsSVec_.size() - 1]);
                    holdFade->CreateNewTextRegion();
                    holdFade->WillFadeSet(false);
                    //
                    fixedStatsSVec_[creature::Traits::Charm] = holdFade;
                    statSetBase_.Set(creature::Traits::Charm, NEXT_VAL);
                    animStatsSVec_.emplace_back(toFade);
                    break;
                }

                case 4:
                {
                    auto ansp = std::make_shared<AnimNum>(
                        NEXT_VAL,
                        creature::Traits::Luck,
                        SMOKE_ANIM_MID_X,
                        SMOKE_ANIM_MID_Y,
                        ANIM_NUM_TARGET_X,
                        statsLckPosTop_,
                        textInfo);

                    animStatsSVec_.emplace_back(ansp);

                    AnimNumSPtr_t toFade(fixedStatsSVec_[creature::Traits::Luck]);
                    toFade->WillFadeSet(true);
                    //
                    AnimNumSPtr_t holdFade(animStatsSVec_[animStatsSVec_.size() - 1]);
                    holdFade->CreateNewTextRegion();
                    holdFade->WillFadeSet(false);
                    //
                    fixedStatsSVec_[creature::Traits::Luck] = holdFade;
                    statSetBase_.Set(creature::Traits::Luck, NEXT_VAL);
                    animStatsSVec_.emplace_back(toFade);
                    break;
                }

                case 5:
                {
                    auto ansp = std::make_shared<AnimNum>(
                        NEXT_VAL,
                        creature::Traits::Speed,
                        SMOKE_ANIM_MID_X,
                        SMOKE_ANIM_MID_Y,
                        ANIM_NUM_TARGET_X,
                        statsSpdPosTop_,
                        textInfo);

                    animStatsSVec_.emplace_back(ansp);

                    AnimNumSPtr_t toFade(fixedStatsSVec_[creature::Traits::Speed]);
                    toFade->WillFadeSet(true);
                    //
                    AnimNumSPtr_t holdFade(animStatsSVec_[animStatsSVec_.size() - 1]);
                    holdFade->CreateNewTextRegion();
                    holdFade->WillFadeSet(false);
                    //
                    fixedStatsSVec_[creature::Traits::Speed] = holdFade;
                    statSetBase_.Set(creature::Traits::Speed, NEXT_VAL);
                    animStatsSVec_.emplace_back(toFade);
                    break;
                }

                case 6:
                default:
                {
                    auto ansp = std::make_shared<AnimNum>(
                        NEXT_VAL,
                        creature::Traits::Intelligence,
                        SMOKE_ANIM_MID_X,
                        SMOKE_ANIM_MID_Y,
                        ANIM_NUM_TARGET_X,
                        statsIntPosTop_,
                        textInfo);

                    animStatsSVec_.emplace_back(ansp);

                    AnimNumSPtr_t toFade(fixedStatsSVec_[creature::Traits::Intelligence]);
                    toFade->WillFadeSet(true);
                    //
                    AnimNumSPtr_t holdFade(animStatsSVec_[animStatsSVec_.size() - 1]);
                    holdFade->CreateNewTextRegion();
                    holdFade->WillFadeSet(false);
                    //
                    fixedStatsSVec_[creature::Traits::Intelligence] = holdFade;
                    statSetBase_.Set(creature::Traits::Intelligence, NEXT_VAL);
                    animStatsSVec_.emplace_back(toFade);
                    break;
                }
            } // end switch()
        }

        // update each active animating fixed number, and check for any still moving
        auto areAnyFixedStillMoving { false };
        for (std::size_t f(0); f < creature::Traits::StatCount; ++f)
        {
            fixedStatsSVec_[f]->UpdateTime(ELAPSED_TIME_SECONDS);

            if (false == fixedStatsSVec_[f]->IsDoneMoving())
            {
                // don't break here because UpdateTime() needs to be called on each
                areAnyFixedStillMoving = true;
            }
        }

        // update each active animating number
        const auto NUM_DIGITS { animStatsSVec_.size() };
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
        else
        {
            if (false == isAnimStats_)
            {
                SetMenuButtonsDisabledWhileStatsAreAnimating(false);
            }
        }*/
    }

    void CharacterStage::SwapAttributes(const creature::Traits::Enum, const creature::Traits::Enum)
    {
        // swap the anim objects
        /*AnimNumSPtr_t tempSPtr(fixedStatsSVec_[static_cast<std::size_t>(B)]);
        fixedStatsSVec_[static_cast<std::size_t>(B)] = fixedStatsSVec_[static_cast<std::size_t>(A)];
        fixedStatsSVec_[static_cast<std::size_t>(A)] = tempSPtr;

        // swap the underlying stat values
        const auto TEMP_TRAIT { statSetBase_.Get(B) };
        statSetBase_.Set(B, statSetBase_.Get(A));
        statSetBase_.Set(A, TEMP_TRAIT);

        gui::SoundManager::Instance()->PlaySfx_TickOn();*/
    }

    void CharacterStage::HandleStuckAnims(const float)
    {
        /*std::vector<std::size_t> stuckAnimIndexVec;
        const auto NUM_ANIMS { animStatsSVec_.size() };
        for (std::size_t i(0); i < NUM_ANIMS; ++i)
        {
            if (animStatsSVec_[i]->UpdateTimer(ELAPSED_TIME_SEC))
            {
                stuckAnimIndexVec.emplace_back(i);
            }
        }

        const auto NUM_STUCK_ANIMS { stuckAnimIndexVec.size() };
        for (std::size_t i(0); i < NUM_STUCK_ANIMS; ++i)
        {
            const typename AnimNumSVec_t::iterator ITR_TO_ERASE {
                animStatsSVec_.begin() + AnimNumSVec_t::iterator::difference_type(i)
            };

            animStatsSVec_.erase(ITR_TO_ERASE);
        }*/
    }

    bool CharacterStage::HandleMenuNavigationKeyRelease(const sf::Event::KeyEvent & KEY_EVENT)
    {
        if (KEY_EVENT.code == sf::Keyboard::B)
        {
            backButtonUPtr_->SetMouseState(gui::MouseState::Over);
            gui::SoundManager::Instance()->PlaySfx_Keypress();
            OnBackButton();
            return true;
        }
        else if (KEY_EVENT.code == sf::Keyboard::S)
        {
            saveButtonUPtr_->SetMouseState(gui::MouseState::Over);
            gui::SoundManager::Instance()->PlaySfx_Keypress();
            OnSaveButton();
            return true;
        }
        else if (KEY_EVENT.code == sf::Keyboard::N)
        {
            nextButtonUPtr_->SetMouseState(gui::MouseState::Over);
            gui::SoundManager::Instance()->PlaySfx_Keypress();
            OnNextButton();
            return true;
        }
        else if (KEY_EVENT.code == sf::Keyboard::H)
        {
            helpButtonUPtr_->SetMouseState(gui::MouseState::Over);
            gui::SoundManager::Instance()->PlaySfx_Keypress();
            Help1Popup();
            return true;
        }
        else
        {
            return false;
        }
    }

    void CharacterStage::ResetForNewCharacterCreation()
    {
        /*initialRollCounter_ = 0;
        raceRadioButtonUPtr_->SetSelectNumber(0);
        roleRadioButtonUPtr_->SetSelectNumber(0);
        nameTextEntryBoxUPtr_->SetText("");
        statModifierTextVec_.clear();

        for (EnumUnderlying_t i(0); i < creature::Traits::StatCount; ++i)
        {
            fixedStatsSVec_[i]->SetIgnoreMe();
        }

        Setup_AttributeDescriptionBox();*/
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

    void CharacterStage::CreateCharacter()
    {
        /*M_HP_ASSERT_OR_LOG_AND_THROW(
            (selectedImageIndex_ < characterImageFilenamesVec_.size()),
            "stage::CharacterStage::CreateCharacter() called when selectedImageIndex_="
                << selectedImageIndex_ << " but characterImageFilenamesVec_.size()="
                << characterImageFilenamesVec_.size() << ".");

        const auto NAME { misc::TrimWhitespaceCopy(nameTextEntryBoxUPtr_->GetText()) };

        const auto SEX_ENUM { static_cast<creature::sex::Enum>(
            sexRadioButtonUPtr_->GetSelectedNumber() + 1) };

        const auto RACE_ENUM { static_cast<creature::race::Enum>(
            raceRadioButtonUPtr_->GetSelectedNumber()) };

        const auto ROLE_ENUM { (
            (RACE_ENUM == creature::race::Wolfen)
                ? creature::role::Wolfen
                : static_cast<creature::role::Enum>(roleRadioButtonUPtr_->GetSelectedNumber())) };

        creature::StatSet statSetFinal(statSetBase_);

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
        */
    }

    creature::sex::Enum CharacterStage::GetCurrentSelectedSex() const
    {
        return creature::sex::Male;
        /*return (
            (sexRadioButtonUPtr_->GetSelectedNumber() == 0) ? creature::sex::Male
                                                            : creature::sex::Female);*/
    }

    void CharacterStage::SetMenuButtonsDisabledWhileStatsAreAnimating(const bool WILL_DISABLE)
    {
        const auto NEW_MOUSE_STATE { (
            (WILL_DISABLE) ? gui::MouseState::Disabled : gui::MouseState::Up) };

        backButtonUPtr_->SetMouseState(NEW_MOUSE_STATE);
        saveButtonUPtr_->SetMouseState(NEW_MOUSE_STATE);
        helpButtonUPtr_->SetMouseState(NEW_MOUSE_STATE);
        nextButtonUPtr_->SetMouseState(NEW_MOUSE_STATE);
    }

    const gui::BoxEntityInfo CharacterStage::MakeGuiBoxEntityInfo() const
    {
        gui::BoxEntityInfo boxInfo;
        boxInfo.SetupImage(woodCachedTexture_);
        boxInfo.SetupBorder(true);
        boxInfo.focus_colors = sfutil::color::GuiFocusColors;
        return boxInfo;
    }

} // namespace stage
} // namespace heroespath
