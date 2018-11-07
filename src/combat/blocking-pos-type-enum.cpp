// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// combat-over-enum.cpp
//
#include "blocking-pos-type-enum.hpp"

#include "misc/log-macros.hpp"

namespace heroespath
{
namespace combat
{

    const std::string BlockingPosType::ToString(const Enum ENUM)
    {
        switch (ENUM)
        {
            case Front:
            {
                return "Front";
            }
            case Support:
            {
                return "Support";
            }
            case Commanders:
            {
                return "Commanders";
            }
            case Reluctant:
            {
                return "Reluctant";
            }
            case Casters:
            {
                return "Casters";
            }
            case Last:
            {
                return "Last";
            }
            case Count:
            {
                return "(Count)";
            }
            default:
            {
                M_HP_LOG_ERR(ValueOutOfRangeErrorString(ENUM));
                return "";
            }
        }
    }

} // namespace combat
} // namespace heroespath
