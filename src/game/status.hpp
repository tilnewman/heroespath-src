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
#include "misc/strings.hpp"
#include "sfutil/color.hpp"

#include <sstream>

namespace heroespath
{
namespace game
{

    using BoolOpt_t = boost::optional<bool>;

    enum class FadeDirection
    {
        In,
        Out
    };

    struct FadeStatus
    {
        const std::string ToString() const
        {
            std::ostringstream ss;

            ss << "FadeStatus(is_fading" << std::boolalpha << is_fading
               << ", direction=" << ((FadeDirection::In == direction) ? "in" : "out")
               << ", current_color=" << current_color << ", target_color=" << target_color
               << ", will_draw_under_popup=" << will_draw_under_popup << ")";

            return ss.str();
        }

        bool is_fading = false;
        sf::Color target_color = sf::Color::Transparent;
        sf::Color current_color = sf::Color::Transparent;
        FadeDirection direction = FadeDirection::Out;
        bool will_draw_under_popup = false;
    };

    struct IStatusForLoop
    {
        virtual ~IStatusForLoop() = default;

        virtual bool IsLoopStopRequested() const = 0;
        virtual void SetLoopRunning(const bool IS_LOOP_RUNNING) = 0;
        virtual void LoopStopRequest() = 0;
        virtual const FadeStatus GetFadeStatus() const = 0;
        virtual void SetFadeTargetColorReached() = 0;
        virtual void SetFadeCurrentColor(const sf::Color &) = 0;
        virtual bool IsGameExitRequested() const = 0;
        virtual void GameExitRequest() = 0;
    };

    class Status : public IStatusForLoop
    {
    public:
        virtual ~Status() = default;

        bool IsLoopRunning() const { return is_loop_running; }
        void SetLoopRunning(const bool IS_LOOP_RUNNING) final { is_loop_running = IS_LOOP_RUNNING; }

        bool IsLoopStopRequested() const final { return is_loop_stop_requested; }
        void LoopStopRequest() final { is_loop_stop_requested = true; }
        void LoopStopRequestReset() { is_loop_stop_requested = false; }

        const FadeStatus GetFadeStatus() const final { return fade_status; }

        void SetFadeCurrentColor(const sf::Color & COLOR) final
        {
            fade_status.current_color = COLOR;
        }

        void SetFadeTargetColorReached() final { fade_status.is_fading = false; }

        void StartFadeOut(const sf::Color & COLOR, const BoolOpt_t & SET_WILL_DRAW_UNDER_POPUP_OPT)
        {
            fade_status.is_fading = true;
            fade_status.target_color = COLOR;
            fade_status.direction = FadeDirection::Out;

            if (SET_WILL_DRAW_UNDER_POPUP_OPT)
            {
                fade_status.will_draw_under_popup = SET_WILL_DRAW_UNDER_POPUP_OPT.value();
            }
        }

        void StartFadeIn(const BoolOpt_t & SET_WILL_DRAW_UNDER_POPUP_OPT)
        {
            fade_status.is_fading = true;
            fade_status.target_color = sf::Color::Transparent;
            fade_status.direction = FadeDirection::In;

            if (SET_WILL_DRAW_UNDER_POPUP_OPT)
            {
                fade_status.will_draw_under_popup = SET_WILL_DRAW_UNDER_POPUP_OPT.value();
            }
        }

        bool IsGameExitRequested() const final { return is_game_exit_requested; }

        void GameExitRequest() final { is_game_exit_requested = true; }

        void GameExitRequestReset() { is_game_exit_requested = false; }

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

            // if (is_loop_stop_requested)
            //{
            //    prefixSeparatorString();
            //    ss << "loop_exit_requested";
            //}

            if (is_game_exit_requested)
            {
                prefixSeparatorString();
                ss << "game_exit_requested";
            }

            if (fade_status.is_fading)
            {
                prefixSeparatorString();
                ss << fade_status.ToString();
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

        bool IsGameExiting() const { return is_game_exiting; }
        void SetGameIsExiting() { is_game_exiting = true; }

    private:
        bool is_loop_running = false;
        bool is_loop_stop_requested = false;
        bool is_game_exit_requested = false;
        bool is_game_exiting = false;
        FadeStatus fade_status = FadeStatus();
    };

} // namespace game
} // namespace heroespath

#endif // HEROESPATH_GAME_STATUS_HPP_INCLUDEDs
