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
#include <SFML/Window/Event.hpp>

#include <iosfwd>

namespace heroespath
{
namespace sfutil
{
    namespace helpers
    {

        constexpr std::string_view eventToNameString(const sf::Event & EVENT) noexcept
        {
            switch (EVENT.type)
            {
                case sf::Event::EventType::Closed: return "Closed";
                case sf::Event::EventType::Resized: return "Resized";
                case sf::Event::EventType::LostFocus: return "LostFocus";
                case sf::Event::EventType::GainedFocus: return "GainedFocus";
                case sf::Event::EventType::TextEntered: return "TextEntered";
                case sf::Event::EventType::KeyPressed: return "KeyPressed";
                case sf::Event::EventType::KeyReleased: return "KeyReleased";
                case sf::Event::EventType::MouseWheelMoved: return "MouseWheelMoved";
                case sf::Event::EventType::MouseWheelScrolled: return "MouseWheelScrolled";
                case sf::Event::EventType::MouseButtonPressed: return "MouseButtonPressed";
                case sf::Event::EventType::MouseButtonReleased: return "MouseButtonReleased";
                case sf::Event::EventType::MouseMoved: return "MouseMoved";
                case sf::Event::EventType::MouseEntered: return "MouseEntered";
                case sf::Event::EventType::MouseLeft: return "MouseLeft";
                case sf::Event::EventType::JoystickButtonPressed: return "JoystickButtonPressed";
                case sf::Event::EventType::JoystickButtonReleased: return "JoystickButtonReleased";
                case sf::Event::EventType::JoystickMoved: return "JoystickMoved";
                case sf::Event::EventType::JoystickConnected: return "JoystickConnected";
                case sf::Event::EventType::JoystickDisconnected: return "JoystickDisconnected";
                case sf::Event::EventType::TouchBegan: return "TouchBegan";
                case sf::Event::EventType::TouchMoved: return "TouchMoved";
                case sf::Event::EventType::TouchEnded: return "TouchEnded";
                case sf::Event::EventType::SensorChanged: return "SensorChanged";
                case sf::Event::EventType::Count: return "Count";
                default:
                    return "sfutil::helpers::eventToNameString(sf::Event::Event.type=out_of_range)";
            }
        }

        constexpr std::string_view eventToDescriptionString(const sf::Event & EVENT) noexcept
        {
            switch (EVENT.type)
            {
                case sf::Event::EventType::Closed: return "The window requested to be closed.";
                case sf::Event::EventType::Resized: return "The window was resized.";
                case sf::Event::EventType::LostFocus: return "The window lost the focus.";
                case sf::Event::EventType::GainedFocus: return "The window gained the focus.";
                case sf::Event::EventType::TextEntered: return "A character was entered.";
                case sf::Event::EventType::KeyPressed: return "A key was pressed.";
                case sf::Event::EventType::KeyReleased: return "A key was released.";
                case sf::Event::EventType::MouseWheelMoved:
                    return "The mouse wheel was scrolled. (deprecated)";
                case sf::Event::EventType::MouseWheelScrolled:
                    return "The mouse wheel was scrolled.";
                case sf::Event::EventType::MouseButtonPressed: return "A mouse button was pressed.";
                case sf::Event::EventType::MouseButtonReleased:
                    return "A mouse button was released.";
                case sf::Event::EventType::MouseMoved: return "The mouse cursor moved.";
                case sf::Event::EventType::MouseEntered:
                    return "The mouse cursor entered the area of the window.";
                case sf::Event::EventType::MouseLeft:
                    return "The mouse cursor left the area of the window.";
                case sf::Event::EventType::JoystickButtonPressed:
                    return "A joystick button was pressed.";
                case sf::Event::EventType::JoystickButtonReleased:
                    return "A joystick button was released.";
                case sf::Event::EventType::JoystickMoved:
                    return "The joystick moved along an axis.";
                case sf::Event::EventType::JoystickConnected: return "A joystick was connected.";
                case sf::Event::EventType::JoystickDisconnected:
                    return "A joystick was disconnected.";
                case sf::Event::EventType::TouchBegan: return "A touch event began.";
                case sf::Event::EventType::TouchMoved: return "A touch moved.";
                case sf::Event::EventType::TouchEnded: return "A touch event ended.";
                case sf::Event::EventType::SensorChanged: return "A sensor value changed.";
                case sf::Event::EventType::Count: return "Count";
                default:
                    return "sfutil::helpers::eventToDescriptionString(sf::Event::Event.type=out_of_"
                           "range)";
            }
        }

        void eventToDetailsStream(std::ostream & os, const sf::Event & EVENT);

    } // namespace helpers

} // namespace sfutil

} // namespace heroespath

namespace sf
{

std::ostream & operator<<(std::ostream & os, const sf::Event & EVENT);
std::ostream & operator<<(std::ostream & os, const sf::Event::KeyEvent & KEY_EVENT);

} // namespace sf

#endif // HEROESPATH_SFUTIL_EVENT_HPP_INCLUDED
