// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// loop.cpp
//
#include "loop.hpp"

#include "game/loop-manager.hpp"
#include "misc/log-macros.hpp"
#include "misc/vectors.hpp"
#include "sfml-util/callback.hpp"
#include "sfml-util/date-time.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/music-operator.hpp"
#include "sfml-util/sound-manager.hpp"
#include "sfml-util/stage.hpp"

#include <iomanip>

namespace heroespath
{
namespace sfml_util
{

    const float Loop::NO_HOLD_TIME_ { -1.0f }; // any negative will work here
    const float Loop::INVALID_MUSIC_FADE_VALUE_ { -1.0f };
    const float Loop::FRAME_TIME_SEC_MIN_ { 0.0001f };

    Loop::Loop(const std::string & NAME)
        : NAME_(std::string(NAME).append("_Loop"))
        , stagePVec_()
        , oneSecondTimerSec_(0.0f)
        , screenFadeColoredRectSlider_()
        , willHoldAfterFade_(false)
        , willExitAfterFade_(false)
        , willExit_(false)
        , popupStagePtrOpt_()
        , holdTimeDurationSec_(NO_HOLD_TIME_)
        , holdTimeElapsedSec_(0.0f)
        , willExitOnKeypress_(false)
        , willExitOnMouseclick_(false)
        , entityWithFocusPtrOpt_()
        , willIgnoreMouse_(false)
        , willIgnoreKeystrokes_(false)
        , popupInfoOpt_()
        , prevKeyStrokeEventType_(sf::Event::EventType::Count)
        , prevKeyStrokeEventKey_(sf::Keyboard::Key::Unknown)
        , isMouseHovering_(false)
        , popupCallbackPtrOpt_()
        , frameRateVec_(4096, 0.0f)
        , frameRateSampleCount_(0)
    {}

    Loop::~Loop() { FreeAllStages(); }

    void Loop::RemoveFocus()
    {
        entityWithFocusPtrOpt_ = boost::none;

        for (auto & nextStagePtr : stagePVec_)
        {
            nextStagePtr->RemoveFocus();
        }
    }

    void Loop::SetFocus(const IEntityPtr_t ENTITY_PTR)
    {
        entityWithFocusPtrOpt_ = ENTITY_PTR;

        for (auto & nextStagePtr : stagePVec_)
        {
            nextStagePtr->SetFocus(ENTITY_PTR);
        }
    }

    void Loop::AssignPopupCallbackHandlerInfo(
        const sfml_util::PopupCallback_t::IHandlerPtr_t POPUP_HANDLER_PTR,
        const popup::PopupInfo & POPUP_INFO)
    {
        popupInfoOpt_ = POPUP_INFO;
        popupCallbackPtrOpt_ = POPUP_HANDLER_PTR;
    }

    void Loop::FakeMouseClick(const sf::Vector2f & NEW_MOUSE_POS_VF)
    {
        ProcessEventMouseButtonPressedLeft(NEW_MOUSE_POS_VF);
        ProcessEventMouseButtonReleasedLeft(NEW_MOUSE_POS_VF);
    }

    void Loop::TestingStrAppend(const std::string & S)
    {
        for (auto & nextStagePtr : stagePVec_)
        {
            nextStagePtr->TestingStrAppend(S);
        }
    }

    void Loop::TestingStrIncrement(const std::string & S)
    {
        for (auto & nextStagePtr : stagePVec_)
        {
            nextStagePtr->TestingStrIncrement(S);
        }
    }

    void Loop::TestingImageSet(const std::string & PATH_STR, const bool WILL_CHECK_FOR_OUTLINE)
    {
        for (auto & nextStagePtr : stagePVec_)
        {
            nextStagePtr->TestingImageSet(PATH_STR, WILL_CHECK_FOR_OUTLINE);
        }
    }

    void Loop::FreeAllStages()
    {
        for (const auto & ISTAGE_PTR : stagePVec_)
        {
            delete ISTAGE_PTR.Ptr();
        }

        stagePVec_.clear();
        FreePopupStage();
    }

    void Loop::FreePopupStage()
    {
        if (popupStagePtrOpt_)
        {
            delete popupStagePtrOpt_->Ptr();
            popupStagePtrOpt_ = boost::none;
        }
    }

