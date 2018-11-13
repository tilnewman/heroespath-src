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

#include "gui/date-time.hpp"
#include "gui/display.hpp"
#include "gui/music-operator.hpp"
#include "gui/sound-manager.hpp"
#include "misc/callback.hpp"
#include "misc/log-macros.hpp"
#include "misc/timing.hpp"
#include "sfutil/event.hpp"
#include "stage/stage-base.hpp"

#include <algorithm>
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
        , prevMousePosV_(gui::Display::Instance()->GetMousePosition())
        , frameMouseInfo_(false, sf::Vector2i())
        , toLogEvents_()
    {
        toLogEvents_.reserve(10);
        iStatus_.SetLoopRunning(true);
    }

    Loop::~Loop() { iStatus_.SetLoopRunning(false); }

    void Loop::Execute()
    {
        gui::Display::Instance()->SetMouseCursorVisibility(!flags_.will_hide_mouse);

        sf::Clock secondClock_;
        sf::Clock frameClock;

        try
        {
            ConsumeAndIgnoreStrayEvents();

            misc::TimeTrials framerateTrials("Framerate", TimeRes::Milli, 200, 0.0);

            const std::size_t FRAMERATE_COLLECTER_INDEX { framerateTrials.AddCollecter(
                flags_.ToString()) };

            while (!iStatus_.IsLoopStopRequested())
            {
                {
                    misc::ScopedContestTimer scopedFramerateTimer(
                        framerateTrials, FRAMERATE_COLLECTER_INDEX);

                    gui::Display::Instance()->ClearToBlack();

                    // TODO TEMP REMOVE remove this crap once all testing is in unit tests
                    ExecuteNextTest();

                    frameMouseInfo_ = UpdateMouseInfo();
                    StopMouseHover(frameMouseInfo_.has_moved);
                    HandleMouseMove();
                    HandleEvents();

                    const auto FRAME_TIME_SEC { frameClock.getElapsedTime().asSeconds() };
                    frameClock.restart();

                    UpdateTimeFade(FRAME_TIME_SEC);
                    UpdateTimeAudio(FRAME_TIME_SEC);
                    UpdateTimeStages(FRAME_TIME_SEC);

                    OncePerSecondTasks(secondClock_);

                    Draw();

                    // this must remain last (just before display) so that any of the code above
                    // can set a popup response and it will always be handled before the loop
                    // exits
                    stages_.HandlePopupResponseCallback();

                    gui::Display::Instance()->DisplayFrameBuffer();
                }

                if (toLogEvents_.empty() == false)
                {
                    for (const auto & EVENT : toLogEvents_)
                    {
                        M_HP_LOG_DEF("Event: " << ToString(EVENT));
                    }
                    toLogEvents_.clear();
                }
            }

            framerateTrials.EndAllContests();
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
        const auto NEW_MOUSE_POS_VI { gui::Display::Instance()->GetMousePosition() };
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

        auto handleStopMouseHover = [](stage::IStagePtr_t iStagePtr) {
            iStagePtr->SetMouseHover(sf::Vector2f(0.0f, 0.0f), false);
            return boost::none;
        };

        stages_.ExecuteOnPopupStage(handleStopMouseHover);
        stages_.ExecuteOnNonPopupStages(handleStopMouseHover);
    }

    void Loop::OncePerSecondTasks(sf::Clock & secondClock)
    {
        if (secondClock.getElapsedTime().asSeconds() < 1.0f)
        {
            return;
        }

        secondClock.restart();

        OncePerSecondTaskStartMouseHover();
        OncePerSecondTaskCheckIfDisplayOpen();
    }

    void Loop::OncePerSecondTaskStartMouseHover()
    {
        // don't show mouseovers while fading, if the mouse is moving, or if already showing
        if (iStatus_.GetFadeStatus().is_fading || frameMouseInfo_.has_moved || isMouseHovering_)
        {
            return;
        }

        isMouseHovering_ = true;

        auto handleStartMouseHover
            = [MOUSE_POS_V = frameMouseInfo_.pos_vf](stage::IStagePtr_t iStagePtr) {
                  iStagePtr->SetMouseHover(MOUSE_POS_V, true);
                  return boost::none;
              };

        stages_.ExecuteOnForegroundStages(handleStartMouseHover);
    }

    void Loop::OncePerSecondTaskCheckIfDisplayOpen()
    {
        if (gui::Display::Instance()->IsOpen() == false)
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

        auto handleMouseMove = [MOUSE_THIS_FRAME = frameMouseInfo_](stage::IStagePtr_t iStagePtr) {
            iStagePtr->UpdateMousePos(MOUSE_THIS_FRAME.pos_vi);
            return boost::none;
        };

        stages_.ExecuteOnForegroundStages(handleMouseMove);
    }

    void Loop::UpdateTimeFade(const float FRAME_TIME_SEC)
    {
        if (iStatus_.GetFadeStatus().is_fading == false)
        {
            return;
        }

        iStatus_.SetFadeCurrentColor(gui::Display::Instance()->UpdateFadeColor(FRAME_TIME_SEC));

        if (gui::Display::Instance()->IsFadeFinished())
        {
            iStatus_.SetFadeTargetColorReached();
            iStatus_.LoopStopRequest();
            stages_.SetIsFadingForAllStages(false);
        }
    }

    void Loop::HandleEvents()
    {
        for (const auto & EVENT : gui::Display::Instance()->PollEvents())
        {
            HandleEvent(EVENT);
        }
    }

    void Loop::HandleEvent(const sf::Event & EVENT)
    {
        toLogEvents_.emplace_back(EVENT);

        if (HandleEventIfWindowClosed(EVENT))
        {
            return;
        }

        if ((EVENT.type == sf::Event::KeyPressed) || (EVENT.type == sf::Event::KeyReleased))
        {
            HandleEventKeyStroke(EVENT);
            return;
        }

        if ((EVENT.type == sf::Event::MouseButtonPressed)
            && (EVENT.mouseButton.button == sf::Mouse::Left))
        {
            HandleEventMouseButtonLeftPressed();
            return;
        }

        if ((EVENT.type == sf::Event::MouseButtonReleased)
            && (EVENT.mouseButton.button == sf::Mouse::Left))
        {
            HandleEventMouseButtonLeftReleased();
            return;
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

        // allow screenshots even if keystrokes are ignored
        if (IS_KEY_STROKE_EVENT_DIFFERENT_FROM_PREV && (IS_KEYPRESS == false)
            && (EVENT.key.code == sf::Keyboard::F12))
        {
            gui::Display::Instance()->TakeScreenshot();
            return;
        }

        // TEMP TODO REMOVE AFTER TESTING
        // allow F1 to instant kill the game even if keystrokes are ignored
        if (IS_KEY_STROKE_EVENT_DIFFERENT_FROM_PREV && (IS_KEYPRESS == false)
            && (EVENT.key.code == sf::Keyboard::F1))
        {
            iStatus_.GameExitRequest();
            iStatus_.LoopStopRequest();
            return;
        }

        if (flags_.will_keystroke_exit)
        {
            iStatus_.LoopStopRequest();
        }

        if (flags_.will_keystroke_ignore)
        {
            return;
        }

        auto handleKeyPressOrRelease = [EVENT](stage::IStagePtr_t iStagePtr) {
            if (EVENT.type == sf::Event::KeyPressed)
            {
                iStagePtr->KeyPress(EVENT.key);
            }
            else
            {
                iStagePtr->KeyRelease(EVENT.key);
            }
            return boost::none;
        };

        stages_.ExecuteOnForegroundStages(handleKeyPressOrRelease);
    }

    void Loop::HandleEventMouseButtonLeftPressed()
    {
        if (flags_.will_mouse_click_exit)
        {
            iStatus_.LoopStopRequest();
            return;
        }

        if (flags_.will_mouse_ignore)
        {
            return;
        }

        auto handleMouseButtonPressed
            = [MOUSE_POS_VF = frameMouseInfo_.pos_vf](stage::IStagePtr_t iStagePtr) {
                  iStagePtr->UpdateMouseDown(MOUSE_POS_VF);
                  return boost::none;
              };

        stages_.ExecuteOnForegroundStages(handleMouseButtonPressed);
    }

    void Loop::HandleEventMouseButtonLeftReleased()
    {
        if (flags_.will_mouse_ignore)
        {
            return;
        }

        auto handleLeftMouseButtonRelease
            = [MOUSE_POS_VF = frameMouseInfo_.pos_vf](stage::IStagePtr_t iStagePtr) {
                  return iStagePtr->UpdateMouseUp(MOUSE_POS_VF);
              };

        stages_.ExecuteOnForegroundStages(handleLeftMouseButtonRelease);
    }

    bool Loop::HandleEventIfWindowClosed(const sf::Event & EVENT)
    {
        if (EVENT.type == sf::Event::Closed)
        {
            toLogEvents_.emplace_back(EVENT);
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
        for (const auto & EVENT : gui::Display::Instance()->PollEvents())
        {
            if (HandleEventIfWindowClosed(EVENT))
            {
                break;
            }
        }
    }

    // I had problems getting smooth mouse wheel motion so I'm pausing this code
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
        auto handleTimeUpdate = [FRAME_TIME_SEC](stage::IStagePtr_t iStagePtr) {
            iStagePtr->UpdateTime(FRAME_TIME_SEC);
            return boost::none;
        };

        stages_.ExecuteOnNonPopupStages(handleTimeUpdate);
        stages_.ExecuteOnPopupStage(handleTimeUpdate);
    }

    void Loop::Draw()
    {
        auto handleDrawing = [](stage::IStagePtr_t iStagePtr) {
            gui::Display::Instance()->DrawStage(iStagePtr);
            return boost::none;
        };

        stages_.ExecuteOnNonPopupStages(handleDrawing);

        if (iStatus_.GetFadeStatus().will_draw_under_popup)
        {
            gui::Display::Instance()->DrawFade();
        }

        stages_.ExecuteOnPopupStage(handleDrawing);

        if (iStatus_.GetFadeStatus().will_draw_under_popup == false)
        {
            gui::Display::Instance()->DrawFade();
        }
    }

    void Loop::ExecuteNextTest()
    {
        auto handlePerformNextTest = [](stage::IStagePtr_t iStagePtr) {
            iStagePtr->PerformNextTest();
            return boost::none;
        };

        stages_.ExecuteOnNonPopupStages(handlePerformNextTest);
    }

    void Loop::UpdateTimeAudio(const float FRAME_TIME_SEC)
    {
        gui::SoundManager::Instance()->UpdateTime(FRAME_TIME_SEC);
    }

} // namespace game
} // namespace heroespath
