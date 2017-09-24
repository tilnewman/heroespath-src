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

#include "popup/popup-manager.hpp"
#include "popup/popup-stage-res-change.hpp"

#include "game/game-data-file.hpp"
#include "game/log-macros.hpp"
#include "game/stage/loop-cmd-addstage.hpp"
#include "game/creature/creature.hpp"
#include "popup/popup-info.hpp"
#include "game/stage/loop-cmd-addstage.hpp"

#include "misc/assertlogandthrow.hpp"

#include <string>


namespace game
{

    std::unique_ptr<LoopManager> LoopManager::instanceUPtr_{ nullptr };
    std::string LoopManager::startupStage_{ LoopState::ToString(LoopState::Intro) };


    LoopManager::LoopManager()
    :
        state_            (LoopState::None),
        cmdQueue_         (),
        loop_             ("DefaultLoop"),
        popupResponse_    (popup::Response::None),
        popupSelection_   (0),
        prevState_        (LoopState::None),
        prevSettingsState_(LoopState::None),
        stateBeforeFade_  (LoopState::None),
        exitSuccess_      (true)
    {
        M_HP_LOG_DBG("Singleton Construction: LoopManager");

        if (startupStage_.empty())
        {
            TransitionTo(LoopState::Intro);
        }
        else
        {
            TransitionTo(LoopState::FromString(startupStage_));
        }
    }


    LoopManager::~LoopManager()
    {
        M_HP_LOG_DBG("Singleton Destruction: LoopManager");
    }


