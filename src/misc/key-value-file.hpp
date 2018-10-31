// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_MISC_KEY_VALUE_FILE_HPP_INCLUDED
#define HEROESPATH_MISC_KEY_VALUE_FILE_HPP_INCLUDED
//
// key-value-file.hpp
//
#include "misc/boost-string-includes.hpp"
#include "misc/log-macros.hpp"
#include "misc/strings.hpp"
#include "misc/type-helpers.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/type_index.hpp>

#include <iostream> //for std::ios
#include <string>
#include <vector>

namespace heroespath
{
namespace misc
{

    // Represents a single line in the KeyValueFile
    // Using this struct allows saving the KeyValueFile without losing spacing and comments.
    // If IsComment(), then key contains the whole line, starting with the commentPrefix_.
    // Keys with empty values are not supported.
    struct KeyValueLine
    {
        // default constructor makes a blank line
        KeyValueLine()
            : key()
            , value()
        {}

        explicit KeyValueLine(const std::string & COMMENT)
            : key(COMMENT)
            , value()
        {}

        explicit KeyValueLine(const std::string & KEY, const std::string & VALUE)
            : key(KEY)
            , value(VALUE)
        {}

        KeyValueLine(const KeyValueLine &) = default;
        KeyValueLine(KeyValueLine &&) = default;
        KeyValueLine & operator=(const KeyValueLine &) = default;
        KeyValueLine & operator=(KeyValueLine &&) = default;

        bool IsBlank() const { return (key.empty() && value.empty()); }
        bool IsComment() const { return (!key.empty() && value.empty()); }
        bool IsKeyValue() const { return (!key.empty() && !value.empty()); }

        std::string key;
        std::string value;
    };

    // Wraps a plain text file of key/value pairs and comments.
    // The file is not loaded on construction, and is not kept open after Load().
    // Does NOT Save() on destruction.
    // Caller is responsible for not mixing keys and separator_ or commentPrefix_.
    // Values can contain separator_ or commentPrefix_.
    // Keys with empty values are not supported.
    class KeyValueFile
    {
    public:
        // throws if FILE_PATH or SEPARATOR is empty
        explicit KeyValueFile(
            const std::string & FILE_PATH = "config.txt",
            const std::string & SEPARATOR = " ",
            const std::string & COMMENT_PREFIX = "#");

        virtual ~KeyValueFile() = default;

        const std::string Path() const { return path_; }

        bool Exists() const;

        const std::string Separator() const { return separator_; }

        const std::string CommentPrefix() const { return commentPrefix_; }

        std::size_t Size() const { return lines_.size(); }

        bool Empty() const { return lines_.empty(); }

        void Clear() { lines_.clear(); }

        // returns false if KEY is empty
        bool ContainsKey(const std::string & KEY);

        // returns false if path_ is invalid or does not exist or failed to open or failed to read,
        // returns true if the file was empty, details are logged if false is returned
        bool ClearAndLoad();

        // returns false if path_ is invalid or failed to open/create or failed to write, returns
        // true if Empty() and an empty file resulted, details are logged if false is returned
        bool DeleteReCreateSave() const;

        // returns an empty string if KEY is empty
        const std::string
            Value(const std::string & KEY, const std::string & RETURN_IF_NOT_FOUND = "") const;

        // returns RETURN_IF_NOT_FOUND if KEY is empty
        template <typename T>
        T ValueAs(
            const std::string & KEY,
            const T RETURN_IF_NOT_FOUND,
            const T RETURN_IF_CONVERSION_ERROR) const
        {
            const auto VALUE_STR { Value(KEY) };

            if (VALUE_STR.empty())
            {
                return RETURN_IF_NOT_FOUND;
            }

            if constexpr (
                std::is_same<std::remove_const_t<T>, bool>::value || std::is_same<T, bool>::value)
            {
                const auto VALUE_STR_CLEANED { boost::algorithm::trim_copy(
                    boost::algorithm::to_lower_copy(VALUE_STR)) };

                if ((VALUE_STR_CLEANED == "yes") || (VALUE_STR_CLEANED == "1")
                    || (VALUE_STR_CLEANED == "true"))
                {
                    return true;
                }

                if ((VALUE_STR_CLEANED == "no") || (VALUE_STR_CLEANED == "0")
                    || (VALUE_STR_CLEANED == "false"))
                {
                    return false;
                }
            }

            // lexical_cast<>() does not work as expeted with one byte arithmetic types, so
            // this block is a work-around for that
            if constexpr (misc::is_number_non_floating_point_v<T> && (sizeof(T) == 1))
            {
                const int NUMBER_MIN_INT { std::numeric_limits<int>::lowest() };
                const int NUMBER_MAX_INT { std::numeric_limits<int>::max() };

                const int NUMBER_MIN_T_AS_INT { static_cast<int>(
                    std::numeric_limits<T>::lowest()) };
                const int NUMBER_MAX_T_AS_INT { static_cast<int>(std::numeric_limits<T>::max()) };

                const int RESULT_AS_INT { ValueAs<int>(KEY, NUMBER_MIN_INT, NUMBER_MAX_INT) };

                if (RESULT_AS_INT == NUMBER_MIN_INT)
                {
                    // key not found error and that has already been logged
                    return RETURN_IF_NOT_FOUND;
                }
                else if (RESULT_AS_INT == NUMBER_MAX_INT)
                {
                    // conversion error and that has already been logged
                    return RETURN_IF_CONVERSION_ERROR;
                }
                else if (
                    (RESULT_AS_INT < NUMBER_MIN_T_AS_INT) || (RESULT_AS_INT > NUMBER_MAX_T_AS_INT))
                {
                    M_HP_LOG_ERR(
                        "For key=\""
                        << KEY << "\" boost::lexical_cast<"
                        << boost::typeindex::type_id<T>().pretty_name() << ">(value=\"" << VALUE_STR
                        << "\") fails because the desired type is a one-byte type with a range "
                           "limited to ["
                        << NUMBER_MIN_T_AS_INT << ", " << NUMBER_MAX_T_AS_INT
                        << "] and the given value of \"" << VALUE_STR << "\" or (" << RESULT_AS_INT
                        << ") is outside of that range.  Returning return_on_conversion_error="
                        << misc::ToString(RETURN_IF_CONVERSION_ERROR));

                    return RETURN_IF_CONVERSION_ERROR;
                }
                else
                {
                    return static_cast<T>(RESULT_AS_INT);
                }
            }

            try
            {
                return boost::lexical_cast<T>(VALUE_STR);
            }
            catch (const std::exception & EXCEPTION)
            {
                M_HP_LOG_ERR(
                    "For key=\"" << KEY << "\" boost::lexical_cast<"
                                 << boost::typeindex::type_id<T>().pretty_name() << ">(value=\""
                                 << VALUE_STR << "\") threw exception \"" << EXCEPTION.what()
                                 << "\".  Returning return_on_conversion_error="
                                 << RETURN_IF_CONVERSION_ERROR << ".  "
                                 << M_HP_VAR_STR(RETURN_IF_NOT_FOUND)
                                 << M_HP_VAR_STR(RETURN_IF_CONVERSION_ERROR));

                return RETURN_IF_CONVERSION_ERROR;
            }
        }

