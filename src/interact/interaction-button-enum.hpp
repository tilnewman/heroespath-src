// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_INTERACT_BUTTON_ENUM_HPP_INCLUDED
#define HEROESPATH_INTERACT_BUTTON_ENUM_HPP_INCLUDED
//
// interact-button-enum.hpp
//
#include "misc/enum-common.hpp"

#include <SFML/Window/Keyboard.hpp>

#include <vector>

namespace heroespath
{
namespace interact
{

    struct Buttons : public EnumBaseCounting<>
    {
        enum Enum : EnumUnderlying_t
        {
            Ignore = 0,
            Continue,
            Goodbye,
            Yes,
            No,
            Unlock,
            Count
        };

        static constexpr sf::Keyboard::Key Key(const Buttons::Enum BUTTON_TYPE) noexcept
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
                default: return sf::Keyboard::KeyCount;
            }
        }
    };

    using ButtonEnumVec_t = std::vector<Buttons::Enum>;

} // namespace interact
} // namespace heroespath

#endif // HEROESPATH_MAP_INTERACT_ENUM_HPP_INCLUDED
