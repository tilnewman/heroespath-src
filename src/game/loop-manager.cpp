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
// loop-manager.cpp
//
#include "loop-manager.hpp"

#include "sfml-util/loop-cmd-popup.hpp"
#include "sfml-util/loop.hpp"

#include "popup/popup-info.hpp"
#include "popup/popup-manager.hpp"
#include "popup/popup-stage-res-change.hpp"

#include "creature/creature.hpp"
#include "game/game-data-file.hpp"
#include "log/log-macros.hpp"
#include "stage/loop-cmd-addstage.hpp"

#include "misc/assertlogandthrow.hpp"

#include <string>

namespace heroespath
{
namespace game
{

    std::unique_ptr<LoopManager> LoopManager::instanceUPtr_{ nullptr };
    std::string LoopManager::startupStage_{ sfml_util::LoopState::ToString(
        sfml_util::LoopState::Intro) };

    LoopManager::LoopManager()
        : state_(sfml_util::LoopState::None)
        , cmdQueue_()
        , loop_("DefaultLoop")
        , popupResponse_(popup::ResponseTypes::None)
        , popupSelection_(0)
        , prevState_(sfml_util::LoopState::None)
        , prevSettingsState_(sfml_util::LoopState::None)
        , stateBeforeFade_(sfml_util::LoopState::None)
        , exitSuccess_(true)
    {
        M_HP_LOG_DBG("Singleton Construction: LoopManager");

        if (startupStage_.empty())
        {
            TransitionTo(sfml_util::LoopState::Intro);
        }
        else
        {
            TransitionTo(sfml_util::LoopState::FromString(startupStage_));
        }
    }

    LoopManager::~LoopManager() { M_HP_LOG_DBG("Singleton Destruction: LoopManager"); }

    LoopManager * game::LoopManager::Instance()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            M_HP_LOG_WRN("Singleton Instance() before Acquire(): LoopManager");
            Acquire();
        }

