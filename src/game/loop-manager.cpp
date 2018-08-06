// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// loop-manager.cpp
//
#include "loop-manager.hpp"

#include "creature/creature.hpp"
#include "creature/player-party-factory.hpp"
#include "creature/player-party.hpp"
#include "game/game-data-file.hpp"
#include "game/game-state-factory.hpp"
#include "log/log-macros.hpp"
#include "misc/assertlogandthrow.hpp"
#include "popup/popup-info.hpp"
#include "popup/popup-manager.hpp"
#include "popup/popup-stage-res-change.hpp"
#include "sfml-util/loop-cmd-popup.hpp"
#include "sfml-util/loop.hpp"
#include "sfml-util/sfml-util-display.hpp"
#include "sfml-util/sfml-util-video-mode.hpp"
#include "stage/loop-cmd-addstage.hpp"

namespace heroespath
{
namespace game
{

    std::unique_ptr<LoopManager> LoopManager::instanceUPtr_;

    std::string LoopManager::startupStageName_ { sfml_util::LoopState::ToString(
        sfml_util::LoopState::Intro) };

    LoopManager::LoopManager()
        : state_(sfml_util::LoopState::None)
        , cmdQueueVec_()
        , loop_("DefaultLoop")
        , popupResponse_(popup::ResponseTypes::None)
        , popupSelection_(0)
        , prevState_(sfml_util::LoopState::None)
        , prevSettingsState_(sfml_util::LoopState::None)
        , stateBeforeFade_(sfml_util::LoopState::None)
        , exitSuccess_(true)
    {
        M_HP_LOG_DBG("Subsystem Construction: LoopManager");
    }

    LoopManager::~LoopManager() { M_HP_LOG_DBG("Subsystem Destruction: LoopManager"); }

    misc::NotNull<LoopManager *> LoopManager::Instance()
    {
        if (!instanceUPtr_)
        {
            M_HP_LOG_ERR("Subsystem Instance() called but instanceUPtr_ was null: LoopManager");
            Acquire();
        }

        return instanceUPtr_.get();
    }

    void LoopManager::Acquire()
    {
        if (!instanceUPtr_)
        {
            instanceUPtr_ = std::make_unique<LoopManager>();
        }
        else
        {
            M_HP_LOG_ERR("Subsystem Acquire() after Construction: LoopManager");
        }
    }

    void LoopManager::Release()
    {
        M_ASSERT_OR_LOGANDTHROW_SS(
            (instanceUPtr_), "LoopManager::Release() found instanceUPtr that was null.");

        instanceUPtr_.reset();
    }

