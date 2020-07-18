// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_MISC_LOG_HPP_INCLUDED
#define HEROESPATH_MISC_LOG_HPP_INCLUDED
//
// log.hpp
//
#include "log-pri-enum.hpp"
#include "misc/not-null.hpp"

#include <iosfwd>
#include <memory>
#include <string>

namespace heroespath
{
namespace misc
{

    // A simple logging utility
    class Log
    {
    public:
        explicit Log(
            const std::string & FILE_NAME,
            const std::string & FILE_EXT = ".txt", // must include the dot if not empty
            const LogPriority::Enum LOWEST_PRI_TO_CONSOLE_ECHO = LogPriority::Error,
            const std::size_t FILE_APPEND_COUNT_BEFORE_FLUSH = 0,
            const std::size_t CONSOLE_APPEND_COUNT_BEFORE_FLUSH = 0);

        ~Log();

        Log(const Log &) = delete;
        Log(Log &&) = delete;
        Log & operator=(const Log &) = delete;
        Log & operator=(Log &&) = delete;

        static misc::NotNull<Log *> Instance();
        static void Create();
        static void Destroy();

        void Append(
            const std::string & MSG,
            const std::string & FILE_PATH = "",
            const std::string & FUNCTION_NAME = "",
            const int LINE = LINE_NUMBER_INVALID_)
        {
            Append(LogPriority::Default, MSG, FILE_PATH, FUNCTION_NAME, LINE);
        }

        void Append(
            const LogPriority::Enum PRIORITY,
            const std::string & MSG,
            const std::string & FILE_PATH = "",
            const std::string & FUNCTION_NAME = "",
            const int LINE = LINE_NUMBER_INVALID_);

        std::size_t LineCount() const { return lineCount_; }

        static void setUnitTestName(const std::string & UNIT_TEST_NAME)
        {
            unitTestName_ = UNIT_TEST_NAME;
        }

    private:
        void OpenFile();

        const std::string MakeCompleteMessage(
            const LogPriority::Enum PRIORITY,
            const std::string & MSG,
            const std::string & FILE_PATH,
            const std::string & FUNCTION_NAME,
            const int LINE) const;

        void AppendDateString(std::ostream &) const;

        void AppendTimeString(std::ostream &) const;

        void AppendFileFunctionLineString(
            const std::string & FILE_PATH,
            const std::string & FUNCTION_NAME,
            const int LINE,
            std::ostream &) const;

        void EchoToConsole(const LogPriority::Enum PRIORITY, const std::string & COMPLETE_MSG);

        bool IsFileReadyForWriting() const;

    private:
        static const int LINE_NUMBER_INVALID_ = -1; // anything less than zero works here
        static constexpr std::string_view SEPARATOR_STR_ = "  ";
        static std::unique_ptr<Log> instanceUPtr_;
        static inline std::string unitTestName_ {};

        std::string fileName_;
        std::string fileNameExtension_; // must include the dot if not empty
        LogPriority::Enum lowestPriToConsoleEcho_;
        std::unique_ptr<std::ofstream> fileStreamUPtr_;
        std::size_t fileAppendCountBeforeFlush_;
        std::size_t fileAppendCountSinceLastFlush_;
        std::size_t consoleAppendCountBeforeFlush_;
        std::size_t consoleAppendCountSinceLastFlush_;
        std::size_t lineCount_;
    };

} // namespace misc
} // namespace heroespath

#endif // HEROESPATH_MISC_LOG_HPP_INCLUDED
