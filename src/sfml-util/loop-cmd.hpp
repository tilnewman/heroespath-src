// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_LOOPCOMMAND_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_LOOPCOMMAND_HPP_INCLUDED
//
// loop-cmd.hpp
//  Code that encapsulates a command that performs some action on a Loop object.
//
#include "misc/not-null.hpp"
#include "sfml-util/loop-state-enum.hpp"
#include "sfml-util/loop.hpp"
#include "sfml-util/sfml-util.hpp"
#include "sfml-util/sound-manager.hpp"
#include "sfml-util/stage.hpp"

#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace heroespath
{
namespace sfml_util
{

    // interface class for all loop commands
    class ILoopCmd
    {
    public:
        virtual ~ILoopCmd() = default;

        // all loop commands have meaningful names for logging
        virtual const std::string GetName() const = 0;

        // perform some action on the loop
        // significance of the return value to be defined by the derived type.
        virtual bool Execute() = 0;
    };

    using ILoopCmdPtr_t = misc::NotNull<ILoopCmd *>;
    using ILoopCmdSPtr_t = std::shared_ptr<ILoopCmd>;
    using ILoopCmdSVec_t = std::vector<ILoopCmdSPtr_t>;

    // A type that performs some action on a Loop
    class LoopCmd : public ILoopCmd
    {
    public:
        LoopCmd(const LoopCmd &) = delete;
        LoopCmd(LoopCmd &&) = delete;
        LoopCmd & operator=(const LoopCmd &) = delete;
        LoopCmd & operator=(LoopCmd &&) = delete;

    public:
        explicit LoopCmd(const std::string & NAME);
        virtual ~LoopCmd();

        virtual const std::string GetName() const { return NAME_; }

        virtual bool Execute() = 0;

    protected:
        const std::string NAME_;
    };

    // executes the loop
    class LoopCmd_Execute : public LoopCmd
    {
    public:
        LoopCmd_Execute()
            : LoopCmd("Execute")
        {}

        virtual ~LoopCmd_Execute() = default;

        virtual bool Execute();
    };

    // holds a LoopManager state change
    class LoopCmd_StateChange : public LoopCmd
    {
    public:
        explicit LoopCmd_StateChange(const LoopState::Enum STATE_NUM)
            : LoopCmd("LoopStateChange")
            , STATE_(STATE_NUM)
        {}

        virtual ~LoopCmd_StateChange() = default;

        virtual bool Execute();

        virtual const std::string GetName() const
        {
            std::ostringstream ss;
            ss << NAME_ << " new_state=" << LoopState::ToString(STATE_);
            return ss.str();
        }

    private:
        const LoopState::Enum STATE_;
    };

    // Sets the visibility of the mouse the loop
    class LoopCmd_SetMouseVisibility : public LoopCmd
    {
    public:
        explicit LoopCmd_SetMouseVisibility(const bool IS_VISIBLE)
            : LoopCmd("SetMouseVisibility")
            , IS_VISIBLE_(IS_VISIBLE)
        {}

        virtual ~LoopCmd_SetMouseVisibility() = default;

        virtual bool Execute();

        virtual const std::string GetName() const
        {
            std::ostringstream ss;
            ss << NAME_ << " visible=" << std::boolalpha << IS_VISIBLE_;
            return ss.str();
        }

    private:
        const bool IS_VISIBLE_;
    };

    // Starts the theme music playing in the Loop
    class LoopCmd_StartMusic : public LoopCmd
    {
    public:
        explicit LoopCmd_StartMusic(
            const music::Enum MUSIC_TO_START,
            const float SPEED_MULT = MusicOperator::FADE_MULT_DEFAULT_IN_,
            const float TARGET_VOLUME = MusicOperator::VOLUME_USE_GLOBAL_)
            : LoopCmd("StartMusic")
            , MUSIC_TO_START_(MUSIC_TO_START)
            , TARGET_VOLUME_(TARGET_VOLUME)
            , SPEED_MULT_(SPEED_MULT)
        {}

        virtual ~LoopCmd_StartMusic() = default;

        virtual bool Execute()
        {
            sfml_util::SoundManager::Instance()->MusicStart(
                MUSIC_TO_START_, SPEED_MULT_, TARGET_VOLUME_);

            return true;
        }

        virtual const std::string GetName() const
        {
            std::ostringstream ss;

            ss << NAME_ << " \"" << music::ToString(MUSIC_TO_START_)
               << "\", target_vol=" << TARGET_VOLUME_ << ", speed_mult=" << SPEED_MULT_;

            return ss.str();
        }

    private:
        const music::Enum MUSIC_TO_START_;
        const float TARGET_VOLUME_;
        const float SPEED_MULT_;
    };

    // Instructs the Loop to start fading out the theme music
    class LoopCmd_StopMusic : public LoopCmd
    {
    public:
        explicit LoopCmd_StopMusic(
            const music::Enum MUSIC_TO_STOP,
            const float SPEED_MULT = MusicOperator::FADE_MULT_DEFAULT_OUT_)
            : LoopCmd("StopMusic")
            , MUSIC_TO_STOP_(MUSIC_TO_STOP)
            , SPEED_MULT_(SPEED_MULT)
        {}

        virtual ~LoopCmd_StopMusic() = default;

        virtual bool Execute()
        {
            sfml_util::SoundManager::Instance()->MusicStop(MUSIC_TO_STOP_, SPEED_MULT_);
            return true;
        }

        virtual const std::string GetName() const
        {
            std::ostringstream ss;

            ss << NAME_ << "  \"" << music::ToString(MUSIC_TO_STOP_)
               << "\", speed_mult=" << SPEED_MULT_;

            return ss.str();
        }

    private:
        const music::Enum MUSIC_TO_STOP_;
        const float SPEED_MULT_;
    };

    // adds the default stage to a Loop
    class LoopCmd_AddStage_Default : public LoopCmd
    {
    public:
        LoopCmd_AddStage_Default()
            : LoopCmd("AddStageDefault")
        {}

        virtual ~LoopCmd_AddStage_Default() = default;

        virtual bool Execute();
    };

    // adds a hold time to the loop
    class LoopCmd_SetHoldTime : public LoopCmd
    {
    public:
        explicit LoopCmd_SetHoldTime(const float SECONDS)
            : LoopCmd("SetHoldTime")
            , TIME_(SECONDS)
        {}

        virtual ~LoopCmd_SetHoldTime() = default;

        virtual bool Execute();

        virtual const std::string GetName() const
        {
            std::ostringstream ss;
            ss << NAME_ << " seconds=" << TIME_;
            return ss.str();
        }

    private:
        const float TIME_;
    };

    // adds a hold time to the loop
    class LoopCmd_RemoveAllStages : public LoopCmd
    {
    public:
        LoopCmd_RemoveAllStages()
            : LoopCmd("RemoveAllStages")
        {}

        virtual ~LoopCmd_RemoveAllStages() = default;

        virtual bool Execute();
    };

    // sets the loop to terminate after the current or next fade
    class LoopCmd_ExitAfterFade : public LoopCmd
    {
    public:
        LoopCmd_ExitAfterFade()
            : LoopCmd("ExitAfterFade")
        {}

        virtual ~LoopCmd_ExitAfterFade() = default;

        virtual bool Execute();
    };

    // fades a loop object in
    class LoopCmd_FadeIn : public LoopCmd
    {
    public:
        explicit LoopCmd_FadeIn(
            const sf::Color & FADE_FROM_COLOR = sf::Color::Black,
            const float SPEED_MULT = 200.0f,
            const bool WILL_HOLD_FADE = false)
            : LoopCmd(std::string("FadeIn"))
            , SPEED_MULT_(SPEED_MULT)
            , FADE_FROM_COLOR_(FADE_FROM_COLOR)
            , WILL_HOLD_FADE_(WILL_HOLD_FADE)
        {}

        virtual ~LoopCmd_FadeIn() = default;

        virtual bool Execute();

        virtual const std::string GetName() const
        {
            std::ostringstream ss;
            ss << NAME_ << " fade_from_color=" << sfml_util::color::ColorToString(FADE_FROM_COLOR_)
               << ", speed_mult=" << SPEED_MULT_ << ", will_hold_after_fade=" << std::boolalpha
               << WILL_HOLD_FADE_;
            return ss.str();
        }

    private:
        const float SPEED_MULT_;
        const sf::Color FADE_FROM_COLOR_;
        bool WILL_HOLD_FADE_;
    };

    // fades-out a loop object in
    class LoopCmd_FadeOut : public LoopCmd
    {
    public:
        explicit LoopCmd_FadeOut(
            const sf::Color & FADE_TO_COLOR = sf::Color::Black,
            const float SPEED_MULT = 300.0f,
            const bool WILL_HOLD_FADE = false)
            : LoopCmd(std::string("FadeOut"))
            , SPEED_MULT_(SPEED_MULT)
            , FADE_TO_COLOR_(FADE_TO_COLOR)
            , WILL_HOLD_FADE_(WILL_HOLD_FADE)
        {}

        virtual ~LoopCmd_FadeOut() = default;

        virtual bool Execute();

        virtual const std::string GetName() const
        {
            std::ostringstream ss;
            ss << NAME_ << " fade_to_color=" << sfml_util::color::ColorToString(FADE_TO_COLOR_)
               << ", speed_mult=" << SPEED_MULT_ << ", will_hold_after_fade=" << std::boolalpha
               << WILL_HOLD_FADE_;
            return ss.str();
        }

    private:
        const float SPEED_MULT_;
        const sf::Color FADE_TO_COLOR_;
        const bool WILL_HOLD_FADE_;
    };

    // sets the loop to terminate after keypresses
    class LoopCmd_ExitAfterKeypress : public LoopCmd
    {
    public:
        explicit LoopCmd_ExitAfterKeypress(const bool WILL_EXIT_ON_KEYSTROKE)
            : LoopCmd("ExitOnKeypress")
            , WILL_EXIT_ON_KEYSTROKE_(WILL_EXIT_ON_KEYSTROKE)
        {}

        virtual ~LoopCmd_ExitAfterKeypress() = default;

        virtual bool Execute();

        virtual const std::string GetName() const
        {
            std::ostringstream ss;
            ss << NAME_ << " willExitOnKeypress=" << std::boolalpha << WILL_EXIT_ON_KEYSTROKE_;
            return ss.str();
        }

    private:
        const bool WILL_EXIT_ON_KEYSTROKE_;
    };

    // sets the loop to terminate after mouse clicks
    class LoopCmd_ExitAfterMouseclick : public LoopCmd
    {
    public:
        explicit LoopCmd_ExitAfterMouseclick(const bool WILL_EXIT_ON_MOUSECLICK)
            : LoopCmd("ExitOnMouseclick")
            , WILL_EXIT_ON_MOUSECLICK_(WILL_EXIT_ON_MOUSECLICK)
        {}

        virtual ~LoopCmd_ExitAfterMouseclick() = default;

        virtual bool Execute();

        virtual const std::string GetName() const
        {
            std::ostringstream ss;
            ss << NAME_ << " willExitOnMouseclicks=" << std::boolalpha << WILL_EXIT_ON_MOUSECLICK_;
            return ss.str();
        }

    private:
        const bool WILL_EXIT_ON_MOUSECLICK_;
    };

    // removes focus from all GuiEntitys
    class LoopCmd_RemoveFocus : public LoopCmd
    {
    public:
        LoopCmd_RemoveFocus()
            : LoopCmd("RemoveFocus")
        {}

        virtual ~LoopCmd_RemoveFocus() = default;

        virtual bool Execute();
    };

    // ignores mouse movement
    class LoopCmd_IgnoreMouse : public LoopCmd
    {
    public:
        explicit LoopCmd_IgnoreMouse(const bool WILL_IGNORE_MOUSE)
            : LoopCmd("IgnoreMouse")
            , WILL_IGNORE_MOUSE_(WILL_IGNORE_MOUSE)
        {}

        virtual ~LoopCmd_IgnoreMouse() = default;

        virtual bool Execute();

    private:
        const bool WILL_IGNORE_MOUSE_;
    };

    // ignores keystrokes
    class LoopCmd_IgnoreKeystrokes : public LoopCmd
    {
    public:
        explicit LoopCmd_IgnoreKeystrokes(const bool WILL_IGNORE_KEYSTROKES)
            : LoopCmd("IgnoreKeystrokes")
            , WILL_IGNORE_KEYSTROKES_(WILL_IGNORE_KEYSTROKES)
        {}

        virtual ~LoopCmd_IgnoreKeystrokes() = default;

        virtual bool Execute();

    private:
        const bool WILL_IGNORE_KEYSTROKES_;
    };

    // fakes a mouse click at the given location
    class LoopCmd_FakeMouseClick : public LoopCmd
    {
    public:
        explicit LoopCmd_FakeMouseClick(const sf::Vector2f & MOUSE_POS_V)
            : LoopCmd("FakeMouseClick")
            , MOUSE_CLICK_POS_(MOUSE_POS_V)
        {}

        virtual ~LoopCmd_FakeMouseClick() = default;

        virtual bool Execute();

        virtual const std::string GetName() const
        {
            std::ostringstream ss;
            ss << NAME_ << " at " << sfml_util::VectorToString(MOUSE_CLICK_POS_);
            return ss.str();
        }

    private:
        const sf::Vector2f MOUSE_CLICK_POS_;
    };

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_LOOPCOMMAND_HPP_INCLUDED
