#ifndef SFMLUTIL_LOOPCOMMAND_INCLUDED
#define SFMLUTIL_LOOPCOMMAND_INCLUDED
//
// loop-cmd.hpp
//  Code that encapsulates a command that performs some action on a Loop object.
//
#include "sfml-util/sfml-util.hpp"
#include "sfml-util/iloop.hpp"
#include "sfml-util/stage.hpp"
#include "sfml-util/sound-manager.hpp"

#include "game/loop-state-enum.hpp"

#include <memory>
#include <string>
#include <vector>
#include <sstream>


namespace sfml_util
{

    //interface class for all loop commands
    class ILoopCmd
    {
    public:
        virtual ~ILoopCmd() {}

        //all loop commands have meaningful names for logging
        virtual const std::string GetName() const = 0;

        //perform some action on the loop
        //significance of the return value to be defined by the derived type.
        virtual bool Execute() = 0;
    };

    using ILoopCmdSPtr_t = std::shared_ptr<ILoopCmd>;
    using ILoopCmdSVec_t = std::vector<ILoopCmdSPtr_t>;


    //A type that performs some action on a Loop
    class LoopCmd : public ILoopCmd
    {
        //prevent copy construction
        LoopCmd(const LoopCmd &) =delete;

        //prevent copy assignment
        LoopCmd & operator=(const LoopCmd &) =delete;

    public:
        LoopCmd(const std::string & NAME, ILoopSPtr_t & iLoopSPtr);
        virtual ~LoopCmd();

        virtual const std::string GetName() const { return NAME_; }

        virtual bool Execute() = 0;

    protected:
        const std::string NAME_;
        ILoopSPtr_t iLoopSPtr_;
    };



    //executes the loop
    class LoopCmd_Execute : public LoopCmd
    {
    public:
        explicit LoopCmd_Execute(ILoopSPtr_t & iLoopSPtr)
        :
            LoopCmd("Execute", iLoopSPtr)
        {}

        virtual ~LoopCmd_Execute()
        {}

        virtual bool Execute()
        {
            return iLoopSPtr_->Execute();
        }
    };



    //holds a LoopManager state change
    class LoopCmd_StateChange : public LoopCmd
    {
    public:
        LoopCmd_StateChange(ILoopSPtr_t & iLoopSPtr, const game::LoopState::Enum STATE_NUM)
        :
            LoopCmd("LoopStateChange", iLoopSPtr),
            STATE_ (STATE_NUM)
        {}

        virtual ~LoopCmd_StateChange()
        {}

        virtual bool Execute()
        {
            iLoopSPtr_->SetState(STATE_);
            return true;
        }

        virtual const std::string GetName() const
        {
            std::ostringstream ss;
            ss << NAME_ << " new_state=" << game::LoopState::ToString(STATE_);
            return ss.str();
        }

    private:
        const game::LoopState::Enum STATE_;
    };



    //Sets the visibility of the mouse the loop
    class LoopCmd_SetMouseVisibility : public LoopCmd
    {
    public:
        LoopCmd_SetMouseVisibility(ILoopSPtr_t & iLoopSPtr, const bool IS_VISIBLE)
        :
            LoopCmd    ("SetMouseVisibility", iLoopSPtr),
            IS_VISIBLE_(IS_VISIBLE)
        {}

        virtual ~LoopCmd_SetMouseVisibility()
        {}

        virtual bool Execute()
        {
            iLoopSPtr_->SetMouseVisibility(IS_VISIBLE_);
            return true;
        }

        virtual const std::string GetName() const
        {
            std::ostringstream ss;
            ss << NAME_ << " visible=" << std::boolalpha << IS_VISIBLE_;
            return ss.str();
        }

    private:
        const bool IS_VISIBLE_;
    };



    //Starts the theme music playing in the Loop
    class LoopCmd_StartMusic : public LoopCmd
    {
    public:
        LoopCmd_StartMusic(ILoopSPtr_t &     iLoopSPtr,
                           const music::Enum MUSIC_TO_START,
                           const float       SPEED_MULT     = MusicOperator::FADE_MULT_DEFAULT_IN_,
                           const float       TARGET_VOLUME  = MusicOperator::VOLUME_USE_GLOBAL_)
        :
            LoopCmd        ("StartMusic", iLoopSPtr),
            MUSIC_TO_START_(MUSIC_TO_START),
            TARGET_VOLUME_ (TARGET_VOLUME),
            SPEED_MULT_    (SPEED_MULT)
        {}

