// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_AVATAR_POSEENUM_HPP_INCLUDED
#define HEROESPATH_AVATAR_POSEENUM_HPP_INCLUDED
//
// pose-enum.hpp
//
#include "misc/enum-util.hpp"

#include <string>

namespace heroespath
{
namespace avatar
{

    struct Pose : public misc::EnumBaseCounting<Pose, misc::EnumFirstValue::Valid>
    {
        enum Enum : misc::EnumUnderlying_t
        {
            Standing = 0,
            Walking,
            GiveTake,
            Fight,
            Cast,
            Blink,
            Dead,
            Count
        };

        static const std::string ToString(const Enum);
    };

} // namespace avatar
} // namespace heroespath

#endif // HEROESPATH_AVATAR_POSEENUM_HPP_INCLUDED