    void LoopManager::Initialize()
    {
        M_ASSERT_OR_LOGANDTHROW_SS(
            (instanceUPtr_), "LoopManager::Initialize() found instanceUPtr that was null.");

        auto const STARTUP_STAGE_ENUM { static_cast<sfml_util::LoopState::Enum>(
            sfml_util::LoopState::FromString(startupStageName_)) };

        if (STARTUP_STAGE_ENUM != sfml_util::LoopState::Intro)
        {
            // TEMP TODO REMOVE -once done testing
            // create a party of characters to work with during testing
            creature::PlayerPartyFactory partyFactory;
            GameStateFactory::Instance()->NewGame(partyFactory.MakeFakeForTesting());
        }

        instanceUPtr_->TransitionTo(STARTUP_STAGE_ENUM);
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
        cmdQueueVec_.emplace_back(std::make_unique<sfml_util::LoopCmd_RemoveAllStages>());

        cmdQueueVec_.emplace_back(
            std::make_unique<sfml_util::LoopCmd_StateChange>(sfml_util::LoopState::Intro));

        cmdQueueVec_.emplace_back(std::make_unique<sfml_util::LoopCmd_ExitAfterKeypress>(true));
        cmdQueueVec_.emplace_back(std::make_unique<sfml_util::LoopCmd_ExitAfterMouseclick>(true));

        cmdQueueVec_.emplace_back(std::make_unique<sfml_util::LoopCmd_SetMouseVisibility>(false));

        cmdQueueVec_.emplace_back(std::make_unique<sfml_util::LoopCmd_AddStage_Default>());

        cmdQueueVec_.emplace_back(std::make_unique<sfml_util::LoopCmd_SetHoldTime>(2.5f));
        cmdQueueVec_.emplace_back(std::make_unique<sfml_util::LoopCmd_Execute>());

        // set the theme music volume to 25% even if the config file has it set lower so it can
        // always be heard during the intro
        auto const VOLUME_IF_INTRO_OR_CAMP_STAGE { 25.0f };
        float targetVolumeToUse(sfml_util::MusicOperator::VOLUME_USE_GLOBAL_);
        if (sfml_util::SoundManager::Instance()->MusicVolume() < VOLUME_IF_INTRO_OR_CAMP_STAGE)
        {
            targetVolumeToUse = VOLUME_IF_INTRO_OR_CAMP_STAGE;
        }

        cmdQueueVec_.emplace_back(std::make_unique<sfml_util::LoopCmd_StartMusic>(
            sfml_util::music::Theme,
            sfml_util::MusicOperator::FADE_MULT_DEFAULT_IN_,
            targetVolumeToUse));

        cmdQueueVec_.emplace_back(std::make_unique<sfml_util::LoopCmd_SetHoldTime>(2.0f));
        cmdQueueVec_.emplace_back(std::make_unique<sfml_util::LoopCmd_Execute>());

        cmdQueueVec_.emplace_back(std::make_unique<sfml_util::LoopCmd_RemoveAllStages>());
        cmdQueueVec_.emplace_back(std::make_unique<stage::LoopCmd_AddStage<stage::IntroStage>>());

        cmdQueueVec_.emplace_back(std::make_unique<sfml_util::LoopCmd_ExitAfterFade>());
        cmdQueueVec_.emplace_back(
            std::make_unique<sfml_util::LoopCmd_FadeIn>(sf::Color::Black, 50.0f));
        cmdQueueVec_.emplace_back(std::make_unique<sfml_util::LoopCmd_Execute>());

        cmdQueueVec_.emplace_back(std::make_unique<sfml_util::LoopCmd_SetHoldTime>(4.0f));
        cmdQueueVec_.emplace_back(std::make_unique<sfml_util::LoopCmd_Execute>());

        cmdQueueVec_.emplace_back(std::make_unique<sfml_util::LoopCmd_ExitAfterKeypress>(true));
        cmdQueueVec_.emplace_back(std::make_unique<sfml_util::LoopCmd_ExitAfterMouseclick>(true));

        cmdQueueVec_.emplace_back(std::make_unique<sfml_util::LoopCmd_ExitAfterFade>());
        cmdQueueVec_.emplace_back(
            std::make_unique<sfml_util::LoopCmd_FadeOut>(sf::Color::Black, 150.0f));
        cmdQueueVec_.emplace_back(std::make_unique<sfml_util::LoopCmd_Execute>());

        TransitionTo_MainMenu();
    }

    void LoopManager::TransitionFrom_Popup()
    {
        loop_.Exit();
        cmdQueueVec_.emplace_back(std::make_unique<sfml_util::LoopCmd_StateChange>(prevState_));
        cmdQueueVec_.emplace_back(std::make_unique<sfml_util::LoopCmd_RemoveStage_Popup>());

        cmdQueueVec_.emplace_back(std::make_unique<sfml_util::LoopCmd_FadeIn>(
            popup::PopupManager::Color_Fade(), popup::PopupManager::SpeedMult_Fade()));

        cmdQueueVec_.emplace_back(std::make_unique<sfml_util::LoopCmd_Execute>());
    }

    void LoopManager::TransitionTo_Exit()
    {
        CommandQueueClear();
        loop_.Exit();

        cmdQueueVec_.emplace_back(std::make_unique<sfml_util::LoopCmd_IgnoreMouse>(true));

        cmdQueueVec_.emplace_back(std::make_unique<sfml_util::LoopCmd_ExitAfterKeypress>(false));
        cmdQueueVec_.emplace_back(std::make_unique<sfml_util::LoopCmd_ExitAfterMouseclick>(false));

        cmdQueueVec_.emplace_back(
            std::make_unique<sfml_util::LoopCmd_StateChange>(sfml_util::LoopState::Exit));

        cmdQueueVec_.emplace_back(
            std::make_unique<sfml_util::LoopCmd_StopMusic>(sfml_util::music::Enum::All, 100.0f));

        cmdQueueVec_.emplace_back(std::make_unique<sfml_util::LoopCmd_ExitAfterFade>());
        cmdQueueVec_.emplace_back(std::make_unique<sfml_util::LoopCmd_FadeOut>());
        cmdQueueVec_.emplace_back(std::make_unique<sfml_util::LoopCmd_Execute>());

        cmdQueueVec_.emplace_back(std::make_unique<sfml_util::LoopCmd_RemoveAllStages>());
    }

