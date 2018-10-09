// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// key-value-file.cpp
//
#include "key-value-file.hpp"

#include "misc/assertlogandthrow.hpp"
#include "misc/filesystem.hpp"
#include "misc/log-macros.hpp"

#include <algorithm>
#include <fstream>

namespace heroespath
{
namespace misc
{

    KeyValueFile::KeyValueFile(
        const std::string & FILE_PATH,
        const std::string & SEPARATOR,
        const std::string & COMMENT_PREFIX)
        : path_(filesystem::CleanPath(FILE_PATH))
        , separator_(SEPARATOR)
        , commentPrefix_(COMMENT_PREFIX)
        , lines_()
    {
        auto makeErrorString = [&]() {
            return "(path_orig=\"" + FILE_PATH + "\", path_final=\"" + path_ + "\", separator=\""
                + SEPARATOR + "\", comment_prefix=\"" + COMMENT_PREFIX + "\")  ";
        };

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (FILE_PATH.empty() == false), "The path given was empty." + makeErrorString());

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (path_.empty() == false), "The path was empty after cleaning." + makeErrorString());

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (SEPARATOR.empty() == false),
            "The key/valye separator string was empty." + makeErrorString());
    }

    bool KeyValueFile::Exists() const { return misc::filesystem::ExistsAndIsFile(path_); }

    bool KeyValueFile::ContainsKey(const std::string & KEY)
    {
        for (const auto & LINE : lines_)
        {
            if (LINE.key == KEY)
            {
                return true;
            }
        }

        return false;
    }

    bool KeyValueFile::ClearAndLoad()
    {
        std::size_t lineCurrentlyProcessing { 0 };

        try
        {
            if (Exists() == false)
            {
                M_HP_LOG_ERR(
                    "Failed because that path does not exist or is not a regular file.  (path=\""
                    + path_ + "\")");

                return false;
            }

            std::ifstream fileStream;
            fileStream.open(path_, std::ios::in);

            if (IsOpenWithGoodStatus(fileStream) == false)
            {
                M_HP_LOG_ERR(
                    "(path=\"" + path_
                    + "\")  Failed because that file could not be opened for reading.  "
                    + MakeStreamErrorString(fileStream));

                return false;
            }

            Clear();

            std::string line;
            while (getline(fileStream, line))
            {
                boost::algorithm::erase_all(line, "\n");
                boost::algorithm::erase_all(line, "\r");
                boost::algorithm::trim(line);
                lines_.emplace_back(MakeKeyValueLine(line, lineCurrentlyProcessing));
                ++lineCurrentlyProcessing;
                line.clear();
            }

            if (IsOpenWithGoodStatus(fileStream))
            {
                M_HP_LOG_ERR(
                    "(path=\"" + path_ + "\", lineCurrentlyProcessing="
                    << lineCurrentlyProcessing
                    << ") Read error.  " + MakeStreamErrorString(fileStream));

                return false;
            }

            return true;
        }
        catch (const std::exception & EXCEPTION)
        {
            M_HP_LOG_ERR(
                "(path=\"" + path_ + "\", lineCurrentlyProcessing="
                << lineCurrentlyProcessing << ")  Failed because an exception was thrown: \""
                << EXCEPTION.what() << "\"");

            return false;
        }
    }

    bool KeyValueFile::DeleteReCreateSave() const
    {
        std::size_t lineCurrentlyProcessing { 0 };

        try
        {
            std::ofstream fileStream;
            fileStream.open(path_, std::ios::trunc);
            if ((fileStream.good() == false) || (fileStream.is_open() == false))
            {
                M_HP_LOG_ERR(
                    "(path=\"" + path_
                    + "\")  Failed because that file could not be opened for writing.");

                return false;
            }

            while ((lineCurrentlyProcessing < lines_.size()) && fileStream.good()
                   && fileStream.is_open())
            {
                const auto LINE { lines_.at(lineCurrentlyProcessing) };

                if (LINE.IsBlank())
                {
                    fileStream << std::endl;
                }
                else if (LINE.IsComment())
                {
                    fileStream << LINE.key << std::endl;
                }
                else if (LINE.IsKeyValue())
                {
                    fileStream << LINE.key << separator_ << LINE.value << std::endl;
                }

                ++lineCurrentlyProcessing;
            }

            if ((fileStream.good() == false) || (fileStream.is_open() == false))
            {
                M_HP_LOG_ERR("(path=\"" + path_ + "\")  Write error.");

                return false;
            }

            return true;
        }
        catch (const std::exception & EXCEPTION)
        {
            M_HP_LOG_ERR(
                "(path=\"" + path_ + "\", lineCurrentlyProcessing="
                << lineCurrentlyProcessing << ")  Failed because an exception was thrown: \""
                << EXCEPTION.what() << "\"");

            return false;
        }
    }

    const std::string
        KeyValueFile::Value(const std::string & KEY, const std::string & RETURN_IF_NOT_FOUND) const
    {
        if (KEY.empty())
        {
            return "";
        }

        for (const auto & LINE : lines_)
        {
            if (LINE.IsKeyValue() && (LINE.key == KEY))
            {
                return LINE.value;
            }
        }

        return RETURN_IF_NOT_FOUND;
    }

    bool KeyValueFile::Set(const std::string & KEY, const std::string & VALUE)
    {
        if (KEY.empty() || VALUE.empty())
        {
            return false;
        }

        auto wasSet { false };
        for (auto & line : lines_)
        {
            if (line.IsKeyValue() && (KEY == line.key))
            {
                line.value = VALUE;
                wasSet = true;
            }
        }

        return wasSet;
    }

    bool KeyValueFile::Append(const std::string & KEY, const std::string & VALUE)
    {
        if (KEY.empty() || VALUE.empty())
        {
            return false;
        }

        for (auto & line : lines_)
        {
            if (line.IsKeyValue() && (KEY == line.key))
            {
                return false;
            }
        }

        lines_.emplace_back(KeyValueLine(KEY, VALUE));
        return true;
    }

    bool KeyValueFile::SetOrAppend(const std::string & KEY, const std::string & VALUE)
    {
        if (Set(KEY, VALUE))
        {
            return true;
        }

        return Append(KEY, VALUE);
    }

    bool KeyValueFile::AppendComment(const std::string & COMMENT)
    {
        if (commentPrefix_.empty())
        {
            return false;
        }

        auto commentFinal { boost::algorithm::trim_copy(COMMENT) };

        if (boost::algorithm::starts_with(commentFinal, commentPrefix_) == false)
        {
            commentFinal = commentPrefix_ + commentFinal;
        }

        lines_.emplace_back(KeyValueLine(commentFinal));
        return true;
    }

    const std::vector<std::string> KeyValueFile::FindAllKeysWithPrefix(
        const std::string & PREFIX, const bool IS_CASE_SENSITIVE) const
    {
        std::vector<std::string> keys;

        if (PREFIX.empty())
        {
            return keys;
        }

        for (const auto & LINE : lines_)
        {
            if (LINE.IsKeyValue() == false)
            {
                continue;
            }

            if (IS_CASE_SENSITIVE)
            {
                if (boost::algorithm::starts_with(LINE.key, PREFIX))
                {
                    keys.emplace_back(LINE.key);
                }
            }
            else
            {
                if (boost::algorithm::istarts_with(LINE.key, PREFIX))
                {
                    keys.emplace_back(LINE.key);
                }
            }
        }

        return keys;
    }

    const std::vector<std::string> KeyValueFile::FindAllKeysWithPostfix(
        const std::string & POSTFIX, const bool IS_CASE_SENSITIVE) const
    {
        std::vector<std::string> keys;

        if (POSTFIX.empty())
        {
            return keys;
        }

        for (const auto & LINE : lines_)
        {
            if (LINE.IsKeyValue() == false)
            {
                continue;
            }

            if (IS_CASE_SENSITIVE)
            {
                if (boost::algorithm::ends_with(LINE.key, POSTFIX))
                {
                    keys.emplace_back(LINE.key);
                }
            }
            else
            {
                if (boost::algorithm::iends_with(LINE.key, POSTFIX))
                {
                    keys.emplace_back(LINE.key);
                }
            }
        }

        return keys;
    }

    const std::vector<std::string> KeyValueFile::FindAllKeysWith(
        const std::string & SEARCH_FOR, const bool IS_CASE_SENSITIVE) const
    {
        std::vector<std::string> keys;

        if (SEARCH_FOR.empty())
        {
            return keys;
        }

        for (const auto & LINE : lines_)
        {
            if (LINE.IsKeyValue() == false)
            {
                continue;
            }

            if (IS_CASE_SENSITIVE)
            {
                if (boost::algorithm::contains(LINE.key, SEARCH_FOR))
                {
                    keys.emplace_back(LINE.key);
                }
            }
            else
            {
                if (boost::algorithm::icontains(LINE.key, SEARCH_FOR))
                {
                    keys.emplace_back(LINE.key);
                }
            }
        }

        return keys;
    }

    const KeyValueLine KeyValueFile::MakeKeyValueLine(
        const std::string & LINE_STR, const std::size_t LINE_NUMBER) const
    {
        if (LINE_STR.empty())
        {
            return KeyValueLine();
        }

        if ((commentPrefix_.empty() == false)
            && (boost::algorithm::starts_with(LINE_STR, commentPrefix_)))
        {
            return KeyValueLine(LINE_STR);
        }

        auto makeErrorMessage = [&](const std::string & MESSAGE) {
            return "(path=\"" + path_ + "\", line=\"" + LINE_STR
                + "\", line_number=" + misc::ToString(LINE_NUMBER) + ")  " + MESSAGE;
        };

        auto returnError = [&](const std::string & MESSAGE) {
            const std::string ERROR_MESSAGE_BASE { makeErrorMessage(MESSAGE) };

            if (commentPrefix_.empty())
            {
                M_HP_LOG_ERR(
                    ERROR_MESSAGE_BASE
                    + "  This line will not be loaded and a blank line will take its place.");

                return KeyValueLine();
            }
            else
            {
                const auto ERROR_MESSAGE_FINAL {
                    ERROR_MESSAGE_BASE
                    + "  This line will be loaded as a comment containing this error message."
                };

                M_HP_LOG_ERR(ERROR_MESSAGE_FINAL);

                return KeyValueLine(commentPrefix_ + ERROR_MESSAGE_FINAL);
            }
        };

        const std::size_t SEPARATOR_POSITION { LINE_STR.find(separator_) };

        if (SEPARATOR_POSITION == std::string::npos)
        {
            return returnError(
                "That line does not contain the key/value separator \"" + separator_ + "\".");
        }

        if (SEPARATOR_POSITION == 0)
        {
            return returnError(
                "That line starts with the key/value separator \"" + separator_ + "\".");
        }

        const auto KEY { boost::trim_copy(LINE_STR.substr(0, SEPARATOR_POSITION)) };

        if (KEY.empty())
        {
            return returnError("That line starts with an empty key.");
        }

        const std::size_t VALUE_BEGIN_POSITION(SEPARATOR_POSITION + separator_.size());

        if (VALUE_BEGIN_POSITION >= LINE_STR.size())
        {
            return returnError(
                "That line starts with the key \"" + KEY + "\" but there is nothing after it.");
        }

        const auto VALUE { boost::trim_copy(LINE_STR.substr(VALUE_BEGIN_POSITION)) };

        if (VALUE.empty())
        {
            return returnError(
                "That line starts with the key \"" + KEY + "\" but the value was empty.");
        }

        return KeyValueLine(KEY, VALUE);
    }

    const std::string KeyValueFile::MakeStreamErrorString(const std::ios & STREAM) const
    {
        std::string errorMessage;

        auto appendFlagToErrorMessageIfSet
            = [&](const bool IS_FLAG_SET, const std::string & FLAG_NAME) {
                  if (IS_FLAG_SET == false)
                  {
                      return;
                  }

                  if (errorMessage.empty() == false)
                  {
                      errorMessage += ", ";
                  }

                  errorMessage += FLAG_NAME;
              };

        appendFlagToErrorMessageIfSet(STREAM.good(), "good=all_good");
        appendFlagToErrorMessageIfSet(STREAM.eof(), "eof=read_past_end=not_an_error)");
        appendFlagToErrorMessageIfSet(STREAM.fail(), "fail=logic_error");
        appendFlagToErrorMessageIfSet(STREAM.bad(), "bad=stream_integrity_or_io_error");

        return "file_stream_status_flags=(" + errorMessage + ")";
    }

} // namespace misc
} // namespace heroespath
