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
//
#include "misc/not-null.hpp"
#include "sfml-util/loop-state-enum.hpp"
#include "sfml-util/loop.hpp"
#include "sfml-util/sound-manager.hpp"

#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace heroespath
{
namespace sfml_util
{

    // Responsible for implementing a common base class for all LoopCmds.
    class LoopCmd
    {
    public:
        explicit LoopCmd(const std::string & NAME)
            : name_(std::string("LoopCommand-").append(NAME))
        {}

        virtual ~LoopCmd() = default;
        virtual const std::string Name() const { return name_; }
        virtual void Execute() = 0;

    private:
        std::string name_;
    };

    using LoopCmdUPtr_t = std::unique_ptr<LoopCmd>;
    using LoopCmdUVec_t = std::vector<LoopCmdUPtr_t>;

    // Responsible for starting execution of the game loop
    class LoopCmd_Execute : public LoopCmd
    {
    public:
        LoopCmd_Execute()
            : LoopCmd("Execute")
        {}

        virtual ~LoopCmd_Execute() = default;
        void Execute() override;
    };

    // Responsible for causing a LoopManager state change
    class LoopCmd_StateChange : public LoopCmd
    {
    public:
        explicit LoopCmd_StateChange(const LoopState::Enum STATE_NUM)
            : LoopCmd("LoopStateChange")
            , newState_(STATE_NUM)
        {}

        virtual ~LoopCmd_StateChange() = default;

        void Execute() override;

        const std::string Name() const override
        {
            std::ostringstream ss;
            ss << LoopCmd::Name() << "To=" << LoopState::ToString(newState_);
            return ss.str();
        }

    private:
        LoopState::Enum newState_;
    };

    // Responsible for setting the mouse visibility
    class LoopCmd_SetMouseVisibility : public LoopCmd
    {
    public:
        explicit LoopCmd_SetMouseVisibility(const bool IS_VISIBLE)
            : LoopCmd("SetMouseVisibility")
            , isVisible_(IS_VISIBLE)
        {}

        virtual ~LoopCmd_SetMouseVisibility() = default;

        void Execute() override;

        const std::string Name() const override
        {
            std::ostringstream ss;
            ss << LoopCmd::Name() << "To=" << std::boolalpha << isVisible_;
            return ss.str();
        }

    private:
        bool isVisible_;
    };

    // Responsible for starting music
    class LoopCmd_StartMusic : public LoopCmd
    {
    public:
        explicit LoopCmd_StartMusic(
            const music::Enum MUSIC_TO_START,
            const float SPEED = MusicOperator::FADE_MULT_DEFAULT_IN_,
            const float TARGET_VOLUME = MusicOperator::VOLUME_USE_GLOBAL_)
            : LoopCmd("StartMusic")
            , musicToStart_(MUSIC_TO_START)
            , targetVolume_(TARGET_VOLUME)
            , speed_(SPEED)
        {}

        virtual ~LoopCmd_StartMusic() = default;

        void Execute() override
        {
            sfml_util::SoundManager::Instance()->MusicStart(musicToStart_, speed_, targetVolume_);
        }

        const std::string Name() const override
        {
            std::ostringstream ss;

            ss << LoopCmd::Name() << "_" << music::ToString(musicToStart_)
               << "_AtVol=" << targetVolume_ << "_WithSpeedMult=" << speed_;

            return ss.str();
        }

    private:
        music::Enum musicToStart_;
        float targetVolume_;
        float speed_;
    };

    // Responsible for fading out music
    class LoopCmd_StopMusic : public LoopCmd
    {
    public:
        explicit LoopCmd_StopMusic(
            const music::Enum MUSIC_TO_STOP,
            const float SPEED = MusicOperator::FADE_MULT_DEFAULT_OUT_)
            : LoopCmd("MusicFadeOut")
            , musicToStop_(MUSIC_TO_STOP)
            , speed_(SPEED)
        {}

        virtual ~LoopCmd_StopMusic() = default;

        void Execute() override
        {
            sfml_util::SoundManager::Instance()->MusicStop(musicToStop_, speed_);
        }

        const std::string Name() const override
        {
            std::ostringstream ss;

            ss << LoopCmd::Name() << "_" << music::ToString(musicToStop_)
               << "_WithSpeedMult=" << speed_;

            return ss.str();
        }

    private:
        music::Enum musicToStop_;
        float speed_;
    };

    // Responsible for setting the hold time
    class LoopCmd_SetHoldTime : public LoopCmd
    {
    public:
        explicit LoopCmd_SetHoldTime(const float SECONDS)
            : LoopCmd("SetHoldTime")
            , holdTimeSec_(SECONDS)
        {}

        virtual ~LoopCmd_SetHoldTime() = default;

        void Execute() override;

        const std::string Name() const override
        {
            std::ostringstream ss;
            ss << LoopCmd::Name() << "To=" << holdTimeSec_;
            return ss.str();
        }

    private:
        float holdTimeSec_;
    };

    // Responsible for removing all stages
    class LoopCmd_RemoveAllStages : public LoopCmd
    {
    public:
        LoopCmd_RemoveAllStages()
            : LoopCmd("RemoveAllStages")
        {}

        virtual ~LoopCmd_RemoveAllStages() = default;
        void Execute() override;
    };

    // Responsible starting a fade-in
    class LoopCmd_FadeIn : public LoopCmd
    {
    public:
        explicit LoopCmd_FadeIn(
            const float SPEED,
            const bool WILL_EXIT_AFTER,
            const sf::Color & FADE_FROM_COLOR = sf::Color::Black)
            : LoopCmd(std::string("FadeIn"))
            , willExitAfter_(WILL_EXIT_AFTER)
            , speed_(SPEED)
            , fadeFromColor_(FADE_FROM_COLOR)
        {}

        virtual ~LoopCmd_FadeIn() = default;
        void Execute() override;
        const std::string Name() const override;

    private:
        bool willExitAfter_;
        float speed_;
        sf::Color fadeFromColor_;
    };

    // Responsible for starting a fade-out
    class LoopCmd_FadeOut : public LoopCmd
    {
    public:
        explicit LoopCmd_FadeOut(
            const float SPEED, const sf::Color & FADE_TO_COLOR = sf::Color::Black)
            : LoopCmd(std::string("FadeOut"))
            , speed_(SPEED)
            , fadeToColor_(FADE_TO_COLOR)
        {}

        virtual ~LoopCmd_FadeOut() = default;
        void Execute() override;
        const std::string Name() const override;

    private:
        float speed_;
        sf::Color fadeToColor_;
    };

    // Responsible for setting the game loop to exit after a keypress
    class LoopCmd_ExitAfterKeypress : public LoopCmd
    {
    public:
        explicit LoopCmd_ExitAfterKeypress(const bool WILL_EXIT_ON_KEYPRESS)
            : LoopCmd("ExitOnKeypress")
            , willExitOnKeypress_(WILL_EXIT_ON_KEYPRESS)
        {}

        virtual ~LoopCmd_ExitAfterKeypress() = default;

        void Execute() override;

        const std::string Name() const override
        {
            std::ostringstream ss;
            ss << LoopCmd::Name() << "To=" << std::boolalpha << willExitOnKeypress_;
            return ss.str();
        }

    private:
        bool willExitOnKeypress_;
    };

    // Responsible for setting the game loop to exit after a mouse-click
    class LoopCmd_ExitAfterMouseclick : public LoopCmd
    {
    public:
        explicit LoopCmd_ExitAfterMouseclick(const bool WILL_EXIT_ON_MOUSECLICK)
            : LoopCmd("ExitOnMouseclick")
            , willExitOnMouseClick_(WILL_EXIT_ON_MOUSECLICK)
        {}

        virtual ~LoopCmd_ExitAfterMouseclick() = default;

        void Execute() override;

        const std::string Name() const override
        {
            std::ostringstream ss;
            ss << LoopCmd::Name() << "To=" << std::boolalpha << willExitOnMouseClick_;
            return ss.str();
        }

    private:
        bool willExitOnMouseClick_;
    };

    // Responsible for removing focus from all GuiEntities
    class LoopCmd_RemoveFocus : public LoopCmd
    {
    public:
        LoopCmd_RemoveFocus()
            : LoopCmd("RemoveFocusFromAllEntities")
        {}

        virtual ~LoopCmd_RemoveFocus() = default;
        void Execute() override;
    };

    // Responsible for setting the game loop to ignore mouse movement
    class LoopCmd_IgnoreMouse : public LoopCmd
    {
    public:
        explicit LoopCmd_IgnoreMouse(const bool WILL_IGNORE_MOUSE)
            : LoopCmd("IgnoreMouse")
            , willIgnoreMouse_(WILL_IGNORE_MOUSE)
        {}

        virtual ~LoopCmd_IgnoreMouse() = default;
        void Execute() override;

    private:
        bool willIgnoreMouse_;
    };

    // Respobsible for setting the game loop to ignore keystrokes
    class LoopCmd_IgnoreKeystrokes : public LoopCmd
    {
    public:
        explicit LoopCmd_IgnoreKeystrokes(const bool WILL_IGNORE_KEYSTROKES)
            : LoopCmd("IgnoreKeystrokes")
            , willIgnoreKeyStrokes_(WILL_IGNORE_KEYSTROKES)
        {}

        virtual ~LoopCmd_IgnoreKeystrokes() = default;
        void Execute() override;

    private:
        bool willIgnoreKeyStrokes_;
    };

    // Responsible fork faking a mouse click at the given location
    class LoopCmd_FakeMouseClick : public LoopCmd
    {
    public:
        explicit LoopCmd_FakeMouseClick(const sf::Vector2f & MOUSE_POS_V)
            : LoopCmd("FakeMouseClick")
            , clickPosV_(MOUSE_POS_V)
        {}

        virtual ~LoopCmd_FakeMouseClick() = default;

        void Execute() override;

        const std::string Name() const override;

    private:
        sf::Vector2f clickPosV_;
    };

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_LOOPCOMMAND_HPP_INCLUDED