        // returns RETURN_IF_NOT_FOUND if KEY is empty
        template <typename T>
        T ValueAs(const std::string & KEY, const T RETURN_IF_NOT_FOUND) const
        {
            return ValueAs(KEY, RETURN_IF_NOT_FOUND, RETURN_IF_NOT_FOUND);
        }

        template <typename T>
        T ValueOrDefault(const std::string & KEY) const
        {
            return ValueAs(KEY, T());
        }

        // returns true if the KEY was found and it's value set, returns false if KEY or VALUE is
        // empty, returns false if KEY is not found, caller is responsible for avoiding keys
        // containing separator_ or commentPrefix_, will set all matching keys if there are
        // duplicates
        bool Set(const std::string & KEY, const std::string & VALUE);

        // returns false if KEY or VALUE is empty, returns false if KEY is found, caller is
        // responsible for avoiding keys containing separator_ or commentPrefix_
        bool Append(const std::string & KEY, const std::string & VALUE);

        // if Set() fails then returns Append()
        bool SetOrAppend(const std::string & KEY, const std::string & VALUE);

        // returns false if commentPrefix_ is empty, COMMENT can be empty, commentPrefix_ will be
        // prepended if not already
        bool AppendComment(const std::string & COMMENT);

        template <typename T>
        bool SetAs(const std::string & KEY, const T & VALUE)
        {
            return Set(KEY, misc::ToString(VALUE));
        }

        template <typename T>
        bool AppendAs(const std::string & KEY, const T & VALUE)
        {
            return Append(KEY, misc::ToString(VALUE));
        }

        template <typename T>
        bool SetOrAppendAs(const std::string & KEY, const T & VALUE)
        {
            return SetOrAppend(KEY, misc::ToString(VALUE));
        }

        template <typename T>
        bool AppendCommentAs(const std::string & KEY, const T & VALUE)
        {
            return AppendComment(KEY, misc::ToString(VALUE));
        }

        void AppendBlankLine() { lines_.emplace_back(KeyValueLine()); }

        // returns empty if PREFIX is empty
        const std::vector<std::string> FindAllKeysWithPrefix(
            const std::string & PREFIX, const bool IS_CASE_SENSITIVE = true) const;

        // returns empty if PREFIX is empty
        const std::vector<std::string> FindAllKeysWithPostfix(
            const std::string & POSTFIX, const bool IS_CASE_SENSITIVE = true) const;

        // returns empty if PREFIX is empty
        const std::vector<std::string> FindAllKeysWith(
            const std::string & SEARCH_FOR, const bool IS_CASE_SENSITIVE = true) const;

    private:
        const KeyValueLine
            MakeKeyValueLine(const std::string & LINE_STR, const std::size_t LINE_NUMBER) const;

        template <typename T>
        bool IsOpenWithGoodStatus(const T & F_STREAM) const
        {
            return (F_STREAM.good() && F_STREAM.is_open());
        }

        template <typename T>
        bool IsOpenWithGoodStatusAndEOF(const T & F_STREAM) const
        {
            return (IsOpenWithGoodStatus(F_STREAM) && F_STREAM.eof());
        }

        const std::string MakeStreamErrorString(const std::ios &) const;

        std::string path_;
        std::string separator_;
        std::string commentPrefix_;
        std::vector<KeyValueLine> lines_;
    };

} // namespace misc
} // namespace heroespath

#endif // HEROESPATH_MISC_KEY_VALUE_FILE_HPP_INCLUDED
