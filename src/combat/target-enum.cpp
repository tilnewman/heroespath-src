// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// target-enum.cpp
//
#include "target-enum.hpp"

#include "misc/random.hpp"

namespace heroespath
{
namespace combat
{

    const std::string TargetType::ActionPhrase(const Enum TARGET_TYPE)
    {
        const std::string PREAMBLE { ((misc::RandomBool()) ? "at " : "upon ") };

        switch (TARGET_TYPE)
        {
            case SingleOpponent: return PREAMBLE + "a single opponent";
            case AllOpponents: return PREAMBLE + "all opponent";
            case SingleCompanion: return PREAMBLE + "a single companion";
            case AllCompanions: return PREAMBLE + "all companion";
            case Item: return PREAMBLE + "an item";
            case QuestSpecific:
            case None:
            case Count:
            default: return "";
        }
    }

} // namespace combat
} // namespace heroespath