    const MouseThisFrame Loop::GatherMouseChanges(sf::Vector2i & prevMousePosVI) const
    {
        const auto NEW_MOUSE_POS_VI { Display::Instance()->GetMousePosition() };
        const auto HAS_MOUSE_MOVED { (NEW_MOUSE_POS_VI != prevMousePosVI) };
        prevMousePosVI = NEW_MOUSE_POS_VI;
        return MouseThisFrame(HAS_MOUSE_MOVED, NEW_MOUSE_POS_VI);
    }

    void Loop::LogFrameRate()
    {
        if (frameRateSampleCount_ > 1)
        {
            // find min, max, and average framerate
            auto min { frameRateVec_[0] };
            auto max { 0.0f };
            auto sum { 0.0f };

            // Skip the first framerate number because it includes time spent
            // logging the prev framerate.
            for (std::size_t i(1); i < frameRateSampleCount_; ++i)
            {
                const auto NEXT_FRAMERATE { frameRateVec_[i] };

                sum += NEXT_FRAMERATE;

                if (NEXT_FRAMERATE < min)
                {
                    min = NEXT_FRAMERATE;
                }

                if (NEXT_FRAMERATE > max)
                {
                    max = NEXT_FRAMERATE;
                }
            }

            const auto AVERAGE_FRAMERATE { sum / static_cast<float>(frameRateSampleCount_) };

            const float STANDARD_DEVIATION { misc::Vector::StandardDeviation(
                frameRateVec_, frameRateSampleCount_, AVERAGE_FRAMERATE, true) };

            M_HP_LOG(
                "Framerate after " << (frameRateSampleCount_ - 1) << " frames [" << min << ", "
                                   << AVERAGE_FRAMERATE << ", " << max
                                   << "], std_dev=" << STANDARD_DEVIATION);

            frameRateSampleCount_ = 0;
        }
    }

    bool Loop::HasOneSecondTimerElapsed(const float ELAPSED_TIME_SEC)
    {
        // "one-second-tasts" are paused while fading
        // if (false == IsFading())
        {
            oneSecondTimerSec_ += ELAPSED_TIME_SEC;
        }

        if (oneSecondTimerSec_ > 1.0f)
        {
            oneSecondTimerSec_ = 0.0f;
            return true;
        }
        else
        {
            return false;
        }
    }

    void Loop::StartMouseHover(const MouseThisFrame & MOUSE_THIS_FRAME)
    {
        // don't show mouseovers while fading, if the mouse is moving, or if already showing
        if ((IsFading() == false) && (MOUSE_THIS_FRAME.has_moved == false)
            && (false == isMouseHovering_))
        {
            isMouseHovering_ = true;

            // popup stages handle mouseovers exclusively when present
            if (popupStagePtrOpt_)
            {
                popupStagePtrOpt_.value()->SetMouseHover(MOUSE_THIS_FRAME.pos_vf, true);
            }
            else
            {
                for (auto & nextStagePtr : stagePVec_)
                {
                    nextStagePtr->SetMouseHover(MOUSE_THIS_FRAME.pos_vf, true);
                }
            }
        }
    }

    void Loop::StopMouseHover(const MouseThisFrame & MOUSE_THIS_FRAME)
    {
        if (isMouseHovering_ && MOUSE_THIS_FRAME.has_moved)
        {
            isMouseHovering_ = false;

            // popup stages process exclusively when present
            if (popupStagePtrOpt_)
            {
                popupStagePtrOpt_.value()->SetMouseHover(MOUSE_THIS_FRAME.pos_vf, false);
            }
            else
            {
                for (auto & nextStagePtr : stagePVec_)
                {
                    nextStagePtr->SetMouseHover(MOUSE_THIS_FRAME.pos_vf, false);
                }
            }
        }
    }

    void Loop::ProcessHoldTime(const float ELAPSED_TIME_SEC)
    {
        if (holdTimeDurationSec_ > 0.0f)
        {
            holdTimeElapsedSec_ += ELAPSED_TIME_SEC;

            if (holdTimeElapsedSec_ > holdTimeDurationSec_)
            {
                holdTimeDurationSec_ = NO_HOLD_TIME_;
                willExit_ = true;
            }
        }
    }

    void Loop::ProcessFade(const float ELAPSED_TIME_SEC)
    {
        if (IsFading())
        {
            game::LoopManager::Instance()->HandleTransitionBeforeFade();

            const auto DID_FADE_FINISH { screenFadeColoredRectSlider_.UpdateAndReturnIsStopped(
                ELAPSED_TIME_SEC) };

            if (DID_FADE_FINISH && willExitAfterFade_)
            {
                willExit_ = true;
            }
        }

        if (IsFading() || willHoldAfterFade_)
        {
            Display::Instance()->DrawFullScreenFader(screenFadeColoredRectSlider_);
        }
    }

