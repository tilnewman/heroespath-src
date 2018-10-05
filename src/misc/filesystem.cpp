// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// filesystem.cpp
//
#include "filesystem.hpp"

#include "misc/assertlogandthrow.hpp"
#include "misc/boost-string-includes.hpp"
#include "misc/platform.hpp"
#include "misc/strings.hpp"
#include "misc/vector-map.hpp"

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

#include <algorithm>
#include <cctype>
#include <sstream>

namespace heroespath
{
namespace misc
{

    namespace bfs = boost::filesystem;

    const std::vector<std::string> filesystem::COMMON_FILE_NAME_PARTS_TO_EXCLUDE_VEC_ {
        ".txt", ".DS_Store", "sample.gif", ".html", ".htm"
    };

    // this value found by experiment to be a good high guess for the game
    const std::size_t filesystem::VECTOR_RESERVE_SIZE { 30 };

    const std::string filesystem::CleanPath(const std::string & PATH_STR_ORIG)
    {
        std::string pathStrToUse { boost::algorithm::trim_copy(PATH_STR_ORIG) };

        if (pathStrToUse.empty())
        {
            return "";
        }

        // the boost::filesystem functions below will throw on Windows if given "c:" or "prn:"
        // without a trailing slash, so add one in that case
        if (IsWindowsDriveNameWithoutTrailingSlash(pathStrToUse))
        {
            pathStrToUse += PreferredSeparatorChar();
        }

        bfs::path boostPath(pathStrToUse);

        try
        {
            boostPath = bfs::canonical(boostPath);
        }
        catch (...)
        {
            boostPath = bfs::path(pathStrToUse);

            try
            {
                boostPath = bfs::weakly_canonical(boostPath);
            }
            catch (...)
            {
                boostPath = bfs::path(pathStrToUse);
            }
        }

        std::string cleanedStr { SlashesToPreferredSeparator(boostPath.make_preferred().string()) };

        if (EndsWithSeparatorFollowedByDots(cleanedStr))
        {
            while ((cleanedStr.empty() == false) && (cleanedStr.back() == '.'))
            {
                cleanedStr.pop_back();
            }
        }

        return cleanedStr;
    }

    const std::string filesystem::CurrentDirectory()
    {
        return CleanPath(bfs::current_path().string());
    }

    bool filesystem::EndsWithFileGuess(const std::string & PATH_STR_ORIG)
    {
        const auto PATH_STR_TRIMMED { boost::trim_copy(PATH_STR_ORIG) };

        if (PATH_STR_TRIMMED.empty())
        {
            return false;
        }

        const bfs::path BOOST_PATH_CLEANED { CleanPath(PATH_STR_TRIMMED) };

        if (bfs::exists(BOOST_PATH_CLEANED))
        {
            return bfs::is_regular_file(BOOST_PATH_CLEANED);
        }

        return (WhatDoesBoostFilenameIndicatePathEndsWith(PATH_STR_ORIG) == LooksLike::File);
    }

    bool filesystem::EndsWithDirectoryGuess(const std::string & PATH_STR_ORIG)
    {
        const auto PATH_STR_TRIMMED { boost::trim_copy(PATH_STR_ORIG) };

        if (PATH_STR_TRIMMED.empty())
        {
            return false;
        }

        const bfs::path BOOST_PATH_CLEANED { CleanPath(PATH_STR_TRIMMED) };

        if (bfs::exists(BOOST_PATH_CLEANED))
        {
            return bfs::is_directory(BOOST_PATH_CLEANED);
        }

        return (WhatDoesBoostFilenameIndicatePathEndsWith(PATH_STR_ORIG) == LooksLike::Directory);
    }

