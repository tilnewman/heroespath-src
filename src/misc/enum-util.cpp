// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// enum-util.cpp
//
#include "enum-util.hpp"

#include "game/game-controller.hpp"
#include "misc/log.hpp"

namespace heroespath
{
namespace misc
{
    namespace enum_util
    {
        namespace helpers
        {

            void
                Log(const bool IS_ERROR,
                    const std::string & MESSAGE,
                    const std::string & FILE,
                    const std::string & FUNCTION,
                    const int LINE,
                    const bool WILL_THROW)
            {
                misc::Log::Instance()->Append(
                    ((IS_ERROR) ? misc::LogPriority::Error : misc::LogPriority::Default),
                    MESSAGE,
                    FILE,
                    FUNCTION,
                    LINE);

                if (WILL_THROW)
                {
                    std::ostringstream ss;

                    ss << "M_HP_ENUM_UTIL_LOG_AND_THROW(" << FILE << ":" << FUNCTION << ":" << LINE
                       << ": \"" << MESSAGE << "\"";

                    throw std::runtime_error(ss.str());
                }
            }

            void TestingSystemLog(const std::string & MESSAGE)
            {
                game::GameController::Instance()->TestingStrIncrement(MESSAGE);
            }

        } // namespace helpers
    } // namespace enum_util
} // namespace misc
} // namespace heroespath
