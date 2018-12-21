// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// command-factory.cpp
//
#include "command-factory.hpp"

#include "gui/display.hpp"
#include "misc/config-file.hpp"
#include "misc/log-macros.hpp"
#include "popup/popup-manager.hpp"

namespace heroespath
{
namespace game
{

    const CommandVec_t CommandFactory::MakeCommandsForPopupSpawn(
        const PopupReplaceCommand & POPUP_ADD_COMMAND, const bool IS_ALREADY_POPUP) const
    {
        CommandVec_t commandVec;

        if (IS_ALREADY_POPUP == false)
        {
            MakeAndAppendCommandFade(
                commandVec, MakeCommandForStateChangeFade(FadeDirection::Out, ForPopup::Yes, true));
        }

        MakeAndAppendCommandReplacePopup(commandVec, POPUP_ADD_COMMAND);
        MakeAndAppendCommandExecute(commandVec, ExecuteCommand());

        VerifyNoneAreEmpty(__func__, commandVec);
        return commandVec;
    }

    const CommandVec_t CommandFactory::MakeCommandsForPopupRemove(
        const ExecuteCommand & EXECUTE_COMMAND_BEFORE_CHANGE) const
    {
        CommandVec_t commandVec;
        MakeAndAppendCommandRemovePopup(commandVec);

        MakeAndAppendCommandFade(
            commandVec, MakeCommandForStateChangeFade(FadeDirection::In, ForPopup::Yes, false));

        MakeAndAppendCommandExecute(commandVec, EXECUTE_COMMAND_BEFORE_CHANGE);

        VerifyNoneAreEmpty(__func__, commandVec);
        return commandVec;
    }

    const CommandVec_t CommandFactory::MakeCommandsForNonPopupStageReplace(
        const StageReplaceCommand & STAGE_SETUP_COMMAND, const bool HAS_CURRENT_STAGE) const
    {
        CommandVec_t commandVec;

        MusicCommandOpt_t musicCommandOpt { MakeCommandForStateChangeMusic(
            STAGE_SETUP_COMMAND.stage) };

        FadeCommandOpt_t fadeOutCommandOpt { MakeCommandForStateChangeFade(
            FadeDirection::Out, ForPopup::No, false) };

        StageReplaceCommandOpt_t stageReplaceCommandOpt { STAGE_SETUP_COMMAND };

        FadeCommandOpt_t fadeInCommandOpt { MakeCommandForStateChangeFade(
            FadeDirection::In, ForPopup::No, false) };

        ExecuteCommandOpt_t executeCommandOpt { MakeCommandForStateChangeExecute(
            STAGE_SETUP_COMMAND.stage) };

        if ((HAS_CURRENT_STAGE == false) || (STAGE_SETUP_COMMAND.stage == stage::Stage::Intro)
            || (STAGE_SETUP_COMMAND.stage == stage::Stage::Test))
        {
            fadeOutCommandOpt = boost::none;
        }

        if (STAGE_SETUP_COMMAND.stage == stage::Stage::Exit)
        {
            stageReplaceCommandOpt = boost::none;
            fadeInCommandOpt = boost::none;
            executeCommandOpt = boost::none;
        }

        MakeAndAppendCommandMusic(commandVec, musicCommandOpt);
        MakeAndAppendCommandFade(commandVec, fadeOutCommandOpt);
        MakeAndAppendCommandReplaceStages(commandVec, stageReplaceCommandOpt);
        MakeAndAppendCommandFade(commandVec, fadeInCommandOpt);
        MakeAndAppendCommandExecute(commandVec, executeCommandOpt);

        VerifyNoneAreEmpty(__func__, commandVec);
        return commandVec;
    }

    void CommandFactory::VerifyNoneAreEmpty(
        const std::string & CREATING_FUNCTION_NAME, const CommandVec_t & CMD_VEC) const
    {
        const auto EMPTY_COMMAND { Command() };

        for (std::size_t i(0); i < CMD_VEC.size(); ++i)
        {
            if (CMD_VEC.at(i) == EMPTY_COMMAND)
            {
                M_HP_LOG_ERR(
                    CREATING_FUNCTION_NAME << " ended up making an empty command at position_index="
                                           << i);
            }
        }
    }

    void CommandFactory::MakeAndAppendCommandMusic(
        CommandVec_t & commandVec, const MusicCommandOpt_t & MUSIC_COMMAND_OPT) const
    {
        if (MUSIC_COMMAND_OPT)
        {
            Command command;
            command.music_opt = MUSIC_COMMAND_OPT;
            commandVec.emplace_back(command);
        }
    }

    void CommandFactory::MakeAndAppendCommandFade(
        CommandVec_t & commandVec, const FadeCommandOpt_t & FADE_COMMAND_OPT) const
    {
        if (FADE_COMMAND_OPT)
        {
            Command command;
            command.execute_opt = ExecuteCommand::MakeForFade(FADE_COMMAND_OPT.value());
            commandVec.emplace_back(command);
        }
    }

    void CommandFactory::MakeAndAppendCommandExecute(
        CommandVec_t & commandVec, const ExecuteCommandOpt_t & EXECUTE_COMMAND_OPT) const
    {
        if (EXECUTE_COMMAND_OPT)
        {
            Command command;
            command.execute_opt = EXECUTE_COMMAND_OPT;
            commandVec.emplace_back(command);
        }
    }