    const std::string
        filesystem::Filename(const std::string & PATH_STR_ORIG, const bool WILL_EXCLUDE_EXTENSION)
    {
        const auto PATH_STR_TRIMMED { boost::trim_copy(PATH_STR_ORIG) };

        if (PATH_STR_TRIMMED.empty())
        {
            return "";
        }

        if (ExistsAndIsDirectory(PATH_STR_ORIG))
        {
            return "";
        }

        if (WhatDoesBoostFilenameIndicatePathEndsWith(PATH_STR_ORIG) == LooksLike::Directory)
        {
            return "";
        }

        const auto BOOST_FILE_NAME_STR { GetBoostFilename(PATH_STR_ORIG) };

        if (BOOST_FILE_NAME_STR.empty())
        {
            return "";
        }

        if (WILL_EXCLUDE_EXTENSION)
        {
            const auto FIRST_DOT_POSITION { BOOST_FILE_NAME_STR.find('.') };
            return BOOST_FILE_NAME_STR.substr(0, FIRST_DOT_POSITION);
        }
        else
        {
            return BOOST_FILE_NAME_STR;
        }
    }

    const std::string filesystem::Extension(
        const std::string & PATH_STR_ORIG, const bool WILL_EXCLUDE_LEADING_DOT)
    {
        const auto FILE_NAME_STR { Filename(PATH_STR_ORIG) };

        if (FILE_NAME_STR.empty())
        {
            return "";
        }

        const auto FIRST_DOT_POSITION_INDEX { FILE_NAME_STR.find('.') };

        if (FIRST_DOT_POSITION_INDEX == std::string::npos)
        {
            return "";
        }

        const auto EXTENSION { FILE_NAME_STR.substr(FIRST_DOT_POSITION_INDEX) };

        if (WILL_EXCLUDE_LEADING_DOT)
        {
            if (EXTENSION.size() == 1)
            {
                return "";
            }
            else
            {
                return EXTENSION.substr(1);
            }
        }
        else
        {
            return EXTENSION;
        }
    }

    bool filesystem::Exists(const std::string & PATH_STR)
    {
        return bfs::exists(bfs::path(CleanPath(PATH_STR)));
    }

    bool filesystem::ExistsAndIsFile(const std::string & FILE_PATH_STR)
    {
        return (Exists(FILE_PATH_STR) && bfs::is_regular_file(bfs::path(CleanPath(FILE_PATH_STR))));
    }

    bool filesystem::ExistsAndIsDirectory(const std::string & DIR_PATH_STR)
    {
        return (Exists(DIR_PATH_STR) && bfs::is_directory(bfs::path(CleanPath(DIR_PATH_STR))));
    }

    const std::vector<std::string> filesystem::FindFiles(
        const bool IS_RECURSIVE,
        const std::string & DIR_PATH_STR_ORIG,
        const std::string & FILENAME_PREFIX,
        const std::string & FILENAME_EXTENSION,
        const std::vector<std::string> & STRING_MATCHES_TO_EXCLUDE)
    {
        const auto DIR_PATH_STR_CLEANED { CleanPath(DIR_PATH_STR_ORIG) };

        if (DIR_PATH_STR_CLEANED.empty())
        {
            return {};
        }

        const bfs::path BOOST_PATH { DIR_PATH_STR_CLEANED };

        if ((bfs::exists(BOOST_PATH) == false) || (bfs::is_directory(BOOST_PATH) == false))
        {
            return {};
        }

        std::vector<std::string> filePathStrings;
        filePathStrings.reserve(VECTOR_RESERVE_SIZE);

        for (auto const & DIR_ENTRY : bfs::directory_iterator(BOOST_PATH))
        {
            auto const ENTRY_PATH { DIR_ENTRY.path() };

            if (IS_RECURSIVE && bfs::is_directory(ENTRY_PATH))
            {
                for (const std::string & SUB_DIR_FILE_PATH_STR : FindFiles(
                         true,
                         ENTRY_PATH.string(),
                         FILENAME_PREFIX,
                         FILENAME_EXTENSION,
                         STRING_MATCHES_TO_EXCLUDE))
                {
                    filePathStrings.emplace_back(SUB_DIR_FILE_PATH_STR);
                }
            }

            if (bfs::is_regular_file(ENTRY_PATH) == false)
            {
                continue;
            }

            const auto FILE_NAME_STR { Filename(ENTRY_PATH.string()) };

            namespace ba = boost::algorithm;

            if ((FILENAME_PREFIX.empty() == false)
                && (ba::starts_with(FILE_NAME_STR, FILENAME_PREFIX) == false))
            {
                continue;
            }

            if ((FILENAME_EXTENSION.empty() == false)
                && (ba::ends_with(FILE_NAME_STR, FILENAME_EXTENSION) == false))
            {
                continue;
            }

            if (misc::ContainsAnyOf(FILE_NAME_STR, STRING_MATCHES_TO_EXCLUDE, false))
            {
                continue;
            }

            filePathStrings.emplace_back(CleanPath(ENTRY_PATH.string()));
        }

        return filePathStrings;
    }