    void Loop::ProcessEvents(const sf::Vector2f & MOUSE_POS_VF)
    {
        sf::Event event;
        if (Display::Instance()->PollEvent(event))
        {
            ProcessEvent(event, MOUSE_POS_VF);
        }
    }

    void Loop::ProcessEvent(const sf::Event & EVENT, const sf::Vector2f & MOUSE_POS_VF)
    {
        if (EVENT.type == sf::Event::Closed)
        {
            M_HP_LOG_ERR(NAME_ << " UNEXPECTED WINDOW CLOSE");
            Display::Instance()->Close();
            willExit_ = true;
        }
        else if ((EVENT.type == sf::Event::KeyPressed) || (EVENT.type == sf::Event::KeyReleased))
        {
            ProcessEventKeyStroke(EVENT);
            return;
        }
        else if ((false == willIgnoreMouse_) && (EVENT.mouseButton.button == sf::Mouse::Left))
        {
            if (EVENT.type == sf::Event::MouseButtonPressed)
            {
                ProcessEventMouseButtonPressedLeft(MOUSE_POS_VF);
            }
            else if (EVENT.type == sf::Event::MouseButtonReleased)
            {
                ProcessEventMouseButtonReleasedLeft(MOUSE_POS_VF);
            }

            // else if (EVENT.type == sf::Event::MouseWheelMoved)
            //{
            //    ProcessMouseWheelRoll(EVENT, NEW_MOUSE_POS_VF);
            //}
        }
    }

    void Loop::ProcessEventKeyStroke(const sf::Event & EVENT)
    {
        const auto IS_KEY_STROKE_EVENT_DIFFERENT_FROM_PREV {
            (EVENT.type != prevKeyStrokeEventType_) || (EVENT.key.code != prevKeyStrokeEventKey_)
        };

        if (IS_KEY_STROKE_EVENT_DIFFERENT_FROM_PREV)
        {
            prevKeyStrokeEventType_ = EVENT.type;
            prevKeyStrokeEventKey_ = EVENT.key.code;

            M_HP_LOG(
                "Key " << ((EVENT.type == sf::Event::KeyReleased) ? "released" : "pressed") << ": "
                       << EVENT.key.code << ((willIgnoreKeystrokes_) ? " IGNORED" : ""));

            // allow screenshots even if keystrokes are ignored
            if (EVENT.key.code == sf::Keyboard::F12)
            {
                Display::Instance()->TakeScreenshot();
            }
        }

        if (willIgnoreKeystrokes_)
        {
            return;
        }

        const auto IS_KEYPRESS { EVENT.type == sf::Event::KeyPressed };

        // popup stages process exclusively when present
        if (popupStagePtrOpt_)
        {
            if (IS_KEYPRESS)
            {
                popupStagePtrOpt_.value()->KeyPress(EVENT.key);
            }
            else
            {
                popupStagePtrOpt_.value()->KeyRelease(EVENT.key);
            }
        }
        else
        {
            // give event to stages for processing
            for (auto & nextStagePtr : stagePVec_)
            {
                if (IS_KEYPRESS)
                {
                    nextStagePtr->KeyPress(EVENT.key);
                }
                else
                {
                    nextStagePtr->KeyRelease(EVENT.key);
                }
            }
        }

        //...and to exit the app on F1 keypresses
        if ((EVENT.type == sf::Event::KeyReleased) && (EVENT.key.code == sf::Keyboard::F1))
        {
            M_HP_LOG(NAME_ << " F1 KEY RELEASED.  Bail.");
            game::LoopManager::Instance()->TransitionTo_Exit();
            willExit_ = true;
        }

        if (willExitOnKeypress_)
        {
            M_HP_LOG(NAME_ << " key event while willExitOnKeypress.  Exiting the loop.");
            sfml_util::SoundManager::Instance()->PlaySfx_Keypress();
            willExit_ = true;
        }
    }

    void Loop::ProcessEventMouseMove(const MouseThisFrame & MOUSE_THIS_FRAME)
    {
        if (willIgnoreMouse_ || (MOUSE_THIS_FRAME.has_moved == false))
        {
            return;
        }

        if (popupStagePtrOpt_)
        {
            popupStagePtrOpt_.value()->UpdateMousePos(MOUSE_THIS_FRAME.pos_vi);
        }
        else
        {
            for (auto & nextStagePtr : stagePVec_)
            {
                nextStagePtr->UpdateMousePos(MOUSE_THIS_FRAME.pos_vi);
            }
        }
    }

