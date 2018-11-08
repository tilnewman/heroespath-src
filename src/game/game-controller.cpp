// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// game-controller.cpp
//
#include "game-controller.hpp"

#include "game/game.hpp"
#include "game/loop.hpp"
#include "gui/display.hpp"
#include "gui/resolution.hpp"
#include "gui/sound-manager.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/enum-util.hpp"
#include "misc/log-macros.hpp"
#include "popup/popup-info.hpp"
#include "popup/popup-manager.hpp"
#include "sfutil/display.hpp"
#include "sfutil/video-mode.hpp"
#include "stage/i-stage.hpp"

#include <sstream>

namespace heroespath
{
namespace game
{

    std::unique_ptr<GameController> GameController::instanceUPtr_;

    std::string GameController::startupStageName_ { stage::Stage::ToString(stage::Stage::Intro) };

    GameController::GameController()
        : status_()
        , stageTracker_()
        , activeStages_()
        , commandFactory_()
        , commandQueue_()
        , prePopupExecuteCommandWithoutFade_()
    {
        M_HP_LOG_DBG("Subsystem Construction: GameController");
    }

    GameController::~GameController() { M_HP_LOG_DBG("Subsystem Destruction: GameController"); }

    misc::NotNull<GameController *> GameController::Instance()
    {
        if (!instanceUPtr_)
        {
            M_HP_LOG_ERR("Subsystem Instance() called but instanceUPtr_ was null: GameController");
            Acquire();
        }

        return instanceUPtr_;
    }

    void GameController::Acquire()
    {
        if (!instanceUPtr_)
        {
            instanceUPtr_ = std::make_unique<GameController>();
        }
        else
        {
            M_HP_LOG_ERR("Subsystem Acquire() after Construction: GameController");
        }
    }

    void GameController::Release()
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (instanceUPtr_), "GameController::Release() found instanceUPtr that was null.");

