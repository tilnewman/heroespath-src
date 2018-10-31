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

#include "misc/log-macros.hpp"
#include "misc/vectors.hpp"
#include "sfml-util/callback.hpp"
#include "sfml-util/date-time.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/music-operator.hpp"
#include "sfml-util/sound-manager.hpp"
#include "sfutil/event.hpp"
#include "stage/stage-base.hpp"

#include <iomanip>

namespace heroespath
{
namespace game
{

    Loop::Loop(ActiveStages & stages, IStatusForLoop & iStatus, const ExecuteCommand FLAGS)
        : stages_(stages)
        , iStatus_(iStatus)
        , flags_(FLAGS)
        , prevKeyStrokeEventType_(sf::Event::EventType::Count)
        , prevKeyStrokeEventKey_(sf::Keyboard::Key::Unknown)
        , isMouseHovering_(false)
        , frameRateVec_(4096, 0.0f) // any number over 1000 should work here
        , frameRateSampleCount_(0)
        , prevMousePosV_(sfml_util::Display::Instance()->GetMousePosition())
        , frameMouseInfo_(false, sf::Vector2i())
    {
        iStatus_.SetLoopRunning(true);
    }

    Loop::~Loop() { iStatus_.SetLoopRunning(false); }

    void Loop::Execute()
    {
        sfml_util::Display::Instance()->SetMouseCursorVisibility(!flags_.will_hide_mouse);

        sf::Clock secondClock_;
        sf::Clock frameClock;

        try
        {
            ConsumeAndIgnoreStrayEvents();

            while (!iStatus_.IsLoopStopRequested())
            {
                sfml_util::Display::Instance()->ClearToBlack();

                frameMouseInfo_ = UpdateMouseInfo();

                const auto FRAME_TIME_SEC { frameClock.getElapsedTime().asSeconds() };
                frameClock.restart();

                RecordFramerate(FRAME_TIME_SEC);

                OncePerSecondTasks(secondClock_);

                sfml_util::SoundManager::Instance()->UpdateTime(FRAME_TIME_SEC);

                // TODO TEMP REMOVE remove this crap once all testing is in unit tests
                ExecuteNextTest();

                StopMouseHover(frameMouseInfo_.has_moved);

                HandleMouseMove();
                HandleEvents();

                UpdateTimeStages(FRAME_TIME_SEC);

                DrawStages();

                // this must remain second-to-last so that any of the code above can set a popup
                // response and it will always be handled before the loop exits
                stages_.HandlePopupResponseCallback();

                // this must remain last so that the fade is the last to be drawn
                HandleFade(FRAME_TIME_SEC);

                sfml_util::Display::Instance()->DisplayFrameBuffer();
            }
        }
        catch (const std::exception & EXCEPTION)
        {
            M_HP_LOG_FAT(
                "Exception=\"" << EXCEPTION.what()
                               << "\" thrown during game loop execution, which is "
                                  "fatal.  Re-Throwing to kill the game.");

            ExecuteCleanup();
            throw;
        }

        ExecuteCleanup();
    }

    void Loop::ExecuteCleanup() { StopMouseHoverImpl(); }

    const MouseThisFrame Loop::UpdateMouseInfo()
    {
        const auto NEW_MOUSE_POS_VI { sfml_util::Display::Instance()->GetMousePosition() };
        const auto HAS_MOUSE_MOVED { (NEW_MOUSE_POS_VI != prevMousePosV_) };
        prevMousePosV_ = NEW_MOUSE_POS_VI;
        return MouseThisFrame(HAS_MOUSE_MOVED, NEW_MOUSE_POS_VI);
    }

    void Loop::StopMouseHover(const bool HAS_MOUSE_MOVED)
    {
        if (isMouseHovering_ && HAS_MOUSE_MOVED)
        {
            StopMouseHoverImpl();
        }
    }

    void Loop::StopMouseHoverImpl()
    {
        if (!isMouseHovering_)
        {
            return;
        }

        isMouseHovering_ = false;

        auto handleStopMouseHover
            = [](game::ActiveStages &, const stage::IStageUPtr_t & iStageUPtr) {
                  iStageUPtr->SetMouseHover(sf::Vector2f(0.0f, 0.0f), false);
              };

        stages_.ExecuteOn(game::WhichStages::All, handleStopMouseHover);
    }

    void Loop::OncePerSecondTasks(sf::Clock & secondClock)
    {
        if (secondClock.getElapsedTime().asSeconds() < 1.0f)
        {
            return;
        }

        secondClock.restart();

        OncePerSecondTaskLogFrameRate();
        OncePerSecondTaskStartMouseHover();
        OncePerSecondTaskCheckIfDisplayOpen();
    }

    void Loop::OncePerSecondTaskLogFrameRate()
    {
        if (frameRateSampleCount_ < 3)
        {
            return;
        }

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
            "Framerate: " << (frameRateSampleCount_ - 1) << " frames in the last second: [" << min
                          << ", " << AVERAGE_FRAMERATE << ", " << max
                          << "], std_dev=" << STANDARD_DEVIATION);

        frameRateSampleCount_ = 0;
    }

