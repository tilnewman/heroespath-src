// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// name-position-enum.cpp
//
#include "name-position-enum.hpp"

#include "misc/log-macros.hpp"

#include <sstream>

namespace heroespath
{
namespace combat
{

    const std::string NamePosition::ToString(const Enum ENUM)
    {
        switch (ENUM)
        {
            case NoName: { return "NoName";
            }
            case SourceBefore: { return "SourceBefore";
            }
            case SourceAfter: { return "SourceAfter";
            }
            case SourceThenTarget: { return "SourceThenTarget";
            }
            case TargetBefore: { return "TargetBefore";
            }
            case TargetAfter: { return "TargetAfter";
            }
            case TargetThenSource: { return "TargetThenSource";
            }
            case Count: { return "(Count)";
            }
            default:
            {
                M_HP_LOG_ERR(
                    "enum_value=" << static_cast<EnumUnderlying_t>(ENUM) << " is invalid. (count="
                                  << static_cast<EnumUnderlying_t>(Count) << ")");

                return "";
            }
        }
    }

    ContentAndNamePos::ContentAndNamePos(
        const std::string & PRE_STR,
        const std::string & CONTENT_STR,
        const std::string & POST_STR,
        const NamePosition::Enum NAME_POS_ENUM)
        : pre_(PRE_STR)
        , content_(CONTENT_STR)
        , post_(POST_STR)
        , posEnum_(NAME_POS_ENUM)
    {}

    ContentAndNamePos::ContentAndNamePos(
        const std::string & CONTENT_STR, const NamePosition::Enum NAME_POS_ENUM)
        : pre_("")
        , content_(CONTENT_STR)
        , post_("")
        , posEnum_(NAME_POS_ENUM)
    {}

    const std::string ContentAndNamePos::Compose(
        const std::string & SOURCE_NAME, const std::string & TARGET_NAME) const
    {
        std::string result(pre_);
        result.reserve(post_.size() + ((SOURCE_NAME.size() + TARGET_NAME.size()) * 2));

        if ((NamePosition::SourceThenTarget == posEnum_)
            || (NamePosition::SourceBefore == posEnum_))
        {
            result += SOURCE_NAME;
        }
        else if (
            (NamePosition::TargetThenSource == posEnum_)
            || (NamePosition::TargetBefore == posEnum_))
        {
            result += TARGET_NAME;
        }

        result += content_;

        if ((NamePosition::TargetThenSource == posEnum_) || (NamePosition::SourceAfter == posEnum_))
        {
            result += SOURCE_NAME;
        }
        else if (
            (NamePosition::SourceThenTarget == posEnum_) || (NamePosition::TargetAfter == posEnum_))
        {
            result += TARGET_NAME;
        }

        result += post_;

        return result;
    }

} // namespace combat
} // namespace heroespath
