#ifndef GAME_LOOPMANAGER_INCLUDED
#define GAME_LOOPMANAGER_INCLUDED
//
// loop-manager.hpp
//  A class that controls the running game loop with a queue of commands
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/loop-cmd.hpp"
#include "sfml-util/cmd-queue.hpp"
#include "sfml-util/resolution.hpp"
#include "sfml-util/music-enum.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/response-enum.hpp"

#include "game/loop-state-enum.hpp"
#include "game/phase-enum.hpp"


namespace game
{

    //forward declarations
    namespace creature
    {
        class Creature;
        using CreaturePtr_t = Creature *;
    }

    class PopupInfo;


    //types required by singletom implementation
    class LoopManager;
    using LoopManagerSPtr_t = std::shared_ptr<LoopManager>;


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
        virtual ~LoopManager();
        static LoopManagerSPtr_t Instance();

        static void SetStartupStage(const std::string & STARTUP_STAGE_NAME);

        inline LoopState::Enum GetState() const                     { return state_; }
        inline LoopState::Enum GetPrevState() const                 { return prevState_; }
        inline sfml_util::Response::Enum GetPopupResponse() const   { return popupResponse_; }
        inline std::size_t GetPopupSelection() const                { return popupSelection_; }
        inline void ClearPopupResponse()                            { popupResponse_ = sfml_util::Response::None; popupSelection_ = 0; }

        Phase::Enum GetPhase() const;

        void Execute();

        void PopupWaitBegin(callback::IPopupHandler_t * const HANDLER_PTR,
                            const PopupInfo &                 POPUP_INFO_SPTR);

        void PopupWaitEnd(const sfml_util::Response::Enum RESPONSE, const std::size_t SELECTION = 0);

        void Goto_Intro();
        void Goto_Credits();
        void Goto_Exit();
        void Goto_MainMenu();
        void Goto_Settings();
        void Goto_Previous();
        void Goto_CharacterCreation();
        void Goto_PartyCreation();
        void Goto_Inn();
        void Goto_Camp();
        void Goto_LoadGameMenu();
        void Goto_Combat();
        void Goto_Test();
        void Goto_Inventory(creature::CreaturePtr_t);

        sfml_util::DisplayChangeResult::Enum ChangeResolution(sfml_util::IStage * const         currentStagePtr_,
                                                              callback::IPopupHandler_t * const HANDLER_PTR,
                                                              const sfml_util::Resolution &     NEW_RES,
                                                              const unsigned                    ANTIALIAS_LEVEL);

        inline void SetTransitionBeforeFade(const LoopState::Enum E) { stateBeforeFade_ = E; }

        void HandleTransitionBeforeFade();

        void FakeMouseClick(const sf::Vector2f &);

        inline void AddStage(sfml_util::IStageSPtr_t & stageSPtr) { currentLoopSPtr_->AddStage(stageSPtr); }
        inline bool IsFading() const                              { return currentLoopSPtr_->IsFading(); }
        inline bool GetIgnoreMouse() const                        { return currentLoopSPtr_->GetIgnoreMouse(); }
        inline void SetIgnoreMouse(const bool B)                  { currentLoopSPtr_->SetIgnoreMouse(B); }

        inline void TestingStrAppend(const std::string & S)             { currentLoopSPtr_->TestingStrAppend(S); }
        inline void TestingStrIncrement(const std::string & S)          { currentLoopSPtr_->TestingStrIncrement(S); }
        inline void TestingImageSet(const sfml_util::TextureSPtr_t & T) { currentLoopSPtr_->TestingImageSet(T); }

    protected:
        void TransitionTo_Intro();

        void TransitionTo_Popup(callback::IPopupHandler_t * const HANDLER_PTR,
                                const PopupInfo &                 POPUP_INFO);

        void TransitionFrom_Popup();
        void TransitionTo_Exit();
        void TransitionTo_Credits();
        void TransitionTo_MainMenu();
        void TransitionTo_Settings();
        void TransitionTo_CharacterCreation();
        void TransitionTo_PartyCreation();
        void TransitionTo_Inn();
        void TransitionTo_Camp();
        void TransitionTo_LoadGameMenu();
        void TransitionTo_Combat();
        void TransitionTo_Test();
        void TransitionTo_Inventory(creature::CreaturePtr_t);
        void TransitionTo(const LoopState::Enum);

        void TransitionHelper(const bool                      WILL_CLEAR_QUEUE,
                              const bool                      WILL_EXIT_LOOP,
                              const bool                      WILL_IGNORE_MOUSE,
                              const bool                      WILL_RESTORE_MOUSE,
                              const bool                      WILL_FINAL_EXECUTE,
                              const LoopState::Enum           NEW_STATE,
                              const sfml_util::ILoopCmdSPtr_t ADD_STAGE_LOOP_CMD,
                              const sfml_util::music::Enum    MUSIC_TO_STOP  = sfml_util::music::All,
                              const sfml_util::music::Enum    MUSIC_TO_START = sfml_util::music::None);

    private:
        static LoopManagerSPtr_t  instanceSPtr_;
        static std::string        startupStage_;
        //
        LoopState::Enum           state_;
        sfml_util::CommandQueue   cmdQueue_;
        sfml_util::ILoopSPtr_t    currentLoopSPtr_;
        sfml_util::Response::Enum popupResponse_;
        std::size_t               popupSelection_;
        LoopState::Enum           prevState_;
        LoopState::Enum           prevSettingsState_;
        LoopState::Enum           stateBeforeFade_;
    };

}
#endif //GAME_LOOPMANAGER_INCLUDED