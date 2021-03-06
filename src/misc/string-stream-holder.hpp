// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_MISC_SRING_STREAM_HOLDER_HPP_INCLUDED
#define HEROESPATH_MISC_SRING_STREAM_HOLDER_HPP_INCLUDED
//
// string-stream-holder.hpp
//
#include "misc/log-pri-enum.hpp"

#include <sstream>

namespace heroespath
{
namespace misc
{

    struct StringStreamHolder
    {
        static void init();
        static void reset();

        // static const std::string toString();

        // static std::ostringstream & get();

        // static void
        //    log(const misc::LogPriority::Enum PRIORITY,
        //        const std::string FILE,
        //        const std::string FUNCTION,
        //        const int LINE);

        static void
            log(const misc::LogPriority::Enum PRIORITY,
                const std::string FILE,
                const std::string FUNCTION,
                const int LINE,
                const std::string MESSAGE);

    private:
        // static std::ostringstream ss;
    };

} // namespace misc
} // namespace heroespath

#endif // HEROESPATH_MISC_SRING_STREAM_HOLDER_HPP_INCLUDED
