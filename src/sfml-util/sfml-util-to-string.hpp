// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_SFMLUTIL_TO_STRING_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_SFMLUTIL_TO_STRING_HPP_INCLUDED
//
// sfml-util-to-string.hpp
//
#include "misc/enum-util.hpp"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/VideoMode.hpp>

#include <boost/type_index.hpp>

#include <sstream>
#include <string>

namespace sf
{

template <typename T>
std::ostream & operator<<(std::ostream & os, const sf::Vector2<T> & V)
{
    os << "(" << V.x << "," << V.y << ")";
    return os;
}

template <typename T>
std::ostream & operator<<(std::ostream & os, const sf::Rect<T> & R)
{
    os << "(" << R.left << "," << R.top << "/" << R.width << "x" << R.height << ")";
    return os;
}

inline std::ostream & operator<<(std::ostream & os, const sf::VideoMode & VM)
{
    os << "(" << VM.width << "x" << VM.height << ":" << VM.bitsPerPixel << ")";
    return os;
}

inline std::ostream & operator<<(std::ostream & os, const sf::Color & C)
{
    os << "(";

    if (sf::Color::Black == C)
    {
        os << "Black";
    }
    else if (sf::Color::White == C)
    {
        os << "White";
    }
    else if (sf::Color::Red == C)
    {
        os << "Red";
    }
    else if (sf::Color::Green == C)
    {
        os << "Green";
    }
    else if (sf::Color::Blue == C)
    {
        os << "Blue";
    }
    else if (sf::Color::Yellow == C)
    {
        os << "Yellow";
    }
    else if (sf::Color::Magenta == C)
    {
        os << "Magenta";
    }
    else if (sf::Color::Cyan == C)
    {
        os << "Cyan";
    }
    else
    {
        os << static_cast<unsigned>(C.r) << "," << static_cast<unsigned>(C.g) << ","
           << static_cast<unsigned>(C.b);

        if (C.a != 255)
        {
            os << "," << static_cast<unsigned>(C.a);
        }
    }

    os << ")";

    return os;
}

inline std::ostream & operator<<(std::ostream & os, const sf::Keyboard::Key KEY)
{
    switch (KEY)
    {
        case sf::Keyboard::A:
        {
            os << "A";
            break;
        }
        case sf::Keyboard::B:
        {
            os << "B";
            break;
        }
        case sf::Keyboard::C:
        {
            os << "C";
            break;
        }
        case sf::Keyboard::D:
        {
            os << "D";
            break;
        }
        case sf::Keyboard::E:
        {
            os << "E";
            break;
        }
        case sf::Keyboard::F:
        {
            os << "F";
            break;
        }
        case sf::Keyboard::G:
        {
            os << "G";
            break;
        }
        case sf::Keyboard::H:
        {
            os << "H";
            break;
        }
        case sf::Keyboard::I:
        {
            os << "I";
            break;
        }
        case sf::Keyboard::J:
        {
            os << "J";
            break;
        }
        case sf::Keyboard::K:
        {
            os << "K";
            break;
        }
        case sf::Keyboard::L:
        {
            os << "L";
            break;
        }
        case sf::Keyboard::M:
        {
            os << "M";
            break;
        }
        case sf::Keyboard::N:
        {
            os << "N";
            break;
        }
        case sf::Keyboard::O:
        {
            os << "O";
            break;
        }
        case sf::Keyboard::P:
        {
            os << "P";
            break;
        }
        case sf::Keyboard::Q:
        {
            os << "Q";
            break;
        }
        case sf::Keyboard::R:
        {
            os << "R";
            break;
        }
        case sf::Keyboard::S:
        {
            os << "S";
            break;
        }
        case sf::Keyboard::T:
        {
            os << "T";
            break;
        }
        case sf::Keyboard::U:
        {
            os << "U";
            break;
        }
        case sf::Keyboard::V:
        {
            os << "V";
            break;
        }
        case sf::Keyboard::W:
        {
            os << "W";
            break;
        }
        case sf::Keyboard::X:
        {
            os << "X";
            break;
        }
        case sf::Keyboard::Y:
        {
            os << "Y";
            break;
        }
        case sf::Keyboard::Z:
        {
            os << "Z";
            break;
        }
        case sf::Keyboard::Num0:
        {
            os << "Num0";
            break;
        }
        case sf::Keyboard::Num1:
        {
            os << "Num1";
            break;
        }
        case sf::Keyboard::Num2:
        {
            os << "Num2";
            break;
        }
        case sf::Keyboard::Num3:
        {
            os << "Num3";
            break;
        }
        case sf::Keyboard::Num4:
        {
            os << "Num4";
            break;
        }
        case sf::Keyboard::Num5:
        {
            os << "Num5";
            break;
        }
        case sf::Keyboard::Num6:
        {
            os << "Num6";
            break;
        }
        case sf::Keyboard::Num7:
        {
            os << "Num7";
            break;
        }
        case sf::Keyboard::Num8:
        {
            os << "Num8";
            break;
        }
        case sf::Keyboard::Num9:
        {
            os << "Num9";
            break;
        }
        case sf::Keyboard::Escape:
        {
            os << "Escape";
            break;
        }
        case sf::Keyboard::LControl:
        {
            os << "LControl";
            break;
        }
        case sf::Keyboard::LShift:
        {
            os << "LShift";
            break;
        }
        case sf::Keyboard::LAlt:
        {
            os << "LAlt";
            break;
        }
        case sf::Keyboard::LSystem:
        {
            os << "LSystem:";
            break;
        }
        case sf::Keyboard::RControl:
        {
            os << "RControl";
            break;
        }
        case sf::Keyboard::RShift:
        {
            os << "RShift";
            break;
        }
        case sf::Keyboard::RAlt:
        {
            os << "RAlt";
            break;
        }
        case sf::Keyboard::RSystem:
        {
            os << "RSystem";
            break;
        }
        case sf::Keyboard::Menu:
        {
            os << "Menu";
            break;
        }
        case sf::Keyboard::LBracket:
        {
            os << "LBracket";
            break;
        }
        case sf::Keyboard::RBracket:
        {
            os << "RBracket";
            break;
        }
        case sf::Keyboard::SemiColon:
        {
            os << "SemiColon";
            break;
        }
        case sf::Keyboard::Comma:
        {
            os << "Comma";
            break;
        }
        case sf::Keyboard::Period:
        {
            os << "Period";
            break;
        }
        case sf::Keyboard::Quote:
        {
            os << "Quote";
            break;
        }
        case sf::Keyboard::Slash:
        {
            os << "Slash";
            break;
        }
        case sf::Keyboard::BackSlash:
        {
            os << "BackSlash";
            break;
        }
        case sf::Keyboard::Tilde:
        {
            os << "Tilde";
            break;
        }
        case sf::Keyboard::Equal:
        {
            os << "Equal";
            break;
        }
        case sf::Keyboard::Dash:
        {
            os << "Dash";
            break;
        }
        case sf::Keyboard::Space:
        {
            os << "Space";
            break;
        }
        case sf::Keyboard::Return:
        {
            os << "Return";
            break;
        }
        case sf::Keyboard::BackSpace:
        {
            os << "BackSpace";
            break;
        }
        case sf::Keyboard::Tab:
        {
            os << "Tab";
            break;
        }
        case sf::Keyboard::PageUp:
        {
            os << "PageUp";
            break;
        }
        case sf::Keyboard::PageDown:
        {
            os << "PageDown";
            break;
        }
        case sf::Keyboard::End:
        {
            os << "End";
            break;
        }
        case sf::Keyboard::Home:
        {
            os << "Home";
            break;
        }
        case sf::Keyboard::Insert:
        {
            os << "Insert";
            break;
        }
        case sf::Keyboard::Delete:
        {
            os << "Delete";
            break;
        }
        case sf::Keyboard::Add:
        {
            os << "Add";
            break;
        }
        case sf::Keyboard::Subtract:
        {
            os << "Subtract";
            break;
        }
        case sf::Keyboard::Multiply:
        {
            os << "Multiply";
            break;
        }
        case sf::Keyboard::Divide:
        {
            os << "Divide";
            break;
        }
        case sf::Keyboard::Left:
        {
            os << "Left";
            break;
        }
        case sf::Keyboard::Right:
        {
            os << "Right";
            break;
        }
        case sf::Keyboard::Up:
        {
            os << "Up";
            break;
        }
        case sf::Keyboard::Down:
        {
            os << "Down";
            break;
        }
        case sf::Keyboard::Numpad0:
        {
            os << "Numpad0";
            break;
        }
        case sf::Keyboard::Numpad1:
        {
            os << "Numpad1";
            break;
        }
        case sf::Keyboard::Numpad2:
        {
            os << "Numpad2";
            break;
        }
        case sf::Keyboard::Numpad3:
        {
            os << "Numpad3";
            break;
        }
        case sf::Keyboard::Numpad4:
        {
            os << "Numpad4";
            break;
        }
        case sf::Keyboard::Numpad5:
        {
            os << "Numpad5";
            break;
        }
        case sf::Keyboard::Numpad6:
        {
            os << "Numpad6";
            break;
        }
        case sf::Keyboard::Numpad7:
        {
            os << "Numpad7";
            break;
        }
        case sf::Keyboard::Numpad8:
        {
            os << "Numpad8";
            break;
        }
        case sf::Keyboard::Numpad9:
        {
            os << "Numpad9";
            break;
        }
        case sf::Keyboard::F1:
        {
            os << "F1";
            break;
        }
        case sf::Keyboard::F2:
        {
            os << "F2";
            break;
        }
        case sf::Keyboard::F3:
        {
            os << "F3";
            break;
        }
        case sf::Keyboard::F4:
        {
            os << "F4";
            break;
        }
        case sf::Keyboard::F5:
        {
            os << "F5";
            break;
        }
        case sf::Keyboard::F6:
        {
            os << "F6";
            break;
        }
        case sf::Keyboard::F7:
        {
            os << "F7";
            break;
        }
        case sf::Keyboard::F8:
        {
            os << "F8";
            break;
        }
        case sf::Keyboard::F9:
        {
            os << "F9";
            break;
        }
        case sf::Keyboard::F10:
        {
            os << "F10";
            break;
        }
        case sf::Keyboard::F11:
        {
            os << "F11";
            break;
        }
        case sf::Keyboard::F12:
        {
            os << "F12";
            break;
        }
        case sf::Keyboard::F13:
        {
            os << "F13";
            break;
        }
        case sf::Keyboard::F14:
        {
            os << "F14";
            break;
        }
        case sf::Keyboard::F15:
        {
            os << "F15";
            break;
        }
        case sf::Keyboard::Pause:
        {
            os << "Pause";
            break;
        }
        case sf::Keyboard::Unknown:
        case sf::Keyboard::KeyCount:
        default:
        {
            os << "UnknownKeyCodeError";
            break;
        }
    }

    return os;
}

} // namespace sf

