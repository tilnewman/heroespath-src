///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software.  If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef HEROESPATH_CREATURE_SUMMON_INFO_HPP_INCLUDED
#define HEROESPATH_CREATURE_SUMMON_INFO_HPP_INCLUDED
//
// summon-info.hpp
//
#include "creature/race-enum.hpp"

#include <cstddef> //for std::size_t
#include <sstream>
#include <string>

namespace heroespath
{
namespace creature
{

    // Responsible for wrapping all the information about how a magical items can summon creatures.
    class SummonInfo
    {
    public:
        explicit SummonInfo(
            const origin_type::Enum TYPE = origin_type::Count,
            const race::Enum RACE = creature::race::Count,
            const role::Enum ROLE = creature::role::Count)
            : type_(TYPE)
            , race_(RACE)
            , role_(ROLE)
            , count_(origin_type::UseCount(TYPE))
        {}

        origin_type::Enum Type() const { return type_; }
        race::Enum Race() const { return race_; }
        role::Enum Role() const { return role_; }
        std::size_t Count() const { return count_; }

        bool IsValid() const
        {
            return (
                (type_ != origin_type::Count) && (race_ != race::Count) && (role_ != role::Count));
        }

        bool CanSummon() const { return (IsValid() && (count_ > 0)); }

        bool IsDefaultAndCompletelyInvalid() const { return (*this == SummonInfo()); }

        const std::string ToString() const
        {
            if (IsDefaultAndCompletelyInvalid())
            {
                return "SummonInfo{None}";
            }
            else
            {
                std::ostringstream ss;

                ss << "SummonInfo{type="
                   << ((type_ != origin_type::Count) ? origin_type::ToString(type_) : "None")
                   << ", race=" << ((race_ != race::Count) ? race::ToString(race_) : "None")
                   << ", role=" << ((role_ != role::Count) ? role::ToString(role_) : "None")
                   << ", charges_remaining=" << count_ << "}";

                return ss.str();
            }
        }

        friend bool operator==(const SummonInfo & L, const SummonInfo & R);
        friend bool operator<(const SummonInfo & L, const SummonInfo & R);

    private:
        origin_type::Enum type_;
        race::Enum race_;
        role::Enum role_;
        std::size_t count_;

    private:
        friend class boost::serialization::access;
        template <typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & type_;
            ar & race_;
            ar & role_;
            ar & count_;
        }
    };

    inline bool operator==(const SummonInfo & L, const SummonInfo & R)
    {
        return (
            std::tie(L.type_, L.race_, L.role_, L.count_)
            == std::tie(R.type_, R.race_, R.role_, R.count_));
    }

    inline bool operator!=(const SummonInfo & L, const SummonInfo & R) { return !(L == R); }

    inline bool operator<(const SummonInfo & L, const SummonInfo & R)
    {
        return (
            std::tie(L.type_, L.race_, L.role_, L.count_)
            < std::tie(R.type_, R.race_, R.role_, R.count_));
    }

} // namespace creature
} // namespace heroespath

#endif // HEROESPATH_CREATURE_SUMMON_INFO_HPP_INCLUDED
