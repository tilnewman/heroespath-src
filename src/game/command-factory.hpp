// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GAME_COMMAND_FACTORY_HPP_INCLUDED
#define HEROESPATH_GAME_COMMAND_FACTORY_HPP_INCLUDED
//
// command-factory.hpp
//
#include "game/command.hpp"
#include "stage/stage-enum.hpp"

#include <vector>

namespace heroespath
{
namespace game
{

    class CommandFactory
    {
    public:
        const CommandVec_t MakeCommandsForPopupSpawn(
            const PopupReplaceCommand & POPUP_ADD_COMMAND, const bool IS_ALREADY_POPUP) const;

        const CommandVec_t
            MakeCommandsForPopupRemove(const ExecuteCommand & EXECUTE_COMMAND_BEFORE_CHANGE) const;

        const CommandVec_t MakeCommandsForNonPopupStageReplace(
            const StageReplaceCommand & STAGE_SETUP_COMMAND, const bool HAS_CURRENT_STAGE) const;

    private:
        void VerifyNoneAreEmpty(
            const std::string & CREATING_FUNCTION_NAME, const CommandVec_t &) const;

        enum class ForPopup : bool
        {
            No = false,
            Yes = true
        };

        void MakeAndAppendCommandMusic(
            CommandVec_t &, const MusicCommandOpt_t & MUSIC_COMMAND_OPT) const;

        void MakeAndAppendCommandFade(
            CommandVec_t &, const FadeCommandOpt_t & FADE_COMMAND_OPT) const;

        void MakeAndAppendCommandExecute(
            CommandVec_t &, const ExecuteCommandOpt_t & EXECUTE_COMMAND_OPT) const;

        void MakeAndAppendCommandReplaceStages(
            CommandVec_t &, const StageReplaceCommandOpt_t & STAGE_SETUP_COMMAND_OPT) const;

        void MakeAndAppendCommandReplacePopup(
            CommandVec_t &, const PopupReplaceCommand & POPUP_ADD_COMMAND) const;

        void MakeAndAppendCommandRemovePopup(CommandVec_t &) const;

        const MusicCommandOpt_t MakeCommandForStateChangeMusic(const stage::Stage::Enum) const;

        const ExecuteCommandOpt_t MakeCommandForStateChangeExecute(const stage::Stage::Enum) const;

        const FadeCommand MakeCommandForStateChangeFade(
            const FadeDirection FADE_DIRECTION,
            const ForPopup IS_FOR_POPUP,
            const BoolOpt_t & SET_WILL_DRAW_UNDER_POPUP_OPT) const;
    };

} // namespace game
} // namespace heroespath

#endif // HEROESPATH_GAME_COMMAND_FACTORY_HPP_INCLUDED
