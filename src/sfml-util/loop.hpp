// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_LOOP_INCLUDED
#define HEROESPATH_SFMLUTIL_LOOP_INCLUDED
//
// loop.hpp
//  Code to manage an event/draw loop.
//
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"
#include "popup/popup-info.hpp"
#include "sfml-util/callback.hpp"
#include "sfml-util/entity.hpp"
#include "sfml-util/fade.hpp"
#include "sfml-util/loop-state-enum.hpp"

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace sfml_util
{

    class IStage;
    using IStagePtr_t = misc::NotNull<IStage *>;
    using IStagePtrOpt_t = boost::optional<IStagePtr_t>;
    using IStagePVec_t = std::vector<IStagePtr_t>;

    // A type that manages an event/draw loop
    class Loop
    {
    public:
        Loop(const Loop &) = delete;
        Loop(Loop &&) = delete;
        Loop & operator=(const Loop &) = delete;
        Loop & operator=(Loop &&) = delete;

    public:
        explicit Loop(const std::string & NAME);
        ~Loop();

        void Exit() { willExit_ = true; }

        void AddStage(const IStagePtr_t ISTAGE_PTR) { stagePVec_.emplace_back(ISTAGE_PTR); }

        void FreeAllStages();
        void FreePopupStage();

        // returns false if the window closed or some other fatal event stopped the loop
        bool Execute();

        void FadeOut(
            const sf::Color & FADE_TO_COLOR,
            const float SPEED_MULT = 200.0f,
            const bool WILL_HOLD_FADE = false);

        void FadeIn(
            const sf::Color & FADE_FROM_COLOR,
            const float SPEED_MULT = 200.0f,
            const bool WILL_HOLD_FADE = false);

        void SetPopup(const IStagePtr_t POPUP_ISTAGE_PTR) { popupStagePtrOpt_ = POPUP_ISTAGE_PTR; }

        void SetHoldTime(const float SECONDS)
        {
            holdTimeCounter_ = 0.0f;
            holdTime_ = SECONDS;
        }

        void SetWillExitAfterFade(const bool B) { willExitAfterFade_ = B; }
        void SetWillHoldFade(const bool NEW_VAL) { willHoldFade_ = NEW_VAL; }

        void SetMouseVisibility(const bool IS_VISIBLE);

        void SetState(const LoopState::Enum E) { state_ = E; }
        LoopState::Enum GetState() const { return state_; }

        void SetWillExitOnKeypress(const bool B) { willExitOnKeypress_ = B; }
        void SetWillExitOnMouseclick(const bool B) { willExitOnMouseclick_ = B; }

        void RemoveFocus();

        void SetFocus(const IEntityPtr_t ENTITY_PTR);

        bool GetIgnoreMouse() const { return willIgnoreMouse_; }
        void SetIgnoreMouse(const bool B) { willIgnoreMouse_ = B; }

        bool GetIgnoreKeystrokes() const { return willIgnoreKeystrokes_; }
        void SetIgnoreKeystrokes(const bool B) { willIgnoreKeystrokes_ = B; }

        void AssignPopupCallbackHandlerInfo(
            const sfml_util::PopupCallback_t::IHandlerPtr_t POPUP_HANDLER_PTR,
            const popup::PopupInfo & POPUP_INFO);

        void FakeMouseClick(const sf::Vector2f & MOUSE_POS_V);

        bool IsFading() const { return continueFading_; }

        void TestingStrAppend(const std::string &);
        void TestingStrIncrement(const std::string &);

        void TestingImageSet(
            const std::string & PATH_STR, const bool WILL_CHECK_FOR_OUTLINE = false);

        bool IsKeyPressed(const sf::Keyboard::Key KEY) const
        {
            return sf::Keyboard::isKeyPressed(KEY);
        }

    protected:
        void LogFrameRate();
        bool ProcessOneSecondTasks(const sf::Vector2i & NEW_MOUSE_POS, bool HAS_MOUSE_MOVED);
        void ProcessMouseHover(const sf::Vector2i & NEW_MOUSE_POS, bool HAS_MOUSE_MOVED);
        void ProcessFader();
        void ProcessPopup();
        void ProcessEvents(const sf::Vector2i & NEW_MOUSE_POS, bool HAS_MOUSE_MOVED);
        void ProcessKeyStrokes(const sf::Event & EVENT);
        void ProcessMouseMove(const sf::Vector2i & NEW_MOUSE_POS);
        void ProcessMouseButtonLeftPressed(const sf::Vector2f &);
        void ProcessMouseButtonLeftReleased(const sf::Vector2f &);
        void ProcessMouseWheelRoll(const sf::Event & EVENT, const sf::Vector2i & NEW_MOUSE_POS);
        void ProcessPopupCallback();
        void ProcessTimeUpdate();
        void ProcessDrawing();
        void PerformNextTest();
        void ProcessHoldTime();
        void ProcessScreenshot();
        void ProcessFramerate();

    protected:
        static const float NO_HOLD_TIME_;
        static const float INVALID_MUSIC_FADE_VALUE_;
        //
        const std::string NAME_;

        // This class owns the lifetime of Stages through these pointers
        IStagePVec_t stagePVec_;

        sf::Clock clock_;
        float oneSecondTimerSec_;
        Fade fader_;
        bool willHoldFade_;
        bool continueFading_;
        bool willExitAfterFade_;
        bool willExit_;

        // this is also an ownership pointer that is NOT a copy of anything in stagePVec_
        IStagePtrOpt_t popupStagePtrOpt_;

        float elapsedTimeSec_;
        bool fatalExitEvent_;
        float holdTime_;
        float holdTimeCounter_;
        bool willExitOnKeypress_;
        bool willExitOnMouseclick_;
        IEntityPtrOpt_t entityWithFocusPtrOpt_;
        bool willIgnoreMouse_;
        bool willIgnoreKeystrokes_;
        popup::PopupInfoOpt_t popupInfoOpt_;
        bool hasFadeStarted_;
        sf::Event::EventType prevEventType_;
        sf::Keyboard::Key prevKeyPressed_;
        bool isMouseHovering_;
        bool takeScreenshot_;
        sfml_util::PopupCallback_t::IHandlerPtrOpt_t popupCallbackPtrOpt_;
        LoopState::Enum state_;
        std::vector<float> frameRateVec_;
        std::size_t frameRateSampleCount_;
        bool willLogFrameRate_;
    };

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_LOOP_INCLUDED
