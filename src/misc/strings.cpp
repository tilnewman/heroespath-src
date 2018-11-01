// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// strings.cpp
//
#include "strings.hpp"

#include "misc/boost-string-includes.hpp"

namespace heroespath
{
namespace misc
{

    const std::string CamelTo(const std::string & STRING_ORIG, const std::string & SEPARATOR)
    {
        char prevChar { 0 };
        std::string result;

        for (const char CURR_CHAR : STRING_ORIG)
        {
            if (misc::IsAlpha(prevChar) && misc::IsAlpha(CURR_CHAR) && IsUpper(prevChar)
                && IsLower(CURR_CHAR))
            {
                result += SEPARATOR;
            }

            result += ToLowerCopy(CURR_CHAR);
            prevChar = CURR_CHAR;
        }

        return result;
    }

    const std::string MakeLoggableString(const std::string & ORIG_STR)
    {
        std::string newString(ORIG_STR);

        boost::algorithm::replace_all(newString, "\n", "\\n");

        const std::size_t MAX_SIZE { 40 };
        if (newString.size() > MAX_SIZE)
        {
            newString.resize(MAX_SIZE - 3);
            newString += "...";
        }

        return newString;
    }

    bool ContainsAnyOf(
        const std::string & STRING_TO_SEARCH,
        const std::vector<std::string> & STRINGS_TO_FIND,
        const bool IS_CASE_SENSITIVE)
    {
        if (STRING_TO_SEARCH.empty() || STRINGS_TO_FIND.empty())
        {
            return false;
        }

        namespace ba = boost::algorithm;

        for (const auto & STRING_TO_FIND : STRINGS_TO_FIND)
        {
            if (STRING_TO_FIND.empty() || (STRING_TO_FIND.size() > STRING_TO_SEARCH.size()))
            {
                continue;
            }

            if (IS_CASE_SENSITIVE)
            {
                if (ba::contains(STRING_TO_SEARCH, STRING_TO_FIND))
                {
                    return true;
                }
            }
            else
            {
                if (ba::icontains(STRING_TO_SEARCH, STRING_TO_FIND))
                {
                    return true;
                }
            }
        }

        return false;
    }

} // namespace misc
} // namespace heroespath
