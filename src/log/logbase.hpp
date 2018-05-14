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
#ifndef HEROESPATH_LOG_LOGBASE_HPP_INCLUDED
#define HEROESPATH_LOG_LOGBASE_HPP_INCLUDED
//
// logbase.hpp
//
//  TODO
//      finish making auto-compiler-gen member functions private etc.
//      quit passing everything around and start using a simple struct that holds all the details of
//      a log (msg/file/line/pri/time/etc) this could also move the timestamp closer to the actual
//      time  of the event
//
#include "log/ilog.hpp"

#include <boost/filesystem.hpp>

#include <fstream>
#include <sstream>
#include <string>

namespace heroespath
{
namespace log
{
    //
    // LogBase Class
    //  A simple general purpose logger class supporting:
    //      thread safety       Using boost::thread::recursive_mutex, you can safely call any public
    //      functions in any order from any thread. cascaded streams    Using the logging macros
    //      i.e. myLog() << "the value is " << myInt;  //TODO Check on this and add to the testing
    //      app file rotation       Users can speficy the number of files to rotate over (including
    //      just 1), and the max size of each (including 0 for no-limit). console output      Using
    //      std::clog, colors available)
    //
    //  Three basic configurations:
    //      One inifitely large log file    - { LogBase
    //      myNeverEndingLog_WellUntilTheDriveFills_WowThisNameIsTooLong(); } One file, but rotates
    //      at size X - { LogBase mySingleRotatingLog ("myLog",      ".txt", "", 1, X); } N files,
    //      rotating at size X     - { LogBase myRotLog            ("myOtherLog", "",
    //      "../../logfileshere", N, X); }
    //
    //  Logging always rotates across multiple files, at least 2.
    //  When one file is full, the next file is first erased before the first line is written.
    //  So, if fileCountMax is set to 2, then you will always have at least one full file.
    //
    //  End-line characters are appended automatically, so don't bother adding your own.
    //  All writes to both the file and std::clog are flushed immedately, but before releasing the
    //  lock. The file itself (std::ofstream) is closed automatically in the destructor of this
    //  class, so don't worry about it. The constructor/Log()/etc will throw std exceptions if a log
    //  file/directpry cannot be written to, or created, etc.
    //
    //  Notes on Constructor Params:
    //      fileName         -empty strings will be changed into the default filename: "log"
    //      fileNameExt      -empty strings allowed here to disable adding anything to the filename.
    //      Defaults to ".txt" filePath         -any platform specific dir string, existing or not.
    //      (i.e. "logs", "../../logout/thing", etc.) fileCountMax     -the number of files used in
    //      rotation, cannot be lower than 2. fileSizeMaxBytes -will be exceeded by the last append
    //      to that file. consoleEchoPri   -can be set to None to disable the echo to console
    //      feature.
    //
    class LogBase : public ILog
    {
    public:
        LogBase(const LogBase &) = delete;
        LogBase(LogBase &&) = delete;
        LogBase & operator=(const LogBase &) = delete;
        LogBase & operator=(LogBase &&) = delete;

        explicit LogBase(
            const std::string & fileName = FILE_NAME_DEFAULT,
            const std::string & fileNameExt = FILE_NAME_EXT_DEFAULT,
            const std::string & filePath = FILE_PATH_DEFAULT,
            const std::size_t fileCountMax = FILE_NUM_MIN,
            const unsigned long fileSizeMaxBytes = FILE_SIZE_LIMIT_DEFAULT,
            const LogPri::Enum consoleEchoPri = LogPri::Disabled);

        virtual ~LogBase();

        virtual void Log(const std::string & MSG);

        virtual void Log(const std::string & FILE, const int LINE, const std::string & MSG);

        virtual void Log(const LogPri::Enum PRI, const std::string & MSG);

        virtual void
            Log(const LogPri::Enum PRI,
                const std::string & FILE,
                const int LINE,
                const std::string & MSG);

        unsigned long GetFileSizeLimitBytes() const;

        std::size_t GetFileNumMax() const;
        std::size_t GetFileNum() const;
        const std::string GetFileName() const;
        const std::string GetFilePath() const;
        const std::string GetFilePathFull() const;

        LogPri::Enum GetConsoleEchoPriority() const;

    private:
        virtual void FlushWrapper(
            const std::string & MSG,
            const LogPri::Enum & PRI = LogPri::Default,
            const std::string & FILE = "",
            const int LINE = -1);

    protected:
        // NOTE:  All of the protected functions are called while
        //       fileAccessMutex_ is already locked, or during construction/destruction.
        virtual void FlushActual(
            const std::string & MSG,
            const LogPri::Enum PRI,
            const std::string & FILE = "",
            const int LINE = -1);

        void OpenNextFile();

        virtual void EchoToConsole(
            const std::string & MSG,
            const LogPri::Enum PRI,
            const std::string & FILE,
            const int LINE);
        //
        virtual void AppendFileName(std::ostream &) const;

        virtual void AppendPriority(std::ostream &, const LogPri::Enum) const;

        virtual void AppendDate(std::ostream &) const;
        virtual void AppendTime(std::ostream &) const;

        virtual void AppendFileLine(std::ostream &, const std::string &, const int) const;
        //
        virtual const std::string GetLogPriName(const LogPri::Enum) const;
        virtual const std::string GetLogPriColor(const LogPri::Enum) const;
        //
        // NOTE:  If writing to the log in these overrides, always
        //       use FlushActual() instead of FlushWrapper().
        void OnFileOpen();
        virtual void OnFileClose(const bool WAS_SIZE_EXCEEDED = false);
        virtual void OnFlushBeforeFileWrite(const std::string &, LogPri::Enum &);
        virtual void OnFlushBeforeConsoleWrite(const std::string &, LogPri::Enum &);

        bool IsFileReady();

    public:
        static const std::string FILE_NAME_DEFAULT;
        static const std::string FILE_NAME_EXT_DEFAULT;
        static const std::string FILE_PATH_DEFAULT;
        static const std::size_t FILE_NUM_MIN;
        static const unsigned long FILE_SIZE_LIMIT_DEFAULT;

    protected:
        const std::string FILE_NAME_;
        const std::string FILE_NAME_EXT_;
        const boost::filesystem::path FILE_PATH_;
        const std::size_t FILE_NUM_MAX_;
        const unsigned long FILE_SIZE_LIMIT_BYTES_;
        const LogPri::Enum CONSOLE_ECHO_PRIORITY_;
        std::size_t fileNum_; // current file being written to
        std::ofstream fileStream_;
        bool isDestructing_;
    };

    // stream helper function allowing { myLog << "Hello."; or myLog << myInt;}
    template <typename T>
    LogBase & operator<<(LogBase & mLogBase, const T & M_VAL)
    {
        std::ostringstream ss;
        ss << M_VAL;
        mLogBase.Log(ss.str());
        return mLogBase;
    }

} // namespace log
} // namespace heroespath

#endif // HEROESPATH_LOG_LOGBASE_HPP_INCLUDED
