// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_MISC_FILESYSTEM_HELPERS_HPP_INCLUDED
#define HEROESPATH_MISC_FILESYSTEM_HELPERS_HPP_INCLUDED
//
// filesystem.hpp
//
#include <boost/system/error_code.hpp>

#include <string>
#include <vector>

namespace heroespath
{
namespace misc
{

    // A collection of static functions that perform all filesystem operations for the game.  The
    // goal is to completely hide which filesystem library we use behind this interface. (I want to
    // eventually migrate from boost::filesystem to std::filesystem...)
    struct filesystem
    {
        // If the path exists or can be made absolute then it is made absolute, otherwise it
        // is just cleaned.  If PATH.empty() then an empty string is returned.  Clean
        // means the following:
        //      - misc path junk is trimmed from both ends
        //      - redundant paths in the middle (i.e. "/foo/././../bar") are resolved
        //      - links/symlinks/etc are all resolved
        //      - directory separators are all converted to whatever the OS prefers
        //      - redundant trailing dots such as "\." or "/." are removed
        static const std::string CleanPath(const std::string & PATH);

        static const std::string CurrentDirectory();

        // If PATH exists and is a regular file then the file name is returned, if PATH
        // does not exist then a best guess attempt is made that returns and empty string on
        // failure.
        static const std::string
            Filename(const std::string & PATH, const bool WILL_EXCLUDE_EXTENSION = false);

        // If the result of Filename(PATH) contains a dot then that dot and everything after it
        // is returned.  Empty input returns empty output.
        static const std::string
            Extension(const std::string & PATH, const bool WILL_EXCLUDE_LEADING_DOT = false);

        // empty input returns false.
        static bool Exists(const std::string & PATH);
        static bool ExistsAndIsFile(const std::string & FILE_PATH);
        static bool ExistsAndIsDirectory(const std::string & DIR_PATH);

        // returns the full/cleaned/absolute paths of all matching files in DIR_PATH or an empty
        // vector, FILE_NAME_EXTENSION should start with a period unless it is empty, if
        // FILE_NAME_BASE or FILE_NAME_EXTENSION are empty then they are ignored, if both are empty
        // then all files are returned, if DIR_PATH is empty, does not exist, or is not a
        // directory then the returned vector will also be empty
        static const std::vector<std::string> FindFiles(
            const bool IS_RECURSIVE,
            const std::string & DIR_PATH,
            const std::string & FILE_NAME_BASE = "",
            const std::string & FILE_NAME_EXTENSION = "",
            const std::vector<std::string> & STRING_MATCHES_TO_EXCLUDE
            = std::vector<std::string>());

        // returns the full/cleaned/absolute paths of all matching directories in DIR_PATH or an
        // empty vector,  if DIR_PATH is empty, does not exist, or is not a directory then the
        // returned vector will also be empty
        static const std::vector<std::string> FindDirectories(
            const bool IS_RECURSIVE,
            const std::string & DIR_PATH,
            const std::string & DIR_NAME_PREFIX = "");

        // returns the full/cleaned/absolute path of the first available numbered filename or an
        // empty string, FILE_NAME_EXTENSION should start with a period unless it is empty, if
        // DIR_PATH is empty or does not exist or is not a directory then the returned vector
        // will also be empty
        static const std::string FindFirstAvailableNumberedFilenamePath(
            const std::string & DIR_PATH,
            const std::string & FILE_NAME,
            const std::string & FILE_NAME_EXTENSION,
            const std::string & NUMBER_SEPARATOR = "-");

        // cleans each path and then sorts by the last number found in each, non-stable, empties are
        // excluded from the output
        static const std::vector<std::string> SortByLastNumberInFilename(
            const std::vector<std::string> & PATH_VEC,
            const bool WILL_MISSING_NUMBERS_APPEAR_FIRST = false,
            const bool WILL_MISSING_NUMBERS_BE_REMOVED = false);

        // trims then merges each path, empty or "." paths will be skipped, if all are skipped then
        // an empty string is returned
        static const std::string CombinePaths(
            const std::string & PATH1,
            const std::string & PATH2,
            const std::string & PATH3 = "",
            const std::string & PATH4 = "");

