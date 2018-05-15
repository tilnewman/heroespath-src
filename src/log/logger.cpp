// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// logger.cpp
//
#include "logger.hpp"

#include "log/log-macros.hpp"
#include "misc/assertlogandthrow.hpp"

namespace heroespath
{
namespace log
{

    std::unique_ptr<Logger> Logger::instanceUPtr_;

    Logger::Logger()
        : LogBase(
              LogBase::FILE_NAME_DEFAULT,
              LogBase::FILE_NAME_EXT_DEFAULT,
              "",
              LogBase::FILE_NUM_MIN,
              LogBase::FILE_SIZE_LIMIT_DEFAULT,
              LogPri::Warning)
    {}

    Logger::~Logger() = default;

    Logger * Logger::Instance()
    {
        if (!instanceUPtr_)
        {
            Acquire();
        }

        return instanceUPtr_.get();
    }

    void Logger::Acquire()
    {
        if (!instanceUPtr_)
        {
            instanceUPtr_ = std::make_unique<Logger>();
        }
    }

    void Logger::Release()
    {
        M_ASSERT_OR_LOGANDTHROW_SS(
            (instanceUPtr_), "log::Logger::Release() found instanceUPtr that was null.");

        instanceUPtr_.reset();
    }

} // namespace log
} // namespace heroespath
