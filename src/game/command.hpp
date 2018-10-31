// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GAME_COMMAND_HPP_INCLUDED
#define HEROESPATH_GAME_COMMAND_HPP_INCLUDED
//
// game-command.hpp
//
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"
#include "popup/popup-info.hpp"
#include "sfml-util/music-enum.hpp"
#include "stage/stage-enum.hpp"
#include "stage/stage-setup-packet.hpp"

#include <memory>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

namespace heroespath
{
namespace game
{

    enum class FadeDirection
    {
        In,
        Out
    };

    // wraps all the information needed to setup a fade in or out
    struct FadeCommand
    {
        FadeCommand(
            const FadeDirection DIRECTION,
            const float SPEED,
            const sf::Color & COLOR = sf::Color::Black)
            : direction(DIRECTION)
            , speed(SPEED)
            , color(COLOR)
        {}

        FadeCommand(const FadeCommand &) = default;
        FadeCommand(FadeCommand &&) = default;
        FadeCommand & operator=(const FadeCommand &) = default;
        FadeCommand & operator=(FadeCommand &&) = default;

        const std::string ToString() const
        {
            std::ostringstream ss;

            ss << "cmd=fade, " << ((FadeDirection::In == direction) ? "in" : "out")
               << ", fade_speed=" << speed << ", fade_color=" << color;

            return ss.str();
        }

        FadeDirection direction;
        float speed;
        sf::Color color;
    };

    using FadeCommandOpt_t = boost::optional<FadeCommand>;

    inline bool operator==(const FadeCommand & L, const FadeCommand & R)
    {
        return (
            (L.direction == R.direction) && (L.color == R.color)
            && misc::IsRealClose(L.speed, R.speed));
    }

    inline bool operator!=(const FadeCommand & L, const FadeCommand & R) { return !(L == R); }

    // wraps all the information needed by Loop::Execute() that does not change
    struct ExecuteCommand
    {
        ExecuteCommand() = default;
        ExecuteCommand(const ExecuteCommand &) = default;
        ExecuteCommand(ExecuteCommand &&) = default;
        ExecuteCommand & operator=(const ExecuteCommand &) = default;
        ExecuteCommand & operator=(ExecuteCommand &&) = default;

        bool DoFlagsMatch(const ExecuteCommand & OTHER_CMD) const
        {
            return (
                std::tie(
                    will_hide_mouse,
                    will_keystroke_exit,
                    will_keystroke_ignore,
                    will_mouse_ignore,
                    will_mouse_click_exit)
                == std::tie(
                    OTHER_CMD.will_hide_mouse,
                    OTHER_CMD.will_keystroke_exit,
                    OTHER_CMD.will_keystroke_ignore,
                    OTHER_CMD.will_mouse_ignore,
                    OTHER_CMD.will_mouse_click_exit));
        }

        static const ExecuteCommand MakeDefaultWithMouseHidden()
        {
            ExecuteCommand executeCommand;
            executeCommand.will_hide_mouse = true;
            return executeCommand;
        }

        static const ExecuteCommand MakeForFade(const FadeCommand & FADE_COMMAND)
        {
            ExecuteCommand executeCommand;
            executeCommand.will_hide_mouse = true;
            executeCommand.will_mouse_ignore = true;
            executeCommand.will_keystroke_ignore = true;
            executeCommand.fade_opt = FADE_COMMAND;
            return executeCommand;
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

            ss << "cmd=execute";

            if (fade_opt)
            {
                prefixSeparatorString();
                ss << fade_opt->ToString();
            }

            if (DoFlagsMatch(MakeForFade(FadeCommand(FadeDirection::In, 0.0f))))
            {
                prefixSeparatorString();
                ss << "flags=fade_default_of_ignore_all";
            }
            else
            {
                if (will_hide_mouse)
                {
                    prefixSeparatorString();
                    ss << "hide_mouse";
                }

                if (will_keystroke_exit)
                {
                    prefixSeparatorString();
                    ss << "keystrokes_exit";
                }

                if (will_keystroke_ignore)
                {
                    prefixSeparatorString();
                    ss << "keystrokes_ignored";
                }

                if (will_mouse_ignore)
                {
                    prefixSeparatorString();
                    ss << "mouse_ignored";
                }

                if (will_mouse_click_exit)
                {
                    prefixSeparatorString();
                    ss << "mouse_clicks_exit";
                }
            }

            return ss.str();
        }

