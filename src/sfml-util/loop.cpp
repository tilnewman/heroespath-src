// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
//
// loop.cpp
//
#include "loop.hpp"

#include "sfml-util/stage.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/sound-manager.hpp"
#include "sfml-util/music-operator.hpp"
#include "sfml-util/date-time.hpp"

#include "log/log-macros.hpp"
#include "game/loop-manager.hpp"

#include "popup/i-popup-callback.hpp"

#include "misc/vectors.hpp"

#include <boost/filesystem.hpp>

#include <iomanip>


namespace heroespath
{
namespace sfml_util
{

    const float Loop::NO_HOLD_TIME_(-1.0f);
    const float Loop::INVALID_MUSIC_FADE_VALUE_(-1.0f);


    Loop::Loop(const std::string & NAME)
    :
        NAME_                (std::string(NAME).append("_Loop")),
        stagePVec_           (),
        clock_               (),
        oneSecondTimerSec_   (0.0f),
        winPtr_              (Display::Instance()->GetWindow()),
        fader_               (0.0f, 0.0f, Display::Instance()->GetWinWidth(), Display::Instance()->GetWinHeight()),
        willHoldFade_        (false),
        continueFading_      (false),
        willExitAfterFade_   (false),
        willExit_            (false),
        popupStagePtr_       (),
        elapsedTimeSec_      (0.0f),
        fatalExitEvent_      (false),
        holdTime_            (NO_HOLD_TIME_),
        holdTimeCounter_     (0.0f),
        willExitOnKeypress_  (false),
        willExitOnMouseclick_(false),
        entityWithFocusSPtr_ (nullptr),
        willIgnoreMouse_     (false),
        willIgnoreKeystrokes_(false),
        popupInfo_           (" ", sfml_util::gui::TextInfo(" ")),//okay to be initially without meaningful values
        hasFadeStarted_      (false),
        prevEventType_       (sf::Event::EventType::Count),
        prevKeyPressed_      (sf::Keyboard::Key::Unknown),
        isMouseHovering_     (false),
        takeScreenshot_      (false),
        popupCallbackPtr_    (),
        state_               (LoopState::Intro),
        frameRateVec_        (),
        frameRateSampleCount_(0),
        willLogFrameRate_    (false)
    {}


    Loop::~Loop()
    {
        FreeAllStages();
    }


    void Loop::ConsumeEvents()
    {
        sf::Event e;
        while (winPtr_->pollEvent(e))
        {}
    }


    void Loop::RemoveFocus()
    {
        entityWithFocusSPtr_ = nullptr;

        for (auto & nextStagePtr : stagePVec_)
        {
            nextStagePtr->RemoveFocus();
        }
    }


    bool Loop::SetFocus(const gui::IGuiEntityPtr_t ENTITY_PTR)
    {
        bool foundStageOwningEntityWithFocus(false);

        entityWithFocusSPtr_ = ENTITY_PTR;

        for (auto & nextStagePtr : stagePVec_)
        {
            if (nextStagePtr->SetFocus(ENTITY_PTR))
            {
                foundStageOwningEntityWithFocus = true;
            }
        }

        return foundStageOwningEntityWithFocus;
    }


