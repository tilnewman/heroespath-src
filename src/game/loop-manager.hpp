// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_LOOPMANAGER_HPP_INCLUDED
#define HEROESPATH_LOOPMANAGER_HPP_INCLUDED
//
// loop-manager.hpp
//  A class that controls the running game loop with a queue of commands
//
#include "game/phase-enum.hpp"
#include "misc/log-macros.hpp"
#include "misc/not-null.hpp"
#include "popup/popup-info.hpp"
#include "popup/popup-response-enum.hpp"
#include "popup/popup-stage-generic.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/loop-cmd-popup.hpp"
#include "sfml-util/loop-cmd.hpp"
#include "sfml-util/loop-state-enum.hpp"
#include "sfml-util/loop.hpp"
#include "sfml-util/music-enum.hpp"
#include "sfml-util/resolution.hpp"

#include <queue>
#include <string>

namespace heroespath
{

namespace creature
{
    class Creature;
    using CreaturePtr_t = misc::NotNull<Creature *>;
} // namespace creature

namespace game
{

    // Subsystem class that keeps track of the game state.
    class LoopManager
    {
    public:
        LoopManager(const LoopManager &) = delete;
        LoopManager(LoopManager &&) = delete;
        LoopManager & operator=(const LoopManager &) = delete;
        LoopManager & operator=(LoopManager &&) = delete;

        LoopManager();
        ~LoopManager();

        static misc::NotNull<LoopManager *> Instance();
        static void Acquire();
        static void Release();
        static void Initialize();

        static void SetStartupStage(const std::string & STARTUP_STAGE_NAME)
        {
            startupStageName_ = STARTUP_STAGE_NAME;
        }

        sfml_util::LoopState::Enum GetState() const { return state_; }
        sfml_util::LoopState::Enum GetPrevState() const { return prevState_; }

        popup::ResponseTypes::Enum GetPopupResponse() const { return popupResponse_; }

        std::size_t GetPopupSelection() const { return popupSelection_; }

        void ClearPopupResponse()
        {
            popupResponse_ = popup::ResponseTypes::None;
            popupSelection_ = 0;
        }

        game::Phase::Enum GetPhase() const;

        bool IsKeyPressed(const sf::Keyboard::Key KEY) const { return loop_.IsKeyPressed(KEY); }

        bool Execute();

        template <typename PopupType_t>
        void PopupWaitBeginSpecific(
            const sfml_util::PopupCallback_t::IHandlerPtr_t POPUP_HANDLER_PTR,
            const popup::PopupInfo & POPUP_INFO)
        {
            popupResponse_ = popup::ResponseTypes::None;
            popupSelection_ = 0;
            TransitionTo_Popup<PopupType_t>(POPUP_HANDLER_PTR, POPUP_INFO);
        }

        void PopupWaitBegin(
            const sfml_util::PopupCallback_t::IHandlerPtr_t POPUP_HANDLER_PTR,
            const popup::PopupInfo & POPUP_INFO)
        {
            PopupWaitBeginSpecific<popup::PopupStageGeneric>(POPUP_HANDLER_PTR, POPUP_INFO);
        }

        void PopupWaitEnd(
            const popup::ResponseTypes::Enum RESPONSE, const std::size_t SELECTION = 0);

        void TransitionTo_Previous(const bool WILL_ADVANCE_TURN = false);

        sfml_util::DisplayChangeResult::Enum ChangeResolution(
            const sfml_util::IStagePtr_t CURRENT_ISTAGE_PTR,
            const sfml_util::PopupCallback_t::IHandlerPtr_t POPUP_HANDLER_PTR,
            const sfml_util::Resolution & NEW_RES,
            const unsigned ANTIALIAS_LEVEL);

        void SetTransitionBeforeFade(const sfml_util::LoopState::Enum E) { stateBeforeFade_ = E; }

        void HandleTransitionBeforeFade();

        void FakeMouseClick(const sf::Vector2f &);

        void AddStage(const sfml_util::IStagePtr_t ISTAGE_PTR) { loop_.AddStage(ISTAGE_PTR); }

        bool IsFading() const { return loop_.IsFading(); }

        bool GetIgnoreMouse() const { return loop_.GetIgnoreMouse(); }

        void SetIgnoreMouse(const bool B) { loop_.SetIgnoreMouse(B); }

        void TestingStrAppend(const std::string & S) { loop_.TestingStrAppend(S); }

        void TestingStrIncrement(const std::string & S) { loop_.TestingStrIncrement(S); }

