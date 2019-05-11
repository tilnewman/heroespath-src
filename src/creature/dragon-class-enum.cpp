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

    const std::string dragon_class::Desc(const dragon_class::Enum DRAGON_CLASS_TYPE)
    {
        return misc::ConfigFile::Instance()->Value(
            "creature-race-desc-dragon-" + NAMEOF_ENUM_STR(DRAGON_CLASS_TYPE));
    }

    dragon_class::Enum dragon_class::ClassFromRank(const Rank_t & RANK)
    {
        if (RANK >= misc::ConfigFile::Instance()->ValueOrDefault<Rank_t>(
                        "creature-dragon-class-rank-min-Elder"))
        {
            return dragon_class::Elder;
        }
        else if (
            RANK >= misc::ConfigFile::Instance()->ValueOrDefault<Rank_t>(
                        "creature-dragon-class-rank-min-Skycaster"))
        {
            return dragon_class::Skycaster;
        }
        else if (
            RANK >= misc::ConfigFile::Instance()->ValueOrDefault<Rank_t>(
                        "creature-dragon-class-rank-min-Wyrm"))
        {
            return dragon_class::Wyrm;
        }
        else if (
            RANK >= misc::ConfigFile::Instance()->ValueOrDefault<Rank_t>(
                        "creature-dragon-class-rank-min-Adult"))
        {
            return dragon_class::Adult;
        }
        else if (
            RANK >= misc::ConfigFile::Instance()->ValueOrDefault<Rank_t>(
                        "creature-dragon-class-rank-min-Juvenile"))
        {
            return dragon_class::Juvenile;
        }
        else if (
            RANK >= misc::ConfigFile::Instance()->ValueOrDefault<Rank_t>(
                        "creature-dragon-class-rank-min-Fledgling"))
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
