// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_MISC_WRAP_ENUM_UTIL_HPP_INCLUDED
#define HEROESPATH_MISC_WRAP_ENUM_UTIL_HPP_INCLUDED
//
// enum-util.hpp
//
//  WARNING:
//      To avoid circular dependancies this file cannot include:
//          "misc/enum-util.hpp"
//          "misc/log-macros.hpp"
//          "misc/assertlogandthrow.hpp"
//          -or any other header that includes these!
//
//      This means that this enum cannot use the enum helper code.
//
namespace heroespath
{
namespace misc
{

    enum class Wrap : bool
    {
        No = false,
        Yes = true
    };

}
} // namespace heroespath

#endif // HEROESPATH_MISC_WRAP_ENUM_UTIL_HPP_INCLUDED
