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
#include "gui/texture-cache.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/enum-util.hpp"
#include "misc/log-macros.hpp"
#include "misc/strings.hpp"
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

    std::string GameController::startupStageName_ { NAMEOF_ENUM(stage::Stage::Intro) };

    GameController::GameController()
        : status_()
        , stageTracker_()
        , activeStages_()
        , commandFactory_()
        , commandQueue_()
        , prePopupExecuteCommandWithoutFade_()
        , commandCounter_(0)
    {
        M_HP_LOG_DBG("Subsystem Construction: GameController");
    }

    GameController::~GameController() { M_HP_LOG_DBG("Subsystem Destruction: GameController"); }

    misc::NotNull<GameController *> GameController::Instance()
    {
        if (!instanceUPtr_)
        {
            M_HP_LOG_ERR("Subsystem Instance() called but instanceUPtr_ was null: GameController");
            Create();
        }

        return misc::NotNull<GameController *>(instanceUPtr_.get());
    }

    void GameController::Create()
    {
        if (!instanceUPtr_)
        {
            instanceUPtr_ = std::make_unique<GameController>();
        }
        else
        {
            M_HP_LOG_ERR("Subsystem Create() after Construction: GameController");
        }
    }

    void GameController::Destroy() { instanceUPtr_.reset(); }

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
                << startupStageName_ << "\" (" << NAMEOF_ENUM(STARTUP_STAGE_ENUM)
                << ").  Jumping to that stage now.");

            instanceUPtr_->TransitionTo(stage::SetupPacket(STARTUP_STAGE_ENUM));
        }
        else
        {
            M_HP_LOG(
                "command line specified which stage to start at \""
                << startupStageName_ << "\" (" << NAMEOF_ENUM(STARTUP_STAGE_ENUM)
                << "), but that stage is not a valid start stage so ignoring it.");

            instanceUPtr_->TransitionTo(stage::Stage::Intro);
        }
    }

    void GameController::ExecptionLogging(const Command & COMMAND) const
    {
        M_HP_LOG(
            "Exception during execution of " + COMMAND.ToString() + ", GameController"
            + status_.ToString());
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
        M_HP_LOG(
            "changing from " << GetStageName() << " stage to " << NAMEOF_ENUM(SETUP_PACKET.stage)
                             << " stage.");

        if (activeStages_.HasPopupStage())
        {
            M_HP_LOG("Detected a popup just before a stage change.  Removing the popup first...");
            StageChangeActualPopupRemove();
        }

        const auto ALL_STAGE_NAMES_BEFORE { activeStages_.AllStageNames() };

        stageTracker_.SetCurrent(SETUP_PACKET.stage);

        activeStages_.RemoveNonPopupStages();

        if (activeStages_.HasAnyStages() == false)
        {
            M_HP_LOG("GameController detected all stages destroyed.  Clearing caches...");

            try
            {
                gui::SoundManager::Instance()->ClearSoundEffectsCache();
            }
            catch (const std::exception & EXCEPTION)
            {
                M_HP_LOG_ERR(
                    "GameController::StageChangeActualNonPopupReplace("
                    << SETUP_PACKET.ToString() << ") started with " << ALL_STAGE_NAMES_BEFORE
                    << ", destroyed them all, and then attempted to clear caches but an exception "
                       "was thrown by SoundManager::ClearSoundEffectsCache(): \""
                    << EXCEPTION.what() << "\".  This will be ignored and the game will continue.");
            }

            try
            {
                gui::TextureCache::Instance()->RemoveAll();
            }
            catch (const std::exception & EXCEPTION)
            {
                M_HP_LOG_ERR(
                    "GameController::StageChangeActualNonPopupReplace("
                    << SETUP_PACKET.ToString() << ") started with " << ALL_STAGE_NAMES_BEFORE
                    << ", destroyed them all, and then attempted to clear caches but an exception "
                       "was thrown by TextureCache::RemoveAll(): \""
                    << EXCEPTION.what() << "\".  This will be ignored and the game will continue.");
            }

            try
            {
                gui::FontManager::Instance()->UnloadAll();
            }
            catch (const std::exception & EXCEPTION)
            {
                M_HP_LOG_ERR(
                    "GameController::StageChangeActualNonPopupReplace("
                    << SETUP_PACKET.ToString() << ") started with " << ALL_STAGE_NAMES_BEFORE
                    << ", destroyed them all, and then attempted to clear caches but an exception "
                       "was thrown by FontManager::UnloadAll(): \""
                    << EXCEPTION.what() << "\".  This will be ignored and the game will continue.");
            }
        }

        activeStages_.AddNonPopupStages(SETUP_PACKET);
        M_HP_LOG("stage replace confirmed");
    }

    void GameController::StageChangeActualPopupReplace(
        const popup::PopupInfo & POPUP_INFO,
        const misc::PopupCallback_t::IHandlerPtr_t POPUP_HANDLER_PTR)
    {
        stageTracker_.SetCurrent(stage::Stage::Popup);
        activeStages_.ReplacePopupStage(POPUP_INFO, POPUP_HANDLER_PTR);
        M_HP_LOG("popup replace confirmed: " + POPUP_INFO.Name());
    }

    void GameController::StageChangeActualPopupRemove()
    {
        stageTracker_.SetCurrentToNonPopupPrevious();
        activeStages_.RemovePopupStage();
        M_HP_LOG("popup remove confirmed");
    }

    void GameController::ExecuteGameCommand(const Command & COMMAND)
    {
        M_HP_LOG(
            "#" << ++commandCounter_ << " Begin: " << COMMAND.ToString() << " while in "
                << GetStageName() << " stage with GameController" << status_.ToString());

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
                ExecptionLogging(COMMAND);

                M_HP_LOG_ERR(
                    "Exception=\"" << EXCEPTION.what()
                                   << "\" thrown during music command processing.  This is not "
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
                ExecptionLogging(COMMAND);

                M_HP_LOG_FAT(
                    "Exception=\"" << EXCEPTION.what()
                                   << "\" thrown during stage replace command processing.  This "
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
                ExecptionLogging(COMMAND);

                M_HP_LOG_FAT(
                    "Exception=\""
                    << EXCEPTION.what()
                    << "\" thrown during popup replace command processing.  This "
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
                ExecptionLogging(COMMAND);

                M_HP_LOG_FAT(
                    "Exception=\"" << EXCEPTION.what()
                                   << "\" thrown during popup remove command processing.  This "
                                      "means a Popup Stage could not be removed, which is "
                                      "fatal.  Re-Throwing to kill the game.");

                throw;
            }
        }

        if (COMMAND.execute_opt)
        {
            const auto EXECUTE_COMMAND { COMMAND.execute_opt.value() };

            if (stage::Stage::IsPlayableAndNotPopup(stageTracker_.GetCurrent())
                && !EXECUTE_COMMAND.fade_opt)
            {
                prePopupExecuteCommandWithoutFade_ = EXECUTE_COMMAND;
            }

            if (EXECUTE_COMMAND.fade_opt)
            {
                try
                {
                    ExecuteGameCommandFade(EXECUTE_COMMAND.fade_opt.value());
                }
                catch (const std::exception & EXCEPTION)
                {
                    ExecptionLogging(COMMAND);

                    M_HP_LOG_ERR(
                        "Exception=\"" << EXCEPTION.what()
                                       << "\" thrown during fade command processing.  This is not "
                                          "considered fatal so the game will continue.");
                }
            }

            gui::Display::Instance()->PollEvents();

            try
            {
                {
                    status_.LoopStopRequestReset();
                    game::Loop loop(activeStages_, status_, EXECUTE_COMMAND);
                    M_HP_LOG("game loop begin");
                    loop.Execute();

                    M_HP_LOG(
                        "game loop end  (after " << loop.DurationSec() << " seconds and "
                                                 << loop.FrameCount() << " frames)");

                    status_.LoopStopRequestReset();
                }
            }
            catch (const std::exception & EXCEPTION)
            {
                ExecptionLogging(COMMAND);

                M_HP_LOG_FAT(
                    "Exception=\""
                    << EXCEPTION.what()
                    << "\" thrown during execute command processing (during the game loop).  This "
                       "is considered fatal, at least until zTn implements some kind of retry...  "
                       "Re-Throwing to kill the game.");

                throw;
            }

            if (status_.IsGameExitRequested() && !status_.IsGameExiting())
            {
                status_.GameExitRequestReset();
                status_.SetGameIsExiting();

                M_HP_LOG("GameController acknowledging the GameExitRequested flag and "
                         "transitioning Stage to Exit.");

                TransitionTo(stage::Stage::Exit);
            }
        }

        M_HP_LOG("#" << commandCounter_ << " End: " << COMMAND.ToString());
    }

    void GameController::ExecuteGameCommandFade(const FadeCommand & FADE_COMMAND)
    {
        const auto IS_FADING_IN { (FADE_COMMAND.direction == FadeDirection::In) };
        if (IS_FADING_IN)
        {
            M_HP_LOG("fade in begin");
            gui::Display::Instance()->FadeInStart(FADE_COMMAND.speed);
            status_.StartFadeIn(FADE_COMMAND.set_will_draw_under_popup_opt);
        }
        else
        {
            M_HP_LOG("fade out begin");
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

    void GameController::StageChangePostPopupSpawn(const PopupReplaceCommand & POPUP_ADD_COMMAND)
    {
        M_HP_LOG("popup spawn requested: " << POPUP_ADD_COMMAND.popup_info.ToStringShort());
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
                + EnumUtil<popup::PopupButtons>::ToString(TYPE) + M_HP_VAR_STR(SELECTION));

            return;
        }

        M_HP_LOG(
            "popup remove requested: will remove " << NAMEOF_ENUM(stageTracker_.GetCurrent())
                                                   << " because button press=" << NAMEOF_ENUM(TYPE)
                                                   << " and selection=" << SELECTION);

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
                    + NAMEOF_ENUM_STR(NON_POPUP_PREVIOUS_STAGE));

                StageReplaceCommand stageReplaceCommand { STAGE_SETUP_COMMAND };
                stageReplaceCommand.stage = NON_POPUP_PREVIOUS_STAGE;

                StageChangePostNonPopupReplace(stageReplaceCommand);
            }
            else
            {
                M_HP_LOG_ERR(
                    "Ignoring attempt to TransitionTo previous stage when there was no valid "
                    "previous stage.  previous="
                    + NAMEOF_ENUM_STR(NON_POPUP_PREVIOUS_STAGE));

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

        M_HP_LOG("stage create/replace requested: " << NAMEOF_ENUM(STAGE_SETUP_COMMAND.stage));

        RequestLoopExit();

        const auto HAS_CURRENT_STAGE { (stageTracker_.GetCurrent() != stage::Stage::None) };

        commandQueue_.ClearAndPush(commandFactory_.MakeCommandsForNonPopupStageReplace(
            STAGE_SETUP_COMMAND, HAS_CURRENT_STAGE));
    }

} // namespace game
} // namespace heroespath
