// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_CREATURE_SEXENUM_HPP_INCLUDED
#define HEROESPATH_CREATURE_SEXENUM_HPP_INCLUDED
//
// sex-enum.hpp
//  An enumeration of all sexes.
//
#include "misc/enum-util.hpp"

#include <string>

namespace heroespath
{
namespace creature
{

    struct sex : misc::EnumBaseCounting<sex, misc::EnumFirstValue::Valid>
    {
        enum Enum : misc::EnumUnderlying_t
        {
            Unknown = 0, // some creatures are bizarre and don't have a sex
            Male,
            Female,
            Count
        };

        static const std::string ToString(const sex::Enum);

        static const std::string HeSheIt(const sex::Enum ENUM, const bool WILL_CAPITALIZE);

        static const std::string HisHerIts(
            const sex::Enum ENUM, const bool WILL_CAPITALIZE, const bool WILL_POSSESSIVE_HER);

        static const std::string HimHerIt(const sex::Enum ENUM, const bool WILL_CAPITALIZE);
    };

} // namespace creature
} // namespace heroespath

#endif // HEROESPATH_CREATURE_SEXENUM_HPP_INCLUDED
