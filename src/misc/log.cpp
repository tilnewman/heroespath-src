// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// log.hpp
//
#include "log.hpp"

#include "gui/date-time.hpp"
#include "misc/enum-util.hpp"
#include "misc/filesystem.hpp"
#include "misc/platform.hpp"

#include <boost/algorithm/string/join.hpp>

#include <algorithm>
#include <exception>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace heroespath
{
namespace misc
{

    const int Log::LINE_NUMBER_INVALID_ { -1 }; // anything less than zero works here
    const char * const Log::SEPARATOR_STR_ { "  " };
    std::unique_ptr<Log> Log::instanceUPtr_ {};

    Log::Log(
        const std::string & FILE_NAME,
        const std::string & FILE_EXT,
        const LogPriority::Enum LOWEST_PRI_TO_CONSOLE_ECHO,
        const std::size_t FILE_APPEND_COUNT_BEFORE_FLUSH,
        const std::size_t CONSOLE_APPEND_COUNT_BEFORE_FLUSH)
        : fileName_(FILE_NAME)
        , fileNameExtension_(FILE_EXT)
        , lowestPriToConsoleEcho_(LOWEST_PRI_TO_CONSOLE_ECHO)
        , fileStreamUPtr_(std::make_unique<std::ofstream>())
        , fileAppendCountBeforeFlush_(FILE_APPEND_COUNT_BEFORE_FLUSH)
        , fileAppendCountSinceLastFlush_(0)
        , consoleAppendCountBeforeFlush_(CONSOLE_APPEND_COUNT_BEFORE_FLUSH)
        , consoleAppendCountSinceLastFlush_(0)
        , lineCount_(0)
    {
        if (FILE_NAME.empty())
        {
            // can't use log or assert macros or Enum::ToString() inside the logger
            std::ostringstream ss;

            ss << __FILE__ << ":" << __func__ << "():" << __LINE__
               << "misc::Log::Log(file_name=" << FILE_NAME
               << ", lowest_pri_to_console_echo=" << int(LOWEST_PRI_TO_CONSOLE_ECHO)
               << ") was given an empty file_name or an invalid lowest_pri_to_console_echo.";

            std::cerr << ss.str() << std::endl;
            throw std::runtime_error(ss.str());
        }

        OpenFile();

        Append(LogPriority::Default, "(log constructed)");
    }

    Log::~Log() { Append(LogPriority::Default, "(log destructed)"); }

    misc::NotNull<Log *> Log::Instance()
    {
        if (!instanceUPtr_)
        {
            Acquire();
        }

        return misc::NotNull<Log *>(instanceUPtr_.get());
    }

    void Log::Acquire()
    {
        if (!instanceUPtr_)
        {
            instanceUPtr_ = std::make_unique<Log>("log", ".txt", LogPriority::Warn);
        }
    }

    void Log::Release()
    {
        if (!instanceUPtr_)
        {
            // can't use log or assert macros or Enum::ToString() inside the logger
            std::ostringstream ss;

            ss << __FILE__ << ":" << __func__ << "():" << __LINE__
               << "misc::Log::Release() found instanceUPtr that was null.";

            std::cerr << ss.str() << std::endl;
            throw std::runtime_error(ss.str());
        }

        instanceUPtr_.reset();
    }

    void Log::Append(
        const LogPriority::Enum PRIORITY,
        const std::string & MSG,
        const std::string & FILE_PATH,
        const std::string & FUNCTION_NAME,
        const int LINE)
    {
        ++lineCount_;

        const auto COMPLETE_MESSAGE { MakeCompleteMessage(
            PRIORITY, MSG, FILE_PATH, FUNCTION_NAME, LINE) };

        if (IsFileReadyForWriting())
        {
            *(fileStreamUPtr_) << COMPLETE_MESSAGE;

            if (++fileAppendCountSinceLastFlush_ > fileAppendCountBeforeFlush_)
            {
                fileAppendCountSinceLastFlush_ = 0;
                *(fileStreamUPtr_) << std::endl;
            }
            else
            {
                *(fileStreamUPtr_) << '\n';
            }

            if (PRIORITY >= lowestPriToConsoleEcho_)
            {
                EchoToConsole(PRIORITY, COMPLETE_MESSAGE);
            }
        }
        else
        {
            std::cerr << "misc::Log::Append() failed because the fileStream_ was closed or "
                         "invalid.  The message that would have been logged follows this line."
                      << std::endl;

            EchoToConsole(PRIORITY, COMPLETE_MESSAGE);
        }
    }

    void Log::OpenFile()
    {
        const auto CURRENT_DIR_PATH_STR { misc::filesystem::CurrentDirectory() };

        const auto FILE_PATH_STR { misc::filesystem::FindFirstAvailableNumberedFilenamePath(
            CURRENT_DIR_PATH_STR, fileName_, fileNameExtension_) };

        const auto DID_EXIST_PRIOR { misc::filesystem::ExistsAndIsFile(FILE_PATH_STR) };

        fileStreamUPtr_->open(FILE_PATH_STR, std::ios::out);

        if (IsFileReadyForWriting() == false)
        {
            const auto ALL_FILE_PATHS_IN_DIR_VEC { misc::filesystem::FindFiles(
                false, CURRENT_DIR_PATH_STR) };

            const auto ALL_FILE_NAMES_IN_DIR_VEC = [&]() {
                std::vector<std::string> fileNames;
                for (const auto & NEXT_PATH_STR : ALL_FILE_PATHS_IN_DIR_VEC)
                {
                    fileNames.emplace_back(misc::filesystem::Filename(NEXT_PATH_STR));
                }
                return fileNames;
            }();

            const auto ALL_FILE_NAMES_IN_DIR_STR {
                "\n" + boost::algorithm::join(ALL_FILE_NAMES_IN_DIR_VEC, "\n")
            };

            if (IsFileReadyForWriting() == false)
            {
                // can't use log or assert macros or Enum::ToString() inside the logger
                std::ostringstream ss;

                ss << __FILE__ << ":" << __func__ << "():" << __LINE__
                   << "misc::Log::OpenFile() failed to open the file.  (file_name=" << fileName_
                   << ", file_ext=" << fileNameExtension_ << ", did_exist_prior=" << std::boolalpha
                   << DID_EXIST_PRIOR
                   << ", "
                      "misc::filesystem::FindFirstAvailableNumberedFilePath()=full_path_attempted="
                   << FILE_PATH_STR << ""
                   << ", current_path=" << CURRENT_DIR_PATH_STR
                   << ", all_files_in_that_current_dir=" << ALL_FILE_NAMES_IN_DIR_STR << ")";

                std::cerr << ss.str() << std::endl;
                throw std::runtime_error(ss.str());
            }
        }
    }

    const std::string Log::MakeCompleteMessage(
        const LogPriority::Enum PRIORITY,
        const std::string & MSG,
        const std::string & FILE_PATH,
        const std::string & FUNCTION_NAME,
        const int LINE) const
    {
        std::ostringstream ss;
        ss << LogPriority::ToStringAcronym(PRIORITY) << SEPARATOR_STR_;
        AppendDateString(ss);
        ss << SEPARATOR_STR_;
        AppendTimeString(ss);
        ss << SEPARATOR_STR_;
        AppendFileFunctionLineString(FILE_PATH, FUNCTION_NAME, LINE, ss);
        ss << SEPARATOR_STR_ << SEPARATOR_STR_ << MSG;
        return ss.str();
    }

    void Log::AppendDateString(std::ostream & ostream) const
    {
        const auto DATE_AND_TIME { gui::DateTime::CurrentDateTime() };

        ostream << std::setfill('0') << DATE_AND_TIME.date.year << "-" << std::setw(2)
                << DATE_AND_TIME.date.month << "-" << std::setw(2) << DATE_AND_TIME.date.day;
    }

    void Log::AppendTimeString(std::ostream & ostream) const
    {
        const auto DATE_AND_TIME { gui::DateTime::CurrentDateTime() };

        ostream << std::setfill('0') << std::setw(2) << DATE_AND_TIME.time.hours << ":"
                << std::setw(2) << DATE_AND_TIME.time.minutes << ":" << std::setw(2)
                << DATE_AND_TIME.time.seconds;

        std::ostringstream ss;
        ss << DATE_AND_TIME.time.milliseconds;
        const auto MILLISECONDS_STR_ORIG { ss.str() };

        ostream << "." << std::setfill('0') << std::setw(2) << MILLISECONDS_STR_ORIG.substr(0, 2);
    }

    void Log::AppendFileFunctionLineString(
        const std::string & FILE_PATH,
        const std::string & FUNCTION_NAME,
        const int LINE,
        std::ostream & ostream) const
    {
        const std::size_t FILE_NAME_LEN_MAX { 40 };
        const std::size_t FUNCTION_NAME_LEN_MAX { 40 };
        const std::size_t LINE_NUMBER_LEN_MAX { 4 };

        const auto FILE_NAME { misc::filesystem::Filename(FILE_PATH) };

        if (FILE_NAME.empty())
        {
            ostream << std::string((FILE_NAME_LEN_MAX - 1), ' ') << "?:";
        }
        else
        {
            ostream << std::setfill(' ') << std::setw(FILE_NAME_LEN_MAX) << FILE_NAME << ":";
        }

        if (FUNCTION_NAME.empty())
        {
            ostream << std::string((FUNCTION_NAME_LEN_MAX - 1), ' ') << "?:";
        }
        else
        {
            ostream << std::setfill(' ') << std::setw(FUNCTION_NAME_LEN_MAX) << FUNCTION_NAME
                    << ":";
        }

        if (LINE < 0)
        {
            ostream << "?" << std::string((LINE_NUMBER_LEN_MAX - 1), ' ');
        }
        else
        {
            ostream << std::setfill('0') << std::setw(LINE_NUMBER_LEN_MAX) << LINE;
        }
    }

    void Log::EchoToConsole(const LogPriority::Enum PRIORITY, const std::string & COMPLETED_MSG)
    {
        if constexpr (!platform_is_any_windows)
        {
            std::clog << LogPriority::ConsoleColorStringBegin(PRIORITY);
        }

        std::clog << COMPLETED_MSG;

        if constexpr (!platform_is_any_windows)
        {
            std::clog << LogPriority::ConsoleColorStringEnd();
        }

        if (++consoleAppendCountSinceLastFlush_ > consoleAppendCountBeforeFlush_)
        {
            consoleAppendCountSinceLastFlush_ = 0;
            std::clog << std::endl;
        }
        else
        {
            std::clog << '\n';
        }
    }

    bool Log::IsFileReadyForWriting() const
    {
        return (fileStreamUPtr_->good()) && (fileStreamUPtr_->is_open());
    }

} // namespace misc
} // namespace heroespath
