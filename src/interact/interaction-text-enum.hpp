// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_MAP_INTERACTION_TEXT_ENUM_HPP_INCLUDED
#define HEROESPATH_MAP_INTERACTION_TEXT_ENUM_HPP_INCLUDED
//
// interaction-text-enum.hpp
//
#include "misc/not-null.hpp"
#include <string>

namespace sf
{
class Font;
}

namespace heroespath
{
using FontPtr_t = misc::NotNull<sf::Font *>;

namespace interact
{

    struct Text
    {
        enum Enum
        {
            System = 0,
            Dialog,
            Count
        };

        static const std::string ToString(const Enum);
        static const FontPtr_t Font(const Enum);
    };

} // namespace interact
} // namespace heroespath

#endif // HEROESPATH_MAP_INTERACTION_TEXT_ENUM_HPP_INCLUDED
