// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------

#define BOOST_TEST_MODULE "HeroesPathTestModule_Misc_Filesystem"

#include <boost/test/unit_test.hpp>

#include "misc/filesystem.hpp"
#include "misc/platform.hpp"
#include "misc/strings.hpp"

#include <boost/algorithm/algorithm.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include <algorithm>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

using namespace heroespath;
using namespace heroespath::misc;
namespace bfs = boost::filesystem;

struct Help
{
    Help()
        : JUNK_WHITESPACE_STR { "   \t  " }
        , TEST_DIR_NAME_STR { "filesystem-test-stuff" }
        , TEST_DIR_PATH_STR { CombinePaths(bfs::current_path().string(), TEST_DIR_NAME_STR) }
        , STRANGE_DIR_NAME_STR { "strange-things" }
        , STRANGE_DIR_PATH_STR { CombinePaths(TEST_DIR_PATH_STR, STRANGE_DIR_NAME_STR) }
        , STRANGE_FILE_STRINGS { ".file-that-starts-with-dot",
                                 "file-that-ends-with-dot.",
                                 "file-with-multiple-extensions.txt.png.pdf" }
        , STRANGE_DIRECTORY_STRINGS { ".dir-that-starts-with-dot",
                                      "dir-that-ends-with-dot.",
                                      "dir-with-extension.txt" }
        , ALL_STRANGE_STRINGS { CombineVectors(STRANGE_FILE_STRINGS, STRANGE_DIRECTORY_STRINGS) }
        , LOG_FILES_IN_FIRST_DIR_ONLY { "log-1.txt", "log-2.txt", "log-3.txt", "log-5.txt" }
        , IMAGE_FILES_IN_FIRST_DIR_ONLY { "image1.png" }
        , JUNK_FILES_IN_FIRST_DIR_ONLY { ".DS_Store", "junk.htm", "junk.html", "sample.gif" }
        , ALL_FILES_IN_FIRST_DIR_ONLY { ".DS_Store", "image1.png", "junk.htm",
                                        "junk.html", "log-1.txt",  "log-2.txt",
                                        "log-3.txt", "log-5.txt",  "sample.gif" }
        , LOG_FILES_RECURSIVE { "log-1.txt", "log-2.txt", "log-3.txt", "log-4.txt",
                                "log-5.txt", "log-6.txt", "log-7.txt" }
        , IMAGE_FILES_RECURSIVE { "image1.png", "image2.png", "image3.png" }
        , ALL_VALID_FILES_RECURSIVE { IMAGE_FILES_RECURSIVE }
        , EMPTY_DIR_NAME_STR { "empty-directory" }
        , EMPTY_DIR_PATH_STR { CombinePaths(TEST_DIR_PATH_STR, EMPTY_DIR_NAME_STR) }
        , NON_EMPTY_DIR_NAME_STR { "non-empty-directory" }
        , NON_EMPTY_DIR_PATH_STR { CombinePaths(TEST_DIR_PATH_STR, NON_EMPTY_DIR_NAME_STR) }
        , EMPTY_VEC {}
        , TO_CREATE_SUB_DIR_PATHS_STR { "create-directory-test-1/create-directory-test-2/"
                                        "create-directory-test-3" }
        , TO_CREATE_DIR_FINAL_PATH_STR { CombinePaths(
              TEST_DIR_PATH_STR, TO_CREATE_SUB_DIR_PATHS_STR) }
    {}

    ~Help()
    {
        try
        {
            // remove anything that failed tests might have left behind
            const bfs::path DIR_TO_DELETE_PATH(
                CombinePaths(TEST_DIR_PATH_STR, "create-directory-test-1"));

            if (bfs::exists(DIR_TO_DELETE_PATH))
            {
                boost::uintmax_t deletedCount { 0 };
                std::string exceptionWhatStr;
                boost::system::error_code errorCode;

                try
                {
                    deletedCount = bfs::remove_all(DIR_TO_DELETE_PATH, errorCode);
                }
                catch (const std::exception & EXCEPTION)
                {
                    exceptionWhatStr = EXCEPTION.what();
                }

                if ((deletedCount == 0) || errorCode || (exceptionWhatStr.empty() == false))
                {
                    std::cerr
                        << "Error:" << __FILE__ << ":" << __LINE__
                        << ", misc::filesystem unit tests, the helper struct 'Help's destructor "
                           "found junk left over by a failed test and tried to "
                           "delete it but failed.  This will prevent future runs of the tests "
                           "from working and needs to be deleted.  Here are some details:  "
                        << "The junk left over was this directory=\"" << DIR_TO_DELETE_PATH.string()
                        << "\" (or maybe this \""
                        << bfs::weakly_canonical(DIR_TO_DELETE_PATH).string()
                        << "\")  (error_code=" << errorCode.value() << ", error_message=\""
                        << errorCode.message() << "\")  (exception.what()=\"" << exceptionWhatStr
                        << "\")";
                }
            }
        }
        catch (...)
        {
            std::cerr << "Error:" << __FILE__ << ":" << __LINE__
                      << "Help Destructor threw an exception!?" << std::endl;
        }
    }