    void CommandFactory::MakeAndAppendCommandReplaceStages(
        CommandVec_t & commandVec, const StageReplaceCommandOpt_t & STAGE_SETUP_COMMAND_OPT) const
    {
        if (STAGE_SETUP_COMMAND_OPT)
        {
            Command command;
            command.stage_opt = STAGE_SETUP_COMMAND_OPT;
            commandVec.emplace_back(command);
        }
    }

    void CommandFactory::MakeAndAppendCommandReplacePopup(
        CommandVec_t & commandVec, const PopupReplaceCommand & POPUP_ADD_COMMAND) const
    {
        Command command;
        command.popup_replace_opt = POPUP_ADD_COMMAND;
        commandVec.emplace_back(command);
    }

    void CommandFactory::MakeAndAppendCommandRemovePopup(CommandVec_t & commandVec) const
    {
        Command command;
        command.will_remove_popup = true;
        commandVec.emplace_back(command);
    }

    const MusicCommandOpt_t
        CommandFactory::MakeCommandForStateChangeMusic(const stage::Stage::Enum STAGE) const
    {
        switch (STAGE)
        {
            case stage::Stage::Settings:
            {
                return MusicCommand(gui::music::All, gui::music::Theme);
            }
            case stage::Stage::Menu:
            {
                return MusicCommand(gui::music::Wind, gui::music::Theme);
            }
            case stage::Stage::Intro:
            {
                // do not start theme music because IntroStage will do that after some custom timing
                return boost::none;
            }
            case stage::Stage::Exit:
            {
                return MusicCommand::MakeToStopAllMusic();
            }
            case stage::Stage::Credits:
            {
                return MusicCommand(gui::music::All, gui::music::Credits);
            }
            case stage::Stage::Party:
            {
                return MusicCommand(gui::music::All, gui::music::PartyCreation);
            }
            case stage::Stage::Camp:
            {
                const auto MUSIC_VOLUME_MIN { misc::ConfigFile::Instance()->ValueOrDefault<float>(
                    "system-audio-music-volume-min") };

                return MusicCommand(gui::music::All, gui::music::Theme, MUSIC_VOLUME_MIN);
            }
            case stage::Stage::Character:
            {
                return MusicCommand(gui::music::All, gui::music::Wind);
            }
            case stage::Stage::Inventory:
            {
                return MusicCommand(gui::music::None, gui::music::Inventory);
            }
            case stage::Stage::Load:
            case stage::Stage::Previous:
            case stage::Stage::Adventure:
            case stage::Stage::None:
            case stage::Stage::Popup:
            case stage::Stage::Next:
            case stage::Stage::Save:
            case stage::Stage::Help:
            case stage::Stage::Inn:
            case stage::Stage::Combat:
            case stage::Stage::Test:
            case stage::Stage::Treasure:
            case stage::Stage::Count:
            default:
            {
                return boost::none;
            }
        }
    }

    const ExecuteCommandOpt_t
        CommandFactory::MakeCommandForStateChangeExecute(const stage::Stage::Enum STAGE) const
    {
        if ((STAGE == stage::Stage::Intro) || (STAGE == stage::Stage::Credits))
        {
            return ExecuteCommand::MakeDefaultWithMouseHidden();
        }
        else if (STAGE == stage::Stage::Exit)
        {
            return boost::none;
        }
        else
        {
            return ExecuteCommand();
        }
    }

    const FadeCommand CommandFactory::MakeCommandForStateChangeFade(
        const FadeDirection DIRECTION,
        const ForPopup IS_FOR_POPUP,
        const BoolOpt_t SET_WILL_DRAW_UNDER_POPUP_OPT) const
    {
        const auto FADE_SPEED = [&]() {
            if (DIRECTION == FadeDirection::In)
            {
                if (IS_FOR_POPUP == ForPopup::Yes)
                {
                    return misc::ConfigFile::Instance()->ValueOrDefault<float>(
                        "system-ui-stage-change-popup-fade-speed-in");
                }
                else
                {
                    return misc::ConfigFile::Instance()->ValueOrDefault<float>(
                        "system-ui-stage-change-non-popup-fade-speed-in");
                }
            }
            else
            {
                if (IS_FOR_POPUP == ForPopup::Yes)
                {
                    return misc::ConfigFile::Instance()->ValueOrDefault<float>(
                        "system-ui-stage-change-popup-fade-speed-out");
                }
                else
                {
                    return misc::ConfigFile::Instance()->ValueOrDefault<float>(
                        "system-ui-stage-change-non-popup-fade-speed-out");
                }
            }
        }();

        if (DIRECTION == FadeDirection::In)
        {
            return FadeCommand(
                DIRECTION, FADE_SPEED, SET_WILL_DRAW_UNDER_POPUP_OPT, sf::Color::Transparent);
        }
        else
        {
            sf::Color fadeToColor { sf::Color::Black };

            if (IS_FOR_POPUP == ForPopup::Yes)
            {
                fadeToColor.a = misc::ConfigFile::Instance()->ValueOrDefault<sf::Uint8>(
                    "system-ui-stage-change-popup-fade-alpha");
            }

            return FadeCommand(DIRECTION, FADE_SPEED, SET_WILL_DRAW_UNDER_POPUP_OPT, fadeToColor);
        }
    }
} // namespace game
} // namespace heroespath
