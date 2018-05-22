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
#include <string>

namespace heroespath
{
namespace interact
{

    struct Interact
    {
        enum Enum
        {
            Lock = 0,
            Conversation,
            Count
        };

        static const std::string ToString(const Interact::Enum);
        static const std::string ImageKey(const Interact::Enum);
    };

} // namespace interact
} // namespace heroespath

#endif // HEROESPATH_INTERACT_INTERACT_ENUM_HPP_INCLUDED