    void LoopManager::TransitionTo_Credits()
    {
        TransitionHelper(
            static_cast<TransOpt>(TransOpt::ClearQueue | TransOpt::MouseIgnore),
            sfml_util::LoopState::Credits,
            std::make_unique<stage::LoopCmd_AddStage<stage::CreditsStage>>(),
            sfml_util::music::All,
            sfml_util::music::Credits);

        cmdQueueVec_.emplace_back(std::make_unique<sfml_util::LoopCmd_IgnoreMouse>(false));
        cmdQueueVec_.emplace_back(std::make_unique<sfml_util::LoopCmd_ExitAfterMouseclick>(true));
        cmdQueueVec_.emplace_back(std::make_unique<sfml_util::LoopCmd_Execute>());
        TransitionTo_MainMenu();
    }

    void LoopManager::TransitionTo_MainMenu()
    {
        TransitionHelper(
            static_cast<TransOpt>(
                TransOpt::MouseIgnore | TransOpt::MouseRestore | TransOpt::FinalExecute),
            sfml_util::LoopState::Menu,
            std::make_unique<stage::LoopCmd_AddStage<stage::MainMenuStage>>(),
            sfml_util::music::Wind,
            sfml_util::music::Theme);
    }

    void LoopManager::TransitionTo_Settings()
    {
        prevSettingsState_ = state_;

        TransitionHelper(
            TransOpt::All,
            sfml_util::LoopState::Settings,
            std::make_unique<stage::LoopCmd_AddStage<stage::SettingsStage>>(),
            sfml_util::music::All,
            sfml_util::music::Theme);

        TransitionTo_MainMenu(); // um...why is this here?  zTn 2017-2-22
    }

    void LoopManager::TransitionTo_CharacterCreation()
    {
        TransitionHelper(
            TransOpt::All,
            sfml_util::LoopState::Character,
            std::make_unique<stage::LoopCmd_AddStage<stage::CharacterStage>>(),
            sfml_util::music::All,
            sfml_util::music::Wind);
    }

    void LoopManager::TransitionTo_PartyCreation()
    {
        TransitionHelper(
            static_cast<TransOpt>(
                TransOpt::ClearQueue | TransOpt::MouseIgnore | TransOpt::MouseRestore),
            sfml_util::LoopState::Party,
            std::make_unique<stage::LoopCmd_AddStage<stage::PartyStage>>(),
            sfml_util::music::All,
            sfml_util::music::PartyCreation);

        cmdQueueVec_.emplace_back(
            std::make_unique<sfml_util::LoopCmd_FakeMouseClick>(sf::Vector2f(400.0f, 370.0f)));

        cmdQueueVec_.emplace_back(std::make_unique<sfml_util::LoopCmd_Execute>());
    }

    void LoopManager::TransitionTo_Inn()
    {
        TransitionHelper(
            TransOpt::All,
            sfml_util::LoopState::Inn,
            std::make_unique<stage::LoopCmd_AddStage<stage::InnStage>>());
    }

    void LoopManager::TransitionTo_Camp()
    {
        TransitionHelper(
            static_cast<TransOpt>(TransOpt::ClearQueue | TransOpt::MouseRestore),
            sfml_util::LoopState::Camp,
            std::make_unique<stage::LoopCmd_AddStage<stage::CampStage>>(),
            sfml_util::music::All,
            sfml_util::music::None);

        // establish the theme music volume
        const float VOLUME_IF_INTRO_OR_CAMP_STAGE(25.0f);

        // zero means 'use whatever music volume is set in the SoundManager'
        float targetVolumeToUse(0.0f);

        if (sfml_util::SoundManager::Instance()->MusicVolume() < VOLUME_IF_INTRO_OR_CAMP_STAGE)
            targetVolumeToUse = VOLUME_IF_INTRO_OR_CAMP_STAGE;

        cmdQueueVec_.emplace_back(std::make_unique<sfml_util::LoopCmd_StartMusic>(
            sfml_util::music::Theme,
            sfml_util::MusicOperator::FADE_MULT_DEFAULT_IN_,
            targetVolumeToUse));

        cmdQueueVec_.emplace_back(std::make_unique<sfml_util::LoopCmd_Execute>());
    }

