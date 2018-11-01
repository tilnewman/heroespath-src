// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GAME_GAME_CONTROLLER_HPP_INCLUDED
#define HEROESPATH_GAME_GAME_CONTROLLER_HPP_INCLUDED
//
// game-controller.hpp
//
#include "game/active-stages.hpp"
#include "game/command-factory.hpp"
#include "game/command-queue.hpp"
#include "game/command.hpp"
#include "game/phase-enum.hpp"
#include "game/stage-tracker.hpp"
#include "game/status.hpp"
#include "misc/not-null.hpp"
#include "popup/popup-response-enum.hpp"
#include "sfml-util/display-change-result.hpp"
#include "stage/stage-enum.hpp"
#include "stage/stage-setup-packet.hpp"

#include <string>

namespace heroespath
{

namespace gui
{
    struct Resolution;
}

namespace popup
{
    class PopupInfo;
}

namespace game
{

    // Controls all high level changes in how the game plays and controls the lifetime of all
    // Stages.
    class GameController
    {
    public:
        GameController();
        ~GameController();

        GameController(const GameController &) = delete;
        GameController(GameController &&) = delete;
        GameController & operator=(const GameController &) = delete;
        GameController & operator=(GameController &&) = delete;

        static misc::NotNull<GameController *> Instance();
        static void Acquire();
        static void Release();
        static void Initialize();

        static void SetStartupStage(const std::string & STARTUP_STAGE_NAME)
        {
            startupStageName_ = STARTUP_STAGE_NAME;
        }

        const std::string GetStageName() const
        {
            return stage::Stage::ToStringNoThrow(stageTracker_.GetCurrent());
        }

        Phase::Enum GetPhase() const { return stageTracker_.GetPhase(); }

        void PlayGame();

        void SpawnPopup(
            const gui::PopupCallback_t::IHandlerPtr_t & POPUP_HANDLER_PTR,
            const popup::PopupInfo & POPUP_INFO);

        void RemovePopup(const popup::ResponseTypes::Enum TYPE, const std::size_t SELECTION = 0);
        void TransitionTo(const stage::Stage::Enum NEW_STAGE);
        void TransitionTo(const stage::SetupPacket & SETUP_PACKET);

        const gui::DisplayChangeResult ChangeResolution(
            const gui::PopupCallback_t::IHandlerPtr_t & POPUP_HANDLER_PTR,
            const gui::Resolution & NEW_RES,
            const unsigned ANTIALIAS_LEVEL);

        // TODO TEMP REMOVE remove this crap once all testing is in unit tests
        void TestingStrAppend(const std::string & MESSAGE);
        void TestingStrIncrement(const std::string & MESSAGE);

        void TestingImageSet(
            const std::string & PATH_STR, const bool WILL_CHECK_FOR_OUTLINE = false);

    private:
        void PreTaskLogs(
            const std::string & NAME_OF_TASK_ABOUT_TO_EXECUTE, const Command & COMMAND) const;
        void ExecuteGameCommand(const Command &);
        void ExecuteGameCommandFade(const FadeCommand &);
        void RequestLoopExit() { status_.LoopStopRequest(); }

        // change stages now
        void StageChangeActualPopupReplace(
            const popup::PopupInfo & POPUP_INFO,
            const gui::PopupCallback_t::IHandlerPtr_t & POPUP_HANDLER_PTR);

        void StageChangeActualPopupRemove();

        void StageChangeActualNonPopupReplace(const stage::SetupPacket & SETUP_PACKET);

        // post game commands to change stages later
        void StageChangePostPopupSpawn(const PopupReplaceCommand & POPUP_ADD_COMMAND);

        void StageChangePostPopupRemove(
            const popup::ResponseTypes::Enum TYPE, const std::size_t SELECTION);

        void StageChangePostNonPopupReplace(const StageReplaceCommand & STAGE_SETUP_COMMAND);

    private:
        static std::unique_ptr<GameController> instanceUPtr_;
        static std::string startupStageName_;

        Status status_;
        StageTracker stageTracker_;
        ActiveStages activeStages_;
        CommandFactory commandFactory_;
        CommandQueue commandQueue_;
        ExecuteCommand prePopupExecuteCommandWithoutFade_;
    };

} // namespace game
} // namespace heroespath

#endif // HEROESPATH_GAME_GAME_CONTROLLER_HPP_INCLUDED