    const std::string filesystem::FindFirstAvailableNumberedFilenamePath(
        const std::string & DIR_PATH_STR_ORIG,
        const std::string & FILE_NAME,
        const std::string & FILE_EXT,
        const std::string & NUMBER_SEPARATOR)
    {
        const auto DIR_PATH_STR_CLEANED { CleanPath(DIR_PATH_STR_ORIG) };

        if (DIR_PATH_STR_CLEANED.empty())
        {
            return "";
        }

        const bfs::path BOOST_DIR_PATH { DIR_PATH_STR_CLEANED };

        if ((bfs::exists(BOOST_DIR_PATH) == false) || (bfs::is_directory(BOOST_DIR_PATH) == false))
        {
            return "";
        }

        bfs::path filePath;

        const std::size_t MAX_ATTEMPT_COUNT { 10000000 };
        for (std::size_t i(1); i <= MAX_ATTEMPT_COUNT; ++i)
        {
            const auto NEXT_NUMBERED_FILE_NAME_STR { MakeNumberedFilename(
                FILE_NAME, NUMBER_SEPARATOR, i, FILE_EXT) };

            filePath = (BOOST_DIR_PATH / NEXT_NUMBERED_FILE_NAME_STR);

            if (bfs::exists(filePath))
            {
                filePath.clear();
            }
            else
            {
                break;
            }
        }

        if (filePath.empty())
        {
            const auto ZERO_NUMBERED_FILE_NAME_STR { MakeNumberedFilename(
                FILE_NAME, NUMBER_SEPARATOR, 0, FILE_EXT) };

            M_HP_LOG_ERR(
                "misc::filesystem::FindFirstAvailableNumberedFilenamePath(dir_path_orig="
                << DIR_PATH_STR_ORIG << ", dir_path_cleaned=\"" << DIR_PATH_STR_CLEANED
                << "\", file_name=" << FILE_NAME << ", file_ext=" << FILE_EXT
                << ", number_prefix=\"" << NUMBER_SEPARATOR
                << "\") failed to find an available numbered filename (one that looks like this \""
                << FILE_NAME << NUMBER_SEPARATOR << "#" << FILE_EXT << "\") after trying "
                << MAX_ATTEMPT_COUNT
                << " times.  So either there really are that many files there or something has "
                   "gone wrong.  Using zero for the number ("
                << ZERO_NUMBERED_FILE_NAME_STR << ") and moving on...");

            filePath = (BOOST_DIR_PATH / ZERO_NUMBERED_FILE_NAME_STR);
        }

        return CleanPath(filePath.string());
    }

