// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_MISC_TO_STRING_PREFIX_ENUM_HPP_INCLUDED
#define HEROESPATH_MISC_TO_STRING_PREFIX_ENUM_HPP_INCLUDED
//
// to-string-prefix-enum.hpp
//
#include "misc/enum-util.hpp"

namespace heroespath
{
namespace misc
{

    // Responsible for enumerating all possible sfml_util::ToString() options
    struct ToStringPrefix : public misc::EnumBaseBitField<ToStringPrefix>
    {
        enum Enum : misc::EnumUnderlying_t
        {
            None = 0,
            Default = None,
            SimpleName = 1 << 0,
            Typename = 1 << 1,
            Namespace = 1 << 2,
            Last = Namespace
        };

        static void ToStringPopulate(
            std::ostringstream & ss,
            const misc::EnumUnderlying_t ENUM_VALUE,
            const std::string & SEPARATOR)
        {
            AppendNameIfBitIsSet(
                ss, ENUM_VALUE, ToStringPrefix::SimpleName, "SimpleName", SEPARATOR);

            AppendNameIfBitIsSet(ss, ENUM_VALUE, ToStringPrefix::Typename, "Typename", SEPARATOR);
            AppendNameIfBitIsSet(ss, ENUM_VALUE, ToStringPrefix::Namespace, "Namespace", SEPARATOR);
        }
    };

} // namespace misc
} // namespace heroespath

#endif // HEROESPATH_MISC_TO_STRING_PREFIX_ENUM_HPP_INCLUDED
