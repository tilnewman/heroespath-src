// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFUTIL_EVENT_HPP_INCLUDED
#define HEROESPATH_SFUTIL_EVENT_HPP_INCLUDED
//
// event.hpp
//
#include "misc/boost-optional-that-throws.hpp"
#include "misc/log-macros.hpp"
#include "misc/strings.hpp"
#include "sfutil/keyboard.hpp"

#include <SFML/Window/Event.hpp>

namespace heroespath
{

namespace sfutil
{

    inline const std::string ToNameString(const sf::Event & EVENT)
    {
        switch (EVENT.type)
        {
            case sf::Event::EventType::Closed:
            {
                return "Closed";
            }
            case sf::Event::EventType::Resized:
            {
                return "Resized";
            }
            case sf::Event::EventType::LostFocus:
            {
                return "LostFocus";
            }
            case sf::Event::EventType::GainedFocus:
            {
                return "GainedFocus";
            }
            case sf::Event::EventType::TextEntered:
            {
                return "TextEntered";
            }
            case sf::Event::EventType::KeyPressed:
            {
                return "KeyPressed";
            }
            case sf::Event::EventType::KeyReleased:
            {
                return "KeyReleased";
            }
            case sf::Event::EventType::MouseWheelMoved:
            {
                return "MouseWheelMoved";
            }
            case sf::Event::EventType::MouseWheelScrolled:
            {
                return "MouseWheelScrolled";
            }
            case sf::Event::EventType::MouseButtonPressed:
            {
                return "MouseButtonPressed";
            }
            case sf::Event::EventType::MouseButtonReleased:
            {
                return "MouseButtonReleased";
            }
            case sf::Event::EventType::MouseMoved:
            {
                return "MouseMoved";
            }
            case sf::Event::EventType::MouseEntered:
            {
                return "MouseEntered";
            }
            case sf::Event::EventType::MouseLeft:
            {
                return "MouseLeft";
            }
            case sf::Event::EventType::JoystickButtonPressed:
            {
                return "JoystickButtonPressed";
            }
            case sf::Event::EventType::JoystickButtonReleased:
            {
                return "JoystickButtonReleased";
            }
            case sf::Event::EventType::JoystickMoved:
            {
                return "JoystickMoved";
            }
            case sf::Event::EventType::JoystickConnected:
            {
                return "JoystickConnected";
            }
            case sf::Event::EventType::JoystickDisconnected:
            {
                return "JoystickDisconnected";
            }
            case sf::Event::EventType::TouchBegan:
            {
                return "TouchBegan";
            }
            case sf::Event::EventType::TouchMoved:
            {
                return "TouchMoved";
            }
            case sf::Event::EventType::TouchEnded:
            {
                return "TouchEnded";
            }
            case sf::Event::EventType::SensorChanged:
            {
                return "SensorChanged";
            }
            case sf::Event::EventType::Count:
            {
                return "(Count)";
            }
            default:
            {
                std::ostringstream ss;
                ss << "(value " << int(EVENT.type) << " out of range error)";
                return ss.str();
            }
        }
    }

    inline const std::string ToDescriptionString(const sf::Event & EVENT)
    {
        switch (EVENT.type)
        {
            case sf::Event::EventType::Closed:
            {
                return "The window requested to be closed.";
            }
            case sf::Event::EventType::Resized:
            {
                return "The window was resized.";
            }
            case sf::Event::EventType::LostFocus:
            {
                return "The window lost the focus.";
            }
            case sf::Event::EventType::GainedFocus:
            {
                return "The window gained the focus.";
            }
            case sf::Event::EventType::TextEntered:
            {
                return "A character was entered.";
            }
            case sf::Event::EventType::KeyPressed:
            {
                return "A key was pressed.";
            }
            case sf::Event::EventType::KeyReleased:
            {
                return "A key was released.";
            }
            case sf::Event::EventType::MouseWheelMoved:
            {
                return "The mouse wheel was scrolled. (deprecated)";
            }
            case sf::Event::EventType::MouseWheelScrolled:
            {
                return "The mouse wheel was scrolled.";
            }
            case sf::Event::EventType::MouseButtonPressed:
            {
                return "A mouse button was pressed.";
            }
            case sf::Event::EventType::MouseButtonReleased:
            {
                return "A mouse button was released.";
            }
            case sf::Event::EventType::MouseMoved:
            {
                return "The mouse cursor moved.";
            }
            case sf::Event::EventType::MouseEntered:
            {
                return "The mouse cursor entered the area of the window.";
            }
            case sf::Event::EventType::MouseLeft:
            {
                return "The mouse cursor left the area of the window.";
            }
            case sf::Event::EventType::JoystickButtonPressed:
            {
                return "A joystick button was pressed.";
            }
            case sf::Event::EventType::JoystickButtonReleased:
            {
                return "A joystick button was released.";
            }
            case sf::Event::EventType::JoystickMoved:
            {
                return "The joystick moved along an axis.";
            }
            case sf::Event::EventType::JoystickConnected:
            {
                return "A joystick was connected.";
            }
            case sf::Event::EventType::JoystickDisconnected:
            {
                return "A joystick was disconnected.";
            }
            case sf::Event::EventType::TouchBegan:
            {
                return "A touch event began.";
            }
            case sf::Event::EventType::TouchMoved:
            {
                return "A touch moved.";
            }
            case sf::Event::EventType::TouchEnded:
            {
                return "A touch event ended.";
            }
            case sf::Event::EventType::SensorChanged:
            {
                return "A sensor value changed.";
            }
            case sf::Event::EventType::Count:
            {
                return "(Count)";
            }
            default:
            {
                std::ostringstream ss;
                ss << "(value " << int(EVENT.type) << " out of range error)";
                return ss.str();
            }
        }
    }

