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
#include <sstream>

namespace heroespath
{
namespace misc
{

    namespace bfs = boost::filesystem;

    const std::vector<std::string> filesystem::COMMON_FILE_NAME_PARTS_TO_EXCLUDE_VEC_ {
        ".txt", ".DS_Store", "sample.gif", ".html", ".htm", "thumbs.db"
    };

    // this value found by experiment to be a good high guess for the game
    const std::size_t filesystem::VECTOR_RESERVE_SIZE { 30 };

    const std::string filesystem::CleanPath(const std::string & PATH_ORIG)
    {
        static misc::VectorMap<std::string, std::string> cacheMap_;

        if (cacheMap_.Empty())
        {
            cacheMap_.Reserve(4096);
        }

        auto cacheIter { cacheMap_.Find(PATH_ORIG) };

        if (std::end(cacheMap_) != cacheIter)
        {
            return cacheIter->second;
        }

        std::string pathStrToUse { misc::TrimWhitespaceCopy(PATH_ORIG) };

        if (pathStrToUse.empty())
        {
            cacheMap_.Append(PATH_ORIG, "");
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

        cacheMap_.Append(PATH_ORIG, cleanedStr);
        return cleanedStr;
    }

    const std::string filesystem::CurrentDirectory()
    {
        static std::string currentDir;

        if (currentDir.empty())
        {
            currentDir = bfs::current_path().string();
        }

        return currentDir;
    }

    const std::string
        filesystem::Filename(const std::string & PATH_ORIG, const bool WILL_EXCLUDE_EXTENSION)
    {
        const auto PATH_TRIMMED { misc::TrimWhitespaceCopy(PATH_ORIG) };

        if (PATH_TRIMMED.empty())
        {
            return "";
        }

        if (ExistsAndIsDirectory(PATH_TRIMMED))
        {
            return "";
        }

        const auto BOOST_FILE_NAME { GetBoostFilename(PATH_TRIMMED) };

        if (BOOST_FILE_NAME.empty())
        {
            return "";
        }

        if (WhatDoesBoostFilenameIndicatePathEndsWith(BOOST_FILE_NAME) == LooksLike::Directory)
        {
            return "";
        }

        if (WILL_EXCLUDE_EXTENSION)
        {
            const auto FIRST_DOT_POSITION { BOOST_FILE_NAME.find('.') };
            return BOOST_FILE_NAME.substr(0, FIRST_DOT_POSITION);
        }
        else
        {
            return BOOST_FILE_NAME;
        }
    }

    const std::string
        filesystem::Extension(const std::string & PATH, const bool WILL_EXCLUDE_LEADING_DOT)
    {
        const auto FILE_NAME { Filename(PATH) };

        if (FILE_NAME.empty())
        {
            return "";
        }

        const auto FIRST_DOT_POSITION_INDEX { FILE_NAME.find('.') };

        if (FIRST_DOT_POSITION_INDEX == std::string::npos)
        {
            return "";
        }

        const auto EXTENSION { FILE_NAME.substr(FIRST_DOT_POSITION_INDEX) };

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

    bool filesystem::Exists(const std::string & PATH_ORIG)
    {
        const auto PATH_TRIMMED { misc::TrimWhitespaceCopy(PATH_ORIG) };

        if (PATH_TRIMMED.empty())
        {
            return false;
        }

        return bfs::exists(bfs::path(PATH_TRIMMED));
    }

    bool filesystem::ExistsAndIsFile(const std::string & FILE_PATH_ORIG)
    {
        const auto FILE_PATH_TRIMMED { misc::TrimWhitespaceCopy(FILE_PATH_ORIG) };

        if (FILE_PATH_TRIMMED.empty())
        {
            return false;
        }

        const auto BOOST_PATH { bfs::path(FILE_PATH_TRIMMED) };

        return (bfs::exists(BOOST_PATH) && bfs::is_regular_file(BOOST_PATH));
    }

    bool filesystem::ExistsAndIsDirectory(const std::string & DIR_PATH_ORIG)
    {
        const auto DIR_PATH_TRIMMED { misc::TrimWhitespaceCopy(DIR_PATH_ORIG) };

        if (DIR_PATH_TRIMMED.empty())
        {
            return false;
        }

        const auto BOOST_PATH { bfs::path(DIR_PATH_TRIMMED) };

        return (bfs::exists(BOOST_PATH) && bfs::is_directory(BOOST_PATH));
    }

    const std::vector<std::string> filesystem::FindFiles(
        const bool IS_RECURSIVE,
        const std::string & DIR_PATH,
        const std::string & FILENAME_PREFIX,
        const std::string & FILENAME_EXTENSION,
        const std::vector<std::string> & STRING_MATCHES_TO_EXCLUDE)
    {
        if (DIR_PATH.empty() || !ExistsAndIsDirectory(DIR_PATH))
        {
            return {};
        }

        const bfs::path BOOST_PATH { DIR_PATH };

        std::vector<std::string> filePathStrings;
        filePathStrings.reserve(VECTOR_RESERVE_SIZE);

        for (const auto & DIR_ENTRY : bfs::directory_iterator(BOOST_PATH))
        {
            const auto ENTRY_BOOST_PATH { DIR_ENTRY.path() };

            if (IS_RECURSIVE && bfs::is_directory(ENTRY_BOOST_PATH))
            {
                for (const std::string & SUB_DIR_FILE_PATH : FindFiles(
                         true,
                         ENTRY_BOOST_PATH.string(),
                         FILENAME_PREFIX,
                         FILENAME_EXTENSION,
                         STRING_MATCHES_TO_EXCLUDE))
                {
                    filePathStrings.emplace_back(SUB_DIR_FILE_PATH);
                }
            }

            if (bfs::is_regular_file(ENTRY_BOOST_PATH) == false)
            {
                continue;
            }

            const auto FILE_NAME { Filename(ENTRY_BOOST_PATH.string()) };

            namespace ba = boost::algorithm;

            if ((FILENAME_PREFIX.empty() == false)
                && (ba::starts_with(FILE_NAME, FILENAME_PREFIX) == false))
            {
                continue;
            }

            if ((FILENAME_EXTENSION.empty() == false)
                && (ba::ends_with(FILE_NAME, FILENAME_EXTENSION) == false))
            {
                continue;
            }

            if (misc::ContainsAnyOf(FILE_NAME, STRING_MATCHES_TO_EXCLUDE, false))
            {
                continue;
            }

            filePathStrings.emplace_back(ENTRY_BOOST_PATH.string());
        }

        return filePathStrings;
    }

    const std::string filesystem::FindFirstAvailableNumberedFilenamePath(
        const std::string & DIR_PATH,
        const std::string & FILE_NAME,
        const std::string & FILE_EXT,
        const std::string & NUMBER_SEPARATOR)
    {
        if (DIR_PATH.empty() || !ExistsAndIsDirectory(DIR_PATH))
        {
            return "";
        }

        const bfs::path BOOST_DIR_PATH { DIR_PATH };

        bfs::path filePath;

        const std::size_t MAX_ATTEMPT_COUNT { 10000000 };
        for (std::size_t i(1); i <= MAX_ATTEMPT_COUNT; ++i)
        {
            const auto NEXT_NUMBERED_FILE_NAME { MakeNumberedFilename(
                FILE_NAME, NUMBER_SEPARATOR, i, FILE_EXT) };

            filePath = (BOOST_DIR_PATH / NEXT_NUMBERED_FILE_NAME);

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
            const auto ZERO_NUMBERED_FILE_NAME { MakeNumberedFilename(
                FILE_NAME, NUMBER_SEPARATOR, 0, FILE_EXT) };

            M_HP_LOG_ERR(
                "misc::filesystem::FindFirstAvailableNumberedFilenamePath(dir_path="
                << DIR_PATH << "\", file_name=" << FILE_NAME << ", file_ext=" << FILE_EXT
                << ", number_prefix=\"" << NUMBER_SEPARATOR
                << "\") failed to find an available numbered filename (one that looks like this \""
                << FILE_NAME << NUMBER_SEPARATOR << "#" << FILE_EXT << "\") after trying "
                << MAX_ATTEMPT_COUNT
                << " times.  So either there really are that many files there or something has "
                   "gone wrong.  Using zero for the number ("
                << ZERO_NUMBERED_FILE_NAME << ") and moving on...");

            filePath = (BOOST_DIR_PATH / ZERO_NUMBERED_FILE_NAME);
        }

        return filePath.string();
    }

    const std::vector<std::string> filesystem::SortByLastNumberInFilename(
        const std::vector<std::string> & PATH_VEC,
        const bool WILL_MISSING_NUMBERS_APPEAR_FIRST,
        const bool WILL_MISSING_NUMBERS_BE_REMOVED)
    {
        if (PATH_VEC.empty())
        {
            return {};
        }

        misc::VectorMap<int, std::string> lastNumberToPathStrMap;
        lastNumberToPathStrMap.Reserve(PATH_VEC.size());

        std::vector<std::string> pathsWithoutNumbers;
        pathsWithoutNumbers.reserve(VECTOR_RESERVE_SIZE);

        for (const auto & PATH : PATH_VEC)
        {
            if (PATH.empty())
            {
                continue;
            }

            const auto LAST_NUMBER_IN_FILE_NAME { misc::FindLastNumber(Filename(PATH)) };

            if (LAST_NUMBER_IN_FILE_NAME < 0)
            {
                if (WILL_MISSING_NUMBERS_BE_REMOVED == false)
                {
                    pathsWithoutNumbers.emplace_back(PATH);
                }

                continue;
            }

            // use Append() to allow duplicates
            lastNumberToPathStrMap.Append(LAST_NUMBER_IN_FILE_NAME, PATH);
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

        for (const auto & NUM_STR_PAIR : lastNumberToPathStrMap)
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

    const std::string filesystem::CombinePaths(
        const std::string & PATH1,
        const std::string & PATH2,
        const std::string & PATH3,
        const std::string & PATH4)
    {
        bfs::path path;

        auto addOrAppendPathIfNotEmpty = [&](const std::string & PATH) {
            if (PATH.empty() || (PATH == "."))
            {
                return;
            }

            if (path.empty())
            {
                path = bfs::path(PATH);
            }
            else
            {
                path /= PATH;
            }
        };

        addOrAppendPathIfNotEmpty(PATH1);
        addOrAppendPathIfNotEmpty(PATH2);
        addOrAppendPathIfNotEmpty(PATH3);
        addOrAppendPathIfNotEmpty(PATH4);

        return path.string();
    }

    const std::string filesystem::AppendPathsToCurrent(
        const std::string & PATH1, const std::string & PATH2, const std::string & PATH3)
    {
        return CombinePaths(CurrentDirectory(), PATH1, PATH2, PATH3);
    }

    const std::string filesystem::LimitPathDept(
        const std::string & PATH, const std::size_t DEPTH_LIMIT, const bool WILL_REMOVE_FROM_END)
    {
        if (DEPTH_LIMIT < 1)
        {
            return "";
        }

        if (PATH.empty())
        {
            return "";
        }

        const bfs::path BOOST_PATH { bfs::path(PATH) };

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

        std::string finalPathStr;
        for (const auto & PATH_PART_STRING : pathPartStrings)
        {
            finalPathStr += PATH_PART_STRING;
            finalPathStr += PreferredSeparatorChar();
        }

        finalPathStr.pop_back();
        return finalPathStr;
    }

    bool filesystem::CreateDirectory(const std::string & DIR_PATH_ORIG)
    {
        const bfs::path BOOST_PATH { DIR_PATH_ORIG };

        if (bfs::exists(BOOST_PATH))
        {
            if (bfs::is_directory(BOOST_PATH))
            {
                M_HP_LOG_WRN(MakeErrorString(
                    DIR_PATH_ORIG, "Asked to create a directory that was already there."));

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

                M_HP_LOG_ERR(MakeErrorString(DIR_PATH_ORIG, ERROR_MESSAGE));
                return false;
            }
        }

        boost::system::error_code errorCode;

        try
        {
            const auto WAS_CREATED { bfs::create_directories(BOOST_PATH, errorCode) };

            if (WAS_CREATED && !errorCode)
            {
                return true;
            }
            else
            {
                M_HP_LOG_ERR(MakeErrorString(
                    DIR_PATH_ORIG,
                    "create_directory() returned " + misc::ToString(WAS_CREATED),
                    errorCode));
            }
        }
        catch (const std::exception & EXCEPTION)
        {
            M_HP_LOG_ERR(MakeErrorString(
                DIR_PATH_ORIG,
                "boost::filesystem::create_directory() is what threw.",
                errorCode,
                EXCEPTION.what()));
        }

        return false;
    }

    bool filesystem::Delete(const std::string & PATH)
    {
        if (PATH.empty())
        {
            M_HP_LOG_ERR(MakeErrorString(PATH, "(The given path was empty after cleaning.)"));
            return false;
        }

        const bfs::path BOOST_PATH { PATH };

        if (bfs::exists(BOOST_PATH) == false)
        {
            M_HP_LOG_WRN(MakeErrorString(PATH, "Failed because nothing exists at that path."));
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
            const auto DELETE_COUNT { bfs::remove_all(BOOST_PATH, errorCode) };

            if ((DELETE_COUNT > 0) && !errorCode)
            {
                return true;
            }
            else
            {
                M_HP_LOG_ERR(MakeErrorString(
                    PATH,
                    "remove_all() returned " + misc::ToString(DELETE_COUNT)
                        + ", meaning that many things were deleted." + makeExtraErrorInfoString(),
                    errorCode));
            }
        }
        catch (const std::exception & EXCEPTION)
        {
            M_HP_LOG_ERR(MakeErrorString(
                PATH,
                "boost::filesystem::remove_all() is what threw" + makeExtraErrorInfoString(),
                errorCode,
                EXCEPTION.what()));
        }

        return false;
    }

    const std::string filesystem::MakeErrorString(
        const std::string & PATH,
        const std::string & MESSAGE,
        const boost::system::error_code & ERROR_CODE,
        const std::string & EXCEPTION)
    {
        const std::string BOOST_SYSTEM_ERROR = [&]() -> std::string {
            if (ERROR_CODE)
            {
                return "[boost_error_code=" + misc::ToString(ERROR_CODE.value())
                    + ", boost_error_message=" + misc::Quoted(ERROR_CODE.message()) + "]";
            }
            else
            {
                return "";
            }
        }();

        std::string finalStr;

        if (MESSAGE.empty() == false)
        {
            finalStr += M_HP_VAR_STR(MESSAGE);
        }

        if (PATH.empty() == false)
        {
            finalStr += M_HP_VAR_STR(PATH);
        }

        if (BOOST_SYSTEM_ERROR.empty() == false)
        {
            finalStr += M_HP_VAR_STR(BOOST_SYSTEM_ERROR);
        }

        if (EXCEPTION.empty() == false)
        {
            finalStr += M_HP_VAR_STR(EXCEPTION);
        }

        return finalStr;
    }

    const std::string filesystem::MakeNumberedFilename(
        const std::string & FILE_NAME,
        const std::string & NUMBER_PREFIX,
        const std::size_t NUMBER,
        const std::string & FILE_EXTENSION)
    {
        return FILE_NAME + NUMBER_PREFIX + misc::ToString(NUMBER) + FILE_EXTENSION;
    }

    bool filesystem::IsWindowsDriveNameWithoutTrailingSlash(const std::string & PATH_TRIMMED)
    {
        if (PATH_TRIMMED.size() < 2)
        {
            return false;
        }

        if (PATH_TRIMMED.back() != ':')
        {
            return false;
        }

        for (std::size_t charIndex(0); charIndex < (PATH_TRIMMED.size() - 1); ++charIndex)
        {
            if (misc::IsAlpha(PATH_TRIMMED.at(charIndex)) == false)
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

    const std::string filesystem::GetBoostFilename(const std::string & PATH_TRIMMED)
    {
        return bfs::path(PATH_TRIMMED).filename().string();
    }

    filesystem::LooksLike
        filesystem::WhatDoesBoostFilenameIndicatePathEndsWith(const std::string & BOOST_FILE_NAME)
    {
        if (BOOST_FILE_NAME.empty())
        {
            return LooksLike::Unknown;
        }

        // only need to handle possible output of boost::filesystem::filename() here

        if (IsSeparator(BOOST_FILE_NAME.back()))
        {
            return LooksLike::Directory;
        }

        if ((BOOST_FILE_NAME == ".") || (BOOST_FILE_NAME == ".."))
        {
            return LooksLike::Directory;
        }

        if (IsWindowsDriveNameWithoutTrailingSlash(BOOST_FILE_NAME))
        {
            return LooksLike::Directory;
        }

        if (EndsWithSeparatorFollowedByDots(BOOST_FILE_NAME))
        {
            return LooksLike::Directory;
        }

        return LooksLike::File;
    }

    bool filesystem::IsSeparator(const char CHAR)
    {
        return ((CHAR == PreferredSeparatorChar()) || (CHAR == '\\') || (CHAR == '/'));
    }

    bool filesystem::EndsWithSeparatorFollowedByDots(const std::string & PATH_TRIMMED)
    {
        if (PATH_TRIMMED.size() < 2 || (PATH_TRIMMED.back() != '.'))
        {
            return false;
        }

        for (auto revIter(std::crbegin(PATH_TRIMMED)); revIter != std::crend(PATH_TRIMMED);
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

    const std::string filesystem::SlashesToPreferredSeparator(const std::string & PATH_ORIG)
    {
        const std::string BLACK_SLASH_STR { "\\" };
        const std::string FORWARD_SLASH_STR { "/" };
        const std::string PREFERRED_SEPARATOR_STR(1, PreferredSeparatorChar());

        std::string newPath { PATH_ORIG };

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
