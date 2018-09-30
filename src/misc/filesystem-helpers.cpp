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

#include <algorithm>
#include <cctype>
#include <sstream>

namespace heroespath
{
namespace misc
{
    namespace filesystem
    {
        const std::vector<std::string> FilenameText::TO_EXCLUDE_VEC_ {
            ".txt", ".DS_Store", "sample.gif", ".html", ".htm"
        };

        namespace bfs = boost::filesystem;

        const std::string Filename(const std::string & PATH_STR, const bool WILL_INCLUDE_EXTENSION)
        {
            std::string fileName { bfs::path(PATH_STR).filename().string() };

            if (("." == fileName) || (".." == fileName) || ("/" == fileName) || ("\\" == fileName))
            {
                return "";
            }
            else
            {
                if (WILL_INCLUDE_EXTENSION == false)
                {
                    const std::string EXT { bfs::path(PATH_STR).extension().string() };
                    if (EXT.empty() == false)
                    {
                        boost::algorithm::erase_last(fileName, EXT);
                    }
                }

                return fileName;
            }
        }

        const std::string Extension(const std::string & PATH_STR)
        {
            return bfs::path(PATH_STR).extension().string();
        }

        const std::string ExtensionWithoutDot(const std::string & PATH_STR)
        {
            return boost::erase_first_copy(Extension(PATH_STR), ".");
        }

        const std::string MakePathPretty(const std::string & PATH_STR)
        {
            return MakePathPretty(bfs::path(PATH_STR)).string();
        }

        const boost::filesystem::path MakePathPretty(const boost::filesystem::path & PATH)
        {
            return bfs::system_complete(PATH).normalize();
        }

        const std::string CurrentDirectoryString() { return CurrentDirectoryPath().string(); }

        const boost::filesystem::path CurrentDirectoryPath()
        {
            return MakePathPretty(bfs::current_path());
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
                auto const PATH { DIR_ENTRY.path() };

                if (bfs::is_regular_file(PATH))
                {
                    auto const FILE_NAME { PATH.filename().string() };

                    namespace ba = boost::algorithm;

                    if (FILENAME_PREFIX.empty() || ba::starts_with(FILE_NAME, FILENAME_PREFIX))
                    {
                        if (FILENAME_EXTENSION.empty()
                            || ba::ends_with(FILE_NAME, FILENAME_EXTENSION))
                        {
                            if (DoesStringContainAnyOf(FILE_NAME, EXCLUDES, false) == false)
                            {
                                filePaths.emplace_back(MakePathPretty(DIR_PATH / FILE_NAME));
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
                filePaths.emplace_back(MakePathPretty(PATH.string()));
            }

            return filePaths;
        }

        const boost::filesystem::path FindFirstAvailableNumberedFilename(
            const boost::filesystem::path & DIR_PATH,
            const std::string & FILE_NAME,
            const std::string & FILE_EXT,
            const std::string & NUMBER_PREFIX)
        {
            M_HP_ASSERT_OR_LOG_AND_THROW(
                (DoesDirectoryExist(DIR_PATH)),
                "misc::filesystem::FindNextAvailableNumberedFilename(dir_path="
                    << DIR_PATH.string() << ", file_name=" << FILE_NAME << ", file_ext=" << FILE_EXT
                    << ") but that dir_path does not exist, or is not a directory.");

            bfs::path filePath;

            std::ostringstream filenameSS;

            const std::size_t MAX_ATTEMPT_COUNT { 1000000000 };
            for (std::size_t i(1); i <= MAX_ATTEMPT_COUNT; ++i)
            {
                M_HP_ASSERT_OR_LOG_AND_THROW(
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

            std::sort(std::begin(numberToStrMap), std::end(numberToStrMap));

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
            auto const FILE_NAME_STR { PATH.filename().string() };

            if (FILE_NAME_STR.empty())
            {
                return 0;
            }

            std::string numberStr { "" };
            auto i { FILE_NAME_STR.size() - 1 };
            do
            {
                auto const CHAR { FILE_NAME_STR.at(i) };
                auto const IS_DIGIT { std::isdigit(static_cast<unsigned char>(CHAR)) != 0 };

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

        const std::string
            CompletePath(const std::string & DIR_PATH_STR, const std::string & FILE_NAME)
        {
            return CompletePath(bfs::path(DIR_PATH_STR), bfs::path(FILE_NAME)).string();
        }

        const std::string
            CompletePath(const boost::filesystem::path & DIR_PATH, const std::string & FILE_NAME)
        {
            return CompletePath(DIR_PATH, bfs::path(FILE_NAME)).string();
        }

        const boost::filesystem::path CompletePath(
            const boost::filesystem::path & DIR_PATH,
            const boost::filesystem::path & RELATIVE_FILE_PATH)
        {
            return MakePathPretty(DIR_PATH / RELATIVE_FILE_PATH);
        }

        const std::string PathWithDepth(const std::string & PATH_STR, const std::size_t DEPTH)
        {
            return PathWithDepth(MakePathPretty(boost::filesystem::path(PATH_STR)), DEPTH).string();
        }

        const boost::filesystem::path
            PathWithDepth(const boost::filesystem::path & PATH_ORIG, const std::size_t DEPTH)
        {
            auto const PATH_FINAL { MakePathPretty(PATH_ORIG) };

            if ((DEPTH == 0) || (std::rbegin(PATH_FINAL) == std::rend(PATH_FINAL)))
            {
                return boost::filesystem::path("");
            }

            std::vector<std::string> pathParts;
            pathParts.reserve(DEPTH);

            std::size_t i(0);
            auto rPathIter { std::rbegin(PATH_FINAL) };

            do
            {
                pathParts.push_back(rPathIter->string());
            } while ((++i < DEPTH) && (++rPathIter != std::rend(PATH_FINAL)));

            std::ostringstream ss;

            for (auto rPartsIter(std::rbegin(pathParts)); rPartsIter != std::rend(pathParts);
                 ++rPartsIter)
            {
                ss << boost::filesystem::path::preferred_separator << *rPartsIter;
            }

            return boost::filesystem::path(ss.str());
        }

        void CreateDirectory(const std::string & PATH)
        {
            CreateDirectory(boost::filesystem::path(PATH));
        }

        void CreateDirectory(const boost::filesystem::path & PATH)
        {
            boost::system::error_code errorCode;

            if (DoesDirectoryExist(PATH))
            {
                M_HP_LOG_WRN(
                    "misc::filesystem::CreateDirectory(\""
                    << PATH.string()
                    << "\") was asked to create that directory but it already existed.");
            }
            else
            {
                const auto WAS_CREATED { boost::filesystem::create_directory(PATH, errorCode) };

                M_HP_ASSERT_OR_LOG_AND_THROW(
                    (WAS_CREATED),
                    "misc::filesystem::CreateDirectory(\""
                        << PATH.string() << "\") failed with error_code=" << errorCode.value()
                        << ", error_message=\"" << errorCode.message() << "\"");
            }
        }

    } // namespace filesystem
} // namespace misc
} // namespace heroespath