        bool will_hide_mouse = false;
        bool will_keystroke_exit = false;
        bool will_keystroke_ignore = false;
        bool will_mouse_ignore = false;
        bool will_mouse_click_exit = false;
        FadeCommandOpt_t fade_opt = boost::none;
    };

    using ExecuteCommandOpt_t = boost::optional<ExecuteCommand>;

    inline bool operator==(const ExecuteCommand & L, const ExecuteCommand & R)
    {
        return (
            std::tie(
                L.will_hide_mouse,
                L.will_keystroke_exit,
                L.will_keystroke_ignore,
                L.will_mouse_ignore,
                L.will_mouse_click_exit,
                L.fade_opt)
            == std::tie(
                R.will_hide_mouse,
                R.will_keystroke_exit,
                R.will_keystroke_ignore,
                R.will_mouse_ignore,
                R.will_mouse_click_exit,
                R.fade_opt));
    }

    inline bool operator!=(const ExecuteCommand & L, const ExecuteCommand & R) { return !(L == R); }

    struct MusicCommand
    {
        MusicCommand(
            const sfml_util::music::Enum TO_STOP = sfml_util::music::None,
            const sfml_util::music::Enum TO_START = sfml_util::music::None,
            const float VOLUME_MIN = 0.0f)
            : to_stop(TO_STOP)
            , to_start(TO_START)
            , volume_min(VOLUME_MIN)
        {}

        MusicCommand(const MusicCommand &) = default;
        MusicCommand(MusicCommand &&) = default;
        MusicCommand & operator=(const MusicCommand &) = default;
        MusicCommand & operator=(MusicCommand &&) = default;

        static const MusicCommand MakeToStopAllMusic()
        {
            return MusicCommand(sfml_util::music::All, sfml_util::music::None);
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

            auto musicToString = [](const sfml_util::music::Enum MUSIC) -> std::string {
                if (MUSIC == sfml_util::music::Count)
                {
                    return "Count";
                }
                else if (MUSIC == sfml_util::music::All)
                {
                    return "All";
                }
                else if (MUSIC == sfml_util::music::None)
                {
                    return "None";
                }
                else
                {
                    return sfml_util::music::ToStringNoThrow(MUSIC);
                }
            };

            ss << "cmd=music";

            if ((to_stop <= sfml_util::music::All) && (to_stop != sfml_util::music::Count))
            {
                prefixSeparatorString();
                ss << "music_stop=" << musicToString(to_stop);
            }

            if (to_start < sfml_util::music::Count)
            {
                prefixSeparatorString();
                ss << "music_start=" << musicToString(to_start);
            }

            if (volume_min > 0.0f)
            {
                prefixSeparatorString();
                ss << "music_vol_min=" << volume_min;
            }

            return ss.str();
        }

        sfml_util::music::Enum to_stop;
        sfml_util::music::Enum to_start;
        float volume_min;
    };

    using MusicCommandOpt_t = boost::optional<MusicCommand>;

    inline bool operator==(const MusicCommand & L, const MusicCommand & R)
    {
        return (
            (L.to_stop == R.to_stop) && (L.to_start == R.to_start)
            && misc::IsRealClose(L.volume_min, R.volume_min));
    }

    inline bool operator!=(const MusicCommand & L, const MusicCommand & R) { return !(L == R); }

    struct PopupReplaceCommand
    {
        PopupReplaceCommand(
            const popup::PopupInfo & POPUP_INFO,
            const sfml_util::PopupCallback_t::IHandlerPtr_t POPUP_HANDLER_PTR)
            : popup_info(POPUP_INFO)
            , handler_ptr(POPUP_HANDLER_PTR)
        {}

