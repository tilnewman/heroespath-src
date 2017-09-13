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
#include "sfml-util/gui/popup-manager.hpp"

#include "game/game-data-file.hpp"
#include "game/log-macros.hpp"
#include "game/stage/loop-cmd-addstage.hpp"
#include "game/creature/creature.hpp"
#include "game/popup-info.hpp"

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
        currentLoopSPtr_  ( new sfml_util::Loop("LoopManagerDefault") ),
        popupResponse_    (sfml_util::Response::None),
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
            "game::LoopManager::Release() found instanceUPtr that was null.");

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
        cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_RemoveAllStages>(currentLoopSPtr_) );

        cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_StateChange>(currentLoopSPtr_, LoopState::Intro) );

        cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_ExitAfterKeypress>(currentLoopSPtr_, true) );
        cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_ExitAfterMouseclick>(currentLoopSPtr_, true) );

        cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_SetMouseVisibility>(currentLoopSPtr_, false) );

        cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_AddStage_Default>(currentLoopSPtr_) );

        cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_SetHoldTime>(currentLoopSPtr_, 2.5f) );
        cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_Execute>(currentLoopSPtr_) );

        //set the theme music volume to 25% even if the config file has it set lower so it can
        //always be heard during the intro
        const float VOLUME_IF_INTRO_OR_CAMP_STAGE(25.0f);
        float targetVolumeToUse(sfml_util::MusicOperator::VOLUME_USE_GLOBAL_);
        if (sfml_util::SoundManager::Instance()->MusicVolume() < VOLUME_IF_INTRO_OR_CAMP_STAGE)
            targetVolumeToUse = VOLUME_IF_INTRO_OR_CAMP_STAGE;

        cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_StartMusic>(currentLoopSPtr_,
                                                                                    sfml_util::music::Theme,
                                                                                    sfml_util::MusicOperator::FADE_MULT_DEFAULT_IN_,
                                                                                    targetVolumeToUse) );

        cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_SetHoldTime>(currentLoopSPtr_, 2.0f) );
        cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_Execute>(currentLoopSPtr_) );

        cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_RemoveAllStages>(currentLoopSPtr_) );
        cmdQueue_.Push( std::make_shared<stage::LoopCmd_AddStage_Intro>(currentLoopSPtr_) );

        cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_ExitAfterFade>(currentLoopSPtr_) );
        cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_FadeIn>(currentLoopSPtr_, sf::Color::Black, 50.0f) );
        cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_Execute>(currentLoopSPtr_) );

        cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_SetHoldTime>(currentLoopSPtr_, 4.0f) );
        cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_Execute>(currentLoopSPtr_) );

        cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_ExitAfterKeypress>(currentLoopSPtr_, true) );
        cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_ExitAfterMouseclick>(currentLoopSPtr_, true) );

        cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_ExitAfterFade>(currentLoopSPtr_) );
        cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_FadeOut>(currentLoopSPtr_, sf::Color::Black, 150.0f) );
        cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_Execute>(currentLoopSPtr_) );

        TransitionTo_MainMenu();
    }

    void LoopManager::TransitionTo_Popup(callback::IPopupHandler_t * const HANDLER_PTR,
                                         const PopupInfo &                 POPUP_INFO)
    {
        cmdQueue_.Clear();
        currentLoopSPtr_->Exit();

        currentLoopSPtr_->AssignPopupCallbackHandlerInfo(HANDLER_PTR, POPUP_INFO);
        cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_ExitAfterKeypress>(currentLoopSPtr_, false) );
        cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_ExitAfterMouseclick>(currentLoopSPtr_, false) );

        cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_StateChange>(currentLoopSPtr_, LoopState::Popup) );
        cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_FadeOut>(currentLoopSPtr_, sfml_util::gui::PopupManager::Color_Fade(), sfml_util::gui::PopupManager::SpeedMult_Fade(), true) );
        cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_Execute>(currentLoopSPtr_) );
        cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_AddStage_Popup>(currentLoopSPtr_, POPUP_INFO) );
        cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_Execute>(currentLoopSPtr_) );
    }


    void LoopManager::TransitionFrom_Popup()
    {
        currentLoopSPtr_->Exit();
        cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_StateChange>(currentLoopSPtr_, prevState_) );
        cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_RemoveStage_Popup>(currentLoopSPtr_) );

        cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_FadeIn>(
            currentLoopSPtr_,
            sfml_util::gui::PopupManager::Color_Fade(),
            sfml_util::gui::PopupManager::SpeedMult_Fade()) );
        cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_Execute>(currentLoopSPtr_) );
    }


    void LoopManager::TransitionTo_Exit()
    {
        cmdQueue_.Clear();
        currentLoopSPtr_->Exit();

        cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_IgnoreMouse>(currentLoopSPtr_, true) );

        cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_ExitAfterKeypress>(currentLoopSPtr_, false) );
        cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_ExitAfterMouseclick>(currentLoopSPtr_, false) );

        cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_StateChange>(currentLoopSPtr_, LoopState::Exit) );
        cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_StopMusic>(currentLoopSPtr_, sfml_util::music::Enum::All, 100.0f) );
        cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_FadeOut>(currentLoopSPtr_) );
        cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_Execute>(currentLoopSPtr_) );
        cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_RemoveAllStages>(currentLoopSPtr_) );
    }


    void LoopManager::TransitionTo_Credits()
    {
        TransitionHelper(true, //WILL_CLEAR_QUEUE
                         true, //WILL_EXIT_LOOP
                         true, //WILL_IGNORE_MOUSE
                         false,//WILL_RESTORE_MOUSE
                         false,//WILL_FINAL_EXECUTE
                         LoopState::Credits,
                         std::make_shared<game::stage::LoopCmd_AddStage_Credits>(currentLoopSPtr_),
                         sfml_util::music::All);

        cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_IgnoreMouse>(currentLoopSPtr_, false) );
        cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_ExitAfterMouseclick>(currentLoopSPtr_, true) );
        cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_Execute>(currentLoopSPtr_) );
        TransitionTo_MainMenu();
    }


    void LoopManager::TransitionTo_MainMenu()
    {
        TransitionHelper(false,//WILL_CLEAR_QUEUE
                         true, //WILL_EXIT_LOOP
                         true, //WILL_IGNORE_MOUSE
                         true, //WILL_RESTORE_MOUSE
                         true, //WILL_FINAL_EXECUTE
                         LoopState::MainMenu,
                         std::make_shared<game::stage::LoopCmd_AddStage_MainMenu>(currentLoopSPtr_),
                         sfml_util::music::Wind,
                         sfml_util::music::Theme);
    }


    void LoopManager::TransitionTo_Settings()
    {
        prevSettingsState_ = state_;

        TransitionHelper(true, //WILL_CLEAR_QUEUE
                         true, //WILL_EXIT_LOOP
                         true, //WILL_IGNORE_MOUSE
                         true, //WILL_RESTORE_MOUSE
                         true, //WILL_FINAL_EXECUTE
                         LoopState::Settings,
                         std::make_shared<game::stage::LoopCmd_AddStage_Settings>(currentLoopSPtr_),
                         sfml_util::music::All,
                         sfml_util::music::Theme);

        TransitionTo_MainMenu();//um...why is this here?  zTn 2017-2-22
    }


    void LoopManager::TransitionTo_CharacterCreation()
    {
        TransitionHelper(true, //WILL_CLEAR_QUEUE
                         true, //WILL_EXIT_LOOP
                         true, //WILL_IGNORE_MOUSE
                         true, //WILL_RESTORE_MOUSE
                         true,//WILL_FINAL_EXECUTE
                         LoopState::CharacterCreation,
                         std::make_shared<game::stage::LoopCmd_AddStage_Character_Creation>(currentLoopSPtr_),
                         sfml_util::music::All,
                         sfml_util::music::Wind);
    }


    void LoopManager::TransitionTo_PartyCreation()
    {
        TransitionHelper(true, //WILL_CLEAR_QUEUE
                         true, //WILL_EXIT_LOOP
                         true, //WILL_IGNORE_MOUSE
                         true, //WILL_RESTORE_MOUSE
                         false,//WILL_FINAL_EXECUTE
                         LoopState::PartyCreation,
                         std::make_shared<game::stage::LoopCmd_AddStage_Party_Creation>(currentLoopSPtr_),
                         sfml_util::music::All,
                         sfml_util::music::PartyCreation);

        cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_FakeMouseClick>(
            currentLoopSPtr_,
            sf::Vector2f(400.0f, 370.0f)) );

        cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_Execute>(currentLoopSPtr_) );
    }


    void LoopManager::TransitionTo_Inn()
    {
        TransitionHelper(true, //WILL_CLEAR_QUEUE
                         true, //WILL_EXIT_LOOP
                         true, //WILL_IGNORE_MOUSE
                         true, //WILL_RESTORE_MOUSE
                         true, //WILL_FINAL_EXECUTE
                         LoopState::Inn,
                         std::make_shared<game::stage::LoopCmd_AddStage_Inn>(currentLoopSPtr_) );
    }


    void LoopManager::TransitionTo_Camp()
    {
        TransitionHelper(true, //WILL_CLEAR_QUEUE
                         true, //WILL_EXIT_LOOP
                         false,//WILL_IGNORE_MOUSE
                         true, //WILL_RESTORE_MOUSE
                         false,//WILL_FINAL_EXECUTE
                         LoopState::Camp,
                         std::make_shared<game::stage::LoopCmd_AddStage_Camp>(currentLoopSPtr_),
                         sfml_util::music::All,
                         sfml_util::music::None);

        //establish the theme music volume
        const float VOLUME_IF_INTRO_OR_CAMP_STAGE(25.0f);

        //zero means 'use whatever music volume is set in the SoundManager'
        float targetVolumeToUse(0.0f);

        if (sfml_util::SoundManager::Instance()->MusicVolume() < VOLUME_IF_INTRO_OR_CAMP_STAGE)
            targetVolumeToUse = VOLUME_IF_INTRO_OR_CAMP_STAGE;

        cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_StartMusic>(
            currentLoopSPtr_,
            sfml_util::music::Theme,
            sfml_util::MusicOperator::FADE_MULT_DEFAULT_IN_,
            targetVolumeToUse) );

        cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_Execute>(currentLoopSPtr_) );
    }


    void LoopManager::TransitionTo_LoadGameMenu()
    {
        TransitionHelper(true, //WILL_CLEAR_QUEUE
                         true, //WILL_EXIT_LOOP
                         true, //WILL_IGNORE_MOUSE
                         true, //WILL_RESTORE_MOUSE
                         true, //WILL_FINAL_EXECUTE
                         LoopState::LoadGameMenu,
                         std::make_shared<game::stage::LoopCmd_AddStage_LoadGameMenu>(
                             currentLoopSPtr_) );
    }


    void LoopManager::TransitionTo_Combat(const bool WILL_ADVANCE_TURN)
    {
        TransitionHelper(true, //WILL_CLEAR_QUEUE
                         true, //WILL_EXIT_LOOP
                         true, //WILL_IGNORE_MOUSE
                         true, //WILL_RESTORE_MOUSE
                         true, //WILL_FINAL_EXECUTE
                         LoopState::Combat,
                         std::make_shared<game::stage::LoopCmd_AddStage_Combat>(currentLoopSPtr_,
                                                                                WILL_ADVANCE_TURN),
                         sfml_util::music::All,
                         sfml_util::music::None);
    }


    void LoopManager::TransitionTo_Test()
    {
        TransitionHelper(true, //WILL_CLEAR_QUEUE
                         true, //WILL_EXIT_LOOP
                         true, //WILL_IGNORE_MOUSE
                         true, //WILL_RESTORE_MOUSE
                         true, //WILL_FINAL_EXECUTE
                         LoopState::Test,
                         std::make_shared<game::stage::LoopCmd_AddStage_Test>(currentLoopSPtr_),
                         sfml_util::music::All,
                         sfml_util::music::None);
    }


    void LoopManager::TransitionTo_Treasure()
    {
        TransitionHelper(true, //WILL_CLEAR_QUEUE
                         true, //WILL_EXIT_LOOP
                         true, //WILL_IGNORE_MOUSE
                         true, //WILL_RESTORE_MOUSE
                         true, //WILL_FINAL_EXECUTE
                         LoopState::Treasure,
                         std::make_shared<game::stage::LoopCmd_AddStage_Treasure>(currentLoopSPtr_),
                         sfml_util::music::All,
                         sfml_util::music::None);
    }


    void LoopManager::TransitionTo_Inventory(const creature::CreaturePtr_t TURN_CREATURE_PTR,
                                             const creature::CreaturePtr_t INVENTORY_CREATURE_PTR,
                                             const Phase::Enum             CURRENT_PHASE)
    {
        prevSettingsState_ = state_;

        TransitionHelper(true, //WILL_CLEAR_QUEUE
                         true, //WILL_EXIT_LOOP
                         true, //WILL_IGNORE_MOUSE
                         true, //WILL_RESTORE_MOUSE
                         true, //WILL_FINAL_EXECUTE
                         LoopState::Inventory,
                         std::make_shared<game::stage::LoopCmd_AddStage_Inventory>(
                             currentLoopSPtr_,
                             TURN_CREATURE_PTR,
                             INVENTORY_CREATURE_PTR,
                             CURRENT_PHASE),
                         sfml_util::music::None,
                         sfml_util::music::Inventory);
    }


    void LoopManager::TransitionHelper(const bool                        WILL_CLEAR_QUEUE,
                                       const bool                        WILL_EXIT_LOOP,
                                       const bool                        WILL_IGNORE_MOUSE,
                                       const bool                        WILL_RESTORE_MOUSE,
                                       const bool                        WILL_FINAL_EXECUTE,
                                       const LoopState::Enum             NEW_STATE,
                                       const sfml_util::ILoopCmdSPtr_t & ADD_STAGE_LOOP_CMD,
                                       const sfml_util::music::Enum      MUSIC_TO_STOP,
                                       const sfml_util::music::Enum      MUSIC_TO_START)
    {
        if (WILL_CLEAR_QUEUE)
        {
            cmdQueue_.Clear();
        }

        if (WILL_EXIT_LOOP)
        {
            currentLoopSPtr_->Exit();
        }

        if (WILL_IGNORE_MOUSE)
        {
            cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_SetMouseVisibility>(currentLoopSPtr_, false) );
            cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_IgnoreMouse>(currentLoopSPtr_, true) );
        }

        cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_IgnoreKeystrokes>(currentLoopSPtr_, true) );

        cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_StateChange>(currentLoopSPtr_, NEW_STATE) );

        cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_ExitAfterKeypress>(currentLoopSPtr_, false) );
        cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_ExitAfterMouseclick>(currentLoopSPtr_, false) );

        if (MUSIC_TO_STOP != sfml_util::music::None)
        {
            cmdQueue_.Push(std::make_shared<sfml_util::LoopCmd_StopMusic>(currentLoopSPtr_, MUSIC_TO_STOP));
        }

        if (MUSIC_TO_START != sfml_util::music::None)
        {
            cmdQueue_.Push(std::make_shared<sfml_util::LoopCmd_StartMusic>(currentLoopSPtr_, MUSIC_TO_START));
        }

        cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_ExitAfterFade>(currentLoopSPtr_) );
        cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_FadeOut>(currentLoopSPtr_) );
        cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_Execute>(currentLoopSPtr_) );

        cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_RemoveAllStages>(currentLoopSPtr_) );

        cmdQueue_.Push(ADD_STAGE_LOOP_CMD);

        cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_ExitAfterFade>(currentLoopSPtr_) );
        cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_FadeIn>(currentLoopSPtr_) );
        cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_Execute>(currentLoopSPtr_) );

        cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_IgnoreKeystrokes>(currentLoopSPtr_, false) );

        if (WILL_RESTORE_MOUSE)
        {
            cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_IgnoreMouse>(currentLoopSPtr_, false) );
            cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_SetMouseVisibility>(currentLoopSPtr_, true) );
        }

        if (WILL_FINAL_EXECUTE)
        {
            cmdQueue_.Push( std::make_shared<sfml_util::LoopCmd_Execute>(currentLoopSPtr_) );
        }
    }


    bool LoopManager::Execute()
    {
        while (false == cmdQueue_.IsEmpty())
        {
            sfml_util::ILoopCmdSPtr_t cmdSPtr(cmdQueue_.Pop());

            //M_HP_LOG("LoopManager::Executing \"" << cmdSPtr->GetName() << "\", while in state \""
            // << LoopState::ToString(state_) << "\"");

            cmdSPtr->Execute();

            const LoopState::Enum CURRENT_STATE( currentLoopSPtr_->GetState() );
            if (CURRENT_STATE != state_)
            {
                prevState_ = state_;
                state_ = CURRENT_STATE;
                M_HP_LOG("LoopManager changed from \"" << LoopState::ToString(prevState_) << "\" to \"" << LoopState::ToString(state_) << "\"");
            }
        }

        return exitSuccess_;
    }


    void LoopManager::PopupWaitBegin(callback::IPopupHandler_t * const HANDLER_PTR,
                                     const PopupInfo &                 POPUP_INFO)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((HANDLER_PTR != nullptr), "LoopManager::PopupWaitBegin(" << POPUP_INFO.ToStringShort(false) << ") given a NULL POPUP_INFO->CallbackHandlerPtr() pointer.");
        M_HP_LOG("LoopManager::PopupWaitBegin(handler=\"" << HANDLER_PTR->HandlerName() << "\", " << POPUP_INFO.ToStringShort(false) << ") while in state=" << LoopState::ToString(prevState_) << ".");
        popupResponse_ = sfml_util::Response::None;
        popupSelection_ = 0;
        TransitionTo_Popup(HANDLER_PTR, POPUP_INFO);
    }


    void LoopManager::PopupWaitEnd(const sfml_util::Response::Enum RESPONSE, const std::size_t SELECTION)
    {
        popupResponse_ = RESPONSE;
        popupSelection_ = SELECTION;

        //M_HP_LOG("LoopManager::PopupWaitEnd(response=\"" << sfml_util::Response::ToString(RESPONSE)
        // << "\", selection=" << SELECTION << ", and will restore state to \""
        // << LoopState::ToString(prevState_) << "\")");

        TransitionFrom_Popup();
    }


    void LoopManager::Goto_Intro()
    {
        TransitionTo_Intro();
    }

    void LoopManager::Goto_Credits()
    {
        TransitionTo_Credits();
    }

    void LoopManager::Goto_Exit()
    {
        TransitionTo_Exit();
    }

    void LoopManager::Goto_MainMenu()
    {
        TransitionTo_MainMenu();
    }

    void LoopManager::Goto_Settings()
    {
        TransitionTo_Settings();
    }

    void LoopManager::Goto_CharacterCreation()
    {
        TransitionTo_CharacterCreation();
    }

    void LoopManager::Goto_PartyCreation()
    {
        TransitionTo_PartyCreation();
    }

    void LoopManager::Goto_Inn()
    {
        TransitionTo_Inn();
    }

    void LoopManager::Goto_Camp()
    {
        TransitionTo_Camp();
    }

    void LoopManager::Goto_LoadGameMenu()
    {
        TransitionTo_LoadGameMenu();
    }

    void LoopManager::Goto_Combat(const bool WILL_ADVANCE_TURN)
    {
        TransitionTo_Combat(WILL_ADVANCE_TURN);
    }

    void LoopManager::Goto_Test()
    {
        TransitionTo_Test();
    }

    void LoopManager::Goto_Treasure()
    {
        TransitionTo_Treasure();
    }

    void LoopManager::Goto_Inventory(const creature::CreaturePtr_t TURN_CREATURE_PTR,
                                     const creature::CreaturePtr_t INVENTORY_CREATURE_PTR,
                                     const Phase::Enum             CURRENT_PHASE)
    {
        TransitionTo_Inventory(TURN_CREATURE_PTR,
                               INVENTORY_CREATURE_PTR,
                               CURRENT_PHASE);
    }

    void LoopManager::Goto_Previous(const bool WILL_ADVANCE_TURN)
    {
        currentLoopSPtr_->Exit();

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
            return;

        TransitionTo(stateBeforeFade_);
        stateBeforeFade_ = LoopState::None;
    }


    void LoopManager::FakeMouseClick(const sf::Vector2f & MOUSE_POS_V)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((currentLoopSPtr_.get() != nullptr),
            "LoopManager::FakeMouseClick(" << sfml_util::VectorToString(MOUSE_POS_V, false)
            << ") was called when currentLoopSPtr_ is null.");

        currentLoopSPtr_->FakeMouseClick(MOUSE_POS_V);
    }


    void LoopManager::TransitionTo(const LoopState::Enum STATE, const bool WILL_ADVANCE_TURN)
    {
        switch (STATE)
        {
            case LoopState::Settings:           { Goto_Settings(); break; }
            case LoopState::MainMenu:           { Goto_MainMenu(); break; }
            case LoopState::Intro:              { Goto_Intro(); break; }
            case LoopState::Exit:               { Goto_Exit(); break; }
            case LoopState::Credits:            { Goto_Credits(); break; }
            case LoopState::PartyCreation:      { Goto_PartyCreation(); break; }
            case LoopState::Camp:               { Goto_Camp(); break; }
            case LoopState::LoadGameMenu:       { Goto_LoadGameMenu(); break; }
            case LoopState::CharacterCreation:  { Goto_CharacterCreation(); break; }
            case LoopState::Inn:                { Goto_Inn(); break; }
            case LoopState::Combat:             { Goto_Combat(WILL_ADVANCE_TURN); break; }
            case LoopState::Test:               { Goto_Test(); break; }
            case LoopState::Treasure:           { Goto_Treasure(); break; }
            case LoopState::Inventory:
            case LoopState::Adventure:
            case LoopState::None:
            case LoopState::Popup:
            case LoopState::Query:
            case LoopState::Count:
            default:
            {
                M_HP_LOG("ERROR:  LoopManager::TransitionTo() called when STATE is "
                    << LoopState::ToString(STATE) << ".  Going to Main Menu...");

                Goto_MainMenu();
                break;
            }
        }
    }


    sfml_util::DisplayChangeResult::Enum LoopManager::ChangeResolution(
        sfml_util::IStage * const               currentStagePtr_,
        game::callback::IPopupHandler_t * const HANDLER_PTR,
        const sfml_util::Resolution &           NEW_RES,
        const unsigned                          ANTIALIAS_LEVEL)
    {
        const sfml_util::DisplayChangeResult::Enum RESULT(sfml_util::Display::ChangeVideoMode(
            NEW_RES, ANTIALIAS_LEVEL));

        if (currentStagePtr_ != nullptr)
        {
            currentStagePtr_->HandleResolutionChange();
        }

        game::Popup::Enum whichPopup(game::Popup::ResolutionChange);

        sfml_util::gui::TextInfo textInfo("Keep this setting?",
                                          sfml_util::FontManager::Instance()->Font_Default1(),
                                          sfml_util::FontManager::Instance()->Size_Normal(),
                                          sf::Color::Black,
                                          sfml_util::Justified::Center);

        switch (RESULT)
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
                whichPopup = game::Popup::Generic;
                textInfo.text = "Unable to set that video mode.";
                break;
            }
        }

        const game::PopupInfo POPUP_INFO("ResolutionChange",
                                               textInfo,
                                               sfml_util::PopupButtons::YesNo,
                                               sfml_util::PopupImage::Banner,
                                               sfml_util::MapByRes(1.0f, 3.0f),
                                               whichPopup,
                                               sfml_util::sound_effect::PromptQuestion);

        PopupWaitBegin(HANDLER_PTR, POPUP_INFO);

        return RESULT;
    }

}
