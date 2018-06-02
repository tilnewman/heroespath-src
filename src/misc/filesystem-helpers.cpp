// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// filesystem-helpers.cpp
//
#include "filesystem-helpers.hpp"

#include "misc/assertlogandthrow.hpp"
#include "misc/boost-string-includes.hpp"
#include "misc/vector-map.hpp"

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

#include <cctype>
#include <sstream>

namespace heroespath
{
namespace misc
{
    namespace filesystem
    {
        const std::vector<std::string> FilenameText::TO_EXCLUDE_VEC_{
            ".txt", ".DS_Store", "sample.gif", ".html", ".htm"
        };

        namespace bfs = boost::filesystem;

        const std::string MakePathPretty(const std::string & PATH_STR)
        {
            return MakePathPretty(bfs::path(PATH_STR)).string();
        }

        const boost::filesystem::path MakePathPretty(const boost::filesystem::path & PATH)
        {
            return bfs::system_complete(PATH).normalize();
        }

        bool DoesFileExist(const std::string & PATH_STR)
        {
            return DoesFileExist(bfs::path(PATH_STR));
        }

        bool DoesFileExist(const boost::filesystem::path & PATH)
        {
            return (bfs::exists(PATH) && bfs::is_regular_file(PATH));
        }

        bool DoesDirectoryExist(const std::string & PATH_STR)
        {
            return DoesDirectoryExist(bfs::path(PATH_STR));
        }

        bool DoesDirectoryExist(const boost::filesystem::path & PATH)
        {
            return (bfs::exists(PATH) && bfs::is_directory(PATH));
        }

        const boost::filesystem::path
            MakeSubDirectoryPathFromCurrent(const std::string & SUB_DIR_NAME)
        {
            return bfs::system_complete(bfs::current_path() / bfs::path(SUB_DIR_NAME)).normalize();
        }

        const std::vector<boost::filesystem::path> FindFilesInDirectory(
            const boost::filesystem::path & DIR_PATH,
            const std::string & FILENAME_PREFIX,
            const std::string & FILENAME_EXTENSION,
            const std::vector<std::string> & EXCLUDES)
        {
            std::vector<bfs::path> filePaths;

            if ((bfs::exists(DIR_PATH) == false) || (bfs::is_directory(DIR_PATH) == false))
            {
                return filePaths;
            }

            for (auto const & DIR_ENTRY : bfs::directory_iterator(DIR_PATH))
            {
                auto const PATH{ DIR_ENTRY.path() };

                if (bfs::is_regular_file(PATH))
                {
                    auto const LEAF_STR{ PATH.leaf().string() };

                    namespace ba = boost::algorithm;

                    if (FILENAME_PREFIX.empty() || ba::starts_with(LEAF_STR, FILENAME_PREFIX))
                    {
                        if (FILENAME_EXTENSION.empty()
                            || ba::ends_with(LEAF_STR, FILENAME_EXTENSION))
                        {
                            if (DoesStringContainAnyOf(LEAF_STR, EXCLUDES, false) == false)
                            {
                                filePaths.emplace_back(MakePathPretty(DIR_PATH / PATH.leaf()));
                            }
                        }
                    }
                }
            }

            return filePaths;
        }

        const std::vector<std::string> FindFilesInDirectory(
            const std::string & DIR_PATH,
            const std::string & FILENAME_PREFIX,
            const std::string & FILENAME_EXTENSION,
            const std::vector<std::string> & EXCLUDES)
        {
            std::vector<std::string> filePaths;

            for (auto const & PATH : FindFilesInDirectory(
                     bfs::path(DIR_PATH), FILENAME_PREFIX, FILENAME_EXTENSION, EXCLUDES))
            {
                filePaths.emplace_back(PATH.string());
            }

            return filePaths;
        }

        const boost::filesystem::path FindFirstAvailableNumberedFilename(
            const boost::filesystem::path & DIR_PATH,
            const std::string & FILE_NAME,
            const std::string & FILE_EXT,
            const std::string & NUMBER_PREFIX)
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (DoesDirectoryExist(DIR_PATH)),
                "misc::filesystem::FindNextAvailableNumberedFilename(dir_path="
                    << DIR_PATH.string() << ", file_name=" << FILE_NAME << ", file_ext=" << FILE_EXT
                    << ") but that dir_path does not exist, or is not a directory.");