    void Loop::AssignPopupCallbackHandlerInfo(
        heroespath::popup::IPopupHandler_t * const HANDLER_PTR,
        const heroespath::popup::PopupInfo & POPUP_INFO)
    {
        popupInfo_ = POPUP_INFO;
        popupCallbackPtr_ = HANDLER_PTR;
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


    void Loop::TestingImageSet(const sf::Texture & TEXTURE)
    {
        for (auto & nextStagePtr : stagePVec_)
        {
            nextStagePtr->TestingImageSet(TEXTURE);
        }
    }


    void Loop::AddStage(IStagePtr_t stageSPtr)
    {
        stagePVec_.push_back(stageSPtr);
    }


    void Loop::FreeAllStages()
    {
        auto const NUM_STAGES{ stagePVec_.size() };
        for (std::size_t i(0); i<NUM_STAGES; ++i)
        {
            if (stagePVec_[i] != nullptr)
            {
                delete stagePVec_[i];
                stagePVec_[i] = nullptr;
            }
        }

        stagePVec_.clear();

        FreePopupStage();
    }


    void Loop::FreePopupStage()
    {
        if (popupStagePtr_ != nullptr)
        {
            delete popupStagePtr_;
            popupStagePtr_ = nullptr;
        }
    }


    void Loop::LogFrameRate()
    {
        if (willLogFrameRate_ && (frameRateSampleCount_ > 0))
        {
            //find min, max, and average framerate
            auto min{ frameRateVec_[0] };
            auto max{ 0.0f };
            auto sum{ 0.0f };

            //Skip the first framerate number because it includes time spent
            //logging the prev framerate.
            for (std::size_t i(1); i < frameRateSampleCount_; ++i)
            {
                auto const NEXT_FRAMERATE{ frameRateVec_[i] };

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

            auto const AVERAGE_FRAMERATE{ sum / static_cast<float>(frameRateSampleCount_) };

            const float STANDARD_DEVIATION{ heroespath::misc::Vector::StandardDeviation(
                frameRateVec_,
                frameRateSampleCount_,
                AVERAGE_FRAMERATE) };

            M_HP_LOG("Frame rate min=" << min
                << ", max=" << max
                << ", count=" << frameRateSampleCount_ - 1
                << ", avg=" << AVERAGE_FRAMERATE
                << ", std_dev=" << STANDARD_DEVIATION << ":  ");
        }
        else
        {
            willLogFrameRate_ = true;
        }

        frameRateSampleCount_ = 0;
    }


    bool Loop::ProcessOneSecondTasks(
        const sf::Vector2i & NEW_MOUSE_POS,
        const bool HAS_MOUSE_MOVED)
    {
        auto willProcessOneSecondTasks{ false };

        if (false == continueFading_)
        {
            oneSecondTimerSec_ += elapsedTimeSec_;
        }

        if (oneSecondTimerSec_ > 1.0f)
        {
            willProcessOneSecondTasks = true;

            oneSecondTimerSec_ = 0.0f;

            LogFrameRate();

            if ((false == continueFading_) &&
                (HAS_MOUSE_MOVED == false) &&
                (isMouseHovering_ == false))
            {
                isMouseHovering_ = true;

                auto const NEW_MOUSE_POS_F{ sfml_util::ConvertVector<int, float>(NEW_MOUSE_POS) };

                //popup stages process exclusively when present
                if (nullptr == popupStagePtr_)
                {
                    for (auto & nextStagePtr : stagePVec_)
                    {
                        nextStagePtr->SetMouseHover(NEW_MOUSE_POS_F, true);
                    }
                }
                else
                {
                    popupStagePtr_->SetMouseHover(NEW_MOUSE_POS_F, true);
                }
            }
        }

        return willProcessOneSecondTasks;
    }


    void Loop::ProcessMouseHover(
        const sf::Vector2i & NEW_MOUSE_POS,
        bool HAS_MOUSE_MOVED)
    {
        if (isMouseHovering_ && HAS_MOUSE_MOVED)
        {
            isMouseHovering_ = false;

            auto const MOUSE_POS_NEW_F{ sfml_util::ConvertVector<int, float>(NEW_MOUSE_POS) };

            //popup stages process exclusively when present
            if (nullptr == popupStagePtr_)
            {
                for (auto & nextStagePtr : stagePVec_)
                {
                    nextStagePtr->SetMouseHover(MOUSE_POS_NEW_F, false);
                }
            }
            else
            {
                popupStagePtr_->SetMouseHover(MOUSE_POS_NEW_F, false);
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
            winPtr_->draw(fader_);
        }

        if (continueFading_)
        {
            heroespath::game::LoopManager::Instance()->HandleTransitionBeforeFade();

            continueFading_ = ! fader_.Update(elapsedTimeSec_);

            hasFadeStarted_ = true;

            if ((false == continueFading_) && willExitAfterFade_)
            {
                willExit_ = true;
            }
        }
    }


    void Loop::ProcessPopup()
    {
        if (nullptr != popupStagePtr_)
        {
            popupStagePtr_->UpdateTime(elapsedTimeSec_);

            sf::RenderStates states;
            popupStagePtr_->Draw( * winPtr_, states);
        }
    }


    void Loop::ProcessEvents(
        const sf::Vector2i & NEW_MOUSE_POS,
        bool HAS_MOUSE_MOVED)
    {
        if ((false == willIgnoreMouse_) && HAS_MOUSE_MOVED)
        {
            ProcessMouseMove(NEW_MOUSE_POS);
        }

        sf::Event e;
        if (winPtr_->pollEvent(e))
        {
            //unexpected window close exit condition
            if (e.type == sf::Event::Closed)
            {
                M_HP_LOG(NAME_ << " UNEXPECTED WINDOW CLOSE");
                winPtr_->close();
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
                    auto const NEW_MOUSE_POS_F{
                        sfml_util::ConvertVector<int, float>(NEW_MOUSE_POS) };

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
                    auto const NEW_MOUSE_POS_F{
                        sfml_util::ConvertVector<int, float>(NEW_MOUSE_POS) };

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
               << ": "   << sfml_util::KeyCodeToString(EVENT.key.code)
               << ((willIgnoreKeystrokes_) ? " IGNORED" : "");

            M_HP_LOG(ss.str());
        }

        //allow screenshots even if keystrokes are ignored
        if (EVENT.key.code == sf::Keyboard::F12)
        {
            takeScreenshot_ = true;
        }

        if (willIgnoreKeystrokes_)
        {
            return;
        }

        //popup stages process exclusively when present
        if (nullptr == popupStagePtr_)
        {
            //give event to stages for processing
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
        else
        {
            if (EVENT.type == sf::Event::KeyPressed)
            {
                popupStagePtr_->KeyPress(EVENT.key);
            }
            else
            {
                popupStagePtr_->KeyRelease(EVENT.key);
            }
        }

        //...and to exit the app on F1 keypresses
        if ((EVENT.type == sf::Event::KeyReleased) && (EVENT.key.code == sf::Keyboard::F1))
        {
            M_HP_LOG(NAME_ << " F1 KEY RELEASED.  Bail.");
            sfml_util::SoundManager::Instance()->PlaySfx_Keypress();
            heroespath::game::LoopManager::Instance()->SetExitSuccess(false);
            heroespath::game::LoopManager::Instance()->TransitionTo_Exit();
        }
        if ((EVENT.key.code == sf::Keyboard::Escape)
            && (heroespath::game::LoopManager::Instance()->GetState() == LoopState::Test))
        {
            M_HP_LOG(NAME_ << " ESCAPE KEY RELEASED WHILE TESTING.  Bail.");
            heroespath::game::LoopManager::Instance()->TransitionTo_Exit();
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
        if (nullptr == popupStagePtr_)
        {
            for (auto & nextStagePtr : stagePVec_)
            {
                nextStagePtr->UpdateMousePos(NEW_MOUSE_POS);
            }
        }
        else
        {
            popupStagePtr_->UpdateMousePos(NEW_MOUSE_POS);
        }
    }


    void Loop::ProcessMouseButtonLeftPressed(const sf::Vector2f & MOUSE_POS_V)
    {
        if (nullptr == popupStagePtr_)
        {
            for (auto & nextStagePtr : stagePVec_)
            {
                nextStagePtr->UpdateMouseDown(MOUSE_POS_V);
            }
        }
        else
        {
            popupStagePtr_->UpdateMouseDown(MOUSE_POS_V);
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
        if (nullptr == popupStagePtr_)
        {
            for (auto & nextStagePtr : stagePVec_)
            {
                auto newEntityWithFocusPtr{ nextStagePtr->UpdateMouseUp(MOUSE_POS_V) };

                if (newEntityWithFocusPtr != nullptr)
                {
                    M_HP_LOG("MouseButtonLeftReleased caused focus in stage \""
                        << nextStagePtr->GetStageName() << "\" on entity \""
                        << newEntityWithFocusPtr->GetEntityName() << "\"");

                    RemoveFocus();
                    newEntityWithFocusPtr->SetHasFocus(true);
                    SetFocus(newEntityWithFocusPtr);
                }
            }
        }
        else
        {
            auto newEntityWithFocusPtr{ popupStagePtr_->UpdateMouseUp(MOUSE_POS_V) };

            if (newEntityWithFocusPtr != nullptr)
            {
                RemoveFocus();
                newEntityWithFocusPtr->SetHasFocus(true);
                SetFocus(newEntityWithFocusPtr);
            }
        }
    }


    void Loop::ProcessMouseWheelRoll(
        const sf::Event & EVENT,
        const sf::Vector2i & NEW_MOUSE_POS)
    {
        auto const NEW_MOUSE_POS_F{ sfml_util::ConvertVector<int, float>(NEW_MOUSE_POS) };

        if (nullptr == popupStagePtr_)
        {
            for (auto & nextStagePtr : stagePVec_)
            {
                nextStagePtr->UpdateMouseWheel(NEW_MOUSE_POS_F, EVENT.mouseWheelScroll.delta);
            }
        }
        else
        {
            popupStagePtr_->UpdateMouseWheel(NEW_MOUSE_POS_F, EVENT.mouseWheelScroll.delta);
        }
    }


    void Loop::ProcessPopupCallback()
    {
        if (popupCallbackPtr_ == nullptr)
        {
            return;
        }

        auto const POPUP_RESPONSE_ENUM{ heroespath::game::LoopManager::Instance()->GetPopupResponse() };
        auto const POPUP_SELECTION{ heroespath::game::LoopManager::Instance()->GetPopupSelection() };

        if (POPUP_RESPONSE_ENUM != heroespath::popup::ResponseTypes::None)
        {
            M_HP_LOG("PopupCallback resp=\""
                << heroespath::popup::ResponseTypes::ToString(POPUP_RESPONSE_ENUM)
                << "\" with selection=" << POPUP_SELECTION
                << " to popup=\"" << popupInfo_.Name() << "\"");

            const heroespath::popup::PopupResponse POPUP_RESPONSE_OBJ(
                popupInfo_,
                POPUP_RESPONSE_ENUM,
                POPUP_SELECTION);

            auto const WILL_RESET_CALLBACKHANDLER{
                popupCallbackPtr_->HandleCallback(POPUP_RESPONSE_OBJ) };

            heroespath::game::LoopManager::Instance()->ClearPopupResponse();

            if (WILL_RESET_CALLBACKHANDLER)
            {
                popupCallbackPtr_ = nullptr;
            }
        }
    }


    void Loop::ProcessScreenshot()
    {
        if (takeScreenshot_)
        {
            takeScreenshot_ = false;

            //acquire screenshot
            const sf::Vector2u WINDOW_SIZE(Display::Instance()->GetWindow()->getSize());
            sf::Texture texture;
            if (texture.create(WINDOW_SIZE.x, WINDOW_SIZE.y) == false)
            {
                M_HP_LOG_ERR("texture.create(" << WINDOW_SIZE.x << "x" << WINDOW_SIZE.y << ") "
                    << "returned false. Unable to take screenshot.");
                return;
            }

            texture.update( * Display::Instance()->GetWindow());

            const sf::Image SCREENSHOT_IMAGE(texture.copyToImage());

            namespace bfs = boost::filesystem;

            //establish the path
            const bfs::path DIR_OBJ(bfs::system_complete(bfs::current_path() / bfs::path("screenshots")));

            //create directory if missing
            if (bfs::exists(DIR_OBJ) == false)
            {
                boost::system::error_code ec;
                if (bfs::create_directory(DIR_OBJ, ec) == false)
                {
                    M_HP_LOG_ERR("sfml_util::Loop::ProcessScreenshot() was unable to create the "
                        << "screenshot directory \"" << DIR_OBJ.string()
                        << "\".  Error code=" << ec << ".  Unable to take screenshot.");
                    return;
                }
            }

            //find the next available filename
            const sfml_util::DateTime DATE_TIME(sfml_util::DateTime::CurrentDateTime());
            std::ostringstream ss;
            bfs::path filePathObj;
            for (std::size_t i(0); ; ++i)
            {
                ss.str("");
                ss << "screenshot"
                    << "_" << DATE_TIME.date.year
                    << "_" << DATE_TIME.date.month
                    << "_" << DATE_TIME.date.day
                    << "__" << DATE_TIME.time.hours
                    << "_" << DATE_TIME.time.minutes
                    << "_" << DATE_TIME.time.seconds;

                if (i > 0)
                {
                    ss << "__" << i;
                }

                ss << ".png";

                filePathObj = (DIR_OBJ / bfs::path(ss.str()));

                if (bfs::exists(filePathObj) == false)
                {
                    break;
                }

                if (i >= 10000)
                {
                    M_HP_LOG_ERR("Loop::ProcessScreenshot() failed to find an available filename"
                        << "after 10,000 tries.  Either 10,000 screenshots have been saved or "
                        << "there is something wrong with this code.  Unable to take screenshot");
                    return;
                }
            }

            if (SCREENSHOT_IMAGE.saveToFile(filePathObj.string()) == false)
            {
                M_HP_LOG("Loop::ProcessScreenshot() failed screenshot saveToFile() \""
                    << filePathObj.string() << "\"");
            }
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
        sf::RenderStates states;
        for (auto & nextStagePtr : stagePVec_)
        {
            nextStagePtr->Draw( * winPtr_, states);
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
        //reset loop variables
        willExit_ = false;
        fatalExitEvent_ = false;
        auto soundManagerPtr{ sfml_util::SoundManager::Instance() };
        frameRateVec_.resize(4096);
        sf::Vector2i prevMousePos{ sf::Mouse::getPosition( * winPtr_) };

        //consume pre-events
        ConsumeEvents();

        clock_.restart();
        while (winPtr_->isOpen() && (false == willExit_))
        {
            auto const NEW_MOUSE_POS{ sf::Mouse::getPosition( * winPtr_) };
            auto const HAS_MOUSE_MOVED{ NEW_MOUSE_POS != prevMousePos };

            winPtr_->clear(sf::Color::Black);
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
            winPtr_->display();
            ProcessScreenshot();

            if (HAS_MOUSE_MOVED)
            {
                prevMousePos = NEW_MOUSE_POS;
            }
        }

        ConsumeEvents();

        //reset hold time
        holdTime_ = NO_HOLD_TIME_;
        holdTimeCounter_ = 0.0f;

        //reset fader
        willExitAfterFade_ = false;

        return fatalExitEvent_;
    }


    void Loop::FadeOut( const sf::Color & FADE_TO_COLOR,
                        const float       SPEED_MULT,
                        const bool        WILL_HOLD_FADE )
    {
        fader_.UpdateRegion(Display::Instance()->GetWinWidth(), Display::Instance()->GetWinHeight());
        fader_.FadeTo(FADE_TO_COLOR, SPEED_MULT);
        continueFading_ = true;
        willExitAfterFade_ = true;
        willHoldFade_ = WILL_HOLD_FADE;
    }


    void Loop::FadeIn(const sf::Color & FADE_FROM_COLOR,
                      const float       SPEED_MULT,
                      const bool        WILL_HOLD_FADE)
    {
        fader_.UpdateRegion(Display::Instance()->GetWinWidth(), Display::Instance()->GetWinHeight());
        fader_.Reset(FADE_FROM_COLOR);
        fader_.FadeTo(sf::Color::Transparent, SPEED_MULT);
        continueFading_ = true;
        willHoldFade_ = WILL_HOLD_FADE;
    }

}
}