    void Loop::OncePerSecondTaskStartMouseHover()
    {
        // don't show mouseovers while fading, if the mouse is moving, or if already showing
        if (iStatus_.IsFading() || frameMouseInfo_.has_moved || isMouseHovering_)
        {
            return;
        }

        isMouseHovering_ = true;

        auto handleStartMouseHover
            = [MOUSE_POS_V = frameMouseInfo_.pos_vf](
                  game::ActiveStages &, const stage::IStageUPtr_t & iStageUPtr) {
                  iStageUPtr->SetMouseHover(MOUSE_POS_V, true);
              };

        stages_.ExecuteOn(game::WhichStages::Foreground, handleStartMouseHover);
    }

    void Loop::OncePerSecondTaskCheckIfDisplayOpen()
    {
        if (sfml_util::Display::Instance()->IsOpen() == false)
        {
            iStatus_.LoopStopRequest();
        }
    }

    void Loop::HandleMouseMove()
    {
        if (flags_.will_mouse_ignore || (frameMouseInfo_.has_moved == false))
        {
            return;
        }

        auto handleMouseMove = [MOUSE_THIS_FRAME = frameMouseInfo_](
                                   game::ActiveStages &, const stage::IStageUPtr_t & iStageUPtr) {
            iStageUPtr->UpdateMousePos(MOUSE_THIS_FRAME.pos_vi);
        };

        stages_.ExecuteOn(game::WhichStages::Foreground, handleMouseMove);
    }

    void Loop::HandleFade(const float FRAME_TIME_SEC)
    {
        if (!iStatus_.IsFading())
        {
            return;
        }

        if (sfml_util::Display::Instance()->UpdateTimeForFade(FRAME_TIME_SEC) == false)
        {
            iStatus_.StopFading();
            iStatus_.LoopStopRequest();
            stages_.SetIsFadingForAllStages(false);
        }

        sfml_util::Display::Instance()->DrawFade();
    }

    void Loop::HandleEvents()
    {
        for (const auto & EVENT : sfml_util::Display::Instance()->PollEvents())
        {
            HandleEvent(EVENT);
        }
    }

    void Loop::HandleEvent(const sf::Event & EVENT)
    {
        if (HandleEventIfWindowClosed(EVENT))
        {
            return;
        }

        if ((EVENT.type == sf::Event::KeyPressed) || (EVENT.type == sf::Event::KeyReleased))
        {
            HandleEventKeyStroke(EVENT);
            return;
        }

        if (EVENT.mouseButton.button == sf::Mouse::Left)
        {
            if (EVENT.type == sf::Event::MouseButtonPressed)
            {
                HandleEventMouseButtonLeftPressed();
                return;
            }
            else if (EVENT.type == sf::Event::MouseButtonReleased)
            {
                HandleEventMouseButtonLeftReleased();
                return;
            }
        }

        if (EVENT.type != sf::Event::EventType::MouseMoved)
        {
            M_HP_LOG_DBG("unhandled " << ToString(EVENT));
        }
    }

    void Loop::HandleEventKeyStroke(const sf::Event & EVENT)
    {
        const auto IS_KEYPRESS { (EVENT.type == sf::Event::KeyPressed) };

        const auto IS_KEY_STROKE_EVENT_DIFFERENT_FROM_PREV {
            (EVENT.type != prevKeyStrokeEventType_) || (EVENT.key.code != prevKeyStrokeEventKey_)
        };

        if (IS_KEY_STROKE_EVENT_DIFFERENT_FROM_PREV)
        {
            prevKeyStrokeEventType_ = EVENT.type;
            prevKeyStrokeEventKey_ = EVENT.key.code;
        }

        auto makeLogString = [&]() {
            return std::string("Key-") + ((IS_KEYPRESS) ? "Press" : "Release") + ": "
                + sfutil::ToString(EVENT.key.code);
        };

        // allow screenshots even if keystrokes are ignored
        if (IS_KEY_STROKE_EVENT_DIFFERENT_FROM_PREV && (EVENT.key.code == sf::Keyboard::F12))
        {
            M_HP_LOG(makeLogString() + " causing screenshot save.");
            sfml_util::Display::Instance()->TakeScreenshot();
            return;
        }

        // TEMP TODO REMOVE AFTER TESTING
        // allow F1 to instant kill the game even if keystrokes are ignored
        if (IS_KEY_STROKE_EVENT_DIFFERENT_FROM_PREV && (EVENT.key.code == sf::Keyboard::F1))
        {
            M_HP_LOG(makeLogString() + " forcing the game to exit.");
            iStatus_.GameExitRequest();
            iStatus_.LoopStopRequest();
            return;
        }

        if (flags_.will_keystroke_exit)
        {
            M_HP_LOG(makeLogString() + " forcing game loop to exit.");
            iStatus_.LoopStopRequest();
        }

        if (flags_.will_keystroke_ignore)
        {
            M_HP_LOG("Ignored " + makeLogString());
            return;
        }

        auto handleKeyPressOrRelease
            = [EVENT](game::ActiveStages &, const stage::IStageUPtr_t & iStageUPtr) {
                  if (EVENT.type == sf::Event::KeyPressed)
                  {
                      iStageUPtr->KeyPress(EVENT.key);
                  }
                  else
                  {
                      iStageUPtr->KeyRelease(EVENT.key);
                  }
              };

        stages_.ExecuteOn(game::WhichStages::Foreground, handleKeyPressOrRelease);
    }

