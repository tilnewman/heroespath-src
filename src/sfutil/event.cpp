// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// event.cpp
//
#include "event.hpp"

#include "misc/strings.hpp"
#include "sfutil/keyboard.hpp"

#include <ostream>

namespace heroespath
{
namespace sfutil
{
    namespace helpers
    {

        void eventToDetailsStream(std::ostream & os, const sf::Event & EVENT)
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
                    os << "(none)";
                    break;
                }
                case sf::Event::EventType::Resized:
                {
                    os << M_HP_VAR_STR(EVENT.size.width) << M_HP_VAR_STR(EVENT.size.height);
                    break;
                }
                case sf::Event::EventType::TextEntered:
                {
                    os << M_HP_VAR_STR(EVENT.text.unicode);
                    break;
                }
                case sf::Event::EventType::KeyPressed:
                case sf::Event::EventType::KeyReleased:
                {
                    os << "(key_event=" << EVENT.key << ")";
                    break;
                }
                case sf::Event::EventType::MouseWheelMoved:
                {
                    os << "(deprecated)";
                    break;
                }
                case sf::Event::EventType::MouseWheelScrolled:
                {
                    os << "(which_wheel="
                       << ((EVENT.mouseWheelScroll.wheel == sf::Mouse::Wheel::HorizontalWheel)
                               ? "horiz"
                               : "vert")
                       << M_HP_VAR_STR(EVENT.mouseWheelScroll.delta)
                       << M_HP_VAR_STR(EVENT.mouseWheelScroll.x)
                       << M_HP_VAR_STR(EVENT.mouseWheelScroll.y);
                    break;
                }
                case sf::Event::EventType::MouseButtonPressed:
                case sf::Event::EventType::MouseButtonReleased:
                {
                    const auto BUTTON_NAME = [&]() -> std::string {
                        switch (EVENT.mouseButton.button)
                        {
                            case sf::Mouse::Button::Left: { return "Left";
                            }
                            case sf::Mouse::Button::Right: { return "Right";
                            }
                            case sf::Mouse::Button::Middle: { return "Middle";
                            }
                            case sf::Mouse::Button::XButton1: { return "XButton1";
                            }
                            case sf::Mouse::Button::XButton2: { return "XButton2";
                            }
                            case sf::Mouse::Button::ButtonCount: { return "Count";
                            }
                            default: { return "(sf::Event::MouseButton_out_of_range)";
                            }
                        }
                    }();

                    os << "(button=" << BUTTON_NAME << ")" << M_HP_VAR_STR(EVENT.mouseButton.x)
                       << M_HP_VAR_STR(EVENT.mouseButton.y);

                    break;
                }
                case sf::Event::EventType::MouseMoved:
                {
                    os << M_HP_VAR_STR(EVENT.mouseMove.x) << M_HP_VAR_STR(EVENT.mouseMove.y);
                    break;
                }
                case sf::Event::EventType::Count:
                {
                    os << "Count";
                    break;
                }
                default:
                {
                    os << "(value " << int(EVENT.type) << " out of range error)";
                    break;
                }
            }
        }

    } // namespace helpers
} // namespace sfutil
} // namespace heroespath

namespace sf
{

std::ostream & operator<<(std::ostream & os, const sf::Event & EVENT)
{
    os << heroespath::sfutil::helpers::eventToNameString(EVENT) << ", details=";
    heroespath::sfutil::helpers::eventToDetailsStream(os, EVENT);
    os << ", \"" << heroespath::sfutil::helpers::eventToDescriptionString(EVENT) << "\"";
    return os;
}

std::ostream & operator<<(std::ostream & os, const sf::Event::KeyEvent & KEY_EVENT)
{
    os << "(key=" << KEY_EVENT.code;

    if (KEY_EVENT.alt)
    {
        os << "+alt";
    }

    if (KEY_EVENT.control)
    {
        os << "+cntrl";
    }

    if (KEY_EVENT.shift)
    {
        os << "+shift";
    }

    if (KEY_EVENT.system)
    {
        os << "+system";
    }

    os << ")";

    return os;
}

} // namespace sf
