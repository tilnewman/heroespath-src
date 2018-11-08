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
#include "misc/enum-common.hpp"

#include <string>

namespace heroespath
{
namespace creature
{

    struct sex : public EnumBaseCounting<EnumFirstValue::Valid>
    {
        enum Enum : EnumUnderlying_t
        {
            Unknown = 0, // some creatures are bizarre and don't have a sex
            Male,
            Female,
            Count
        };

        static const std::string ToString(const Enum);
        static const std::string Name(const Enum ENUM) { return ToString(ENUM); }
        static const std::string HeSheIt(const Enum ENUM, const bool WILL_CAPITALIZE);
        static const std::string HimHerIt(const Enum ENUM, const bool WILL_CAPITALIZE);

        static const std::string
            HisHerIts(const Enum ENUM, const bool WILL_CAPITALIZE, const bool WILL_POSSESSIVE_HER);
    };

} // namespace creature
} // namespace heroespath

#endif // HEROESPATH_CREATURE_SEXENUM_HPP_INCLUDED