    const std::vector<std::string> filesystem::SortByLastNumberInFilename(
        const std::vector<std::string> & PATH_STR_VEC,
        const bool WILL_MISSING_NUMBERS_APPEAR_FIRST,
        const bool WILL_MISSING_NUMBERS_BE_REMOVED)
    {
        if (PATH_STR_VEC.empty())
        {
            return {};
        }

        misc::VectorMap<int, std::string> lastNumberToPathStrMap;
        lastNumberToPathStrMap.Reserve(PATH_STR_VEC.size());

        std::vector<std::string> pathsWithoutNumbers;
        pathsWithoutNumbers.reserve(VECTOR_RESERVE_SIZE);

        for (auto const & PATH_STR_ORIG : PATH_STR_VEC)
        {
            const auto PATH_STR_CLEANED { CleanPath(PATH_STR_ORIG) };

            if (PATH_STR_CLEANED.empty())
            {
                continue;
            }

            const auto LAST_NUMBER_IN_FILE_NAME { misc::FindLastNumber(
                Filename(PATH_STR_CLEANED)) };

            if (LAST_NUMBER_IN_FILE_NAME < 0)
            {
                if (WILL_MISSING_NUMBERS_BE_REMOVED == false)
                {
                    pathsWithoutNumbers.emplace_back(PATH_STR_CLEANED);
                }

                continue;
            }

            // use Append() to allow duplicates
            lastNumberToPathStrMap.Append(LAST_NUMBER_IN_FILE_NAME, PATH_STR_CLEANED);
        }

        std::sort(std::begin(lastNumberToPathStrMap), std::end(lastNumberToPathStrMap));
        std::sort(std::begin(pathsWithoutNumbers), std::end(pathsWithoutNumbers));

        std::vector<std::string> finalVec;
        finalVec.reserve(lastNumberToPathStrMap.Size() + pathsWithoutNumbers.size());

        if (WILL_MISSING_NUMBERS_APPEAR_FIRST)
        {
            std::copy(
                std::cbegin(pathsWithoutNumbers),
                std::cend(pathsWithoutNumbers),
                std::back_inserter(finalVec));
        }

        for (auto const & NUM_STR_PAIR : lastNumberToPathStrMap)
        {
            finalVec.emplace_back(NUM_STR_PAIR.second);
        }

        if (WILL_MISSING_NUMBERS_APPEAR_FIRST == false)
        {
            std::copy(
                std::cbegin(pathsWithoutNumbers),
                std::cend(pathsWithoutNumbers),
                std::back_inserter(finalVec));
        }

        return finalVec;
    }

    const std::string filesystem::CombinePathsThenClean(
        const std::string & PATH1,
        const std::string & PATH2,
        const std::string & PATH3,
        const std::string & PATH4)
    {
        bfs::path path;

        auto addOrAppendPathIfNotEmpty = [&](const std::string & PATH_STR_ORIG) {
            const auto PATH_STR_TRIMMED { boost::algorithm::trim_copy(PATH_STR_ORIG) };

            if (PATH_STR_TRIMMED.empty() || (PATH_STR_TRIMMED == "."))
            {
                return;
            }

            if (path.empty())
            {
                path = bfs::path(PATH_STR_TRIMMED);
            }
            else
            {
                path /= PATH_STR_TRIMMED;
            }
        };

        addOrAppendPathIfNotEmpty(PATH1);
        addOrAppendPathIfNotEmpty(PATH2);
        addOrAppendPathIfNotEmpty(PATH3);
        addOrAppendPathIfNotEmpty(PATH4);

        if (path.empty())
        {
            return "";
        }
        else
        {
            return CleanPath(path.string());
        }
    }

    const std::string filesystem::AppendPathsToCurrentThenClean(
        const std::string & PATH1, const std::string & PATH2, const std::string & PATH3)
    {
        return CombinePathsThenClean(CurrentDirectory(), PATH1, PATH2, PATH3);
    }

