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
#include "sfml-util/text-button.hpp"

#include <SFML/Window/Keyboard.hpp>

#include <string>
#include <vector>

namespace heroespath
{
namespace interact
{

    struct Buttons
    {
        enum Enum
        {
            Ignore = 0,
            Continue,
            Goodbye,
            Yes,
            No,
            Unlock,
            Count
        };

        static const std::string ToString(const Enum);
        static sf::Keyboard::Key Key(const Enum);
    };

    using ButtonEnumVec_t = std::vector<Buttons::Enum>;

} // namespace interact
} // namespace heroespath

#endif // HEROESPATH_MAP_INTERACT_ENUM_HPP_INCLUDED
