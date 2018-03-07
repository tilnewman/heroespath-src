// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software.  If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
//
// logbase.cpp
//
#include "logbase.hpp"
//
#include <algorithm>
#include <cassert>
#include <exception>
#include <iostream>
#include <sstream>
#include <string>

namespace bfs = boost::filesystem;

namespace heroespath
{
namespace log
{

    // static member initializers
    const std::string LogBase::FILE_NAME_DEFAULT{ "log" };
    const std::string LogBase::FILE_NAME_EXT_DEFAULT{ ".txt" };
    const std::string LogBase::FILE_PATH_DEFAULT{ "log_files" };
    const std::size_t LogBase::FILE_NUM_MIN{ 2 };
    const unsigned long LogBase::FILE_SIZE_LIMIT_DEFAULT{ 1 << 30 };

    LogBase::LogBase(
        const std::string & fileName,
        const std::string & fileNameExt,
        const std::string & filePath,
        const std::size_t fileCountMax,
        const unsigned long fileSizeMaxBytes,
        const LogPri::Enum consoleEchoPri)
        : FILE_NAME_((fileName.empty() == true) ? FILE_NAME_DEFAULT : fileName)
        , FILE_NAME_EXT_(fileNameExt)
        , FILE_PATH_(bfs::current_path() / bfs::path(std::string(filePath)))
        , FILE_NUM_MAX_((fileCountMax < FILE_NUM_MIN) ? FILE_NUM_MIN : fileCountMax)
        , FILE_SIZE_LIMIT_BYTES_(fileSizeMaxBytes)
        , CONSOLE_ECHO_PRIORITY_(consoleEchoPri)
        , fileNum_(0)
        , fileStream_()
        , isDestructing_(false)
    {
        LogBase::OpenNextFile();
    }

    LogBase::~LogBase()
    {
        isDestructing_ = true;
        try
        {
            LogBase::OnFileClose(false);
        }
        catch (...)
        {
        }
    }

    void LogBase::Log(const std::string & MSG) { FlushWrapper(MSG); }

    void LogBase::Log(const std::string & FILE, const int LINE, const std::string & MSG)
    {
        FlushWrapper(MSG, LogPri::Default, FILE, LINE);
    }

    void LogBase::Log(const LogPri::Enum PRI, const std::string & MSG) { FlushWrapper(MSG, PRI); }

    void LogBase::Log(
        const LogPri::Enum PRI, const std::string & FILE, const int LINE, const std::string & MSG)
    {
        FlushWrapper(MSG, PRI, FILE, LINE);
    }

    unsigned long LogBase::GetFileSizeLimitBytes() const { return FILE_SIZE_LIMIT_BYTES_; }

    std::size_t LogBase::GetFileNumMax() const { return FILE_NUM_MAX_; }

    std::size_t LogBase::GetFileNum() const { return fileNum_; }

    const std::string LogBase::GetFileName() const
    {
        std::ostringstream ss;
        AppendFileName(ss);
        return ss.str();
    }

    const std::string LogBase::GetFilePath() const { return FILE_PATH_.string(); }

    const std::string LogBase::GetFilePathFull() const
    {
        std::ostringstream fileNameStream;
        AppendFileName(fileNameStream);

        auto const FULL_PATH{ bfs::system_complete(bfs::path(FILE_PATH_ / fileNameStream.str())) };

        return FULL_PATH.string();
    }

    LogPri::Enum LogBase::GetConsoleEchoPriority() const { return CONSOLE_ECHO_PRIORITY_; }

    const std::string LogBase::GetLogPriName(const LogPri::Enum PRI) const
    {
        switch (PRI)
        {
            case LogPri::None:
            {
                return "---";
            }
            case LogPri::Debug:
            {
                return "DBG";
            }
            case LogPri::Default:
            {
                return "DEF";
            }
            case LogPri::Warn:
            {
                return "WRN";
            }
            case LogPri::Error:
            {
                return "ERR";
            }
            case LogPri::Fatal:
            {
                return "FAT";
            }
            case LogPri::Count: // intentional fall-through
            default:
            {
                assert(!"LogBase::Enum value in LogBase::GetLogPriName()");
                return "ERR";
            }
        }
    }

