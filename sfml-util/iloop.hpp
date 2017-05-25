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
#ifndef SFMLUTIL_ILOOP_INCLUDED
#define SFMLUTIL_ILOOP_INCLUDED
//
// iloop.hpp
//  Interface class to the event/draw loop.
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/sfml-window.hpp"
#include "sfml-util/music-enum.hpp"

#include "game/i-popup-callback.hpp"
#include "game/loop-state-enum.hpp"

#include <memory>
#include <string>


namespace sfml_util
{

    //forward declarations
    class IStage;
    using IStageSPtr_t = std::shared_ptr<IStage>;


    //interface class for all Loops
    class ILoop
    {
    public:
        virtual ~ILoop() {}

        virtual bool Execute() = 0;
        virtual void Exit() = 0;
        virtual void AddStage(IStageSPtr_t &) = 0;
        virtual void RemoveAllStages() = 0;

        virtual void FadeOut(const sf::Color & FADE_TO_COLOR,
                             const float       SPEED_MULT     = 200.0f,
                             const bool        WILL_HOLD_FADE = false) = 0;

        virtual void FadeIn(const sf::Color & FADE_FROM_COLOR,
                            const float       SPEED_MULT      = 200.0f,
                            const bool        WILL_HOLD_FADE  = false) = 0;

        virtual void SetPopup(IStageSPtr_t & popupStageSPtr) = 0;
        virtual void SetHoldTime(const float SECONDS) = 0;
        virtual void SetWillHoldFade(const bool) = 0;
        virtual void SetWillExitAfterFade(const bool) = 0;

        virtual void SetMouseVisibility(const bool IS_VISIBLE) = 0;

        virtual void SetState(const game::LoopState::Enum) = 0;
        virtual game::LoopState::Enum GetState() const = 0;

        virtual void ConsumeEvents() = 0;

        virtual void SetWillExitOnKeypress(const bool) = 0;
        virtual void SetWillExitOnMouseclick(const bool) = 0;

        virtual void RemoveFocus() = 0;

        virtual bool GetIgnoreMouse() const = 0;
        virtual void SetIgnoreMouse(const bool WILL_IGNORE_MOUSE) = 0;

        virtual bool GetIgnoreKeystrokes() const = 0;
        virtual void SetIgnoreKeystrokes(const bool WILL_IGNORE_KEYSTROKES) = 0;

        virtual void AssignPopupCallbackHandlerInfo(game::callback::IPopupHandler_t * const HANDLER_PTR,
                                                    const game::PopupInfo &                 POPUP_INFO) = 0;

        virtual void FakeMouseClick(const sf::Vector2f & MOUSE_POS_V) = 0;

        virtual bool IsFading() const = 0;

        virtual void TestingStrAppend(const std::string &) = 0;
        virtual void TestingStrIncrement(const std::string &) = 0;
        virtual void TestingImageSet(const sfml_util::TextureSPtr_t &) = 0;

    protected:
        virtual void LogFrameRate() = 0;
        virtual void ProcessFader() = 0;
        virtual void ProcessPopup() = 0;
        virtual void ProcessEvents() = 0;
        virtual void ProcessKeyStrokes(const sf::Event & EVENT) = 0;
        virtual void ProcessMouseMove() = 0;
        virtual void ProcessMouseButtonLeftPressed(const sf::Vector2f &) = 0;
        virtual void ProcessMouseButtonLeftReleased(const sf::Vector2f &) = 0;
        virtual void ProcessMouseWheelRoll(const sf::Event & EVENT) = 0;
        virtual void ProcessPopupCallback() = 0;
        virtual void ProcessTimeUpdate() = 0;
        virtual void ProcessDrawing() = 0;
        virtual void PerformNextTest() = 0;
        virtual void ProcessScreenshot() = 0;
    };

    using ILoopSPtr_t = std::shared_ptr<ILoop>;

}
#endif //SFMLUTIL_ILOOP_INCLUDED
