#ifndef SFMLUTIL_LOOP_INCLUDED
#define SFMLUTIL_LOOP_INCLUDED
//
// loop.hpp
//  Code to manage an event/draw loop.
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/iloop.hpp"
#include "sfml-util/fade.hpp"

#include "game/loop-state-enum.hpp"

#include <memory>
#include <string>
#include <vector>
#include <list>


namespace sfml_util
{

    //forward declaration
    class IStage;
    using IStageSPtr_t = std::shared_ptr<IStage>;
    using IStageSVec_t = std::vector<IStageSPtr_t>;


    //A type that manages an event/draw loop
    class Loop : public ILoop
    {
        //prevent copy construction
        Loop(const Loop &) =delete;

        //prevent copy assignment
        Loop & operator=(const Loop &) =delete;

    public:
        explicit Loop(const std::string & NAME);
        virtual ~Loop();

        inline virtual void Exit() { willExit_ = true; }

        virtual void AddStage(IStageSPtr_t & stageSPtr);
        virtual void RemoveAllStages();

        //returns false if the window closed or some other fatal event stopped the loop
        virtual bool Execute();

        virtual void FadeOut(const sf::Color & FADE_TO_COLOR,
                             const float       SPEED_MULT     = 200.0f,
                             const bool        WILL_HOLD_FADE = false);

        virtual void FadeIn(const sf::Color & FADE_FROM_COLOR,
                            const float       SPEED_MULT     = 200.0f,
                            const bool        WILL_HOLD_FADE = false);

        inline virtual void SetPopup(IStageSPtr_t & popupStageSPtr)     { popupStageSPtr_ = popupStageSPtr; }
        inline virtual void SetHoldTime(const float SECONDS)            { holdTimeCounter_ = 0.0f; holdTime_ = SECONDS; }

        inline void SetWillExitAfterFade(const bool B)                  { willExitAfterFade_ = B; }
        inline virtual void SetWillHoldFade(const bool NEW_VAL)         { willHoldFade_ = NEW_VAL; }

        inline virtual void SetMouseVisibility(const bool IS_VISIBLE)   { winSPtr_->setMouseCursorVisible(IS_VISIBLE); }

        inline virtual void SetState(const game::LoopState::Enum E)   { state_ = E; }
        inline virtual game::LoopState::Enum  GetState() const        { return state_; }

        virtual void ConsumeEvents();

        inline virtual void SetWillExitOnKeypress(const bool B)     { willExitOnKeypress_ = B; }
        inline virtual void SetWillExitOnMouseclick(const bool B)   { willExitOnMouseclick_ = B; }

        virtual void RemoveFocus();
        virtual bool SetFocus(const gui::IGuiEntitySPtr_t & ENTITY_SPTR);//returns true if a stage was found owning the GuiEntity

        inline virtual bool GetIgnoreMouse() const                  { return willIgnoreMouse_; }
        inline virtual void SetIgnoreMouse(const bool B)            { willIgnoreMouse_ = B; }

        inline virtual bool GetIgnoreKeystrokes() const             { return willIgnoreKeystrokes_; }
        inline virtual void SetIgnoreKeystrokes(const bool B)       { willIgnoreKeystrokes_ = B; }

        virtual void AssignPopupCallbackHandlerInfo(game::callback::IPopupHandler_t * const HANDLER_PTR,
                                                    const game::PopupInfo &                 POPUP_INFO);

        virtual void FakeMouseClick(const sf::Vector2f & MOUSE_POS_V);

        inline virtual bool IsFading() const { return continueFading_; }

        virtual void TestingStrAppend(const std::string &);
        virtual void TestingStrIncrement(const std::string &);
        virtual void TestingImageSet(const sfml_util::TextureSPtr_t &);

    protected:
        virtual void LogFrameRate();
        virtual bool ProcessOneSecondTasks();
        virtual void ProcessMouseHover();
        virtual void ProcessFader();
        virtual void ProcessPopup();
        virtual void ProcessEvents();
        virtual void ProcessKeyStrokes(const sf::Event EVENT);
        virtual void ProcessMouseMove();
        virtual void ProcessMouseButtonLeftPressed(const sf::Vector2f &);
        virtual void ProcessMouseButtonLeftReleased(const sf::Vector2f &);
        virtual void ProcessMouseWheelRoll(const sf::Event EVENT);
        virtual void ProcessPopupCallback();
        virtual void ProcessTimeUpdate();
        virtual void ProcessDrawing();
        virtual void PerformNextTest();
        virtual void ProcessHoldTime();
        virtual void ProcessScreenshot();

    protected:
        static const float NO_HOLD_TIME_;
        static const float INVALID_MUSIC_FADE_VALUE_;
        //
        const std::string     NAME_;
        IStageSVec_t          stageSVec_;
        sf::Clock             clock_;
        float                 oneSecondTimerSec_;
        WinSPtr_t             winSPtr_;
        Fade                  fader_;
        bool                  willHoldFade_;
        bool                  continueFading_;
        bool                  willExitAfterFade_;
        bool                  willExit_;
        IStageSPtr_t          popupStageSPtr_;
        float                 elapsedTimeSec_;
        bool                  fatalExitEvent_;
        float                 holdTime_;
        float                 holdTimeCounter_;
        bool                  willExitOnKeypress_;
        bool                  willExitOnMouseclick_;
        gui::IGuiEntitySPtr_t entityWithFocusSPtr_;
        bool                  willIgnoreMouse_;
        bool                  willIgnoreKeystrokes_;
        game::PopupInfo popupInfo_;
        bool                  hasFadeStarted_;
        sf::Event::EventType  prevEventType_;
        sf::Keyboard::Key     prevKeyPressed_;
        sf::Vector2f          mousePosV_;
        bool                  isMouseHovering_;
        bool                  takeScreenshot_;
        game::callback::IPopupHandler_t * popupCallbackPtr_;
        game::LoopState::Enum state_;
        std::vector<float>    frameRateVec_;
        std::size_t           frameRateSampleCount_;
        bool                  willLogFrameRate_;
    };

    using LoopSPtr_t = std::shared_ptr<Loop>;
    using LoopSVec_t = std::vector<LoopSPtr_t>;

}
#endif //SFMLUTIL_LOOP_INCLUDED