        virtual ~LoopCmd_StartMusic()
        {}

        virtual bool Execute()
        {
            sfml_util::SoundManager::Instance()->MusicStart(MUSIC_TO_START_, SPEED_MULT_, TARGET_VOLUME_);
            return true;
        }

        virtual const std::string GetName() const
        {
            std::ostringstream ss;

            ss << NAME_ << " \"" << music::ToString(MUSIC_TO_START_)
               << "\", target_vol=" << TARGET_VOLUME_
               << ", speed_mult=" << SPEED_MULT_;

            return ss.str();
        }

    private:
        const music::Enum MUSIC_TO_START_;
        const float       TARGET_VOLUME_;
        const float       SPEED_MULT_;
    };



    //Instructs the Loop to start fading out the theme music
    class LoopCmd_StopMusic : public LoopCmd
    {
    public:
        LoopCmd_StopMusic(ILoopSPtr_t &       iLoopSPtr,
                          const music::Enum   MUSIC_TO_STOP,
                          const float         SPEED_MULT    = MusicOperator::FADE_MULT_DEFAULT_OUT_)
        :
            LoopCmd       ("StopMusic", iLoopSPtr),
            MUSIC_TO_STOP_(MUSIC_TO_STOP),
            SPEED_MULT_   (SPEED_MULT)
        {}

        virtual ~LoopCmd_StopMusic()
        {}

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
        const float       SPEED_MULT_;
    };



    //adds the default stage to a Loop
    class LoopCmd_AddStage_Default : public LoopCmd
    {
    public:
        explicit LoopCmd_AddStage_Default(ILoopSPtr_t & iLoopSPtr)
        :
            LoopCmd("AddStageDefault", iLoopSPtr)
        {}

        virtual ~LoopCmd_AddStage_Default()
        {}

        virtual bool Execute()
        {
            sfml_util::IStageSPtr_t stageSPtr( new Stage("Default") );
            iLoopSPtr_->AddStage( stageSPtr );
            return true;
        }
    };



    //adds a hold time to the loop
    class LoopCmd_SetHoldTime : public LoopCmd
    {
    public:
        LoopCmd_SetHoldTime(ILoopSPtr_t & iLoopSPtr, const float SECONDS)
        :
            LoopCmd("SetHoldTime", iLoopSPtr),
            TIME_  (SECONDS)
        {}

        virtual ~LoopCmd_SetHoldTime()
        {}

        virtual bool Execute()
        {
            iLoopSPtr_->SetHoldTime(TIME_);
            return true;
        }

        virtual const std::string GetName() const
        {
            std::ostringstream ss;
            ss << NAME_ << " seconds=" << TIME_;
            return ss.str();
        }

    private:
        const float TIME_;
    };



    //adds a hold time to the loop
    class LoopCmd_RemoveAllStages : public LoopCmd
    {
    public:
        explicit LoopCmd_RemoveAllStages(ILoopSPtr_t & iLoopSPtr)
        :
            LoopCmd("RemoveAllStages", iLoopSPtr)
        {}

        virtual ~LoopCmd_RemoveAllStages()
        {}

        virtual bool Execute()
        {
            iLoopSPtr_->RemoveAllStages();
            return true;
        }
    };



    //sets the loop to terminate after the current or next fade
    class LoopCmd_ExitAfterFade : public LoopCmd
    {
    public:
        explicit LoopCmd_ExitAfterFade(ILoopSPtr_t & iLoopSPtr)
        :
            LoopCmd("ExitAfterFade", iLoopSPtr)
        {}

        virtual ~LoopCmd_ExitAfterFade()
        {}

        virtual bool Execute()
        {
            iLoopSPtr_->SetWillExitAfterFade(true);
            return true;
        }
    };



