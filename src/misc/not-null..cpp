// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// not-null.cpp
//
#include "not-null.hpp"

#include <string>

namespace heroespath
{
namespace misc
{
    namespace helpers
    {

        const std::runtime_error makeException(
            const std::string_view TYPE_T_NAME,
            const std::string_view MESSAGE,
            const std::string_view TYPE_U_NAME)
        {
            std::string completeMessage = std::string("NotNull<T=").append(TYPE_T_NAME);

            if (!TYPE_U_NAME.empty())
            {
                completeMessage += std::string(", U=").append(TYPE_U_NAME);
            }

            return std::runtime_error(completeMessage.append(">").append(MESSAGE));
        }

        const std::runtime_error makeException(
            const std::string_view TYPE_T_NAME,
            const std::string_view MESSAGE_1,
            const std::string_view TYPE_U_NAME,
            const std::string_view MESSAGE_2)
        {
            return std::runtime_error(std::string("NotNull<T=")
                                          .append(TYPE_T_NAME)
                                          .append(MESSAGE_1)
                                          .append(TYPE_U_NAME)
                                          .append(MESSAGE_2));
        }

    } // namespace helpers
} // namespace misc
} // namespace heroespath