        PopupReplaceCommand(const PopupReplaceCommand &) = default;
        PopupReplaceCommand(PopupReplaceCommand &&) = default;
        PopupReplaceCommand & operator=(const PopupReplaceCommand &) = default;
        PopupReplaceCommand & operator=(PopupReplaceCommand &&) = default;

        const std::string ToString() const
        {
            return "cmd=popup_spawn=" + popup_info.ToStringShort();
        }

        popup::PopupInfo popup_info;
        sfml_util::PopupCallback_t::IHandlerPtr_t handler_ptr;
    };

    using PopupReplaceCommandOpt_t = boost::optional<PopupReplaceCommand>;

    inline bool operator==(const PopupReplaceCommand & L, const PopupReplaceCommand & R)
    {
        return (
            L.popup_info.IsAsCloseAsIsReasonableToCheck(R.popup_info)
            && (L.handler_ptr == R.handler_ptr));
    }

    inline bool operator!=(const PopupReplaceCommand & L, const PopupReplaceCommand & R)
    {
        return !(L == R);
    }

    using StageReplaceCommand = stage::SetupPacket;
    using StageReplaceCommandOpt_t = boost::optional<StageReplaceCommand>;

    struct Command
    {
        Command() = default;
        Command(const Command &) = default;
        Command(Command &&) = default;
        Command & operator=(const Command &) = default;
        Command & operator=(Command &&) = default;

        const std::string ToString() const
        {
            std::ostringstream ss;

            if (music_opt)
            {
                ss << music_opt->ToString();
            }

            if (stage_opt)
            {
                ss << stage_opt->ToString();
            }

            if (popup_replace_opt)
            {
                ss << popup_replace_opt->ToString();
            }

            if (will_remove_popup)
            {
                ss << "cmd=popup_remove";
            }

            if (execute_opt)
            {
                ss << execute_opt->ToString();
            }

            if (ss.str().empty())
            {
                return "GameCommand(empty=ERROR=\"game commands should never be empty\")";
            }
            else
            {
                return "GameCommand(" + ss.str() + ")";
            }
        }

        MusicCommandOpt_t music_opt = boost::none;
        StageReplaceCommandOpt_t stage_opt = boost::none;
        PopupReplaceCommandOpt_t popup_replace_opt = boost::none;
        bool will_remove_popup = false;
        ExecuteCommandOpt_t execute_opt = boost::none;
    };

    using CommandOpt_t = boost::optional<Command>;
    using CommandVec_t = std::vector<Command>;

    inline bool operator==(const Command & L, const Command & R)
    {
        return (
            std::tie(
                L.music_opt, L.stage_opt, L.popup_replace_opt, L.will_remove_popup, L.execute_opt)
            == std::tie(
                R.music_opt, R.stage_opt, R.popup_replace_opt, R.will_remove_popup, R.execute_opt));
    }

    inline bool operator!=(const Command & L, const Command & R) { return !(L == R); }

    inline std::ostream & operator<<(std::ostream & os, const game::FadeCommand & COMMAND)
    {
        os << COMMAND.ToString();
        return os;
    }

    inline std::ostream & operator<<(std::ostream & os, const game::ExecuteCommand & COMMAND)
    {
        os << COMMAND.ToString();
        return os;
    }

    inline std::ostream & operator<<(std::ostream & os, const game::MusicCommand & COMMAND)
    {
        os << COMMAND.ToString();
        return os;
    }

    inline std::ostream & operator<<(std::ostream & os, const game::PopupReplaceCommand & COMMAND)
    {
        os << COMMAND.ToString();
        return os;
    }

    inline std::ostream & operator<<(std::ostream & os, const game::StageReplaceCommand & COMMAND)
    {
        os << COMMAND.ToString();
        return os;
    }

    inline std::ostream & operator<<(std::ostream & os, const game::Command & COMMAND)
    {
        os << COMMAND.ToString();
        return os;
    }

} // namespace game
} // namespace heroespath

#endif // HEROESPATH_GAME_COMMAND_HPP_INCLUDED