        return instanceUPtr_.get();
    }

    void LoopManager::Acquire()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            instanceUPtr_ = std::make_unique<LoopManager>();
        }
        else
        {
            M_HP_LOG_WRN("Singleton Acquire() after Construction: LoopManager");
        }
    }

    void LoopManager::Release()
    {
        M_ASSERT_OR_LOGANDTHROW_SS(
            (instanceUPtr_.get() != nullptr),
            "LoopManager::Release() found instanceUPtr that was null.");

        instanceUPtr_.reset();
    }

    game::Phase::Enum LoopManager::GetPhase() const
    {
        if (sfml_util::LoopState::Adventure == state_)
        {
            return game::Phase::Exploring;
        }
        else if (sfml_util::LoopState::Combat == state_)
        {
            return game::Phase::Combat;
        }
        else if (sfml_util::LoopState::Inventory == state_)
        {
            return game::Phase::Inventory;
        }
        else if (sfml_util::LoopState::Popup == state_)
        {
            if (sfml_util::LoopState::Adventure == prevState_)
            {
                return game::Phase::Exploring;
            }
            else if (sfml_util::LoopState::Combat == prevState_)
            {
                return game::Phase::Combat;
            }
            else if (sfml_util::LoopState::Inventory == prevState_)
            {
                return game::Phase::Inventory;
            }
        }

        return static_cast<game::Phase::Enum>(0);
    }

    void LoopManager::TransitionTo_Intro()
    {
        cmdQueue_.push(std::make_shared<sfml_util::LoopCmd_RemoveAllStages>());

        cmdQueue_.push(
            std::make_shared<sfml_util::LoopCmd_StateChange>(sfml_util::LoopState::Intro));

        cmdQueue_.push(std::make_shared<sfml_util::LoopCmd_ExitAfterKeypress>(true));
        cmdQueue_.push(std::make_shared<sfml_util::LoopCmd_ExitAfterMouseclick>(true));

        cmdQueue_.push(std::make_shared<sfml_util::LoopCmd_SetMouseVisibility>(false));

        cmdQueue_.push(std::make_shared<sfml_util::LoopCmd_AddStage_Default>());

        cmdQueue_.push(std::make_shared<sfml_util::LoopCmd_SetHoldTime>(2.5f));
        cmdQueue_.push(std::make_shared<sfml_util::LoopCmd_Execute>());

        // set the theme music volume to 25% even if the config file has it set lower so it can
        // always be heard during the intro
        auto const VOLUME_IF_INTRO_OR_CAMP_STAGE{ 25.0f };
        float targetVolumeToUse(sfml_util::MusicOperator::VOLUME_USE_GLOBAL_);
        if (sfml_util::SoundManager::Instance()->MusicVolume() < VOLUME_IF_INTRO_OR_CAMP_STAGE)
        {
            targetVolumeToUse = VOLUME_IF_INTRO_OR_CAMP_STAGE;
        }

        cmdQueue_.push(std::make_shared<sfml_util::LoopCmd_StartMusic>(
            sfml_util::music::Theme,
            sfml_util::MusicOperator::FADE_MULT_DEFAULT_IN_,
            targetVolumeToUse));

        cmdQueue_.push(std::make_shared<sfml_util::LoopCmd_SetHoldTime>(2.0f));
        cmdQueue_.push(std::make_shared<sfml_util::LoopCmd_Execute>());

        cmdQueue_.push(std::make_shared<sfml_util::LoopCmd_RemoveAllStages>());
        cmdQueue_.push(std::make_shared<stage::LoopCmd_AddStage<stage::IntroStage>>());

        cmdQueue_.push(std::make_shared<sfml_util::LoopCmd_ExitAfterFade>());
        cmdQueue_.push(std::make_shared<sfml_util::LoopCmd_FadeIn>(sf::Color::Black, 50.0f));
        cmdQueue_.push(std::make_shared<sfml_util::LoopCmd_Execute>());

        cmdQueue_.push(std::make_shared<sfml_util::LoopCmd_SetHoldTime>(4.0f));
        cmdQueue_.push(std::make_shared<sfml_util::LoopCmd_Execute>());

        cmdQueue_.push(std::make_shared<sfml_util::LoopCmd_ExitAfterKeypress>(true));
        cmdQueue_.push(std::make_shared<sfml_util::LoopCmd_ExitAfterMouseclick>(true));

        cmdQueue_.push(std::make_shared<sfml_util::LoopCmd_ExitAfterFade>());
        cmdQueue_.push(std::make_shared<sfml_util::LoopCmd_FadeOut>(sf::Color::Black, 150.0f));
        cmdQueue_.push(std::make_shared<sfml_util::LoopCmd_Execute>());

        TransitionTo_MainMenu();
    }

    void LoopManager::TransitionFrom_Popup()
    {
        loop_.Exit();
        cmdQueue_.push(std::make_shared<sfml_util::LoopCmd_StateChange>(prevState_));
        cmdQueue_.push(std::make_shared<sfml_util::LoopCmd_RemoveStage_Popup>());

        cmdQueue_.push(std::make_shared<sfml_util::LoopCmd_FadeIn>(
            popup::PopupManager::Color_Fade(), popup::PopupManager::SpeedMult_Fade()));

        cmdQueue_.push(std::make_shared<sfml_util::LoopCmd_Execute>());
    }

    void LoopManager::TransitionTo_Exit()
    {
        CommandQueueClear();
        loop_.Exit();

        cmdQueue_.push(std::make_shared<sfml_util::LoopCmd_IgnoreMouse>(true));

        cmdQueue_.push(std::make_shared<sfml_util::LoopCmd_ExitAfterKeypress>(false));
        cmdQueue_.push(std::make_shared<sfml_util::LoopCmd_ExitAfterMouseclick>(false));

        cmdQueue_.push(
            std::make_shared<sfml_util::LoopCmd_StateChange>(sfml_util::LoopState::Exit));
        cmdQueue_.push(
            std::make_shared<sfml_util::LoopCmd_StopMusic>(sfml_util::music::Enum::All, 100.0f));
        cmdQueue_.push(std::make_shared<sfml_util::LoopCmd_FadeOut>());
        cmdQueue_.push(std::make_shared<sfml_util::LoopCmd_Execute>());
        cmdQueue_.push(std::make_shared<sfml_util::LoopCmd_RemoveAllStages>());
    }

    void LoopManager::TransitionTo_Credits()
    {
        TransitionHelper(
            static_cast<TransOpt>(TransOpt::ClearQueue | TransOpt::MouseIgnore),
            sfml_util::LoopState::Credits,
            std::make_shared<stage::LoopCmd_AddStage<stage::CreditsStage>>(),
            sfml_util::music::All,
            sfml_util::music::Credits);

        cmdQueue_.push(std::make_shared<sfml_util::LoopCmd_IgnoreMouse>(false));
        cmdQueue_.push(std::make_shared<sfml_util::LoopCmd_ExitAfterMouseclick>(true));
        cmdQueue_.push(std::make_shared<sfml_util::LoopCmd_Execute>());
        TransitionTo_MainMenu();
    }

    void LoopManager::TransitionTo_MainMenu()
    {
        TransitionHelper(
            static_cast<TransOpt>(
                TransOpt::MouseIgnore | TransOpt::MouseRestore | TransOpt::FinalExecute),
            sfml_util::LoopState::MainMenu,
            std::make_shared<stage::LoopCmd_AddStage<stage::MainMenuStage>>(),
            sfml_util::music::Wind,
            sfml_util::music::Theme);
    }

    void LoopManager::TransitionTo_Settings()
    {
        prevSettingsState_ = state_;

        TransitionHelper(
            TransOpt::All,
            sfml_util::LoopState::Settings,
            std::make_shared<stage::LoopCmd_AddStage<stage::SettingsStage>>(),
            sfml_util::music::All,
            sfml_util::music::Theme);

        TransitionTo_MainMenu(); // um...why is this here?  zTn 2017-2-22
    }

    void LoopManager::TransitionTo_CharacterCreation()
    {
        TransitionHelper(
            TransOpt::All,
            sfml_util::LoopState::CharacterCreation,
            std::make_shared<stage::LoopCmd_AddStage<stage::CharacterStage>>(),
            sfml_util::music::All,
            sfml_util::music::Wind);
    }

    void LoopManager::TransitionTo_PartyCreation()
    {
        TransitionHelper(
            static_cast<TransOpt>(
                TransOpt::ClearQueue | TransOpt::MouseIgnore | TransOpt::MouseRestore),
            sfml_util::LoopState::PartyCreation,
            std::make_shared<stage::LoopCmd_AddStage<stage::PartyStage>>(),
            sfml_util::music::All,
            sfml_util::music::PartyCreation);

        cmdQueue_.push(
            std::make_shared<sfml_util::LoopCmd_FakeMouseClick>(sf::Vector2f(400.0f, 370.0f)));

        cmdQueue_.push(std::make_shared<sfml_util::LoopCmd_Execute>());
    }

    void LoopManager::TransitionTo_Inn()
    {
        TransitionHelper(
            TransOpt::All,
            sfml_util::LoopState::Inn,
            std::make_shared<stage::LoopCmd_AddStage<stage::InnStage>>());
    }

    void LoopManager::TransitionTo_Camp()
    {
        TransitionHelper(
            static_cast<TransOpt>(TransOpt::ClearQueue | TransOpt::MouseRestore),
            sfml_util::LoopState::Camp,
            std::make_shared<stage::LoopCmd_AddStage<stage::CampStage>>(),
            sfml_util::music::All,
            sfml_util::music::None);

        // establish the theme music volume
        const float VOLUME_IF_INTRO_OR_CAMP_STAGE(25.0f);

        // zero means 'use whatever music volume is set in the SoundManager'
        float targetVolumeToUse(0.0f);

        if (sfml_util::SoundManager::Instance()->MusicVolume() < VOLUME_IF_INTRO_OR_CAMP_STAGE)
            targetVolumeToUse = VOLUME_IF_INTRO_OR_CAMP_STAGE;

        cmdQueue_.push(std::make_shared<sfml_util::LoopCmd_StartMusic>(
            sfml_util::music::Theme,
            sfml_util::MusicOperator::FADE_MULT_DEFAULT_IN_,
            targetVolumeToUse));

        cmdQueue_.push(std::make_shared<sfml_util::LoopCmd_Execute>());
    }

    void LoopManager::TransitionTo_LoadGameMenu()
    {
        TransitionHelper(
            TransOpt::All,
            sfml_util::LoopState::LoadGameMenu,
            std::make_shared<stage::LoopCmd_AddStage<stage::LoadGameStage>>());
    }

    void LoopManager::TransitionTo_Combat(const bool WILL_ADVANCE_TURN)
    {
        TransitionHelper(
            TransOpt::All,
            sfml_util::LoopState::Combat,
            std::make_shared<stage::LoopCmd_AddStage_Combat>(WILL_ADVANCE_TURN),
            sfml_util::music::All,
            sfml_util::music::None);
    }

    void LoopManager::TransitionTo_Test()
    {
        TransitionHelper(
            TransOpt::All,
            sfml_util::LoopState::Test,
            std::make_shared<stage::LoopCmd_AddStage<stage::TestingStage>>(),
            sfml_util::music::All,
            sfml_util::music::None);
    }

    void LoopManager::TransitionTo_Treasure()
    {
        TransitionHelper(
            static_cast<TransOpt>(
                TransOpt::ClearQueue | TransOpt::FinalExecute | TransOpt::MouseRestore),
            sfml_util::LoopState::Treasure,
            std::make_shared<stage::LoopCmd_AddStage<stage::TreasureStage>>(),
            sfml_util::music::All,
            sfml_util::music::None);
    }

    void LoopManager::TransitionTo_Adventure()
    {
        TransitionHelper(
            TransOpt::All,
            sfml_util::LoopState::Adventure,
            std::make_shared<stage::LoopCmd_AddStage<stage::AdventureStage>>(),
            sfml_util::music::All,
            sfml_util::music::None);
    }

    void LoopManager::TransitionTo_Inventory(
        const creature::CreaturePtr_t TURN_CREATURE_PTR,
        const creature::CreaturePtr_t INVENTORY_CREATURE_PTR,
        const game::Phase::Enum CURRENT_PHASE)
    {
        prevSettingsState_ = state_;

        TransitionHelper(
            TransOpt::All,
            sfml_util::LoopState::Inventory,
            std::make_shared<stage::LoopCmd_AddStage_Inventory>(
                TURN_CREATURE_PTR, INVENTORY_CREATURE_PTR, CURRENT_PHASE),
            sfml_util::music::None,
            sfml_util::music::Inventory);
    }

    void LoopManager::TransitionHelper(
        const TransOpt OPTIONS,
        const sfml_util::LoopState::Enum NEW_STATE,
        const sfml_util::ILoopCmdSPtr_t & ADD_STAGE_LOOP_CMD,
        const sfml_util::music::Enum MUSIC_TO_STOP,
        const sfml_util::music::Enum MUSIC_TO_START)
    {
        if ((OPTIONS == TransOpt::All) || (OPTIONS & TransOpt::ClearQueue))
        {
            CommandQueueClear();
        }

        loop_.Exit();

        if ((OPTIONS == TransOpt::All) || (OPTIONS & TransOpt::MouseIgnore))
        {
            cmdQueue_.push(std::make_shared<sfml_util::LoopCmd_SetMouseVisibility>(false));
            cmdQueue_.push(std::make_shared<sfml_util::LoopCmd_IgnoreMouse>(true));
        }

        // cmdQueue_.push( std::make_shared<sfml_util::LoopCmd_IgnoreKeystrokes>(true) );

        cmdQueue_.push(std::make_shared<sfml_util::LoopCmd_StateChange>(NEW_STATE));

        cmdQueue_.push(std::make_shared<sfml_util::LoopCmd_ExitAfterKeypress>(false));
        cmdQueue_.push(std::make_shared<sfml_util::LoopCmd_ExitAfterMouseclick>(false));

        if (MUSIC_TO_STOP != sfml_util::music::None)
        {
            cmdQueue_.push(std::make_shared<sfml_util::LoopCmd_StopMusic>(MUSIC_TO_STOP));
        }

        if (MUSIC_TO_START != sfml_util::music::None)
        {
            cmdQueue_.push(std::make_shared<sfml_util::LoopCmd_StartMusic>(MUSIC_TO_START));
        }

        cmdQueue_.push(std::make_shared<sfml_util::LoopCmd_ExitAfterFade>());
        cmdQueue_.push(std::make_shared<sfml_util::LoopCmd_FadeOut>());
        cmdQueue_.push(std::make_shared<sfml_util::LoopCmd_Execute>());

        cmdQueue_.push(std::make_shared<sfml_util::LoopCmd_RemoveAllStages>());

        cmdQueue_.push(ADD_STAGE_LOOP_CMD);

        cmdQueue_.push(std::make_shared<sfml_util::LoopCmd_ExitAfterFade>());
        cmdQueue_.push(std::make_shared<sfml_util::LoopCmd_FadeIn>());
        cmdQueue_.push(std::make_shared<sfml_util::LoopCmd_Execute>());

        // cmdQueue_.push( std::make_shared<sfml_util::LoopCmd_IgnoreKeystrokes>(false) );

        if ((OPTIONS == TransOpt::All) || (OPTIONS & TransOpt::MouseRestore))
        {
            cmdQueue_.push(std::make_shared<sfml_util::LoopCmd_IgnoreMouse>(false));
            cmdQueue_.push(std::make_shared<sfml_util::LoopCmd_SetMouseVisibility>(true));
        }

        if ((OPTIONS == TransOpt::All) || (OPTIONS & TransOpt::FinalExecute))
        {
            cmdQueue_.push(std::make_shared<sfml_util::LoopCmd_Execute>());
        }
    }

    bool LoopManager::Execute()
    {
        while (false == cmdQueue_.empty())
        {
            auto cmdSPtr{ cmdQueue_.front() };
            cmdQueue_.pop();

            // M_HP_LOG("LoopManager::Execute() executing: " << cmdSPtr->GetName() );

            cmdSPtr->Execute();

            const sfml_util::LoopState::Enum CURRENT_STATE(loop_.GetState());
            if (CURRENT_STATE != state_)
            {
                prevState_ = state_;
                state_ = CURRENT_STATE;

                M_HP_LOG(
                    "LoopManager changed from \""
                    << sfml_util::LoopState::ToString(prevState_) << "\" to \""
                    << sfml_util::LoopState::ToString(state_) << "\"");
            }
        }

        return exitSuccess_;
    }

    void LoopManager::PopupWaitEnd(
        const popup::ResponseTypes::Enum RESPONSE, const std::size_t SELECTION)
    {
        popupResponse_ = RESPONSE;
        popupSelection_ = SELECTION;
        TransitionFrom_Popup();
    }

    void LoopManager::TransitionTo_Previous(const bool WILL_ADVANCE_TURN)
    {
        loop_.Exit();

        sfml_util::LoopState::Enum prevStateToUse(prevState_);
        if (prevState_ == sfml_util::LoopState::Popup)
        {
            prevStateToUse = prevSettingsState_;
        }

        TransitionTo(prevStateToUse, WILL_ADVANCE_TURN);
    }

    void LoopManager::HandleTransitionBeforeFade()
    {
        if (sfml_util::LoopState::None == stateBeforeFade_)
        {
            return;
        }

        TransitionTo(stateBeforeFade_);
        stateBeforeFade_ = sfml_util::LoopState::None;
    }

    void LoopManager::FakeMouseClick(const sf::Vector2f & MOUSE_POS_V)
    {
        loop_.FakeMouseClick(MOUSE_POS_V);
    }

    void LoopManager::TransitionTo(
        const sfml_util::LoopState::Enum STATE, const bool WILL_ADVANCE_TURN)
    {
        switch (STATE)
        {
            case sfml_util::LoopState::Settings:
            {
                TransitionTo_Settings();
                break;
            }
            case sfml_util::LoopState::MainMenu:
            {
                TransitionTo_MainMenu();
                break;
            }
            case sfml_util::LoopState::Intro:
            {
                TransitionTo_Intro();
                break;
            }
            case sfml_util::LoopState::Exit:
            {
                TransitionTo_Exit();
                break;
            }
            case sfml_util::LoopState::Credits:
            {
                TransitionTo_Credits();
                break;
            }
            case sfml_util::LoopState::PartyCreation:
            {
                TransitionTo_PartyCreation();
                break;
            }
            case sfml_util::LoopState::Camp:
            {
                TransitionTo_Camp();
                break;
            }
            case sfml_util::LoopState::LoadGameMenu:
            {
                TransitionTo_LoadGameMenu();
                break;
            }
            case sfml_util::LoopState::CharacterCreation:
            {
                TransitionTo_CharacterCreation();
                break;
            }
            case sfml_util::LoopState::Inn:
            {
                TransitionTo_Inn();
                break;
            }
            case sfml_util::LoopState::Combat:
            {
                TransitionTo_Combat(WILL_ADVANCE_TURN);
                break;
            }
            case sfml_util::LoopState::Test:
            {
                TransitionTo_Test();
                break;
            }
            case sfml_util::LoopState::Treasure:
            {
                TransitionTo_Treasure();
                break;
            }
            case sfml_util::LoopState::Inventory:
            case sfml_util::LoopState::Adventure:
            {
                TransitionTo_Adventure();
                break;
            }
            case sfml_util::LoopState::None:
            case sfml_util::LoopState::Popup:
            case sfml_util::LoopState::Query:
            case sfml_util::LoopState::Count:
            default:
            {
                M_HP_LOG(
                    "ERROR:  LoopManager::TransitionTo() called when STATE is "
                    << sfml_util::LoopState::ToString(STATE) << ".  Going to Main Menu...");

                TransitionTo_MainMenu();
                break;
            }
        }
    }

    sfml_util::DisplayChangeResult::Enum LoopManager::ChangeResolution(
        sfml_util::IStage * const currentStagePtr_,
        popup::IPopupHandler_t * const HANDLER_PTR,
        const sfml_util::Resolution & NEW_RES,
        const unsigned ANTIALIAS_LEVEL)
    {
        auto const CHANGE_RESULT{ sfml_util::Display::ChangeVideoMode(NEW_RES, ANTIALIAS_LEVEL) };

        if (currentStagePtr_ != nullptr)
        {
            currentStagePtr_->HandleResolutionChange();
        }

        auto isPopupTypeResolutionChange{ true };

        sfml_util::gui::TextInfo textInfo(
            "Keep this setting?",
            sfml_util::FontManager::Instance()->Font_Default1(),
            sfml_util::FontManager::Instance()->Size_Normal(),
            sf::Color::Black,
            sfml_util::Justified::Center);

        switch (CHANGE_RESULT)
        {
            case sfml_util::DisplayChangeResult::Success:
            {
                break;
            }

            case sfml_util::DisplayChangeResult::FailChange:
            {
                textInfo.justified = sfml_util::Justified::Left;

                std::ostringstream ss;
                ss << "Unable to switch to that resolution.  Switched to "
                   << sfml_util::VideoModeToString(sfml_util::Display::GetCurrentVideoMode())
                   << " AA=" << sfml_util::Display::Instance()->AntialiasLevel() << ".  Keep?";

                textInfo.text = ss.str();
                break;
            }

            case sfml_util::DisplayChangeResult::FailNoChange:
            case sfml_util::DisplayChangeResult::FailThenRevert:
            case sfml_util::DisplayChangeResult::Count:
            default:
            {
                isPopupTypeResolutionChange = false;
                textInfo.text = "Unable to set that video mode.";
                break;
            }
        }

        const popup::PopupInfo POPUP_INFO(
            "ResolutionChangePopup",
            textInfo,
            popup::PopupButtons::YesNo,
            popup::PopupImage::Regular,
            sfml_util::sound_effect::PromptQuestion);

        if (isPopupTypeResolutionChange)
        {
            PopupWaitBeginSpecific<popup::PopupStageResChange>(HANDLER_PTR, POPUP_INFO);
        }
        else
        {
            PopupWaitBegin(HANDLER_PTR, POPUP_INFO);
        }

        return CHANGE_RESULT;
    }

    void LoopManager::CommandQueueClear()
    {
        const std::size_t CMD_COUNT(cmdQueue_.size());
        for (std::size_t i(0); i < CMD_COUNT; ++i)
        {
            cmdQueue_.pop();
        }
    }
} // namespace game
} // namespace heroespath
