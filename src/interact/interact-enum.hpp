// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_INTERACT_INTERACT_ENUM_HPP_INCLUDED
#define HEROESPATH_INTERACT_INTERACT_ENUM_HPP_INCLUDED
//
// interact-enum.hpp
//
#include "misc/enum-common.hpp"

#include <string>

namespace heroespath
{
namespace interact
{

    struct Interact : public EnumBaseCounting<EnumFirstValue::Valid>
    {
        enum Enum : EnumUnderlying_t
        {
            Lock = 0,
            Conversation,
            Count
        };

        static const std::string ToString(const Enum);
        static const std::string ImageKey(const Enum);
    };

} // namespace interact
} // namespace heroespath

#endif // HEROESPATH_INTERACT_INTERACT_ENUM_HPP_INCLUDED
