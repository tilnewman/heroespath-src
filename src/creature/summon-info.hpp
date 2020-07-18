// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_CREATURE_SUMMON_INFO_HPP_INCLUDED
#define HEROESPATH_CREATURE_SUMMON_INFO_HPP_INCLUDED
//
// summon-info.hpp
//
#include "creature/race-enum.hpp"

#include <cstddef>
#include <string>

namespace heroespath
{
namespace creature
{

    // Responsible for wrapping all the information about how a magical items can summon creatures.
    class SummonInfo
    {
    public:
        constexpr explicit SummonInfo(
            const origin_type::Enum TYPE = origin_type::Count,
            const race::Enum RACE = creature::race::Count,
            const role::Enum ROLE = creature::role::Count) noexcept
            : type_(TYPE)
            , race_(RACE)
            , role_(ROLE)
            , count_(origin_type::UseCount(TYPE))
        {}

        constexpr SummonInfo(const SummonInfo &) noexcept = default;
        constexpr SummonInfo(SummonInfo &&) noexcept = default;
        constexpr SummonInfo & operator=(const SummonInfo &) noexcept = default;
        constexpr SummonInfo & operator=(SummonInfo &&) noexcept = default;

        constexpr origin_type::Enum Type() const noexcept { return type_; }
        constexpr race::Enum Race() const noexcept { return race_; }
        constexpr role::Enum Role() const noexcept { return role_; }
        constexpr std::size_t Count() const noexcept { return count_; }

        constexpr bool IsValid() const noexcept
        {
            return ((type_ < origin_type::Count) && (race_ < race::Count) && (count_ > 0));
        }

        const std::string ToString() const
        {
            std::string str;
            str.reserve(64);
            str += "SummonInfo{";

            if (!IsValid())
            {
                str += "INVALID, ";
            }

            str += "type=";
            str += NAMEOF_ENUM(type_);
            str += ", race=";
            str += NAMEOF_ENUM(race_);
            str += ", role=";
            str += NAMEOF_ENUM(role_);
            str += ", charges_remaining=";
            str += std::to_string(count_);
            str += "}";

            return str;
        }

        constexpr Score_t Score() const noexcept
        {
            // Use a creature's rank min/max to establish a kind of combined power/worth/value
            // summon score, then append that to the summoning item's score.

            const auto CREATURE_RANK_RANGE { creature::race::RaceRoleRanks(race_, role_) };

            const double AVG
                = ((CREATURE_RANK_RANGE.Min().GetAs<double>()
                    + CREATURE_RANK_RANGE.Max().GetAs<double>())
                   * 0.5);

            return Score_t::Make((AVG * static_cast<double>(count_)) * 5.0);
        }

        friend constexpr bool operator==(const SummonInfo & L, const SummonInfo & R) noexcept;
        friend constexpr bool operator<(const SummonInfo & L, const SummonInfo & R) noexcept;

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

    constexpr bool operator==(const SummonInfo & L, const SummonInfo & R) noexcept
    {
        return !(
            (L.type_ != R.type_) || (L.race_ != R.race_) || (L.role_ != R.role_)
            || (L.count_ != R.count_));
    }

    constexpr bool operator!=(const SummonInfo & L, const SummonInfo & R) noexcept
    {
        return !(L == R);
    }

    constexpr bool operator<(const SummonInfo & L, const SummonInfo & R) noexcept
    {
        return (
            std::tie(L.type_, L.race_, L.role_, L.count_)
            < std::tie(R.type_, R.race_, R.role_, R.count_));
    }

    constexpr const SummonInfo SummonInfoEmpty;

} // namespace creature
} // namespace heroespath

#endif // HEROESPATH_CREATURE_SUMMON_INFO_HPP_INCLUDED