    //fades a loop object in
    class LoopCmd_FadeIn : public LoopCmd
    {
    public:
        explicit LoopCmd_FadeIn(ILoopSPtr_t &     iLoopSPtr,
                                const sf::Color & FADE_FROM_COLOR = sf::Color::Black,
                                const float       SPEED_MULT      = 200.0f,
                                const bool        WILL_HOLD_FADE  = false )
        :
            LoopCmd         (std::string("FadeIn"), iLoopSPtr),
            SPEED_MULT_     (SPEED_MULT),
            FADE_FROM_COLOR_(FADE_FROM_COLOR),
            WILL_HOLD_FADE_ (WILL_HOLD_FADE)
        {}

        virtual ~LoopCmd_FadeIn()
        {}

        virtual bool Execute()
        {
            iLoopSPtr_->FadeIn(FADE_FROM_COLOR_, SPEED_MULT_, WILL_HOLD_FADE_);
            return true;
        }

        virtual const std::string GetName() const
        {
            std::ostringstream ss;
            ss << NAME_
               << " fade_from_color=" << sfml_util::color::ColorToString(FADE_FROM_COLOR_)
               << ", speed_mult=" << SPEED_MULT_
               << ", will_hold_after_fade=" << std::boolalpha << WILL_HOLD_FADE_;
            return ss.str();
        }

    private:
        const float     SPEED_MULT_;
        const sf::Color FADE_FROM_COLOR_;
        bool            WILL_HOLD_FADE_;
    };



    //fades-out a loop object in
    class LoopCmd_FadeOut : public LoopCmd
    {
    public:
        explicit LoopCmd_FadeOut(ILoopSPtr_t &     iLoopSPtr,
                                 const sf::Color & FADE_TO_COLOR  = sf::Color::Black,
                                 const float       SPEED_MULT     = 300.0f,
                                 const bool        WILL_HOLD_FADE = false)
        :
            LoopCmd        (std::string("FadeOut"), iLoopSPtr),
            SPEED_MULT_    (SPEED_MULT),
            FADE_TO_COLOR_ (FADE_TO_COLOR),
            WILL_HOLD_FADE_(WILL_HOLD_FADE)
        {}

        virtual ~LoopCmd_FadeOut()
        {}

        virtual bool Execute()
        {
            iLoopSPtr_->FadeOut(FADE_TO_COLOR_, SPEED_MULT_, WILL_HOLD_FADE_);
            return true;
        }

        virtual const std::string GetName() const
        {
            std::ostringstream ss;
            ss << NAME_
               << " fade_to_color=" << sfml_util::color::ColorToString(FADE_TO_COLOR_)
               << ", speed_mult=" << SPEED_MULT_
               << ", will_hold_after_fade=" << std::boolalpha << WILL_HOLD_FADE_;
            return ss.str();
        }

    private:
        const float     SPEED_MULT_;
        const sf::Color FADE_TO_COLOR_;
        const bool      WILL_HOLD_FADE_;
    };



    //sets the loop to terminate after keypresses
    class LoopCmd_ExitAfterKeypress : public LoopCmd
    {
    public:
        LoopCmd_ExitAfterKeypress(ILoopSPtr_t & iLoopSPtr, const bool WILL_EXIT_ON_KEYSTROKE)
        :
            LoopCmd("ExitOnKeypress", iLoopSPtr),
            WILL_EXIT_ON_KEYSTROKE_(WILL_EXIT_ON_KEYSTROKE)
        {}

        virtual ~LoopCmd_ExitAfterKeypress()
        {}

        virtual bool Execute()
        {
            iLoopSPtr_->SetWillExitOnKeypress(WILL_EXIT_ON_KEYSTROKE_);
            return true;
        }

        virtual const std::string GetName() const
        {
            std::ostringstream ss;
            ss  << NAME_
                << " willExitOnKeypress=" << std::boolalpha << WILL_EXIT_ON_KEYSTROKE_;
            return ss.str();
        }

    private:
        const bool WILL_EXIT_ON_KEYSTROKE_;
    };



