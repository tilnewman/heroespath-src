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

    struct Interact : public EnumBaseCounting<>
    {
        enum Enum : EnumUnderlying_t
        {
            Lock = 0,
            Conversation,
            Count
        };

        static constexpr std::string_view ImageKey(const Enum INTERACTION_TYPE) noexcept
        {
            switch (INTERACTION_TYPE)
            {
                case Lock: return "media-image-misc-lock";
                case Conversation: return "media-image-misc-talk";
                case Count:
                default: return "interact::ImageKey(ENUM=out_of_bounds)";
            }
        }
    };

} // namespace interact
} // namespace heroespath

#endif // HEROESPATH_INTERACT_INTERACT_ENUM_HPP_INCLUDED