            bfs::path filePath;

            std::ostringstream filenameSS;

            const std::size_t MAX_ATTEMPT_COUNT{ 1000000 };
            for (std::size_t i(1);; ++i)
            {
                M_ASSERT_OR_LOGANDTHROW_SS(
                    (i < MAX_ATTEMPT_COUNT),
                    "misc::filesystem::FindNextAvailableNumberedFilename(dir_path="
                        << DIR_PATH.string() << ", file_name=" << FILE_NAME << ", file_ext="
                        << FILE_EXT << ") failed to find an available filename after "
                        << MAX_ATTEMPT_COUNT << " attempts.");

                filenameSS.str("");
                filenameSS << FILE_NAME << NUMBER_PREFIX << i << FILE_EXT;

                filePath = DIR_PATH / bfs::path(filenameSS.str());

                if (bfs::exists(filePath) == false)
                {
                    break;
                }
            }

            return MakePathPretty(filePath);
        }

        const std::string FindFirstAvailableNumberedFilename(
            const std::string & DIR_PATH_STR,
            const std::string & FILE_NAME,
            const std::string & FILE_EXT,
            const std::string & NUMBER_PREFIX)
        {
            return FindFirstAvailableNumberedFilename(
                       bfs::path(DIR_PATH_STR), FILE_NAME, FILE_EXT, NUMBER_PREFIX)
                .string();
        }

        void SortByNumberInFilename(std::vector<std::string> & filePathStrings)
        {
            std::vector<bfs::path> filePaths;

            for (auto const & FILE_PATH_STR : filePathStrings)
            {
                filePaths.emplace_back(bfs::path(FILE_PATH_STR));
            }

            SortByNumberInFilename(filePaths);

            filePathStrings.clear();

            for (auto const & PATH : filePaths)
            {
                filePathStrings.emplace_back(PATH.string());
            }
        }

        void SortByNumberInFilename(std::vector<boost::filesystem::path> & filePaths)
        {
            misc::VectorMap<int, boost::filesystem::path> numberToStrMap;

            for (auto const & PATH : filePaths)
            {
                // use Append() to allow for duplicates
                numberToStrMap.Append(FindLastNumberInFilename(PATH), PATH);
            }

            filePaths.clear();

            numberToStrMap.Sort();

            for (auto const & NUM_STR_PAIR : numberToStrMap)
            {
                filePaths.emplace_back(NUM_STR_PAIR.second);
            }
        }

        int FindLastNumberInFilename(const std::string & PATH_STR)
        {
            return FindLastNumberInFilename(bfs::path(PATH_STR));
        }

        int FindLastNumberInFilename(const boost::filesystem::path & PATH)
        {
            auto const FILE_NAME_STR{ PATH.leaf().string() };

            if (FILE_NAME_STR.empty())
            {
                return 0;
            }

            std::string numberStr{ "" };
            auto i{ FILE_NAME_STR.size() - 1 };
            do
            {
                auto const CHAR{ FILE_NAME_STR.at(i) };
                auto const IS_DIGIT{ std::isdigit(static_cast<unsigned char>(CHAR)) != 0 };

                if ((numberStr.empty() == false) && (IS_DIGIT == false))
                {
                    break;
                }
                else if (IS_DIGIT)
                {
                    numberStr.push_back(CHAR);
                }

                --i;
            } while (i != 0);

            std::reverse(numberStr.begin(), numberStr.end());

            try
            {
                return boost::lexical_cast<int>(numberStr);
            }
            catch (...)
            {
                return 0;
            }
        }

        bool DoesStringContainAnyOf(
            const std::string & S,
            const std::vector<std::string> & MATCH_STRINGS,
            const bool IS_CASE_SENSITIVE)
        {
            namespace ba = boost::algorithm;

            for (auto const & STRING_TO_FIND : MATCH_STRINGS)
            {
                if (IS_CASE_SENSITIVE)
                {
                    if (ba::contains(S, STRING_TO_FIND))
                    {
                        return true;
                    }
                }
                else
                {
                    if (ba::icontains(S, STRING_TO_FIND))
                    {
                        return true;
                    }
                }
            }

            return false;
        }

    } // namespace filesystem
} // namespace misc
} // namespace heroespath