    //sets the loop to terminate after mouse clicks
    class LoopCmd_ExitAfterMouseclick : public LoopCmd
    {
    public:
        LoopCmd_ExitAfterMouseclick(ILoopSPtr_t & iLoopSPtr, const bool WILL_EXIT_ON_MOUSECLICK)
        :
            LoopCmd("ExitOnMouseclick", iLoopSPtr),
            WILL_EXIT_ON_MOUSECLICK_(WILL_EXIT_ON_MOUSECLICK)
        {}

        virtual ~LoopCmd_ExitAfterMouseclick()
        {}

        virtual bool Execute()
        {
            iLoopSPtr_->SetWillExitOnMouseclick(WILL_EXIT_ON_MOUSECLICK_);
            return true;
        }

        virtual const std::string GetName() const
        {
            std::ostringstream ss;
            ss  << NAME_
                << " willExitOnMouseclicks=" << std::boolalpha << WILL_EXIT_ON_MOUSECLICK_;
            return ss.str();
        }

    private:
        const bool WILL_EXIT_ON_MOUSECLICK_;
    };



    //removes focus from all GuiEntitys
    class LoopCmd_RemoveFocus : public LoopCmd
    {
    public:
        explicit LoopCmd_RemoveFocus(ILoopSPtr_t & iLoopSPtr)
        :
            LoopCmd("RemoveFocus", iLoopSPtr)
        {}

        virtual ~LoopCmd_RemoveFocus()
        {}

        virtual bool Execute()
        {
            iLoopSPtr_->RemoveFocus();
            return true;
        }
    };



    //ignores mouse movement
    class LoopCmd_IgnoreMouse : public LoopCmd
    {
    public:
        LoopCmd_IgnoreMouse(ILoopSPtr_t & iLoopSPtr, const bool WILL_IGNORE_MOUSE)
        :
            LoopCmd("IgnoreMouse", iLoopSPtr),
            WILL_IGNORE_MOUSE_(WILL_IGNORE_MOUSE)
        {}

        virtual ~LoopCmd_IgnoreMouse()
        {}

        virtual bool Execute()
        {
            iLoopSPtr_->SetIgnoreMouse(WILL_IGNORE_MOUSE_);
            return true;
        }
    private:
        const bool WILL_IGNORE_MOUSE_;
    };


    //ignores keystrokes
    class LoopCmd_IgnoreKeystrokes : public LoopCmd
    {
    public:
        LoopCmd_IgnoreKeystrokes(ILoopSPtr_t & iLoopSPtr, const bool WILL_IGNORE_KEYSTROKES)
            :
            LoopCmd("IgnoreKeystrokes", iLoopSPtr),
            WILL_IGNORE_KEYSTROKES_(WILL_IGNORE_KEYSTROKES)
        {}

        virtual ~LoopCmd_IgnoreKeystrokes()
        {}

        virtual bool Execute()
        {
            iLoopSPtr_->SetIgnoreKeystrokes(WILL_IGNORE_KEYSTROKES_);
            return true;
        }
    private:
        const bool WILL_IGNORE_KEYSTROKES_;
    };


    //fakes a mouse click at the given location
    class LoopCmd_FakeMouseClick : public LoopCmd
    {
    public:
        LoopCmd_FakeMouseClick(ILoopSPtr_t & iLoopSPtr, const sf::Vector2f & MOUSE_POS_V)
            :
            LoopCmd("FakeMouseClick", iLoopSPtr),
            MOUSE_CLICK_POS_(MOUSE_POS_V)
        {}

        virtual ~LoopCmd_FakeMouseClick()
        {}

        virtual bool Execute()
        {
            iLoopSPtr_->FakeMouseClick(MOUSE_CLICK_POS_);
            return true;
        }

        virtual const std::string GetName() const
        {
            std::ostringstream ss;
            ss << NAME_ << " at " << sfml_util::VectorToString(MOUSE_CLICK_POS_);
            return ss.str();
        }

    private:
        const sf::Vector2f MOUSE_CLICK_POS_;
    };

    using LoopCmdSPtr_t = std::shared_ptr<LoopCmd>;
    using LoopCmdSVec_t = std::vector<LoopCmdSPtr_t>;

}
#endif //SFMLUTIL_LOOPCOMMAND_INCLUDED