    const std::string LogBase::GetLogPriColor(const LogPri::Enum PRI) const
    {
// prevent the \e below from generating warnings in visual studio
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
#pragma warning(push)
#pragma warning(disable : 4129)
#endif

        switch (PRI)
        {
            case LogPri::None:
            {
                return "";
            }
            case LogPri::Debug:
            {
                return "\033[36;40m";
            } // cyan on black
            case LogPri::Default:
            {
                return "";
            }
            case LogPri::Warn:
            {
                return "\033[33;40m";
            } // yellow on black
            case LogPri::Error:
            {
                return "\033[31;40m";
            } // red on black
            case LogPri::Fatal:
            {
                return "\033[30;41m";
            } // black on red
            // note how the Count enum is used as a special case for terminating color output
            case LogPri::Count: // intentional fall-through
            default:
            {
                return "\033[0;0m";
            } // diable color, return to default
        }

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
#pragma warning(pop)
#endif
    }

    void LogBase::FlushWrapper(
        const std::string & MSG,
        const log::LogPri::Enum & PRI,
        const std::string & FILE,
        const int LINE)
    {
        // This project turned out to be single-threaded so there is no need for this
        // boost::recursive_mutex::scoped_lock lock(fileAccessMutex_);

        // handle file rotation, if it is limited, and we are over the limit
        if (static_cast<unsigned long>(fileStream_.tellp()) >= FILE_SIZE_LIMIT_BYTES_)
        {
            OpenNextFile();
        }

        // temp to allow On..() functions to change the priority if needed
        LogPri::Enum tempPri(PRI);

        OnFlushBeforeFileWrite(MSG, tempPri);
        FlushActual(MSG, tempPri, FILE, LINE);
        OnFlushBeforeConsoleWrite(MSG, tempPri);
        EchoToConsole(MSG, tempPri, FILE, LINE);
    }

    void LogBase::FlushActual(
        const std::string & MSG,
        const log::LogPri::Enum PRI,
        const std::string & FILE,
        const int LINE)
    {
        AppendPriority(fileStream_, PRI);
        fileStream_ << "  ";

        AppendDate(fileStream_);
        fileStream_ << "  ";

        AppendTime(fileStream_);
        fileStream_ << "  ";

        AppendFileLine(fileStream_, FILE, LINE);
        fileStream_ << "  ";

        // NOTE: std::endl forces the stream to flush
        fileStream_ << MSG << std::endl;
    }

    void LogBase::OpenNextFile()
    {
        // close if needed
        if (true == fileStream_.is_open())
        {
            OnFileClose(true);
            fileStream_.close();
        }

        // create directory if needed
        if (false == bfs::exists(FILE_PATH_))
        {
            const bool DIR_CREATE_RESULT(bfs::create_directories(FILE_PATH_));
            if (false == DIR_CREATE_RESULT)
            {
                std::ostringstream es;
                es << "LogBase was unable to create the log directory \"" << GetFilePath() << "\"";
                std::cerr << es.str() << std::endl;

                throw std::runtime_error(es.str());
            }
        }

        // which file to use next
        if (++fileNum_ > FILE_NUM_MAX_)
        {
            fileNum_ = FILE_NUM_MIN;
        }

        // new file name (complete with file number, and extension postfixes)
        std::ostringstream fileNameStream;
        AppendFileName(fileNameStream);

        // boost path object for that file
        auto filePath(bfs::system_complete(bfs::path(FILE_PATH_ / fileNameStream.str())));

        // check for pre-existing files
        while (bfs::exists(filePath))
        {
            ++fileNum_;
            fileNameStream.str("");
            AppendFileName(fileNameStream);
            filePath = bfs::system_complete(bfs::path(FILE_PATH_ / fileNameStream.str()));
        }

        // open the file
        // NOTE:  Since there are always multiple log files used in rotation,
        //       there is no need to append.
        //       Everytime we switch to a new file, the entire file gets erased.
        //       (enforced by the std::ios::out flag)
        fileStream_.open(filePath.string().c_str(), std::ios::out);
        fileStream_.seekp(0, std::ios::beg);

        // verify the file stream is open and ready
        if (false == IsFileReady())
        {
            std::ostringstream es;
            es << "LogBase was unable to open \"" << filePath.string() << "\" for writing.";
            std::cerr << es.str() << std::endl;

            throw std::runtime_error(es.str());
        }

        OnFileOpen();
    }

