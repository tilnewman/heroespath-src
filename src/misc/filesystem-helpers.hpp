// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_MISC_FILESYSTEM_HELPERS_HPP_INCLUDED
#define HEROESPATH_MISC_FILESYSTEM_HELPERS_HPP_INCLUDED
//
// filesystem-helpers.hpp
//
#include <boost/filesystem/path.hpp>

#include <string>
#include <vector>

namespace heroespath
{
namespace misc
{
    namespace filesystem
    {

        struct FilenameText
        {
            static const std::vector<std::string> TO_EXCLUDE_VEC_;
        };

        // returns the filename, if WILL_INCLUDE_EXTENSION then the dot and extension is appended if
        // it exists
        const std::string
            Filename(const std::string & PATH_STR, const bool WILL_INCLUDE_EXTENSION = true);

        // returns the filename extension, if only a dot is returned then there was a dot with
        // nothing after, if empty is returned then there was no dot after the filename, same as
        // bfs::extension()
        const std::string Extension(const std::string & PATH_STR);

        // returns all characters in the extension except for the leading dot
        const std::string ExtensionWithoutDot(const std::string & PATH_STR);

        const std::string MakePathPretty(const std::string &);
        const boost::filesystem::path MakePathPretty(const boost::filesystem::path &);

        bool DoesFileExist(const std::string &);
        bool DoesFileExist(const boost::filesystem::path &);

        bool DoesDirectoryExist(const std::string &);
        bool DoesDirectoryExist(const boost::filesystem::path &);

        const boost::filesystem::path
            MakeSubDirectoryPathFromCurrent(const std::string & SUB_DIR_NAME);

        // non-recursive, does NOT throw if dir does not exist
        const std::vector<boost::filesystem::path> FindFilesInDirectory(
            const boost::filesystem::path & DIR_PATH,
            const std::string & FILENAME_PREFIX = "",
            const std::string & FILENAME_EXTENSION = "",
            const std::vector<std::string> & EXCLUDES = std::vector<std::string>());

        // non-recursive, does NOT throw if dir does not exist
        const std::vector<std::string> FindFilesInDirectory(
            const std::string & DIR_PATH,
            const std::string & FILENAME_PREFIX = "",
            const std::string & FILENAME_EXTENSION = "",
            const std::vector<std::string> & EXCLUDES = std::vector<std::string>());

        // throws if dir does not exist
        const boost::filesystem::path FindFirstAvailableNumberedFilename(
            const boost::filesystem::path & DIR_PATH,
            const std::string & FILE_NAME,
            const std::string & FILE_EXT,
            const std::string & NUMBER_PREFIX = "-");

        // throws if dir does not exist
        const std::string FindFirstAvailableNumberedFilename(
            const std::string & DIR_PATH_STR,
            const std::string & FILE_NAME,
            const std::string & FILE_EXT,
            const std::string & NUMBER_PREFIX = "-");

        void SortByNumberInFilename(std::vector<std::string> &);

        void SortByNumberInFilename(std::vector<boost::filesystem::path> &);

        int FindLastNumberInFilename(const std::string &);

        int FindLastNumberInFilename(const boost::filesystem::path &);

        bool DoesStringContainAnyOf(
            const std::string & S,
            const std::vector<std::string> & MATCH_STRINGS,
            const bool IS_CASE_SENSITIVE);

        const std::string CompletePath(
            const std::string & DIR_PATH_STR, const std::string & RELATIVE_FILE_PATH_STR);

        const std::string CompletePath(
            const boost::filesystem::path & DIR_PATH, const std::string & RELATIVE_FILE_PATH_STR);

        const boost::filesystem::path CompletePath(
            const boost::filesystem::path & DIR_PATH,
            const boost::filesystem::path & RELATIVE_FILE_PATH);

        const std::string PathWithDepth(const std::string & PATH_STR, const std::size_t DEPTH);

        const boost::filesystem::path
            PathWithDepth(const boost::filesystem::path & PATH, const std::size_t DEPTH);

    } // namespace filesystem
} // namespace misc
} // namespace heroespath

#endif // HEROESPATH_MISC_FILESYSTEM_HELPERS_HPP_INCLUDED
