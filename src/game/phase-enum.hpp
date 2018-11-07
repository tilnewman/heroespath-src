// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GAME_PHASEENUM_HPP_INCLUDED
#define HEROESPATH_GAME_PHASEENUM_HPP_INCLUDED
//
// phase-enum.hpp
//
#include "misc/enum-util.hpp"

#include <string>

namespace heroespath
{
namespace game
{

    struct Phase : misc::EnumBaseBitField<Phase>
    {
        enum Enum : misc::EnumUnderlying_t
        {
            None = 0,
            Combat = 1 << 0,
            Exploring = 1 << 1,
            Conversation = 1 << 2,
            Quest = 1 << 3,
            Inventory = 1 << 4,
            Last = Inventory
        };

        static const std::string ToStringPopulate(
            const misc::EnumUnderlying_t ENUM_VALUE, const std::string & SEPARATOR);
    };

} // namespace game
} // namespace heroespath

#endif // HEROESPATH_GAME_PHASEENUM_HPP_INCLUDED
