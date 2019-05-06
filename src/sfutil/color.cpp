// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// color.cpp
//
#include "color.hpp"

#include <ostream>
#include <string>

namespace heroespath
{
namespace misc
{
    const std::string ToString(const sf::Color & C)
    {
        std::string str;
        str.reserve(32);

        str += ("(");

        if (sf::Color::Black == C)
        {
            str += "Black";
        }
        else if (sf::Color::White == C)
        {
            str += "White";
        }
        else if (sf::Color::Red == C)
        {
            str += "Red";
        }
        else if (sf::Color::Green == C)
        {
            str += "Green";
        }
        else if (sf::Color::Blue == C)
        {
            str += "Blue";
        }
        else if (sf::Color::Yellow == C)
        {
            str += "Yellow";
        }
        else if (sf::Color::Magenta == C)
        {
            str += "Magenta";
        }
        else if (sf::Color::Cyan == C)
        {
            str += "Cyan";
        }
        else
        {
            str += std::to_string(static_cast<unsigned>(C.r)) + ","
                + std::to_string(static_cast<unsigned>(C.g)) + ","
                + std::to_string(static_cast<unsigned>(C.b));

            if (C.a != 255)
            {
                str += "," + std::to_string(static_cast<unsigned>(C.a));
            }
        }

        str += ")";

        return str;
    }

} // namespace misc
} // namespace heroespath

namespace sf
{

std::ostream & operator<<(std::ostream & os, const sf::Color & C)
{
    os << heroespath::misc::ToString(C);
    return os;
}

} // namespace sf