    void Loop::ProcessEventMouseButtonPressedLeft(const sf::Vector2f & MOUSE_POS_VF)
    {
        if (popupStagePtrOpt_)
        {
            popupStagePtrOpt_.value()->UpdateMouseDown(MOUSE_POS_VF);
        }
        else
        {
            for (auto & nextStagePtr : stagePVec_)
            {
                nextStagePtr->UpdateMouseDown(MOUSE_POS_VF);
            }
        }

        if (willExitOnMouseclick_)
        {
            M_HP_LOG(NAME_ << " mouse click while willExitOnMouseclick.  Exiting...");
            sfml_util::SoundManager::Instance()->PlaySfx_MouseClick();
            willExit_ = true;
        }
    }

    void Loop::ProcessEventMouseButtonReleasedLeft(const sf::Vector2f & MOUSE_POS_VF)
    {
        if (popupStagePtrOpt_)
        {
            SetNewFocusEntity(popupStagePtrOpt_.value()->UpdateMouseUp(MOUSE_POS_VF));
        }
        else
        {
            for (auto & nextStagePtr : stagePVec_)
            {
                if (SetNewFocusEntity(nextStagePtr->UpdateMouseUp(MOUSE_POS_VF)))
                {
                    break;
                }
            }
        }
    }

    // I had problems getting smooth mousewheel motion so I'm pausing this code
    // void Loop::ProcessMouseWheelRoll(const sf::Event & EVENT, const sf::Vector2i &
    // NEW_MOUSE_POS)
    //{
    //    const sf::Vector2f NEW_MOUSE_POS_F(NEW_MOUSE_POS);
    //
    //    if (popupStagePtrOpt_)
    //    {
    //        popupStagePtrOpt_.value()->UpdateMouseWheel(
    //            NEW_MOUSE_POS_F, EVENT.mouseWheelScroll.delta);
    //    }
    //    else
    //    {
    //        for (auto & nextStagePtr : stagePVec_)
    //        {
    //            nextStagePtr->UpdateMouseWheel(NEW_MOUSE_POS_F, EVENT.mouseWheelScroll.delta);
    //        }
    //    }
    //}

    void Loop::ProcessPopupCallback()
    {
        if (!popupCallbackPtrOpt_ || !popupInfoOpt_)
        {
            return;
        }

        const auto POPUP_RESPONSE_ENUM { game::LoopManager::Instance()->GetPopupResponse() };
        const auto POPUP_SELECTION { game::LoopManager::Instance()->GetPopupSelection() };

        if (POPUP_RESPONSE_ENUM != popup::ResponseTypes::None)
        {
            M_HP_LOG(
                "PopupCallback resp=\"" << popup::ResponseTypes::ToString(POPUP_RESPONSE_ENUM)
                                        << "\" with selection=" << POPUP_SELECTION << " to popup=\""
                                        << popupInfoOpt_->Name() << "\"");

            popup::PopupResponse response(
                popupInfoOpt_->Name(), POPUP_RESPONSE_ENUM, POPUP_SELECTION);

            const sfml_util::PopupCallback_t::PacketPtr_t CALLBACK_PACKET_PTR(&response);

            const auto WILL_RESET_CALLBACKHANDLER { popupCallbackPtrOpt_.value()->HandleCallback(
                CALLBACK_PACKET_PTR) };

            game::LoopManager::Instance()->ClearPopupResponse();

            if (WILL_RESET_CALLBACKHANDLER)
            {
                popupCallbackPtrOpt_ = boost::none;
            }
        }
    }

    void Loop::ProcessFramerate(const float ELAPSED_TIME_SEC_ORIG)
    {
        const auto ELAPSED_TIME_SEC_FINAL { (
            (ELAPSED_TIME_SEC_ORIG > FRAME_TIME_SEC_MIN_) ? ELAPSED_TIME_SEC_ORIG
                                                          : FRAME_TIME_SEC_MIN_) };

        frameRateVec_[frameRateSampleCount_++] = (1.0f / ELAPSED_TIME_SEC_FINAL);

        if (frameRateSampleCount_ >= frameRateVec_.size())
        {
            frameRateVec_.resize(frameRateSampleCount_ * 2);
        }
    }