namespace heroespath
{
namespace sfml_util
{

    // Responsible for enumerating all possible sfml_util::ToString() options
    struct ToStringPrefix : public misc::EnumBaseBitField<ToStringPrefix>
    {
        enum Enum : misc::EnumUnderlying_t
        {
            None = 0,
            Default = None,
            Name = 1 << 0,
            Typename = 1 << 1,
            Namespace = 1 << 2,
            Last = Namespace
        };

        static void ToStringPopulate(
            std::ostringstream & ss,
            const misc::EnumUnderlying_t ENUM_VALUE,
            const std::string & SEPARATOR)
        {
            AppendNameIfBitIsSet(ss, ENUM_VALUE, ToStringPrefix::Name, "Name", SEPARATOR);
            AppendNameIfBitIsSet(ss, ENUM_VALUE, ToStringPrefix::Typename, "Typename", SEPARATOR);
            AppendNameIfBitIsSet(ss, ENUM_VALUE, ToStringPrefix::Namespace, "Namespace", SEPARATOR);
        }
    };

    template <typename T = void>
    const std::string
        ToStringHelper(const ToStringPrefix::Enum OPTIONS, const std::string & OUTER_TYPE_NAME)
    {
        std::ostringstream ss;

        if (OPTIONS & ToStringPrefix::Namespace)
        {
            ss << "sf::";
        }

        if (OPTIONS & ToStringPrefix::Name)
        {
            ss << OUTER_TYPE_NAME;
        }

        if constexpr (std::is_same<T, void>::value == false)
        {
            if (OPTIONS & ToStringPrefix::Typename)
            {
                ss << "<" << boost::typeindex::type_id<T>().pretty_name() << ">";
            }
        }

        return ss.str();
    }

