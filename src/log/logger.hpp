// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_LOG_LOGGER_HPP_INCLUDED
#define HEROESPATH_LOG_LOGGER_HPP_INCLUDED
//
// logger.hpp
//
#include "log/logbase.hpp"

#include <memory>
#include <string>

namespace heroespath
{
namespace log
{

    // Subsystem class that logs to a file and std out.
    class Logger : public LogBase
    {
    public:
        Logger(const Logger &) = delete;
        Logger(Logger &&) = delete;
        Logger & operator=(const Logger &) = delete;
        Logger & operator=(Logger &&) = delete;

    public:
        Logger();
        virtual ~Logger();

        static Logger * Instance();
        static void Acquire();
        static void Release();

    private:
        static std::unique_ptr<Logger> instanceUPtr_;
    };

} // namespace log
} // namespace heroespath

#endif // HEROESPATH_LOG_LOGGER_HPP_INCLUDED