        void
            TestingImageSet(const std::string & PATH_STR, const bool WILL_CHECK_FOR_OUTLINE = false)
        {
            loop_.TestingImageSet(PATH_STR, WILL_CHECK_FOR_OUTLINE);
        }

        void SetExitSuccess(const bool WAS_SUCCESS) { exitSuccess_ = WAS_SUCCESS; }

        void TransitionTo_Exit();
        void TransitionTo_Credits();
        void TransitionTo_MainMenu();
        void TransitionTo_Settings();
        void TransitionTo_CharacterCreation();
        void TransitionTo_PartyCreation();
        void TransitionTo_Inn();
        void TransitionTo_Camp();
        void TransitionTo_LoadGameMenu();
        void TransitionTo_Combat(const bool WILL_ADVANCE_TURN);
        void TransitionTo_Test();
        void TransitionTo_Treasure();
        void TransitionTo_Adventure();

        void TransitionTo_Inventory(
            const creature::CreaturePtr_t TURN_CREATURE_PTR,
            const creature::CreaturePtr_t INVENTORY_CREATURE_PTR,
            const game::Phase::Enum CURRENT_PHASE);

        sfml_util::Loop & CommandLoopAccess(const sfml_util::LoopCmd * const) { return loop_; }

        void TransitionTo(const sfml_util::LoopState::Enum, const bool WILL_ADVANCE_TURN = false);

    private:
        void TransitionTo_Intro();

        template <typename PopupType_t>
        void TransitionTo_Popup(
            const sfml_util::PopupCallback_t::IHandlerPtr_t POPUP_HANDLER_PTR,
            const popup::PopupInfo & POPUP_INFO)
        {
            M_HP_LOG_DBG(
                "LoopManager::TransitionTo_Popup(" << POPUP_INFO.Name() << ") "
                                                   << cmdQueueVec_.size());
            CommandQueueClear();
            loop_.Exit();

            loop_.AssignPopupCallbackHandlerInfo(POPUP_HANDLER_PTR, POPUP_INFO);

            cmdQueueVec_.emplace_back(
                std::make_unique<sfml_util::LoopCmd_ExitAfterKeypress>(false));

            cmdQueueVec_.emplace_back(
                std::make_unique<sfml_util::LoopCmd_ExitAfterMouseclick>(false));

            cmdQueueVec_.emplace_back(
                std::make_unique<sfml_util::LoopCmd_StateChange>(sfml_util::LoopState::Popup));

            cmdQueueVec_.emplace_back(std::make_unique<sfml_util::LoopCmd_FadeOut>(
                popup::PopupManager::Color_Fade(), popup::PopupManager::SpeedMult_Fade(), true));

            cmdQueueVec_.emplace_back(std::make_unique<sfml_util::LoopCmd_Execute>());

            cmdQueueVec_.emplace_back(
                std::make_unique<sfml_util::LoopCmd_AddStage_Popup_Specific<PopupType_t>>(
                    loop_, POPUP_INFO));

            cmdQueueVec_.emplace_back(std::make_unique<sfml_util::LoopCmd_Execute>());
        }

        void TransitionFrom_Popup();

        enum TransOpt : unsigned
        {
            None = 0,
            ClearQueue = 1 << 0,
            MouseIgnore = 1 << 1,
            MouseRestore = 1 << 2,
            FinalExecute = 1 << 3,
            All = ClearQueue | MouseIgnore | MouseRestore | FinalExecute
        };

        void TransitionHelper(
            const TransOpt OPTIONS,
            const sfml_util::LoopState::Enum NEW_STATE,
            sfml_util::LoopCmdUPtr_t && loopCmdUPtr,
            const sfml_util::music::Enum MUSIC_TO_STOP = sfml_util::music::All,
            const sfml_util::music::Enum MUSIC_TO_START = sfml_util::music::None);

        void CommandQueueClear();

    private:
        static std::unique_ptr<LoopManager> instanceUPtr_;
        static std::string startupStageName_;
        //
        sfml_util::LoopState::Enum state_;
        sfml_util::LoopCmdUVec_t cmdQueueVec_;
        sfml_util::Loop loop_;
        popup::ResponseTypes::Enum popupResponse_;
        std::size_t popupSelection_;
        sfml_util::LoopState::Enum prevState_;
        sfml_util::LoopState::Enum prevSettingsState_;
        sfml_util::LoopState::Enum stateBeforeFade_;
        bool exitSuccess_;
    };

} // namespace game
} // namespace heroespath

#endif // HEROESPATH_LOOPMANAGER_HPP_INCLUDED