    const std::string filesystem::LimitPathDept(
        const std::string & PATH_STR_ORIG,
        const std::size_t DEPTH_LIMIT,
        const bool WILL_REMOVE_FROM_END)
    {
        if (DEPTH_LIMIT < 1)
        {
            return "";
        }

        const auto PATH_STR_CLEANED { CleanPath(PATH_STR_ORIG) };

        if (PATH_STR_CLEANED.empty())
        {
            return "";
        }

        const bfs::path BOOST_PATH { bfs::path(PATH_STR_CLEANED) };

        std::vector<std::string> pathPartStrings;
        pathPartStrings.reserve(VECTOR_RESERVE_SIZE);

        for (const auto & PATH_SEGMENT : BOOST_PATH)
        {
            const auto PATH_SEGMENT_STR { PATH_SEGMENT.string() };

            if (PATH_SEGMENT_STR.empty() == false)
            {
                pathPartStrings.emplace_back(PATH_SEGMENT_STR);
            }
        }

        if (DEPTH_LIMIT >= pathPartStrings.size())
        {
            return BOOST_PATH.string();
        }

        while ((pathPartStrings.empty() == false) && (pathPartStrings.size() != DEPTH_LIMIT))
        {
            if (WILL_REMOVE_FROM_END)
            {
                pathPartStrings.pop_back();
            }
            else
            {
                pathPartStrings.erase(std::begin(pathPartStrings));
            }
        }

        std::ostringstream ss;
        for (const auto & PATH_PART_STRING : pathPartStrings)
        {
            ss << PATH_PART_STRING << PreferredSeparatorChar();
        }

        std::string finalPathStr { ss.str() };
        finalPathStr.pop_back();

        return finalPathStr;
    }

    bool filesystem::CreateDirectory(const std::string & PATH_STR_ORIG)
    {
        const auto PATH_STR_CLEANED { CleanPath(PATH_STR_ORIG) };

        if (PATH_STR_CLEANED.empty())
        {
            M_HP_LOG_ERR(MakeErrorString(
                PATH_STR_ORIG, PATH_STR_CLEANED, "(The given path was empty after cleaning.)"));

            return false;
        }

        const bfs::path BOOST_PATH { PATH_STR_CLEANED };

        if (bfs::exists(BOOST_PATH))
        {
            if (bfs::is_directory(BOOST_PATH))
            {
                M_HP_LOG_WRN(MakeErrorString(
                    PATH_STR_ORIG,
                    PATH_STR_CLEANED,
                    "(Asked to create a directory that was already there.)"));

                return true;
            }
            else
            {
                const auto ERROR_MESSAGE {
                    "(Failed because something already exists at that path and it's not a "
                    "directory)(is_regular="
                    + misc::ToString(bfs::is_regular(BOOST_PATH))
                    + ", is_regular_file=" + misc::ToString(bfs::is_regular_file(BOOST_PATH))
                    + ", status_file_type=" + misc::ToString(bfs::status(BOOST_PATH).type())
                };

                M_HP_LOG_ERR(MakeErrorString(PATH_STR_ORIG, PATH_STR_CLEANED, ERROR_MESSAGE));
                return false;
            }
        }

        boost::system::error_code errorCode;

        try
        {
            const auto WAS_CREATED { bfs::create_directories(
                bfs::path(PATH_STR_CLEANED), errorCode) };

            if (WAS_CREATED && !errorCode)
            {
                return true;
            }
            else
            {
                M_HP_LOG_ERR(MakeErrorString(
                    PATH_STR_ORIG,
                    PATH_STR_CLEANED,
                    "(create_directory() returned " + misc::ToString(WAS_CREATED) + ")",
                    errorCode));
            }
        }
        catch (const std::exception & EXCEPTION)
        {
            M_HP_LOG_ERR(MakeErrorString(
                PATH_STR_ORIG,
                PATH_STR_CLEANED,
                "(boost::filesystem::create_directory() is what threw)",
                errorCode,
                EXCEPTION.what()));
        }

        return false;
    }

