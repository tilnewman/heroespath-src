// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// interaction-button-enum.cpp
//
#include "interaction-button-enum.hpp"

#include "misc/log-macros.hpp"

namespace heroespath
{
namespace interact
{

    const std::string Buttons::ToString(const Buttons::Enum BUTTON_TYPE)
    {
        switch (BUTTON_TYPE)
        {
            case Ignore: return "Ignore";
            case Continue: return "Continue";
            case Goodbye: return "Goodbye";
            case Yes: return "Yes";
            case No: return "No";
            case Unlock: return "Unlock";
            case Count:
            {
                return "(Count)";
            }
            default:
            {
                M_HP_LOG_ERR(
                    "enum_value=" << static_cast<EnumUnderlying_t>(BUTTON_TYPE)
                                  << " is invalid. (count=" << static_cast<EnumUnderlying_t>(Count)
                                  << ")");

                return "";
            }
        }
    }

    sf::Keyboard::Key Buttons::Key(const Buttons::Enum BUTTON_TYPE)
    {
        switch (BUTTON_TYPE)
        {
            case Ignore: return sf::Keyboard::I;
            case Continue: return sf::Keyboard::C;
            case Goodbye: return sf::Keyboard::G;
            case Yes: return sf::Keyboard::Y;
            case No: return sf::Keyboard::N;
            case Unlock: return sf::Keyboard::U;
            case Count:
            default:
            {
                M_HP_LOG_ERR(
                    "enum_value=" << static_cast<EnumUnderlying_t>(BUTTON_TYPE)
                                  << " is invalid. (count=" << static_cast<EnumUnderlying_t>(Count)
                                  << ")");

                return sf::Keyboard::KeyCount;
            }
        }
    }
} // namespace interact
} // namespace heroespath
