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

#include "misc/log-macros.hpp"
#include "misc/random.hpp"

namespace heroespath
{
namespace combat
{

    const std::string TargetType::ToString(const Enum TARGET_TYPE)
    {
        switch (TARGET_TYPE)
        {
            case None:
            {
                return "None";
            }
            case SingleOpponent:
            {
                return "SingleOpponent";
            }
            case AllOpponents:
            {
                return "AllOpponents";
            }
            case SingleCompanion:
            {
                return "SingleCompanion";
            }
            case AllCompanions:
            {
                return "AllCompanions";
            }
            case Item:
            {
                return "Item";
            }
            case QuestSpecific:
            {
                return "QuestSpecific";
            }
            case Count:
            {
                return "(Count)";
            }
            default:
            {
                M_HP_LOG_ERR(
                    "enum_value=" << static_cast<EnumUnderlying_t>(TARGET_TYPE)
                                  << " is invalid. (count=" << static_cast<EnumUnderlying_t>(Count)
                                  << ")");

                return "";
            }
        }
    }

    const std::string TargetType::Name(const Enum TARGET_TYPE)
    {
        switch (TARGET_TYPE)
        {
            case None:
            {
                return "(None)";
            }
            case SingleOpponent:
            {
                return "Single Opponent";
            }
            case AllOpponents:
            {
                return "All Opponents";
            }
            case SingleCompanion:
            {
                return "Single Companion";
            }
            case AllCompanions:
            {
                return "All Companions";
            }
            case Item:
            {
                return "Item";
            }
            case QuestSpecific:
            {
                return "(Quest Specific)";
            }
            case Count:
            {
                return "(Count)";
            }
            default:
            {
                M_HP_LOG_ERR(
                    "enum_value=" << static_cast<EnumUnderlying_t>(TARGET_TYPE)
                                  << " is invalid. (count=" << static_cast<EnumUnderlying_t>(Count)
                                  << ")");

                return "";
            }
        }
    }

    const std::string TargetType::ActionPhrase(const Enum TARGET_TYPE)
    {
        const std::string PREAMBLE { ((misc::random::Bool()) ? "at " : "upon ") };

        switch (TARGET_TYPE)
        {
            case SingleOpponent:
            {
                return PREAMBLE + "a single opponent";
            }
            case AllOpponents:
            {
                return PREAMBLE + "all opponent";
            }
            case SingleCompanion:
            {
                return PREAMBLE + "a single companion";
            }
            case AllCompanions:
            {
                return PREAMBLE + "all companion";
            }
            case Item:
            {
                return PREAMBLE + "an item";
            }
            case QuestSpecific:
            case None:
            case Count:
            default:
            {
                return "";
            }
        }
    }

} // namespace combat
} // namespace heroespath