    void Loop::HandleEventMouseButtonLeftPressed()
    {
        if (flags_.will_mouse_click_exit)
        {
            M_HP_LOG("mouse click forcing game loop to exit");
            iStatus_.LoopStopRequest();
            return;
        }

        if (flags_.will_mouse_ignore)
        {
            M_HP_LOG("left mouse button press ignored");
            return;
        }

        M_HP_LOG_DBG(
            "mouse button (left) pressed at " << frameMouseInfo_.pos_vf << " (before handling)");

        auto handleMouseButtonPressed
            = [MOUSE_POS_VF = frameMouseInfo_.pos_vf](
                  game::ActiveStages &, const stage::IStageUPtr_t & iStageUPtr) {
                  iStageUPtr->UpdateMouseDown(MOUSE_POS_VF);
              };

        stages_.ExecuteOn(game::WhichStages::Foreground, handleMouseButtonPressed);

        M_HP_LOG_DBG(
            "mouse button (left) pressed at " << frameMouseInfo_.pos_vf << " (after handling)");
    }

    void Loop::HandleEventMouseButtonLeftReleased()
    {
        if (flags_.will_mouse_ignore)
        {
            M_HP_LOG("left mouse button release ignored");
            return;
        }

        M_HP_LOG_DBG(
            "mouse button (left) released at " << frameMouseInfo_.pos_vf << " (before handling)");

        auto handleLeftMouseButtonRelease
            = [MOUSE_POS_VF = frameMouseInfo_.pos_vf](
                  game::ActiveStages & stages, const stage::IStageUPtr_t & iStageUPtr) {
                  const auto IENTITY_WITH_FOCUS_PTR_OPT { iStageUPtr->UpdateMouseUp(MOUSE_POS_VF) };

                  if (IENTITY_WITH_FOCUS_PTR_OPT)
                  {
                      stages.Focus(IENTITY_WITH_FOCUS_PTR_OPT.value());
                  }
              };

        stages_.ExecuteOn(game::WhichStages::Foreground, handleLeftMouseButtonRelease);

        M_HP_LOG_DBG(
            "mouse button (left) released at " << frameMouseInfo_.pos_vf << " (after handling)");
    }

    bool Loop::HandleEventIfWindowClosed(const sf::Event & EVENT)
    {
        if (EVENT.type == sf::Event::Closed)
        {
            M_HP_LOG_ERR("unexpected window close event");
            iStatus_.GameExitRequest();
            iStatus_.LoopStopRequest();
            return true;
        }
        else
        {
            return false;
        }
    }

    void Loop::ConsumeAndIgnoreStrayEvents()
    {
        for (const auto & EVENT : sfml_util::Display::Instance()->PollEvents())
        {
            if (HandleEventIfWindowClosed(EVENT))
            {
                break;
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
    //        for (auto & iStageUPtr : stages_.non_popup_uvec)
    //        {
    //            iStageUPtr->UpdateMouseWheel(NEW_MOUSE_POS_F, EVENT.mouseWheelScroll.delta);
    //        }
    //    }
    //}

    void Loop::UpdateTimeStages(const float FRAME_TIME_SEC)
    {
        auto handleTimeUpdate
            = [FRAME_TIME_SEC](game::ActiveStages &, const stage::IStageUPtr_t & iStageUPtr) {
                  iStageUPtr->UpdateTime(FRAME_TIME_SEC);
              };

        stages_.ExecuteOn(game::WhichStages::All, handleTimeUpdate, true);
    }

    void Loop::DrawStages()
    {
        auto handleDrawing = [](game::ActiveStages &, const stage::IStageUPtr_t & iStageUPtr) {
            sfml_util::Display::Instance()->DrawStage(iStageUPtr.get());
        };

        stages_.ExecuteOn(game::WhichStages::All, handleDrawing, true);
    }

    void Loop::ExecuteNextTest()
    {
        auto handlePerformNextTest
            = [](game::ActiveStages &, const stage::IStageUPtr_t & iStageUPtr) {
                  iStageUPtr->PerformNextTest();
              };

        stages_.ExecuteOn(game::WhichStages::NonPopupOnly, handlePerformNextTest);
    }

    void Loop::RecordFramerate(const float FRAME_TIME_SEC)
    {
        const float FRAME_TIME_MIN_SEC { 0.000001f };
        const auto FRAME_TIME_SEC_TO_USE { std::max(FRAME_TIME_SEC, FRAME_TIME_MIN_SEC) };

        frameRateVec_[frameRateSampleCount_++] = (1.0f / FRAME_TIME_SEC_TO_USE);

        if (frameRateSampleCount_ >= frameRateVec_.size())
        {
            frameRateVec_.resize(frameRateSampleCount_ * 2);
        }
    }

} // namespace game
} // namespace heroespath