        instanceUPtr_.reset();
    }

    void GameController::Initialize()
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (instanceUPtr_), "GameController::Initialize() found instanceUPtr that was null.");

        const auto STARTUP_STAGE_ENUM { static_cast<stage::Stage::Enum>(
            EnumUtil<stage::Stage>::FromString(startupStageName_)) };

        if (stage::Stage::IsPlayableAndNotPopup(STARTUP_STAGE_ENUM))
        {
            M_HP_LOG(
                "command line specified which stage to start at \""
                << startupStageName_ << "\" (" << stage::Stage::ToString(STARTUP_STAGE_ENUM)
                << ").  Jumping to that stage now.");

            // TEMP TODO REMOVE -once done testing
            Game::Instance()->MakeNewForTestingAndSet();

            instanceUPtr_->TransitionTo(stage::SetupPacket(STARTUP_STAGE_ENUM));
        }
        else
        {
            M_HP_LOG(
                "command line specified which stage to start at \""
                << startupStageName_ << "\" (" << stage::Stage::ToString(STARTUP_STAGE_ENUM)
                << "), but that stage is not a valid start stage so ignoring it.");

            instanceUPtr_->TransitionTo(stage::Stage::Intro);
        }
    }

    void GameController::PreTaskLogs(
        const std::string & NAME_OF_TASK_ABOUT_TO_EXECUTE, const Command & COMMAND) const
    {
        M_HP_LOG("Pre \"" << NAME_OF_TASK_ABOUT_TO_EXECUTE << "\" " << COMMAND.ToString());

        M_HP_LOG(
            "Pre \"" << NAME_OF_TASK_ABOUT_TO_EXECUTE << "\" GameControllerStatus(stage="
                     << GetStageName() << ", phase=" << game::Phase::ToString(GetPhase())
                     << ", cmd_q_size=" << commandQueue_.Size() << ", " << status_.ToString()
                     << ")");
    }

    void GameController::PlayGame()
    {
        while (commandQueue_.IsEmpty() == false)
        {
            const auto COMMAND_OPT { commandQueue_.Pop() };

            if (COMMAND_OPT)
            {
                ExecuteGameCommand(COMMAND_OPT.value());
            }
        }

        M_HP_LOG("GameController reached the end of the GameCommandQueue.  Shutting down.");
    }

    void GameController::SpawnPopup(
        const misc::PopupCallback_t::IHandlerPtr_t POPUP_HANDLER_PTR,
        const popup::PopupInfo & POPUP_INFO)
    {
        StageChangePostPopupSpawn(PopupReplaceCommand(POPUP_INFO, POPUP_HANDLER_PTR));
    }

    void GameController::RemovePopup(
        const popup::PopupButtons::Enum TYPE, const std::size_t SELECTION)
    {
        StageChangePostPopupRemove(TYPE, SELECTION);
    }

    void GameController::TransitionTo(const stage::Stage::Enum NEW_STAGE)
    {
        TransitionTo(stage::SetupPacket(NEW_STAGE));
    }

    void GameController::TransitionTo(const stage::SetupPacket & SETUP_PACKET)
    {
        StageChangePostNonPopupReplace(SETUP_PACKET);
    }

    void GameController::StageChangeActualNonPopupReplace(const stage::SetupPacket & SETUP_PACKET)
    {
        stageTracker_.SetCurrent(SETUP_PACKET.stage);
        activeStages_.ReplaceStages(SETUP_PACKET);
    }

    void GameController::StageChangeActualPopupReplace(
        const popup::PopupInfo & POPUP_INFO,
        const misc::PopupCallback_t::IHandlerPtr_t POPUP_HANDLER_PTR)
    {
        stageTracker_.SetCurrent(stage::Stage::Popup);
        activeStages_.ReplacePopupStage(POPUP_INFO, POPUP_HANDLER_PTR);
    }

    void GameController::StageChangeActualPopupRemove()
    {
        stageTracker_.SetCurrentToNonPopupPrevious();
        activeStages_.RemovePopupStage();
    }

    void GameController::ExecuteGameCommand(const Command & COMMAND)
    {
        // this order is critical

        PreTaskLogs("GameCommand", COMMAND);

        if (COMMAND.music_opt)
        {
            try
            {
                const auto MUSIC_COMMAND { COMMAND.music_opt.value() };

                if (EnumUtil<gui::music>::IsValid(MUSIC_COMMAND.to_stop)
                    || (gui::music::All == MUSIC_COMMAND.to_stop))
                {
                    gui::SoundManager::Instance()->MusicStop(
                        MUSIC_COMMAND.to_stop, gui::MusicOperator::FADE_MULT_DEFAULT_OUT_);
                }

                if (EnumUtil<gui::music>::IsValid(MUSIC_COMMAND.to_start))
                {
                    auto volume { gui::SoundManager::Instance()->MusicVolume() };
                    if (volume < MUSIC_COMMAND.volume_min)
                    {
                        volume = MUSIC_COMMAND.volume_min;
                    }

                    gui::SoundManager::Instance()->MusicStart(
                        MUSIC_COMMAND.to_start, gui::MusicOperator::FADE_MULT_DEFAULT_IN_, volume);
                }
            }
            catch (const std::exception & EXCEPTION)
            {
                PreTaskLogs("Music Exception", COMMAND);

                M_HP_LOG_ERR(
                    "Exception=\"" << EXCEPTION.what()
                                   << "\" thrown during music comamnd processing.  This is not "
                                      "considered fatal so the game will continue.");
            }
        }

        if (COMMAND.stage_opt)
        {
            try
            {
                StageChangeActualNonPopupReplace(COMMAND.stage_opt.value());
            }
            catch (const std::exception & EXCEPTION)
            {
                PreTaskLogs("Stage Replace Exception", COMMAND);

                M_HP_LOG_FAT(
                    "Exception=\"" << EXCEPTION.what()
                                   << "\" thrown during stage replace comamnd processing.  This "
                                      "means a Stage could not be constructed and setup, which is "
                                      "fatal.  Re-Throwing to kill the game.");

                throw;
            }
        }

        if (COMMAND.popup_replace_opt)
        {
            try
            {
                const auto POPUP_REPLACE_COMMAND { COMMAND.popup_replace_opt.value() };

                StageChangeActualPopupReplace(
                    POPUP_REPLACE_COMMAND.popup_info, POPUP_REPLACE_COMMAND.handler_ptr);
            }
            catch (const std::exception & EXCEPTION)
            {
                PreTaskLogs("Popup Replace Exception", COMMAND);

                M_HP_LOG_FAT(
                    "Exception=\""
                    << EXCEPTION.what()
                    << "\" thrown during popup replace comamnd processing.  This "
                       "means a Popup Stage could not be constructed and setup, which is "
                       "fatal.  Re-Throwing to kill the game.");

                throw;
            }
        }

        if (COMMAND.will_remove_popup)
        {
            try
            {
                StageChangeActualPopupRemove();
            }
            catch (const std::exception & EXCEPTION)
            {
                PreTaskLogs("Popup Remove Exception", COMMAND);

                M_HP_LOG_FAT(
                    "Exception=\"" << EXCEPTION.what()
                                   << "\" thrown during popup remove comamnd processing.  This "
                                      "means a Popup Stage could not be rmeoved, which is "
                                      "fatal.  Re-Throwing to kill the game.");

                throw;
            }
        }

        if (COMMAND.execute_opt)
        {
            const auto EXECUTE_COMMAND { COMMAND.execute_opt.value() };

            if (stage::Stage::IsPlayableAndNotPopup(stageTracker_.GetCurrent()))
            {
                prePopupExecuteCommandWithoutFade_ = EXECUTE_COMMAND;
                prePopupExecuteCommandWithoutFade_.fade_opt = boost::none;
            }

            if (EXECUTE_COMMAND.fade_opt)
            {
                try
                {
                    ExecuteGameCommandFade(EXECUTE_COMMAND.fade_opt.value());
                }
                catch (const std::exception & EXCEPTION)
                {
                    PreTaskLogs("Fade Exception", COMMAND);

                    M_HP_LOG_ERR(
                        "Exception=\"" << EXCEPTION.what()
                                       << "\" thrown during fade comamnd processing.  This is not "
                                          "considered fatal so the game will continue.");
                }
            }

            gui::Display::Instance()->PollEvents();

            try
            {
                {
                    status_.LoopStopRequestReset();
                    game::Loop loop(activeStages_, status_, EXECUTE_COMMAND);
                    loop.Execute();
                }
            }
            catch (const std::exception & EXCEPTION)
            {
                PreTaskLogs("GameLoop Exception", COMMAND);

                M_HP_LOG_FAT(
                    "Exception=\""
                    << EXCEPTION.what()
                    << "\" thrown during exeute comamnd processing (during the game loop).  This "
                       "is considered fatal, at least untul zTn implements some kind of retry...  "
                       "Re-Throwing to kill the game.");

                throw;
            }

            if (status_.IsGameExitRequested())
            {
                status_.GameExitRequestReset();

                M_HP_LOG("GameController acknowledging the GameExitRequested flag and "
                         "transitioning Stage to Exit.");

                TransitionTo(stage::Stage::Exit);
            }
        }
    }

    void GameController::ExecuteGameCommandFade(const FadeCommand & FADE_COMMAND)
    {
        const auto IS_FADING_IN { (FADE_COMMAND.direction == FadeDirection::In) };
        if (IS_FADING_IN)
        {
            gui::Display::Instance()->FadeInStart(FADE_COMMAND.speed);
            status_.StartFadeIn(FADE_COMMAND.set_will_draw_under_popup_opt);
        }
        else
        {
            gui::Display::Instance()->FadeOutStart(FADE_COMMAND.color, FADE_COMMAND.speed);
            status_.StartFadeOut(FADE_COMMAND.color, FADE_COMMAND.set_will_draw_under_popup_opt);
        }

        activeStages_.SetIsFadingForAllStages(true);
    }

    const gui::DisplayChangeResult GameController::ChangeResolution(
        const misc::PopupCallback_t::IHandlerPtr_t POPUP_HANDLER_PTR,
        const gui::Resolution & NEW_RES,
        const unsigned ANTIALIAS_LEVEL)
    {
        const auto RESULT { gui::Display::Instance()->ChangeVideoMode(NEW_RES, ANTIALIAS_LEVEL) };

        if (RESULT.Changed() == false)
        {
            return RESULT;
        }

        if (RESULT.did_res_change)
        {
            auto handleResolutionChange = [](stage::IStagePtr_t iStagePtr) {
                iStagePtr->HandleResolutionChange();
                return boost::none;
            };

            activeStages_.ExecuteOnNonPopupStages(handleResolutionChange);
            activeStages_.ExecuteOnPopupStage(handleResolutionChange);
        }

        gui::TextInfo textInfo(
            "Keep this setting?",
            gui::GuiFont::Default,
            gui::FontManager::Instance()->Size_Normal(),
            sf::Color::Black,
            gui::Justified::Center);

        const auto POPUP_INFO { popup::PopupManager::Instance()->CreateResolutionChangePopupInfo(
            textInfo) };

        SpawnPopup(POPUP_HANDLER_PTR, POPUP_INFO);

        return RESULT;
    }

    void GameController::TestingStrAppend(const std::string & MESSAGE)
    {
        auto handleTestStringAppend = [MESSAGE](stage::IStagePtr_t iStagePtr) {
            iStagePtr->TestingStrAppend(MESSAGE);
            return boost::none;
        };

        activeStages_.ExecuteOnNonPopupStages(handleTestStringAppend);
    }

    void GameController::TestingStrIncrement(const std::string & MESSAGE)
    {
        auto handleTestStringIncrement = [MESSAGE](stage::IStagePtr_t iStagePtr) {
            iStagePtr->TestingStrIncrement(MESSAGE);
            return boost::none;
        };

        activeStages_.ExecuteOnNonPopupStages(handleTestStringIncrement);
    }

    void GameController::TestingImageSet(
        const std::string & PATH_STR, const bool WILL_CHECK_FOR_OUTLINE)
    {
        auto handleTestImageSet = [PATH_STR, WILL_CHECK_FOR_OUTLINE](stage::IStagePtr_t iStagePtr) {
            iStagePtr->TestingImageSet(PATH_STR, WILL_CHECK_FOR_OUTLINE);
            return boost::none;
        };

        activeStages_.ExecuteOnNonPopupStages(handleTestImageSet);
    }

    void GameController::StageChangePostPopupSpawn(const PopupReplaceCommand & POPUP_ADD_COMMAND)
    {
        M_HP_LOG("popup spawn post: " << POPUP_ADD_COMMAND.popup_info.ToStringShort());
        RequestLoopExit();

        commandQueue_.ClearAndPush(commandFactory_.MakeCommandsForPopupSpawn(
            POPUP_ADD_COMMAND, activeStages_.HasPopupStage()));
    }

    void GameController::StageChangePostPopupRemove(
        const popup::PopupButtons::Enum TYPE, const std::size_t SELECTION)
    {
        if (!activeStages_.HasPopupStage())
        {
            M_HP_LOG_ERR(
                "A popup response was given to GameController but there was no loop or no "
                "popup_stage."
                + M_HP_VAR_STR(TYPE) + M_HP_VAR_STR(SELECTION));

            return;
        }

        M_HP_LOG(
            "popup remove post: type=" << popup::PopupButtons::ToString(TYPE)
                                       << ", selection=" << SELECTION);

        const auto CURRENT_EXECUTION_COMMAND_WITHOUT_FADE { prePopupExecuteCommandWithoutFade_ };

        activeStages_.SetPopupResponse(TYPE, SELECTION);

        RequestLoopExit();

        commandQueue_.ClearAndPush(
            commandFactory_.MakeCommandsForPopupRemove(CURRENT_EXECUTION_COMMAND_WITHOUT_FADE));
    }

    void GameController::StageChangePostNonPopupReplace(
        const StageReplaceCommand & STAGE_SETUP_COMMAND)
    {
        if (STAGE_SETUP_COMMAND.stage == stage::Stage::Previous)
        {
            const auto NON_POPUP_PREVIOUS_STAGE { stageTracker_.GetNonPopupPrevious() };

            if (stage::Stage::IsPlayableAndNotPopup(NON_POPUP_PREVIOUS_STAGE))
            {
                M_HP_LOG_DBG(
                    "Transitioning to previous stage: "
                    + stage::Stage::ToString(NON_POPUP_PREVIOUS_STAGE));

                StageReplaceCommand stageReplaceCommand { STAGE_SETUP_COMMAND };
                stageReplaceCommand.stage = NON_POPUP_PREVIOUS_STAGE;

                StageChangePostNonPopupReplace(stageReplaceCommand);
            }
            else
            {
                M_HP_LOG_ERR(
                    "Ignoring attempt to TransitionTo previous stage when there was no valid "
                    "previous stage.  previous="
                    + stage::Stage::ToString(NON_POPUP_PREVIOUS_STAGE));

                return;
            }
        }

        if (stage::Stage::IsPlayableAndNotPopup(STAGE_SETUP_COMMAND.stage) == false)
        {
            M_HP_LOG_ERR(
                "Ignoring attempt to TransitionTo(setup_packet="
                << STAGE_SETUP_COMMAND.ToString()
                << ") because that stage is not valid (stage::Stage::IsPlayableAndNotPopup()).");

            return;
        }

        if ((STAGE_SETUP_COMMAND.stage == stage::Stage::Inventory)
            && !STAGE_SETUP_COMMAND.inventory_packet_opt)
        {
            M_HP_LOG_ERR(
                "Ignoring attempt to TransitionTo(setup_packet="
                << STAGE_SETUP_COMMAND.ToString()
                << ") because even though the stage is set to Inventory the setup packet has no "
                   "inventory information.");

            return;
        }

        M_HP_LOG(
            "non-popup stage create/replace post: stage="
            << stage::Stage::ToString(STAGE_SETUP_COMMAND.stage));

        RequestLoopExit();

        commandQueue_.ClearAndPush(
            commandFactory_.MakeCommandsForNonPopupStageReplace(STAGE_SETUP_COMMAND));
    }

} // namespace game
} // namespace heroespath
