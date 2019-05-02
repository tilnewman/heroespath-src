// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// string-stream-holder.cpp
//
#include "string-stream-holder.hpp"

#include "misc/log.hpp"

#include <sstream>
#include <stdexcept>
#include <string>

namespace heroespath
{
namespace misc
{

    std::unique_ptr<std::ostringstream>
        stringStreamHolder::ss_uptr(std::make_unique<std::ostringstream>());

    bool stringStreamHolder::hasInitAlready(false);

    void stringStreamHolder::init()
    {
        ss_uptr->imbue(std::locale::classic());
        *(ss_uptr) << std::boolalpha;
    }

    void stringStreamHolder::reset()
    {
        ss_uptr->clear();
        ss_uptr->str("");
    }

    const std::string stringStreamHolder::toString() { return ss_uptr->str(); }

    std::ostream & stringStreamHolder::ostreamer()
    {
        if (!hasInitAlready)
        {
            init();
            hasInitAlready = true;
        }

        reset();
        return *(ss_uptr);
    }

    void stringStreamHolder::log(
        const bool WILL_THROW,
        const misc::LogPriority::Enum PRIORITY,
        const std::string & FILE,
        const std::string & FUNCTION,
        const int LINE)
    {
        log(WILL_THROW, PRIORITY, FILE, FUNCTION, LINE, ss_uptr->str());
    }

    void stringStreamHolder::log(
        const bool WILL_THROW,
        const misc::LogPriority::Enum PRIORITY,
        const std::string & FILE,
        const std::string & FUNCTION,
        const int LINE,
        const std::string & MESSAGE)
    {
        Log::Instance()->Append(PRIORITY, MESSAGE, FILE, FUNCTION, LINE);

        if (WILL_THROW)
        {
            throw std::runtime_error(MESSAGE);
        }
    }

} // namespace misc
} // namespace heroespath
