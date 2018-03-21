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
#ifndef HEROESPATH_SFMLUTIL_LOOP_INCLUDED
#define HEROESPATH_SFMLUTIL_LOOP_INCLUDED
//
// loop.hpp
//  Code to manage an event/draw loop.
//
#include "sfml-util/fade.hpp"
#include "sfml-util/gui/gui-entity.hpp"
#include "sfml-util/loop-state-enum.hpp"
#include "sfml-util/sfml-graphics.hpp"

#include "popup/i-popup-callback.hpp"
#include "popup/popup-info.hpp"

#include <list>
#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace sfml_util
{

    // forward declaration
    class IStage;
    using IStagePtr_t = IStage *;
    using IStagePVec_t = std::vector<IStagePtr_t>;

    // A type that manages an event/draw loop
    class Loop
    {
    public:
        Loop(const Loop &) = delete;
        Loop(const Loop &&) = delete;
        Loop & operator=(const Loop &) = delete;
        Loop & operator=(const Loop &&) = delete;

    public:
        explicit Loop(const std::string & NAME);
        ~Loop();

        void Exit() { willExit_ = true; }

        void AddStage(IStagePtr_t);
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

        void SetPopup(IStagePtr_t popupStagePtr) { popupStagePtr_ = popupStagePtr; }
        void SetHoldTime(const float SECONDS)
        {
            holdTimeCounter_ = 0.0f;
            holdTime_ = SECONDS;
        }

        void SetWillExitAfterFade(const bool B) { willExitAfterFade_ = B; }
        void SetWillHoldFade(const bool NEW_VAL) { willHoldFade_ = NEW_VAL; }

        void SetMouseVisibility(const bool IS_VISIBLE)
        {
            winPtr_->setMouseCursorVisible(IS_VISIBLE);
        }

        void SetState(const LoopState::Enum E) { state_ = E; }
        LoopState::Enum GetState() const { return state_; }

        void ConsumeEvents();

        void SetWillExitOnKeypress(const bool B) { willExitOnKeypress_ = B; }
        void SetWillExitOnMouseclick(const bool B) { willExitOnMouseclick_ = B; }

        void RemoveFocus();

        // returns true if a stage was found owning the GuiEntity
        bool SetFocus(const gui::IGuiEntityPtr_t ENTITY_PTR);

        bool GetIgnoreMouse() const { return willIgnoreMouse_; }
        void SetIgnoreMouse(const bool B) { willIgnoreMouse_ = B; }

        bool GetIgnoreKeystrokes() const { return willIgnoreKeystrokes_; }
        void SetIgnoreKeystrokes(const bool B) { willIgnoreKeystrokes_ = B; }

        void AssignPopupCallbackHandlerInfo(
            popup::IPopupHandler_t * const HANDLER_PTR, const popup::PopupInfo & POPUP_INFO);

        void FakeMouseClick(const sf::Vector2f & MOUSE_POS_V);

        bool IsFading() const { return continueFading_; }

        void TestingStrAppend(const std::string &);
        void TestingStrIncrement(const std::string &);

        void TestingImageSet(
            const sf::Texture &,
            const bool = false,
            const std::string & = "",
            const std::string & = "",
            const std::string & = "");

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
        IStagePVec_t stagePVec_;
        sf::Clock clock_;
        float oneSecondTimerSec_;
        WinPtr_t winPtr_;
        Fade fader_;
        bool willHoldFade_;
        bool continueFading_;
        bool willExitAfterFade_;
        bool willExit_;
        IStagePtr_t popupStagePtr_;
        float elapsedTimeSec_;
        bool fatalExitEvent_;
        float holdTime_;
        float holdTimeCounter_;
        bool willExitOnKeypress_;
        bool willExitOnMouseclick_;
        gui::IGuiEntityPtr_t entityWithFocusSPtr_;
        bool willIgnoreMouse_;
        bool willIgnoreKeystrokes_;
        popup::PopupInfo popupInfo_;
        bool hasFadeStarted_;
        sf::Event::EventType prevEventType_;
        sf::Keyboard::Key prevKeyPressed_;
        bool isMouseHovering_;
        bool takeScreenshot_;
        popup::IPopupHandler_t * popupCallbackPtr_;
        LoopState::Enum state_;
        std::vector<float> frameRateVec_;
        std::size_t frameRateSampleCount_;
        bool willLogFrameRate_;
    };
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_LOOP_INCLUDED
