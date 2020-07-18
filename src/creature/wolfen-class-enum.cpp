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

namespace heroespath
{
namespace creature
{

    const std::string WolfenClass::Desc(const WolfenClass::Enum WOLFEN_CLASS_TYPE)
    {
        return misc::ConfigFile::Instance()->Value(
            "creature-race-desc-wolfen-" + NAMEOF_ENUM_STR(WOLFEN_CLASS_TYPE));
    }

    WolfenClass::Enum WolfenClass::ClassFromRank(const Rank_t & RANK)
    {
        if (RANK >= misc::ConfigFile::Instance()->ValueOrDefault<Rank_t>(
                "creature-wolfen-class-rank-min-Elder"))
        {
            return WolfenClass::Elder;
        }
        else if (
            RANK >= misc::ConfigFile::Instance()->ValueOrDefault<Rank_t>(
                "creature-wolfen-class-rank-min-Highborn"))
        {
            return WolfenClass::Highborn;
        }
        else if (
            RANK >= misc::ConfigFile::Instance()->ValueOrDefault<Rank_t>(
                "creature-wolfen-class-rank-min-Noble"))
        {
            return WolfenClass::Noble;
        }
        else if (
            RANK >= misc::ConfigFile::Instance()->ValueOrDefault<Rank_t>(
                "creature-wolfen-class-rank-min-Adult"))
        {
            return WolfenClass::Adult;
        }
        else if (
            RANK >= misc::ConfigFile::Instance()->ValueOrDefault<Rank_t>(
                "creature-wolfen-class-rank-min-Juvenile"))
        {
            return WolfenClass::Juvenile;
        }
        else
        {
            return WolfenClass::Pup;
        }
    }

} // namespace creature
} // namespace heroespath
