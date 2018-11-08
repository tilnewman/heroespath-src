// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// dragon-class-enum.hpp
//
#include "dragon-class-enum.hpp"

#include "misc/config-file.hpp"

namespace heroespath
{
namespace creature
{

    const std::string dragon_class::ToString(const dragon_class::Enum DRAGON_CLASS_TYPE)
    {
        switch (DRAGON_CLASS_TYPE)
        {
            case Hatchling:
            {
                return "Hatchling";
            }
            case Whelp:
            {
                return "Whelp";
            }
            case Fledgling:
            {
                return "Fledgling";
            }
            case Juvenile:
            {
                return "Juvenile";
            }
            case Adult:
            {
                return "Adult";
            }
            case Wyrm:
            {
                return "Wyrm";
            }
            case Skycaster:
            {
                return "Skycaster";
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
                    "enum_value=" << static_cast<EnumUnderlying_t>(DRAGON_CLASS_TYPE)
                                  << " is invalid. (count=" << static_cast<EnumUnderlying_t>(Count)
                                  << ")");

                return "";
            }
        }
    }

    const std::string dragon_class::Desc(const dragon_class::Enum DRAGON_CLASS_TYPE)
    {
        std::ostringstream ss;
        ss << "heroespath-creature-race-desc-dragon-" << ToString(DRAGON_CLASS_TYPE);
        return misc::ConfigFile::Instance()->Value(ss.str());
    }

    dragon_class::Enum dragon_class::ClassFromRank(const Rank_t & RANK)
    {
        if (RANK >= Rank_t(misc::ConfigFile::Instance()->ValueOrDefault<int>(
                "heroespath-creature-dragon-class-rank-min-Elder")))
        {
            return dragon_class::Elder;
        }
        else if (
            RANK >= Rank_t(misc::ConfigFile::Instance()->ValueOrDefault<int>(
                "heroespath-creature-dragon-class-rank-min-Skycaster")))
        {
            return dragon_class::Skycaster;
        }
        else if (
            RANK >= Rank_t(misc::ConfigFile::Instance()->ValueOrDefault<int>(
                "heroespath-creature-dragon-class-rank-min-Wyrm")))
        {
            return dragon_class::Wyrm;
        }
        else if (
            RANK >= Rank_t(misc::ConfigFile::Instance()->ValueOrDefault<int>(
                "heroespath-creature-dragon-class-rank-min-Adult")))
        {
            return dragon_class::Adult;
        }
        else if (
            RANK >= Rank_t(misc::ConfigFile::Instance()->ValueOrDefault<int>(
                "heroespath-creature-dragon-class-rank-min-Juvenile")))
        {
            return dragon_class::Juvenile;
        }
        else if (
            RANK >= Rank_t(misc::ConfigFile::Instance()->ValueOrDefault<int>(
                "heroespath-creature-dragon-class-rank-min-Fledgling")))
        {
            return dragon_class::Fledgling;
        }
        else
        {
            return dragon_class::Whelp;
        }
    }

} // namespace creature
} // namespace heroespath
