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

    const float Loop::NO_HOLD_TIME_(-1.0f);
    const float Loop::INVALID_MUSIC_FADE_VALUE_(-1.0f);

    Loop::Loop(const std::string & NAME)
        : NAME_(std::string(NAME).append("_Loop"))
        , stagePVec_()
        , clock_()
        , oneSecondTimerSec_(0.0f)
        , fader_(
              0.0f, 0.0f, Display::Instance()->GetWinWidth(), Display::Instance()->GetWinHeight())
        , willHoldFade_(false)
        , continueFading_(false)
        , willExitAfterFade_(false)
        , willExit_(false)
        , popupStagePtrOpt_()
        , elapsedTimeSec_(0.0f)
        , fatalExitEvent_(false)
        , holdTime_(NO_HOLD_TIME_)
        , holdTimeCounter_(0.0f)
        , willExitOnKeypress_(false)
        , willExitOnMouseclick_(false)
        , entityWithFocusPtrOpt_()
        , willIgnoreMouse_(false)
        , willIgnoreKeystrokes_(false)
        , popupInfoOpt_()
        , // okay to be initially without meaningful values
        hasFadeStarted_(false)
        , prevEventType_(sf::Event::EventType::Count)
        , prevKeyPressed_(sf::Keyboard::Key::Unknown)
        , isMouseHovering_(false)
        , takeScreenshot_(false)
        , popupCallbackPtrOpt_()
        , state_(LoopState::Intro)
        , frameRateVec_()
        , frameRateSampleCount_(0)
        , willLogFrameRate_(false)
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

    void Loop::FakeMouseClick(const sf::Vector2f & MOUSE_POS_V)
    {
        ProcessMouseButtonLeftPressed(MOUSE_POS_V);
        ProcessMouseButtonLeftReleased(MOUSE_POS_V);
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
        for (auto const & ISTAGE_PTR : stagePVec_)
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

    void Loop::LogFrameRate()
    {
        if (willLogFrameRate_ && (frameRateSampleCount_ > 0))
        {
            // find min, max, and average framerate
            auto min { frameRateVec_[0] };
            auto max { 0.0f };
            auto sum { 0.0f };

            // Skip the first framerate number because it includes time spent
            // logging the prev framerate.
            for (std::size_t i(1); i < frameRateSampleCount_; ++i)
            {
                auto const NEXT_FRAMERATE { frameRateVec_[i] };

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

            auto const AVERAGE_FRAMERATE { sum / static_cast<float>(frameRateSampleCount_) };

            const float STANDARD_DEVIATION { misc::Vector::StandardDeviation(
                frameRateVec_, frameRateSampleCount_, AVERAGE_FRAMERATE, true) };

            M_HP_LOG(
                "Frame rate min=" << min << ", max=" << max << ", count="
                                  << frameRateSampleCount_ - 1 << ", avg=" << AVERAGE_FRAMERATE
                                  << ", std_dev=" << STANDARD_DEVIATION);
        }
        else
        {
            willLogFrameRate_ = true;
        }

        frameRateSampleCount_ = 0;
    }

    bool Loop::ProcessOneSecondTasks(const sf::Vector2i & NEW_MOUSE_POS, const bool HAS_MOUSE_MOVED)
    {
        auto willProcessOneSecondTasks { false };

        if (false == continueFading_)
        {
            oneSecondTimerSec_ += elapsedTimeSec_;
        }

        if (oneSecondTimerSec_ > 1.0f)
        {
            willProcessOneSecondTasks = true;

            oneSecondTimerSec_ = 0.0f;

            LogFrameRate();

            if ((false == continueFading_) && (HAS_MOUSE_MOVED == false)
                && (isMouseHovering_ == false))
            {
                isMouseHovering_ = true;

                const sf::Vector2f NEW_MOUSE_POS_F(NEW_MOUSE_POS);

                // popup stages process exclusively when present
                if (popupStagePtrOpt_)
                {
                    popupStagePtrOpt_.value()->SetMouseHover(NEW_MOUSE_POS_F, true);
                }
                else
                {
                    for (auto & nextStagePtr : stagePVec_)
                    {
                        nextStagePtr->SetMouseHover(NEW_MOUSE_POS_F, true);
                    }
                }
            }
        }

        return willProcessOneSecondTasks;
    }

    void Loop::ProcessMouseHover(const sf::Vector2i & NEW_MOUSE_POS, bool HAS_MOUSE_MOVED)
    {
        if (isMouseHovering_ && HAS_MOUSE_MOVED)
        {
            isMouseHovering_ = false;

            const sf::Vector2f MOUSE_POS_NEW_F(NEW_MOUSE_POS);

            // popup stages process exclusively when present
            if (popupStagePtrOpt_)
            {
                popupStagePtrOpt_.value()->SetMouseHover(MOUSE_POS_NEW_F, false);
            }
            else
            {
                for (auto & nextStagePtr : stagePVec_)
                {
                    nextStagePtr->SetMouseHover(MOUSE_POS_NEW_F, false);
                }
            }
        }
    }

    void Loop::ProcessHoldTime()
    {
        if (holdTime_ > 0.0f)
        {
            holdTimeCounter_ += elapsedTimeSec_;
            if (holdTimeCounter_ > holdTime_)
            {
                holdTime_ = NO_HOLD_TIME_;
                willExit_ = true;
            }
        }
    }

    void Loop::ProcessFader()
    {
        if ((hasFadeStarted_) && ((continueFading_ || willHoldFade_)))
        {
            Display::Instance()->DrawFader(fader_);
        }

        if (continueFading_)
        {
            game::LoopManager::Instance()->HandleTransitionBeforeFade();

            continueFading_ = !fader_.Update(elapsedTimeSec_);

            hasFadeStarted_ = true;

            if ((false == continueFading_) && willExitAfterFade_)
            {
                willExit_ = true;
            }
        }
    }

    void Loop::ProcessPopup()
    {
        if (popupStagePtrOpt_)
        {
            popupStagePtrOpt_.value()->UpdateTime(elapsedTimeSec_);
            Display::Instance()->DrawStage(popupStagePtrOpt_.value());
        }
    }

    void Loop::ProcessEvents(const sf::Vector2i & NEW_MOUSE_POS, bool HAS_MOUSE_MOVED)
    {
        if ((false == willIgnoreMouse_) && HAS_MOUSE_MOVED)
        {
            ProcessMouseMove(NEW_MOUSE_POS);
        }

        sf::Event e;
        if (Display::Instance()->PollEvent(e))
        {
            // unexpected window close exit condition
            if (e.type == sf::Event::Closed)
            {
                M_HP_LOG_WRN(NAME_ << " UNEXPECTED WINDOW CLOSE");
                Display::Instance()->Close();
                fatalExitEvent_ = true;
            }

            if ((e.type == sf::Event::KeyPressed) || (e.type == sf::Event::KeyReleased))
            {
                ProcessKeyStrokes(e);
            }

            if ((false == willIgnoreMouse_) && (e.type == sf::Event::MouseButtonPressed))
            {
                if (e.mouseButton.button == sf::Mouse::Left)
                {
                    const sf::Vector2f NEW_MOUSE_POS_F(NEW_MOUSE_POS);

                    ProcessMouseButtonLeftPressed(NEW_MOUSE_POS_F);
                }
                else
                {
                    M_HP_LOG("Unsupported mouse button pressed: " << e.mouseButton.button);
                }
            }

            if ((false == willIgnoreMouse_) && (e.type == sf::Event::MouseButtonReleased))
            {
                if (e.mouseButton.button == sf::Mouse::Left)
                {
                    const sf::Vector2f NEW_MOUSE_POS_F(NEW_MOUSE_POS);

                    ProcessMouseButtonLeftReleased(NEW_MOUSE_POS_F);
                }
                else
                {
                    M_HP_LOG("Unsupported mouse button released: " << e.mouseButton.button);
                }
            }

            if ((false == willIgnoreMouse_) && (e.type == sf::Event::MouseWheelMoved))
            {
                ProcessMouseWheelRoll(e, NEW_MOUSE_POS);
            }
        }
    }

    void Loop::ProcessKeyStrokes(const sf::Event & EVENT)
    {
        if ((EVENT.type != prevEventType_) || (EVENT.key.code != prevKeyPressed_))
        {
            prevEventType_ = EVENT.type;
            prevKeyPressed_ = EVENT.key.code;

            std::ostringstream ss;
            ss << "Key " << ((EVENT.type == sf::Event::KeyReleased) ? "released" : "pressed")
               << ": " << EVENT.key.code << ((willIgnoreKeystrokes_) ? " IGNORED" : "");

            M_HP_LOG(ss.str());
        }

        // allow screenshots even if keystrokes are ignored
        if (EVENT.key.code == sf::Keyboard::F12)
        {
            takeScreenshot_ = true;
        }

        if (willIgnoreKeystrokes_)
        {
            return;
        }

        // popup stages process exclusively when present
        if (popupStagePtrOpt_)
        {
            if (EVENT.type == sf::Event::KeyPressed)
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
                if (EVENT.type == sf::Event::KeyPressed)
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
        }
        if ((EVENT.key.code == sf::Keyboard::Escape)
            && (game::LoopManager::Instance()->GetState() == LoopState::Test))
        {
            M_HP_LOG(NAME_ << " ESCAPE KEY RELEASED WHILE TESTING.  Bail.");
            game::LoopManager::Instance()->TransitionTo_Exit();
        }
        else
        {
            if (willExitOnKeypress_)
            {
                M_HP_LOG(NAME_ << " key event while willExitOnKeypress.  Exiting the loop.");
                sfml_util::SoundManager::Instance()->PlaySfx_Keypress();
                willExit_ = true;
            }
        }
    }

    void Loop::ProcessMouseMove(const sf::Vector2i & NEW_MOUSE_POS)
    {
        if (popupStagePtrOpt_)
        {
            popupStagePtrOpt_.value()->UpdateMousePos(NEW_MOUSE_POS);
        }
        else
        {
            for (auto & nextStagePtr : stagePVec_)
            {
                nextStagePtr->UpdateMousePos(NEW_MOUSE_POS);
            }
        }
    }

    void Loop::ProcessMouseButtonLeftPressed(const sf::Vector2f & MOUSE_POS_V)
    {
        if (popupStagePtrOpt_)
        {
            popupStagePtrOpt_.value()->UpdateMouseDown(MOUSE_POS_V);
        }
        else
        {
            for (auto & nextStagePtr : stagePVec_)
            {
                nextStagePtr->UpdateMouseDown(MOUSE_POS_V);
            }
        }

        if (willExitOnMouseclick_)
        {
            M_HP_LOG(NAME_ << " mouse click while willExitOnMouseclick.  Exiting...");
            sfml_util::SoundManager::Instance()->PlaySfx_MouseClick();
            willExit_ = true;
        }
    }

    void Loop::ProcessMouseButtonLeftReleased(const sf::Vector2f & MOUSE_POS_V)
    {
        if (popupStagePtrOpt_)
        {
            auto newEntityWithFocusPtrOpt { popupStagePtrOpt_.value()->UpdateMouseUp(MOUSE_POS_V) };

            if (newEntityWithFocusPtrOpt)
            {
                auto const NEW_ENTITY_WITH_FOCUS_PTR { newEntityWithFocusPtrOpt.value() };

                RemoveFocus();
                NEW_ENTITY_WITH_FOCUS_PTR->SetHasFocus(true);
                SetFocus(NEW_ENTITY_WITH_FOCUS_PTR);
            }
        }
        else
        {
            for (auto & nextStagePtr : stagePVec_)
            {
                auto newEntityWithFocusPtrOpt { nextStagePtr->UpdateMouseUp(MOUSE_POS_V) };

                if (newEntityWithFocusPtrOpt)
                {
                    auto const NEW_ENTITY_WITH_FOCUS_PTR { newEntityWithFocusPtrOpt.value() };

                    M_HP_LOG(
                        "MouseButtonLeftReleased caused focus in stage \""
                        << nextStagePtr->GetStageName() << "\" on entity \""
                        << NEW_ENTITY_WITH_FOCUS_PTR->GetEntityName() << "\"");

                    RemoveFocus();
                    NEW_ENTITY_WITH_FOCUS_PTR->SetHasFocus(true);
                    SetFocus(NEW_ENTITY_WITH_FOCUS_PTR);

                    // TODO um...shoudln't we break here?
                }
            }
        }
    }

    void Loop::ProcessMouseWheelRoll(const sf::Event & EVENT, const sf::Vector2i & NEW_MOUSE_POS)
    {
        const sf::Vector2f NEW_MOUSE_POS_F(NEW_MOUSE_POS);

        if (popupStagePtrOpt_)
        {
            popupStagePtrOpt_.value()->UpdateMouseWheel(
                NEW_MOUSE_POS_F, EVENT.mouseWheelScroll.delta);
        }
        else
        {
            for (auto & nextStagePtr : stagePVec_)
            {
                nextStagePtr->UpdateMouseWheel(NEW_MOUSE_POS_F, EVENT.mouseWheelScroll.delta);
            }
        }
    }

    void Loop::ProcessPopupCallback()
    {
        if (!popupCallbackPtrOpt_ || !popupInfoOpt_)
        {
            return;
        }

        auto const POPUP_RESPONSE_ENUM { game::LoopManager::Instance()->GetPopupResponse() };
        auto const POPUP_SELECTION { game::LoopManager::Instance()->GetPopupSelection() };

        if (POPUP_RESPONSE_ENUM != popup::ResponseTypes::None)
        {
            M_HP_LOG(
                "PopupCallback resp=\"" << popup::ResponseTypes::ToString(POPUP_RESPONSE_ENUM)
                                        << "\" with selection=" << POPUP_SELECTION << " to popup=\""
                                        << popupInfoOpt_->Name() << "\"");

            popup::PopupResponse response(
                popupInfoOpt_->Name(), POPUP_RESPONSE_ENUM, POPUP_SELECTION);

            const sfml_util::PopupCallback_t::PacketPtr_t CALLBACK_PACKET_PTR(&response);

            auto const WILL_RESET_CALLBACKHANDLER { popupCallbackPtrOpt_.value()->HandleCallback(
                CALLBACK_PACKET_PTR) };

            game::LoopManager::Instance()->ClearPopupResponse();

            if (WILL_RESET_CALLBACKHANDLER)
            {
                popupCallbackPtrOpt_ = boost::none;
            }
        }
    }

    void Loop::ProcessScreenshot()
    {
        if (takeScreenshot_)
        {
            takeScreenshot_ = false;
            Display::Instance()->TakeScreenshot();
        }
    }

    void Loop::ProcessFramerate()
    {
        elapsedTimeSec_ = clock_.getElapsedTime().asSeconds();
        clock_.restart();

        frameRateVec_[frameRateSampleCount_++] = (1.0f / elapsedTimeSec_);
        if (frameRateSampleCount_ >= frameRateVec_.size())
        {
            frameRateVec_.resize(frameRateSampleCount_ * 2);
        }
    }

    void Loop::ProcessTimeUpdate()
    {
        for (auto & nextStagePtr : stagePVec_)
        {
            nextStagePtr->UpdateTime(elapsedTimeSec_);
        }
    }

    void Loop::ProcessDrawing()
    {
        for (auto const & ISTAGE_PTR : stagePVec_)
        {
            Display::Instance()->DrawStage(ISTAGE_PTR);
        }
    }

    void Loop::PerformNextTest()
    {
        for (auto & nextStagePtr : stagePVec_)
        {
            nextStagePtr->PerformNextTest();
        }
    }

    bool Loop::Execute()
    {
        // reset loop variables
        willExit_ = false;
        fatalExitEvent_ = false;
        auto soundManagerPtr { sfml_util::SoundManager::Instance() };
        frameRateVec_.resize(4096);
        auto prevMousePos { Display::Instance()->GetMousePosition() };

        // consume pre-events
        Display::Instance()->ConsumeEvents();

        clock_.restart();
        while (Display::Instance()->IsOpen() && (false == willExit_))
        {
            auto const NEW_MOUSE_POS { Display::Instance()->GetMousePosition() };
            auto const HAS_MOUSE_MOVED { NEW_MOUSE_POS != prevMousePos };

            Display::Instance()->ClearToBlack();
            ProcessFramerate();
            soundManagerPtr->UpdateTime(elapsedTimeSec_);
            PerformNextTest();
            ProcessMouseHover(NEW_MOUSE_POS, HAS_MOUSE_MOVED);
            ProcessOneSecondTasks(NEW_MOUSE_POS, HAS_MOUSE_MOVED);
            ProcessHoldTime();
            ProcessEvents(NEW_MOUSE_POS, HAS_MOUSE_MOVED);
            ProcessTimeUpdate();
            ProcessDrawing();
            ProcessFader();
            ProcessPopup();
            ProcessPopupCallback();
            Display::Instance()->DisplayFrameBuffer();
            ProcessScreenshot();

            if (HAS_MOUSE_MOVED)
            {
                prevMousePos = NEW_MOUSE_POS;
            }
        }

        Display::Instance()->ConsumeEvents();

        // reset hold time
        holdTime_ = NO_HOLD_TIME_;
        holdTimeCounter_ = 0.0f;

        // reset fader
        willExitAfterFade_ = false;

        return fatalExitEvent_;
    }

    void Loop::FadeOut(
        const sf::Color & FADE_TO_COLOR, const float SPEED_MULT, const bool WILL_HOLD_FADE)
    {
        fader_.UpdateRegion(
            Display::Instance()->GetWinWidth(), Display::Instance()->GetWinHeight());

        fader_.FadeTo(FADE_TO_COLOR, SPEED_MULT);
        continueFading_ = true;
        willExitAfterFade_ = true;
        willHoldFade_ = WILL_HOLD_FADE;
    }

    void Loop::FadeIn(
        const sf::Color & FADE_FROM_COLOR, const float SPEED_MULT, const bool WILL_HOLD_FADE)
    {
        fader_.UpdateRegion(
            Display::Instance()->GetWinWidth(), Display::Instance()->GetWinHeight());

        fader_.Reset(FADE_FROM_COLOR);
        fader_.FadeTo(sf::Color::Transparent, SPEED_MULT);
        continueFading_ = true;
        willHoldFade_ = WILL_HOLD_FADE;
    }

    void Loop::SetMouseVisibility(const bool IS_VISIBLE)
    {
        Display::Instance()->SetMouseCursorVisibility(IS_VISIBLE);
    }

} // namespace sfml_util
} // namespace heroespath