    void LoopManager::TransitionTo_LoadGameMenu()
    {
        TransitionHelper(
            TransOpt::All,
            sfml_util::LoopState::Load,
            std::make_unique<stage::LoopCmd_AddStage<stage::LoadGameStage>>());
    }

    void LoopManager::TransitionTo_Combat(const bool WILL_ADVANCE_TURN)
    {
        TransitionHelper(
            TransOpt::All,
            sfml_util::LoopState::Combat,
            std::make_unique<stage::LoopCmd_AddStage_Combat>(WILL_ADVANCE_TURN),
            sfml_util::music::All,
            sfml_util::music::None);
    }

    void LoopManager::TransitionTo_Test()
    {
        TransitionHelper(
            TransOpt::All,
            sfml_util::LoopState::Test,
            std::make_unique<stage::LoopCmd_AddStage<stage::TestingStage>>(),
            sfml_util::music::All,
            sfml_util::music::None);
    }

    void LoopManager::TransitionTo_Treasure()
    {
        TransitionHelper(
            static_cast<TransOpt>(
                TransOpt::ClearQueue | TransOpt::FinalExecute | TransOpt::MouseRestore),
            sfml_util::LoopState::Treasure,
            std::make_unique<stage::LoopCmd_AddStage<stage::TreasureStage>>(),
            sfml_util::music::All,
            sfml_util::music::None);
    }

    void LoopManager::TransitionTo_Adventure()
    {
        TransitionHelper(
            TransOpt::All,
            sfml_util::LoopState::Adventure,
            std::make_unique<stage::LoopCmd_AddStage<stage::AdventureStage>>(),
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
            std::make_unique<stage::LoopCmd_AddStage_Inventory>(
                TURN_CREATURE_PTR, INVENTORY_CREATURE_PTR, CURRENT_PHASE),
            sfml_util::music::None,
            sfml_util::music::Inventory);
    }

    void LoopManager::TransitionHelper(
        const TransOpt OPTIONS,
        const sfml_util::LoopState::Enum NEW_STATE,
        sfml_util::LoopCmdUPtr_t && loopCmdUPtr,
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
            cmdQueueVec_.emplace_back(
                std::make_unique<sfml_util::LoopCmd_SetMouseVisibility>(false));

            cmdQueueVec_.emplace_back(std::make_unique<sfml_util::LoopCmd_IgnoreMouse>(true));
        }

        cmdQueueVec_.emplace_back(std::make_unique<sfml_util::LoopCmd_StateChange>(NEW_STATE));

        cmdQueueVec_.emplace_back(std::make_unique<sfml_util::LoopCmd_ExitAfterKeypress>(false));
        cmdQueueVec_.emplace_back(std::make_unique<sfml_util::LoopCmd_ExitAfterMouseclick>(false));

        if (MUSIC_TO_STOP != sfml_util::music::None)
        {
            cmdQueueVec_.emplace_back(
                std::make_unique<sfml_util::LoopCmd_StopMusic>(MUSIC_TO_STOP));
        }

        if (MUSIC_TO_START != sfml_util::music::None)
        {
            cmdQueueVec_.emplace_back(
                std::make_unique<sfml_util::LoopCmd_StartMusic>(MUSIC_TO_START));
        }

        cmdQueueVec_.emplace_back(std::make_unique<sfml_util::LoopCmd_ExitAfterFade>());
        cmdQueueVec_.emplace_back(std::make_unique<sfml_util::LoopCmd_FadeOut>());
        cmdQueueVec_.emplace_back(std::make_unique<sfml_util::LoopCmd_Execute>());

        cmdQueueVec_.emplace_back(std::make_unique<sfml_util::LoopCmd_RemoveAllStages>());

        cmdQueueVec_.emplace_back(std::move(loopCmdUPtr));

        cmdQueueVec_.emplace_back(std::make_unique<sfml_util::LoopCmd_ExitAfterFade>());
        cmdQueueVec_.emplace_back(std::make_unique<sfml_util::LoopCmd_FadeIn>());
        cmdQueueVec_.emplace_back(std::make_unique<sfml_util::LoopCmd_Execute>());