        static const std::string CombinePathsAndClean(
            const std::string & PATH1,
            const std::string & PATH2,
            const std::string & PATH3 = "",
            const std::string & PATH4 = "")
        {
            return CleanPath(CombinePaths(PATH1, PATH2, PATH3, PATH4));
        }

        // returns the current system directory plus the combined paths, empty strings are ignored,
        // if all strings are empty then an empty string is returned
        static const std::string AppendPathsToCurrent(
            const std::string & PATH1,
            const std::string & PATH2 = "",
            const std::string & PATH3 = "");

        static const std::string AppendPathsToCurrentAndClean(
            const std::string & PATH1,
            const std::string & PATH2 = "",
            const std::string & PATH3 = "")
        {
            return CleanPath(AppendPathsToCurrent(PATH1, PATH2, PATH3));
        }

        // returns a path that contains at most DEPTH_LIMIT directories, empty input returns empty,
        // if DEPTH_LIMIT is zero then returns empty, the given path is first cleaned but the
        // resulting path is not
        static const std::string LimitPathDept(
            const std::string & PATH,
            const std::size_t DEPTH_LIMIT,
            const bool WILL_REMOVE_FROM_END = false);

        // if the directory was created then true is returned, if multiple directories are specified
        // then they will all be created, if the directory was already there a warning is logged and
        // true is returned, if DIR_PATH is empty then an error is logged and false is returned,
        // if the directory could not be created then a detailed error is logged and false is
        // returned, DIR_PATH is CleanPath()'d before it is used
        static bool CreateDirectory(const std::string & DIR_PATH);

        // if whatever PATH refers to was deleted then true is returned, if PATH does not
        // exist then a warning is logged and true is returned, if PATH is empty then an error
        // is logged and false is returned, if whatever PATH refers to could not be deleted then
        // a detailed error is logged and false is returned, PATH is CleanPath()'d first
        static bool Delete(const std::string & PATH);

        static const std::vector<std::string> COMMON_FILE_NAME_PARTS_TO_EXCLUDE_VEC_;

        static const std::string MakeErrorString(
            const std::string & PATH,
            const std::string & MESSAGE,
            const boost::system::error_code & ERROR_CODE = boost::system::error_code(),
            const std::string & EXCEPTION_MESSAGE = "");

        static const std::string MakeNumberedFilename(
            const std::string & FILE_NAME,
            const std::string & NUMBER_PREFIX,
            const std::size_t NUMBER,
            const std::string & FILE_NAME_EXTENSION);

        // on windows the bfs::path::preferred_separator is a wchar_t that when streamed turns into
        // a number, so this makes sure it is just one character
        static char PreferredSeparatorChar();

        static bool IsSeparator(const char);

        // for some reason boost::filesystem make_preferred() does not always work
        static const std::string SlashesToPreferredSeparator(const std::string & PATH);

    private:
        static const std::size_t VECTOR_RESERVE_SIZE;

        enum class LooksLike
        {
            Unknown,
            File,
            Directory
        };

        // assumes input has had whitespace trimmed, detecting these are important because
        // boost::filesystem functions will throw on Windows if given a bare drive letter such as
        // "c:" or "prn:" without the tailing slash.
        static bool IsWindowsDriveNameWithoutTrailingSlash(const std::string & PATH_TRIMMED);

        // assumes input has had whitespace trimmed
        static LooksLike
            WhatDoesBoostFilenameIndicatePathEndsWith(const std::string & BOOST_FILE_NAME);

        // //assumes input has had whitespace trimmed, boost::filesystem::filename() has some nice
        // behavior that actually helps identify directories as well as files...
        static const std::string GetBoostFilename(const std::string & PATH_TRIMMED);

        // assumes input has had whitespace trimmed
        static bool EndsWithSeparatorFollowedByDots(const std::string & PATH_TRIMMED);
    };

} // namespace misc
} // namespace heroespath

#endif // HEROESPATH_MISC_FILESYSTEM_HELPERS_HPP_INCLUDED
