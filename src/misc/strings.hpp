// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_MISC_STRINGS_HPP_INCLUDED
#define HEROESPATH_MISC_STRINGS_HPP_INCLUDED
//
// strings.hpp
//
#include "misc/to-string-prefix-enum.hpp"

#include <boost/type_index.hpp>

#include <sstream>
#include <string>

namespace heroespath
{
namespace misc
{

    template <typename T = void>
    const std::string MakeToStringPrefix(
        const ToStringPrefix::Enum OPTIONS,
        const std::string & CONTAINER_NAME,
        const std::string & NAMESPACE_PREFIX_STR = "")
    {
        std::ostringstream ss;

        if (OPTIONS & ToStringPrefix::Namespace)
        {
            std::string namespacePrefixToUse { NAMESPACE_PREFIX_STR };

            if ((NAMESPACE_PREFIX_STR.size() > 2)
                && (NAMESPACE_PREFIX_STR[NAMESPACE_PREFIX_STR.size() - 1] != ':')
                && (NAMESPACE_PREFIX_STR[NAMESPACE_PREFIX_STR.size() - 2] != ':'))
            {
                namespacePrefixToUse += "::";
            }

            ss << namespacePrefixToUse;
        }

        if (OPTIONS & ToStringPrefix::SimpleName)
        {
            ss << CONTAINER_NAME;
        }

        if constexpr (std::is_same<T, void>::value == false)
        {
            if (OPTIONS & ToStringPrefix::Typename)
            {
                ss << "<" << boost::typeindex::type_id<T>().pretty_name() << ">";
            }
        }

        return ss.str();
    }

    template <typename T>
    const std::string NumberToStringWithOrdinalSuffix(const T NUMBER)
    {
        std::ostringstream ss;
        ss << NUMBER;

        if (NUMBER == 1)
        {
            ss << "st";
        }
        else if (NUMBER == 2)
        {
            ss << "nd";
        }
        else if (NUMBER == 3)
        {
            ss << "rd";
        }
        else
        {
            ss << "th";
        }

        return ss.str();
    }

    const std::string MakeLoggableString(const std::string &);

} // namespace misc
} // namespace heroespath

#endif // HEROESPATH_MISC_STRINGS_HPP_INCLUDED