    LoopManager * LoopManager::Instance()
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
            instanceUPtr_.reset(new LoopManager);
        }
        else
        {
            M_HP_LOG_WRN("Singleton Acquire() after Construction: LoopManager");
        }
    }


    void LoopManager::Release()
    {
        M_ASSERT_OR_LOGANDTHROW_SS((instanceUPtr_.get() != nullptr),
            "LoopManager::Release() found instanceUPtr that was null.");

        instanceUPtr_.reset();
    }


    Phase::Enum LoopManager::GetPhase() const
    {
        if (LoopState::Adventure == state_)
        {
            return Phase::Exploring;
        }
        else if (LoopState::Combat == state_)
        {
            return Phase::Combat;
        }
        else if (LoopState::Inventory == state_)
        {
            return Phase::Inventory;
        }
        else if (LoopState::Popup == state_)
        {
            if (LoopState::Adventure == prevState_)
            {
                return Phase::Exploring;
            }
            else if (LoopState::Combat == prevState_)
            {
                return Phase::Combat;
            }
            else if (LoopState::Inventory == prevState_)
            {
                return Phase::Inventory;
            }
        }

        return static_cast<Phase::Enum>(0);
    }


    void LoopManager::TransitionTo_Intro()
    {
        cmdQueue_.push( std::make_shared<sfml_util::LoopCmd_RemoveAllStages>() );

        cmdQueue_.push( std::make_shared<sfml_util::LoopCmd_StateChange>(LoopState::Intro) );

        cmdQueue_.push( std::make_shared<sfml_util::LoopCmd_ExitAfterKeypress>(true) );
        cmdQueue_.push( std::make_shared<sfml_util::LoopCmd_ExitAfterMouseclick>(true) );

        cmdQueue_.push( std::make_shared<sfml_util::LoopCmd_SetMouseVisibility>(false) );

        cmdQueue_.push( std::make_shared<sfml_util::LoopCmd_AddStage_Default>() );

        cmdQueue_.push( std::make_shared<sfml_util::LoopCmd_SetHoldTime>(2.5f) );
        cmdQueue_.push( std::make_shared<sfml_util::LoopCmd_Execute>() );

        //set the theme music volume to 25% even if the config file has it set lower so it can
        //always be heard during the intro
        const float VOLUME_IF_INTRO_OR_CAMP_STAGE(25.0f);
        float targetVolumeToUse(sfml_util::MusicOperator::VOLUME_USE_GLOBAL_);
        if (sfml_util::SoundManager::Instance()->MusicVolume() < VOLUME_IF_INTRO_OR_CAMP_STAGE)
            targetVolumeToUse = VOLUME_IF_INTRO_OR_CAMP_STAGE;

        cmdQueue_.push( std::make_shared<sfml_util::LoopCmd_StartMusic>(
            sfml_util::music::Theme,
            sfml_util::MusicOperator::FADE_MULT_DEFAULT_IN_,
            targetVolumeToUse) );

        cmdQueue_.push( std::make_shared<sfml_util::LoopCmd_SetHoldTime>(2.0f) );
        cmdQueue_.push( std::make_shared<sfml_util::LoopCmd_Execute>() );

        cmdQueue_.push( std::make_shared<sfml_util::LoopCmd_RemoveAllStages>() );
        cmdQueue_.push( std::make_shared< stage::LoopCmd_AddStage<stage::IntroStage> >() );

        cmdQueue_.push( std::make_shared<sfml_util::LoopCmd_ExitAfterFade>() );
        cmdQueue_.push( std::make_shared<sfml_util::LoopCmd_FadeIn>(sf::Color::Black, 50.0f) );
        cmdQueue_.push( std::make_shared<sfml_util::LoopCmd_Execute>() );

        cmdQueue_.push( std::make_shared<sfml_util::LoopCmd_SetHoldTime>(4.0f) );
        cmdQueue_.push( std::make_shared<sfml_util::LoopCmd_Execute>() );

        cmdQueue_.push( std::make_shared<sfml_util::LoopCmd_ExitAfterKeypress>(true) );
        cmdQueue_.push( std::make_shared<sfml_util::LoopCmd_ExitAfterMouseclick>(true) );

        cmdQueue_.push( std::make_shared<sfml_util::LoopCmd_ExitAfterFade>() );
        cmdQueue_.push( std::make_shared<sfml_util::LoopCmd_FadeOut>(sf::Color::Black, 150.0f) );
        cmdQueue_.push( std::make_shared<sfml_util::LoopCmd_Execute>() );

        TransitionTo_MainMenu();
    }


    void LoopManager::TransitionFrom_Popup()
    {
        loop_.Exit();
        cmdQueue_.push( std::make_shared<sfml_util::LoopCmd_StateChange>(prevState_) );
        cmdQueue_.push( std::make_shared<sfml_util::LoopCmd_RemoveStage_Popup>() );

        cmdQueue_.push( std::make_shared<sfml_util::LoopCmd_FadeIn>(
            popup::PopupManager::Color_Fade(),
            popup::PopupManager::SpeedMult_Fade()) );

        cmdQueue_.push( std::make_shared<sfml_util::LoopCmd_Execute>() );
    }


    void LoopManager::TransitionTo_Exit()
    {
        CommandQueueClear();
        loop_.Exit();

        cmdQueue_.push( std::make_shared<sfml_util::LoopCmd_IgnoreMouse>(true) );

        cmdQueue_.push( std::make_shared<sfml_util::LoopCmd_ExitAfterKeypress>(false) );
        cmdQueue_.push( std::make_shared<sfml_util::LoopCmd_ExitAfterMouseclick>(false) );

        cmdQueue_.push( std::make_shared<sfml_util::LoopCmd_StateChange>(LoopState::Exit) );
        cmdQueue_.push( std::make_shared<sfml_util::LoopCmd_StopMusic>(sfml_util::music::Enum::All, 100.0f) );
        cmdQueue_.push( std::make_shared<sfml_util::LoopCmd_FadeOut>() );
        cmdQueue_.push( std::make_shared<sfml_util::LoopCmd_Execute>() );
        cmdQueue_.push( std::make_shared<sfml_util::LoopCmd_RemoveAllStages>() );
    }


    void LoopManager::TransitionTo_Credits()
    {
        TransitionHelper(
            static_cast<TransOpt>(TransOpt::ClearQueue | TransOpt::MouseIgnore),
            LoopState::Credits,
            std::make_shared< stage::LoopCmd_AddStage<stage::CreditsStage> >(),
            sfml_util::music::All,
            sfml_util::music::Credits);

        cmdQueue_.push( std::make_shared<sfml_util::LoopCmd_IgnoreMouse>(false) );
        cmdQueue_.push( std::make_shared<sfml_util::LoopCmd_ExitAfterMouseclick>(true) );
        cmdQueue_.push( std::make_shared<sfml_util::LoopCmd_Execute>() );
        TransitionTo_MainMenu();
    }


    void LoopManager::TransitionTo_MainMenu()
    {
        TransitionHelper(
            static_cast<TransOpt>(
                TransOpt::MouseIgnore |
                TransOpt::MouseRestore |
                TransOpt::FinalExecute),
            LoopState::MainMenu,
            std::make_shared< stage::LoopCmd_AddStage<stage::MainMenuStage> >(),
            sfml_util::music::Wind,
            sfml_util::music::Theme);
    }


    void LoopManager::TransitionTo_Settings()
    {
        prevSettingsState_ = state_;

        TransitionHelper(
            TransOpt::All,
            LoopState::Settings,
            std::make_shared< stage::LoopCmd_AddStage<stage::SettingsStage> >(),
            sfml_util::music::All,
            sfml_util::music::Theme);

        TransitionTo_MainMenu();//um...why is this here?  zTn 2017-2-22
    }


    void LoopManager::TransitionTo_CharacterCreation()
    {
        TransitionHelper(
            TransOpt::All,
            LoopState::CharacterCreation,
            std::make_shared< stage::LoopCmd_AddStage<stage::CharacterStage> >(),
            sfml_util::music::All,
            sfml_util::music::Wind);
    }


    void LoopManager::TransitionTo_PartyCreation()
    {
        TransitionHelper(
            static_cast<TransOpt>(
                TransOpt::ClearQueue |
                TransOpt::MouseIgnore |
                TransOpt::MouseRestore),
            LoopState::PartyCreation,
            std::make_shared< stage::LoopCmd_AddStage<stage::PartyStage> >(),
            sfml_util::music::All,
            sfml_util::music::PartyCreation);

        cmdQueue_.push( std::make_shared<sfml_util::LoopCmd_FakeMouseClick>(
            sf::Vector2f(400.0f, 370.0f)) );

        cmdQueue_.push( std::make_shared<sfml_util::LoopCmd_Execute>() );
    }


    void LoopManager::TransitionTo_Inn()
    {
        TransitionHelper(
            TransOpt::All,
            LoopState::Inn,
            std::make_shared< stage::LoopCmd_AddStage<stage::InnStage> >() );
    }


    void LoopManager::TransitionTo_Camp()
    {
        TransitionHelper(
            static_cast<TransOpt>(TransOpt::ClearQueue | TransOpt::MouseRestore),
            LoopState::Camp,
            std::make_shared< stage::LoopCmd_AddStage<stage::CampStage> >(),
            sfml_util::music::All,
            sfml_util::music::None);

        //establish the theme music volume
        const float VOLUME_IF_INTRO_OR_CAMP_STAGE(25.0f);

        //zero means 'use whatever music volume is set in the SoundManager'
        float targetVolumeToUse(0.0f);

        if (sfml_util::SoundManager::Instance()->MusicVolume() < VOLUME_IF_INTRO_OR_CAMP_STAGE)
            targetVolumeToUse = VOLUME_IF_INTRO_OR_CAMP_STAGE;

        cmdQueue_.push( std::make_shared<sfml_util::LoopCmd_StartMusic>(
            sfml_util::music::Theme,
            sfml_util::MusicOperator::FADE_MULT_DEFAULT_IN_,
            targetVolumeToUse) );

        cmdQueue_.push( std::make_shared<sfml_util::LoopCmd_Execute>() );
    }


    void LoopManager::TransitionTo_LoadGameMenu()
    {
        TransitionHelper(
            TransOpt::All,
            LoopState::LoadGameMenu,
            std::make_shared< stage::LoopCmd_AddStage<stage::LoadGameStage> >() );
    }


    void LoopManager::TransitionTo_Combat(const bool WILL_ADVANCE_TURN)
    {
        TransitionHelper(
            TransOpt::All,
            LoopState::Combat,
            std::make_shared<stage::LoopCmd_AddStage_Combat>(WILL_ADVANCE_TURN),
            sfml_util::music::All,
            sfml_util::music::None);
    }


    void LoopManager::TransitionTo_Test()
    {
        TransitionHelper(
            TransOpt::All,
            LoopState::Test,
            std::make_shared< stage::LoopCmd_AddStage<stage::TestingStage> >(),
            sfml_util::music::All,
            sfml_util::music::None);
    }


    void LoopManager::TransitionTo_Treasure()
    {
        TransitionHelper(
            static_cast<TransOpt>(
                TransOpt::ClearQueue |
                TransOpt::FinalExecute |
                TransOpt::MouseRestore),
            LoopState::Treasure,
            std::make_shared < stage::LoopCmd_AddStage< stage::TreasureStage> >(),
            sfml_util::music::All,
            sfml_util::music::None);
    }


    void LoopManager::TransitionTo_Adventure()
    {
        TransitionHelper(
            TransOpt::All,
            LoopState::Adventure,
            std::make_shared< stage::LoopCmd_AddStage<stage::AdventureStage> >(),
            sfml_util::music::All,
            sfml_util::music::None);
    }


    void LoopManager::TransitionTo_Inventory(const creature::CreaturePtr_t TURN_CREATURE_PTR,
                                             const creature::CreaturePtr_t INVENTORY_CREATURE_PTR,
                                             const Phase::Enum             CURRENT_PHASE)
    {
        prevSettingsState_ = state_;

        TransitionHelper(
            TransOpt::All,
            LoopState::Inventory,
            std::make_shared<stage::LoopCmd_AddStage_Inventory>(
                TURN_CREATURE_PTR,
                INVENTORY_CREATURE_PTR,
                CURRENT_PHASE),
            sfml_util::music::None,
            sfml_util::music::Inventory);
    }


    void LoopManager::TransitionHelper(
        const TransOpt                    OPTIONS,
        const LoopState::Enum             NEW_STATE,
        const sfml_util::ILoopCmdSPtr_t & ADD_STAGE_LOOP_CMD,
        const sfml_util::music::Enum      MUSIC_TO_STOP,
        const sfml_util::music::Enum      MUSIC_TO_START)
    {
        if ((OPTIONS == TransOpt::All) || (OPTIONS & TransOpt::ClearQueue))
        {
            CommandQueueClear();
        }

        loop_.Exit();

        if ((OPTIONS == TransOpt::All) || (OPTIONS & TransOpt::MouseIgnore))
        {
            cmdQueue_.push( std::make_shared<sfml_util::LoopCmd_SetMouseVisibility>(false) );
            cmdQueue_.push( std::make_shared<sfml_util::LoopCmd_IgnoreMouse>(true) );
        }

        //cmdQueue_.push( std::make_shared<sfml_util::LoopCmd_IgnoreKeystrokes>(true) );

        cmdQueue_.push( std::make_shared<sfml_util::LoopCmd_StateChange>(NEW_STATE) );

        cmdQueue_.push( std::make_shared<sfml_util::LoopCmd_ExitAfterKeypress>(false) );
        cmdQueue_.push( std::make_shared<sfml_util::LoopCmd_ExitAfterMouseclick>(false) );

        if (MUSIC_TO_STOP != sfml_util::music::None)
        {
            cmdQueue_.push(std::make_shared<sfml_util::LoopCmd_StopMusic>(MUSIC_TO_STOP));
        }

        if (MUSIC_TO_START != sfml_util::music::None)
        {
            cmdQueue_.push(std::make_shared<sfml_util::LoopCmd_StartMusic>(MUSIC_TO_START));
        }

        cmdQueue_.push( std::make_shared<sfml_util::LoopCmd_ExitAfterFade>() );
        cmdQueue_.push( std::make_shared<sfml_util::LoopCmd_FadeOut>() );
        cmdQueue_.push( std::make_shared<sfml_util::LoopCmd_Execute>() );

        cmdQueue_.push( std::make_shared<sfml_util::LoopCmd_RemoveAllStages>() );

        cmdQueue_.push(ADD_STAGE_LOOP_CMD);

        cmdQueue_.push( std::make_shared<sfml_util::LoopCmd_ExitAfterFade>() );
        cmdQueue_.push( std::make_shared<sfml_util::LoopCmd_FadeIn>() );
        cmdQueue_.push( std::make_shared<sfml_util::LoopCmd_Execute>() );

        //cmdQueue_.push( std::make_shared<sfml_util::LoopCmd_IgnoreKeystrokes>(false) );

        if ((OPTIONS == TransOpt::All) || (OPTIONS & TransOpt::MouseRestore))
        {
            cmdQueue_.push( std::make_shared<sfml_util::LoopCmd_IgnoreMouse>(false) );
            cmdQueue_.push( std::make_shared<sfml_util::LoopCmd_SetMouseVisibility>(true) );
        }

        if ((OPTIONS == TransOpt::All) || (OPTIONS & TransOpt::FinalExecute))
        {
            cmdQueue_.push( std::make_shared<sfml_util::LoopCmd_Execute>() );
        }
    }


    bool LoopManager::Execute()
    {
        while (false == cmdQueue_.empty())
        {
            sfml_util::ILoopCmdSPtr_t cmdSPtr{ cmdQueue_.front() };
            cmdQueue_.pop();

            cmdSPtr->Execute();

            const LoopState::Enum CURRENT_STATE(loop_.GetState() );
            if (CURRENT_STATE != state_)
            {
                prevState_ = state_;
                state_ = CURRENT_STATE;

                M_HP_LOG("LoopManager changed from \"" << LoopState::ToString(prevState_)
                    << "\" to \"" << LoopState::ToString(state_) << "\"");
            }
        }

        return exitSuccess_;
    }


    void LoopManager::PopupWaitEnd(
        const popup::Response::Enum RESPONSE, const std::size_t SELECTION)
    {
        popupResponse_ = RESPONSE;
        popupSelection_ = SELECTION;
        TransitionFrom_Popup();
    }


    void LoopManager::TransitionTo_Previous(const bool WILL_ADVANCE_TURN)
    {
        loop_.Exit();

        LoopState::Enum prevStateToUse(prevState_);
        if (prevState_ == LoopState::Popup)
        {
            prevStateToUse = prevSettingsState_;
        }

        TransitionTo(prevStateToUse, WILL_ADVANCE_TURN);
    }


    void LoopManager::HandleTransitionBeforeFade()
    {
        if (LoopState::None == stateBeforeFade_)
        {
            return;
        }

        TransitionTo(stateBeforeFade_);
        stateBeforeFade_ = LoopState::None;
    }


    void LoopManager::FakeMouseClick(const sf::Vector2f & MOUSE_POS_V)
    {
        loop_.FakeMouseClick(MOUSE_POS_V);
    }


    void LoopManager::TransitionTo(
        const LoopState::Enum STATE, const bool WILL_ADVANCE_TURN)
    {
        switch (STATE)
        {
            case LoopState::Settings:           { TransitionTo_Settings(); break; }
            case LoopState::MainMenu:           { TransitionTo_MainMenu(); break; }
            case LoopState::Intro:              { TransitionTo_Intro(); break; }
            case LoopState::Exit:               { TransitionTo_Exit(); break; }
            case LoopState::Credits:            { TransitionTo_Credits(); break; }
            case LoopState::PartyCreation:      { TransitionTo_PartyCreation(); break; }
            case LoopState::Camp:               { TransitionTo_Camp(); break; }
            case LoopState::LoadGameMenu:       { TransitionTo_LoadGameMenu(); break; }
            case LoopState::CharacterCreation:  { TransitionTo_CharacterCreation(); break; }
            case LoopState::Inn:                { TransitionTo_Inn(); break; }
            case LoopState::Combat:             { TransitionTo_Combat(WILL_ADVANCE_TURN); break; }
            case LoopState::Test:               { TransitionTo_Test(); break; }
            case LoopState::Treasure:           { TransitionTo_Treasure(); break; }
            case LoopState::Inventory:
            case LoopState::Adventure:          { TransitionTo_Adventure(); break; }
            case LoopState::None:
            case LoopState::Popup:
            case LoopState::Query:
            case LoopState::Count:
            default:
            {
                M_HP_LOG("ERROR:  LoopManager::TransitionTo() called when STATE is "
                    << LoopState::ToString(STATE) << ".  Going to Main Menu...");

                TransitionTo_MainMenu();
                break;
            }
        }
    }


    sfml_util::DisplayChangeResult::Enum LoopManager::ChangeResolution(
        sfml_util::IStage * const               currentStagePtr_,
        popup::IPopupHandler_t * const          HANDLER_PTR,
        const sfml_util::Resolution &           NEW_RES,
        const unsigned                          ANTIALIAS_LEVEL)
    {
        auto const CHANGE_RESULT{
            sfml_util::Display::ChangeVideoMode(NEW_RES, ANTIALIAS_LEVEL) };

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
                    << sfml_util::VideoModeToString( sfml_util::Display::GetCurrentVideoMode() )
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
            popup::PopupImage::Banner,
            sfml_util::MapByRes(1.0f, 3.0f),
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

}
