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
#include "sfml-util/slider-colored-rect.hpp"

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

    struct MouseThisFrame
    {
        MouseThisFrame(const bool HAS_MOVED, const sf::Vector2i & POS_VI)
            : has_moved(HAS_MOVED)
            , pos_vi(POS_VI)
            , pos_vf(POS_VI)
        {}

        MouseThisFrame(const MouseThisFrame &) = default;
        MouseThisFrame(MouseThisFrame &&) = default;
        MouseThisFrame & operator=(const MouseThisFrame &) = default;
        MouseThisFrame & operator=(MouseThisFrame &&) = default;

        bool has_moved;
        sf::Vector2i pos_vi;
        sf::Vector2f pos_vf;
    };

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

        void Execute();

        void FadeOut(const float SPEED, const sf::Color & FADE_TO_COLOR);

        void FadeIn(
            const float SPEED, const bool WILL_EXIT_AFTER, const sf::Color & FADE_FROM_COLOR);

        void SetPopup(const IStagePtr_t POPUP_ISTAGE_PTR) { popupStagePtrOpt_ = POPUP_ISTAGE_PTR; }

        void SetHoldTime(const float SECONDS)
        {
            holdTimeElapsedSec_ = 0.0f;
            holdTimeDurationSec_ = SECONDS;
        }

        void SetMouseVisibility(const bool IS_VISIBLE);

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

        bool IsFading() const { return (screenFadeColoredRectSlider_.IsMoving()); }

        void TestingStrAppend(const std::string &);
        void TestingStrIncrement(const std::string &);

        void TestingImageSet(
            const std::string & PATH_STR, const bool WILL_CHECK_FOR_OUTLINE = false);

        bool IsKeyPressed(const sf::Keyboard::Key KEY) const
        {
            return sf::Keyboard::isKeyPressed(KEY);
        }

    private:
        const MouseThisFrame GatherMouseChanges(sf::Vector2i & prevMousePosVI) const;
        void LogFrameRate();
        bool HasOneSecondTimerElapsed(const float ELAPSED_TIME_SEC);
        void StartMouseHover(const MouseThisFrame & MOUSE_THIS_FRAME);
        void StopMouseHover(const MouseThisFrame & MOUSE_THIS_FRAME);
        void ProcessFade(const float ELAPSED_TIME_SEC);
        void ProcessPopup(const float ELAPSED_TIME_SEC);
        void ProcessEvents(const sf::Vector2f & MOUSE_POS_VF);
        void ProcessEvent(const sf::Event & EVENT, const sf::Vector2f & MOUSE_POS_VF);
        void ProcessEventKeyStroke(const sf::Event & EVENT);
        void ProcessEventMouseMove(const MouseThisFrame & MOUSE_THIS_FRAME);
        void ProcessEventMouseButtonPressedLeft(const sf::Vector2f & MOUSE_POS_VF);
        void ProcessEventMouseButtonReleasedLeft(const sf::Vector2f & MOUSE_POS_VF);
        void ProcessPopupCallback();
        void ProcessTimeUpdate(const float ELAPSED_TIME_SEC);
        void ProcessDrawing();
        void PerformNextTest();
        void ProcessHoldTime(const float ELAPSED_TIME_SEC);
        void ProcessFramerate(const float ELAPSED_TIME_SEC);

        // returns true if optional was not empty and a new focus entity was set
        bool SetNewFocusEntity(const sfml_util::IEntityPtrOpt_t & IENTITY_PTR_OPT);

    private:
        static const float NO_HOLD_TIME_;
        static const float INVALID_MUSIC_FADE_VALUE_;
        static const float FRAME_TIME_SEC_MIN_;

        const std::string NAME_;

        // This class owns the lifetime of Stages through these pointers
        IStagePVec_t stagePVec_;

        float oneSecondTimerSec_;

        ColoredRectSlider screenFadeColoredRectSlider_;
        bool willHoldAfterFade_;
        bool willExitAfterFade_;
        bool willExit_;

        // this is also an ownership pointer that is NOT a copy of anything in stagePVec_
        IStagePtrOpt_t popupStagePtrOpt_;

        float holdTimeDurationSec_;
        float holdTimeElapsedSec_;
        bool willExitOnKeypress_;
        bool willExitOnMouseclick_;
        IEntityPtrOpt_t entityWithFocusPtrOpt_;
        bool willIgnoreMouse_;
        bool willIgnoreKeystrokes_;
        popup::PopupInfoOpt_t popupInfoOpt_;
        sf::Event::EventType prevKeyStrokeEventType_;
        sf::Keyboard::Key prevKeyStrokeEventKey_;
        bool isMouseHovering_;
        sfml_util::PopupCallback_t::IHandlerPtrOpt_t popupCallbackPtrOpt_;
        std::vector<float> frameRateVec_;
        std::size_t frameRateSampleCount_;
    };

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_LOOP_INCLUDED
