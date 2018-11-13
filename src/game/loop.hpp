// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GAME_LOOP_INCLUDED
#define HEROESPATH_GAME_LOOP_INCLUDED
//
// loop.hpp
//
#include "game/active-stages.hpp"
#include "game/command.hpp"
#include "game/status.hpp"
#include "gui/i-entity.hpp"
#include "misc/boost-optional-that-throws.hpp"
#include "misc/callback.hpp"
#include "misc/not-null.hpp"

#include <SFML/System/Clock.hpp>

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace game
{

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
        // intentionally taking stages by reference because this class and GameController will need
        // to be able to make changes on the one true instance, while intentionally taking FLAGS by
        // const-copy so this function has its own copy to work with and the caller is free to make
        // changes to their own copy
        Loop(ActiveStages & stages, IStatusForLoop & iStatus, const ExecuteCommand FLAGS);
        ~Loop();

        Loop(const Loop &) = delete;
        Loop(Loop &&) = delete;
        Loop & operator=(const Loop &) = delete;
        Loop & operator=(Loop &&) = delete;

        void Execute();

    private:
        void ExecuteCleanup();
        const MouseThisFrame UpdateMouseInfo();
        void ExecuteNextTest();

        void UpdateTimeStages(const float FRAME_TIME_SEC);
        void UpdateTimeFade(const float FRAME_TIME_SEC);
        void UpdateTimeAudio(const float FRAME_TIME_SEC);
        void HandleMouseMove();

        void StopMouseHover(const bool HAS_MOUSE_MOVED);
        void StopMouseHoverImpl();

        void OncePerSecondTasks(sf::Clock & secondClock);
        void OncePerSecondTaskStartMouseHover();
        void OncePerSecondTaskCheckIfDisplayOpen();

        void HandleEvents();
        void HandleEvent(const sf::Event &);
        void HandleEventKeyStroke(const sf::Event &);
        void HandleEventMouseButtonLeftPressed();
        void HandleEventMouseButtonLeftReleased();
        bool HandleEventIfWindowClosed(const sf::Event &);

        void ConsumeAndIgnoreStrayEvents();

        void Draw();

    private:
        ActiveStages & stages_;
        IStatusForLoop & iStatus_;
        const ExecuteCommand flags_;
        sf::Event::EventType prevKeyStrokeEventType_;
        sf::Keyboard::Key prevKeyStrokeEventKey_;
        bool isMouseHovering_;
        sf::Vector2i prevMousePosV_;
        MouseThisFrame frameMouseInfo_;
        std::vector<sf::Event> toLogEvents_;
    };

} // namespace game
} // namespace heroespath

#endif // HEROESPATH_GAME_LOOP_INCLUDED