    bool filesystem::Delete(const std::string & PATH_STR_ORIG)
    {
        const auto PATH_STR_CLEANED { CleanPath(PATH_STR_ORIG) };

        if (PATH_STR_CLEANED.empty())
        {
            M_HP_LOG_ERR(MakeErrorString(
                PATH_STR_ORIG, PATH_STR_CLEANED, "(The given path was empty after cleaning.)"));

            return false;
        }

        const bfs::path BOOST_PATH { PATH_STR_CLEANED };

        if (bfs::exists(BOOST_PATH) == false)
        {
            M_HP_LOG_WRN(MakeErrorString(
                PATH_STR_ORIG, PATH_STR_CLEANED, "(Failed because nothing exists at that path.)"));

            return true;
        }

        boost::system::error_code errorCode;

        auto makeExtraErrorInfoString = [&]() {
            const auto IS_REGULAR { bfs::is_regular(BOOST_PATH) };
            const auto IS_REGULAR_FILE { bfs::is_regular_file(BOOST_PATH) };
            const auto IS_DIRECTORY { bfs::is_directory(BOOST_PATH) };
            const auto IS_OTHER { bfs::is_other(BOOST_PATH) };

            return "(is_regular=" + misc::ToString(IS_REGULAR) + ", is_regular_file="
                + misc::ToString(IS_REGULAR_FILE) + ", is_directory=" + misc::ToString(IS_DIRECTORY)
                + ", is_other=" + misc::ToString(IS_OTHER) + ")";
        };

        try
        {
            const auto DELETE_COUNT { bfs::remove_all(bfs::path(PATH_STR_CLEANED), errorCode) };

            if ((DELETE_COUNT > 0) && !errorCode)
            {
                return true;
            }
            else
            {
                M_HP_LOG_ERR(MakeErrorString(
                    PATH_STR_ORIG,
                    PATH_STR_CLEANED,
                    "(remove_all() returned " + misc::ToString(DELETE_COUNT)
                        + ", meaning that many things were deleted.)" + makeExtraErrorInfoString(),
                    errorCode));
            }
        }
        catch (const std::exception & EXCEPTION)
        {
            M_HP_LOG_ERR(MakeErrorString(
                PATH_STR_ORIG,
                PATH_STR_CLEANED,
                "(boost::filesystem::remove_all() is what threw)" + makeExtraErrorInfoString(),
                errorCode,
                EXCEPTION.what()));
        }

        return false;
    }

    const std::string filesystem::MakeErrorString(
        const std::string & PATH_ORIG,
        const std::string & PATH_CLEANED,
        const std::string & MESSAGE,
        const boost::system::error_code & ERROR_CODE,
        const std::string & EXCEPTION_MESSAGE)
    {
        std::ostringstream ss;

        if (PATH_ORIG.empty() == false)
        {
            ss << "(path_orig=\"" << PATH_ORIG << "\")";
        }

        if (PATH_CLEANED.empty() == false)
        {
            ss << "(path_clean=\"" << PATH_CLEANED << "\")";
        }

        if (MESSAGE.empty() == false)
        {
            ss << "(" << MESSAGE << ")";
        }

        if (ERROR_CODE)
        {
            ss << "(error_code=" << ERROR_CODE.value() << ", error_message=\""
               << ERROR_CODE.message() << "\")";
        }

        if (EXCEPTION_MESSAGE.empty() == false)
        {
            ss << "(threw std::exception.what()=\"" << EXCEPTION_MESSAGE << "\")";
        }

        return ss.str();
    }

    const std::string filesystem::MakeNumberedFilename(
        const std::string & FILE_NAME,
        const std::string & NUMBER_PREFIX,
        const std::size_t NUMBER,
        const std::string & FILE_EXTENSION)
    {
        std::ostringstream ss;
        ss << FILE_NAME << NUMBER_PREFIX << NUMBER << FILE_EXTENSION;
        return ss.str();
    }