    static const std::string PreferredSlashes(const std::string & PATH_STR_ORIG)
    {
        const std::string BLACK_SLASH_STR { "\\" };
        const std::string FORWARD_SLASH_STR { "/" };
        const auto PREFERRED_SEPARATOR_STR { ToString(bfs::path::preferred_separator) };

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

    static const std::string
        CombinePaths(const std::string & PARENT_PATH_STR, const std::string & CHILD_PATH_STR)
    {
        const auto PARENT_PATH { bfs::path(PARENT_PATH_STR) };
        const auto CHILD_PATH { bfs::path(CHILD_PATH_STR) };
        const auto COMBINED_PATH { (PARENT_PATH / CHILD_PATH) };
        const auto FINAL_PATH { bfs::weakly_canonical(COMBINED_PATH).make_preferred() };
        return FINAL_PATH.string();
    }

    static const std::vector<std::string>
        CombineVectors(const std::vector<std::string> & VECA, const std::vector<std::string> & VECB)
    {
        std::vector<std::string> combinedVec { VECA };
        combinedVec.insert(std::end(combinedVec), std::cbegin(VECB), std::cend(VECB));
        return combinedVec;
    }

    static const std::vector<std::string>
        KeepOnlyFilenameAndStdSort(const std::vector<std::string> & PATH_VEC)
    {
        std::vector<std::string> filenameVec;
        for (const auto & PATH_STR : PATH_VEC)
        {
            filenameVec.emplace_back(filesystem::Filename(PATH_STR));
        }

        std::sort(std::begin(filenameVec), std::end(filenameVec));
        return filenameVec;
    }

    static void
        coutVectorOfStrings(const std::vector<std::string> & VEC, const std::string & TITLE = "")
    {
        std::cout << TITLE << ":";

        for (const auto & STRING : VEC)
        {
            std::cout << "\n\t\"" << STRING << "\"";
        }

        std::cout << std::endl << std::endl;
    }

    const std::string JUNK_WHITESPACE_STR;
    const std::string TEST_DIR_NAME_STR;
    const std::string TEST_DIR_PATH_STR;
    const std::string STRANGE_DIR_NAME_STR;
    const std::string STRANGE_DIR_PATH_STR;
    const std::vector<std::string> STRANGE_FILE_STRINGS;
    const std::vector<std::string> STRANGE_DIRECTORY_STRINGS;
    const std::vector<std::string> ALL_STRANGE_STRINGS;
    const std::vector<std::string> LOG_FILES_IN_FIRST_DIR_ONLY;
    const std::vector<std::string> IMAGE_FILES_IN_FIRST_DIR_ONLY;
    const std::vector<std::string> JUNK_FILES_IN_FIRST_DIR_ONLY;
    const std::vector<std::string> ALL_FILES_IN_FIRST_DIR_ONLY;
    const std::vector<std::string> LOG_FILES_RECURSIVE;
    const std::vector<std::string> IMAGE_FILES_RECURSIVE;
    const std::vector<std::string> ALL_VALID_FILES_RECURSIVE;
    const std::string EMPTY_DIR_NAME_STR;
    const std::string EMPTY_DIR_PATH_STR;
    const std::string NON_EMPTY_DIR_NAME_STR;
    const std::string NON_EMPTY_DIR_PATH_STR;
    const std::vector<std::string> EMPTY_VEC;
    const std::string TO_CREATE_SUB_DIR_PATHS_STR;
    const std::string TO_CREATE_DIR_FINAL_PATH_STR;
};

BOOST_AUTO_TEST_CASE(misc_filesystem__CleanPath_A)
{
    Help help;

    const bfs::path CURRENT_PATH { bfs::canonical(bfs::current_path()) };
    const std::string CURRENT_PATH_STR { CURRENT_PATH.string() };

    // empty case should always return empty
    BOOST_CHECK(filesystem::CleanPath("") == "");

    // whitespace is trimmed so paths with only whitespace should always return empty
    BOOST_CHECK(filesystem::CleanPath(" ") == "");
    BOOST_CHECK(filesystem::CleanPath("\t") == "");

    // names without directory separators that do not exist in the current dir should only have
    // slashes fixed
    BOOST_CHECK(filesystem::CleanPath("a") == "a");
    BOOST_CHECK(filesystem::CleanPath("ab") == "ab");
    BOOST_CHECK(filesystem::CleanPath("abc") == "abc");

    BOOST_CHECK(filesystem::CleanPath("a b") == "a b");
    BOOST_CHECK(filesystem::CleanPath("a b c") == "a b c");

    // names can have excessive dots
    BOOST_CHECK(filesystem::CleanPath(".a") == ".a");
    BOOST_CHECK(filesystem::CleanPath("..a") == "..a");
    BOOST_CHECK(filesystem::CleanPath("...a") == "...a");

    BOOST_CHECK(filesystem::CleanPath("a.") == "a.");
    BOOST_CHECK(filesystem::CleanPath("a..") == "a..");
    BOOST_CHECK(filesystem::CleanPath("a...") == "a...");

    BOOST_CHECK(filesystem::CleanPath(".a.") == ".a.");
    BOOST_CHECK(filesystem::CleanPath("..a..") == "..a..");
    BOOST_CHECK(filesystem::CleanPath("...a...") == "...a...");

    BOOST_CHECK(filesystem::CleanPath(".a ..b ...c d. e.. f...") == ".a ..b ...c d. e.. f...");

    BOOST_CHECK(filesystem::CleanPath(".a/..a/...a/a./a../a...") == ".a/..a/...a/a./a../a...");

    // boost::filesystem functions threw exceptions with this kind of garbage, but we don't need to
    // support it...so whatever.
    BOOST_CHECK(filesystem::CleanPath(". .") == ". .");
    BOOST_CHECK(filesystem::CleanPath(". . .") == ". . .");
    BOOST_CHECK(filesystem::CleanPath(".. ..") == ".. ..");
    BOOST_CHECK(filesystem::CleanPath(".. .. ..") == ".. .. ..");
    BOOST_CHECK(filesystem::CleanPath("... ...") == "... ...");
    BOOST_CHECK(filesystem::CleanPath("... ... ...") == "... ... ...");

    BOOST_CHECK(
        filesystem::CleanPath(
            "   This is a much longer and more screwy non-path string.123 -- - --- .\t")
        == "This is a much longer and more screwy non-path string.123 -- - --- .");

    // the valid dot directories should become relative to the current source dir
    BOOST_CHECK(filesystem::CleanPath(".") == help.PreferredSlashes(CURRENT_PATH_STR));

    BOOST_CHECK(
        filesystem::CleanPath("..")
        == bfs::canonical(CURRENT_PATH / bfs::path("..")).make_preferred().string());

    // paths that do not exist will still have slashes changed to what the os prefers and should
    // have redundant dot names resolved
    BOOST_CHECK(filesystem::CleanPath("a/b") == help.PreferredSlashes("a/b"));
    BOOST_CHECK(filesystem::CleanPath("a\\b") == help.PreferredSlashes("a/b"));

    BOOST_CHECK(filesystem::CleanPath("a/b/c") == help.PreferredSlashes("a/b/c"));
    BOOST_CHECK(filesystem::CleanPath("a/b\\c") == help.PreferredSlashes("a/b/c"));

    BOOST_CHECK(
        filesystem::CleanPath("a/./././b/../b/../b/c/../c/d/..") == help.PreferredSlashes("a/b/c"));

    BOOST_CHECK(filesystem::CleanPath("a/b/") == help.PreferredSlashes("a/b/"));

    // NOTE:  CleanPath() will remove the redundant trailing dots
    BOOST_CHECK(filesystem::CleanPath("a/b/.") == help.PreferredSlashes("a/b/"));

    // NOTE:  CleanPath("/a/b") results differ between Windows and everyone else...
    //  Windows="C:\a\b"
    //  Linux/MacOS="/a/b"

    // test windows drive letter paths
    // if the OS is not windows then these will be treated like all other non-existing paths and it
    // is important to note that behavior is the same, so the tests below will work the same on any
    // platform

    // boost filesystem functions don't handle paths that are just windows drive names follow by a
    // color when there is no trailing slash, so CleanPath() will add that trailing slash in these
    // cases, also note the case is preserved
    BOOST_CHECK(filesystem::CleanPath("c:") == Help::PreferredSlashes("c:\\"));
    BOOST_CHECK(filesystem::CleanPath("C:") == Help::PreferredSlashes("C:\\"));
    BOOST_CHECK(filesystem::CleanPath("c:\\") == Help::PreferredSlashes("c:\\"));
    BOOST_CHECK(filesystem::CleanPath("C:\\") == Help::PreferredSlashes("C:\\"));
    BOOST_CHECK(filesystem::CleanPath("c:/") == Help::PreferredSlashes("c:/"));
    BOOST_CHECK(filesystem::CleanPath("C:/") == Help::PreferredSlashes("C:/"));

    // test what you get from the root
#if defined(HEROESPATH_PLATFORM_DETECTED_IS_WINDOWS)
    BOOST_CHECK(filesystem::CleanPath("/") == "c:\\");
#else
    BOOST_CHECK(filesystem::CleanPath("/") == "/");
#endif
}

BOOST_AUTO_TEST_CASE(misc_filesystem__CurrentDirectory)
{
    BOOST_CHECK(filesystem::CurrentDirectory() == bfs::current_path().make_preferred().string());

    BOOST_CHECK(
        filesystem::CurrentDirectory()
        == bfs::canonical(bfs::current_path()).make_preferred().string());
}

BOOST_AUTO_TEST_CASE(misc_filesystem__CombinePathsThenClean)
{
    Help help;

    // empty or whitespace input that should always return empty
    BOOST_CHECK(filesystem::CombinePathsThenClean("", "") == "");
    BOOST_CHECK(filesystem::CombinePathsThenClean("", "", "", "") == "");

    BOOST_CHECK(
        filesystem::CombinePathsThenClean(
            help.JUNK_WHITESPACE_STR,
            help.JUNK_WHITESPACE_STR,
            help.JUNK_WHITESPACE_STR,
            help.JUNK_WHITESPACE_STR)
        == "");

    // test all non-existing input with whitespace junk
    BOOST_CHECK(
        filesystem::CombinePathsThenClean("a b", "c", ".d", "e.")
        == help.PreferredSlashes("a b/c/.d/e."));

    BOOST_CHECK(
        filesystem::CombinePathsThenClean(
            "a b", help.JUNK_WHITESPACE_STR, ".d", help.JUNK_WHITESPACE_STR)
        == help.PreferredSlashes("a b/.d"));

    BOOST_CHECK(
        filesystem::CombinePathsThenClean(
            help.JUNK_WHITESPACE_STR, "c", help.JUNK_WHITESPACE_STR, "e.")
        == help.PreferredSlashes("c/e."));

    // test existing

    // NOTE:  CombinePathsThenClean() skips single dot paths
    BOOST_CHECK(filesystem::CombinePathsThenClean(".", ".", ".", ".") == "");

    BOOST_CHECK(
        filesystem::CombinePathsThenClean(".", "..", ".", "../../..")
        == help.CombinePaths("..", "../../.."));

    BOOST_CHECK(
        filesystem::CombinePathsThenClean("../../..", ".", "..", ".")
        == help.CombinePaths(".", "../../../.."));

    BOOST_CHECK(
        filesystem::CombinePathsThenClean(help.TEST_DIR_NAME_STR, "") == help.TEST_DIR_PATH_STR);
    BOOST_CHECK(
        filesystem::CombinePathsThenClean("", help.TEST_DIR_NAME_STR) == help.TEST_DIR_PATH_STR);

    BOOST_CHECK(
        filesystem::CombinePathsThenClean(help.TEST_DIR_NAME_STR, ".") == help.TEST_DIR_PATH_STR);
    BOOST_CHECK(
        filesystem::CombinePathsThenClean(".", help.TEST_DIR_NAME_STR) == help.TEST_DIR_PATH_STR);

    BOOST_CHECK(
        filesystem::CombinePathsThenClean(help.TEST_DIR_NAME_STR, "..")
        == filesystem::CurrentDirectory());

    BOOST_CHECK(
        filesystem::CombinePathsThenClean("..", help.TEST_DIR_NAME_STR)
        == help.CombinePaths("..", help.TEST_DIR_NAME_STR));

    BOOST_CHECK(
        filesystem::CombinePathsThenClean(".", help.TEST_DIR_NAME_STR, "../../..")
        == help.CombinePaths(help.TEST_DIR_NAME_STR, "../../.."));

    BOOST_CHECK(
        filesystem::CombinePathsThenClean("../../..", help.TEST_DIR_NAME_STR, ".")
        == help.CombinePaths("../../..", help.TEST_DIR_NAME_STR));

    // test repeated existing
    BOOST_CHECK(
        filesystem::CombinePathsThenClean(
            help.TEST_DIR_NAME_STR,
            help.TEST_DIR_NAME_STR,
            help.TEST_DIR_NAME_STR,
            help.TEST_DIR_NAME_STR)
        == help.PreferredSlashes(
               filesystem::CurrentDirectory() + "/" + help.TEST_DIR_NAME_STR + "/"
               + help.TEST_DIR_NAME_STR + "/" + help.TEST_DIR_NAME_STR + "/"
               + help.TEST_DIR_NAME_STR));

    // test mixed cases
    BOOST_CHECK(
        filesystem::CombinePathsThenClean(
            "a", help.TEST_DIR_NAME_STR, help.TEST_DIR_NAME_STR, help.TEST_DIR_NAME_STR)
        == help.PreferredSlashes(
               std::string("a") + "/" + help.TEST_DIR_NAME_STR + "/" + help.TEST_DIR_NAME_STR + "/"
               + help.TEST_DIR_NAME_STR));
}

BOOST_AUTO_TEST_CASE(misc_filesystem__AppendPathsToCurrentThenClean)
{
    Help help;

    // empty or whitespace input that should always return empty
    BOOST_CHECK(
        filesystem::AppendPathsToCurrentThenClean("", "") == filesystem::CurrentDirectory());

    BOOST_CHECK(
        filesystem::AppendPathsToCurrentThenClean("", "", "") == filesystem::CurrentDirectory());

    BOOST_CHECK(
        filesystem::AppendPathsToCurrentThenClean(
            help.JUNK_WHITESPACE_STR, help.JUNK_WHITESPACE_STR, help.JUNK_WHITESPACE_STR)
        == filesystem::CurrentDirectory());

    // test all non-existing input with whitespace junk
    BOOST_CHECK(
        filesystem::AppendPathsToCurrentThenClean("a b", "c", ".d")
        == help.PreferredSlashes(filesystem::CurrentDirectory() + "/a b/c/.d"));

    BOOST_CHECK(
        filesystem::AppendPathsToCurrentThenClean("a b", help.JUNK_WHITESPACE_STR, ".d")
        == help.PreferredSlashes(filesystem::CurrentDirectory() + "/a b/.d"));

    BOOST_CHECK(
        filesystem::AppendPathsToCurrentThenClean(
            help.JUNK_WHITESPACE_STR, "c", help.JUNK_WHITESPACE_STR)
        == help.PreferredSlashes(filesystem::CurrentDirectory() + "/c"));

    // test existing
    BOOST_CHECK(
        filesystem::AppendPathsToCurrentThenClean(".", ".", ".") == filesystem::CurrentDirectory());

    BOOST_CHECK(
        filesystem::AppendPathsToCurrentThenClean("..", ".", "../../..")
        == help.CombinePaths(".", "../../../.."));

    BOOST_CHECK(
        filesystem::AppendPathsToCurrentThenClean("../../..", ".", "..")
        == help.CombinePaths(".", "../../../.."));

    BOOST_CHECK(
        filesystem::AppendPathsToCurrentThenClean(help.TEST_DIR_NAME_STR, "")
        == help.TEST_DIR_PATH_STR);

    BOOST_CHECK(
        filesystem::AppendPathsToCurrentThenClean("", help.TEST_DIR_NAME_STR)
        == help.TEST_DIR_PATH_STR);

    BOOST_CHECK(
        filesystem::AppendPathsToCurrentThenClean(help.TEST_DIR_NAME_STR, ".")
        == help.TEST_DIR_PATH_STR);

    BOOST_CHECK(
        filesystem::AppendPathsToCurrentThenClean(".", help.TEST_DIR_NAME_STR)
        == help.TEST_DIR_PATH_STR);

    BOOST_CHECK(
        filesystem::AppendPathsToCurrentThenClean(help.TEST_DIR_NAME_STR, "..")
        == filesystem::CurrentDirectory());

    BOOST_CHECK(
        filesystem::AppendPathsToCurrentThenClean("..", help.TEST_DIR_NAME_STR)
        == help.CombinePaths("..", help.TEST_DIR_NAME_STR));

    BOOST_CHECK(
        filesystem::AppendPathsToCurrentThenClean(".", help.TEST_DIR_NAME_STR, "../../..")
        == help.CombinePaths(help.TEST_DIR_NAME_STR, "../../.."));

    BOOST_CHECK(
        filesystem::AppendPathsToCurrentThenClean("../../..", help.TEST_DIR_NAME_STR, ".")
        == help.CombinePaths("../../..", help.TEST_DIR_NAME_STR));

    // test repeated existing
    BOOST_CHECK(
        filesystem::AppendPathsToCurrentThenClean(
            help.TEST_DIR_NAME_STR, help.TEST_DIR_NAME_STR, help.TEST_DIR_NAME_STR)
        == help.PreferredSlashes(
               filesystem::CurrentDirectory() + "/" + help.TEST_DIR_NAME_STR + "/"
               + help.TEST_DIR_NAME_STR + "/" + help.TEST_DIR_NAME_STR));

    // test mixed cases
    BOOST_CHECK(
        filesystem::AppendPathsToCurrentThenClean(
            "a", help.TEST_DIR_NAME_STR, help.TEST_DIR_NAME_STR)
        == help.PreferredSlashes(
               filesystem::CurrentDirectory() + std::string("/a") + "/" + help.TEST_DIR_NAME_STR
               + "/" + help.TEST_DIR_NAME_STR));
}

BOOST_AUTO_TEST_CASE(misc_filesystem__StrangeThings)
{
    Help help;

    // test paths that do exist are made absolute
    BOOST_CHECK(filesystem::CleanPath(help.TEST_DIR_NAME_STR) == help.TEST_DIR_PATH_STR);
    BOOST_CHECK(filesystem::CleanPath(help.STRANGE_DIR_PATH_STR) == help.STRANGE_DIR_PATH_STR);

    for (const auto & STANGE_STR : help.ALL_STRANGE_STRINGS)
    {
        BOOST_CHECK(
            filesystem::CombinePathsThenClean(help.STRANGE_DIR_PATH_STR, STANGE_STR)
            == help.CombinePaths(help.STRANGE_DIR_PATH_STR, STANGE_STR));
    }
}

BOOST_AUTO_TEST_CASE(misc_filesystem__EndsWithFileGuess)
{
    Help help;

    BOOST_CHECK(filesystem::EndsWithFileGuess("") == false);
    BOOST_CHECK(filesystem::EndsWithFileGuess(help.JUNK_WHITESPACE_STR) == false);

    BOOST_CHECK(filesystem::EndsWithFileGuess("abc") == true);

    BOOST_CHECK(filesystem::EndsWithFileGuess(".") == false);
    BOOST_CHECK(filesystem::EndsWithFileGuess("..") == false);
    BOOST_CHECK(filesystem::EndsWithFileGuess("c:") == false);
    BOOST_CHECK(filesystem::EndsWithFileGuess("c:\\") == false);
    BOOST_CHECK(filesystem::EndsWithFileGuess(help.TEST_DIR_NAME_STR) == false);
    BOOST_CHECK(filesystem::EndsWithFileGuess(help.TEST_DIR_PATH_STR) == false);

    BOOST_CHECK(filesystem::EndsWithFileGuess("bar") == true);
    BOOST_CHECK(filesystem::EndsWithFileGuess(".bar") == true);
    BOOST_CHECK(filesystem::EndsWithFileGuess("bar.") == true);
    BOOST_CHECK(filesystem::EndsWithFileGuess("bar.txt") == true);

    BOOST_CHECK(filesystem::EndsWithFileGuess("bar/") == false);
    BOOST_CHECK(filesystem::EndsWithFileGuess(".bar/") == false);
    BOOST_CHECK(filesystem::EndsWithFileGuess("bar./") == false);
    BOOST_CHECK(filesystem::EndsWithFileGuess("bar.txt/") == false);

    BOOST_CHECK(filesystem::EndsWithFileGuess("bar/.") == false);
    BOOST_CHECK(filesystem::EndsWithFileGuess(".bar/.") == false);
    BOOST_CHECK(filesystem::EndsWithFileGuess("bar./.") == false);
    BOOST_CHECK(filesystem::EndsWithFileGuess("bar.txt/.") == false);

    BOOST_CHECK(filesystem::EndsWithFileGuess("bar/..") == false);
    BOOST_CHECK(filesystem::EndsWithFileGuess(".bar/..") == false);
    BOOST_CHECK(filesystem::EndsWithFileGuess("bar./..") == false);
    BOOST_CHECK(filesystem::EndsWithFileGuess("bar.txt/..") == false);

    BOOST_CHECK(filesystem::EndsWithFileGuess("bar\\") == false);
    BOOST_CHECK(filesystem::EndsWithFileGuess(".bar\\") == false);
    BOOST_CHECK(filesystem::EndsWithFileGuess("bar.\\") == false);
    BOOST_CHECK(filesystem::EndsWithFileGuess("bar.txt\\") == false);

    BOOST_CHECK(filesystem::EndsWithFileGuess("bar\\.") == false);
    BOOST_CHECK(filesystem::EndsWithFileGuess(".bar\\.") == false);
    BOOST_CHECK(filesystem::EndsWithFileGuess("bar.\\.") == false);
    BOOST_CHECK(filesystem::EndsWithFileGuess("bar.txt\\.") == false);

    BOOST_CHECK(filesystem::EndsWithFileGuess("bar\\..") == false);
    BOOST_CHECK(filesystem::EndsWithFileGuess(".bar\\..") == false);
    BOOST_CHECK(filesystem::EndsWithFileGuess("bar.\\..") == false);
    BOOST_CHECK(filesystem::EndsWithFileGuess("bar.txt\\..") == false);

    //

    BOOST_CHECK(filesystem::EndsWithFileGuess("foo/bar") == true);
    BOOST_CHECK(filesystem::EndsWithFileGuess("foo/.bar") == true);
    BOOST_CHECK(filesystem::EndsWithFileGuess("foo/bar.") == true);
    BOOST_CHECK(filesystem::EndsWithFileGuess("foo/bar.txt") == true);

    BOOST_CHECK(filesystem::EndsWithFileGuess("foo/bar/") == false);
    BOOST_CHECK(filesystem::EndsWithFileGuess("foo/.bar/") == false);
    BOOST_CHECK(filesystem::EndsWithFileGuess("foo/bar./") == false);
    BOOST_CHECK(filesystem::EndsWithFileGuess("foo/bar.txt/") == false);

    BOOST_CHECK(filesystem::EndsWithFileGuess("foo/bar/.") == false);
    BOOST_CHECK(filesystem::EndsWithFileGuess("foo/.bar/.") == false);
    BOOST_CHECK(filesystem::EndsWithFileGuess("foo/bar./.") == false);
    BOOST_CHECK(filesystem::EndsWithFileGuess("foo/bar.txt/.") == false);

    BOOST_CHECK(filesystem::EndsWithFileGuess("foo/bar/..") == false);
    BOOST_CHECK(filesystem::EndsWithFileGuess("foo/.bar/..") == false);
    BOOST_CHECK(filesystem::EndsWithFileGuess("foo/bar./..") == false);
    BOOST_CHECK(filesystem::EndsWithFileGuess("foo/bar.txt/..") == false);

    BOOST_CHECK(filesystem::EndsWithFileGuess("foo/bar\\") == false);
    BOOST_CHECK(filesystem::EndsWithFileGuess("foo/.bar\\") == false);
    BOOST_CHECK(filesystem::EndsWithFileGuess("foo/bar.\\") == false);
    BOOST_CHECK(filesystem::EndsWithFileGuess("foo/bar.txt\\") == false);

    BOOST_CHECK(filesystem::EndsWithFileGuess("foo/bar\\.") == false);
    BOOST_CHECK(filesystem::EndsWithFileGuess("foo/.bar\\.") == false);
    BOOST_CHECK(filesystem::EndsWithFileGuess("foo/bar.\\.") == false);
    BOOST_CHECK(filesystem::EndsWithFileGuess("foo/bar.txt\\.") == false);

    BOOST_CHECK(filesystem::EndsWithFileGuess("foo/bar\\..") == false);
    BOOST_CHECK(filesystem::EndsWithFileGuess("foo/.bar\\..") == false);
    BOOST_CHECK(filesystem::EndsWithFileGuess("foo/bar.\\..") == false);
    BOOST_CHECK(filesystem::EndsWithFileGuess("foo/bar.txt\\..") == false);

    for (const auto & STANGE_STR : help.STRANGE_FILE_STRINGS)
    {
        BOOST_CHECK(
            filesystem::EndsWithFileGuess(
                filesystem::CombinePathsThenClean(help.STRANGE_DIR_PATH_STR, STANGE_STR))
            == true);
    }

    for (const auto & STANGE_STR : help.STRANGE_DIRECTORY_STRINGS)
    {
        BOOST_CHECK(
            filesystem::EndsWithFileGuess(
                filesystem::CombinePathsThenClean(help.STRANGE_DIR_PATH_STR, STANGE_STR))
            == false);
    }
}

BOOST_AUTO_TEST_CASE(misc_filesystem__EndsWithDirectoryGuess)
{
    Help help;

    BOOST_CHECK(filesystem::EndsWithDirectoryGuess("") == false);
    BOOST_CHECK(filesystem::EndsWithDirectoryGuess(help.JUNK_WHITESPACE_STR) == false);

    BOOST_CHECK(filesystem::EndsWithDirectoryGuess("abc") == false);

    BOOST_CHECK(filesystem::EndsWithDirectoryGuess(".") == true);
    BOOST_CHECK(filesystem::EndsWithDirectoryGuess("..") == true);
    BOOST_CHECK(filesystem::EndsWithDirectoryGuess("c:") == true);
    BOOST_CHECK(filesystem::EndsWithDirectoryGuess("c:\\") == true);
    BOOST_CHECK(filesystem::EndsWithDirectoryGuess(help.TEST_DIR_NAME_STR) == true);
    BOOST_CHECK(filesystem::EndsWithDirectoryGuess(help.TEST_DIR_PATH_STR) == true);

    BOOST_CHECK(filesystem::EndsWithDirectoryGuess("bar") == false);
    BOOST_CHECK(filesystem::EndsWithDirectoryGuess(".bar") == false);
    BOOST_CHECK(filesystem::EndsWithDirectoryGuess("bar.") == false);
    BOOST_CHECK(filesystem::EndsWithDirectoryGuess("bar.txt") == false);

    BOOST_CHECK(filesystem::EndsWithDirectoryGuess("bar/") == true);
    BOOST_CHECK(filesystem::EndsWithDirectoryGuess(".bar/") == true);
    BOOST_CHECK(filesystem::EndsWithDirectoryGuess("bar./") == true);
    BOOST_CHECK(filesystem::EndsWithDirectoryGuess("bar.txt/") == true);

    BOOST_CHECK(filesystem::EndsWithDirectoryGuess("bar/.") == true);
    BOOST_CHECK(filesystem::EndsWithDirectoryGuess(".bar/.") == true);
    BOOST_CHECK(filesystem::EndsWithDirectoryGuess("bar./.") == true);
    BOOST_CHECK(filesystem::EndsWithDirectoryGuess("bar.txt/.") == true);

    BOOST_CHECK(filesystem::EndsWithDirectoryGuess("bar/..") == true);
    BOOST_CHECK(filesystem::EndsWithDirectoryGuess(".bar/..") == true);
    BOOST_CHECK(filesystem::EndsWithDirectoryGuess("bar./..") == true);
    BOOST_CHECK(filesystem::EndsWithDirectoryGuess("bar.txt/..") == true);

    BOOST_CHECK(filesystem::EndsWithDirectoryGuess("bar\\") == true);
    BOOST_CHECK(filesystem::EndsWithDirectoryGuess(".bar\\") == true);
    BOOST_CHECK(filesystem::EndsWithDirectoryGuess("bar.\\") == true);
    BOOST_CHECK(filesystem::EndsWithDirectoryGuess("bar.txt\\") == true);

    BOOST_CHECK(filesystem::EndsWithDirectoryGuess("bar\\.") == true);
    BOOST_CHECK(filesystem::EndsWithDirectoryGuess(".bar\\.") == true);
    BOOST_CHECK(filesystem::EndsWithDirectoryGuess("bar.\\.") == true);
    BOOST_CHECK(filesystem::EndsWithDirectoryGuess("bar.txt\\.") == true);

    BOOST_CHECK(filesystem::EndsWithDirectoryGuess("bar\\..") == true);
    BOOST_CHECK(filesystem::EndsWithDirectoryGuess(".bar\\..") == true);
    BOOST_CHECK(filesystem::EndsWithDirectoryGuess("bar.\\..") == true);
    BOOST_CHECK(filesystem::EndsWithDirectoryGuess("bar.txt\\..") == true);

    //

    BOOST_CHECK(filesystem::EndsWithDirectoryGuess("foo/bar") == false);
    BOOST_CHECK(filesystem::EndsWithDirectoryGuess("foo/.bar") == false);
    BOOST_CHECK(filesystem::EndsWithDirectoryGuess("foo/bar.") == false);
    BOOST_CHECK(filesystem::EndsWithDirectoryGuess("foo/bar.txt") == false);

    BOOST_CHECK(filesystem::EndsWithDirectoryGuess("foo/bar/") == true);
    BOOST_CHECK(filesystem::EndsWithDirectoryGuess("foo/.bar/") == true);
    BOOST_CHECK(filesystem::EndsWithDirectoryGuess("foo/bar./") == true);
    BOOST_CHECK(filesystem::EndsWithDirectoryGuess("foo/bar.txt/") == true);

    BOOST_CHECK(filesystem::EndsWithDirectoryGuess("foo/bar/.") == true);
    BOOST_CHECK(filesystem::EndsWithDirectoryGuess("foo/.bar/.") == true);
    BOOST_CHECK(filesystem::EndsWithDirectoryGuess("foo/bar./.") == true);
    BOOST_CHECK(filesystem::EndsWithDirectoryGuess("foo/bar.txt/.") == true);

    BOOST_CHECK(filesystem::EndsWithDirectoryGuess("foo/bar/..") == true);
    BOOST_CHECK(filesystem::EndsWithDirectoryGuess("foo/.bar/..") == true);
    BOOST_CHECK(filesystem::EndsWithDirectoryGuess("foo/bar./..") == true);
    BOOST_CHECK(filesystem::EndsWithDirectoryGuess("foo/bar.txt/..") == true);

    BOOST_CHECK(filesystem::EndsWithDirectoryGuess("foo/bar\\") == true);
    BOOST_CHECK(filesystem::EndsWithDirectoryGuess("foo/.bar\\") == true);
    BOOST_CHECK(filesystem::EndsWithDirectoryGuess("foo/bar.\\") == true);
    BOOST_CHECK(filesystem::EndsWithDirectoryGuess("foo/bar.txt\\") == true);

    BOOST_CHECK(filesystem::EndsWithDirectoryGuess("foo/bar\\.") == true);
    BOOST_CHECK(filesystem::EndsWithDirectoryGuess("foo/.bar\\.") == true);
    BOOST_CHECK(filesystem::EndsWithDirectoryGuess("foo/bar.\\.") == true);
    BOOST_CHECK(filesystem::EndsWithDirectoryGuess("foo/bar.txt\\.") == true);

    BOOST_CHECK(filesystem::EndsWithDirectoryGuess("foo/bar\\..") == true);
    BOOST_CHECK(filesystem::EndsWithDirectoryGuess("foo/.bar\\..") == true);
    BOOST_CHECK(filesystem::EndsWithDirectoryGuess("foo/bar.\\..") == true);
    BOOST_CHECK(filesystem::EndsWithDirectoryGuess("foo/bar.txt\\..") == true);

    for (const auto & STANGE_STR : help.STRANGE_FILE_STRINGS)
    {
        BOOST_CHECK(
            filesystem::EndsWithDirectoryGuess(
                filesystem::CombinePathsThenClean(help.STRANGE_DIR_PATH_STR, STANGE_STR))
            == false);
    }

    for (const auto & STANGE_STR : help.STRANGE_DIRECTORY_STRINGS)
    {
        BOOST_CHECK(
            filesystem::EndsWithDirectoryGuess(
                filesystem::CombinePathsThenClean(help.STRANGE_DIR_PATH_STR, STANGE_STR))
            == true);
    }
}

BOOST_AUTO_TEST_CASE(misc_filesystem__Filename_INCLUDING_EXTENSION)
{
    Help help;

    BOOST_CHECK(filesystem::Filename("") == "");
    BOOST_CHECK(filesystem::Filename(help.JUNK_WHITESPACE_STR) == "");

    BOOST_CHECK(filesystem::Filename("abc") == "abc");

    BOOST_CHECK(filesystem::Filename(".") == "");
    BOOST_CHECK(filesystem::Filename("..") == "");
    BOOST_CHECK(filesystem::Filename("c:") == "");
    BOOST_CHECK(filesystem::Filename("c:\\") == "");
    BOOST_CHECK(filesystem::Filename(help.TEST_DIR_NAME_STR) == "");
    BOOST_CHECK(filesystem::Filename(help.TEST_DIR_PATH_STR) == "");

    BOOST_CHECK(filesystem::Filename("bar") == "bar");
    BOOST_CHECK(filesystem::Filename(".bar") == ".bar");
    BOOST_CHECK(filesystem::Filename("bar.") == "bar.");
    BOOST_CHECK(filesystem::Filename("bar.txt") == "bar.txt");

    BOOST_CHECK(filesystem::Filename("bar/") == "");
    BOOST_CHECK(filesystem::Filename(".bar/") == "");
    BOOST_CHECK(filesystem::Filename("bar./") == "");
    BOOST_CHECK(filesystem::Filename("bar.txt/") == "");

    BOOST_CHECK(filesystem::Filename("bar/.") == "");
    BOOST_CHECK(filesystem::Filename(".bar/.") == "");
    BOOST_CHECK(filesystem::Filename("bar./.") == "");
    BOOST_CHECK(filesystem::Filename("bar.txt/.") == "");

    BOOST_CHECK(filesystem::Filename("bar/..") == "");
    BOOST_CHECK(filesystem::Filename(".bar/..") == "");
    BOOST_CHECK(filesystem::Filename("bar./..") == "");
    BOOST_CHECK(filesystem::Filename("bar.txt/..") == "");

    BOOST_CHECK(filesystem::Filename("bar\\") == "");
    BOOST_CHECK(filesystem::Filename(".bar\\") == "");
    BOOST_CHECK(filesystem::Filename("bar.\\") == "");
    BOOST_CHECK(filesystem::Filename("bar.txt\\") == "");

    BOOST_CHECK(filesystem::Filename("bar\\.") == "");
    BOOST_CHECK(filesystem::Filename(".bar\\.") == "");
    BOOST_CHECK(filesystem::Filename("bar.\\.") == "");
    BOOST_CHECK(filesystem::Filename("bar.txt\\.") == "");

    BOOST_CHECK(filesystem::Filename("bar\\..") == "");
    BOOST_CHECK(filesystem::Filename(".bar\\..") == "");
    BOOST_CHECK(filesystem::Filename("bar.\\..") == "");
    BOOST_CHECK(filesystem::Filename("bar.txt\\..") == "");

    //

    BOOST_CHECK(filesystem::Filename("foo/bar") == "bar");
    BOOST_CHECK(filesystem::Filename("foo/.bar") == ".bar");
    BOOST_CHECK(filesystem::Filename("foo/bar.") == "bar.");
    BOOST_CHECK(filesystem::Filename("foo/bar.txt") == "bar.txt");

    BOOST_CHECK(filesystem::Filename("foo/bar/") == "");
    BOOST_CHECK(filesystem::Filename("foo/.bar/") == "");
    BOOST_CHECK(filesystem::Filename("foo/bar./") == "");
    BOOST_CHECK(filesystem::Filename("foo/bar.txt/") == "");

    BOOST_CHECK(filesystem::Filename("foo/bar/.") == "");
    BOOST_CHECK(filesystem::Filename("foo/.bar/.") == "");
    BOOST_CHECK(filesystem::Filename("foo/bar./.") == "");
    BOOST_CHECK(filesystem::Filename("foo/bar.txt/.") == "");

    BOOST_CHECK(filesystem::Filename("foo/bar/..") == "");
    BOOST_CHECK(filesystem::Filename("foo/.bar/..") == "");
    BOOST_CHECK(filesystem::Filename("foo/bar./..") == "");
    BOOST_CHECK(filesystem::Filename("foo/bar.txt/..") == "");

    BOOST_CHECK(filesystem::Filename("foo/bar\\") == "");
    BOOST_CHECK(filesystem::Filename("foo/.bar\\") == "");
    BOOST_CHECK(filesystem::Filename("foo/bar.\\") == "");
    BOOST_CHECK(filesystem::Filename("foo/bar.txt\\") == "");

    BOOST_CHECK(filesystem::Filename("foo/bar\\.") == "");
    BOOST_CHECK(filesystem::Filename("foo/.bar\\.") == "");
    BOOST_CHECK(filesystem::Filename("foo/bar.\\.") == "");
    BOOST_CHECK(filesystem::Filename("foo/bar.txt\\.") == "");

    BOOST_CHECK(filesystem::Filename("foo/bar\\..") == "");
    BOOST_CHECK(filesystem::Filename("foo/.bar\\..") == "");
    BOOST_CHECK(filesystem::Filename("foo/bar.\\..") == "");
    BOOST_CHECK(filesystem::Filename("foo/bar.txt\\..") == "");

    for (const auto & STANGE_STR : help.STRANGE_FILE_STRINGS)
    {
        BOOST_CHECK(
            filesystem::Filename(
                filesystem::CombinePathsThenClean(help.STRANGE_DIR_PATH_STR, STANGE_STR))
            == STANGE_STR);
    }

    for (const auto & STANGE_STR : help.STRANGE_DIRECTORY_STRINGS)
    {
        BOOST_CHECK(
            filesystem::Filename(
                filesystem::CombinePathsThenClean(help.STRANGE_DIR_PATH_STR, STANGE_STR))
            == "");
    }
}

BOOST_AUTO_TEST_CASE(misc_filesystem__Filename_EXLUDING_EXTENSION)
{
    Help help;

    BOOST_CHECK(filesystem::Filename("", true) == "");
    BOOST_CHECK(filesystem::Filename(help.JUNK_WHITESPACE_STR, true) == "");

    BOOST_CHECK(filesystem::Filename("abc", true) == "abc");

    BOOST_CHECK(filesystem::Filename(".", true) == "");
    BOOST_CHECK(filesystem::Filename("..", true) == "");
    BOOST_CHECK(filesystem::Filename("c:", true) == "");
    BOOST_CHECK(filesystem::Filename("c:\\", true) == "");
    BOOST_CHECK(filesystem::Filename(help.TEST_DIR_NAME_STR, true) == "");
    BOOST_CHECK(filesystem::Filename(help.TEST_DIR_PATH_STR, true) == "");

    BOOST_CHECK(filesystem::Filename("bar", true) == "bar");
    BOOST_CHECK(filesystem::Filename(".bar", true) == ""); // ***
    BOOST_CHECK(filesystem::Filename("bar.", true) == "bar"); // ***
    BOOST_CHECK(filesystem::Filename("bar.txt", true) == "bar"); //***

    BOOST_CHECK(filesystem::Filename("bar/", true) == "");
    BOOST_CHECK(filesystem::Filename(".bar/", true) == "");
    BOOST_CHECK(filesystem::Filename("bar./", true) == "");
    BOOST_CHECK(filesystem::Filename("bar.txt/", true) == "");

    BOOST_CHECK(filesystem::Filename("bar/.", true) == "");
    BOOST_CHECK(filesystem::Filename(".bar/.", true) == "");
    BOOST_CHECK(filesystem::Filename("bar./.", true) == "");
    BOOST_CHECK(filesystem::Filename("bar.txt/.", true) == "");

    BOOST_CHECK(filesystem::Filename("bar/..", true) == "");
    BOOST_CHECK(filesystem::Filename(".bar/..", true) == "");
    BOOST_CHECK(filesystem::Filename("bar./..", true) == "");
    BOOST_CHECK(filesystem::Filename("bar.txt/..", true) == "");

    BOOST_CHECK(filesystem::Filename("bar\\", true) == "");
    BOOST_CHECK(filesystem::Filename(".bar\\", true) == "");
    BOOST_CHECK(filesystem::Filename("bar.\\", true) == "");
    BOOST_CHECK(filesystem::Filename("bar.txt\\", true) == "");

    BOOST_CHECK(filesystem::Filename("bar\\.", true) == "");
    BOOST_CHECK(filesystem::Filename(".bar\\.", true) == "");
    BOOST_CHECK(filesystem::Filename("bar.\\.", true) == "");
    BOOST_CHECK(filesystem::Filename("bar.txt\\.", true) == "");

    BOOST_CHECK(filesystem::Filename("bar\\..", true) == "");
    BOOST_CHECK(filesystem::Filename(".bar\\..", true) == "");
    BOOST_CHECK(filesystem::Filename("bar.\\..", true) == "");
    BOOST_CHECK(filesystem::Filename("bar.txt\\..", true) == "");

    //

    BOOST_CHECK(filesystem::Filename("foo/bar", true) == "bar");
    BOOST_CHECK(filesystem::Filename("foo/.bar", true) == ""); // ***
    BOOST_CHECK(filesystem::Filename("foo/bar.", true) == "bar"); // ***
    BOOST_CHECK(filesystem::Filename("foo/bar.txt", true) == "bar"); // ***

    BOOST_CHECK(filesystem::Filename("foo/bar/", true) == "");
    BOOST_CHECK(filesystem::Filename("foo/.bar/", true) == "");
    BOOST_CHECK(filesystem::Filename("foo/bar./", true) == "");
    BOOST_CHECK(filesystem::Filename("foo/bar.txt/", true) == "");

    BOOST_CHECK(filesystem::Filename("foo/bar/.", true) == "");
    BOOST_CHECK(filesystem::Filename("foo/.bar/.", true) == "");
    BOOST_CHECK(filesystem::Filename("foo/bar./.", true) == "");
    BOOST_CHECK(filesystem::Filename("foo/bar.txt/.", true) == "");

    BOOST_CHECK(filesystem::Filename("foo/bar/..", true) == "");
    BOOST_CHECK(filesystem::Filename("foo/.bar/..", true) == "");
    BOOST_CHECK(filesystem::Filename("foo/bar./..", true) == "");
    BOOST_CHECK(filesystem::Filename("foo/bar.txt/..", true) == "");

    BOOST_CHECK(filesystem::Filename("foo/bar\\", true) == "");
    BOOST_CHECK(filesystem::Filename("foo/.bar\\", true) == "");
    BOOST_CHECK(filesystem::Filename("foo/bar.\\", true) == "");
    BOOST_CHECK(filesystem::Filename("foo/bar.txt\\", true) == "");

    BOOST_CHECK(filesystem::Filename("foo/bar\\.", true) == "");
    BOOST_CHECK(filesystem::Filename("foo/.bar\\.", true) == "");
    BOOST_CHECK(filesystem::Filename("foo/bar.\\.", true) == "");
    BOOST_CHECK(filesystem::Filename("foo/bar.txt\\.", true) == "");

    BOOST_CHECK(filesystem::Filename("foo/bar\\..", true) == "");
    BOOST_CHECK(filesystem::Filename("foo/.bar\\..", true) == "");
    BOOST_CHECK(filesystem::Filename("foo/bar.\\..", true) == "");
    BOOST_CHECK(filesystem::Filename("foo/bar.txt\\..", true) == "");

    BOOST_CHECK(
        filesystem::Filename(
            filesystem::CombinePathsThenClean(
                help.STRANGE_DIR_PATH_STR, ".file-that-starts-with-dot"),
            true)
        == "");

    BOOST_CHECK(
        filesystem::Filename(
            filesystem::CombinePathsThenClean(
                help.STRANGE_DIR_PATH_STR, "file-that-ends-with-dot."),
            true)
        == "file-that-ends-with-dot");

    BOOST_CHECK(
        filesystem::Filename(
            filesystem::CombinePathsThenClean(
                help.STRANGE_DIR_PATH_STR, "file-with-multiple-extensions.txt.png.pdf"),
            true)
        == "file-with-multiple-extensions");

    for (const auto & STANGE_STR : help.STRANGE_DIRECTORY_STRINGS)
    {
        BOOST_CHECK(
            filesystem::Filename(
                filesystem::CombinePathsThenClean(help.STRANGE_DIR_PATH_STR, STANGE_STR), true)
            == "");
    }
}

BOOST_AUTO_TEST_CASE(misc_filesystem__Extension_INCLUDING_DOT)
{
    Help help;

    BOOST_CHECK(filesystem::Extension("") == "");
    BOOST_CHECK(filesystem::Extension(help.JUNK_WHITESPACE_STR) == "");

    BOOST_CHECK(filesystem::Extension("abc") == "");

    BOOST_CHECK(filesystem::Extension(".") == "");
    BOOST_CHECK(filesystem::Extension("..") == "");
    BOOST_CHECK(filesystem::Extension("c:") == "");
    BOOST_CHECK(filesystem::Extension("c:\\") == "");
    BOOST_CHECK(filesystem::Extension(help.TEST_DIR_NAME_STR) == "");
    BOOST_CHECK(filesystem::Extension(help.TEST_DIR_PATH_STR) == "");

    BOOST_CHECK(filesystem::Extension("bar") == "");
    BOOST_CHECK(filesystem::Extension(".bar") == ".bar");
    BOOST_CHECK(filesystem::Extension("bar.") == ".");
    BOOST_CHECK(filesystem::Extension("bar.txt") == ".txt");

    BOOST_CHECK(filesystem::Extension("bar/") == "");
    BOOST_CHECK(filesystem::Extension(".bar/") == "");
    BOOST_CHECK(filesystem::Extension("bar./") == "");
    BOOST_CHECK(filesystem::Extension("bar.txt/") == "");

    BOOST_CHECK(filesystem::Extension("bar/.") == "");
    BOOST_CHECK(filesystem::Extension(".bar/.") == "");
    BOOST_CHECK(filesystem::Extension("bar./.") == "");
    BOOST_CHECK(filesystem::Extension("bar.txt/.") == "");

    BOOST_CHECK(filesystem::Extension("bar/..") == "");
    BOOST_CHECK(filesystem::Extension(".bar/..") == "");
    BOOST_CHECK(filesystem::Extension("bar./..") == "");
    BOOST_CHECK(filesystem::Extension("bar.txt/..") == "");

    BOOST_CHECK(filesystem::Extension("bar\\") == "");
    BOOST_CHECK(filesystem::Extension(".bar\\") == "");
    BOOST_CHECK(filesystem::Extension("bar.\\") == "");
    BOOST_CHECK(filesystem::Extension("bar.txt\\") == "");

    BOOST_CHECK(filesystem::Extension("bar\\.") == "");
    BOOST_CHECK(filesystem::Extension(".bar\\.") == "");
    BOOST_CHECK(filesystem::Extension("bar.\\.") == "");
    BOOST_CHECK(filesystem::Extension("bar.txt\\.") == "");

    BOOST_CHECK(filesystem::Extension("bar\\..") == "");
    BOOST_CHECK(filesystem::Extension(".bar\\..") == "");
    BOOST_CHECK(filesystem::Extension("bar.\\..") == "");
    BOOST_CHECK(filesystem::Extension("bar.txt\\..") == "");

    //

    BOOST_CHECK(filesystem::Extension("foo/bar") == "");
    BOOST_CHECK(filesystem::Extension("foo/.bar") == ".bar");
    BOOST_CHECK(filesystem::Extension("foo/bar.") == ".");
    BOOST_CHECK(filesystem::Extension("foo/bar.txt") == ".txt");

    BOOST_CHECK(filesystem::Extension("foo/bar/") == "");
    BOOST_CHECK(filesystem::Extension("foo/.bar/") == "");
    BOOST_CHECK(filesystem::Extension("foo/bar./") == "");
    BOOST_CHECK(filesystem::Extension("foo/bar.txt/") == "");

    BOOST_CHECK(filesystem::Extension("foo/bar/.") == "");
    BOOST_CHECK(filesystem::Extension("foo/.bar/.") == "");
    BOOST_CHECK(filesystem::Extension("foo/bar./.") == "");
    BOOST_CHECK(filesystem::Extension("foo/bar.txt/.") == "");

    BOOST_CHECK(filesystem::Extension("foo/bar/..") == "");
    BOOST_CHECK(filesystem::Extension("foo/.bar/..") == "");
    BOOST_CHECK(filesystem::Extension("foo/bar./..") == "");
    BOOST_CHECK(filesystem::Extension("foo/bar.txt/..") == "");

    BOOST_CHECK(filesystem::Extension("foo/bar\\") == "");
    BOOST_CHECK(filesystem::Extension("foo/.bar\\") == "");
    BOOST_CHECK(filesystem::Extension("foo/bar.\\") == "");
    BOOST_CHECK(filesystem::Extension("foo/bar.txt\\") == "");

    BOOST_CHECK(filesystem::Extension("foo/bar\\.") == "");
    BOOST_CHECK(filesystem::Extension("foo/.bar\\.") == "");
    BOOST_CHECK(filesystem::Extension("foo/bar.\\.") == "");
    BOOST_CHECK(filesystem::Extension("foo/bar.txt\\.") == "");

    BOOST_CHECK(filesystem::Extension("foo/bar\\..") == "");
    BOOST_CHECK(filesystem::Extension("foo/.bar\\..") == "");
    BOOST_CHECK(filesystem::Extension("foo/bar.\\..") == "");
    BOOST_CHECK(filesystem::Extension("foo/bar.txt\\..") == "");

    BOOST_CHECK(
        filesystem::Extension(filesystem::CombinePathsThenClean(
            help.STRANGE_DIR_PATH_STR, ".file-that-starts-with-dot"))
        == ".file-that-starts-with-dot");

    BOOST_CHECK(
        filesystem::Extension(filesystem::CombinePathsThenClean(
            help.STRANGE_DIR_PATH_STR, "file-that-ends-with-dot."))
        == ".");

    BOOST_CHECK(
        filesystem::Extension(filesystem::CombinePathsThenClean(
            help.STRANGE_DIR_PATH_STR, "file-with-multiple-extensions.txt.png.pdf"))
        == ".txt.png.pdf");

    for (const auto & STANGE_STR : help.STRANGE_DIRECTORY_STRINGS)
    {
        BOOST_CHECK(
            filesystem::Extension(
                filesystem::CombinePathsThenClean(help.STRANGE_DIR_PATH_STR, STANGE_STR))
            == "");
    }
}

BOOST_AUTO_TEST_CASE(misc_filesystem__Extension_EXCLUDING_DOT)
{
    Help help;

    BOOST_CHECK(filesystem::Extension("", true) == "");
    BOOST_CHECK(filesystem::Extension(help.JUNK_WHITESPACE_STR, true) == "");

    BOOST_CHECK(filesystem::Extension("abc", true) == "");

    BOOST_CHECK(filesystem::Extension(".", true) == "");
    BOOST_CHECK(filesystem::Extension("..", true) == "");
    BOOST_CHECK(filesystem::Extension("c:", true) == "");
    BOOST_CHECK(filesystem::Extension("c:\\", true) == "");
    BOOST_CHECK(filesystem::Extension(help.TEST_DIR_NAME_STR, true) == "");
    BOOST_CHECK(filesystem::Extension(help.TEST_DIR_PATH_STR, true) == "");

    BOOST_CHECK(filesystem::Extension("bar", true) == "");
    BOOST_CHECK(filesystem::Extension(".bar", true) == "bar"); // ***
    BOOST_CHECK(filesystem::Extension("bar.", true) == ""); // ***
    BOOST_CHECK(filesystem::Extension("bar.txt", true) == "txt"); // ***

    BOOST_CHECK(filesystem::Extension("bar/", true) == "");
    BOOST_CHECK(filesystem::Extension(".bar/", true) == "");
    BOOST_CHECK(filesystem::Extension("bar./", true) == "");
    BOOST_CHECK(filesystem::Extension("bar.txt/", true) == "");

    BOOST_CHECK(filesystem::Extension("bar/.", true) == "");
    BOOST_CHECK(filesystem::Extension(".bar/.", true) == "");
    BOOST_CHECK(filesystem::Extension("bar./.", true) == "");
    BOOST_CHECK(filesystem::Extension("bar.txt/.", true) == "");

    BOOST_CHECK(filesystem::Extension("bar/..", true) == "");
    BOOST_CHECK(filesystem::Extension(".bar/..", true) == "");
    BOOST_CHECK(filesystem::Extension("bar./..", true) == "");
    BOOST_CHECK(filesystem::Extension("bar.txt/..", true) == "");

    BOOST_CHECK(filesystem::Extension("bar\\", true) == "");
    BOOST_CHECK(filesystem::Extension(".bar\\", true) == "");
    BOOST_CHECK(filesystem::Extension("bar.\\", true) == "");
    BOOST_CHECK(filesystem::Extension("bar.txt\\", true) == "");

    BOOST_CHECK(filesystem::Extension("bar\\.", true) == "");
    BOOST_CHECK(filesystem::Extension(".bar\\.", true) == "");
    BOOST_CHECK(filesystem::Extension("bar.\\.", true) == "");
    BOOST_CHECK(filesystem::Extension("bar.txt\\.", true) == "");

    BOOST_CHECK(filesystem::Extension("bar\\..", true) == "");
    BOOST_CHECK(filesystem::Extension(".bar\\..", true) == "");
    BOOST_CHECK(filesystem::Extension("bar.\\..", true) == "");
    BOOST_CHECK(filesystem::Extension("bar.txt\\..", true) == "");

    //

    BOOST_CHECK(filesystem::Extension("foo/bar", true) == "");
    BOOST_CHECK(filesystem::Extension("foo/.bar", true) == "bar"); // ***
    BOOST_CHECK(filesystem::Extension("foo/bar.", true) == ""); // ***
    BOOST_CHECK(filesystem::Extension("foo/bar.txt", true) == "txt"); // ***

    BOOST_CHECK(filesystem::Extension("foo/bar/", true) == "");
    BOOST_CHECK(filesystem::Extension("foo/.bar/", true) == "");
    BOOST_CHECK(filesystem::Extension("foo/bar./", true) == "");
    BOOST_CHECK(filesystem::Extension("foo/bar.txt/", true) == "");

    BOOST_CHECK(filesystem::Extension("foo/bar/.", true) == "");
    BOOST_CHECK(filesystem::Extension("foo/.bar/.", true) == "");
    BOOST_CHECK(filesystem::Extension("foo/bar./.", true) == "");
    BOOST_CHECK(filesystem::Extension("foo/bar.txt/.", true) == "");

    BOOST_CHECK(filesystem::Extension("foo/bar/..", true) == "");
    BOOST_CHECK(filesystem::Extension("foo/.bar/..", true) == "");
    BOOST_CHECK(filesystem::Extension("foo/bar./..", true) == "");
    BOOST_CHECK(filesystem::Extension("foo/bar.txt/..", true) == "");

    BOOST_CHECK(filesystem::Extension("foo/bar\\", true) == "");
    BOOST_CHECK(filesystem::Extension("foo/.bar\\", true) == "");
    BOOST_CHECK(filesystem::Extension("foo/bar.\\", true) == "");
    BOOST_CHECK(filesystem::Extension("foo/bar.txt\\", true) == "");

    BOOST_CHECK(filesystem::Extension("foo/bar\\.", true) == "");
    BOOST_CHECK(filesystem::Extension("foo/.bar\\.", true) == "");
    BOOST_CHECK(filesystem::Extension("foo/bar.\\.", true) == "");
    BOOST_CHECK(filesystem::Extension("foo/bar.txt\\.", true) == "");

    BOOST_CHECK(filesystem::Extension("foo/bar\\..", true) == "");
    BOOST_CHECK(filesystem::Extension("foo/.bar\\..", true) == "");
    BOOST_CHECK(filesystem::Extension("foo/bar.\\..", true) == "");
    BOOST_CHECK(filesystem::Extension("foo/bar.txt\\..", true) == "");

    BOOST_CHECK(
        filesystem::Extension(
            filesystem::CombinePathsThenClean(
                help.STRANGE_DIR_PATH_STR, ".file-that-starts-with-dot"),
            true)
        == "file-that-starts-with-dot");

    BOOST_CHECK(
        filesystem::Extension(
            filesystem::CombinePathsThenClean(
                help.STRANGE_DIR_PATH_STR, "file-that-ends-with-dot."),
            true)
        == "");

    BOOST_CHECK(
        filesystem::Extension(
            filesystem::CombinePathsThenClean(
                help.STRANGE_DIR_PATH_STR, "file-with-multiple-extensions.txt.png.pdf"),
            true)
        == "txt.png.pdf");

    for (const auto & STANGE_STR : help.STRANGE_DIRECTORY_STRINGS)
    {
        BOOST_CHECK(
            filesystem::Extension(
                filesystem::CombinePathsThenClean(help.STRANGE_DIR_PATH_STR, STANGE_STR), true)
            == "");
    }
}

BOOST_AUTO_TEST_CASE(misc_filesystem__Exists)
{
    Help help;

    BOOST_CHECK(filesystem::Exists("") == false);
    BOOST_CHECK(filesystem::Exists(help.JUNK_WHITESPACE_STR) == false);

    BOOST_CHECK(filesystem::Exists("abc") == false);

    BOOST_CHECK(filesystem::Exists(".") == true);
    BOOST_CHECK(filesystem::Exists("..") == true);
    BOOST_CHECK(filesystem::Exists(help.TEST_DIR_NAME_STR) == true);
    BOOST_CHECK(filesystem::Exists(help.TEST_DIR_PATH_STR) == true);

    for (const auto & STANGE_STR : help.STRANGE_FILE_STRINGS)
    {
        BOOST_CHECK(
            filesystem::Exists(
                filesystem::CombinePathsThenClean(help.STRANGE_DIR_PATH_STR, STANGE_STR))
            == true);
    }

    for (const auto & STANGE_STR : help.STRANGE_DIRECTORY_STRINGS)
    {
        BOOST_CHECK(
            filesystem::Exists(
                filesystem::CombinePathsThenClean(help.STRANGE_DIR_PATH_STR, STANGE_STR))
            == true);
    }
}

BOOST_AUTO_TEST_CASE(misc_filesystem__ExistsAndIsFile)
{
    Help help;

    BOOST_CHECK(filesystem::ExistsAndIsFile("") == false);
    BOOST_CHECK(filesystem::ExistsAndIsFile(help.JUNK_WHITESPACE_STR) == false);

    BOOST_CHECK(filesystem::ExistsAndIsFile("abc") == false);

    BOOST_CHECK(filesystem::ExistsAndIsFile(".") == false);
    BOOST_CHECK(filesystem::ExistsAndIsFile("..") == false);
    BOOST_CHECK(filesystem::ExistsAndIsFile(help.TEST_DIR_NAME_STR) == false);
    BOOST_CHECK(filesystem::ExistsAndIsFile(help.TEST_DIR_PATH_STR) == false);

    for (const auto & STANGE_STR : help.STRANGE_FILE_STRINGS)
    {
        BOOST_CHECK(
            filesystem::ExistsAndIsFile(
                filesystem::CombinePathsThenClean(help.STRANGE_DIR_PATH_STR, STANGE_STR))
            == true);
    }

    for (const auto & STANGE_STR : help.STRANGE_DIRECTORY_STRINGS)
    {
        BOOST_CHECK(
            filesystem::ExistsAndIsFile(
                filesystem::CombinePathsThenClean(help.STRANGE_DIR_PATH_STR, STANGE_STR))
            == false);
    }
}

BOOST_AUTO_TEST_CASE(misc_filesystem__ExistsAndIsDirectory)
{
    Help help;

    BOOST_CHECK(filesystem::ExistsAndIsDirectory("") == false);
    BOOST_CHECK(filesystem::ExistsAndIsDirectory(help.JUNK_WHITESPACE_STR) == false);

    BOOST_CHECK(filesystem::ExistsAndIsDirectory("abc") == false);

    BOOST_CHECK(filesystem::ExistsAndIsDirectory(".") == true);
    BOOST_CHECK(filesystem::ExistsAndIsDirectory("..") == true);
    BOOST_CHECK(filesystem::ExistsAndIsDirectory("../../././../..") == true);
    BOOST_CHECK(filesystem::ExistsAndIsDirectory(help.TEST_DIR_NAME_STR) == true);
    BOOST_CHECK(filesystem::ExistsAndIsDirectory(help.TEST_DIR_PATH_STR) == true);

    for (const auto & STANGE_STR : help.STRANGE_FILE_STRINGS)
    {
        BOOST_CHECK(
            filesystem::ExistsAndIsDirectory(
                filesystem::CombinePathsThenClean(help.STRANGE_DIR_PATH_STR, STANGE_STR))
            == false);
    }

    for (const auto & STANGE_STR : help.STRANGE_DIRECTORY_STRINGS)
    {
        BOOST_CHECK(
            filesystem::ExistsAndIsDirectory(
                filesystem::CombinePathsThenClean(help.STRANGE_DIR_PATH_STR, STANGE_STR))
            == true);
    }
}

BOOST_AUTO_TEST_CASE(misc_filesystem__FindFiles)
{
    Help help;

    //
    BOOST_CHECK(
        help.KeepOnlyFilenameAndStdSort(filesystem::FindFiles(false, help.EMPTY_DIR_PATH_STR))
            .empty());

    BOOST_CHECK(
        help.KeepOnlyFilenameAndStdSort(filesystem::FindFiles(true, help.EMPTY_DIR_PATH_STR))
            .empty());

    BOOST_CHECK(
        help.KeepOnlyFilenameAndStdSort(filesystem::FindFiles(true, help.EMPTY_DIR_PATH_STR, "abc"))
            .empty());

    BOOST_CHECK(help.KeepOnlyFilenameAndStdSort(
                        filesystem::FindFiles(true, help.EMPTY_DIR_PATH_STR, "", ".abc"))
                    .empty());

    BOOST_CHECK(
        help.KeepOnlyFilenameAndStdSort(filesystem::FindFiles(
                                            false,
                                            help.EMPTY_DIR_PATH_STR,
                                            "",
                                            "",
                                            filesystem::COMMON_FILE_NAME_PARTS_TO_EXCLUDE_VEC_))
            .empty());

    //
    BOOST_CHECK(
        help.KeepOnlyFilenameAndStdSort(filesystem::FindFiles(false, help.NON_EMPTY_DIR_PATH_STR))
        == help.ALL_FILES_IN_FIRST_DIR_ONLY);

    BOOST_CHECK(
        help.KeepOnlyFilenameAndStdSort(
            filesystem::FindFiles(false, help.NON_EMPTY_DIR_PATH_STR, "log"))
        == help.LOG_FILES_IN_FIRST_DIR_ONLY);

    BOOST_CHECK(
        help.KeepOnlyFilenameAndStdSort(
            filesystem::FindFiles(false, help.NON_EMPTY_DIR_PATH_STR, "", ".txt"))
        == help.LOG_FILES_IN_FIRST_DIR_ONLY);

    BOOST_CHECK(
        help.KeepOnlyFilenameAndStdSort(
            filesystem::FindFiles(false, help.NON_EMPTY_DIR_PATH_STR, "image", ""))
        == help.IMAGE_FILES_IN_FIRST_DIR_ONLY);

    BOOST_CHECK(
        help.KeepOnlyFilenameAndStdSort(
            filesystem::FindFiles(false, help.NON_EMPTY_DIR_PATH_STR, "", ".png"))
        == help.IMAGE_FILES_IN_FIRST_DIR_ONLY);

    //
    BOOST_CHECK(
        help.KeepOnlyFilenameAndStdSort(
            filesystem::FindFiles(true, help.NON_EMPTY_DIR_PATH_STR, "log"))
        == help.LOG_FILES_RECURSIVE);

    BOOST_CHECK(
        help.KeepOnlyFilenameAndStdSort(
            filesystem::FindFiles(true, help.NON_EMPTY_DIR_PATH_STR, "", ".txt"))
        == help.LOG_FILES_RECURSIVE);

    BOOST_CHECK(
        help.KeepOnlyFilenameAndStdSort(
            filesystem::FindFiles(true, help.NON_EMPTY_DIR_PATH_STR, "image", ""))
        == help.IMAGE_FILES_RECURSIVE);

    BOOST_CHECK(
        help.KeepOnlyFilenameAndStdSort(
            filesystem::FindFiles(true, help.NON_EMPTY_DIR_PATH_STR, "", ".png"))
        == help.IMAGE_FILES_RECURSIVE);

    //
    BOOST_CHECK(
        help.KeepOnlyFilenameAndStdSort(filesystem::FindFiles(
            true,
            help.NON_EMPTY_DIR_PATH_STR,
            "",
            "",
            filesystem::COMMON_FILE_NAME_PARTS_TO_EXCLUDE_VEC_))
        == help.ALL_VALID_FILES_RECURSIVE);
}

BOOST_AUTO_TEST_CASE(misc_filesystem__FindFirstAvailableNumberedFilenamePath)
{
    Help help;

    BOOST_CHECK(
        filesystem::Filename(filesystem::FindFirstAvailableNumberedFilenamePath(
            help.NON_EMPTY_DIR_PATH_STR, "logger", ".txt", "_"))
        == "logger_1.txt");

    BOOST_CHECK(
        filesystem::Filename(filesystem::FindFirstAvailableNumberedFilenamePath(
            help.NON_EMPTY_DIR_PATH_STR, "log", ".txt"))
        == "log-4.txt");
}

BOOST_AUTO_TEST_CASE(misc_filesystem__SortByLastNumberInFilename)
{
    Help help;

    // empty cases
    BOOST_CHECK(filesystem::SortByLastNumberInFilename({}, false, false) == help.EMPTY_VEC);

    BOOST_CHECK(filesystem::SortByLastNumberInFilename({ "" }, true, false) == help.EMPTY_VEC);

    BOOST_CHECK(
        filesystem::SortByLastNumberInFilename({ "", "", "" }, false, true) == help.EMPTY_VEC);

    BOOST_CHECK(
        filesystem::SortByLastNumberInFilename(
            { help.JUNK_WHITESPACE_STR, "", help.JUNK_WHITESPACE_STR }, true, true)
        == help.EMPTY_VEC);

    const std::vector<std::string> NUMBERED_FILE_PATHS_NOT_SORTED
        = { "file-9-999-5.ext", "foo/file-1.ext",   "foo/bar/file-2.ext",
            "file-000.ext",     "file-09.ext",      "/foo99/bar99/file-4.ext",
            "file-666-.ext6",   "foo99/file-3.ext", "file-with-no-number.ext",
            "7file-.ext",       "8f8ile-8-8-8.ext8" };

    const std::vector<std::string> NUMBERED_FILE_PATHS_SORTED_MISSING_LAST
        = { "file-000.ext",
            "foo/file-1.ext",
            "foo/bar/file-2.ext",
            "foo99/file-3.ext",
            "/foo99/bar99/file-4.ext",
            "file-9-999-5.ext",
            "file-666-.ext6",
            "7file-.ext",
            "8f8ile-8-8-8.ext8",
            "file-09.ext",
            "file-with-no-number.ext" };

    const std::vector<std::string> NUMBERED_FILE_PATHS_SORTED_MISSING_FIRST
        = { "file-with-no-number.ext", "file-000.ext",     "foo/file-1.ext",
            "foo/bar/file-2.ext",      "foo99/file-3.ext", "/foo99/bar99/file-4.ext",
            "file-9-999-5.ext",        "file-666-.ext6",   "7file-.ext",
            "8f8ile-8-8-8.ext8",       "file-09.ext" };

    const std::vector<std::string> NUMBERED_FILE_PATHS_SORTED_MISSING_EXCLUDED
        = { "file-000.ext",
            "foo/file-1.ext",
            "foo/bar/file-2.ext",
            "foo99/file-3.ext",
            "/foo99/bar99/file-4.ext",
            "file-9-999-5.ext",
            "file-666-.ext6",
            "7file-.ext",
            "8f8ile-8-8-8.ext8",
            "file-09.ext" };

    BOOST_CHECK(
        filesystem::SortByLastNumberInFilename(NUMBERED_FILE_PATHS_NOT_SORTED, false, false)
        == NUMBERED_FILE_PATHS_SORTED_MISSING_LAST);

    BOOST_CHECK(
        filesystem::SortByLastNumberInFilename(NUMBERED_FILE_PATHS_NOT_SORTED, true, false)
        == NUMBERED_FILE_PATHS_SORTED_MISSING_FIRST);

    BOOST_CHECK(
        filesystem::SortByLastNumberInFilename(NUMBERED_FILE_PATHS_NOT_SORTED, false, true)
        == NUMBERED_FILE_PATHS_SORTED_MISSING_EXCLUDED);

    BOOST_CHECK(
        filesystem::SortByLastNumberInFilename(NUMBERED_FILE_PATHS_NOT_SORTED, true, true)
        == NUMBERED_FILE_PATHS_SORTED_MISSING_EXCLUDED);

    // actual example reading from filesystem
    {
        const auto LOG_FILES_FULL_PATHS_ORIG { filesystem::FindFiles(
            true, help.NON_EMPTY_DIR_PATH_STR, "log", ".txt") };

        const auto LOG_FILES_FULL_PATHS_SORTED { filesystem::SortByLastNumberInFilename(
            LOG_FILES_FULL_PATHS_ORIG) };

        std::vector<std::string> fileNameStrings;

        for (const auto & PATH_STR : LOG_FILES_FULL_PATHS_SORTED)
        {
            fileNameStrings.emplace_back(filesystem::Filename(PATH_STR));
        }

        BOOST_CHECK(fileNameStrings == help.LOG_FILES_RECURSIVE);
    }
}

BOOST_AUTO_TEST_CASE(misc_filesystem__LimitPathDept)
{
    const std::string PATH_STR { "one/two/three/file.ext" };

    // empty cases
    BOOST_CHECK(filesystem::LimitPathDept("", 0) == "");
    BOOST_CHECK(filesystem::LimitPathDept("", 1) == "");
    BOOST_CHECK(filesystem::LimitPathDept("", std::numeric_limits<std::size_t>::max()) == "");

    // removing from the beginning
    BOOST_CHECK(filesystem::LimitPathDept(PATH_STR, 0) == "");
    BOOST_CHECK(filesystem::LimitPathDept(PATH_STR, 1) == "file.ext");
    BOOST_CHECK(filesystem::LimitPathDept(PATH_STR, 2) == "three/file.ext");
    BOOST_CHECK(filesystem::LimitPathDept(PATH_STR, 3) == "two/three/file.ext");
    BOOST_CHECK(filesystem::LimitPathDept(PATH_STR, 4) == PATH_STR);
    BOOST_CHECK(filesystem::LimitPathDept(PATH_STR, 5) == PATH_STR);

    BOOST_CHECK(
        filesystem::LimitPathDept(PATH_STR, std::numeric_limits<std::size_t>::max()) == PATH_STR);

    // removing from the end
    BOOST_CHECK(filesystem::LimitPathDept(PATH_STR, 0, true) == "");
    BOOST_CHECK(filesystem::LimitPathDept(PATH_STR, 1, true) == "one");
    BOOST_CHECK(filesystem::LimitPathDept(PATH_STR, 2, true) == "one/two");
    BOOST_CHECK(filesystem::LimitPathDept(PATH_STR, 3, true) == "one/two/three");
    BOOST_CHECK(filesystem::LimitPathDept(PATH_STR, 4, true) == PATH_STR);
    BOOST_CHECK(filesystem::LimitPathDept(PATH_STR, 5, true) == PATH_STR);

    BOOST_CHECK(
        filesystem::LimitPathDept(PATH_STR, std::numeric_limits<std::size_t>::max()) == PATH_STR);
}

BOOST_AUTO_TEST_CASE(misc_filesystem__CreateDirectory_and_Delete)
{
    // this block is only to force ~Help() that will cleanup any mess left by these tests
    {
        Help help;

        // empty cases that should fail
        BOOST_CHECK(filesystem::CreateDirectory("") == false);
        BOOST_CHECK(filesystem::CreateDirectory(help.JUNK_WHITESPACE_STR) == false);

        // pre-existing case that should log a warning but return true
        BOOST_CHECK(filesystem::ExistsAndIsDirectory(help.EMPTY_DIR_PATH_STR));
        BOOST_CHECK(filesystem::CreateDirectory(help.EMPTY_DIR_PATH_STR));
        BOOST_CHECK(filesystem::ExistsAndIsDirectory(help.EMPTY_DIR_PATH_STR));
    }

    // this block is only to force ~Help() that will cleanup any mess left by these tests
    {
        Help help;

        const std::string RELATIVE_PATHS_TO_CREATE { help.TEST_DIR_NAME_STR + "/"
                                                     + help.TO_CREATE_SUB_DIR_PATHS_STR };

        BOOST_CHECK(filesystem::Exists(RELATIVE_PATHS_TO_CREATE) == false);
        BOOST_CHECK(filesystem::Exists(help.TO_CREATE_DIR_FINAL_PATH_STR) == false);

        // create with relative path
        BOOST_CHECK(filesystem::CreateDirectory(RELATIVE_PATHS_TO_CREATE));

        BOOST_CHECK(filesystem::ExistsAndIsDirectory(RELATIVE_PATHS_TO_CREATE));
        BOOST_CHECK(filesystem::ExistsAndIsDirectory(help.TO_CREATE_DIR_FINAL_PATH_STR));

        // delete last/deepest dir with relative path
        BOOST_CHECK(filesystem::Delete(RELATIVE_PATHS_TO_CREATE));

        BOOST_CHECK(filesystem::Exists(RELATIVE_PATHS_TO_CREATE) == false);
        BOOST_CHECK(filesystem::Exists(help.TO_CREATE_DIR_FINAL_PATH_STR) == false);

        const std::string RELATIE_PATH_TO_FIRST_DIR { help.TEST_DIR_NAME_STR + "/"
                                                      + "create-directory-test-1" };

        // delete all remaining with relative path
        BOOST_CHECK(filesystem::Delete(RELATIE_PATH_TO_FIRST_DIR));
        BOOST_CHECK(filesystem::Exists(RELATIE_PATH_TO_FIRST_DIR) == false);
        BOOST_CHECK(filesystem::Exists(filesystem::CleanPath(RELATIE_PATH_TO_FIRST_DIR)) == false);
    }

    // this block is only to force ~Help() that will cleanup any mess left by these tests
    {
        Help help;

        // create with absolute path
        BOOST_CHECK(filesystem::Exists(help.TO_CREATE_DIR_FINAL_PATH_STR) == false);
        BOOST_CHECK(filesystem::CreateDirectory(help.TO_CREATE_DIR_FINAL_PATH_STR));
        BOOST_CHECK(filesystem::ExistsAndIsDirectory(help.TO_CREATE_DIR_FINAL_PATH_STR));

        // delete last/deepest dir with absolute path
        BOOST_CHECK(filesystem::ExistsAndIsDirectory(help.TO_CREATE_DIR_FINAL_PATH_STR));
        BOOST_CHECK(filesystem::Delete(help.TO_CREATE_DIR_FINAL_PATH_STR));
        BOOST_CHECK(filesystem::Exists(help.TO_CREATE_DIR_FINAL_PATH_STR) == false);

        // delete all remaining with absolute path

        const std::string ABSOLUTE_PATH_TO_FIRST_DIR { filesystem::CombinePathsThenClean(
            help.TEST_DIR_NAME_STR, "create-directory-test-1") };

        BOOST_CHECK(filesystem::ExistsAndIsDirectory(ABSOLUTE_PATH_TO_FIRST_DIR));
        BOOST_CHECK(filesystem::Delete(ABSOLUTE_PATH_TO_FIRST_DIR));
        BOOST_CHECK(filesystem::Exists(ABSOLUTE_PATH_TO_FIRST_DIR) == false);
    }
}