    template <typename T>
    const std::string ToString(
        const sf::Vector2<T> & V, const ToStringPrefix::Enum OPTIONS = ToStringPrefix::Default)
    {
        std::ostringstream ss;
        ss << ToStringHelper<T>(OPTIONS, "Vector2") << V;
        return ss.str();
    }

    template <typename T>
    const std::string ToString(
        const sf::Rect<T> & R, const ToStringPrefix::Enum OPTIONS = ToStringPrefix::Default)
    {
        std::ostringstream ss;
        ss << ToStringHelper<T>(OPTIONS, "Rect") << R;
        return ss.str();
    }

    inline const std::string ToString(
        const sf::VideoMode & VM, const ToStringPrefix::Enum OPTIONS = ToStringPrefix::Default)
    {
        std::ostringstream ss;
        ss << ToStringHelper(OPTIONS, "VideoMode") << VM;
        return ss.str();
    }

    inline const std::string
        ToString(const sf::Color & C, const ToStringPrefix::Enum OPTIONS = ToStringPrefix::Default)
    {
        std::ostringstream ss;
        ss << ToStringHelper(OPTIONS, "Color") << C;
        return ss.str();
    }

    inline const std::string ToString(
        const sf::Keyboard::Key KEY, const ToStringPrefix::Enum OPTIONS = ToStringPrefix::Default)
    {
        std::ostringstream ss;
        ss << ToStringHelper(OPTIONS, "Key") << KEY;
        return ss.str();
    }

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_SFMLUTIL_TO_STRING_HPP_INCLUDED
