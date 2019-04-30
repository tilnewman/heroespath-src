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

namespace sf
{

std::ostream & operator<<(std::ostream & os, const sf::Color & C)
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

} // namespace sf
