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

#include <boost/tokenizer.hpp>
#include <boost/type_index.hpp>

namespace heroespath
{
namespace misc
{

    const std::string CamelTo(
        const std::string & STRING_ORIG,
        const std::string & SEPARATOR,
        const CaseChange CASE_CHANGES)
    {
        char prevChar { 0 };
        std::string result;

        for (const char CURR_CHAR : STRING_ORIG)
        {
            if (IsAlpha(prevChar) && IsAlpha(CURR_CHAR)
                && (IsUpper(prevChar) != IsUpper(CURR_CHAR)))
            {
                if ((CASE_CHANGES == CaseChange::Both)
                    || ((CASE_CHANGES == CaseChange::LowerToUpper) && IsLower(prevChar))
                    || ((CASE_CHANGES == CaseChange::UpperToLower) && IsUpper(prevChar)))
                {
                    result += SEPARATOR;
                }
            }

            result += CURR_CHAR;
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

    const std::vector<std::string> SplitByChars(const std::string & TO_SPLIT, const SplitHow HOW)
    {
        std::vector<std::string> results;

        if (TO_SPLIT.empty())
        {
            return results;
        }

        if (HOW.separator_chars.empty())
        {
            results.emplace_back(TO_SPLIT);
            return results;
        }

        const auto EMPTY_TOKEN_POLICY { (
            (HOW.will_skip_empty) ? boost::drop_empty_tokens : boost::keep_empty_tokens) };

        using CharSeparator_t = boost::char_separator<char>;
        const CharSeparator_t CHAR_SEPARATOR(HOW.separator_chars.c_str(), "", EMPTY_TOKEN_POLICY);

        const boost::tokenizer<CharSeparator_t> TOKENIZER(TO_SPLIT, CHAR_SEPARATOR);

        for (const auto & PIECE_STR_ORIG : TOKENIZER)
        {
            const auto PIECE_STR_FINAL { (
                (HOW.will_trim_each) ? boost::algorithm::trim_copy(PIECE_STR_ORIG)
                                     : PIECE_STR_ORIG) };

            if ((HOW.will_skip_empty == false) || (PIECE_STR_FINAL.empty() == false))
            {
                results.emplace_back(PIECE_STR_FINAL);
            }
        }

        return results;
    }

} // namespace misc
} // namespace heroespath
