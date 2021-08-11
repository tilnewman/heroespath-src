// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// wolfen-class-enum.hpp
//
#include "wolfen-class-enum.hpp"

#include "misc/config-file.hpp"
#include "misc/log-macros.hpp"

namespace heroespath
{
namespace creature
{

    const std::string wolfen_class::ToString(const wolfen_class::Enum WOLFEN_CLASS_TYPE)
    {
        switch (WOLFEN_CLASS_TYPE)
        {
            case Pup:
            {
                return "Pup";
            }
            case Juvenile:
            {
                return "Juvenile";
            }
            case Adult:
            {
                return "Adult";
            }
            case Noble:
            {
                return "Noble";
            }
            case Highborn:
            {
                return "Highborn";
            }
            case Elder:
            {
                return "Elder";
            }
            case Count:
            {
                return "(Count)";
            }
            default:
            {
                M_HP_LOG_ERR(
                    "enum_value=" << static_cast<EnumUnderlying_t>(WOLFEN_CLASS_TYPE)
                                  << " is invalid. (count=" << static_cast<EnumUnderlying_t>(Count)
                                  << ")");

                return "";
            }
        }
    }

    const std::string wolfen_class::Desc(const wolfen_class::Enum WOLFEN_CLASS_TYPE)
    {
        return misc::ConfigFile::Instance()->Value(
            "creature-race-desc-wolfen-" + ToString(WOLFEN_CLASS_TYPE));
    }

    wolfen_class::Enum wolfen_class::ClassFromRank(const Rank_t & RANK)
    {
        if (RANK >= Rank_t(misc::ConfigFile::Instance()->ValueOrDefault<int>(
                "creature-wolfen-class-rank-min-Elder")))
        {
            return wolfen_class::Elder;
        }
        else if (
            RANK >= Rank_t(misc::ConfigFile::Instance()->ValueOrDefault<int>(
                "creature-wolfen-class-rank-min-Highborn")))
        {
            return wolfen_class::Highborn;
        }
        else if (
            RANK >= Rank_t(misc::ConfigFile::Instance()->ValueOrDefault<int>(
                "creature-wolfen-class-rank-min-Noble")))
        {
            return wolfen_class::Noble;
        }
        else if (
            RANK >= Rank_t(misc::ConfigFile::Instance()->ValueOrDefault<int>(
                "creature-wolfen-class-rank-min-Adult")))
        {
            return wolfen_class::Adult;
        }
        else if (
            RANK >= Rank_t(misc::ConfigFile::Instance()->ValueOrDefault<int>(
                "creature-wolfen-class-rank-min-Juvenile")))
        {
            return wolfen_class::Juvenile;
        }
        else
        {
            return wolfen_class::Pup;
        }
    }

} // namespace creature
} // namespace heroespath