    bool filesystem::IsWindowsDriveNameWithoutTrailingSlash(const std::string & PATH_STR_ORIG)
    {
        const auto PATH_STR_TRIMMED { boost::trim_copy(PATH_STR_ORIG) };

        if (PATH_STR_TRIMMED.size() < 2)
        {
            return false;
        }

        if (PATH_STR_TRIMMED.back() != ':')
        {
            return false;
        }

        for (std::size_t charIndex(0); charIndex < (PATH_STR_TRIMMED.size() - 1); ++charIndex)
        {
            if (std::isalpha(static_cast<int>(PATH_STR_TRIMMED.at(charIndex))) == false)
            {
                return false;
            }
        }

        return true;
    }

    char filesystem::PreferredSeparatorChar()
    {
        return static_cast<char>(bfs::path::preferred_separator);
    }

    const std::string filesystem::GetBoostFilename(const std::string & PATH_STR_ORIG)
    {
        const auto PATH_STR_TRIMMED { boost::trim_copy(PATH_STR_ORIG) };
        return bfs::path(PATH_STR_TRIMMED).filename().string();
    }

    filesystem::LooksLike
        filesystem::WhatDoesBoostFilenameIndicatePathEndsWith(const std::string & PATH_STR_ORIG)
    {
        const auto BOOST_FILE_NAME_STR { GetBoostFilename(PATH_STR_ORIG) };

        if (BOOST_FILE_NAME_STR.empty())
        {
            return LooksLike::Unknown;
        }
        else
        {
            // only need to handle possible output of boost::filesystem::filename() here

            if (IsSeparator(BOOST_FILE_NAME_STR.back()))
            {
                return LooksLike::Directory;
            }

            if ((BOOST_FILE_NAME_STR == ".") || (BOOST_FILE_NAME_STR == ".."))
            {
                return LooksLike::Directory;
            }

            if (IsWindowsDriveNameWithoutTrailingSlash(BOOST_FILE_NAME_STR))
            {
                return LooksLike::Directory;
            }

            if (EndsWithSeparatorFollowedByDots(BOOST_FILE_NAME_STR))
            {
                return LooksLike::Directory;
            }

            return LooksLike::File;
        }
    }

    bool filesystem::IsSeparator(const char CHAR)
    {
        return ((CHAR == PreferredSeparatorChar()) || (CHAR == '\\') || (CHAR == '/'));
    }

    bool filesystem::EndsWithSeparatorFollowedByDots(const std::string & PATH_STR_ORIG)
    {
        const auto PATH_STR_TRIMMED { boost::trim_copy(PATH_STR_ORIG) };

        if (PATH_STR_TRIMMED.size() < 2 || (PATH_STR_TRIMMED.back() != '.'))
        {
            return false;
        }

        for (auto revIter(std::crbegin(PATH_STR_TRIMMED)); revIter != std::crend(PATH_STR_TRIMMED);
             ++revIter)
        {
            const auto CHAR { *revIter };

            if (CHAR == '.')
            {
                continue;
            }

            return IsSeparator(CHAR);
        }

        return false;
    }

    const std::string filesystem::SlashesToPreferredSeparator(const std::string & PATH_STR_ORIG)
    {
        const std::string BLACK_SLASH_STR { "\\" };
        const std::string FORWARD_SLASH_STR { "/" };
        const std::string PREFERRED_SEPARATOR_STR(1, PreferredSeparatorChar());

        std::string newPath { PATH_STR_ORIG };

        if (PREFERRED_SEPARATOR_STR != BLACK_SLASH_STR)
        {
            boost::algorithm::replace_all(newPath, BLACK_SLASH_STR, PREFERRED_SEPARATOR_STR);
        }

        if (PREFERRED_SEPARATOR_STR != FORWARD_SLASH_STR)
        {
            boost::algorithm::replace_all(newPath, FORWARD_SLASH_STR, PREFERRED_SEPARATOR_STR);
        }

        return newPath;
    }

} // namespace misc
} // namespace heroespath
