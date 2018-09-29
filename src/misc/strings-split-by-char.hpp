// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_MISC_STRINGS_SPLIT_BY_CHAR_HPP_INCLUDED
#define HEROESPATH_MISC_STRINGS_SPLIT_BY_CHAR_HPP_INCLUDED
//
// strings-split-by-char.hpp
//
#include <boost/tokenizer.hpp>
#include <boost/type_index.hpp>

#include <string>

namespace heroespath
{
namespace misc
{

    // (See comments for the function below)  This version splits on any chars found in SEP_CHARS
    // and NOT on the entire string as a whole.  If SEP_CHARS.empty() then INPUT_STR is appended to
    // outContainer and 1 is returned.
    template <typename T>
    std::size_t SplitByChars(
        const std::string & INPUT_STR,
        T & outContainer,
        const std::string & SEP_CHARS,
        const bool WILL_SKIP_EMPTY = false,
        const bool WILL_TRIM = false)
    {
        if (INPUT_STR.empty())
        {
            return 0;
        }

        const auto EMPTY_TOKEN_POLICY { (
            (WILL_SKIP_EMPTY) ? boost::drop_empty_tokens : boost::keep_empty_tokens) };

        using CharSeparator_t = boost::char_separator<char>;
        const CharSeparator_t CHAR_SEPARATOR(SEP_CHARS.c_str(), "", EMPTY_TOKEN_POLICY);

        const boost::tokenizer<CharSeparator_t> TOKENIZER(INPUT_STR, CHAR_SEPARATOR);

        std::size_t appendCount(0);
        for (const auto & PIECE_STR_ORIG : TOKENIZER)
        {
            const auto PIECE_STR { (
                (WILL_TRIM)
                    ? boost::algorithm::trim_copy_if(PIECE_STR_ORIG, boost::algorithm::is_space())
                    : PIECE_STR_ORIG) };

            if ((WILL_SKIP_EMPTY == false) || (PIECE_STR.empty() == false))
            {
                ++appendCount;
                outContainer.insert(outContainer.end(), PIECE_STR);
            }
        }

        return appendCount;
    }

    // INPUT_STR is split into pieces by SEP_CHAR (that do not contain SEP_CHAR) and are then
    // appended to outContainer. Returns the number of strings appended to outContainer and NOT
    // the final size. Trim chars will depend on boost's locale, but typically include:
    // space,tab,carr-ret,newline,v-tab,form-feed.  If trim makes a piece empty then WILL_SKIP_EMPTY
    // determines if it is appended.
    template <typename T>
    std::size_t SplitByChar(
        const std::string & INPUT_STR,
        T & outContainer,
        const char SEP_CHAR,
        const bool WILL_SKIP_EMPTY = false,
        const bool WILL_TRIM = false)
    {
        return SplitByChars(
            INPUT_STR, outContainer, std::string(1, SEP_CHAR), WILL_SKIP_EMPTY, WILL_TRIM);
    }

} // namespace misc
} // namespace heroespath

#endif // HEROESPATH_MISC_STRINGS_SPLIT_BY_CHAR_HPP_INCLUDED