        // cmdQueue_.push( std::make_unique<sfml_util::LoopCmd_IgnoreKeystrokes>(false) );

        if ((OPTIONS == TransOpt::All) || (OPTIONS & TransOpt::MouseRestore))
        {
            cmdQueueVec_.emplace_back(std::make_unique<sfml_util::LoopCmd_IgnoreMouse>(false));
            cmdQueueVec_.emplace_back(
                std::make_unique<sfml_util::LoopCmd_SetMouseVisibility>(true));
        }

        if ((OPTIONS == TransOpt::All) || (OPTIONS & TransOpt::FinalExecute))
        {
            cmdQueueVec_.emplace_back(std::make_unique<sfml_util::LoopCmd_Execute>());
        }
    }

    bool LoopManager::Execute()
    {
        while (false == cmdQueueVec_.empty())
        {
            // M_HP_LOG("LoopManager::Execute() executing: " << cmdQueueVec_.front()->GetName() );

            cmdQueueVec_.front()->Execute();
            cmdQueueVec_.erase(std::begin(cmdQueueVec_));

            auto const CURRENT_STATE { loop_.GetState() };
            if (CURRENT_STATE != state_)
            {
                prevState_ = state_;
                state_ = CURRENT_STATE;

                M_HP_LOG(
                    "LoopManager changed state from " << sfml_util::LoopState::ToString(prevState_)
                                                      << " to "
                                                      << sfml_util::LoopState::ToString(state_));
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
            case sfml_util::LoopState::Menu:
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
            case sfml_util::LoopState::Party:
            {
                TransitionTo_PartyCreation();
                break;
            }
            case sfml_util::LoopState::Camp:
            {
                TransitionTo_Camp();
                break;
            }
            case sfml_util::LoopState::Load:
            {
                TransitionTo_LoadGameMenu();
                break;
            }
            case sfml_util::LoopState::Character:
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
            case sfml_util::LoopState::Previous:
            {
                TransitionTo_Previous();
                break;
            }
            case sfml_util::LoopState::None:
            case sfml_util::LoopState::Popup:
            case sfml_util::LoopState::Query:
            case sfml_util::LoopState::Start:
            case sfml_util::LoopState::Next:
            case sfml_util::LoopState::Save:
            case sfml_util::LoopState::Help:
            case sfml_util::LoopState::Count:
            default:
            {
                std::ostringstream ss;

                ss << "sfml_util::LoopManager::TransitionTo(new_loop_state=\""
                   << sfml_util::LoopState::ToString(STATE)
                   << "\", will_adv_turn=" << std::boolalpha << WILL_ADVANCE_TURN
                   << ") but that new_loop_state is invalid.";

                throw std::range_error(ss.str());
            }
        }
    }

    sfml_util::DisplayChangeResult::Enum LoopManager::ChangeResolution(
        const sfml_util::IStagePtr_t CURRENT_ISTAGE_PTR,
        const sfml_util::gui::PopupCallback_t::IHandlerPtr_t POPUP_HANDLER_PTR,
        const sfml_util::Resolution & NEW_RES,
        const unsigned ANTIALIAS_LEVEL)
    {
        auto const CHANGE_RESULT { sfml_util::Display::Instance()->ChangeVideoMode(
            NEW_RES, ANTIALIAS_LEVEL) };

        CURRENT_ISTAGE_PTR->HandleResolutionChange();

        auto isPopupTypeResolutionChange { true };

        sfml_util::gui::TextInfo textInfo(
            "Keep this setting?",
            sfml_util::FontManager::Instance()->GetFont(sfml_util::GuiFont::Default),
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
                ss << "Unable to switch to that resolution.  Switched to video mode="
                   << sfml_util::Display::GetCurrentVideoMode()
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
            PopupWaitBeginSpecific<popup::PopupStageResChange>(POPUP_HANDLER_PTR, POPUP_INFO);
        }
        else
        {
            PopupWaitBegin(POPUP_HANDLER_PTR, POPUP_INFO);
        }

        return CHANGE_RESULT;
    }

    void LoopManager::CommandQueueClear() { cmdQueueVec_.clear(); }

} // namespace game
} // namespace heroespath