    inline const std::string ToString(const sf::Event::KeyEvent & KEY_EVENT)
    {
        std::ostringstream ss;

        ss << "(key=" << sfutil::ToString(KEY_EVENT.code);

        if (KEY_EVENT.alt)
        {
            ss << "+alt";
        }

        if (KEY_EVENT.control)
        {
            ss << "+cntrl";
        }

        if (KEY_EVENT.shift)
        {
            ss << "+shift";
        }

        if (KEY_EVENT.system)
        {
            ss << "+system";
        }

        ss << ")";

        return ss.str();
    }

    inline const std::string ToDetailsString(const sf::Event & EVENT)
    {
        switch (EVENT.type)
        {
            case sf::Event::EventType::Closed:
            case sf::Event::EventType::LostFocus:
            case sf::Event::EventType::GainedFocus:
            case sf::Event::EventType::MouseEntered:
            case sf::Event::EventType::MouseLeft:
            case sf::Event::EventType::JoystickButtonPressed:
            case sf::Event::EventType::JoystickButtonReleased:
            case sf::Event::EventType::JoystickMoved:
            case sf::Event::EventType::JoystickConnected:
            case sf::Event::EventType::JoystickDisconnected:
            case sf::Event::EventType::TouchBegan:
            case sf::Event::EventType::TouchMoved:
            case sf::Event::EventType::TouchEnded:
            case sf::Event::EventType::SensorChanged:
            {
                return "(none)";
            }
            case sf::Event::EventType::Resized:
            {
                return M_HP_VAR_STR(EVENT.size.width) + M_HP_VAR_STR(EVENT.size.height);
            }
            case sf::Event::EventType::TextEntered:
            {
                return M_HP_VAR_STR(EVENT.text.unicode);
            }
            case sf::Event::EventType::KeyPressed:
            case sf::Event::EventType::KeyReleased:
            {
                return "(key_event=" + ToString(EVENT.key) + ")";
            }
            case sf::Event::EventType::MouseWheelMoved:
            {
                return "(deprecated)";
            }
            case sf::Event::EventType::MouseWheelScrolled:
            {
                return std::string("(which_wheel=")
                    + ((EVENT.mouseWheelScroll.wheel == sf::Mouse::Wheel::HorizontalWheel) ? "horiz"
                                                                                           : "vert")
                    + M_HP_VAR_STR(EVENT.mouseWheelScroll.delta)
                    + M_HP_VAR_STR(EVENT.mouseWheelScroll.x)
                    + M_HP_VAR_STR(EVENT.mouseWheelScroll.y);
            }
            case sf::Event::EventType::MouseButtonPressed:
            case sf::Event::EventType::MouseButtonReleased:
            {
                const auto BUTTON_NAME = [&]() -> std::string {
                    switch (EVENT.mouseButton.button)
                    {
                        case sf::Mouse::Button::Left:
                        {
                            return "Left";
                        }
                        case sf::Mouse::Button::Right:
                        {
                            return "Right";
                        }
                        case sf::Mouse::Button::Middle:
                        {
                            return "Middle";
                        }
                        case sf::Mouse::Button::XButton1:
                        {
                            return "XButton1";
                        }
                        case sf::Mouse::Button::XButton2:
                        {
                            return "XButton2";
                        }
                        case sf::Mouse::Button::ButtonCount:
                        {
                            return "(Count)";
                        }
                        default:
                        {
                            std::ostringstream ss;
                            ss << "(value " << int(EVENT.mouseButton.button)
                               << " out of range error)";
                            return ss.str();
                        }
                    }
                }();

                return "(button=" + BUTTON_NAME + ")" + M_HP_VAR_STR(EVENT.mouseButton.x)
                    + M_HP_VAR_STR(EVENT.mouseButton.y);
            }
            case sf::Event::EventType::MouseMoved:
            {
                return M_HP_VAR_STR(EVENT.mouseMove.x) + M_HP_VAR_STR(EVENT.mouseMove.y);
            }
            case sf::Event::EventType::Count:
            {
                return "(Count)";
            }
            default:
            {
                std::ostringstream ss;
                ss << "(value " << int(EVENT.type) << " out of range error)";
                return ss.str();
            }
        }
    }

} // namespace sfutil

inline const std::string ToString(
    const sf::Event & EVENT,
    const misc::ToStringPrefix::Enum OPTIONS = misc::ToStringPrefix::Default)
{
    std::ostringstream ss;

    ss << sfutil::ToNameString(EVENT) << ", details=" << sfutil::ToDetailsString(EVENT) << ", \""
       << sfutil::ToDescriptionString(EVENT) << "\"";

    return misc::MakeToStringPrefix(OPTIONS, "Event") + ss.str();
}

inline const std::string ToString(
    const sf::Event::KeyEvent & KEY_EVENT,
    const misc::ToStringPrefix::Enum OPTIONS = misc::ToStringPrefix::Default)
{
    return misc::MakeToStringPrefix(OPTIONS, "KeyEvent") + sfutil::ToString(KEY_EVENT);
}

inline std::ostream & operator<<(std::ostream & os, const sf::Event & EVENT)
{
    os << ToString(EVENT);
    return os;
}

inline std::ostream & operator<<(std::ostream & os, const sf::Event::KeyEvent & KEY_EVENT)
{
    os << ToString(KEY_EVENT);
    return os;
}

} // namespace heroespath

#endif // HEROESPATH_SFUTIL_EVENT_HPP_INCLUDED
