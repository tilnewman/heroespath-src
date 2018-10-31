// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GAME_STATUS_HPP_INCLUDED
#define HEROESPATH_GAME_STATUS_HPP_INCLUDED
//
// status.hpp
//
#include "misc/log-macros.hpp"

#include <sstream>

namespace heroespath
{
namespace game
{

    struct IStatusForLoop
    {
        virtual ~IStatusForLoop() = default;

        virtual bool IsLoopStopRequested() const = 0;
        virtual void SetLoopRunning(const bool IS_LOOP_RUNNING) = 0;
        virtual void LoopStopRequest() = 0;
        virtual bool IsFading() const = 0;
        virtual void StopFading() = 0;
        virtual bool IsGameExitRequested() const = 0;
        virtual void GameExitRequest() = 0;
    };

    class Status : public IStatusForLoop
    {
    public:
        virtual ~Status() = default;

        bool IsLoopRunning() const { return is_loop_running; }

        void SetLoopRunning(const bool IS_LOOP_RUNNING) final
        {
            is_loop_running = IS_LOOP_RUNNING;
            M_HP_LOG_DBG("game loop " << ((IS_LOOP_RUNNING) ? "starting" : "stopped"));
        }

        bool IsLoopStopRequested() const final { return is_loop_stop_requested; }

        void LoopStopRequest() final
        {
            is_loop_stop_requested = true;
            M_HP_LOG_DBG("game loop stop requested");
        }

        void LoopStopRequestReset()
        {
            is_loop_stop_requested = false;
            M_HP_LOG_DBG("game loop stop request reset");
        }

        bool IsFading() const final { return is_fading; }

        void StartFading(const bool IS_FADING_IN)
        {
            is_fading = true;
            M_HP_LOG_DBG("fade " << ((IS_FADING_IN) ? "in" : "out") << " started");
        }

        void StopFading() final
        {
            is_fading = false;
            M_HP_LOG_DBG("fade finished");
        }

        bool IsGameExitRequested() const final { return will_exit_game; }

        void GameExitRequest() final
        {
            will_exit_game = true;
            M_HP_LOG_DBG("game exit requested");
        }

        void GameExitRequestReset()
        {
            will_exit_game = false;
            M_HP_LOG_DBG("game exit request reset");
        }

        const std::string ToString() const
        {
            std::ostringstream ss;

            auto prefixSeparatorString = [&]() {
                if (ss.str().empty() == false)
                {
                    ss << ", ";
                }
            };

            if (is_loop_running)
            {
                prefixSeparatorString();
                ss << "loop_is_running";
            }

            if (is_loop_stop_requested)
            {
                prefixSeparatorString();
                ss << "loop_exit_requested";
            }

            if (is_fading)
            {
                prefixSeparatorString();
                ss << "fading";
            }

            if (will_exit_game)
            {
                prefixSeparatorString();
                ss << "game_exit_requested";
            }

            if (ss.str().empty())
            {
                return "Status(none)";
            }
            else
            {
                return "Status(" + ss.str() + ")";
            }
        }

    private:
        bool is_loop_running = false;
        bool is_loop_stop_requested = false;
        bool is_fading = false;
        bool will_exit_game = false;
    };

} // namespace game
} // namespace heroespath

#endif // HEROESPATH_GAME_STATUS_HPP_INCLUDEDs
