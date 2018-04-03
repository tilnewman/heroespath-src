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
#ifndef HEROESPATH_LOOPMANAGER_HPP_INCLUDED
#define HEROESPATH_LOOPMANAGER_HPP_INCLUDED
//
// loop-manager.hpp
//  A class that controls the running game loop with a queue of commands
//
#include "game/phase-enum.hpp"
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
#include "sfml-util/sfml-graphics.hpp"

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

    // Singleton class that keeps track of the game state
    class LoopManager
    {
    public:
        LoopManager(const LoopManager &) = delete;
        LoopManager(LoopManager &&) = delete;
        LoopManager & operator=(const LoopManager &) = delete;
        LoopManager & operator=(LoopManager &&) = delete;

    public:
        LoopManager();
        ~LoopManager();

        static LoopManager * Instance();
        static void Acquire();
        static void Release();
        static void Initialize();

        static void SetStartupStage(const std::string & STARTUP_STAGE_NAME)
        {
            startupStage_ = STARTUP_STAGE_NAME;
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
            popup::IPopupHandler_t * const HANDLER_PTR, const popup::PopupInfo & POPUP_INFO)
        {
            popupResponse_ = popup::ResponseTypes::None;
            popupSelection_ = 0;
            TransitionTo_Popup<PopupType_t>(HANDLER_PTR, POPUP_INFO);
        }

        void PopupWaitBegin(
            popup::IPopupHandler_t * const HANDLER_PTR, const popup::PopupInfo & POPUP_INFO)
        {
            PopupWaitBeginSpecific<popup::PopupStageGeneric>(HANDLER_PTR, POPUP_INFO);
        }

        void PopupWaitEnd(
            const popup::ResponseTypes::Enum RESPONSE, const std::size_t SELECTION = 0);

        void TransitionTo_Previous(const bool WILL_ADVANCE_TURN = false);

        sfml_util::DisplayChangeResult::Enum ChangeResolution(
            const sfml_util::IStagePtr_t CURRENT_ISTAGE_PTR,
            popup::IPopupHandler_t * const HANDLER_PTR,
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

        void TestingImageSet(
            const sf::Texture & TEXTURE,
            const bool WILL_CHECK_FOR_OUTLINE = false,
            const std::string & CATEGORY_NAME = "",
            const std::string & TYPE_NAME = "",
            const std::string & PATH = "")
        {
            loop_.TestingImageSet(TEXTURE, WILL_CHECK_FOR_OUTLINE, CATEGORY_NAME, TYPE_NAME, PATH);
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

        sfml_util::Loop & CommandLoopAccess(const sfml_util::ILoopCmd *) { return loop_; }

    private:
        void TransitionTo_Intro();

        template <typename PopupType_t>
        void TransitionTo_Popup(
            popup::IPopupHandler_t * const HANDLER_PTR, const popup::PopupInfo & POPUP_INFO)
        {
            CommandQueueClear();
            loop_.Exit();

            loop_.AssignPopupCallbackHandlerInfo(HANDLER_PTR, POPUP_INFO);
            cmdQueue_.push(std::make_shared<sfml_util::LoopCmd_ExitAfterKeypress>(false));
            cmdQueue_.push(std::make_shared<sfml_util::LoopCmd_ExitAfterMouseclick>(false));

            cmdQueue_.push(
                std::make_shared<sfml_util::LoopCmd_StateChange>(sfml_util::LoopState::Popup));

            cmdQueue_.push(std::make_shared<sfml_util::LoopCmd_FadeOut>(
                popup::PopupManager::Color_Fade(), popup::PopupManager::SpeedMult_Fade(), true));

            cmdQueue_.push(std::make_shared<sfml_util::LoopCmd_Execute>());

            cmdQueue_.push(
                std::make_shared<sfml_util::LoopCmd_AddStage_Popup_Specific<PopupType_t>>(
                    loop_, POPUP_INFO));

            cmdQueue_.push(std::make_shared<sfml_util::LoopCmd_Execute>());
        }

        void TransitionFrom_Popup();

        void TransitionTo(const sfml_util::LoopState::Enum, const bool WILL_ADVANCE_TURN = false);

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
            const sfml_util::ILoopCmdSPtr_t & ADD_STAGE_LOOP_CMD,
            const sfml_util::music::Enum MUSIC_TO_STOP = sfml_util::music::All,
            const sfml_util::music::Enum MUSIC_TO_START = sfml_util::music::None);

        void CommandQueueClear();

    private:
        static std::unique_ptr<LoopManager> instanceUPtr_;
        static std::string startupStage_;
        //
        sfml_util::LoopState::Enum state_;
        std::queue<sfml_util::ILoopCmdSPtr_t> cmdQueue_;
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
