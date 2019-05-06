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
#include "misc/log-macros.hpp"
#include "misc/nameof.hpp"
#include "misc/strings.hpp"
#include "misc/strong-numeric-type.hpp"
#include "misc/type-helpers.hpp"

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

            if constexpr (are_same_v<T, bool>)
            {
                const auto VALUE_STR_CLEANED { TrimWhitespaceCopy(ToLowerCopy(VALUE_STR)) };

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

                M_HP_LOG_ERR("With T=bool, key=\"" << KEY << "\", failed to parse as bool.");

                return RETURN_IF_CONVERSION_ERROR;
            }
            else
            {
                T result(RETURN_IF_CONVERSION_ERROR);
                if (misc::ToNumber(VALUE_STR, result))
                {
                    return result;
                }
                else
                {
                    M_HP_LOG_ERR(
                        "With T=" << NAMEOF_TYPE_T(T) << ", key=\"" << KEY
                                  << "\", failed to parse as that type.");

                    return RETURN_IF_CONVERSION_ERROR;
                }
            }
        }

        // returns RETURN_IF_NOT_FOUND if KEY is empty
        template <typename T>
        T ValueAs(const std::string & KEY, const T RETURN_IF_NOT_FOUND) const
        {
            return ValueAs(KEY, RETURN_IF_NOT_FOUND, RETURN_IF_NOT_FOUND);
        }

        // returns RETURN_IF_NOT_FOUND if KEY is empty
        template <typename V, typename P>
        const StrongNumericType<V, P> ValueAs(
            const std::string & KEY, const StrongNumericType<V, P> & RETURN_IF_NOT_FOUND) const
        {
            return StrongNumericType<V, P>::Make(
                ValueAs(KEY, RETURN_IF_NOT_FOUND.Get(), RETURN_IF_NOT_FOUND.Get()));
        }

        template <typename T>
        T ValueOrDefault(const std::string & KEY) const
        {
            return ValueAs(KEY, T());
        }

        template <typename S, typename V, typename P>
        std::enable_if_t<are_same_v<S, StrongNumericType<V, P>>, const StrongNumericType<V, P>>
            ValueAs(const std::string & KEY) const
        {
            return ValueAs(KEY, S());
        }

        // returns true if the KEY was found and it's value set, returns false if KEY or VALUE
        // is empty, returns false if KEY is not found, caller is responsible for avoiding keys
        // containing separator_ or commentPrefix_, will set all matching keys if there are
        // duplicates
        bool Set(const std::string & KEY, const std::string & VALUE);

        // returns false if KEY or VALUE is empty, returns false if KEY is found, caller is
        // responsible for avoiding keys containing separator_ or commentPrefix_
        bool Append(const std::string & KEY, const std::string & VALUE);

        // if Set() fails then returns Append()
        bool SetOrAppend(const std::string & KEY, const std::string & VALUE);

        // returns false if commentPrefix_ is empty, COMMENT can be empty, commentPrefix_ will
        // be prepended if not already
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
