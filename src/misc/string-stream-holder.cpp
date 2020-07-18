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

#include <stdexcept>
#include <string>

namespace heroespath
{
namespace misc
{

    //std::ostringstream StringStreamHolder::ss;

    void StringStreamHolder::init()
    {
        //ss.imbue(std::locale::classic());
        //ss << std::boolalpha;
    }

    void StringStreamHolder::reset()
    {
        //ss.clear();
        //ss.str("");
    }

    //const std::string StringStreamHolder::toString() { return ss.str(); }

    //std::ostringstream & StringStreamHolder::get()
    //{
    //    reset();
    //    return ss;
    //}

    // void StringStreamHolder::log(
    //    const misc::LogPriority::Enum PRIORITY,
    //    const std::string FILE,
    //    const std::string FUNCTION,
    //    const int LINE)
    //{
    //    return log(PRIORITY, FILE, FUNCTION, LINE, ss.str());
    //}

    void StringStreamHolder::log(
        const misc::LogPriority::Enum PRIORITY,
        const std::string FILE,
        const std::string FUNCTION,
        const int LINE,
        const std::string MESSAGE)
    {
        Log::Instance()->Append(PRIORITY, MESSAGE, FILE, FUNCTION, LINE);
    }

} // namespace misc
} // namespace heroespath