    void Loop::ProcessTimeUpdate(const float ELAPSED_TIME_SEC)
    {
        for (auto & nextStagePtr : stagePVec_)
        {
            nextStagePtr->UpdateTime(ELAPSED_TIME_SEC);
        }

        if (popupStagePtrOpt_)
        {
            popupStagePtrOpt_.value()->UpdateTime(ELAPSED_TIME_SEC);
        }
    }

    void Loop::ProcessDrawing()
    {
        for (const auto & ISTAGE_PTR : stagePVec_)
        {
            Display::Instance()->DrawStage(ISTAGE_PTR);
        }

        if (popupStagePtrOpt_)
        {
            Display::Instance()->DrawStage(popupStagePtrOpt_.value());
        }
    }

    void Loop::PerformNextTest()
    {
        for (auto & nextStagePtr : stagePVec_)
        {
            nextStagePtr->PerformNextTest();
        }
    }

    bool Loop::SetNewFocusEntity(const sfml_util::IEntityPtrOpt_t & IENTITY_PTR_OPT)
    {
        if (IENTITY_PTR_OPT)
        {
            const auto NEW_ENTITY_WITH_FOCUS_PTR { IENTITY_PTR_OPT.value() };
            RemoveFocus();
            NEW_ENTITY_WITH_FOCUS_PTR->SetHasFocus(true);
            SetFocus(NEW_ENTITY_WITH_FOCUS_PTR);
            return true;
        }
        else
        {
            return false;
        }
    }

    void Loop::Execute()
    {
        // reset loop variables
        sf::Clock frameClock;
        frameRateSampleCount_ = 0;
        willExit_ = false;
        auto prevMousePos { Display::Instance()->GetMousePosition() };

        // consume pre-events
        Display::Instance()->ConsumeEvents();

        while (Display::Instance()->IsOpen() && (false == willExit_))
        {
            // eventually we should move all testing to unit tests and remove this line
            PerformNextTest();

            Display::Instance()->ClearToBlack();

            const auto ELAPSED_TIME_SEC { frameClock.getElapsedTime().asSeconds() };
            frameClock.restart();

            ProcessFramerate(ELAPSED_TIME_SEC);

            const MouseThisFrame MOUSE_THIS_FRAME { GatherMouseChanges(prevMousePos) };

            if (HasOneSecondTimerElapsed(ELAPSED_TIME_SEC))
            {
                LogFrameRate();
                StartMouseHover(MOUSE_THIS_FRAME);
            }

            ProcessHoldTime(ELAPSED_TIME_SEC);
            ProcessEventMouseMove(MOUSE_THIS_FRAME);
            ProcessEvents(MOUSE_THIS_FRAME.pos_vf);
            ProcessTimeUpdate(ELAPSED_TIME_SEC);
            sfml_util::SoundManager::Instance()->UpdateTime(ELAPSED_TIME_SEC);
            ProcessDrawing();
            ProcessFade(ELAPSED_TIME_SEC);
            ProcessPopupCallback();

            Display::Instance()->DisplayFrameBuffer();
        }

        Display::Instance()->ConsumeEvents();

        // reset hold time
        holdTimeDurationSec_ = NO_HOLD_TIME_;
        holdTimeElapsedSec_ = 0.0f;

        // reset fade states
        willExitAfterFade_ = false;
        willHoldAfterFade_ = false;
    }

    void Loop::FadeOut(const float SPEED, const sf::Color & FADE_TO_COLOR)
    {
        screenFadeColoredRectSlider_ = ColoredRectSlider(
            Display::Instance()->FullScreenRect(),
            sf::Color::Transparent,
            FADE_TO_COLOR,
            SPEED,
            WillOscillate::No,
            WillAutoStart::Yes);

        willHoldAfterFade_ = true;
        willExitAfterFade_ = true;
    }

    void Loop::FadeIn(
        const float SPEED, const bool WILL_EXIT_AFTER, const sf::Color & FADE_FROM_COLOR)
    {
        screenFadeColoredRectSlider_ = ColoredRectSlider(
            Display::Instance()->FullScreenRect(),
            FADE_FROM_COLOR,
            sf::Color::Transparent,
            SPEED,
            WillOscillate::No,
            WillAutoStart::Yes);

        willHoldAfterFade_ = false;
        willExitAfterFade_ = WILL_EXIT_AFTER;
    }

    void Loop::SetMouseVisibility(const bool IS_VISIBLE)
    {
        Display::Instance()->SetMouseCursorVisibility(IS_VISIBLE);
    }

} // namespace sfml_util
} // namespace heroespath
