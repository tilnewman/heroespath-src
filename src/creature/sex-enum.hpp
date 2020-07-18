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

    struct sex : public EnumBaseCounting<>
    {
        enum Enum : EnumUnderlying_t
        {
            Unknown = 0, // some creatures are bizarre and don't have a sex
            Male,
            Female,
            Count
        };

        static constexpr std::string_view
            HeSheIt(const sex::Enum SEX_TYPE, const bool WILL_CAPITALIZE) noexcept
        {
            switch (SEX_TYPE)
            {
                case Unknown: return ((WILL_CAPITALIZE) ? "It" : "it");
                case Male: return ((WILL_CAPITALIZE) ? "He" : "he");
                case Female: return ((WILL_CAPITALIZE) ? "She" : "she");
                case Count: return "Count";
                default: return "creature::sex::HeSheIt(ENUM=out_of_bounds)";
            }
        }

        static constexpr std::string_view
            HimHerIt(const sex::Enum SEX_TYPE, const bool WILL_CAPITALIZE) noexcept
        {
            switch (SEX_TYPE)
            {
                case Unknown: return ((WILL_CAPITALIZE) ? "It" : "it");
                case Male: return ((WILL_CAPITALIZE) ? "Him" : "him");
                case Female: return ((WILL_CAPITALIZE) ? "Her" : "her");
                case Count: return "Count";
                default: return "creature::sex::HimHerIt(ENUM=out_of_bounds)";
            }
        }

        static constexpr std::string_view HisHerIts(
            const sex::Enum SEX_TYPE,
            const bool WILL_CAPITALIZE,
            const bool WILL_POSSESSIVE_HER) noexcept
        {
            switch (SEX_TYPE)
            {
                case Unknown: return ((WILL_CAPITALIZE) ? "Its" : "its");
                case Male: return ((WILL_CAPITALIZE) ? "His" : "his");

                case Female:
                    return (
                        (WILL_CAPITALIZE) ? ((WILL_POSSESSIVE_HER) ? "Hers" : "Her")
                                          : ((WILL_POSSESSIVE_HER) ? "hers" : "her"));

                case Count: return "Count";
                default: return "creature::sex::HisHerIts(ENUM=out_of_bounds)";
            }
        }
    };

} // namespace creature
} // namespace heroespath

#endif // HEROESPATH_CREATURE_SEXENUM_HPP_INCLUDED