    void LogBase::EchoToConsole(
        const std::string & MSG,
        const log::LogPri::Enum PRI,
        const std::string & FILE,
        const int LINE)
    {
        if ((PRI >= CONSOLE_ECHO_PRIORITY_) && (PRI != LogPri::None) && (PRI != LogPri::Count))
        {
            // NOTE: std::endl forces the stream to flush
            std::clog << LogBase::GetLogPriColor(PRI);

            AppendFileLine(std::clog, FILE, LINE);
            AppendPriority(std::clog, PRI);

            std::clog << " " << MSG << LogBase::GetLogPriColor(LogPri::Count) << std::endl;
        }
    }

    void LogBase::AppendFileName(std::ostream & stream) const
    {
        stream << FILE_NAME_ << "-" << fileNum_ << FILE_NAME_EXT_;
    }

    void LogBase::AppendPriority(std::ostream & stream, const LogPri::Enum PRI) const
    {
        stream << LogBase::GetLogPriName(PRI);
    }

    void LogBase::AppendDate(std::ostream & stream) const
    {
        auto const DATE_INFO{
            boost::posix_time::microsec_clock::universal_time().date().year_month_day()
        };

        stream << std::setfill('0') << DATE_INFO.year << "-" << std::setw(2)
               << static_cast<int>(DATE_INFO.month) << "-" << std::setw(2) << DATE_INFO.day;
    }

    void LogBase::AppendTime(std::ostream & stream) const
    {
        auto const TIME_OF_DAY_DURATION{
            boost::posix_time::microsec_clock::local_time().time_of_day()
        };

        stream << std::setfill('0') << std::setw(2) << TIME_OF_DAY_DURATION.hours() << ":"
               << std::setw(2) << TIME_OF_DAY_DURATION.minutes() << ":" << std::setw(2)
               << TIME_OF_DAY_DURATION.seconds() << ":" << std::setw(6)
               << TIME_OF_DAY_DURATION.fractional_seconds();
    }

    void LogBase::AppendFileLine(
        std::ostream & stream, const std::string & FILE, const int LINE) const
    {
        // skip if no file/line specified
        if (false == FILE.empty())
        {
            std::string fileNameOnly("");

            if (false == isDestructing_)
                fileNameOnly = bfs::path(FILE).filename().string();

            if (fileNameOnly.empty())
            {
                stream << FILE << ":";
            }
            else
            {
                stream << std::setfill(' ') << std::setw(20) << fileNameOnly << ":";
            }

            stream << std::setfill('0') << std::setw(4) << LINE << " ";
        }
    }

    // NOTE:  When writing to the log in these overrides, always use FlushActual()!
    void LogBase::OnFlushBeforeFileWrite(const std::string &, LogPri::Enum &)
    {
        // no-op until overridden
    }

    // NOTE:  When writing to the log in these overrides, always use FlushActual()!
    void LogBase::OnFlushBeforeConsoleWrite(const std::string &, LogPri::Enum &)
    {
        // no-op until overridden
    }

    // NOTE:  When writing to the log in these overrides, always use FlushActual()!
    void LogBase::OnFileOpen()
    {
        std::ostringstream s1;
        s1 << "Start of logfile:  " << GetFileName();
        s1 << "  (which is " << GetFileNum() << " of " << GetFileNumMax() << ")";
        FlushActual(s1.str(), LogPri::None, __FILE__, __LINE__);

        std::ostringstream s2;
        s2 << "Full path of this log file:  \"" << GetFilePathFull() << "\"";
        FlushActual(s2.str(), LogPri::None, __FILE__, __LINE__);
    }

    // NOTE:  When writing to the log in these overrides, always use FlushActual()!
    void LogBase::OnFileClose(const bool WAS_SIZE_EXCEEDED)
    {
        if (IsFileReady())
        {
            std::ostringstream s1;
            s1 << "End of logfile:  " << GetFileName();
            s1 << "  (which is " << GetFileNum() << " of " << GetFileNumMax() << ")";
            FlushActual(s1.str(), LogPri::None, __FILE__, __LINE__);

            std::ostringstream s2;
            if (WAS_SIZE_EXCEEDED)
            {
                s2 << "This log file is ending after exceeding the size limit of "
                   << GetFileSizeLimitBytes() << "bytes.";
            }
            else
            {
                s2 << "This log file is ending because the application is stopping.";
            }

            FlushActual(s2.str(), LogPri::None, __FILE__, __LINE__);
        }
    }

    bool LogBase::IsFileReady() { return (fileStream_.good()) && (fileStream_.is_open()); }
} // namespace log
} // namespace heroespath
