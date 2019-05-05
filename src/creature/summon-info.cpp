// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// summon-info.cpp
//
#include "summon-info.hpp"

#include "misc/enum-util.hpp"

#include <sstream>

namespace heroespath
{
namespace creature
{

    SummonInfo::SummonInfo(
        const origin_type::Enum TYPE, const race::Enum RACE, const role::Enum ROLE)
        : type_(TYPE)
        , race_(RACE)
        , role_(ROLE)
        , count_(((EnumUtil<origin_type>::IsValid(TYPE)) ? origin_type::UseCount(TYPE) : 0))
    {}

    const std::string SummonInfo::ToString() const
    {
        if (IsDefaultAndCompletelyInvalid())
        {
            return "SummonInfo{None}";
        }
        else
        {
            std::ostringstream ss;

            ss << "SummonInfo{type=" << origin_type::ToString(type_)
               << ", race=" << race::ToString(race_) << ", role=" << role::ToString(role_)
               << ", charges_remaining=" << count_ << "}";

            return ss.str();
        }
    }

} // namespace creature
} // namespace heroespath
