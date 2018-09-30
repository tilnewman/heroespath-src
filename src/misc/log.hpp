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

#include <fstream>
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
            const LogPriority::Enum LOWEST_PRI_TO_CONSOLE_ECHO = LogPriority::Count,
            const std::size_t FILE_APPEND_COUNT_BEFORE_FLUSH = 0,
            const std::size_t CONSOLE_APPEND_COUNT_BEFORE_FLUSH = 0);

        ~Log();

        Log(const Log &) = delete;
        Log(Log &&) = delete;
        Log & operator=(const Log &) = delete;
        Log & operator=(Log &&) = delete;

        static Log * Instance();
        static void Acquire();
        static void Release();

        void Append(
            const std::string & MSG,
            const std::string & FILE_PATH = "",
            const int LINE = LINE_NUMBER_INVALID_)
        {
            Append(LogPriority::Default, MSG, FILE_PATH, LINE);
        }

        void Append(
            const LogPriority::Enum PRIORITY,
            const std::string & MSG,
            const std::string & FILE_PATH = "",
            const int LINE = LINE_NUMBER_INVALID_);

    private:
        void OpenFile();

        const std::string MakeCompleteMessage(
            const LogPriority::Enum PRIORITY,
            const std::string & MSG,
            const std::string & FILE_PATH,
            const int LINE) const;

        void AppendDateString(std::ostream &) const;

        void AppendTimeString(std::ostream &) const;

        void AppendFileAndLineString(
            const std::string & FILE_PATH, const int LINE, std::ostream &) const;

        void EchoToConsole(const LogPriority::Enum PRIORITY, const std::string & COMPLETE_MSG);

        bool IsFileReadyForWriting() const;

    private:
        static const int LINE_NUMBER_INVALID_;
        static const std::string SEPARATOR_STR_;
        static std::unique_ptr<Log> instanceUPtr_;

        std::string fileName_;
        std::string fileNameExtension_; // must include the dot if not empty
        LogPriority::Enum lowestPriToConsoleEcho_;
        std::ofstream fileStream_;
        std::size_t fileAppendCountBeforeFlush_;
        std::size_t fileAppendCountSinceLastFlush_;
        std::size_t consoleAppendCountBeforeFlush_;
        std::size_t consoleAppendCountSinceLastFlush_;
    };

} // namespace misc
} // namespace heroespath

#endif // HEROESPATH_MISC_LOG_HPP_INCLUDED
