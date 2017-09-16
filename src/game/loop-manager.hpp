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
#ifndef GAME_LOOPMANAGER_INCLUDED
#define GAME_LOOPMANAGER_INCLUDED
//
// loop-manager.hpp
//  A class that controls the running game loop with a queue of commands
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/loop-cmd.hpp"
#include "sfml-util/resolution.hpp"
#include "sfml-util/music-enum.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/response-enum.hpp"
#include "sfml-util/loop.hpp"

#include "game/loop-state-enum.hpp"
#include "game/phase-enum.hpp"

#include <queue>
#include <string>


namespace game
{

    //forward declarations
    namespace creature
    {
        class Creature;
        using CreaturePtr_t = Creature *;
    }

    class PopupInfo;


    //Singleton class that keeps track of the game state
    class LoopManager
    {
        //prevent copy construction
        LoopManager(const LoopManager &) =delete;

        //prevent copy assignment
        LoopManager & operator=(const LoopManager &) =delete;

        //prevent non-singleton construction
        LoopManager();

    public:
        ~LoopManager();

        static LoopManager * Instance();
        static void Acquire();
        static void Release();

        inline static void SetStartupStage(const std::string & STARTUP_STAGE_NAME)
        {
            startupStage_ = STARTUP_STAGE_NAME;
        }

        inline LoopState::Enum GetState() const                     { return state_; }
        inline LoopState::Enum GetPrevState() const                 { return prevState_; }
        inline sfml_util::Response::Enum GetPopupResponse() const   { return popupResponse_; }
        inline std::size_t GetPopupSelection() const                { return popupSelection_; }

        inline void ClearPopupResponse()
        {
            popupResponse_ = sfml_util::Response::None;
            popupSelection_ = 0;
        }

        Phase::Enum GetPhase() const;

        bool Execute();

        void PopupWaitBegin(callback::IPopupHandler_t * const HANDLER_PTR,
                            const PopupInfo &                 POPUP_INFO_SPTR);

        void PopupWaitEnd(const sfml_util::Response::Enum RESPONSE,
                          const std::size_t SELECTION = 0);

        void TransitionTo_Previous(const bool WILL_ADVANCE_TURN = false);

        sfml_util::DisplayChangeResult::Enum ChangeResolution(
            sfml_util::IStage * const         currentStagePtr_,
            callback::IPopupHandler_t * const HANDLER_PTR,
            const sfml_util::Resolution &     NEW_RES,
            const unsigned                    ANTIALIAS_LEVEL);

        inline void SetTransitionBeforeFade(const LoopState::Enum E)
        {
            stateBeforeFade_ = E;
        }

        void HandleTransitionBeforeFade();

        void FakeMouseClick(const sf::Vector2f &);

        inline void AddStage(sfml_util::IStagePtr_t stagePtr)
        {
            loop_.AddStage(stagePtr);
        }

        inline bool IsFading() const
        {
            return loop_.IsFading();
        }

        inline bool GetIgnoreMouse() const
        {
            return loop_.GetIgnoreMouse();
        }

        inline void SetIgnoreMouse(const bool B)
        {
            loop_.SetIgnoreMouse(B);
        }

        inline void TestingStrAppend(const std::string & S)
        {
            loop_.TestingStrAppend(S);
        }

        inline void TestingStrIncrement(const std::string & S)
        {
            loop_.TestingStrIncrement(S);
        }

        inline void TestingImageSet(const sf::Texture & T)
        {
            loop_.TestingImageSet(T);
        }

        inline void SetExitSuccess(const bool WAS_SUCCESS) { exitSuccess_ = WAS_SUCCESS; }

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

        void TransitionTo_Inventory(const creature::CreaturePtr_t TURN_CREATURE_PTR,
            const creature::CreaturePtr_t INVENTORY_CREATURE_PTR,
            const Phase::Enum             CURRENT_PHASE);

        inline sfml_util::Loop & CommandLoopAccess(const sfml_util::ILoopCmd *)
        {
            return loop_;
        }

    private:
        void TransitionTo_Intro();

        void TransitionTo_Popup(callback::IPopupHandler_t * const HANDLER_PTR,
                                const PopupInfo &                 POPUP_INFO);

        void TransitionFrom_Popup();
        
        void TransitionTo(const LoopState::Enum, const bool WILL_ADVANCE_TURN = false);

        void TransitionHelper(
            const bool                        WILL_CLEAR_QUEUE,
            const bool                        WILL_EXIT_LOOP,
            const bool                        WILL_IGNORE_MOUSE,
            const bool                        WILL_RESTORE_MOUSE,
            const bool                        WILL_FINAL_EXECUTE,
            const LoopState::Enum             NEW_STATE,
            const sfml_util::ILoopCmdSPtr_t & ADD_STAGE_LOOP_CMD,
            const sfml_util::music::Enum      MUSIC_TO_STOP  = sfml_util::music::All,
            const sfml_util::music::Enum      MUSIC_TO_START = sfml_util::music::None);

        void CommandQueueClear();

    private:
        static std::unique_ptr<LoopManager> instanceUPtr_;
        static std::string startupStage_;
        //
        LoopState::Enum           state_;
        std::queue<sfml_util::ILoopCmdSPtr_t> cmdQueue_;
        sfml_util::Loop           loop_;
        sfml_util::Response::Enum popupResponse_;
        std::size_t               popupSelection_;
        LoopState::Enum           prevState_;
        LoopState::Enum           prevSettingsState_;
        LoopState::Enum           stateBeforeFade_;
        bool                      exitSuccess_;
    };

}
#endif //GAME_LOOPMANAGER_INCLUDED
